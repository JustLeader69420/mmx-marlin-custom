#include "udiskPrint.h"

#ifdef HAS_UDISK

#include "stm32_usb.h"
#include "../module/planner.h"
#include "../module/printcounter.h"
#include "../sd/cardreader.h"
#include "../feature/powerloss.h"

UdiskPrint udisk;
static FIL udisk_fp;
uint64_t UdiskPrint::fsize=0;  // 文件大小
uint64_t UdiskPrint::psize=0;  // 已经读取到的文件大小
    int  UdiskPrint::r_size=0;    // 存放读取到的文件大小
uint16_t UdiskPrint::fileNum=0;// 遍历到的文件数量
    bool UdiskPrint::udiskFlag=false;
    bool UdiskPrint::udiskPause=false;
    bool UdiskPrint::udiskStop=false;

char UdiskPrint::get(void){
	char a = 0;
	UINT n = 0;
	FRESULT res;

	res = f_read(&udisk_fp, &a, 1, &n);
	if((n!=1) || (res!=FR_OK)) a = -1;
	else psize++;

	return a;
}
void UdiskPrint::fileHasFinish(){
	static uint8_t udisk_finish_lock = false;

	if(udisk_finish_lock) return;

	udisk_finish_lock = true;
  planner.synchronize();    // 等待队列完成
  f_close(&udisk_fp);       // 关闭U盘文件
  udiskFlag = false;

  marlin_state = MF_SD_COMPLETE;
	udisk_finish_lock = false;
}
void UdiskPrint::pauseUdiskPrint(void){
	if(udiskFlag)
			udiskPause = true;
}
void UdiskPrint::resumeUdiskPrint(void){
	if(udiskFlag)
			udiskPause = false;
}
void UdiskPrint::abortUdiskPrint(){
  if(udiskFlag){
    udiskFlag = false;
    udiskStop = true;
  }
  //f_close(fp);              // 关闭U盘文件
}
void UdiskPrint::startUdiskPrint(uint64_t _size){
  udiskFlag = true; udiskPause = false; udiskStop = false;
  psize = 0; r_size = 0; fsize = _size;
  
  print_job_timer.start();
}
// void UdiskPrint::resumeUdiskPrint(uint64_t _size, uint32_t _psize, millis_t resume_time){
//   udiskFlag = true;psize=_psize;r_size=0;fsize = _size;
  
//   print_job_timer.resume(resume_time);
// }
uint8_t UdiskPrint::getPrintProgress(void){
  return psize*100/fsize;
}
bool UdiskPrint::usbIsReady(void){
  return (usb_is_ready());
}

char *fileSuffix(char *fname){
	char *p = strtok(fname,".");
	char *q = p;
	while(p != NULL){
    q = p;
    p = strtok(NULL, ".");
	}
	return q;
}
void UdiskPrint::ls(char *path, uint8_t layer){
	FRESULT res = FR_OK;
	FILINFO fno;
	DIR dir;
	char *fn;
	char filename[256] = {0};
	fileNum = 0;

	res = f_opendir(&dir, path);
	if (res != FR_OK) return;
	while(1){
    res = f_readdir(&dir, &fno);
    // 读取失败或者读到的为空
    if(res != FR_OK || fno.fname[0] == 0)
    {
      break;
    }
    // 读取到隐藏文件
    if((fno.fname[0] == '.') && ((fno.fattrib & AM_HID) == AM_HID))
    {
      continue;
    }
    // 如果是普通gcode文件或者文件夹
    if(((fno.fattrib & AM_DIR) == AM_DIR)){
      memset(filename, 0, sizeof(filename));
      if(layer)
        sprintf_P(filename, "%s/%s", path, fno.fname);
      else
        sprintf_P(filename, "/%s", fno.fname);
      // SERIAL_ECHO(filename);
      // SERIAL_EOL();
      ls(filename, layer+1);
    }
    else
    {
      // 获取最后的后缀名，复制是为了不改变源字符串
      char copyname[256] = {0};
      strcpy(copyname, fno.fname);
      char *p = fileSuffix(copyname);
      // 当后缀名不为“gcode”跳过
      if(strcmp(p, "gcode") != 0) continue;
      if(layer){
        memset(filename, 0, sizeof(filename));
        sprintf_P(filename, "%s/%s", path, fno.fname);
        SERIAL_ECHO(filename);
      }
      else
        SERIAL_ECHO(fno.fname);
      SERIAL_CHAR('^');
      SERIAL_ECHOLN((uint32_t)fno.fsize);
      // memset(filename, 0, sizeof(filename));
      // sprintf_P(filename, "%s %ld", fno.fname, fno.fsize);
      // SERIAL_ECHOLNPGM_P(filename);
    }
	}
	f_closedir(&dir);
}

void UdiskPrint::openFileRead(const char * const path) {
	if (!usb_is_ready()) return;
	// delay(50);
	FILINFO fno;
	FRESULT res = f_stat(path, &fno);
	announceOpen(2, fno.altname);
	res = f_open(&udisk_fp, path, FA_READ);
	if(res == FR_OK)
	{ 
    // Don't remove this block, as the PORT_REDIRECT is a RAII
    udisk.init();                               // 初始化数据
    udisk.startUdiskPrint(fno.fsize);           // 开始U盘打印
    // PORT_REDIRECT(SerialMask::All);
    SERIAL_ECHOLNPAIR(STR_SD_FILE_OPENED, fno.altname, STR_SD_SIZE, (uint32_t)fno.fsize);
    SERIAL_ECHOLNPGM(STR_SD_FILE_SELECTED);
    TERN_(POWER_LOSS_RECOVERY, recovery.prepare_u(path));
	}
	else
    SERIAL_ECHOLNPAIR(STR_SD_OPEN_FILE_FAIL, path, ".");
			
	delay(50);
}
void UdiskPrint::resumeUdiskPrint(const char * const path, uint32_t _psize, millis_t resume_time){

	if (!usb_is_ready()) return;

	FILINFO fno;
	FRESULT res = f_stat(path, &fno);
	res = f_open(&udisk_fp, path, FA_READ);
	if(res != FR_OK)    return;

	udiskFlag = true;   udiskPause = false; udiskStop = false;
	psize = _psize;     r_size = 0;
	fsize = fno.fsize;
	{
    // PORT_REDIRECT(SerialMask::All);
    SERIAL_ECHOLNPAIR(STR_SD_FILE_OPENED, fno.altname, STR_SD_SIZE, (uint32_t)fno.fsize);
    SERIAL_ECHOLNPGM(STR_SD_FILE_SELECTED);
	}
	res = f_lseek(&udisk_fp, _psize);
	if (res == FR_OK) SERIAL_ECHOLNPAIR("the_recover:", _psize);
	else SERIAL_ECHOLNPAIR("huifuShibai:", _psize);
	// print_job_timer.resume(resume_time);
}
void UdiskPrint::closeFile(){
  f_close(&udisk_fp);
}

#endif
