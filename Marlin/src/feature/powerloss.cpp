/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/**
 * feature/powerloss.cpp - Resume an SD print after power-loss
 */

#include "../inc/MarlinConfigPre.h"

#if ENABLED(POWER_LOSS_RECOVERY)

#include "powerloss.h"
#include "../core/macros.h"
#ifdef HAS_UDISK
  #include "../lcd/extui/lib/tsc/Menu/PrintUdisk.h"
  #include "../udisk/udiskPrint.h"
#endif

bool PrintJobRecovery::enabled; // Initialized by settings.load()

SdFile PrintJobRecovery::file;
job_recovery_info_t PrintJobRecovery::info;
const char PrintJobRecovery::filename[5] = "/PLR";
const char PrintJobRecovery::filename1[6] = "0:PLR";
uint8_t PrintJobRecovery::queue_index_r;
uint32_t PrintJobRecovery::cmd_sdpos, // = 0
         PrintJobRecovery::sdpos[BUFSIZE];

#if ENABLED(DWIN_CREALITY_LCD)
  bool PrintJobRecovery::dwin_flag; // = false
#endif

#include "../sd/cardreader.h"
#include "../lcd/ultralcd.h"
#include "../gcode/queue.h"
#include "../gcode/gcode.h"
#include "../module/motion.h"
#include "../module/planner.h"
#include "../module/printcounter.h"
#include "../module/temperature.h"
#include "../core/serial.h"

#if ENABLED(FWRETRACT)
  #include "fwretract.h"
#endif

#define DEBUG_OUT ENABLED(DEBUG_POWER_LOSS_RECOVERY)
#include "../core/debug_out.h"




PrintJobRecovery recovery;
#ifdef HAS_UDISK
  typedef enum{
    SDCARD = 0,
    USBDISK
  };
  // uint16_t plr_num = 0;
  uint8_t sd_or_udisk = 0;
  bool plr_flag = false;
  FIL udiskfile;
  DIR udiskFolder;
#endif

#ifdef __cplusplus
  extern "C"{
#endif
    // extern uint8_t udiskMounted;
#ifdef __cplusplus
  }
#endif

#ifndef POWER_LOSS_PURGE_LEN
  #define POWER_LOSS_PURGE_LEN 0
#endif
// #ifndef POWER_LOSS_ZRAISE
//   #define POWER_LOSS_ZRAISE 2     // Move on loss with backup power, or on resume without it
// #endif

#if DISABLED(BACKUP_POWER_SUPPLY)
  #undef POWER_LOSS_RETRACT_LEN   // No retract at outage without backup power
#endif
#ifndef POWER_LOSS_RETRACT_LEN
  #define POWER_LOSS_RETRACT_LEN 0
#endif

/**
 * Clear the recovery info
 */
void PrintJobRecovery::init() { memset(&info, 0, sizeof(info)); }

// init the pin PC6
// extern "C" {
// void EXIT6_Init(void){
//   // 设置好中断线和GPIO映射关系，设置好中断触发模式
//   GPIO_InitTypeDef GPIO_Initure;
//   __HAL_RCC_GPIOC_CLK_ENABLE();     // enabled the pin_c clock

//   GPIO_Initure.Pin = GPIO_PIN_6;    // the 6 pin
//   GPIO_Initure.Mode = GPIO_MODE_IT_FALLING; // External Interrupt Mode with Falling edge trigger detection ,下降沿触发的外部中断
//   GPIO_Initure.Pull = GPIO_PULLUP;  // Pull-up activation, 默认上拉
//   HAL_GPIO_Init(GPIOC, &GPIO_Initure);

