#ifndef _POPUP_H_
#define _POPUP_H_

#include <stdint.h>
#include "../GUI.h"
#include "../LCD_Init.h"

//TODO:fix here for more resolution
#if LCD_WIDTH_PIXEL == 320 && LCD_HEIGHT_PIXEL == 240
  #define POPUP_RECT_WINDOW         {30, 25, 290, 215}

  #define POPUP_RECT_SINGLE_CONFIRM {110,  165, 210, 205}
  #define POPUP_RECT_DOUBLE_CONFIRM {40,  165, 140, 205}
  #define POPUP_RECT_DOUBLE_CANCEL  {180, 165, 280, 205}
  
  #define POPUP_TITLE_HEIGHT  30
  #define POPUP_TEXT_HEIGHT   100
  #define POPUP_BOTTOM_HEIGHT 50
#elif LCD_WIDTH_PIXEL == 480 && LCD_HEIGHT_PIXEL == 320
  #define POPUP_RECT_WINDOW         {80, 50, 400, 270}

  #define POPUP_RECT_SINGLE_CONFIRM {180, 210, 300, 260}
  #define POPUP_RECT_DOUBLE_CONFIRM {90,  210, 210, 260}
  #define POPUP_RECT_DOUBLE_CANCEL  {270, 210, 390, 260}
  #define POPUP_RECT_THREE_CONFIRM  { 85, 210, 185, 260}
  #define POPUP_RECT_THREE_DELETE   {190, 210, 290, 260}
  #define POPUP_RECT_THREE_CANCEL   {295, 210, 395, 260}
  
  #define POPUP_TITLE_HEIGHT  40
  #define POPUP_TEXT_HEIGHT   110
  #define POPUP_BOTTOM_HEIGHT 70
#endif

#if 1
enum ABL_MODE{
  ABL_BLANK=0,
  ABL_STANDBY,
  ABL_INIT,
  ABL_HEATING,
  ABL_START,
  ABL_LEVELING,
  ABL_DONE,
  ABL_ERROR,
  ABL_CLOSE_WINDOW,
};
enum Unload_State:uint8_t{
  UNLOAD_IDLE = 0,
  UNLOAD_WAIT_HEATING,
  UNLOAD_LOAD,
  UNLOAD_UNLOAD,
  UNLOAD_WAIT_RUN,
  UNLOAD_END,
  UNLOAD_AUTO_CLOSE,
};
#else
#define ABL_STANDBY         1
#define ABL_INIT            2
#define ABL_HEATING         3
#define ABL_START           4
#define ABL_LEVELING        5
#define ABL_DONE            6
#define ABL_CLOSE_WINDOW    7
#endif

enum
{
  KEY_POPUP_CONFIRM = 0,
  KEY_POPUP_CANCEL,
  KEY_POPUP_DELETE,
};

extern BUTTON bottomSingleBtn;
extern BUTTON bottomDoubleBtn[];
extern BUTTON bottomThreeBtn[];
extern GUI_RECT threeBtnRect[];
extern GUI_RECT doubleBtnRect[];
extern GUI_RECT singleBtnRect;
extern WINDOW window;

extern ABL_MODE ABL_STATUS;

void changeSOF(bool _SOF);
void windowSetButton(const BUTTON *btn);
void windowReDrawButton(uint8_t positon, uint8_t is_press);
void popupDrawPage(BUTTON *btn, const uint8_t *title, const uint8_t *context, const uint8_t *yes, const uint8_t *no);
void popupDrawPage_T(BUTTON *btn, const uint8_t *title, const uint8_t *context, const uint8_t *yes, const uint8_t *no, const uint8_t *del);
void popupReminder_p(uint8_t* info, uint8_t* context);
void popupReminder_B(uint8_t* info, uint8_t* context);
void popupReminder_SF(uint8_t* info, uint8_t* context, bool _SOF);
void popupReminder(uint8_t* info, uint8_t* context);
void menuPopup(void);
void menuPopup_B(void);
void menuPopup_ABL(void);
void GUI_DrawWindow_ABL(const WINDOW *window, const uint8_t *title, const uint8_t *inf);
void menuPopuCap(void);
void menuPopuCap2(void);
void setDialogInfo(BUTTON *btn, uint8_t *title, uint8_t *context, uint8_t *yes, uint8_t *no, void (*ok_action)(), void (*cancel_action)());
void menuCallBackDialog(void);
void menuDialog(void);
void menuPopup_Unload(void);
void setUnloadState(uint8_t state, uint8_t e);

#endif
