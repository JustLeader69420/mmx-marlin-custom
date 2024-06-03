// #include "PreheatMenu.h"
#include "../TSC_Menu.h"


MENUITEMS preheatItems = {
// title
LABEL_PREHEAT,
// icon                       label
  {
    {ICON_PREHEAT,              LABEL_BACKGROUND}, //was LABEL_PREHEAT_PLA
    {ICON_PREHEAT,              LABEL_BACKGROUND}, //was LABEL_PREHEAT_ABS
    {ICON_PREHEAT,              LABEL_BACKGROUND}, //was LABEL_PREHEAT_TPU
    {ICON_BOTH,                 LABEL_PREHEAT_BOTH},
    {ICON_STOP,                 LABEL_STOP},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_FEATURE_SETTINGS,     LABEL_SETTINGS},
//    {ICON_PREHEAT_CUSTOM1,      LABEL_PREHEAT_CUSTOM1},
//    {ICON_PREHEAT_CUSTOM2,      LABEL_PREHEAT_CUSTOM2},
    // #ifdef UNIFIED_MENU
    // {ICON_BACKGROUND,           LABEL_BACKGROUND},
    // #else
    // {ICON_HEAT,                 LABEL_HEAT},
    // #endif
    {ICON_BACK,                 LABEL_BACK},
  }
};

const ITEM itemToolPreheat[] = {
// icon                       label
  {ICON_BOTH,         LABEL_PREHEAT_BOTH},
  {ICON_BED,                  LABEL_BED},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
};   

const uint16_t preheat_bed_temp[] = PREHEAT_BED;
const uint16_t preheat_hotend_temp[] = PREHEAT_HOTEND;
uint16_t PreheatTemp[THE_MATERIAL_NUM][2] = {{75, 210}, {95, 255}, {80,220}};

/* 获取当前预设温度
 * @param name  哪种材料
 * @param Temp  温度值
 */
void getCurrentPreheat(uint8_t name, uint16_t *Temp){
  Temp[0] = PreheatTemp[name][0];
  Temp[1] = PreheatTemp[name][1];
}
/* 设置预设温度
 * @param name  哪种材料
 * @param Temp  温度值
 */
void setCurrentPreheat(uint8_t name, uint16_t *Temp){
  PreheatTemp[name][0] = Temp[0];
  PreheatTemp[name][1] = Temp[1];
}

const GUI_RECT info_rect[] = {
  {START_X + 0 * ICON_WIDTH + 0 * SPACE_X + INFO_OFFSET_X0,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y0,
   START_X + 1 * ICON_WIDTH + 0 * SPACE_X - INFO_OFFSET_X1,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y0 + BYTE_HEIGHT},

  {START_X + 0 * ICON_WIDTH + 0 * SPACE_X + INFO_OFFSET_X0 + 4,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y1,
   START_X + 1 * ICON_WIDTH + 0 * SPACE_X - INFO_OFFSET_X1,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y1 + BYTE_HEIGHT},
  
  {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + INFO_OFFSET_X0,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y0,
   START_X + 2 * ICON_WIDTH + 1 * SPACE_X - INFO_OFFSET_X1,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y0 + BYTE_HEIGHT},
  
  {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + INFO_OFFSET_X0 + 4,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y1,
   START_X + 2 * ICON_WIDTH + 1 * SPACE_X - INFO_OFFSET_X1,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y1 + BYTE_HEIGHT},

  {START_X + 2 * ICON_WIDTH + 2 * SPACE_X + INFO_OFFSET_X0,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y0,
   START_X + 3 * ICON_WIDTH + 2 * SPACE_X - INFO_OFFSET_X1,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y0 + BYTE_HEIGHT},

  {START_X + 2 * ICON_WIDTH + 2 * SPACE_X + INFO_OFFSET_X0 + 4,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y1,
   START_X + 3 * ICON_WIDTH + 2 * SPACE_X - INFO_OFFSET_X1,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y1 + BYTE_HEIGHT},
  
  {START_X + 3 * ICON_WIDTH + 3 * SPACE_X + INFO_OFFSET_X0,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y0,
   START_X + 4 * ICON_WIDTH + 3 * SPACE_X - INFO_OFFSET_X1,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y0 + BYTE_HEIGHT},
  
  {START_X + 3 * ICON_WIDTH + 3 * SPACE_X + INFO_OFFSET_X0 + 4,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y1,
   START_X + 4 * ICON_WIDTH + 3 * SPACE_X - INFO_OFFSET_X1,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y1 + BYTE_HEIGHT},
};