//   // 设置中断优先级
//   HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0); // Sets the priority of an interrupt.设置中断的优先级。优先级最高（抢占优先级为1，子优先级为1）
//   HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);         // 使能中断线5~9.
// }
// void EXTI9_5_IRQHandler(){
//   if(print_job_timer.isRunning()) recovery.outage();
//   __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);     // 清除中断6引脚的标志位
// }
// }
#if PIN_EXISTS(POWER_LOSS)
void StartSavePLR(void){
  thermalManager.disable_all_heaters();
  disable_all_steppers();
  // recovery.write();
  if(print_job_timer.isRunning()) recovery.outage();
}
#endif
void PrintJobRecovery::setup(){
  #if PIN_EXISTS(POWER_LOSS)
   #if 0
    #if ENABLED(POWER_LOSS_PULL)
      #if POWER_LOSS_STATE == LOW
        SET_INPUT_PULLUP(POWER_LOSS_PIN);
      #else
        SET_INPUT_PULLDOWN(POWER_LOSS_PIN);
      #endif
    #else
      SET_INPUT(POWER_LOSS_PIN);
    #endif
   #else
    stm32_interrupt_enable(GPIOC, GPIO_PIN_6, StartSavePLR, GPIO_MODE_IT_FALLING);
   #endif
  #endif
}

/**
 * Enable or disable then call changed()
 */
void PrintJobRecovery::enable(const bool onoff) {
  enabled = onoff;
  changed();
}

/**
 * The enabled state was changed:
 *  - Enabled: Purge the job recovery file
 *  - Disabled: Write the job recovery file
 */
void PrintJobRecovery::changed() {
  if (!enabled)
    purge();
  else if (IS_SD_PRINTING() TERN_( HAS_UDISK, || udisk.isUdiskPrint() ))
    save(true);
}

/**
 * Check for Print Job Recovery during setup()
 *
 * If a saved state exists send 'M1000 S' to initiate job recovery.
 */
void PrintJobRecovery::check() {
  //if (!card.isMounted()) card.mount();
  if (card.isMounted()) {
    TERN_( HAS_UDISK, sd_or_udisk = SDCARD;)
    load();
    if (!valid()) return cancel();
    queue.inject_P(PSTR("M1000S"));
  }
}
#ifdef HAS_UDISK
  void PrintJobRecovery::check_u() {
    // 检测到有插U盘且SD卡没有检测到断电文件
    // if(udiskMounted && plr_num<500)
    if(udisk.usbIsReady()/* && plr_num<500*/)
    {
      sd_or_udisk = USBDISK;
      // if() return;
      // plr_num++;
      if(!(load(sd_or_udisk))){
        plr_flag = false;
        // return;
      }
      else{
        plr_flag = true;
        // plr_num = 1000;
        queue.inject_P(PSTR("M1000S"));
      }
    }
  }
#endif
/**
 * Delete the recovery file and clear the recovery data
 */
void PrintJobRecovery::purge() {
  init();
  #ifdef HAS_UDISK
  // if(plr_flag){
    f_unlink(filename1);
    plr_flag = false;
  // }
  #endif
  // else if(!plr_flag)
    card.removeJobRecoveryFile();
}

/**
 * Load the recovery data, if it exists
 */
void PrintJobRecovery::load() {
  if (exists()) {
    open(true);
    (void)file.read(&info, sizeof(info));
    close();
  }
  debug(PSTR("Load"));
}
#ifdef HAS_UDISK
  bool PrintJobRecovery::load(uint8_t ifudisk) {
    debug(PSTR("Load"));
    // f_opendir(&udiskFolder, "/");
    if (f_open(&udiskfile, filename1, FA_READ) == FR_OK) {
      UINT res;
      init();
      f_read(&udiskfile,&info, sizeof(info), &res);
      f_close(&udiskfile);
      return true;
    }
    // f_closedir(&udiskFolder);
    return false;
  }
#endif

/**
 * Set info fields that won't change
 */
