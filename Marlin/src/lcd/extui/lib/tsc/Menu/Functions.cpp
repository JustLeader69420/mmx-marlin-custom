#include "Functions.h"
#include "../TSC_Menu.h"



MENUITEMS functionsItems = {
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


void menuCallBackFunctions(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0: 
      // TODO: Verify corrent print resume button function
      ExtUI::setUserConfirmed();
      ExtUI::resumePrint();
      break;
    
    case KEY_ICON_2:
      infoMenu.menu[++infoMenu.cur] = menuFeatureSettings; // Increments menu depth, displays next menu?
      break;

    case KEY_ICON_1:
      // TODO: Verify corrent touchscreen calibration button function
      TSC_Calibration();
      storePara();
      LOGO_ReadDisplay();
      break;

    case KEY_ICON_7:
      infoMenu.cur--;
      break;
    
    default:
      break;
  }
}

void menuSettings(void)
{

  //Some other click processing stuff goes here ig

  menuDrawPage(&functionsItems);
  menuSetFrontCallBack(menuCallBackFunctions);

}
