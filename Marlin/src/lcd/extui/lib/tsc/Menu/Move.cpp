#include "../TSC_Menu.h"

MENUITEMS moveItems = {
//title
LABEL_MOVE,
//icon
{
  {ICON_Z_DEC, LABEL_Z_DEC},
  {ICON_Y_INC, LABEL_Y_DEC},
  {ICON_Z_INC, LABEL_Z_INC},
  {ICON_10_MM,  LABEL_10_MM},
  
//  #ifdef QUICK_PRINT
//   {ICON_X_DEC, LABEL_X_INC},
//   {ICON_Y_DEC, LABEL_Y_INC},
//   {ICON_X_INC, LABEL_X_DEC},
//  #else
  {ICON_X_DEC, LABEL_X_DEC},
  {ICON_Y_DEC, LABEL_Y_INC},
  {ICON_X_INC, LABEL_X_INC},
//  #endif
  {ICON_BACK,  LABEL_BACK},
}
};

#define ITEM_MOVE_LEN_NUM 3
const ITEM itemMoveLen[ITEM_MOVE_LEN_NUM] = {
//icon          label
  {ICON_01_MM,  LABEL_01_MM},
  {ICON_1_MM,   LABEL_1_MM},
  {ICON_10_MM,  LABEL_10_MM},
};

GUI_RECT busyrect = {50, 0, LCD_WIDTH_PIXEL, TITLE_END_Y-10};

const float item_move_len[ITEM_MOVE_LEN_NUM] = {0.1f, 1, 10};
static uint8_t item_move_len_i = 2;
// xyz轴变化的mm值
static float x_add_mm   = 0;
static float y_add_mm   = 0;
static float z_add_mm   = 0;
// 临时存放xyz的值
static int   x_add_mm_t = 0;
static int   y_add_mm_t = 0;
static int   z_add_mm_t = 0;
// 发送Gcode命令
static char G0_STR[64] = {0};
// 判断当前系统是否繁忙
static bool isBusy = false;
static uint8_t top_info_ai = 0, top_info_bi = 0;    // 用于是否需要清空顶部背景

static void setAxisPosition(float diff_mm, const ExtUI::axis_t axis)
{
  ExtUI::setAxisPosition_mm(ExtUI::getAxisPosition_mm(axis) + diff_mm, axis);
}

extern GUI_RECT titleRect;
static void redrawPosition(void)
{
  GUI_SetBkColor(TITLE_COLOR);
  
  GUI_DispString(START_X+1*SPACE_X+1*ICON_WIDTH, (TITLE_END_Y-10-BYTE_HEIGHT)/2,(uint8_t *)"X:");
  GUI_DispFloat(START_X+1*SPACE_X+1*ICON_WIDTH+2*BYTE_WIDTH, (TITLE_END_Y-10-BYTE_HEIGHT)/2, statusMsg.x, 4, 1, LEFT);

  GUI_DispString(START_X+2*SPACE_X+2*ICON_WIDTH, (TITLE_END_Y-10-BYTE_HEIGHT)/2,(uint8_t *)"Y:");
  GUI_DispFloat(START_X+2*SPACE_X+2*ICON_WIDTH+2*BYTE_WIDTH, (TITLE_END_Y-10-BYTE_HEIGHT)/2, statusMsg.y, 4, 1, LEFT);

  GUI_DispString(START_X+3*SPACE_X+3*ICON_WIDTH, (TITLE_END_Y-10-BYTE_HEIGHT)/2,(uint8_t *)"Z:");
  GUI_DispFloat(START_X+3*SPACE_X+3*ICON_WIDTH+2*BYTE_WIDTH, (TITLE_END_Y-10-BYTE_HEIGHT)/2, statusMsg.z, 4, 1, LEFT);

  GUI_SetBkColor(BK_COLOR);
}

// 绘制繁忙信息
void drawTopInfo(int16_t inf)
{
  GUI_SetColor(RED);
  GUI_SetBkColor(BLACK);
  GUI_DispStringInPrect(&busyrect, textSelect(inf));
  GUI_SetColor(FK_COLOR);
}
void drawTopInfo(const uint8_t *p)
{
  GUI_SetBkColor(BLACK);
  GUI_SetColor(RED);
  GUI_DispStringInPrect(&busyrect, p);
  GUI_SetColor(FK_COLOR);
}

