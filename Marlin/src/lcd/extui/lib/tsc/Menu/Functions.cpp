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
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, 
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, 
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, 
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};


void menuCallBackFunctionsmenu(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0: 
      // TODO: Verify correct print resume button function
      //popupDrawPage(bottomDoubleBtn, textSelect(LABEL_POWER_FAILED), (uint8_t *)recovery.info.sd_filename, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
      ExtUI::setUserConfirmed();
      ExtUI::resumePrint();
      break;
    
    case KEY_ICON_2:
      //infoMenu.menu[++infoMenu.cur] = menuFeatureSettings; // Increments menu depth, displays next menu?
      break;

    case KEY_ICON_1:
      // TODO: Verify correct touchscreen calibration button function
      //popupDrawPage(bottomDoubleBtn, textSelect(LABEL_POWER_FAILED), (uint8_t *)recovery.info.sd_filename, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
      TSC_Calibration();
      storePara();
      LOGO_ReadDisplay();
      infoMenu.menu[++infoMenu.cur] = menuStatus;
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
