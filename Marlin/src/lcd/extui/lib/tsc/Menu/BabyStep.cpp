#include "../TSC_Menu.h"
#include "../../../../../feature/babystep.h"
#include "../../../../../module/settings.h"


//1 title, ITEM_PER_PAGE items(icon+label) 
MENUITEMS babyStepItems = {
//title
  LABEL_BABYSTEP,
//icon                        label
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

#define ITEM_BABYSTEP_UNIT_NUM 3
const ITEM itemBabyStepUnit[ITEM_BABYSTEP_UNIT_NUM] = {
// icon                       label
  {ICON_001_MM,               LABEL_001_MM},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_1_MM,                 LABEL_1_MM},
};
const float item_babystep_unit[ITEM_BABYSTEP_UNIT_NUM] = {0.01f, 0.1f, 1};

static ELEMENTS elementsUnit;
// static float tempData = 0.0f;
float old_baby_step_value = 0.0f;
TERN_(LEVELING_OFFSET, float babystep_value = 0.0f);      // 存放babystep的值，只有合并入调平数据才会清零
bool autoCloseBabysetp = false;   // 是否自动关闭babystep界面
// float old_z_offset = 0.0f;

static void initElements(uint8_t position)
{
  elementsUnit.totaled = ITEM_BABYSTEP_UNIT_NUM;
  elementsUnit.list = itemBabyStepUnit;
  elementsUnit.ele  = item_babystep_unit;

  for(uint8_t i=0; i<elementsUnit.totaled; i++)
  {
    if(memcmp(&elementsUnit.list[i], &babyStepItems.items[position], sizeof(ITEM)) == 0)
    {
      elementsUnit.cur = i;
      break;
    }
  }
}

float getBabyStepZAxisTotalMM(){
  return babystep.axis_total[BS_TOTAL_IND(WHO_AXIS)] * planner.steps_to_mm[WHO_AXIS];
}

static int WhatIsBabyStepMM(float mm){
  float oldmm = getBabyStepZAxisTotalMM();
  // 超越值
  if((oldmm>=BABYSTEP_MAX_HIGH && mm>0) || (oldmm<=-BABYSTEP_MAX_HIGH && mm<0))
  {
    return 0;
  }
  // 快到顶
  if((oldmm+mm) >= BABYSTEP_MAX_HIGH)
  {
    return 1;
  }
  // 快到底
  if((oldmm+mm) <= -BABYSTEP_MAX_HIGH)
  {
    return 2;
  }
  // 常规
  return 3;
}

void setBabyStepZAxisIncMM(float mm){
  float oldbabystep = getBabyStepZAxisTotalMM();
  float changed_num = 0.0f;
  #if defined(LEVELING_OFFSET)
    float oldoffset = getLevelingOffset();
  #else
    float oldoffset = getCurrentOffset();
  #endif

  switch (WhatIsBabyStepMM(mm))
  {
    case 1:
      changed_num = BABYSTEP_MAX_HIGH - oldbabystep;    // 值快到顶了
      break;

    case 2:
      changed_num = (-BABYSTEP_MAX_HIGH) - oldbabystep;   // 值快到底了
      break;

    case 3:
      changed_num = mm;              // 值正常
      break;
    
    default:                                    // 其它
      break;
  }
  babystep.add_mm(WHO_AXIS, changed_num);
 #if 0//defined(LEVELING_OFFSET)
  setLevelingOffset(oldoffset + (getBabyStepZAxisTotalMM()-oldbabystep));
 #else
  setCurrentOffset(oldoffset+changed_num);
 #endif
}

