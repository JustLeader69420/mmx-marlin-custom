#include "../TSC_Menu.h"

static ExtUI::FileList filelist;

bool enter_by_icon = false; // 是否通过点击图标进入Prinf
bool can_print_flag = true; // 能够进行打印的标志
uint8_t auto_close_flag = 0;// 界面是否需要自动关闭 0:无需自动关闭  1:需要自动关闭  2:判断文件打印需要执行自动关闭

#if ENABLED(SHOW_THUMBNAIL)

#else
LISTITEMS printItems = {
//  title
LABEL_BACKGROUND,
  {
  // type        icon            label
   {LIST_LABEL,  NULL,           NULL},
   {LIST_LABEL,  NULL,           NULL},
   {LIST_LABEL,  NULL,           NULL},
   {LIST_LABEL,  NULL,           NULL},
   {LIST_LABEL,  NULL,           NULL},
   {LIST_BUTTON, CHAR_PAGEUP,    NULL},
   {LIST_BUTTON, CHAR_PAGEDOWN,  NULL},
   {LIST_BUTTON, CHAR_BACK,      NULL},
  }
};


// File list number per page
#define NUM_PER_PAGE	5

SCROLL   titleScroll;
GUI_RECT titleRect={10, (TITLE_END_Y - 10 - BYTE_HEIGHT) / 2, LCD_WIDTH_PIXEL-10, (TITLE_END_Y -10 - BYTE_HEIGHT) / 2 + BYTE_HEIGHT};

SCROLL   gcodeScroll;
uint8_t curPage = 0;

char title[1024] = "SD:";
bool EnterDir(const char *nextdir)
{
  // if(strlen(infoFile.title)+strlen(nextdir)+2>=MAX_PATH_LEN) return 0;
  strcat(title, "/");
  strcat(title, nextdir);
  return 1;
}
void ExitDir(void)
{
  int i = strlen(title);
  for(; i > 0 && title[i] != '/';i--)
  {	
    title[i]=0; // 找到上一个目录
  }
  title[i]=0;
}


void gocdeListDraw(void)
{
  Scroll_CreatePara(&titleScroll, (uint8_t* )title, &titleRect);
  printItems.title.address = (uint8_t* )title;
  GUI_SetBkColor(TITLE_COLOR);
  GUI_ClearPrect(&titleRect);
  GUI_SetBkColor(BK_COLOR);

  uint8_t i = 0;

  for(i=0;  (i + curPage*NUM_PER_PAGE < filelist.count())&&(i < NUM_PER_PAGE);  i++)
  { // folder
    if (!filelist.seek(i + curPage * NUM_PER_PAGE)) continue;
    if (filelist.isDir())
      printItems.items[i].icon = CHAR_FOLDER;
    else
      printItems.items[i].icon = CHAR_FILE;
    printItems.items[i].label = (uint8_t* )filelist.filename();
    menuDrawListItem(&printItems.items[i], i);
  }

  for(; (i<NUM_PER_PAGE); i++)			//background
  {
    printItems.items[i].icon = NULL;
    menuDrawListItem(&printItems.items[i], i);
  }
}

const int16_t labelVolumeError[] = {LABEL_READ_TFTSD_ERROR, LABEL_READ_U_DISK_ERROR, LABEL_READ_ONBOARDSD_ERROR};