void PrintJobRecovery::prepare() {
  card.getAbsFilename(info.sd_filename);  // SD filename
  cmd_sdpos = 0;
}
void PrintJobRecovery::prepare_u(const char *name) {
 #ifdef HAS_UDISK
  memset(info.sd_filename, 0, sizeof(info.sd_filename));
  strcpy(info.sd_filename, name);
 #endif
  cmd_sdpos = 0;
}

/**
 * Save the current machine state to the power-loss recovery file
 */
void PrintJobRecovery::save(const bool force/*=false*/, const float zraise/*=0*/) {

  #if SAVE_INFO_INTERVAL_MS > 0
    static millis_t next_save_ms; // = 0
    millis_t ms = millis();
  #endif

  #ifndef POWER_LOSS_MIN_Z_CHANGE
    #define POWER_LOSS_MIN_Z_CHANGE 0.05  // Vase-mode-friendly out of the box
  #endif

  // Did Z change since the last call?
  if (force
    #if DISABLED(SAVE_EACH_CMD_MODE)      // Always save state when enabled
      #if SAVE_INFO_INTERVAL_MS > 0       // Save if interval is elapsed
        || ELAPSED(ms, next_save_ms)
      #endif
      // Save if Z is above the last-saved position by some minimum height
      || current_position.z > info.current_position.z + POWER_LOSS_MIN_Z_CHANGE
    #endif
  ) {

    #if SAVE_INFO_INTERVAL_MS > 0
      next_save_ms = ms + SAVE_INFO_INTERVAL_MS;
    #endif

    // Set Head and Foot to matching non-zero values
    if (!++info.valid_head) ++info.valid_head; // non-zero in sequence
    //if (!IS_SD_PRINTING()) info.valid_head = 0;
    info.valid_foot = info.valid_head;

    // Machine state
    info.current_position = current_position;
    info.zraise = zraise;
    info.z_current_position = (float)(planner.position.z)/800.0;
    TERN_(HAS_HOME_OFFSET, info.home_offset = home_offset);
    TERN_(HAS_POSITION_SHIFT, info.position_shift = position_shift);
    info.feedrate = uint16_t(feedrate_mm_s * 60.0f);

    #if HAS_MULTI_EXTRUDER
      info.active_extruder = active_extruder;
    #endif

    #if DISABLED(NO_VOLUMETRICS)
      info.volumetric_enabled = parser.volumetric_enabled;
      #if HAS_MULTI_EXTRUDER
        for (int8_t e = 0; e < EXTRUDERS; e++) info.filament_size[e] = planner.filament_size[e];
      #else
        if (parser.volumetric_enabled) info.filament_size[0] = planner.filament_size[active_extruder];
      #endif
    #endif

    #if !PIN_EXISTS(POWER_LOSS)    
      #if EXTRUDERS
        HOTEND_LOOP() info.target_temperature[e] = thermalManager.temp_hotend[e].target;
      #endif
      TERN_(HAS_HEATED_BED, info.target_temperature_bed = thermalManager.temp_bed.target);
    #endif

    #if HAS_FAN
      COPY(info.fan_speed, thermalManager.fan_speed);
    #endif

    #if HAS_LEVELING
      info.leveling = planner.leveling_active;
      info.fade = TERN0(ENABLE_LEVELING_FADE_HEIGHT, planner.z_fade_height);
    #endif

    TERN_(GRADIENT_MIX, memcpy(&info.gradient, &mixer.gradient, sizeof(info.gradient)));

    #if ENABLED(FWRETRACT)
      COPY(info.retract, fwretract.current_retract);
      info.retract_hop = fwretract.current_hop;
    #endif

    // Relative axis modes
    info.axis_relative = gcode.axis_relative;

    // Elapsed print job time
    info.print_job_elapsed = print_job_timer.duration();

    // Misc. Marlin flags
    info.flag.dryrun = !!(marlin_debug_flags & MARLIN_DEBUG_DRYRUN);
    info.flag.allow_cold_extrusion = TERN0(PREVENT_COLD_EXTRUSION, thermalManager.allow_cold_extrude);

    //
    if(IS_SD_PRINTING())
      write();
   #ifdef HAS_UDISK
    else if(udisk.isUdiskPrint()){
      // info.sdpos = udisk.getPrintSize();
      usb_write();
    }
   #endif
  }
}

