#ifndef __UDISKP_
#define __UDISKP_

#include "../inc/MarlinConfig.h"

#ifdef HAS_UDISK

#include "ff.h"
class UdiskPrint{

public:
    static bool eof(void){return psize>=fsize;}
    // static int get(TCHAR *rbuf, int len, FIL *fp){
    //     r_size = f_gets_p(rbuf, len, fp);
    //     psize += r_size;
    //     return r_size;
    // }
    static char get(void);
    static uint64_t getPrintSize(){return psize;}
    static uint64_t getFileSize(){return fsize;}
    static void init(){fsize=0;psize=0;r_size=0;udiskFlag=false;udiskPause=false;udiskStop=false;}
    static void fileHasFinish();
    static bool isUdiskPrint(){return udiskFlag;}
    static bool isUdiskPause(){return udiskPause;}
    static bool isUdiskStop() {return udiskStop;}
    static void startUdiskPrint(uint64_t _size);
    // static void resumeUdiskPrint(uint64_t _size, uint32_t _psize, millis_t resume_time);
    static void setFileSize(uint64_t _size){fsize = _size;}
    static uint8_t getPrintProgress(void);
    static void pauseUdiskPrint(void);
    static void resumeUdiskPrint(void);
    static void abortUdiskPrint();
    static void closeFile(void);
    static bool usbIsReady(void);
    static void ls(char *path, uint8_t layer);
    static void openFileRead(const char * const path);
    static void resumeUdiskPrint(const char * const path, uint32_t _psize, millis_t resume_time);
  #ifdef USB_CONNECT_PIN
    static void InitUdiskPin(){OUT_WRITE(USB_CONNECT_PIN,LOW); delay(10); WRITE(USB_CONNECT_PIN,HIGH);}
    static void ResetPin(){WRITE(USB_CONNECT_PIN, LOW); delay(10); WRITE(USB_CONNECT_PIN, HIGH);}
  #endif
private:
    static uint64_t fsize;  // 文件大小
    static uint64_t psize;  // 已经读取到的文件大小
    static int r_size;      // 存放读取到的文件大小
    // static bool noRead = false;
    static bool udiskFlag;
    static bool udiskPause;
    static bool udiskStop;
    static uint16_t fileNum;
};

extern UdiskPrint udisk;

#endif

#endif //__UDISKP