void menuCallBackPrint(void)
{
  static bool lock = false;
  uint8_t update = 0;
  KEY_VALUES key_num = menuKeyGetValue();

  GUI_SetBkColor(TITLE_COLOR);
  Scroll_DispString(&titleScroll, LEFT);    //
  GUI_SetBkColor(BK_COLOR);
  #ifndef GCODE_LIST_MODE
    Scroll_DispString(&gcodeScroll, CENTER); //
  #endif

  if (lock) return;
  lock = true;
  switch(key_num)
  {
    case KEY_ICON_5:  //up
      if(curPage > 0)
      {
        curPage--;
        update=1;
      }
      break;

    case KEY_ICON_6:	//down
      if(curPage+1 < (filelist.count()+(NUM_PER_PAGE-1))/NUM_PER_PAGE)
      {
        curPage++;
        update=1;
      }
      break;

    case KEY_ICON_7:  //back
      curPage = 0;
      if(filelist.isAtRootDir())
      {
       // clearInfoFile();
        infoMenu.cur--;
        break;
      }
      else
      {
        ExitDir();
       // scanPrintFiles();	
        filelist.upDir();
        update = 1;
      }
      break;

    case KEY_IDLE:
      break;

    default:                   
      if(key_num <= KEY_ICON_4) //0~4 = 5 file
      {	//seek用于定位到该文件，gc或者目录返回true
        if (filelist.seek(curPage * NUM_PER_PAGE + key_num))
        {
          if (filelist.isDir()) {
            if(EnterDir(filelist.shortFilename()) == false) break;
            update = 1;
            curPage = 0;
            filelist.changeDir(filelist.shortFilename());
          } else { //gcode
           #if 0
            if(can_print_flag){ // 如果可以打印就开始打印，否则跳过
              if(LongTimeTouch){}
              #ifdef AUTO_BED_LEVELING_BILINEAR
                set_bed_leveling_enabled(true);
              #endif
              #if ENABLED(FILAMENT_RUNOUT_SENSOR)
                ExtUI::setFilamentRunoutState(false);   // 重置断料检测
              #endif

              ExtUI::printFile(filelist.shortFilename());
            }
           #else
            if(can_print_flag){ // 如果可以打印就开始打印，否则跳过
              if(LongTimeTouch)
                infoMenu.menu[++infoMenu.cur] = menuDeleteFile;
              else
                infoMenu.menu[++infoMenu.cur] = menuWhetherToPtint;
            }
           #endif
          }
        }
      }
      break;
  }
    
  if(update)
  {
    update = 0;
    gocdeListDraw();
  }
  
  if(!IS_SD_INSERTED())
  {
   // resetInfoFile();
    infoMenu.cur--;
  }
  lock = false;
}
// menuPrintFromSource
void menuPrint(void)
{
  GUI_Clear(BK_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL, textSelect(LABEL_LOADING));
  if (ExtUI::isMediaInserted())
  {
    if(enter_by_icon){
      enter_by_icon = false;
      filelist.firstOpenPrint();
      memset(title, 0, sizeof(title));
      sprintf(title, "SD:");
    }
    filelist.refresh();
    for(uint8_t i = 0; i < NUM_PER_PAGE; i++) printItems.items[i].icon = NULL;
    menuDrawListPage(&printItems);
    gocdeListDraw();
    menuSetFrontCallBack(menuCallBackPrint);
  }
  else
  {
    GUI_DispStringInRect(0, 0, LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL, textSelect(LABEL_READ_TFTSD_ERROR));
    ExtUI::delay_ms(1000);
    infoMenu.cur--;
  }
}

// 选择是否打印界面，有"Yes""No""Delete"选择
#if 0
void menuCallBackChoseFileList()
{
  uint16_t key_num = KEY_GetValue(3, threeBtnRect);
  switch(key_num){
    case KEY_POPUP_CONFIRM:
    infoMenu.cur--;

      break;
    case KEY_POPUP_CANCEL:
    infoMenu.cur--;
      
      break;
    case KEY_POPUP_DELETE:
      
    infoMenu.cur--;
      break;

    default:break;
  }
}
void menuChoseFileList()
{
  uint8_t printText[128];
  sprintf_P((char*)printText, "%s %s ?", textSelect(LABEL_PRINT), filelist.longFilename());
  popupDrawPage_T(bottomThreeBtn, textSelect(LABEL_PRINT), printText, (uint8_t*)"Yes", (uint8_t*)"No", (uint8_t*)"Delete");
  menuSetFrontCallBack(menuCallBackChoseFileList);
}
#endif

