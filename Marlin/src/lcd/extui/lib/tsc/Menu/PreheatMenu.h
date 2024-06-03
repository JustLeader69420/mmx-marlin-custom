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
    THE_PREHEAT1=0, //was PLA
    THE_PREHEAT2,   //was ABS
    THE_PREHEAT3,   //was TPU
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
const uint16_t originalPreheatTemp[THE_MATERIAL_NUM][2] = {{PREHEAT_1_TEMP_BED, PREHEAT_1_TEMP_HOTEND}, {PREHEAT_2_TEMP_BED, PREHEAT_2_TEMP_HOTEND}, {PREHEAT_3_TEMP_BED,PREHEAT_3_TEMP_HOTEND}};
extern const GUI_RECT info_rect[];

void menuPreheat(void);
void getCurrentPreheat(uint8_t name, uint16_t *Temp);
void setCurrentPreheat(uint8_t name, uint16_t *Temp);
void drawTemperatureInfo(void);

#endif

