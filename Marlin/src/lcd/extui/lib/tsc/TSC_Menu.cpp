/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* DGUS implementation written by coldtobi in 2019 for Marlin */

#include "../../../../inc/MarlinConfigPre.h"

#if ENABLED(MD_FSMC_LCD)

#include "../../ui_api.h"

#include "../../../../MarlinCore.h"
#include "../../../../module/temperature.h"
#include "../../../../module/motion.h"
#include "../../../../gcode/queue.h"
#include "../../../../module/planner.h"
#include "../../../../sd/cardreader.h"
#include "../../../../libs/duration_t.h"
#include "../../../../libs/buzzer.h"
#include "../../../../module/printcounter.h"
#include "TSC_Menu.h"


MENU  infoMenu = {.menu = {menuStatus}, .cur = 0};  // Menu structure
FP_MENU menuFrontCallBack = 0; 

TSCBoot boot;


#define Buzzer_TurnOn(fre, ms) BUZZ(ms, fre)
void Buzzer_play(SOUND sound){
  if(infoSettings.silent) return;
  switch (sound)
  {
    case sound_ok:
      Buzzer_TurnOn(3800,40);
      Buzzer_TurnOn(0,20);
      Buzzer_TurnOn(5500,50);
      break;
    case sound_success:
      Buzzer_TurnOn(3500,50);
      Buzzer_TurnOn(0,50);
      Buzzer_TurnOn(3500,50);
      Buzzer_TurnOn(0,50);
      Buzzer_TurnOn(3500,50);
      break;
    case sound_cancel:
      Buzzer_TurnOn(5500,50);
      Buzzer_TurnOn(0,20);
      Buzzer_TurnOn(3800,40);
      break;
      case sound_notify:
      Buzzer_TurnOn(3090,50);
      Buzzer_TurnOn(0,50);
      Buzzer_TurnOn(4190,50);
      break;
    case sound_error:
      Buzzer_TurnOn(2200,200);
      Buzzer_TurnOn(0,60);
      Buzzer_TurnOn(2200,200);
      Buzzer_TurnOn(0,60);
      Buzzer_TurnOn(2200,200);
      break;
    case sound_keypress:
    default:
      Buzzer_TurnOn(LCD_FEEDBACK_FREQUENCY_HZ, LCD_FEEDBACK_FREQUENCY_DURATION_MS);
      break;
  }
}

// 存储gcode cmd到gcode队列
// 如果gcode队列已满，在标题栏中提醒。
bool storeCmd(const char *cmd)
{
  if (queue.length >= BUFSIZE) {
    reminderMessage(LABEL_BUSY, STATUS_BUSY);
    return false;
  }
  queue.enqueue_one_now(cmd);
  return true;
}

// Store gcode cmd to gcode queue
// If the gcode queue is full, reminde in title bar,  waiting for available queue and store the command.
void mustStoreCmd(const char *cmd)
{
  if (queue.length >= BUFSIZE) reminderMessage(LABEL_BUSY, STATUS_BUSY);
  queue.enqueue_one_now(cmd);
}

extern SETTINGS lastSettings; // Last Settings para
extern uint32_t TSC_Para[7],
                lastTSC_Para[7];
void LCD_Setup() {
  XPT2046_Init();
  W25Qxx_Init();
  readStoredPara();
  LCD_Init();
  LOGO_ReadDisplay();
  boot.scanUpdates();
  // while(1);
  if(readStoredPara() == false || boot.scanResetFile() || infoSettings.tsccalibnextboot == 1) // Read settings parameter
  {
    TSC_Calibration();
    infoSettings.tsccalibnextboot = 0;
    storePara();
    LOGO_ReadDisplay();
  }
 
  memcpy(&lastSettings, &infoSettings, sizeof(SETTINGS));
  memcpy(lastTSC_Para, TSC_Para, sizeof(TSC_Para));
  ExtUI::setFilamentRunoutEnabled(infoSettings.runout);
}

uint8_t *get_str(const int a, uint8_t *str){
  if(a<10){
    char b[4];
    itoa(a,b,10);
    sprintf_P((char*)str, "0%s", b);
  }else itoa(a, (char*)str, 10);

  return str;
}
uint8_t *get_time_str(const uint32_t hour,const uint8_t min,const uint8_t sec, uint8_t *str){
  uint8_t hours[4], mins[4], secs[4];
  get_str(hour, hours);
  get_str(min, mins);
  get_str(sec, secs);
  sprintf_P((char*)str, "%s:%s:%s", hours, mins, secs);
  return str;
}

static bool hasPrintingMenu = false;
void menuUpdate(void) {
  static FP_MENU lastMenu = 0;
  if (lastMenu != infoMenu.menu[infoMenu.cur]) { // Menu switch
    lastMenu = infoMenu.menu[infoMenu.cur];
    lastMenu();
  }

  if (menuFrontCallBack) { // Call back is vaild
    menuFrontCallBack();
  }

  loopFrontEnd();

  if (isPrinting()) {
    if (!hasPrintingMenu) {
      hasPrintingMenu = true;
      infoMenu.menu[++infoMenu.cur] = menuPrinting;
    }
  } else {
    if (hasPrintingMenu) {
      hasPrintingMenu = false;
      while (infoMenu.menu[infoMenu.cur] != menuPrinting && infoMenu.cur) {
        infoMenu.cur--;
      }
      
      if (infoMenu.cur) infoMenu.cur--;
      if(auto_close_flag == 2)  auto_close_flag = 1;
      // Abort print not print completed, need not popup
      if (!card.flag.abort_sd_printing) {
        if(infoSettings.auto_off) {
          // Auto shut down after printing
          infoMenu.menu[++infoMenu.cur] = menuShutDown;
        } else {
          // popup print completed
         #if ENABLED(USART_LCD)
          // infoMenu.cur--;
         #else
          uint8_t finish_show[64] = {0};
          Buzzer_play(sound_success);
          uint32_t printedTime = print_job_timer.duration();
          uint32_t  hour = printedTime/3600%1000;   // 让小时数最大可以计数到999
          uint8_t   min = printedTime%3600/60,
                    sec = printedTime%60;
          uint8_t time_str[16] = {0};
          get_time_str(hour, min, sec, time_str);
          sprintf_P((char*)finish_show, "%s\n%s: %s", textSelect(LABEL_PRINTING_COMPLETED),textSelect(LABEL_ELAPSED_TIME), time_str);
          
          popupReminder(textSelect(LABEL_PRINT), finish_show);
         #endif
        }
      }
    }
  }
}

void menuSetFrontCallBack(FP_MENU cb) {
  menuFrontCallBack = cb;
}
#endif