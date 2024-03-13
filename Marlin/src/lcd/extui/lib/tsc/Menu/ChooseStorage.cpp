#include "../TSC_Menu.h"

#ifdef HAS_UDISK

#include "Print.h"
#include "PrintUdisk.h"

//1title, ITEM_PER_PAGE item
const MENUITEMS chooseStorageItems = {
// title
LABEL_PRINT,
// icon                 label
 {{ICON_BACKGROUND,     LABEL_BACKGROUND},
  {ICON_SD_SOURCE,      LABEL_TFTSD},
  {ICON_UDISK,          LABEL_U_DISK},
  {ICON_BACKGROUND,     LABEL_BACKGROUND},
  {ICON_BACKGROUND,     LABEL_BACKGROUND},
  {ICON_BACKGROUND,     LABEL_BACKGROUND},
  {ICON_BACKGROUND,     LABEL_BACKGROUND},
  {ICON_BACK,           LABEL_BACK},}
};

void menuCallBackChooseStorage() {
    KEY_VALUES key_num = menuKeyGetValue();
    switch(key_num)
    {
        case KEY_ICON_1: 
            if(z_values[1][1] != 0){
                planner.leveling_active = true;
                set_bed_leveling_enabled(true);
            }
            enter_by_icon = true;
            infoMenu.menu[++infoMenu.cur] = menuPrint;
            break;
        
        case KEY_ICON_2:
            infoMenu.menu[++infoMenu.cur] = menuPrintUdisk;
            break;      

        case KEY_ICON_7:
            infoMenu.cur--;
            break;
            
        default: break;
    }
}

void menuChooseStorage()
{
    
    menuDrawPage(&chooseStorageItems);
    menuSetFrontCallBack(menuCallBackChooseStorage);
}

#endif
