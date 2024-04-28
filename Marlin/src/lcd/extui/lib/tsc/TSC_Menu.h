#pragma once

#include "../../../../inc/MarlinConfigPre.h"

#include "../../ui_api.h"

#include "../../../../MarlinCore.h"
#include "../../../../module/temperature.h"
#include "../../../../module/motion.h"
#include "../../../../gcode/queue.h"
#include "../../../../module/planner.h"
#include "../../../../sd/cardreader.h"
#include "../../../../libs/duration_t.h"
#include "../../../../module/printcounter.h"
#ifdef HAS_UDISK
  #include "../../../../udisk/udiskPrint.h"
#endif

#ifdef AUTO_BED_LEVELING_BILINEAR
  #include "../../../../feature/bedlevel/bedlevel.h"
#endif
// Hardware
#include "Hal/stm32f4_fsmc.h"
#include "Hal/sw_spi.h"
#include "Hal/w25qxx.h"
#include "Hal/xpt2046.h"
#include "Hal/touch_process.h"
#include "Hal/STM32_Flash.h"

// API
#include "Language/utf8_decode.h"
#include "Language/Language.h"
#include "LCD_Init.h"
#include "GUI.h"
#include "boot.h"
#include "flashStore.h"

// Menu
// NOTE: ORDER MATTERS
#include "Menu/Popup.h"
#include "Menu/PowerFailed.h"
#include "Menu/UsrConfirm.h"
#include "Menu/menu.h"
#include "Menu/Functions.h"
#include "Menu/StatusScreen.h"
#include "Menu/MainPage.h"
#include "Menu/PreheatMenu.h"
#include "Menu/Heat.h"
#include "Menu/Move.h"
#include "Menu/Home.h"
#include "Menu/Print.h"
#include "Menu/Printing.h"
#include "Menu/Speed.h"
#include "Menu/BabyStep.h"
#include "Menu/More.h"
#include "Menu/Extrude.h"
#include "Menu/Fan.h"
#include "Menu/Settings.h"
#include "Menu/ScreenSettings.h"
#include "Menu/TSCAdjust.h"
#include "Menu/MachineSettings.h"
#include "Menu/FeatureSettings.h"
#include "Menu/leveling.h"
#include "Menu/ProbeOffset.h"
#include "Menu/PrintUdisk.h"
#include "Menu/ChooseStorage.h"
#include "Menu/PreheatSet.h"
#if ENABLED(NEW_BOARD)
  #include "Menu/Test_current.h"
  #include "Menu/Test_endstop.h"
  #include "Menu/Test_music.h"
#endif
#if 1//ENABLED(LEVELING_OFFSET)
  #include "Menu/LevelingOffset.h"
#endif
#include "Menu/Developer_mode.h"

typedef enum
{
  sound_success,
  sound_error,
  sound_ok,
  sound_cancel,
  sound_notify,
  sound_keypress,
}SOUND;
void Buzzer_play(SOUND sound);

#define MAX_MENU_DEPTH 10       // max sub menu depth
typedef void (*FP_MENU)(void); 

typedef struct
{
  FP_MENU menu[MAX_MENU_DEPTH];  // Menu function buffer
  uint8_t cur;                   // Current menu index in buffer
}MENU;

extern MENU  infoMenu;  // Menu structure
extern TSCBoot boot;

void menuUpdate(void);
void menuSetFrontCallBack(FP_MENU cb);
void LCD_Setup();
bool storeCmd(const char *cmd);
void mustStoreCmd(const char *cmd);
