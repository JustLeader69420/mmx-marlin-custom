// #include "PrintUdisk.h"
#include "../TSC_Menu.h"
// #include "stm32_usb.h"

#ifdef HAS_UDISK

static ExtUI::FileList filelistUdisk;
static const GUI_RECT RecProgress = {60,150,316,175};
// bool UDiskPrint = false;
// bool UDiskPausePrint = false;
// bool UDiskStopPrint = false;
bool UDiskPrintFinish = false;
uint64_t UDiskFileSize = 0;
uint64_t UDiskPrintSize = 0;
FIL udisk_fp;

LISTITEMS printItemsUdisk = {
//  title
LABEL_BACKGROUND,
  {
// icon                       label
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

SCROLL   titleScrollUdisk;
GUI_RECT titleRectUdisk={10, (TITLE_END_Y - 10 - BYTE_HEIGHT) / 2, LCD_WIDTH_PIXEL-10, (TITLE_END_Y -10 - BYTE_HEIGHT) / 2 + BYTE_HEIGHT};

SCROLL   gcodeScrollUdisk;
uint8_t curPageUdisk = 0;
char gUdiskPath[1024];

char titleUdisk[128] = "UDisk:";
bool EnterDirUdisk(const char *nextdir)
{
  // if(strlen(infoFile.title)+strlen(nextdir)+2>=MAX_PATH_LEN) return 0;
  strcat(gUdiskPath, "/");
  strcat(gUdiskPath, nextdir);
  return 1;
}
void ExitDirUdisk(void)
{
  int i = strlen(gUdiskPath);
  for(; i > 0 && gUdiskPath[i] != '/';i--)//从右往左找/
  {	
  }
  gUdiskPath[i]=0;
}
enum {
  FTYPE_GCODE = 0,
  FTYPE_DIR,
};
// typedef struct
// {
//   int isDir;
//   char fname[255]; //store short fname
// }ufl_t;
FILINFO ufilelist[NUM_PER_PAGE];
int udisk_cwd_fcount; //when change dir, need update
bool udisk_at_root = true;
char filePath[256] = {0};

static inline bool is_dir_or_gcode(FILINFO *fno)
{
  return !( fno->fattrib &  AM_SYS ) &&
         (fno->fattrib & AM_DIR ) ||
          strstr(fno->altname, ".GCO") ;
}
//
// Get the number of (compliant) items in the folder
//
int udisk_file_count_dir_and_gcode(char *path) {
  DIR dir;
  int c = 0;
  FRESULT res;
  FILINFO fileinfo;
  res = f_opendir(&dir, path);
  while (1) {
    res = f_readdir(&dir, &fileinfo);
    if (res != FR_OK || fileinfo.fname[0] == 0) break;
    if ( fileinfo.fattrib &  AM_SYS ) continue; //ignore system file
    if (  (fileinfo.fattrib & AM_DIR ) ||
          strstr(fileinfo.altname, ".GCO") )
    {
      c++;
      rtt.printf("-%s/%s \n", path, fileinfo.fname);
    }
  }
  f_closedir(&dir);
    // c += is_dir_or_gcode(p);
    // flag.filenameIsDir                                  // All Directories are ok
    // || (p.name[8] == 'G' && p.name[9] != '~')           // Non-backup *.G* files are accepted
  return c;
}

DIR workDir;
FILINFO workFileinfo;

//true if read valid in current pos
bool udisk_seek(const uint16_t pos) {
    if ( (pos + 1) > udisk_cwd_fcount )
      return false;
    
    f_opendir(&workDir, gUdiskPath);
    for (int c=0; c < udisk_cwd_fcount;) {
      f_readdir(&workDir, &workFileinfo);
      // rtt.printf("list file:%s\n", workFileinfo.fname);
      if (is_dir_or_gcode(&workFileinfo)) {
        
        if (c == pos) {
          rtt.printf("chose file:%s\n", workFileinfo.fname);
          return true;
        }c++;
      }
    }
}
  #if 0
#define copysize 1024*2
void copy_file_to_sdcard(char* path)
{
  // uint8_t buffer[512];
  uint8_t *buffer= new uint8_t [copysize];
  if(buffer == NULL) return;
  FIL fp;
  FRESULT res;
  UINT rd_cnt;
  FILINFO info;
  uint32_t fsize=0;//文件大小
  uint8_t i=0, j=0, str[6] = {0};
  long size_r = 0;
  float size_p = 0;  // 读取的大小，和进度条每格大小
  
  f_stat(path, &info);          // 获取文件信息
  fsize = info.fsize;
  SERIAL_ECHOLNPAIR("file size:", fsize);
  size_p = fsize/30.0f;

  popupDrawPage(NULL , (const uint8_t *)info.fname, NULL, NULL, NULL);

    f_open(&fp, path,  FA_READ | FA_OPEN_ALWAYS);
    card.openFileWrite(path);
    GUI_SetColor(GANTRYLBL_COLOR);
    GUI_SetBkColor(BLACK);
    while (1) {
      HAL_watchdog_refresh();
      res = f_read(&fp, buffer, copysize, &rd_cnt);
      card.write(buffer, rd_cnt);
      if (rd_cnt == 0) break;
      // 绘制进度条
      size_r += rd_cnt;
      if(i*size_p < size_r){
        i += (copysize>size_p ? copysize/size_p : 1);
        for(;j<i;j++)
          GUI_DispString(120+j*8, 120, (const uint8_t *)"█");
      }
      // 绘制百分比
      sprintf_P((char*)str, "%d%%", (size_r*100/fsize));
      GUI_DispString(220, 155, str);
    }
    
    delete [] buffer;
    f_close(&fp);
    card.closefile();
}
  #endif

void gocdeListDrawUdisk(void)
{
  memset(titleUdisk, 0, sizeof(titleUdisk));
  strncpy(titleUdisk, gUdiskPath, sizeof(titleUdisk));
  Scroll_CreatePara(&titleScrollUdisk, (uint8_t* )titleUdisk, &titleRectUdisk);
  printItemsUdisk.title.address = (uint8_t* )titleUdisk;
  GUI_SetBkColor(TITLE_COLOR);
  GUI_ClearPrect(&titleRectUdisk);
  GUI_SetBkColor(BK_COLOR);

  uint8_t i = 0;
  udisk_cwd_fcount =  udisk_file_count_dir_and_gcode(gUdiskPath);
  rtt.printf("found udisk cnt %d\n", udisk_cwd_fcount);
  for(i=0; (i + curPageUdisk * NUM_PER_PAGE < udisk_cwd_fcount)
          && (i < NUM_PER_PAGE); i++)                  // folder
  {
    if (!udisk_seek(i + curPageUdisk * NUM_PER_PAGE)) continue;
    if (workFileinfo.fattrib & AM_DIR) 
      printItemsUdisk.items[i].icon = CHAR_FOLDER;
    else 
      printItemsUdisk.items[i].icon = CHAR_FILE;
    printItemsUdisk.items[i].label = (uint8_t* )workFileinfo.fname;
    menuDrawListItem(&printItemsUdisk.items[i], i);
  }

  for(; (i<NUM_PER_PAGE); i++)			//background
  {
    printItemsUdisk.items[i].icon = NULL;
    menuDrawListItem(&printItemsUdisk.items[i], i);
  }
}

const int16_t labelVolumeError[] = {LABEL_READ_TFTSD_ERROR, LABEL_READ_U_DISK_ERROR, LABEL_READ_ONBOARDSD_ERROR};

void menuCallBackPrintUdisk(void)
{
  static bool lock = false;
  uint8_t update = 0;
  KEY_VALUES key_num = menuKeyGetValue();

  GUI_SetBkColor(TITLE_COLOR);
  Scroll_DispString(&titleScrollUdisk, LEFT);    //
  GUI_SetBkColor(BK_COLOR);
  #ifndef GCODE_LIST_MODE
    Scroll_DispString(&gcodeScrollUdisk, CENTER); //
  #endif

  if (lock) return;
  lock = true;
  switch(key_num)
  {
    case KEY_ICON_5:			
      if(curPageUdisk > 0)
      {
        curPageUdisk--;
        update=1;
      }
      break;

    case KEY_ICON_6:	
      if(curPageUdisk+1 < (udisk_cwd_fcount+(NUM_PER_PAGE-1))/NUM_PER_PAGE)
      {
        curPageUdisk++;
        update=1;
      }
      break;

    case KEY_ICON_7:
      curPageUdisk = 0;
      if(udisk_at_root)
      {
       // clearInfoFile();//如果是根目录就退回到UI
        infoMenu.cur--;
        break;
      }
      else
      {
        ExitDirUdisk(); //返回上级目录
        // filelistUdisk.upDir();
        f_opendir(&workDir, gUdiskPath);
        // f_getcwd(gUdiskPath, sizeof(gUdiskPath));
        if (gUdiskPath[0] == 0) {
          udisk_at_root = true;
        }
        update = 1;
      }
      break;

    case KEY_IDLE:
      break;

    default:                   
      if(key_num <= KEY_ICON_4)
      {
        if (udisk_seek(curPageUdisk * NUM_PER_PAGE + key_num))
        {
          if (workFileinfo.fattrib & AM_DIR) {
            EnterDirUdisk(workFileinfo.fname);
            if(FR_OK != f_opendir(&workDir, gUdiskPath)) break;
            update = 1;
            curPageUdisk = 0;
            udisk_at_root = false;
          } else { //gcode
            // rtt.printf("FIXME: print:%s\n", workFileinfo.fname);
           #if 0
            
            //card use short filename
            if (!card.getroot().exists(workFileinfo.altname)) {
              // copy a file to sd card. FIXME: check if filesize = 0.
              copy_file_to_sdcard(workFileinfo.altname);
              ExtUI::printFile(workFileinfo.altname);
            }
            else {
              ExtUI::printFile(workFileinfo.altname);
            }
           #elif 0
            if(can_print_flag){ // 如果可以打印就开始打印，否则跳过
            
              #ifdef AUTO_BED_LEVELING_BILINEAR
                set_bed_leveling_enabled(true);
              #endif
              #if ENABLED(FILAMENT_RUNOUT_SENSOR)
                ExtUI::setFilamentRunoutState(false);   // 重置断料检测
              #endif

              memset(filePath, 0, sizeof(filePath));
              sprintf_P(filePath, "%s/%s", gUdiskPath, workFileinfo.altname);
              f_open(&udisk_fp, filePath,  FA_READ | FA_OPEN_ALWAYS);
              // print_job_timer.reset();
              // print_job_timer.start();
              // UDiskPrintSize = 0;
              // udisk.setFileSize();
              UDiskPrint = true;UDiskPrintFinish = false;UDiskStopPrint=false;
              udisk.startUdiskPrint(workFileinfo.fsize);
              // infoMenu.menu[++infoMenu.cur] = menuPrinting;
              queue.enqueue_now_P("M24\n");
            }
           #else
            if(can_print_flag){ // 如果可以打印就开始打印，否则跳过
              if(LongTimeTouch)
                infoMenu.menu[++infoMenu.cur] = menuDeleteUdiskFile;
              else
                infoMenu.menu[++infoMenu.cur] = menuWhetherToPtintUdisk;
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
    gocdeListDrawUdisk();
  }
  
  // if(!IS_SD_INSERTED())
  // {
  //  // resetInfoFile();
  //   infoMenu.cur--;
  // }
  lock = false;
}
// menuPrintFromSource
void menuPrintUdisk(void)
{
  GUI_Clear(BK_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL, textSelect(LABEL_LOADING));
  if (udisk.usbIsReady())
  {
    filelistUdisk.refresh();
    for(uint8_t i = 0; i < NUM_PER_PAGE; i++) printItemsUdisk.items[i].icon = NULL;
    menuDrawListPage(&printItemsUdisk);
    gocdeListDrawUdisk();
    menuSetFrontCallBack(menuCallBackPrintUdisk);
  }
  else
  {
    GUI_DispStringInRect(0, 0, LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL, textSelect(LABEL_READ_U_DISK_ERROR));
    ExtUI::delay_ms(1000);
    infoMenu.cur--;
  }
}

void menuCallBackWhetherToPtintUdisk()
{
  uint16_t key_num = KEY_GetValue(2, doubleBtnRect);
  switch(key_num)
  {
    case KEY_POPUP_CONFIRM:
      #ifdef AUTO_BED_LEVELING_BILINEAR
        set_bed_leveling_enabled(true);
      #endif
      #if ENABLED(FILAMENT_RUNOUT_SENSOR)
        ExtUI::setFilamentRunoutState(false);   // 重置断料检测
      #endif

      memset(filePath, 0, sizeof(filePath));
      sprintf_P(filePath, "%s/%s", gUdiskPath, workFileinfo.altname);
      udisk.openFileRead(filePath);
      queue.enqueue_now_P("M24\n");
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
void menuWhetherToPtintUdisk()
{
  uint8_t printText[128];
  sprintf_P((char*)printText, "%s \n%s ?", textSelect(LABEL_PRINT), workFileinfo.fname);
  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_PRINT), printText, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
  menuSetFrontCallBack(menuCallBackWhetherToPtintUdisk);
}
void menuCallBackDeleteUdiskFile()
{
  uint16_t key_num = KEY_GetValue(2, doubleBtnRect);
  switch(key_num)
  {
    case KEY_POPUP_CONFIRM:
      memset(filePath, 0, sizeof(filePath));
      sprintf_P(filePath, "%s/%s", gUdiskPath, workFileinfo.altname);
      f_unlink(filePath);
      udisk.ResetPin();
      infoMenu.cur--;
      break;
    case KEY_POPUP_CANCEL:
      infoMenu.cur--;
      break;
    default:break;
  }
}
void menuDeleteUdiskFile()
{
  uint8_t printText[128];
  sprintf_P((char*)printText, "Delete file \n%s ?", workFileinfo.fname);
  changeSOF(false);
  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_WARNING), printText, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
  menuSetFrontCallBack(menuCallBackDeleteUdiskFile);
}

#endif
