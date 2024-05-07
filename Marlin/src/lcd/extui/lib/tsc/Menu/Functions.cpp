#include "Functions.h"
#include "../TSC_Menu.h"
//#include "../../../../extui/ui_api.h"   Doesn't seem to be needed
#include "../Hal/touch_process.h"
#include "../flashStore.h"
#include "../boot.h"



MENUITEMS functionsmenuItems = {
// title
LABEL_FUNCTIONS,
// icon                       label
 {
  {ICON_RESUME,               LABEL_RESUME},              //Resume print button
  {ICON_TSCADJUST,            LABEL_TOUCHSCREEN_ADJUST},  //Touchscreen calibration button
  {ICON_SHUTDOWN,             LABEL_SLEEP},               //Turn display off button
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, 
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, 
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, 
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}               //Back button
};


void menuCallBackFunctionsmenu(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0: 
      //popupDrawPage(bottomDoubleBtn, textSelect(LABEL_POWER_FAILED), (uint8_t *)recovery.info.sd_filename, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
      ExtUI::setUserConfirmed();
      ExtUI::resumePrint();
      break;

    case KEY_ICON_1:
      //popupDrawPage(bottomDoubleBtn, textSelect(LABEL_POWER_FAILED), (uint8_t *)recovery.info.sd_filename, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
      TSC_Calibration();
      storePara();
      LOGO_ReadDisplay();
      infoMenu.menu[++infoMenu.cur] = menuStatus;
      // TODO: Reboot instead, menu moving is weird, could exhaust the menu tree depth limit, if user calibrates screen multiple times without rebooting?
      break;

    case KEY_ICON_2:
      //infoMenu.menu[++infoMenu.cur] = menuFeatureSettings; // Increments menu depth, displays next menu?
      LCD_LED_Off();
      // To activate screen again, click the screen

      //LCD_LED_On(); to turn screen on
      break;

    case KEY_ICON_7:
      infoMenu.cur--;
      break;
    
    default:
      break;
  }
}

void menuFunctionsmenu(void)
{

  menuDrawPage(&functionsmenuItems);
  menuSetFrontCallBack(menuCallBackFunctionsmenu);
  

}