void menuCallBackWhetherToPtint()
{
  uint16_t key_num = KEY_GetValue(2, doubleBtnRect);
  switch(key_num)
  {
    case KEY_POPUP_CONFIRM:
      // infoMenu.cur--;
      #ifdef AUTO_BED_LEVELING_BILINEAR
        set_bed_leveling_enabled(true);
      #endif
      #if ENABLED(FILAMENT_RUNOUT_SENSOR)
        ExtUI::setFilamentRunoutState(false);   // 重置断料检测
      #endif

      ExtUI::printFile(filelist.shortFilename());
      auto_close_flag = 2;
      break;
    case KEY_POPUP_CANCEL:
      infoMenu.cur--;
      break;
    default:break;
  }
  if(auto_close_flag == 1)
  {
    auto_close_flag = 0;
    infoMenu.cur--;
  }
}
void menuWhetherToPtint()
{
  uint8_t printText[128];
  sprintf_P((char*)printText, "%s \n%s ?", textSelect(LABEL_PRINT), filelist.longFilename());
  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_PRINT), printText, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
  menuSetFrontCallBack(menuCallBackWhetherToPtint);
}
void menuCallBackDeleteFile()
{
  uint16_t key_num = KEY_GetValue(2, doubleBtnRect);
  switch(key_num)
  {
    case KEY_POPUP_CONFIRM:
      card.removeFile(filelist.shortFilename());
      infoMenu.cur--;
      break;
    case KEY_POPUP_CANCEL:
      infoMenu.cur--;
      break;
    default:break;
  }
}
void menuDeleteFile()
{
  uint8_t printText[128];
  sprintf_P((char*)printText, "Delete file \n%s ?", filelist.longFilename());
  changeSOF(false);
  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_WARNING), printText, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
  menuSetFrontCallBack(menuCallBackDeleteFile);
}

/*
MENUITEMS sourceSelItems = {
//  title
LABEL_PRINT,
// icon                       label
 {{ICON_SD_SOURCE,            LABEL_TFTSD},
 #ifdef ONBOARD_SD_SUPPORT
  {ICON_BSD_SOURCE,           LABEL_ONBOARDSD},
 #endif
 #ifdef U_DISK_SUPPROT
  {ICON_U_DISK,               LABEL_U_DISK},
 #else
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
 #endif
 #ifndef ONBOARD_SD_SUPPORT
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
 #endif
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

void menuPrint(void)
{
  KEY_VALUES  key_num = KEY_IDLE;
  
  menuDrawPage(&sourceSelItems,false);
  while(infoMenu.menu[infoMenu.cur] == menuPrint)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        infoFile.source = TFT_SD;
        infoMenu.menu[++infoMenu.cur] = menuPrintFromSource;
        infoMenu.menu[++infoMenu.cur] = menuPowerOff;
        goto selectEnd;
      
      #ifdef ONBOARD_SD_SUPPORT
      case KEY_ICON_1:
        infoFile.source = BOARD_SD;
        infoMenu.menu[++infoMenu.cur] = menuPrintFromSource;   //TODO: fix here,  onboard sd card PLR feature
        goto selectEnd;
      #endif
      
      #ifdef U_DISK_SUPPROT
      #ifdef ONBOARD_SD_SUPPORT
      case KEY_ICON_2:
      #else
      case KEY_ICON_1:
      #endif
        infoFile.source = TFT_UDISK;
        infoMenu.menu[++infoMenu.cur] = menuPrintFromSource;
        infoMenu.menu[++infoMenu.cur] = menuPowerOff;
        goto selectEnd;
      #endif
      
      case KEY_ICON_7:
        infoMenu.cur--;
        return;
      
      default: break;
    }
    loopProcess();
  }
  
selectEnd:  
  resetInfoFile();
  powerFailedSetDriverSource(getCurFileSource());
}
*/
#endif