void showBabyStepText(void){
  GUI_ClearPrect(&exhibitRect);
  GUI_DispString(CENTER_X - 10*BYTE_WIDTH, CENTER_Y-BYTE_HEIGHT, (const uint8_t *)"BabyStep:");
  GUI_DispString(CENTER_X - 10*BYTE_WIDTH, CENTER_Y+BYTE_HEIGHT, (const uint8_t *)"Z Offset:");
}
void showBabyStep(void)
{
 #if 0
  GUI_DispFloat(CENTER_X - 5*BYTE_WIDTH/2, CENTER_Y, getBabyStepZAxisTotalMM(), 3, 2, RIGHT);
 #else
  GUI_ClearRect(CENTER_X, CENTER_Y-BYTE_HEIGHT, 3*ICON_WIDTH+2*SPACE_X+START_X, CENTER_Y+1*BYTE_HEIGHT-1);
  GUI_ClearRect(CENTER_X, CENTER_Y+BYTE_HEIGHT, 3*ICON_WIDTH+2*SPACE_X+START_X, CENTER_Y+2*BYTE_HEIGHT-1);
  GUI_DispFloat(CENTER_X, CENTER_Y-BYTE_HEIGHT, getBabyStepZAxisTotalMM(), 3, 2, RIGHT);
  GUI_DispFloat(CENTER_X, CENTER_Y+BYTE_HEIGHT, TERN(LEVELING_OFFSET, getLevelingOffset(), getCurrentOffset()), 3, 2, RIGHT);
 #endif
}

void menuCallBackBabyStep(void)
{
  static float baby_step_value = 0.0f;
  KEY_VALUES key_num = menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0:
      setBabyStepZAxisIncMM(-elementsUnit.ele[elementsUnit.cur]);
      break;
      
    case KEY_ICON_3:
      setBabyStepZAxisIncMM(elementsUnit.ele[elementsUnit.cur]);
      break;

    case KEY_ICON_4:
       #if 1
        #if ENABLED(LEVELING_OFFSET)
          babystep_value = getBabyStepZAxisTotalMM();     // 获取当前babystep的值
          old_baby_step_value = babystep_value;
          if(settings.save())
            popupReminder_B(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_SYCHRONZIED_VALUE));  // 保存成功，提示并自动退出
        #else
          if(settings.save()){
            popupReminder_B(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_SUCCESS));  // 保存成功，提示并自动退出
          }
          else{
            popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_FAILED), false);
          }
        #endif
       #elif ENABLED(AUTO_BED_LEVELING_BILINEAR) && ENABLED(LEVELING_OFFSET)
        TERN_(LEVELING_OFFSET, setLevelingOffset(getBabyStepZAxisTotalMM() - old_baby_step_value);) // 将当前BabyStep的值赋给z_offset
        old_baby_step_value = getBabyStepZAxisTotalMM();
        popupReminder_B(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_SYCHRONZIED_VALUE));  // 提示并自动退出
       #else
        if(old_z_offset != getLevelingOffset()){
          if(settings.save()){
            old_z_offset = getLevelingOffset();
            popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_SUCCESS), true);
          }else{
            popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_FAILED), false);
          }
        }
        else{
          popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_FAILED), false);
        }
      //  #else
      //   settings.save();                              // 保存，注意保存的是z_offset的值，而不是BabyStep的值，BabyStep每次复位都会被清零，防止干扰
       #endif
      break;

    case KEY_ICON_5:
      elementsUnit.cur = (elementsUnit.cur + 1) % elementsUnit.totaled;
      babyStepItems.items[key_num] = elementsUnit.list[elementsUnit.cur];
      menuDrawItem(&babyStepItems.items[key_num], key_num);
      break;

    case KEY_ICON_6:
      setBabyStepZAxisIncMM(-baby_step_value);
      break;

    case KEY_ICON_7:
      infoMenu.cur--;
      break;

    default: break;
  }
  if(baby_step_value != getBabyStepZAxisTotalMM())
  {
    baby_step_value = getBabyStepZAxisTotalMM();
    showBabyStep();
  }
  if(autoCloseBabysetp){
    autoCloseBabysetp = false;
    infoMenu.cur--;
  }
}

void menuBabyStep()
{
  initElements(KEY_ICON_5);
  menuDrawPage(&babyStepItems);
  showBabyStepText();
  showBabyStep();
  menuSetFrontCallBack(menuCallBackBabyStep);
}
