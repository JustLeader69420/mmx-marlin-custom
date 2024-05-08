#include "Functions.h"
#include "../TSC_Menu.h"
//#include "../../../../extui/ui_api.h"   Doesn't seem to be needed
#include "../Hal/touch_process.h"
#include "../flashStore.h"
#include "../boot.h"
#include "../TSC_Menu.h"



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



void menuCallBackTSCCalibrationConfirm(void)
{
  uint16_t key_num = KEY_GetValue(2, doubleBtnRect);
  switch(key_num)
  {
    case KEY_POPUP_CONFIRM:
      // TODO: Do magic to calibrate on reboot
      //paraExists = false;
      HAL_reboot();
      //infoMenu.cur--;
      break;

    case KEY_POPUP_CANCEL:	
      //infoMenu.cur--;
      break;		
  }
}


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
      popupDrawPage(bottomDoubleBtn, textSelect(LABEL_RESTART_TO_CONTINUE), (uint8_t *)"This action requires a reboot to continue", textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
      menuSetFrontCallBack(menuCallBackTSCCalibrationConfirm);

      //TSC_Calibration();
      //storePara();
      infoMenu.menu[++infoMenu.cur] = menuStatus;
      break;

    case KEY_ICON_2:
      //infoMenu.menu[++infoMenu.cur] = menuFeatureSettings; // Increments menu depth, displays next menu?
      LCD_LED_Off();
      // To activate screen again, click the screen

      //LCD_LED_On(); to turn screen on
      break;

    case KEY_ICON_3:
      HAL_reboot();
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
