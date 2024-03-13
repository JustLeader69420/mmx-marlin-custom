#include "../TSC_Menu.h"
#include "../../../../../module/settings.h"
#include "../../../../../module/probe.h"
#include "../../../../../feature/babystep.h"
#include "../../../../../gcode/gcode.h"
// #include <math.h>

#if ENABLED(LEVELING_OFFSET)

//1 titl, ITEM_PER_PAGE item
MENUITEMS LevelingOffsetItems = {
// title
LABEL_PROBE_OFFSET,
// icon                        label
 {{ICON_DOWN,                 LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_UP,                   LABEL_INC},
  {ICON_EEPROM_SAVE,          LABEL_EEPROM_SAVE},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_NORMAL_SPEED,         LABEL_VALUE_ZERO},
  {ICON_BACK,                 LABEL_BACK},}
};

typedef struct
{
  const ITEM *list;
  const float *ele;
  uint8_t cur;
  uint8_t totaled;
}ELEMENTS;

#define ITEM_LEVELING_OFFSET_UNIT_NUM 3
const ITEM itemProbeOffsetUnit[ITEM_LEVELING_OFFSET_UNIT_NUM] = {
// icon                       label
  {ICON_001_MM,               LABEL_001_MM},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_1_MM,                 LABEL_1_MM},
};
const float item_LevelingOffset_unit[ITEM_LEVELING_OFFSET_UNIT_NUM] = {0.01f, 0.1f, 1};

static ELEMENTS elementsUnit;
float LevelingOffset = LODEVA;    // 当前值,默认0.6
float oldLevelingOffset = 0.0f; // 上一个值

static void initElements(uint8_t position)
{
  elementsUnit.totaled = ITEM_LEVELING_OFFSET_UNIT_NUM;
  elementsUnit.list = itemProbeOffsetUnit;
  elementsUnit.ele  = item_LevelingOffset_unit;

  for(uint8_t i=0; i<elementsUnit.totaled; i++)
  {
    if(memcmp(&elementsUnit.list[i], &LevelingOffsetItems.items[position], sizeof(ITEM)) == 0)
    {
      elementsUnit.cur = i;
      break;
    }
  }
}

/* 获取调平补偿 */
float getLevelingOffset(void)
{  
  return LevelingOffset;
}
/* 设置调平补偿 */
void setLevelingOffset(float offset)
{ 
 #if 1
  LevelingOffset += offset;
  if(LevelingOffset>= (BABYSTEP_MAX_HIGH*2))  LevelingOffset =  (BABYSTEP_MAX_HIGH*2);
  if(LevelingOffset<=(-BABYSTEP_MAX_HIGH*2))  LevelingOffset = (-BABYSTEP_MAX_HIGH*2);
 #else
  LevelingOffset = offset;
 #endif
}
// 置零
void resetLevelingOffset(void){
  LevelingOffset = 0.0f;
}
// 将数据保存到存储器
bool saveOffset(){
  float newLevelingOffset = LevelingOffset - oldLevelingOffset; // 求变化量
  if (leveling_is_valid() && ((int)(newLevelingOffset*100) != 0)) {   // 判断调平数据有效 且 变化不小于0.01
    LOOP_L_N(py, GRID_MAX_POINTS_Y) {
      LOOP_L_N(px, GRID_MAX_POINTS_X) {
        z_values[px][py] += newLevelingOffset;
      }
    } // 循环将数组内的数据和并。
    
    oldLevelingOffset = LevelingOffset; // 告知已将偏移值合并到调平数据内
    return settings.save(); //判断是否保存成功
  }
  return false; // 没触发保存，保存失败
}

void showLevelingOffset(void)
{
  GUI_DispFloat(CENTER_X - 5*BYTE_WIDTH/2, CENTER_Y, getLevelingOffset(), 3, 2, RIGHT);
}
void LevelingOffsetReDraw(void)
{
  GUI_DispFloat(CENTER_X - 5*BYTE_WIDTH/2, CENTER_Y, getLevelingOffset(), 3, 2, RIGHT);
}


