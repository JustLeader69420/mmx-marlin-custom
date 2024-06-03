#if 1
#include "../TSC_Menu.h"
/************************************界面参数**************************************************/
MENUITEMS preheatSetItems = {       // 选择界面
    // title
    LABEL_SETTINGS,
    // icon                       label
    {{ICON_PREHEAT,              LABEL_BACKGROUND}, //was LABEL_PREHEAT_PLA
    {ICON_PREHEAT,              LABEL_BACKGROUND}, //was LABEL_PREHEAT_ABS
    {ICON_PREHEAT,              LABEL_BACKGROUND}, //was LABEL_PREHEAT_TPU
    // {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACK,                 LABEL_BACK},}
};
extern const GUI_RECT info_rect[4];
MENUITEMS setPreheatItems = {       // 设置界面
    // title
    LABEL_BACKGROUND,
    // icon                       label
    {{ICON_DEC,                  LABEL_DEC},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_INC,                  LABEL_INC},
    {ICON_NOZZLE,               LABEL_NOZZLE},
    {ICON_5_DEGREE,             LABEL_5_DEGREE},
    {ICON_EEPROM_SAVE,          LABEL_EEPROM_SAVE},
    {ICON_BACK,                 LABEL_BACK},}
};
extern const ITEM itemTool[];       // 选择设置目标的图标和标签
extern const ITEM itemDegree[ITEM_DEGREE_NUM];  // 变化量图标和标签，1、5、10
const GUI_RECT preheatRect[4] = {   // 中间的显示区域
    {LCD_WIDTH_PIXEL/2-7*BYTE_WIDTH, TITLE_END_Y+(ICON_HEIGHT-1)/2-BYTE_HEIGHT, LCD_WIDTH_PIXEL/2, TITLE_END_Y+(ICON_HEIGHT-1)/2},
    {LCD_WIDTH_PIXEL/2+1, TITLE_END_Y+(ICON_HEIGHT-1)/2-BYTE_HEIGHT, LCD_WIDTH_PIXEL/2+BYTE_WIDTH*4+1, TITLE_END_Y+(ICON_HEIGHT-1)/2},
    {LCD_WIDTH_PIXEL/2-4*BYTE_WIDTH, TITLE_END_Y+(ICON_HEIGHT+1)/2, LCD_WIDTH_PIXEL/2, TITLE_END_Y+(ICON_HEIGHT+1)/2+BYTE_HEIGHT},
    {LCD_WIDTH_PIXEL/2+1, TITLE_END_Y+(ICON_HEIGHT+1)/2, LCD_WIDTH_PIXEL/2+BYTE_WIDTH*4+1, TITLE_END_Y+(ICON_HEIGHT+1)/2+BYTE_HEIGHT}
};

/************************************函数参数**************************************************/
const uint8_t MaterialPreset[][8] = {"PLA", "ABS"};
static uint8_t MaterialNum = 0;
static uint8_t preheatIndex = 1;
static uint16_t oldTemp[2] = {0};   //旧预设值内装有床的温度，喷嘴的温度
static uint16_t newTemp[2] = {0};   //新预设值内装有床的温度，喷嘴的温度
extern const  uint8_t item_degree[ITEM_DEGREE_NUM];
static bool changeNum = false;        // 数值是否发生变化
static uint8_t changeIndex = 1;   // 变化量

