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
  {ICON_SHUTDOWN,             LABEL_SLEEP},               //Turn display off button
  {ICON_RESUME,               LABEL_RESUME},              //Resume print button
  {ICON_SHUTDOWN,             LABEL_RESTART},          //Reboot machine button
  {ICON_TSCADJUST,            LABEL_TOUCHSCREEN_ADJUST},  //Touchscreen calibration button
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, 
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, 
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}               //Back button
};

void(* resetFunc) (void) = 0; // Software reboot the microcontroller by jumping to address 0

void showResetMessage() {
  GUI_Clear(BK_COLOR);
  //menuClearGaps();
  uint16_t start_y = (TITLE_END_Y - 10 - BYTE_HEIGHT) / 2;
  GUI_DispLenStringCustom(10, start_y, (char *) textSelect(LABEL_REBOOTING), LCD_WIDTH_PIXEL-20);
}

void menuCallBackTSCCalibrationConfirm(void)
{
  uint16_t key_num = KEY_GetValue(2, doubleBtnRect);
  switch(key_num)
  {
    case KEY_POPUP_CONFIRM:
      infoSettings.tsccalibnextboot = 1; // Set calibration to run on next boot
      storePara(); // Store settings
      showResetMessage();
      resetFunc(); // Reboot the microcontroller
      //infoMenu.cur--;
      break;

    case KEY_POPUP_CANCEL:	
      infoMenu.cur--;
      //infoMenu.menu[++infoMenu.cur] = menuStatus;
      break;		
  }
}

void menuCallBackRebootConfirm(void)
{
  uint16_t key_num = KEY_GetValue(2, doubleBtnRect);
  switch(key_num)
  {
    case KEY_POPUP_CONFIRM:
      showResetMessage();
      resetFunc(); // Reboot the microcontroller
      break;

    case KEY_POPUP_CANCEL:	
      infoMenu.cur--;
      break;
  }
}


void menuCallBackFunctionsmenu(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0: //Turn screen off
      // TODO: Add one-time popup about turning screen on again
      LCD_LED_Off(); //LCD_LED_On() to turn screen on; To activate screen again, click the screen - screen turns on when off and clicked (touch_process.cpp)
      infoMenu.cur = 0; // Send to main status screen
      break;
    case KEY_ICON_1: //Resume print
      // TODO: Add confirmation?
      //popupDrawPage(bottomDoubleBtn, textSelect(LABEL_POWER_FAILED), (uint8_t *)recovery.info.sd_filename, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
      ExtUI::setUserConfirmed();
      ExtUI::resumePrint();
      break;
    case KEY_ICON_2: //Restart machine
      // TODO: Add to Language files
      popupDrawPage(bottomDoubleBtn, textSelect(LABEL_CONFIRM), (uint8_t *)"Are you sure you want to restart the machine?", textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
      menuSetFrontCallBack(menuCallBackRebootConfirm);
      break;
    case KEY_ICON_3: //TSC Calibration
      // TODO: Add to Language files
      popupDrawPage(bottomDoubleBtn, textSelect(LABEL_RESTART_TO_CONTINUE), (uint8_t *)"This action requires a reboot to continue. Make sure you aren't printing or doing any data saving :)", textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
      menuSetFrontCallBack(menuCallBackTSCCalibrationConfirm);
      break;
    case KEY_ICON_7: //Back button
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
