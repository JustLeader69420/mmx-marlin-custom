#include "../TSC_Menu.h"

#ifdef HAS_UDISK
  #include "ff.h"
#endif

#if ENABLED(USART_LCD)
  #include "../../../../../lcd_show_addr.h"
#endif
extern bool wait_quick_stop_step;
static bool pop_up_lock = true;          // 弹窗锁,防止多次点击造成机器死机,true开启,false关闭

//1title, ITEM_PER_PAGE item(icon + label) 
MENUITEMS printingItems = {
//  title
LABEL_BACKGROUND,
// icon                       label
 {{ICON_PAUSE,                LABEL_PAUSE},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_STOP,                 LABEL_STOP},
  {ICON_HEAT,                 LABEL_HEAT},
  {ICON_PERCENTAGE,           LABEL_PERCENTAGE},
  {ICON_BABYSTEP,             LABEL_BABYSTEP},
  {ICON_MORE,                 LABEL_MORE},}
};

const ITEM itemIsPause[2] = {
// icon                       label
  {ICON_PAUSE,                LABEL_PAUSE},
  {ICON_RESUME,               LABEL_RESUME},
};

bool isPrintInfoMenu(){
  return (infoMenu.menu[infoMenu.cur] == menuPrinting);
}

//
bool isPrinting(void)
{
  return (ExtUI::isPrintingFromMedia() && !card.flag.abort_sd_printing) TERN_( HAS_UDISK, || (udisk.isUdiskPrint() && !udisk.isUdiskStop()));
}

//
bool isPaused(void)
{
  return ExtUI::isPrintingFromMediaPaused();
}

uint8_t getPrintProgress(void)
{
 #ifdef HAS_UDISK
  if(udisk.isUdiskPrint())  return udisk.getPrintProgress();
  else
 #endif
    return card.percentDone();
}
uint8_t getUDiskPrintProgress(void)
{
  
}

uint32_t getPrintTime(void)
{
  return print_job_timer.duration();
}



//only return gcode file name except path
//for example:"SD:/test/123.gcode"
//only return "123.gcode"
uint8_t *getCurGcodeName(char *path)
{
  int i=strlen(path);
  for(; path[i]!='/'&& i>0; i--)
  {}
  return (uint8_t* )(&path[i+1]);
}

void resumeToPause(bool is_pause)
{
  if(infoMenu.menu[infoMenu.cur] != menuPrinting) return;
  printingItems.items[KEY_ICON_0] = itemIsPause[is_pause];
  menuDrawItem(&itemIsPause[is_pause],0);
}

bool setPrintPause(bool is_pause)
{
  static bool pauseLock = false;
  if(pauseLock)                      return false;
  if(!isPrinting())                  return false;
  if(isPaused() == is_pause)         return false;
  pauseLock = true;
  if (is_pause) {
    ExtUI::pausePrint();
  } else {

    #if ENABLED(FILAMENT_RUNOUT_SENSOR)
      if((!ExtUI::getFilamentRunoutEnabled()) || (READ(FIL_RUNOUT_PIN) != FIL_RUNOUT_STATE))   // 检测是否有耗材
      {
        ExtUI::setFilamentRunoutState(false);
    #endif
    
        ExtUI::setUserConfirmed();  // 停止等待
        ExtUI::resumePrint();

    #if ENABLED(FILAMENT_RUNOUT_SENSOR)
      }
      else
      {
        TERN_(EXTENSIBLE_UI, ExtUI::onFilamentRunout(ExtUI::getActiveTool()));  // 跳出filament runout弹窗
      }
    #endif

  }
  pauseLock = false;
  return true;
}

#if 0//def HAS_UDISK
bool setUDiskPrintPause(){
  static bool UDiskPauseLock = false;
  if(UDiskPauseLock)  return false;
  UDiskPauseLock = true;
  if(!udisk.isUdiskPause()){ // 处于非暂停状态就暂停
    ExtUI::pausePrint();
  } else {              // 处于暂停状态就恢复

    #if ENABLED(FILAMENT_RUNOUT_SENSOR)
      if(READ(FIL_RUNOUT_PIN) != FIL_RUNOUT_STATE){   // 检测是否有耗材
        ExtUI::setFilamentRunoutState(false);
    #endif
    
        ExtUI::setUserConfirmed();
        ExtUI::resumePrint();

    #if ENABLED(FILAMENT_RUNOUT_SENSOR)
      }
      else
        TERN_(EXTENSIBLE_UI, ExtUI::onFilamentRunout(ExtUI::getActiveTool()));  // 跳出filament runout弹窗
    #endif

  }
  UDiskPauseLock = false;
  return true;
}
#endif