void menuCallBackLevelingOffset(void)
{
  bool leveling_offset_value_change = false;    // 值是否发生变化
  float ts_value = 0.0f;   // 临时存放数据
  KEY_VALUES key_num =  menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0:
      setLevelingOffset(- elementsUnit.ele[elementsUnit.cur]);
      leveling_offset_value_change = true;
      break;
    case KEY_ICON_3:
      setLevelingOffset(elementsUnit.ele[elementsUnit.cur]);
      leveling_offset_value_change = true;
      break;
    case KEY_ICON_4:
      if(saveOffset())
        popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_SUCCESS), true);
      else
        popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_FAILED), false);
      
      break;
    case KEY_ICON_5:
      elementsUnit.cur = (elementsUnit.cur + 1) % elementsUnit.totaled;
      LevelingOffsetItems.items[key_num] = elementsUnit.list[elementsUnit.cur];
      menuDrawItem(&LevelingOffsetItems.items[key_num], key_num);
      break;
    case KEY_ICON_6:
      resetLevelingOffset(); // 置零
      leveling_offset_value_change = true;
      break;
    case KEY_ICON_7:
      infoMenu.cur--;
      break;
    default :
      break;
  }
  if(leveling_offset_value_change)  // 值发生变化，显示刷新
  {
    leveling_offset_value_change = false;
    LevelingOffsetReDraw();
  }
}

void menuLevelingOffset()
{
  // oldLevelingOffset = LevelingOffset;
  initElements(KEY_ICON_5);
  menuDrawPage(&LevelingOffsetItems);
  showLevelingOffset();
  menuSetFrontCallBack(menuCallBackLevelingOffset);
}


void menuCallBackSetLevelingOffset()
{
  bool leveling_offset_value_change = false;    // 值是否发生变化
  float ts_value = 0.0f;   // 临时存放数据
  KEY_VALUES key_num =  menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0:
      setBabyStepZAxisIncMM(-elementsUnit.ele[elementsUnit.cur]);
      setLevelingOffset(- elementsUnit.ele[elementsUnit.cur]);
      leveling_offset_value_change = true;
      break;
    case KEY_ICON_3:
      setBabyStepZAxisIncMM(elementsUnit.ele[elementsUnit.cur]);
      setLevelingOffset(elementsUnit.ele[elementsUnit.cur]);
      leveling_offset_value_change = true;
      break;
    case KEY_ICON_4:
      if(saveOffset())
        popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_SUCCESS), true);
      else
        popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_FAILED), false);
      
      break;
    case KEY_ICON_5:
      elementsUnit.cur = (elementsUnit.cur + 1) % elementsUnit.totaled;
      LevelingOffsetItems.items[key_num] = elementsUnit.list[elementsUnit.cur];
      menuDrawItem(&LevelingOffsetItems.items[key_num], key_num);
      break;
    case KEY_ICON_6:
      resetLevelingOffset(); // 置零
      leveling_offset_value_change = true;
      break;
    case KEY_ICON_7:
      mustStoreCmd("M420 S0\n");
      mustStoreCmd("G28\n");
      infoMenu.cur--;
      break;
    default :
      break;
  }
  if(leveling_offset_value_change)  // 值发生变化，显示刷新
  {
    leveling_offset_value_change = false;
    LevelingOffsetReDraw();
  }
}
void menuSetLevelingOffset()
{
  initElements(KEY_ICON_5);
  menuDrawPage(&LevelingOffsetItems);
  showLevelingOffset();
  menuSetFrontCallBack(menuCallBackSetLevelingOffset);
}

#endif


