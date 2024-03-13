#ifndef _PREHEATMENU_H_
#define _PREHEATMENU_H_

//                       PLA       ABS   "CUSTOM2" "SHUTDOWN" "CUSTOM3"
#define PREHEAT_BED      {75,      95,       55,       0,       55}
#define PREHEAT_HOTEND   {210,     255,      200,      0,      200}

#define INFO_OFFSET_X0  54
#define INFO_OFFSET_X1  15
#define INFO_OFFSET_Y0  20
#define INFO_OFFSET_Y1  61

enum{
    THE_PLA=0,
    THE_ABS,
    THE_TPU,
    THE_MATERIAL_NUM,
};  // 枚举材料
typedef enum
{
  BOTH = 0,
  BED_PREHEAT = 1,
  NOZZLE0_PREHEAT = 2,
}TOOLPREHEAT;
enum{
  THE_BED=0,
  THE_NOZZLE,
};

extern uint16_t PreheatTemp[THE_MATERIAL_NUM][2];
const uint16_t originalPreheatTemp[THE_MATERIAL_NUM][2] = {{75, 210}, {95, 255}, {80,220}};
extern const GUI_RECT info_rect[];

void menuPreheat(void);
void getCurrentPreheat(uint8_t name, uint16_t *Temp);
void setCurrentPreheat(uint8_t name, uint16_t *Temp);
void drawTemperatureInfo(void);

#endif

