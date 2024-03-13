#ifndef _HEAT_H_
#define _HEAT_H_
#include "menu.h"
#include "stdint.h"

#define HEAT_DISPLAY_ID  {"Bed",  "T0",      "T1",      "T2",      "T3",      "T4",      "T5"}
#define ITEM_DEGREE_NUM 3
extern const ITEM itemTool[];
extern const ITEM itemDegree[ITEM_DEGREE_NUM];
extern const  uint8_t item_degree[ITEM_DEGREE_NUM];
extern const char* const heatDisplayID[];

void heatSetCurHeaterIndex(uint8_t index);
void menuHeat(void);

#endif
