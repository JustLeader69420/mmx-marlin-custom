#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include "stdint.h"

enum
{
  TFT_ENGLISH = 0,  //英
  TFT_CZECH,     //捷克语 - Čeština
  TFT_CZECH2,
  TFT_SLOVAK,    //斯洛伐克语
  TFT_GERMAN,       //德
  TFT_CHINESE,      //中
  TFT_RUSSIAN,      //俄
  TFT_JAPANESE,     //日
  // TFT_ARMENIAN,  //亚美尼亚语
  TFT_SPAIN,        //西班牙语
  TFT_FRENCH,       //法
  // TFT_PORTUGUESE,//葡萄牙语
  TFT_ITALIAN,      //意大利语
  // TFT_POLISH,    //波兰语
  // TFT_KOREAN,       //韩语
  TFT_TURKEY,       //土耳其语


// Add languages here, the order in which the languages are here also applies to the selection menu.
// These are defined in Language.cpp at the bottom

  LANGUAGE_NUM,
};

enum
{
  LABEL_LANGUAGE =0,
  LABEL_HEAT,
  LABEL_MOVE,
  LABEL_HOME,
  LABEL_PRINT,
  LABEL_EXTRUDE,
  LABEL_FAN,
  LABEL_SETTINGS,
  LABEL_SCREEN_SETTINGS,
  LABEL_MACHINE_SETTINGS,
  LABEL_FEATURE_SETTINGS,
  LABEL_SILENT_ON,
  LABEL_SILENT_OFF,
  LABEL_SHUT_DOWN,
  LABEL_RGB_SETTINGS,
  LABEL_RGB_RED,
  LABEL_RGB_GREEN,
  LABEL_RGB_BLUE,
  LABEL_RGB_WHITE,
  LABEL_RGB_OFF,
  LABEL_GCODE,
  LABEL_CUSTOM,
  LABEL_CUSTOM_0,
  LABEL_CUSTOM_1,
  LABEL_CUSTOM_2,
  LABEL_CUSTOM_3,
  LABEL_CUSTOM_4,
  LABEL_CUSTOM_5,
  LABEL_CUSTOM_6,
  LABEL_LEVELING,
  LABEL_POINT_1,
  LABEL_POINT_2,
  LABEL_POINT_3,
  LABEL_POINT_4,
  LABEL_ABL,
  LABEL_BLTOUCH,
  LABEL_BLTOUCH_TEST,
  LABEL_BLTOUCH_DEPLOY,
  LABEL_BLTOUCH_STOW,
  LABEL_BLTOUCH_REPEAT,
  LABEL_PROBE_OFFSET,
  LABEL_EEPROM_SAVE,
  LABEL_INC,
  LABEL_DEC,
  LABEL_NOZZLE,
  LABEL_BED,
  LABEL_STOP,
  LABEL_BACK,
  LABEL_PAGE_UP,
  LABEL_PAGE_DOWN,
  LABEL_PAUSE,
  LABEL_RESUME,
  LABEL_LOAD,
  LABEL_UNLOAD,
  LABEL_SLOW_SPEED,
  LABEL_NORMAL_SPEED,
  LABEL_FAST_SPEED,
  LABEL_FAN_FULL_SPEED,
  LABEL_FAN_HALF_SPEED,
  LABEL_ROTATE_UI,
  LABEL_TOUCHSCREEN_ADJUST,
  LABEL_MORE,
  LABEL_SCREEN_INFO,
  LABEL_WHITE,
  LABEL_BLACK,
  LABEL_BLUE,
  LABEL_RED,
  LABEL_GREEN,
  LABEL_CYAN,
  LABEL_YELLOW,
  LABEL_BROWN,
  LABEL_GRAY,
  LABEL_DISCONNECT,
  LABEL_BAUDRATE_115200,
  LABEL_BAUDRATE_250000,
  LABEL_RUNOUT_OFF,
  LABEL_RUNOUT_ON,
  LABEL_SMART_RUNOUT_ON,
  LABEL_PERCENTAGE,
  LABEL_BABYSTEP,
  LABEL_PERCENTAGE_SPEED,
  LABEL_PERCENTAGE_FLOW,
  LABEL_VALUE_ZERO,
  LABEL_1_DEGREE,
  LABEL_5_DEGREE,
  LABEL_10_DEGREE,
  LABEL_X_INC,
  LABEL_Y_INC,
  LABEL_Z_INC,
  LABEL_X_DEC,
  LABEL_Y_DEC,
  LABEL_Z_DEC,
  LABEL_X_HOME,
  LABEL_Y_HOME,
  LABEL_Z_HOME,
  LABEL_001_MM,
  LABEL_01_MM,
  LABEL_1_MM,
  LABEL_5_MM,
  LABEL_10_MM,
  LABEL_1_PERCENT,
  LABEL_5_PERCENT,
  LABEL_10_PERCENT,
  LABEL_READY,
  LABEL_BUSY,
  LABEL_UNCONNECTED,
  LABEL_DISCONNECT_INFO,
  LABEL_LOADING,
  LABEL_POWER_FAILED,
  LABEL_CONTINUE,
  LABEL_CANNEL,
  LABEL_ADJUST_TITLE,
  LABEL_ADJUST_INFO,
  LABEL_ADJUST_OK,
  LABEL_ADJUST_FAILED,
  LABEL_WARNING,
  LABEL_STOP_PRINT,
  LABEL_CONFIRM,
  LABEL_TFTSD,
  LABEL_READ_TFTSD_ERROR,
  LABEL_TFTSD_INSERTED,
  LABEL_TFTSD_REMOVED,
  LABEL_U_DISK,
  LABEL_READ_U_DISK_ERROR,
  LABEL_U_DISK_INSERTED,
  LABEL_U_DISK_REMOVED,
  LABEL_ONBOARDSD,
  LABEL_READ_ONBOARDSD_ERROR,
  LABEL_FILAMENT_RUNOUT,  
  LABEL_PREHEAT,
  LABEL_PREHEAT_BOTH,
  LABEL_PREHEAT_PLA,
  LABEL_PREHEAT_PETG,
  LABEL_PREHEAT_ABS,
  LABEL_PREHEAT_TPU,
  LABEL_PREHEAT_CUSTOM1,
  LABEL_PREHEAT_CUSTOM2,
  LABEL_IS_PAUSE,
  LABEL_AUTO_SHUT_DOWN,
  LABEL_MANUAL_SHUT_DOWN, 
  LABEL_UNIFIEDMOVE,
  lABEL_UNIFIEDHEAT,
  LABEL_COOLDOWN,
  LABEL_EMERGENCYSTOP,
  LABEL_TOUCH_TO_EXIT,
  LABEL_MAINMENU,
  LABEL_WAIT_TEMP_SHUT_DOWN,
  LABEL_FORCE_SHUT_DOWN,
  LABEL_SHUTTING_DOWN,
  LABEL_PRINTING_COMPLETED,
  LABEL_STATUS_INFO,
  LABEL_ERROR,
  LABEL_TIPS,
  LABEL_EEPROM_SAVE_SUCCESS,
  LABEL_EEPROM_SAVE_FAILED,
  LABEL_ELAPSED_TIME,
  LABEL_SAVE_POPUP,
  LABEL_SYCHRONZIED_VALUE,
  LABEL_CHOOSE_STORAGE,
  LABEL_FUNCTIONS, //Custom functions menu
  LABEL_SLEEP,     // Functions menu>turn off screen
  LABEL_RESTART_TO_CONTINUE, // Restart to continue
  LABEL_RESTART,             // Restart machine button
  
  LABEL_NUM,    //The amount of labels
  LABEL_BACKGROUND, // I guess this returns null, so nothing gets displayed
};

uint8_t * textSelect(uint8_t sel);

#endif
