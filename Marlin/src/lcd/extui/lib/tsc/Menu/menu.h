#ifndef _MENU_H_
#define _MENU_H_

#include "stdint.h"
#include "stdbool.h"
#include "../GUI.h"


#define IDLE_TOUCH	0xFFFF
typedef enum
{
  KEY_ICON_0 = 0,
  KEY_ICON_1,
  KEY_ICON_2,
  KEY_ICON_3,
  KEY_ICON_4,
  KEY_ICON_5,
  KEY_ICON_6,
  KEY_ICON_7,
  KEY_ICON_8,
  KEY_LABEL_0,
  KEY_LABEL_1,
  KEY_LABEL_2,
  KEY_LABEL_3,
  KEY_LABEL_4,
  KEY_LABEL_5,
  KEY_LABEL_6,
  KEY_LABEL_7,
  KEY_LABEL_8,
  KEY_LOCKED = IDLE_TOUCH-1,
  KEY_IDLE = IDLE_TOUCH,
}KEY_VALUES;

#ifdef AUTO_BED_LEVELING_BILINEAR
  enum
  {
    KEY_CUBE_INCREASE = 0,
    KEY_CUBE_LOWER,
    KEY_CUBE_SAVE,
    KEY_CUBE_BACK,

    #if GRID_MAX_POINTS_Y>0
      #if GRID_MAX_POINTS_X>0
        KEY_0_0,
      #endif
      #if GRID_MAX_POINTS_X>1
        KEY_0_1,
      #endif
      #if GRID_MAX_POINTS_X>2
        KEY_0_2,
      #endif
      #if GRID_MAX_POINTS_X>3
        KEY_0_3,
      #endif
      #if GRID_MAX_POINTS_X>4
        KEY_0_4,
      #endif
      #if GRID_MAX_POINTS_X>5
        KEY_0_5,
      #endif
    #endif
    #if GRID_MAX_POINTS_Y>1
      #if GRID_MAX_POINTS_X>0
        KEY_1_0,
      #endif
      #if GRID_MAX_POINTS_X>1
        KEY_1_1,
      #endif
      #if GRID_MAX_POINTS_X>2
        KEY_1_2,
      #endif
      #if GRID_MAX_POINTS_X>3
        KEY_1_3,
      #endif
      #if GRID_MAX_POINTS_X>4
        KEY_1_4,
      #endif
      #if GRID_MAX_POINTS_X>5
        KEY_1_5,
      #endif
    #endif
    #if GRID_MAX_POINTS_Y>2
      #if GRID_MAX_POINTS_X>0
        KEY_2_0,
      #endif
      #if GRID_MAX_POINTS_X>1
        KEY_2_1,
      #endif
      #if GRID_MAX_POINTS_X>2
        KEY_2_2,
      #endif
      #if GRID_MAX_POINTS_X>3
        KEY_2_3,
      #endif
      #if GRID_MAX_POINTS_X>4
        KEY_2_4,
      #endif
      #if GRID_MAX_POINTS_X>5
        KEY_2_5,
      #endif
    #endif
    #if GRID_MAX_POINTS_Y>3
      #if GRID_MAX_POINTS_X>0
        KEY_3_0,
      #endif
      #if GRID_MAX_POINTS_X>1
        KEY_3_1,
      #endif
      #if GRID_MAX_POINTS_X>2
        KEY_3_2,
      #endif
      #if GRID_MAX_POINTS_X>3
        KEY_3_3,
      #endif
      #if GRID_MAX_POINTS_X>4
        KEY_3_4,
      #endif
      #if GRID_MAX_POINTS_X>5
        KEY_3_5,
      #endif
    #endif
    #if GRID_MAX_POINTS_Y>4
      #if GRID_MAX_POINTS_X>0
        KEY_4_0,
      #endif
      #if GRID_MAX_POINTS_X>1
        KEY_4_1,
      #endif
      #if GRID_MAX_POINTS_X>2
        KEY_4_2,
      #endif
      #if GRID_MAX_POINTS_X>3
        KEY_4_3,
      #endif
      #if GRID_MAX_POINTS_X>4
        KEY_4_4,
      #endif
      #if GRID_MAX_POINTS_X>5
        KEY_4_5,
      #endif
    #endif
    #if GRID_MAX_POINTS_Y>5
      #if GRID_MAX_POINTS_X>0
        KEY_5_0,
      #endif
      #if GRID_MAX_POINTS_X>1
        KEY_5_1,
      #endif
      #if GRID_MAX_POINTS_X>2
        KEY_5_2,
      #endif
      #if GRID_MAX_POINTS_X>3
        KEY_5_3,
      #endif
      #if GRID_MAX_POINTS_X>4
        KEY_5_4,
      #endif
      #if GRID_MAX_POINTS_X>5
        KEY_5_5,
      #endif
    #endif
    KEY_6_6,
    KEY_X_X = IDLE_TOUCH,
  }KEY_CUBE;