/************************************设置界面**************************************************/
void showPreheatTemp(){
    for(uint8_t i=0;i<4;i++)    GUI_ClearPrect(&preheatRect[i]);
    switch (preheatIndex){
        case 0:
            GUI_SetColor(ORANGE);
            GUI_SetBkColor(BLACK);
            GUI_DispStringInPrect(&preheatRect[2], (const uint8_t *)"BED:");
            GUI_DispDec(preheatRect[3].x0, preheatRect[3].y0, newTemp[0], 3, RIGHT);
            GUI_SetColor(FK_COLOR);
            GUI_SetBkColor(BK_COLOR);
            GUI_DispStringInPrect(&preheatRect[0], (const uint8_t *)"NOZZLE:");
            GUI_DispDec(preheatRect[1].x0, preheatRect[1].y0, newTemp[1], 3, RIGHT);
        break;

        case 1:
            GUI_SetColor(ORANGE);
            GUI_SetBkColor(BLACK);
            GUI_DispStringInPrect(&preheatRect[0], (const uint8_t *)"NOZZLE:");
            GUI_DispDec(preheatRect[1].x0, preheatRect[1].y0, newTemp[1], 3, RIGHT);
            GUI_SetColor(FK_COLOR);
            GUI_SetBkColor(BK_COLOR);
            GUI_DispStringInPrect(&preheatRect[2], (const uint8_t *)"BED:");
            GUI_DispDec(preheatRect[3].x0, preheatRect[3].y0, newTemp[0], 3, RIGHT);
        break;
        
        default: break;
    }
}
bool comparePreheat(void){
    if(oldTemp[0] == newTemp[0] && oldTemp[1] == newTemp[1])    return false;
    else return true;
}
void menuCallBackSetPreHeat(void){
    KEY_VALUES  key_num = menuKeyGetValue();
    switch(key_num){
        case KEY_ICON_0:
            newTemp[preheatIndex] -= item_degree[changeIndex];
            changeNum = true;
        break;

        case KEY_ICON_3:
            newTemp[preheatIndex] += item_degree[changeIndex];
            changeNum = true;
        break;
            
        case KEY_ICON_4:
            preheatIndex = (preheatIndex+1) % 2;
            setPreheatItems.items[key_num] = itemTool[preheatIndex];
            menuDrawItem(&setPreheatItems.items[key_num], key_num);
            changeNum = true;
        break;

        case KEY_ICON_5:
            changeIndex = (changeIndex+1) % 3;
            setPreheatItems.items[key_num] = itemDegree[changeIndex];
            menuDrawItem(&setPreheatItems.items[key_num], key_num);
        break;

        case KEY_ICON_6:
            if(comparePreheat){
                setCurrentPreheat(MaterialNum, newTemp);    // 设置最新预设温度
                getCurrentPreheat(MaterialNum, oldTemp);    // 获取最新预设温度
                storeCmd("M500");   // 保存  
            }
        break;

        case KEY_ICON_7:
            infoMenu.cur--;
        break;

        default:break;
    }
    if(changeNum){
        changeNum = false;
        showPreheatTemp();
    }
}
void menuSetPreheat(void){
    getCurrentPreheat(MaterialNum, oldTemp);
    getCurrentPreheat(MaterialNum, newTemp);
    preheatIndex = 1;   // 进入界面一定是喷嘴最先
    changeNum = false;
    setPreheatItems.items[KEY_ICON_4] = itemTool[preheatIndex];
    menuDrawPage(&setPreheatItems);
    menuDrawTitle(MaterialPreset[MaterialNum]);
    
    showPreheatTemp();
    menuSetFrontCallBack(menuCallBackSetPreHeat);
}

/************************************选择界面**************************************************/
void menuCallBackPreHeatSet(void){
    KEY_VALUES  key_num = menuKeyGetValue();
    switch(key_num){
        case KEY_ICON_0:
            MaterialNum = THE_PREHEAT1;
            infoMenu.menu[++infoMenu.cur] = menuSetPreheat;
        break;

        case KEY_ICON_1:
            MaterialNum = THE_PREHEAT2;
            infoMenu.menu[++infoMenu.cur] = menuSetPreheat;
        break;

        case KEY_ICON_2:
            MaterialNum = THE_PREHEAT3;
            infoMenu.menu[++infoMenu.cur] = menuSetPreheat;
        break;
        
        case KEY_ICON_7:
            infoMenu.cur--;
        break;

        default:break;
    }
}

void menuPreheatSet(void){
    menuDrawPage(&preheatSetItems);
    menuDrawItemLabel(PREHEAT_1_LABEL, 0);
    menuDrawItemLabel(PREHEAT_2_LABEL, 1);
    menuDrawItemLabel(PREHEAT_3_LABEL, 2);
    drawTemperatureInfo();
    menuSetFrontCallBack(menuCallBackPreHeatSet);
}

#endif