#define THE_FLOAT_MAGNIFICATION 1000
#define SET_LEVELING_VALUE 0.05f
static bed_mesh_t old_z_values;
static bool LO_flag = false;            // LevelingOffset flag
static float the_babystep = 0.0f;
const static float Lx_min=probe.min_x(), Ly_min=probe.min_y(),Lx_max=probe.max_x(), Ly_max=probe.max_y();
const static float LSridSpacing_x = (Lx_max-Lx_min)/(GRID_MAX_POINTS_X-1), LSridSpacing_y = (Ly_max-Ly_min)/(GRID_MAX_POINTS_Y-1);
static uint8_t x = 0xff, y = 0xff;    //用于确定点击的坐标，0xff为没选中。
static uint8_t x2 = 0xff, y2 = 0xff;  //用于存放上一次选中的坐标,0xff为没选中// static xy_pos_t theProbePos;
// this is the button icon info
// LEVELINGITEMS levelingSetItems = 
uint8_t *levelingSetButtonItems[4] = 
{
  CHAR_INCREASE,
  CHAR_LOWER,
  CHAR_SAVE,
  CHAR_SBACK,
};
// this is the leveling value storage
static uint8_t levelingSetCubeItems[ITEM_CUBE_NUM-4][8] = 
{
};
void moveToProbePos(uint8_t x,uint8_t y)
{
  xyz_pos_t npos = {Lx_min+LSridSpacing_x*x, Ly_min+LSridSpacing_y*y, 0};
  // mustStoreCmd("G0Z5\n");
  planner.synchronize();
  do_blocking_move_to_z(5, 10);
  planner.synchronize();
  do_blocking_move_to(npos, 80);
  planner.synchronize();
}
void setLO_flag(bool _flag){
  LO_flag = _flag;
}
bool ifSaveEEPROM(void){
  bool save_flag = false;
  uint8_t x=0,y=0;
  float d_value = 0.0f;
  // 检测是否修改了某项数据
  for(y=0; y<GRID_MAX_POINTS_Y; y++){
    for(x=0; x<GRID_MAX_POINTS_X; x++){
      d_value = (z_values[x][y] - old_z_values[x][y]);
      if(d_value>0.0005f || d_value<-0.0005f){
        save_flag = true;
        break;
      }
    }
    if(save_flag) break;
  }

  if (save_flag) {return settings.save();}
  else {return false;}
}

/*
 * the float value convert to string (now it can only support printf two decimal places)
 * @parm value: the float value
 * @parm num:   How many decimal places do you want to keep
 * @parm string:Conversion results
 */