#if PIN_EXISTS(POWER_LOSS)

  #if ENABLED(BACKUP_POWER_SUPPLY)

    void PrintJobRecovery::retract_and_lift(const float &zraise) {
      #if POWER_LOSS_RETRACT_LEN || POWER_LOSS_ZRAISE

        gcode.set_relative_mode(true);  // Use relative coordinates

        #if POWER_LOSS_RETRACT_LEN
          // Retract filament now
          gcode.process_subcommands_now_P(PSTR("G1 F3000 E-" STRINGIFY(POWER_LOSS_RETRACT_LEN)));
        #endif

        #if POWER_LOSS_ZRAISE
          // Raise the Z axis now
          if (zraise) {
            char cmd[20], str_1[16];
            sprintf_P(cmd, PSTR("G0 Z%s"), dtostrf(zraise, 1, 3, str_1));
            gcode.process_subcommands_now(cmd);
          }
        #else
          UNUSED(zraise);
        #endif

        //gcode.axis_relative = info.axis_relative;
        planner.synchronize();
      #endif
    }

  #endif

  /**
   * An outage was detected by a sensor pin.
   *  - If not SD printing, let the machine turn off on its own with no "KILL" screen
   *  - Disable all heaters first to save energy
   *  - Save the recovery data for the current instant
   *  - If backup power is available Retract E and Raise Z
   *  - Go to the KILL screen
   */
  void PrintJobRecovery::_outage() {
    #if ENABLED(BACKUP_POWER_SUPPLY)
      static bool lock = false;
      if (lock) return; // No re-entrance from idle() during retract_and_lift()
      lock = true;
    #endif

    #if POWER_LOSS_ZRAISE
      // Get the limited Z-raise to do now or on resume
      const float zraise = _MAX(0, _MIN(current_position.z + POWER_LOSS_ZRAISE, Z_MAX_POS - 1) - current_position.z);
    #else
      constexpr float zraise = 0;
    #endif
    // 缓存温度数据
    #if EXTRUDERS
      HOTEND_LOOP() info.target_temperature[e] = thermalManager.degTargetHotend(e);
    #endif
    TERN_(HAS_HEATED_BED, info.target_temperature_bed = thermalManager.degTargetBed());
    
    thermalManager.disable_all_heaters();
    disable_all_steppers();

    // Save, including the limited Z raise
    if (IS_SD_PRINTING() TERN_(HAS_UDISK, ||udisk.isUdiskPrint())) save(true, zraise);

    // Disable all heaters to reduce power loss
    thermalManager.disable_all_heaters();

    #if ENABLED(BACKUP_POWER_SUPPLY)
      // Do a hard-stop of the steppers (with possibly a loud thud)
      quickstop_stepper();
      // With backup power a retract and raise can be done now
      retract_and_lift(zraise);
    #endif

    kill(GET_TEXT(MSG_OUTAGE_RECOVERY));
  }

#endif
// static uint8_t test_file[] = "1234567890abcdefghijklmnopqrstuvwxyz";
/**
 * Save the recovery info the recovery file
 */