const GUI_RECT progressRect = {1*SPACE_X_PER_ICON, 0*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y + ICON_HEIGHT/4,
                               3*SPACE_X_PER_ICON, 0*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y + ICON_HEIGHT*3/4};

#define BED_X  (progressRect.x1 - 9 * BYTE_WIDTH)
#define TEMP_Y (progressRect.y1 + 3)
#define TIME_Y (TEMP_Y + 1 * BYTE_HEIGHT + 3)
#define POSITION_XY_Y (progressRect.y0 - 1 * BYTE_HEIGHT - 3)

static void redrawToolAct(void)
{
  GUI_DispDec(progressRect.x0+BYTE_WIDTH*3, TEMP_Y, (int16_t)statusMsg.actHotend, 3, RIGHT);
}
static void redrawToolTag(void)
{
  GUI_DispDec(progressRect.x0+BYTE_WIDTH*7, TEMP_Y, (int16_t)statusMsg.tagHotend,  3, LEFT);
}

static void redrawBedAct(void)
{
  GUI_DispDec(BED_X + 2 * BYTE_WIDTH, TEMP_Y, (int16_t)statusMsg.actBed, 3, RIGHT);
}
static void redrawBedTag(void)
{
  GUI_DispDec(BED_X + 6 * BYTE_WIDTH, TEMP_Y, (int16_t)statusMsg.tagBed,  3, LEFT);
}

static void redrawPositionX(void)
{
  GUI_DispFloat(progressRect.x0 + 2*BYTE_WIDTH, POSITION_XY_Y, statusMsg.x, 4, 2, LEFT);
}
static void redrawPositionY(void)
{
  GUI_DispFloat(BED_X + 2*BYTE_WIDTH, POSITION_XY_Y, statusMsg.y, 4, 2, LEFT);
}
static void redrawPositionZ(void)
{
  GUI_DispFloat(BED_X + 2*BYTE_WIDTH, TIME_Y, statusMsg.z, 4, 2, LEFT);
}

void reDrawTime(void)
{
  uint32_t printedTime = getPrintTime();
  uint32_t  hour = printedTime/3600%1000;   // 让小时数最大可以计数到999
  uint8_t   min = printedTime%3600/60,
            sec = printedTime%60;

  GUI_SetNumMode(GUI_NUMMODE_ZERO);
  GUI_DispDecTime(progressRect.x0 + 2 * BYTE_WIDTH, TIME_Y, hour, 3, RIGHT);
  GUI_DispDecTime(progressRect.x0 + 6 * BYTE_WIDTH, TIME_Y, min,  2, RIGHT);
  GUI_DispDecTime(progressRect.x0 + 9 * BYTE_WIDTH, TIME_Y, sec,  2, RIGHT);
  GUI_SetNumMode(GUI_NUMMODE_SPACE);
  
}

#if ENABLED(USART_LCD)
  void send_time(uint32_t pt){
    int hour = pt/3600%1000;   // 让小时数最大可以计数到999
    int min  = pt%3600/60,
        sec  = pt%60;
    char send_str[16] = {0};
    memset(send_str, 0, sizeof(send_str));
    addHeadMess(send_str);
    send_str[2] = 9+3;
    send_str[4] = PRING_TIME >> 8;    send_str[5] = PRING_TIME & 0xff;
    sprintf_P(&send_str[6], "%d:%d:%d", hour, min, sec);
    send_hexPGM(send_str, send_str[2]+3);
    // delay(50);
  }
#endif

void reDrawProgress(uint8_t progress)
{	  
  char buf[5];
  uint16_t progressX = map(progress, 0, 100, progressRect.x0, progressRect.x1);
  GUI_FillRectColor(progressRect.x0, progressRect.y0, progressX, progressRect.y1,BLUE);
  GUI_FillRectColor(progressX, progressRect.y0, progressRect.x1, progressRect.y1,GRAY);
  sprintf(buf, "%3d%%", progress);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_DispStringInPrect(&progressRect, (uint8_t *)buf);    
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);                     
}

extern SCROLL   titleScroll;
extern GUI_RECT titleRect;