void drawTemperatureInfo(void)
{
  GUI_SetColor(BLACK);
  GUI_SetBkColor(WHITE);

  GUI_ClearPrect(&info_rect[0]);
  GUI_DispDec(info_rect[0].x0, info_rect[0].y0, PreheatTemp[THE_PREHEAT1][THE_NOZZLE], 3, LEFT);

  GUI_ClearPrect(&info_rect[1]);
  GUI_DispDec(info_rect[1].x0, info_rect[1].y0, PreheatTemp[THE_PREHEAT1][THE_BED], 3, LEFT);

  GUI_ClearPrect(&info_rect[2]);
  GUI_DispDec(info_rect[2].x0, info_rect[2].y0, PreheatTemp[THE_PREHEAT2][THE_NOZZLE], 3, LEFT);

  GUI_ClearPrect(&info_rect[3]);
  GUI_DispDec(info_rect[3].x0, info_rect[3].y0, PreheatTemp[THE_PREHEAT2][THE_BED], 3, LEFT);

  GUI_ClearPrect(&info_rect[4]);
  GUI_DispDec(info_rect[4].x0, info_rect[4].y0, PreheatTemp[THE_PREHEAT3][THE_NOZZLE], 3, LEFT);

  GUI_ClearPrect(&info_rect[5]);
  GUI_DispDec(info_rect[5].x0, info_rect[5].y0, PreheatTemp[THE_PREHEAT3][THE_BED], 3, LEFT);
  
  GUI_SetColor(FK_COLOR);
  GUI_SetBkColor(BK_COLOR);
}
// 绘制有颜色的温度
static void drawTemperatureColor(void)
{
  GUI_SetBkColor(WHITE);

  {
    if(ExtUI::getTargetTemp_celsius(ExtUI::getActiveTool()) == PreheatTemp[THE_PREHEAT1][THE_NOZZLE]) GUI_SetColor(ORANGE);
    else  GUI_SetColor(BLACK);
    GUI_ClearPrect(&info_rect[0]);
    GUI_DispDec(info_rect[0].x0, info_rect[0].y0, PreheatTemp[THE_PREHEAT1][THE_NOZZLE], 3, LEFT);
  }
  {
    if(ExtUI::getTargetTemp_celsius(ExtUI::BED) == PreheatTemp[THE_PREHEAT1][THE_BED]) GUI_SetColor(ORANGE);
    else  GUI_SetColor(BLACK);
    GUI_ClearPrect(&info_rect[1]);
    GUI_DispDec(info_rect[1].x0, info_rect[1].y0, PreheatTemp[THE_PREHEAT1][THE_BED], 3, LEFT);
  }
  {
    if(ExtUI::getTargetTemp_celsius(ExtUI::getActiveTool()) == PreheatTemp[THE_PREHEAT2][THE_NOZZLE]) GUI_SetColor(ORANGE);
    else  GUI_SetColor(BLACK);
    GUI_ClearPrect(&info_rect[2]);
    GUI_DispDec(info_rect[2].x0, info_rect[2].y0, PreheatTemp[THE_PREHEAT2][THE_NOZZLE], 3, LEFT);
  }
  {
    if(ExtUI::getTargetTemp_celsius(ExtUI::BED) == PreheatTemp[THE_PREHEAT2][THE_BED]) GUI_SetColor(ORANGE);
    else  GUI_SetColor(BLACK);
    GUI_ClearPrect(&info_rect[3]);
    GUI_DispDec(info_rect[3].x0, info_rect[3].y0, PreheatTemp[THE_PREHEAT2][THE_BED], 3, LEFT);
  }
  {
    if(ExtUI::getTargetTemp_celsius(ExtUI::getActiveTool()) == PreheatTemp[THE_PREHEAT3][THE_NOZZLE]) GUI_SetColor(ORANGE);
    else  GUI_SetColor(BLACK);
    GUI_ClearPrect(&info_rect[4]);
    GUI_DispDec(info_rect[4].x0, info_rect[4].y0, PreheatTemp[THE_PREHEAT3][THE_NOZZLE], 3, LEFT);
  }
  {
    if(ExtUI::getTargetTemp_celsius(ExtUI::BED) == PreheatTemp[THE_PREHEAT3][THE_BED]) GUI_SetColor(ORANGE);
    else  GUI_SetColor(BLACK);
    GUI_ClearPrect(&info_rect[5]);
    GUI_DispDec(info_rect[5].x0, info_rect[5].y0, PreheatTemp[THE_PREHEAT3][THE_BED], 3, LEFT);
  }
  
  GUI_SetColor(FK_COLOR);
  GUI_SetBkColor(BK_COLOR);
}

void menuCallBackPreHeat() {
  static TOOLPREHEAT nowHeater = BOTH;
  KEY_VALUES  key_num = menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0:
    case KEY_ICON_1:
    case KEY_ICON_2:
      switch(nowHeater){
        case BOTH:
          ExtUI::setTargetTemp_celsius(PreheatTemp[key_num][THE_NOZZLE],  ExtUI::getActiveTool());
          ExtUI::setTargetTemp_celsius(PreheatTemp[key_num][THE_BED],  ExtUI::BED);
          break;
        case BED_PREHEAT:
          ExtUI::setTargetTemp_celsius(PreheatTemp[key_num][THE_BED],  ExtUI::BED);
          ExtUI::setTargetTemp_celsius(0,  ExtUI::getActiveTool());
          break;
        case NOZZLE0_PREHEAT:
          ExtUI::setTargetTemp_celsius(PreheatTemp[key_num][THE_NOZZLE],  ExtUI::getActiveTool());
          ExtUI::setTargetTemp_celsius(0,  ExtUI::BED);
          break;
      }
      drawTemperatureColor();
      break;
      
    case KEY_ICON_3:
      nowHeater = (TOOLPREHEAT)((nowHeater+1) % 3);
      preheatItems.items[key_num] = itemToolPreheat[nowHeater];
      menuDrawItem(&preheatItems.items[key_num], key_num);
      break;

    case KEY_ICON_4:
          ExtUI::setTargetTemp_celsius(0,  ExtUI::getActiveTool());
          ExtUI::setTargetTemp_celsius(0,  ExtUI::BED);
          drawTemperatureColor();
          break;
    
    // #ifndef UNIFIED_MENU
    case KEY_ICON_6:
      infoMenu.menu[++infoMenu.cur] = menuPreheatSet;
      break;
    
    // #endif
    case KEY_ICON_7:
      infoMenu.cur--; break;
    default:break;
  }
  // drawTemperatureInfo();
}

void menuPreheat(void)
{
  menuDrawPage(&preheatItems);
  menuDrawItemLabel(PREHEAT_1_LABEL, 0);
  menuDrawItemLabel(PREHEAT_2_LABEL, 1);
  menuDrawItemLabel(PREHEAT_3_LABEL, 2);
  drawTemperatureColor();
  menuSetFrontCallBack(menuCallBackPreHeat);
}