void PrintJobRecovery::write() {
 #if 1
  debug(PSTR("Write"));

  open(false);
  file.seekSet(0);
  const int16_t ret = file.write(&info, sizeof(info));
  if (ret == -1) DEBUG_ECHOLNPGM("Power-loss file write failed.");
  if (!file.close()) DEBUG_ECHOLNPGM("Power-loss file close failed.");
 #else
  open(false);
  file.seekSet(0);
  file.write(&test_file, sizeof(test_file));
  file.write(&test_file, sizeof(test_file));
  file.write(&test_file, sizeof(test_file));
  file.close();
 #endif
}
#ifdef HAS_UDISK
  void PrintJobRecovery::usb_write() {
    if(!udisk.usbIsReady()) return;
    debug(PSTR("usb Write"));
    FRESULT fp = f_open(&udiskfile, filename1, FA_WRITE | FA_OPEN_ALWAYS);
    fp = f_lseek(&udiskfile,0);
    UINT ret = 0;
    fp = f_write(&udiskfile,&info, sizeof(info), &ret);
    if (ret <= 0) DEBUG_ECHOLNPGM("Power-loss file write failed.");
    if (f_close(&udiskfile)) DEBUG_ECHOLNPGM("Power-loss file close failed.");
  }
#endif

/**
 * Resume the saved print job
 */