void printingDrawPage(void)
{
 #ifdef HAS_UDISK
  if(udisk.isUdiskPrint())
    printingItems.title.address = (uint8_t *)workFileinfo.fname;
  else
    printingItems.title.address = (uint8_t *)card.longest_filename(); //getCurGcodeName(infoFile.title);
 #else
  printingItems.title.address = (uint8_t *)card.longest_filename(); //getCurGcodeName(infoFile.title);
 #endif
 #if ENABLED(USART_LCD)
  char send_str[64] = {0};
  char send_str2[8] = {0x5A, 0xA5, 0x05, 0x82, 0x2B, 0xBE, 0x00, 0x01};
  memset(send_str, 0, sizeof(send_str));
  addHeadMess(send_str);
  send_str[4] = PRINTING_HEAD_TEXT >> 8;  send_str[5] = PRINTING_HEAD_TEXT & 0xff;
  sprintf(&send_str[6], "%s", printingItems.title.address);
  send_str[2] = 34+3;
  send_hexPGM(send_str, send_str[2]+3);
  delay(20);
  send_hexPGM(send_str2, 8);
  delay(20);
 #endif

  menuDrawPage(&printingItems);
  //	Scroll_CreatePara(&titleScroll, infoFile.title,&titleRect);  //
  // printed time
  GUI_DispString(progressRect.x0, TIME_Y, (uint8_t* )"T:");
  GUI_DispString(progressRect.x0+BYTE_WIDTH*5, TIME_Y, (uint8_t* )":");
  GUI_DispString(progressRect.x0+BYTE_WIDTH*8, TIME_Y, (uint8_t* )":");
  // nozzle temperature 
  GUI_DispString(progressRect.x0+BYTE_WIDTH*2, TEMP_Y,(uint8_t* )":");
  GUI_DispString(progressRect.x0+BYTE_WIDTH*6, TEMP_Y,(uint8_t* )"/");
  // hotbed temperature
  GUI_DispString(BED_X, TEMP_Y, (uint8_t* )"B:");
  GUI_DispString(BED_X+BYTE_WIDTH*5, TEMP_Y, (uint8_t* )"/");

  GUI_DispString(progressRect.x0, POSITION_XY_Y, (uint8_t* )"X:");
  GUI_DispString(BED_X, POSITION_XY_Y, (uint8_t* )"Y:");
  GUI_DispString(BED_X, TIME_Y, (uint8_t* )"Z:");

  reDrawProgress(getPrintProgress());
  reDrawTime();

  statusMsg_GetCurMsg(&statusMsg);
  GUI_DispString(progressRect.x0, TEMP_Y, (uint8_t* )heatDisplayID[statusMsg.curTool + 1]);
  redrawToolAct();
  redrawToolTag();
  redrawBedAct();
  redrawBedTag();

  redrawPositionX();
  redrawPositionY();
  redrawPositionZ();
}

uint8_t printPaused = 0;
uint8_t printPaused2 = 0;
static uint8_t lastProgress = 0;
static uint32_t printedTime = 0;
void menuCallBackPrinting(void)	
{
  STATUS_MSG tempMsg;
  statusMsg_GetCurMsg(&tempMsg);
  
  KEY_VALUES  key_num = menuKeyGetValue();
//  Scroll_DispString(&titleScroll, LEFT); //Scroll display file name will take too many CPU cycles

  // Actual temperature refresh per 1 sec
  static uint32_t nowTime_ms = 0;
  if (millis() - nowTime_ms > 1000) { // Refresh per 1 sec
    nowTime_ms = millis();
    // Refresh position
    if (statusMsg.x != tempMsg.x){
      statusMsg.x = tempMsg.x;
      redrawPositionX();
    }
    if (statusMsg.y != tempMsg.y){
      statusMsg.y = tempMsg.y;
      redrawPositionY();
    }
    if (statusMsg.z != tempMsg.z){
      statusMsg.z = tempMsg.z;
      redrawPositionZ();
    }
    // Refresh temperature
    //  if (tool != tempTool)
    if (statusMsg.actHotend != tempMsg.actHotend) {
      statusMsg.actHotend = tempMsg.actHotend;
      redrawToolAct();
    }    
    if (statusMsg.actBed != tempMsg.actBed) {
      statusMsg.actBed = tempMsg.actBed;
      redrawBedAct();
    }
  }

  // Target temperature refresh in real time  
  if (statusMsg.tagHotend != tempMsg.tagHotend) {
    statusMsg.tagHotend = tempMsg.tagHotend;
    redrawToolTag();
  }
  if (statusMsg.tagBed != tempMsg.tagBed) {
    statusMsg.tagBed = tempMsg.tagBed;
    redrawBedTag();
  }
  
  if (lastProgress != getPrintProgress())
  {
    lastProgress = getPrintProgress();
    reDrawProgress(lastProgress);
    #if ENABLED(USART_LCD)
      
      // delay(50);
    #endif
  }
  if (printedTime != getPrintTime())
  {
    printedTime = getPrintTime();
    reDrawTime();
  }

  switch(key_num)
  {
    case KEY_ICON_0:
     #if 0//def HAS_UDISK
      if(udisk.isUdiskPrint()){
        if(printPaused2 == udisk.isUdiskPause()){
          printPaused2 = !udisk.isUdiskPause();
          resumeToPause(printPaused2);
          setUDiskPrintPause();
        }
      }
      else
     #endif
      {
        if(printPaused == isPaused()){
          printPaused = !isPaused();
          resumeToPause(printPaused);
          setPrintPause(!isPaused());
        }
      }
    break;
    
    case KEY_ICON_3:
      if(isPrinting())
        infoMenu.menu[++infoMenu.cur] = menuStopPrinting;	
    break;
      
    case KEY_ICON_4:
      infoMenu.menu[++infoMenu.cur] = menuHeat;
    break;
    
    case KEY_ICON_5:
      infoMenu.menu[++infoMenu.cur] = menuSpeed;
    break;
    
    case KEY_ICON_6:
      infoMenu.menu[++infoMenu.cur] = menuBabyStep;
    break;
    
    case KEY_ICON_7:
      infoMenu.menu[++infoMenu.cur] = menuMore;
    break;
    
    default :break;
  }
}

