#include "../TSC_Menu.h"
#include "Popup.h"
#include "../../../../../gcode/gcode.h"

//1 title, ITEM_PER_PAGE items(icon+label) 
const MENUITEMS mainPageItems = {
// title
LABEL_MAINMENU,
// icon                       label
  #ifdef UNIFIED_MENU //if Unified menu is selected
   {{ICON_HEAT_FAN,             lABEL_UNIFIEDHEAT},
    {ICON_HOME_MOVE,            LABEL_UNIFIEDMOVE},
    {ICON_EXTRUDE,              LABEL_EXTRUDE},
    {ICON_PRINT,                LABEL_PRINT},
    {ICON_GCODE,                LABEL_GCODE},
    {ICON_STOP,                 LABEL_EMERGENCYSTOP},
    {ICON_SETTINGS,             LABEL_SETTINGS},
    {ICON_BACK,                 LABEL_BACK},}
  #else
  #ifdef STATUS_SCREEN
   {{ICON_HEAT,                 LABEL_PREHEAT},
    {ICON_MOVE,                 LABEL_MOVE},
    {ICON_HOME,                 LABEL_HOME},
    {ICON_LEVELING,             LABEL_LEVELING},
    {ICON_EXTRUDE,              LABEL_EXTRUDE},
    {ICON_FAN,                  LABEL_FAN},
    {ICON_SETTINGS,             LABEL_SETTINGS},
    {ICON_BACK,                 LABEL_BACK},}
  #else
   {{ICON_MOVE,                 LABEL_MOVE},
    {ICON_HOME,                 LABEL_HOME},
    {ICON_EXTRUDE,              LABEL_EXTRUDE},
    {ICON_PERCENTAGE,           LABEL_PERCENTAGE},
    {ICON_SETTINGS,             LABEL_SETTINGS},
    {ICON_FAN,                  LABEL_FAN},
    #ifdef AUTO_BED_LEVELING_BILINEAR
     #ifdef LEVELING_OFFSET
      {ICON_PROBE_OFFSET,         LABEL_PROBE_OFFSET},
     #else
      {ICON_BABYSTEP,         LABEL_BABYSTEP},
     #endif
    #else
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
    #endif
    {ICON_BACK,                 LABEL_BACK}}
    #endif
  #endif
};
static void needGoHome(void){
  char text[32]="Homing...\nPlease wait...";
  char text2[32];
  wait_for_user_response(5);
  GUI_Clear(BLACK);
  // GUI_DispStringInRect(0, 0, LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL, /*textSelect(LABEL_HOME)*/"homing...");
  popupDrawPage(NULL , textSelect(LABEL_TIPS), (uint8_t *)text, NULL, NULL);
  key_lock = true;
  // memset(text, 0, 32);
  gcode.process_subcommands_now("G28");
  gcode.process_subcommands_now("M420 S1");
  // ExtUI::delay_ms(10);
  wait_for_user_response(5);
  planner.synchronize();
  sprintf_P(text2, "G0 X%d Y%d Z0 F%d", X_BED_SIZE/2, Y_BED_SIZE/2, HOMING_FEEDRATE_XY);
  gcode.process_subcommands_now(text2);
  planner.synchronize();
  // sprintf_P(text2, "G0 Z0 F%d", HOMING_FEEDRATE_Z);
  // gcode.process_subcommands_now(text);
  // planner.synchronize();
  key_lock = false;
  // infoMenu.cur--;
  infoMenu.menu[++infoMenu.cur] = menuBabyStep;
}
static void noGoHome(void){
  infoMenu.menu[++infoMenu.cur] = menuBabyStep;
}
void menuCallBackMainPage() {
  KEY_VALUES key_num = menuKeyGetValue();
  switch(key_num)
  {
    #ifdef UNIFIED_MENU //if Unified menu is selected
      case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;     break;
      case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuUnifiedMove;     break;      
      case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuExtrude;         break;
      case KEY_ICON_3: infoMenu.menu[++infoMenu.cur] = menuPrint;           break;
      case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuSendGcode;       break;
      case KEY_ICON_5: { 
        /*
        for(TOOL i = BED; i < HEATER_NUM; i++)
        {
          storeCmd("%s S0\n", heatCmd[i]);  disable all heaters
        }
        storeCmd("M18\n");  //disable all stepper motors
        storeCmd("M107\n"); //disable cooling fan
        */
        storeCmd("M112\n"); //Emergency Stop : Used for emergency stopping, a reset is required to return to operational mode.
                            // it may need to wait for a space to open up in the command queue.
                            // Enable EMERGENCY_PARSER in Marlin Firmware for an instantaneous M112 command.

          }
        break;
      
      case KEY_ICON_6: infoMenu.menu[++infoMenu.cur] = menuSettings;        break;
      case KEY_ICON_7: infoMenu.cur--;        break;
      default:break;
    #else
    #ifdef STATUS_SCREEN
      case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuPreheat;   break;
      case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuMove;      break;
      case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuHome;      break;
      case KEY_ICON_3:
        #ifdef AUTO_BED_LEVELING
          infoMenu.menu[++infoMenu.cur] = menuAutoLeveling;
        #else
          infoMenu.menu[++infoMenu.cur] = menuManualLeveling;
        #endif
        break;      
      case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuExtrude;   break;
      case KEY_ICON_5: infoMenu.menu[++infoMenu.cur] = menuFan;       break;
      case KEY_ICON_6: infoMenu.menu[++infoMenu.cur] = menuSettings;  break;
      case KEY_ICON_7: infoMenu.cur--;        break;
      default:break;
    #else
      //case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuPreheat;   break;
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuMove;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuHome;
        break;

      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuExtrude;
        break;

      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuSpeed;
        break;

      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuSettings;
        break;

      case KEY_ICON_5:
        infoMenu.menu[++infoMenu.cur] = menuFan;
        break;

     #ifdef AUTO_BED_LEVELING_BILINEAR
      case KEY_ICON_6:
        #if ENABLED(LEVELING_OFFSET)
          // infoMenu.menu[++infoMenu.cur] = menuLevelingOffset;
          setLO_flag(true);
          infoMenu.menu[++infoMenu.cur] = menuSetLevelingValue;
        #else
          setDialogInfo(bottomDoubleBtn, textSelect(LABEL_TIPS), (uint8_t*)"Do homing now?", (uint8_t*)"Yes", (uint8_t*)"No", needGoHome, noGoHome);
          infoMenu.menu[++infoMenu.cur] = menuDialog;
        #endif
        break;
     #endif

      case KEY_ICON_7:
        infoMenu.cur--;
        break;
        
      default:break;
    #endif
    #endif
  }
}

void menuMain(void)
{
  menuDrawPage(&mainPageItems);
  menuSetFrontCallBack(menuCallBackMainPage);
}