void PrintJobRecovery::resume() {

  char cmd[MAX_CMD_SIZE+16], str_1[16], str_2[16];

  const uint32_t resume_sdpos = info.sdpos; // Get here before the stepper ISR overwrites it

  // Apply the dry-run flag if enabled
  if (info.flag.dryrun) marlin_debug_flags |= MARLIN_DEBUG_DRYRUN;

  // Restore cold extrusion permission
  TERN_(PREVENT_COLD_EXTRUSION, thermalManager.allow_cold_extrude = info.flag.allow_cold_extrusion);

  #if HAS_LEVELING
    // Make sure leveling is off before any G92 and G28
    gcode.process_subcommands_now_P(PSTR("M420 S0 Z0"));
  #endif

  // 先加热，防止模具脱落
  #if HAS_HEATED_BED
    const int16_t bt = info.target_temperature_bed;
    if (bt) {
      // Restore the bed temperature
      sprintf_P(cmd, PSTR("M190 S%i"), bt);
      gcode.process_subcommands_now(cmd);
    }
  #endif

  // Restore all hotend temperatures
  #if HAS_HOTEND
    HOTEND_LOOP() {
      const int16_t et = info.target_temperature[e];
      if (et) {
        #if HAS_MULTI_HOTEND
          sprintf_P(cmd, PSTR("T%i S"), e);
          gcode.process_subcommands_now(cmd);
        #endif
        sprintf_P(cmd, PSTR("M109 S%i"), et);
        gcode.process_subcommands_now(cmd);
      }
    }
  #endif
  
  gcode.process_subcommands_now("G92.9 E0"); // Reset E to 0
  
  // Interpret the saved Z according to flags
  float z_now = info.current_position.z + info.zraise;

  // Reset E, raise Z, home XY...
  #if Z_HOME_DIR > 0

    // If Z homing goes to max, just reset E and home all
    gcode.process_subcommands_now_P(PSTR(
      "G28R0"
    ));

  #else // "G92.9 E0 ..."

    // Set Z to the real position
    sprintf_P(cmd, PSTR("G92.9 Z%s\n"), dtostrf(z_now, 1, 3, str_1));
    gcode.process_subcommands_now(cmd);

    // raise the z axie
    gcode.process_subcommands_now("G91\n");
    gcode.process_subcommands_now("G0 Z5\n");
    gcode.process_subcommands_now("G90\n");

    // 复位XY轴,R0是Z轴不抬升
    gcode.process_subcommands_now_P(PSTR(
      "G28 R0"                               // No raise during G28
      #if IS_CARTESIAN && DISABLED(POWER_LOSS_RECOVER_ZHOME)
        "XY"                                // Don't home Z on Cartesian unless overridden
      #endif
    ));

  #endif

  // Pretend that all axes are homed
  set_all_homed();

  #if ENABLED(POWER_LOSS_RECOVER_ZHOME)
    // Z has been homed so restore Z to ZsavedPos + POWER_LOSS_ZRAISE
    sprintf_P(cmd, PSTR("G1 F500 Z%s"), dtostrf(info.current_position.z + POWER_LOSS_ZRAISE, 1, 3, str_1));
    gcode.process_subcommands_now(cmd);
  #endif

  // Recover volumetric extrusion state
  #if DISABLED(NO_VOLUMETRICS)
    #if HAS_MULTI_EXTRUDER
      for (int8_t e = 0; e < EXTRUDERS; e++) {
        sprintf_P(cmd, PSTR("M200 T%i D%s"), e, dtostrf(info.filament_size[e], 1, 3, str_1));
        gcode.process_subcommands_now(cmd);
      }
      if (!info.volumetric_enabled) {
        sprintf_P(cmd, PSTR("M200 T%i D0"), info.active_extruder);
        gcode.process_subcommands_now(cmd);
      }
    #else
      if (info.volumetric_enabled) {
        sprintf_P(cmd, PSTR("M200 D%s"), dtostrf(info.filament_size[0], 1, 3, str_1));
        gcode.process_subcommands_now(cmd);
      }
    #endif
  #endif


  // Select the previously active tool (with no_move)
  #if HAS_MULTI_EXTRUDER
    sprintf_P(cmd, PSTR("T%i S"), info.active_extruder);
    gcode.process_subcommands_now(cmd);
  #endif

  // Restore print cooling fan speeds
  FANS_LOOP(i) {
    uint8_t f = info.fan_speed[i];
    if (f) {
      sprintf_P(cmd, PSTR("M106 P%i S%i"), i, f);
      gcode.process_subcommands_now(cmd);
    }
  }

  // Restore retract and hop state
  #if ENABLED(FWRETRACT)
    LOOP_L_N(e, EXTRUDERS) {
      if (info.retract[e] != 0.0) {
        fwretract.current_retract[e] = info.retract[e];
        fwretract.retracted[e] = true;
      }
    }
    fwretract.current_hop = info.retract_hop;
  #endif

  // #if HAS_LEVELING
  //   // Restore leveling state before 'G92 Z' to ensure
  //   // the Z stepper count corresponds to the native Z.
  //   if (info.fade || info.leveling) {
  //     sprintf_P(cmd, PSTR("M420 S%i Z%s"), int(info.leveling), dtostrf(info.fade, 1, 1, str_1));
  //     gcode.process_subcommands_now(cmd);
  //   }
  // #endif

  #if ENABLED(GRADIENT_MIX)
    memcpy(&mixer.gradient, &info.gradient, sizeof(info.gradient));
  #endif

  // Un-retract if there was a retract at outage
  #if POWER_LOSS_RETRACT_LEN
    gcode.process_subcommands_now_P(PSTR("G1 E" STRINGIFY(POWER_LOSS_RETRACT_LEN) " F3000"));
  #endif

  // Additional purge if configured
  #if POWER_LOSS_PURGE_LEN
    sprintf_P(cmd, PSTR("G1 E%d F200"), (POWER_LOSS_PURGE_LEN) + (POWER_LOSS_RETRACT_LEN));
    gcode.process_subcommands_now(cmd);
  #endif

  #if ENABLED(NOZZLE_CLEAN_FEATURE)
    gcode.process_subcommands_now_P(PSTR("G12"));
  #endif

 #if 0
  // Move back to the saved Z
  dtostrf(info.z_current_position, 1, 3, str_1);
  // dtostrf(info.current_position.z, 1, 3, str_1);
  #if Z_HOME_DIR > 0 || ENABLED(POWER_LOSS_RECOVER_ZHOME)
    sprintf_P(cmd, PSTR("G1 Z%s F200"), str_1);
  #else
    // gcode.process_subcommands_now_P(PSTR("G1 Z0 F200")); // 导致Z轴向下移的罪魁祸首
    sprintf_P(cmd, PSTR("G92.9 Z%s"), str_1);
  #endif
  gcode.process_subcommands_now(cmd);

  // dtostrf(info.current_position.z, 1, 3, str_1); // 之前为了解决轴向下移的方法，向上移。
  // sprintf_P(cmd, PSTR("G1 Z%s"), str_1);
  // gcode.process_subcommands_now(cmd);

  // Move back to the saved XY
  sprintf_P(cmd, PSTR("G1 X%s Y%s F3000"),
    dtostrf(info.current_position.x, 1, 3, str_1),
    dtostrf(info.current_position.y, 1, 3, str_2)
  );
  gcode.process_subcommands_now(cmd);
 #endif

  // Restore the XY
  sprintf_P(cmd, PSTR("G1 X%s Y%s F3000"),
    dtostrf(info.current_position.x, 1, 3, str_1),
    dtostrf(info.current_position.y, 1, 3, str_2)
  );
  gcode.process_subcommands_now(cmd);
  
  #if HAS_LEVELING
    // Restore leveling state before 'G92 Z' to ensure the Z stepper count corresponds to the native Z.
    if (info.fade || info.leveling) {
      sprintf_P(cmd, PSTR("M420 S%i Z%s"), int(info.leveling), dtostrf(info.fade, 1, 1, str_1));
      gcode.process_subcommands_now(cmd);
    }
    z_now += 5.0f;  // 上面抬升了5mm
    // The physical Z was adjusted at power-off so undo the M420S1 correction to Z with G92.9.
    sprintf_P(cmd, PSTR("G92.9 Z%s"), dtostrf(z_now, 1, 1, str_1));
    gcode.process_subcommands_now(cmd);
  #endif

  // Move back down to the saved Z for printing
  sprintf_P(cmd, PSTR("G1 Z%s F600"), dtostrf(info.current_position.z, 1, 3, str_1));
  gcode.process_subcommands_now(cmd);

  // Restore the feedrate
  sprintf_P(cmd, PSTR("G1 F%d"), info.feedrate);
  gcode.process_subcommands_now(cmd);

  // Restore E position with G92.9
  sprintf_P(cmd, PSTR("G92.9 E%s"), dtostrf(info.current_position.e, 1, 3, str_1));
  gcode.process_subcommands_now(cmd);

  // Relative axis modes
  gcode.axis_relative = info.axis_relative;

  TERN_(HAS_HOME_OFFSET, home_offset = info.home_offset);
  TERN_(HAS_POSITION_SHIFT, position_shift = info.position_shift);
  #if HAS_HOME_OFFSET || HAS_POSITION_SHIFT
    LOOP_XYZ(i) update_workspace_offset((AxisEnum)i);
  #endif

  #if ENABLED(DEBUG_POWER_LOSS_RECOVERY)
    const uint8_t old_flags = marlin_debug_flags;
    marlin_debug_flags |= MARLIN_DEBUG_ECHO;
  #endif

  // // Continue to apply PLR when a file is resumed!
  // enable(true);

  // Resume the SD file from the last position
  char *fn = info.sd_filename;
  TERN_( HAS_UDISK, if(!plr_flag){ )
    if(*fn == '/') fn++;
    extern const char M23_STR[];
    sprintf_P(cmd, M23_STR, fn);
    gcode.process_subcommands_now(cmd);
    sprintf_P(cmd, PSTR("M24 S%ld T%ld"), resume_sdpos, info.print_job_elapsed);
    gcode.process_subcommands_now(cmd);

    TERN_(DEBUG_POWER_LOSS_RECOVERY, marlin_debug_flags = old_flags);
 #ifdef HAS_UDISK
  }
  else{
    f_stat(info.sd_filename, &workFileinfo);
    memset(filePath, 0, sizeof(filePath));
    strcpy(filePath, info.sd_filename);
    udisk.resumeUdiskPrint(filePath, resume_sdpos, info.print_job_elapsed);
    gcode.process_subcommands_now("M24\n");
  }
 #endif
 
  // Continue to apply PLR when a file is resumed!
  enable(true);
}