void menuCallBackMove()
{
  STATUS_MSG tempMsg;
  statusMsg_GetCurMsg(&tempMsg);
  // Position refresh per 1 sec
  static uint32_t nowTime_ms = 0;

  if(queue.length == 0){    // G命令队列为空
    isBusy = false;
    // 判断哪个轴移动了
    if(x_add_mm != 0){
      x_add_mm_t = (int)(getAxisPosition_mm(ExtUI::X) * 10) + (int)(x_add_mm * 10);
      // 防止超越机器越界
      x_add_mm_t = (x_add_mm_t > X_BED_SIZE*10) ? X_BED_SIZE*10 : x_add_mm_t;    
      if(x_add_mm_t < 0)  x_add_mm_t = 0;
      // 清空，防数据干扰
      x_add_mm = 0;
      // 清空数组，防数据干扰
      memset(G0_STR, 0, sizeof(G0_STR));
      sprintf(G0_STR, "G0 X%d.%d F3000\n", x_add_mm_t/10, x_add_mm_t%10);   // 生成G命令
      SERIAL_ECHOLNPAIR("test: ", G0_STR);    // 测试语句
      queue.enqueue_one_now(G0_STR);    // G命令入队
    }
    else if(y_add_mm != 0){
      y_add_mm_t = (int)(getAxisPosition_mm(ExtUI::Y) * 10) + (int)(y_add_mm * 10);
      y_add_mm_t = (y_add_mm_t > Y_BED_SIZE*10) ? Y_BED_SIZE*10 : y_add_mm_t;
      if(y_add_mm_t < 0)  y_add_mm_t = 0;
      y_add_mm = 0;
      memset(G0_STR, 0, sizeof(G0_STR));
      sprintf(G0_STR, "G0 Y%d.%d F3000\n", y_add_mm_t/10, y_add_mm_t%10);
      queue.enqueue_one_now(G0_STR);
    }
    else if(z_add_mm != 0){
      z_add_mm_t = (int)(getAxisPosition_mm(ExtUI::Z) * 10) + (int)(z_add_mm * 10);
      z_add_mm_t = (z_add_mm_t > Z_BED_SIZE*10) ? Z_BED_SIZE*10 : z_add_mm_t;
      if(z_add_mm_t < 0)  z_add_mm_t = 0;
      z_add_mm = 0;
      memset(G0_STR, 0, sizeof(G0_STR));
      #ifdef QUICK_PRINT
        sprintf(G0_STR, "G0 Z%d.%d F3000\n", z_add_mm_t/10, z_add_mm_t%10);
      #else
        sprintf(G0_STR, "G0 Z%d.%d F1000\n", z_add_mm_t/10, z_add_mm_t%10);
      #endif
      queue.enqueue_one_now(G0_STR);
    }
  }else isBusy = true;

  if (millis() - nowTime_ms > 1000) { // Refresh per 1 sec
    nowTime_ms = millis();
    // Refresh position
    if ((!isBusy) && (statusMsg.x != tempMsg.x || statusMsg.y != tempMsg.y || statusMsg.z != tempMsg.z)){
      if(top_info_bi == 0){
        top_info_bi++; top_info_ai = 0;
        GUI_ClearRect(95, 0, LCD_WIDTH_PIXEL, TITLE_END_Y-10);
      }
      statusMsg.x = tempMsg.x;
      statusMsg.y = tempMsg.y;
      statusMsg.z = tempMsg.z;
      redrawPosition();
    }
    else if(isBusy){
      if(top_info_ai == 0){
        top_info_ai++; top_info_bi = 0;
        GUI_ClearRect(95, 0, LCD_WIDTH_PIXEL, TITLE_END_Y-10);
      }
      drawTopInfo(LABEL_BUSY);  // 绘制繁忙信息
    }
  }

  KEY_VALUES key_num = menuKeyGetValue();
  switch (key_num)
  {
    case KEY_ICON_6: x_add_mm += item_move_len[item_move_len_i];    break;
    case KEY_ICON_5: y_add_mm += item_move_len[item_move_len_i];    break;
    case KEY_ICON_2: z_add_mm += item_move_len[item_move_len_i];    break;
    case KEY_ICON_3: 
      item_move_len_i = (item_move_len_i+1)%ITEM_MOVE_LEN_NUM;            
      moveItems.items[key_num] = itemMoveLen[item_move_len_i];
      menuDrawItem(&moveItems.items[key_num], key_num);
      break;
    case KEY_ICON_4: x_add_mm -= item_move_len[item_move_len_i];    break;
    case KEY_ICON_1: y_add_mm -= item_move_len[item_move_len_i];    break;
    case KEY_ICON_0: z_add_mm -= item_move_len[item_move_len_i];    break;
    case KEY_ICON_7: infoMenu.cur--; break;  
    default:
      break;
  }
}

void menuMove()
{
  x_add_mm = y_add_mm = z_add_mm = top_info_ai = top_info_bi = 0; // 防止上一次界面的干扰
  statusMsg_GetCurMsg(&statusMsg);
  menuDrawPage(&moveItems);
  redrawPosition();
  menuSetFrontCallBack(menuCallBackMove);
}