void menuPrinting(void)
{
  printPaused = isPaused();
 #if 0//def HAS_UDISK
  printPaused2 = udisk.isUdiskPause();
  if(udisk.isUdiskPrint())
    printingItems.items[KEY_ICON_0] = itemIsPause[printPaused2];
  else
 #endif
    printingItems.items[KEY_ICON_0] = itemIsPause[printPaused];
  printingDrawPage();

  #if ENABLED(USART_LCD)
    // 串口屏切换到打印界面
    char send_str[10] = {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x10};
    send_hexPGM(send_str, 10);
    // delay(50);
  #endif
  
  menuSetFrontCallBack(menuCallBackPrinting);
}

void menuCallBackStopPrinting(void)
{
  uint16_t key_num = KEY_GetValue(2, doubleBtnRect);
  if(!pop_up_lock){
    switch(key_num)
    {
      case KEY_POPUP_CONFIRM:
        pop_up_lock = true;       // 开启弹窗锁
        can_print_flag = false;   // 不能进行打印
        wait_quick_stop_step = true;  // 等待快速停止电机
        TERN_( HAS_UDISK, udisk.abortUdiskPrint(); )
        ExtUI::stopPrint();
        set_bed_leveling_enabled(false);
        break;

      case KEY_POPUP_CANCEL:
        pop_up_lock = true;       // 开启弹窗锁
        infoMenu.cur--;
        break;
      default:break;
    }
  }
}

void menuStopPrinting(void)
{
  pop_up_lock = false;
  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_WARNING), textSelect(LABEL_STOP_PRINT), textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
  menuSetFrontCallBack(menuCallBackStopPrinting);
}

// Shut down menu, when the hotend temperature is higher than "AUTO_SHUT_DOWN_MAXTEMP"
// wait for cool down, in the meantime, you can shut down by force
void menuCallBackShutDown(void)
{
  static bool lock = false;
  bool shutDown = false;
  bool tempIsLower = true;
  uint16_t key_num = KEY_GetValue(2, doubleBtnRect);

  if (lock) return;
  lock = true;
  switch(key_num)
  {
    case KEY_POPUP_CONFIRM:
      shutDown = true;
      break;

    case KEY_POPUP_CANCEL:
      infoMenu.cur--;
      break;
  }
  for (ExtUI::extruder_t i = ExtUI::E0; i < EXTRUDERS; i = (ExtUI::extruder_t)(i + 1)) {
    if (ExtUI::getActualTemp_celsius(i) >= AUTO_SHUT_DOWN_MAXTEMP) {
      tempIsLower = false;
    }
  }
  if(tempIsLower || shutDown)
  {
    for (ExtUI::fan_t i = ExtUI::FAN0; i < FAN_COUNT; i = (ExtUI::fan_t)(i + 1))
      ExtUI::setTargetFan_percent(0, i);
    storeCmd(PSTR("M81"));
    infoMenu.cur--;
    popupReminder(textSelect(LABEL_SHUT_DOWN), textSelect(LABEL_SHUTTING_DOWN));
  }
  lock = false;
}

void menuShutDown(void)
{
  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_SHUT_DOWN), textSelect(LABEL_WAIT_TEMP_SHUT_DOWN), textSelect(LABEL_FORCE_SHUT_DOWN), textSelect(LABEL_CANNEL)); 
  for (ExtUI::fan_t i = ExtUI::FAN0; i < FAN_COUNT; i = (ExtUI::fan_t)(i + 1))
    ExtUI::setTargetFan_percent(100, i);
  menuSetFrontCallBack(menuCallBackShutDown);
}