#if ENABLED(DEBUG_POWER_LOSS_RECOVERY)

  void PrintJobRecovery::debug(PGM_P const prefix) {
    DEBUG_PRINT_P(prefix);
    DEBUG_ECHOLNPAIR(" Job Recovery Info...\nvalid_head:", int(info.valid_head), " valid_foot:", int(info.valid_foot));
    if (info.valid_head) {
      if (info.valid_head == info.valid_foot) {
        DEBUG_ECHOPGM("current_position: ");
        LOOP_XYZE(i) {
          if (i) DEBUG_CHAR(',');
          DEBUG_DECIMAL(info.current_position[i]);
        }
        DEBUG_EOL();

        DEBUG_ECHOLNPAIR("zraise: ", info.zraise);
        
        DEBUG_ECHOLNPAIR("z_current_position: ", info.z_current_position);

        #if HAS_HOME_OFFSET
          DEBUG_ECHOPGM("home_offset: ");
          LOOP_XYZ(i) {
            if (i) DEBUG_CHAR(',');
            DEBUG_DECIMAL(info.home_offset[i]);
          }
          DEBUG_EOL();
        #endif

        #if HAS_POSITION_SHIFT
          DEBUG_ECHOPGM("position_shift: ");
          LOOP_XYZ(i) {
            if (i) DEBUG_CHAR(',');
            DEBUG_DECIMAL(info.position_shift[i]);
          }
          DEBUG_EOL();
        #endif

        DEBUG_ECHOLNPAIR("feedrate: ", info.feedrate);

        #if HAS_MULTI_EXTRUDER
          DEBUG_ECHOLNPAIR("active_extruder: ", int(info.active_extruder));
        #endif

        #if HAS_HOTEND
          DEBUG_ECHOPGM("target_temperature: ");
          HOTEND_LOOP() {
            DEBUG_ECHO(info.target_temperature[e]);
            if (e < HOTENDS - 1) DEBUG_CHAR(',');
          }
          DEBUG_EOL();
        #endif

        #if HAS_HEATED_BED
          DEBUG_ECHOLNPAIR("target_temperature_bed: ", info.target_temperature_bed);
        #endif

        #if HAS_FAN
          DEBUG_ECHOPGM("fan_speed: ");
          FANS_LOOP(i) {
            DEBUG_ECHO(int(info.fan_speed[i]));
            if (i < FAN_COUNT - 1) DEBUG_CHAR(',');
          }
          DEBUG_EOL();
        #endif

        #if HAS_LEVELING
          DEBUG_ECHOLNPAIR("leveling: ", int(info.leveling), " fade: ", info.fade);
        #endif
        #if ENABLED(FWRETRACT)
          DEBUG_ECHOPGM("retract: ");
          for (int8_t e = 0; e < EXTRUDERS; e++) {
            DEBUG_ECHO(info.retract[e]);
            if (e < EXTRUDERS - 1) DEBUG_CHAR(',');
          }
          DEBUG_EOL();
          DEBUG_ECHOLNPAIR("retract_hop: ", info.retract_hop);
        #endif
        DEBUG_ECHOLNPAIR("sd_filename: ", info.sd_filename);
        DEBUG_ECHOLNPAIR("sdpos: ", info.sdpos);
        DEBUG_ECHOLNPAIR("print_job_elapsed: ", info.print_job_elapsed);
        DEBUG_ECHOLNPAIR("dryrun: ", int(info.flag.dryrun));
        DEBUG_ECHOLNPAIR("allow_cold_extrusion: ", int(info.flag.allow_cold_extrusion));
      }
      else
        DEBUG_ECHOLNPGM("INVALID DATA");
    }
    DEBUG_ECHOLNPGM("---");
  }

#endif // DEBUG_POWER_LOSS_RECOVERY

#endif // POWER_LOSS_RECOVERY
