#include "../../inc/MarlinConfig.h"

#if ENABLED(USART_LCD)

#include "../gcode.h"
#include "../../lcd_show_addr.h"
#include "../../lcd/mingda_lcd/print_list.h"
#include "../../lcd/extui/ui_api.h"
#include "../../lcd/extui/lib/tsc/Menu/Speed.h"
#include "../../lcd/extui/lib/tsc/Menu/Printing.h"
#include "../../lcd/extui/lib/tsc/Menu/Babystep.h"
#if ENABLED(LEVELING_OFFSET)
#include "../../lcd/extui/lib/tsc/Menu/LevelingOffset.h"
#endif

char Send_Success[] = "Save successfully";
char Send_Failed[] = "Fail to save ";

// 更新E轴挤出或者zoffset
void GcodeSuite::M1107(){
  uint16_t n=0,m=0;
  feedRate_t the_feedrate = 0;
  float s,e_v;
 #if ENABLED(LEVELING_OFFSET)
  if (parser.seenval('S')){
    if(saveOffset()){
      char dwin_send_W[32] = {0};
      addHeadMess(dwin_send_W);
      dwin_send_W[4] = INFO_WARNING >> 8; dwin_send_W[5] = INFO_WARNING & 0xff;
      sprintf_P(&dwin_send_W[6], "%s", Send_Success);
      dwin_send_W[2] = 32 - 3;
      send_hexPGM(dwin_send_W, dwin_send_W[2] + 3);
    }
    else{
      char dwin_send_F[32] = {0};
      addHeadMess(dwin_send_F);
      dwin_send_F[4] = INFO_WARNING >> 8; dwin_send_F[5] = INFO_WARNING & 0xff;
      sprintf_P(&dwin_send_F[6], "%s", Send_Failed);
      dwin_send_F[2] = 32 - 3;
      send_hexPGM(dwin_send_F, dwin_send_F[2] + 3);
    }
  }
  else if (parser.seenval('Z')){
    s = parser.value_float();
    // reset zoffset
    if(s==0 || ((s>=-0.00001)&&(s<=0.00001))){
      resetLevelingOffset();
    }
    else{
      setLevelingOffset(s);
    }
  }
 #endif
  else if ((parser.seenval('E')) && (parser.linearval('F') > 0)){ // E value and speed
    e_v = parser.value_axis_units(E_AXIS);
    the_feedrate = parser.value_feedrate();
    ExtUI::setAxisPosition_mm(e_v, ExtUI::E0, the_feedrate);
  }
}
// 选择U盘或者SD卡
void GcodeSuite::M1108(){
  uint16_t n=0;
  if (parser.seenval('S')){
    n = parser.value_ushort();
  }
  switch(n){
    case 1: dwinCurPage = 0;get_sdcard_list(); break;
    case 2:break;
    default:break;
  }
}
// 获取打印列表
void GcodeSuite::M1109(){
  uint16_t n=0;
  if (parser.seenval('S')){
    n = parser.value_ushort();
  }
  switch(n){
    case 1: select_file(0); break;
    case 2: select_file(1); break;
    case 3: select_file(2); break;
    case 4: select_file(3); break;
    case 5: select_file(4); break;
    case 6: select_file(5); break;
    case 7: if(dwinCurPage<=0) return; dwinCurPage--; get_sdcard_list(); break;
    case 8: if((dwinCurPage+1)*LIST_LINE >= read_filenum()) return; dwinCurPage++;  get_sdcard_list(); break;
    case 9: lcdExitDir(); break;
    default:break;
  }
}
void GcodeSuite::M1110(){
  uint16_t n=0, m=0, o=0;
  static uint8_t old_data_num = 0;
  if (parser.seenval('S')){
    n = parser.value_ushort();
  }
  else if (parser.seenval('Z')){
    m = parser.value_ushort();
    setGercentageValue(0, m);
  }
  else if (parser.seenval('F')){
    o = parser.value_ushort();
    setGercentageValue(1, o);
  }

  switch(n){
    // pause
    case 1: 
     #if 0//def HAS_UDISK
      if(udisk.isUdiskPrint()){
        if(printPaused2 == udisk.isUdiskPause()){
          printPaused2 = !udisk.isUdiskPause();
          resumeAndPauseIcon(printPaused2);
          setUDiskPrintPause();
        }
      }
      else
     #endif
      {
        if(printPaused == isPaused()){
          printPaused = !isPaused();
          resumeAndPauseIcon(printPaused);
          setPrintPause(!isPaused());
        }
      }
    break;

    // stop
    case 2: usart_lcd_stop_printf(); break;

    // case 3:
    //  #ifdef AUTO_BED_LEVELING_BILINEAR
    //   setLevelingOffset(getBabyStepZAxisTotalMM() - old_data_num);
    //   old_data_num = getBabyStepZAxisTotalMM();
    //  #endif
    // break;

    // // updata
    // case 4:
    //   old_data_num = getBabyStepZAxisTotalMM();
    // break;

    default:break;
  }
}

// babystep save
void GcodeSuite::M1111(){
  float s=0;
  if (parser.seenval('Z')){
    s = parser.value_float();
    setLevelingOffset(s);
  }
}
#endif
