#include "../TSC_Menu.h"

bool stop_home = false; // 停止复位的标志，注该标志影响到阻塞状态，慎用！！！记得及时关闭

#ifndef TEST_FW

  //1title, ITEM_PER_PAGE item
  const MENUITEMS homeItems = {
    // title
    LABEL_HOME,
    // icon                       label
    { {ICON_HOME,                 LABEL_HOME},
      {ICON_X_HOME,               LABEL_X_HOME},
      {ICON_Y_HOME,               LABEL_Y_HOME},
      {ICON_Z_HOME,               LABEL_Z_HOME},
      {ICON_EM_STOP,              LABEL_EMERGENCYSTOP},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACKGROUND,           LABEL_BACKGROUND},
      {ICON_BACK,                 LABEL_BACK},}
  };

  void menuCallBackHome(void)
  {
    KEY_VALUES key_num = menuKeyGetValue();

    switch(key_num)
    {
      case KEY_ICON_0:
        stop_home = false; storeCmd(PSTR("G28"));
        break;

      case KEY_ICON_1:
        stop_home = false; storeCmd(PSTR("G28 X"));
        break;

      case KEY_ICON_2:
        stop_home = false; storeCmd(PSTR("G28 Y"));
        break;

      case KEY_ICON_3:
        stop_home = false; storeCmd(PSTR("G28 Z"));
        break;

      case KEY_ICON_4:
        stop_home = true;
        storeCmd(PSTR("M410"));
        storeCmd(PSTR("M18"));
        break; //M410快速停止电机，在这用于关闭定时；M18/M84，解锁电机

      case KEY_ICON_7:
        stop_home = false; infoMenu.cur--;
        break;
        
      default:break;            
    }
  }

  void menuHome()
  {
    menuDrawPage(&homeItems);
    menuSetFrontCallBack(menuCallBackHome);
  }

#else
  static uint8_t move_num = 0;
  //1title, ITEM_PER_PAGE item
  MENUITEMS homeItems = {
  // title
  LABEL_HOME,
  // icon                       label
  {
    {ICON_Z_HOME,               LABEL_Z_HOME},
    {ICON_Z_DEC,                LABEL_Z_DEC},
    {ICON_Z_INC,                LABEL_Z_INC},
    {ICON_EM_STOP,              LABEL_EMERGENCYSTOP},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACK,                 LABEL_BACK},}
  };
  ITEM itemHomeLen[2] = {
    //icon          label
    {ICON_1_MM,   LABEL_1_MM},
    {ICON_10_MM,  LABEL_10_MM},
  };

  void menuCallBackHome(void)
  {
    KEY_VALUES key_num = menuKeyGetValue();

    switch(key_num)
    {
      case KEY_ICON_0: stop_home = false; storeCmd(PSTR("G28 XY")); break;
      case KEY_ICON_1: stop_home = false; if(move_num) storeCmd(PSTR("G1 X-10 Y-10")); else storeCmd(PSTR("G1 X-1 Y-1")); break;
      case KEY_ICON_2: stop_home = false; if(move_num) storeCmd(PSTR("G1 X10 Y10")); else storeCmd(PSTR("G1 X1 Y1")); break;
      case KEY_ICON_3: stop_home = true;  storeCmd(PSTR("M410")); storeCmd(PSTR("M18")); break; //M410快速停止电机，在这用于关闭定时；M18/M84，解锁电机
      case KEY_ICON_4: stop_home = false;
                       move_num = (move_num+1)%2;
                       homeItems.items[key_num] = itemHomeLen[move_num];
                       menuDrawItem(&homeItems.items[key_num], key_num);  break;
      case KEY_ICON_7: stop_home = false; storeCmd(PSTR("G90\n")); infoMenu.cur--; break;
      default:break;            
    }
  }

  void menuHome()
  {
    homeItems.items[4] = itemHomeLen[move_num];
    menuDrawPage(&homeItems);
    storeCmd(PSTR("G91\n"));
    menuSetFrontCallBack(menuCallBackHome);
  }

#endif
