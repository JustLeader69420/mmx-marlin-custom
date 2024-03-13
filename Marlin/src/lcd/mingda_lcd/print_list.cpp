#include "../../inc/MarlinConfig.h"

#if ENABLED(USART_LCD)

#include "print_list.h"
#include "../../lcd_show_addr.h"
#include "../../sd/cardreader.h"
#include "../extui/ui_api.h"
#include "../../gcode/queue.h"
#include "../extui/lib/tsc/TSC_Menu.h"

SdFile rootDir;
static SdVolume volume;
static ExtUI::FileList filelist;

char dwinCurPage = 0;
char print_list[6][128] = {0};
char dwin_path[128] = {0};

void send_text_msg(void){
    char send_str[] = {0x5A, 0xA5, 0x03, 0x82, 0x00, 0x00};
    send_hexPGM(send_str, 6);
}

void get_sdcard_list(void){
    if (!(card.flag.mounted)) return;
    int i=0;    dir_t p; char a = 0;
    char send_str[128] = {0};
    char send_str2[8] = {0};
    send_text_msg();
    delay(50);
    for(i=0;(i < LIST_LINE); i++)// folder
    {
        if (!filelist.seek(i + dwinCurPage * LIST_LINE)){
            memset(send_str, 0, sizeof(send_str));
            addHeadMess(send_str);
            send_str[2] = 0x22+3;
            send_str[4] = (LIST_VP_1 + (i*0x100)) >> 8;         send_str[5] = (LIST_VP_1 + (i*0x100)) & 0xFF;
            // SERIAL_ECHO(filelist.filename());
            send_hexPGM(send_str, 0x22+3+3);
            delay(20);
            continue;
        }
        if (filelist.isDir()){
            addHeadMess(send_str2);
            send_str2[2] = 0x05;
            send_str2[4] = (LIST_COLOR_1 + (i*0x10)) >> 8;  send_str2[5] = (LIST_COLOR_1 + (i*0x10)) & 0xFF;
            send_str2[6] = DWIN_COLOR_ORANGE >> 8;          send_str2[7] = DWIN_COLOR_ORANGE & 0xFF;
            send_hexPGM(send_str2, 8);
        }
        else{
            addHeadMess(send_str2);
            send_str2[2] = 0x05;
            send_str2[4] = (LIST_COLOR_1 + (i*0x10)) >> 8;  send_str2[5] = (LIST_COLOR_1 + (i*0x10)) & 0xFF;
            send_str2[6] = DWIN_COLOR_WHITE >> 8;           send_str2[7] = DWIN_COLOR_WHITE & 0xFF;
            send_hexPGM(send_str2, 8);
        }
        delay(20);
        memset(send_str, 0, sizeof(send_str));
        addHeadMess(send_str);
        send_str[2] = 0x22+3;
        send_str[4] = (LIST_VP_1 + (i*0x100)) >> 8;         send_str[5] = (LIST_VP_1 + (i*0x100)) & 0xFF;
        strcat(&send_str[6], filelist.filename());
        // SERIAL_ECHO(filelist.filename());
        send_hexPGM(send_str, 0x22+3+3);
        delay(20);
    }
}
uint16_t read_filenum(void){
    return filelist.count();
}

bool lcdEnterDir(const char *nextdir){
    char send_str[128] = {0};
    
    strcat(dwin_path, "/");
    strcat(dwin_path, nextdir);

    memset(send_str, 0, sizeof(send_str));
    addHeadMess(send_str);

    send_str[2] = 0x22+3;
    send_str[4] = (PL_HEAD_TEXT) >> 8;         send_str[5] = (PL_HEAD_TEXT) & 0xFF;
    strcat(&send_str[6], dwin_path);
    send_hexPGM(send_str, 0x22+6);

    return 1;
}

void lcdExitDir(){
    if (filelist.isAtRootDir()) {  // 根目录
        char send_str[10] = {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x0D};
        send_hexPGM(send_str, 10);
    } else {                       // 非根目录
        filelist.upDir();

        int i = strlen(dwin_path);
        for(; i > 0 && dwin_path[i] != '/';i--){	
            dwin_path[i]=0; // 找到上一个目录
        }
        dwin_path[i]=0;

        dwinCurPage = 0;
        get_sdcard_list();
    }
    char send_str[128] = {0};

    memset(send_str, 0, sizeof(send_str));
    addHeadMess(send_str);

    send_str[2] = 0x22+3;
    send_str[4] = (PL_HEAD_TEXT) >> 8;         send_str[5] = (PL_HEAD_TEXT) & 0xFF;
    strcat(&send_str[6], dwin_path);
    send_hexPGM(send_str, 0x22+6);
}

void select_file(int i){
    char DWIN_M23[256] = {0};
    filelist.seek(i + dwinCurPage * LIST_LINE);
    
    if (filelist.isDir()) {
        if(lcdEnterDir(filelist.shortFilename()));
        dwinCurPage = 0;
        filelist.changeDir(filelist.shortFilename());
        get_sdcard_list();
    } else {
        if (!(card.flag.mounted)) return;
        // card.openAndPrintFile(filelist.shortFilename());
        // send_hexPGM(DWIN_M23, strlen(DWIN_M23));
        if(can_print_flag){ // 如果可以打印就开始打印，否则跳过
            #ifdef AUTO_BED_LEVELING_BILINEAR
                set_bed_leveling_enabled(true);
            #endif
            
            #if ENABLED(FILAMENT_RUNOUT_SENSOR)
                ExtUI::setFilamentRunoutState(false);   // 重置断料检测
            #endif

            ExtUI::printFile(filelist.shortFilename());
        }
    }
}

#endif