void floatToString(float value, int num, char* string){
  int magnification = 100;// (int)pow(10, num);
  int temp = 0;
  // is zero?
  if(value<=0.00001f && value>=-0.00001f){
    string[0] = '0';
    string[1] = 0;
  }
  else{
    temp = value*magnification;
    if(temp>0)
    {
      sprintf(string, "%d.%02d", temp/magnification, temp%magnification);
    }
    else{
      sprintf(string, "-%d.%02d", (-temp)/magnification, (-temp)%magnification);
    }
  }
}
static void needGoHomeBeforeZoffset(void){
  char str[32] = "Homing...\nPlease wait...";
  if (LO_flag) {
    setLO_flag(false);
    GUI_Clear(BLACK);
    // GUI_DispStringInRect(0, 0, LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL, /*textSelect(LABEL_HOME)*/"homing...");
    popupDrawPage(NULL , textSelect(LABEL_TIPS), (uint8_t *)str, NULL, NULL);
    key_lock = true;
    gcode.process_subcommands_now("G28");
    gcode.process_subcommands_now("M420 S1");
    // t=0;
    // while(queue.length>0){
    wait_for_user_response(10);
    planner.synchronize();
    //   if(t++>600){  // 60s
    //     infoMenu.cur--;
    //     return;
    //   }
    // }
    // infoMenu.cur--;
    key_lock = false;
  }
}
void menuCallBackSetLevelingValue()
{
  uint16_t key_num =  menuKeyGetLevelingValue();
  char str[16];
  uint8_t isClick = 0;
  int the_value = 0;
  float the_babystep = getBabyStepZAxisTotalMM();
  static bool unlock = true;
  if(unlock){
    unlock = false;
    switch(key_num)
    {
      case KEY_CUBE_INCREASE:
        // babystep.add_mm(Z_AXIS, 0.1f);
        if(x<GRID_MAX_POINTS_X && y<GRID_MAX_POINTS_Y){
          // old_z_values[x][y] += SET_LEVELING_VALUE;
          // the_value = (old_z_values[x][y]+1.0f/THE_FLOAT_MAGNIFICATION/100)*THE_FLOAT_MAGNIFICATION;
          // // the_value += (SET_LEVELING_VALUE*THE_FLOAT_MAGNIFICATION);

          // if(old_z_values[x][y]<=0.00001 && old_z_values[x][y]>=-0.00001){
          //   levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][0] = '0';
          //   levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][1] = 0;
          // }else if(old_z_values[x][y]<0){
          //   sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "-%d.%d%d", (-the_value)/1000, (-the_value/100)%10, (-the_value/10)%10);
          // }else{
          //   sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "%d.%d%d", the_value/1000, (the_value/100)%10, (the_value/10)%10);
          // }
          // // sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "%f", old_z_values);
          z_values[x][y] += SET_LEVELING_VALUE;
          //the_babystep += SET_LEVELING_VALUE;
          // setBabyStepZAxisIncMM(SET_LEVELING_VALUE);
          babystep.add_mm(WHO_AXIS, SET_LEVELING_VALUE);
          floatToString(z_values[x][y], 2, (char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x]);
          menuDrawCubeItem_check(levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], y*GRID_MAX_POINTS_X+4+x);
        }
        break;
      case KEY_CUBE_LOWER:
        // babystep.add_mm(Z_AXIS, -0.1f);
        if(x<GRID_MAX_POINTS_X && y<GRID_MAX_POINTS_Y){
          // old_z_values[x][y] -= 0.1f;
          // the_value = old_z_values[x][y]*100;
          // if(the_value<0){
          //   sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "-%d.%d", (-the_value)/100, (-the_value)%100);
          // }else if(the_value == 0){
          //   levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][0] = '0';
          //   levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][1] = 0;
          // }else{
          //   sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "%d.%d", the_value/100, the_value%100);
          // }
          
          // the_value = (old_z_values[x][y]-1.0f/THE_FLOAT_MAGNIFICATION/100)*THE_FLOAT_MAGNIFICATION;
          // // the_value -= (SET_LEVELING_VALUE*THE_FLOAT_MAGNIFICATION);

          // if(old_z_values[x][y]<=0.00001 && old_z_values[x][y]>=-0.00001){
          //   levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][0] = '0';
          //   levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][1] = 0;
          // }else if(the_value<0){
          //   sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "-%d.%d%d", (-the_value)/1000, (-the_value/100)%10, (-the_value/10)%10);
          // }else{
          //   sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "%d.%d%d", the_value/1000, (the_value/100)%10, (the_value/10)%10);
          // }
          z_values[x][y] -= SET_LEVELING_VALUE;
          //the_babystep -= SET_LEVELING_VALUE;
          // setBabyStepZAxisIncMM(-SET_LEVELING_VALUE);
          babystep.add_mm(WHO_AXIS, -SET_LEVELING_VALUE);
          floatToString(z_values[x][y], 2, (char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x]);
          menuDrawCubeItem_check(levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], y*GRID_MAX_POINTS_X+4+x);
        }
        break;
      case KEY_CUBE_SAVE:
        if (ifSaveEEPROM()){
          popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_SUCCESS), true);
        }else{
          popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_FAILED), false);
        }
        break;
      case KEY_CUBE_BACK:
        // storeCmd("M420 S0\n");
        // storeCmd("G28 Z\n");
        // GUI_Clear(BLACK);
        // GUI_DispStringInRect(0, 0, LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL, textSelect(LABEL_HOME));
        // key_lock = true;
        // gcode.process_subcommands_now("G28Z");
        gcode.process_subcommands_now("M420 S0\n");
        // // t=0;
        // // while(queue.length>0){
        //   ExtUI::delay_ms(100);
        //   planner.synchronize();
        // //   if(t++>600){  // 60s
        // //     infoMenu.cur--;
        // //     return;
        // //   }
        // // }
        // key_lock = false;
        infoMenu.cur--;
        break;

    #if 0
      #if GRID_MAX_POINTS_Y>0
        #if GRID_MAX_POINTS_X>0
          case KEY_0_0:
            if(0==y && 0==x)    { isClick = 2; }
            else{ y2 = 0; x2 = 0; isClick = 1; }
            break;
        #endif
        #if GRID_MAX_POINTS_X>1
          case KEY_0_1:
            if(0==y && 1==x)    { isClick = 2; }
            else{ y2 = 0; x2 = 1; isClick = 1; }
            break;
        #endif
        #if GRID_MAX_POINTS_X>2
          case KEY_0_2:
            if(0==y && 2==x)    { isClick = 2; }
            else{ y2 = 0; x2 = 2; isClick = 1; }
            break;
        #endif
        #if GRID_MAX_POINTS_X>3
          case KEY_0_3:
            if(0==y && 3==x)    { isClick = 2; }
            else{ y2 = 0; x2 = 3; isClick = 1; }
            break;
        #endif
        #if GRID_MAX_POINTS_X>4
          case KEY_0_4:
            if(0==y && 4==x)    { isClick = 2; }
            else{ y2 = 0; x2 = 4; isClick = 1; }
            break;
        #endif
        #if GRID_MAX_POINTS_X>5
          case KEY_0_5:
            if(0==y && 5==x)    { isClick = 2; }
            else{ y2 = 0; x2 = 5; isClick = 1; }
            break;
        #endif
      #endif
      #if GRID_MAX_POINTS_Y>1
        #if GRID_MAX_POINTS_X>0
          case KEY_1_0:
            break;
        #endif
        #if GRID_MAX_POINTS_X>1
          case KEY_1_1:
            break;
        #endif
        #if GRID_MAX_POINTS_X>2
          case KEY_1_2:
            break;
        #endif
        #if GRID_MAX_POINTS_X>3
          case KEY_1_3:
            break;
        #endif
        #if GRID_MAX_POINTS_X>4
          case KEY_1_4:
            break;
        #endif
        #if GRID_MAX_POINTS_X>5
          case KEY_1_5:
            break;
        #endif
      #endif
      #if GRID_MAX_POINTS_Y>2
        #if GRID_MAX_POINTS_X>0
          case KEY_2_0:
            break;
        #endif
        #if GRID_MAX_POINTS_X>1
          case KEY_2_1:
            break;
        #endif
        #if GRID_MAX_POINTS_X>2
          case KEY_2_2:
            break;
        #endif
        #if GRID_MAX_POINTS_X>3
          case KEY_2_3:
            break;
        #endif
        #if GRID_MAX_POINTS_X>4
          case KEY_2_4:
            break;
        #endif
        #if GRID_MAX_POINTS_X>5
          case KEY_2_5:
            break;
        #endif
      #endif
      #if GRID_MAX_POINTS_Y>3
        #if GRID_MAX_POINTS_X>0
          case KEY_3_0:
            break;
        #endif
        #if GRID_MAX_POINTS_X>1
          case KEY_3_1:
            break;
        #endif
        #if GRID_MAX_POINTS_X>2
          case KEY_3_2:
            break;
        #endif
        #if GRID_MAX_POINTS_X>3
          case KEY_3_3:
            break;
        #endif
        #if GRID_MAX_POINTS_X>4
          case KEY_3_4:
            break;
        #endif
        #if GRID_MAX_POINTS_X>5
          case KEY_3_5:
            break;
        #endif
      #endif
      #if GRID_MAX_POINTS_Y>4
        #if GRID_MAX_POINTS_X>0
          case KEY_4_0:
            break;
        #endif
        #if GRID_MAX_POINTS_X>1
          case KEY_4_1:
            break;
        #endif
        #if GRID_MAX_POINTS_X>2
          case KEY_4_2:
            break;
        #endif
        #if GRID_MAX_POINTS_X>3
          case KEY_4_3:
            break;
        #endif
        #if GRID_MAX_POINTS_X>4
          case KEY_4_4:
            break;
        #endif
        #if GRID_MAX_POINTS_X>5
          case KEY_4_5:
            break;
        #endif
      #endif
      #if GRID_MAX_POINTS_Y>5
        #if GRID_MAX_POINTS_X>0
          case KEY_5_0:
            break;
        #endif
        #if GRID_MAX_POINTS_X>1
          case KEY_5_1:
            break;
        #endif
        #if GRID_MAX_POINTS_X>2
          case KEY_5_2:
            break;
        #endif
        #if GRID_MAX_POINTS_X>3
          case KEY_5_3:
            break;
        #endif
        #if GRID_MAX_POINTS_X>4
          case KEY_5_4:
            break;
        #endif
        #if GRID_MAX_POINTS_X>5
          case KEY_5_5:
            break;
        #endif
      #endif
    #elif 1
      default:
        if (key_num <= ITEM_CUBE_NUM) {
          if (key_num == 4){x = 0; y = 0;isClick = 1;}
          else{
            x = (key_num-4)%GRID_MAX_POINTS_X;
            y = (key_num-4)/GRID_MAX_POINTS_X;
            isClick = 1;
          }
        }
        break;
    #endif
    }
    if(1 == isClick){
      isClick = 0;
      // if(x<GRID_MAX_POINTS_X && y<GRID_MAX_POINTS_Y)
      // setBabyStepZAxisIncMM(-the_babystep);
      babystep.add_mm(WHO_AXIS, -the_babystep);

      // delay(50);
      while(babystep.has_steps()){idle();}  // 等待babystep走完
      // Click uncheck a second time
      if(x==x2 && y==y2){
        menuDrawCubeItem(levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], key_num);
        x = x2 = 0xff; y = y2 = 0xff;
      }
      else{
        menuDrawCubeItem_check(levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], key_num);
        // Uncheck the status of the last check
        if((x2!=0xff)&&(y2!=0xff)){
          menuDrawCubeItem(levelingSetCubeItems[y2*GRID_MAX_POINTS_X+x2], y2*GRID_MAX_POINTS_X+x2+4);
        }
        moveToProbePos(x,y);
        x2 = x; y2 = y;
      }
      // delay(1000);
    }
    else if(2 == isClick){
      menuDrawCubeItem(levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], key_num);
      x = 255; y = 255;
    }
    unlock = true;
  }
}
void menuSetLevelingValue()
{
  // int the_value = 0;
  uint32_t t = 0;
  if(leveling_is_valid()){
    for(y=0; y<GRID_MAX_POINTS_Y; y++){
      for(x=0; x<GRID_MAX_POINTS_X; x++){
        // the_value = 0;
        // old_z_values[x][y] = 0.0f;
        // if(leveling_is_valid()){
        //   the_value = z_values[x][y] * 100;
        //   old_z_values[x][y] = z_values[x][y];
        // }

        // if(the_value == 0){
        //   // sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "0");
        //   levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][0] = '0';
        //   levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][1] = 0;
        // }
        // else{
        //   if (the_value>0)
        //   {
        //     sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "%d.%d", the_value/100, the_value%100);
        //   }
        //   else if (the_value<0)
        //   {
        //     sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "-%d.%d", (-the_value)/100, (-the_value)%100);
        //   }
        // }
        old_z_values[x][y] = z_values[x][y];
        floatToString(z_values[x][y], 2, (char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x]);
      }
    }
  }
  else{
    GUI_Clear(BLACK);
    key_lock = true;
    GUI_DispStringInRect(0, 0, LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL, (const uint8_t*)"No ABL");
    wait_for_user_response(1000);
    setLO_flag(false);
    key_lock = false;
    infoMenu.cur--;
    return;
  }
  needGoHomeBeforeZoffset();
  x = 0xff; y = 0xff;
  x2 = 0xff; y2 = 0xff;
  GUI_Clear(BLACK);
  menuDrawCubePage(levelingSetButtonItems, levelingSetCubeItems);
  menuSetFrontCallBack(menuCallBackSetLevelingValue);
}