#endif

#define ITEM_PER_PAGE  8
#define ITEM_CUBE_NUM GRID_MAX_POINTS_X*GRID_MAX_POINTS_Y+4
#define PAGE_BUTTON    3
#define PAGE_THUMBNAIL 6

typedef union
{
  uint32_t index;    // language index, address = textSelect(index);
  uint8_t *address;
}LABEL;

typedef struct
{
  uint16_t icon;
  LABEL label;
}ITEM;

typedef enum
{
  LIST_LABEL = 0,
  LIST_BUTTON,
}LISTITEM_TYPE;
typedef enum{
  ICON_FOLDER = 0,
  ICON_FILE,
  ICON_NO = 0xff,
}ICON_TYPE;

typedef struct
{
  LISTITEM_TYPE type;
  uint8_t* icon;
  uint8_t* label;
}LISTITEM;
typedef struct
{
  LABEL title;
  //uint16_t titleIconChar;
  LISTITEM  items[ITEM_PER_PAGE];
}LISTITEMS;

typedef struct
{
  LISTITEM_TYPE type;
  uint8_t* icon;
  uint8_t* label;
  ICON_TYPE  icon_flag;
}LISTITEM2;
typedef struct
{
  LABEL title;
  //uint16_t titleIconChar;
  LISTITEM2  items[PAGE_BUTTON+PAGE_THUMBNAIL];
}LISTITEMS2;

typedef struct
{
  LABEL title;
  ITEM  items[ITEM_PER_PAGE];
}MENUITEMS;


typedef enum
{
  STATUS_IDLE = 0,
  STATUS_BUSY,
  STATUS_NORMAL
}SYS_STATUS;

typedef struct
{
  GUI_RECT rect;
  uint32_t time;
  uint8_t  status;
  int16_t inf;
}REMINDER;

#define CHAR_FOLDER                (uint8_t *)"\u0862"
#define CHAR_FILE                  (uint8_t *)"\u0863"
#define CHAR_PAGEUP                (uint8_t *)"\u0867\u0868"
#define CHAR_PAGEDOWN              (uint8_t *)"\u0869\u086A"
#define CHAR_BACK                  (uint8_t *)"\u086B\u086C"
#define CHAR_LOWER                 (uint8_t *)"\u089E"
#define CHAR_INCREASE              (uint8_t *)"\u089F"
#define CHAR_SBACK                 (uint8_t *)"\u08A1"    // 小退出图标
#define CHAR_SAVE                  (uint8_t *)"\u08A7"
// #define CHAR_LOWER                 0x089E
// #define CHAR_INCREASE              0x089F
// #define CHAR_SBACK                 0x08A1    // 小退出图标
// #define CHAR_SAVE                  0x08A7


extern const GUI_RECT exhibitRect;
extern bool key_lock;

#define CENTER_Y  (int16_t)((exhibitRect.y1 - exhibitRect.y0)/2 + exhibitRect.y0)
#define CENTER_X  (int16_t)((exhibitRect.x1 - exhibitRect.x0 - BYTE_WIDTH)/2 + exhibitRect.x0)

uint8_t *labelGetAddress(const LABEL *label);
void reminderMessage(int16_t inf, SYS_STATUS status);
void volumeReminderMessage(int16_t inf, SYS_STATUS status);

void menuDrawListItem(const LISTITEM *item, uint8_t position);
void menuDrawItem (const ITEM *menuItem, uint8_t positon);
void menuDrawItemLabel (const uint8_t*, uint8_t positon);
void menuDrawTitle(const uint8_t *content);
void menuDrawPage (const MENUITEMS *menuItems);
void menuDrawCubeItem_check(const uint8_t *icon, uint8_t position);
void menuDrawCubeItem(const uint8_t *icon, uint8_t position);
void menuDrawCubePage(uint8_t** buttonItems, uint8_t(* cubeItems)[8]);

void menuDrawListPage(const LISTITEMS *listItems);
void itemDrawIconPress(uint8_t positon, uint8_t is_press);
void itemDrawCubeIconPress(uint8_t position, uint8_t is_press);
KEY_VALUES menuKeyGetValue(void);
uint16_t   menuKeyGetLevelingValue(void);


void loopBackEnd(void);
void loopFrontEnd(void);
void loopProcess (void);
void menuClearGaps(void);

#endif

