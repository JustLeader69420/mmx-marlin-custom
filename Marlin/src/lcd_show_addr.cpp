#include "lcd/extui/lib/tsc/TSC_Menu.h"
// #include <stdio.h>
#include "lcd_show_addr.h"

#if ENABLED(USART_LCD)

STATUS_MSG statusMsg_P = {0};
static uint8_t old_language = 0;
void addHeadMess(char *str){
    str[0] = SEND_HEAD1;
    str[1] = SEND_HEAD2;
    str[3] = SEND_WRITE;
}
void Init_usart_lcd(void){
    char send_str[8] = {0};
}

void Init_send_str(char *str, int len){
    for(int i; i<len; i++){
        str[i] = 0x00;i++;
        str[i] = 0x20;
    }
}
void Updata_usart_lcd(void){
    STATUS_MSG tempMsg;
    statusMsg_GetCurMsg(&tempMsg);
    // 普通时刻一直获取xyz坐标
    if(!(isPrinting())){
        if (statusMsg_P.x != tempMsg.x) {
            delay(10);
            statusMsg_P.x = tempMsg.x;
            char dwin_send_x[16] = {0};
            memset(dwin_send_x, 0, sizeof(dwin_send_x));
            int tagX = (int)(tempMsg.x * 10);
            addHeadMess(dwin_send_x);
            dwin_send_x[4] = MOVE_X_TEXT >> 8;              dwin_send_x[5] = MOVE_X_TEXT & 0x00ff;
            sprintf_P(&dwin_send_x[6], "%d.%d", tagX/10, tagX % 10);
            dwin_send_x[2] = 6+3;
            send_hexPGM(dwin_send_x, dwin_send_x[2]+3);
        }
        if (statusMsg_P.y != tempMsg.y) {
            delay(10);
            statusMsg_P.y = tempMsg.y;
            char dwin_send_y[16] = {0};
            memset(dwin_send_y, 0, sizeof(dwin_send_y));
            int tagY = (int)(tempMsg.y * 10);
            addHeadMess(dwin_send_y);
            dwin_send_y[4] = MOVE_Y_TEXT >> 8;              dwin_send_y[5] = MOVE_Y_TEXT & 0x00ff;
            sprintf_P(&dwin_send_y[6], "%d.%d", tagY/10, tagY%10);
            dwin_send_y[2] = 6+3;
            send_hexPGM(dwin_send_y, dwin_send_y[2]+3);
        }
        if (statusMsg_P.z != tempMsg.z) {
            delay(10);
            statusMsg_P.z = tempMsg.z;
            char dwin_send_z[16] = {0};
            memset(dwin_send_z, 0, sizeof(dwin_send_z));
            int tagZ = (int)(tempMsg.z * 10);
            addHeadMess(dwin_send_z);
            dwin_send_z[4] = MOVE_Z_TEXT >> 8;              dwin_send_z[5] = MOVE_Z_TEXT & 0x00ff;
            sprintf_P(&dwin_send_z[6], "%d.%d", tagZ/10, tagZ%10);
            dwin_send_z[2] = 6+3;
            send_hexPGM(dwin_send_z, dwin_send_z[2]+3);
        }
        if (statusMsg_P.e0 != tempMsg.e0) {
            delay(10);
            statusMsg_P.e0 = tempMsg.e0;
            char dwin_send_e[8] = {0};
            short tagE = (short)(tempMsg.e0);
            addHeadMess(dwin_send_e);
            dwin_send_e[2] = 5;
            dwin_send_e[4] = EXTRUDE_LEN >> 8;              dwin_send_e[5] = EXTRUDE_LEN & 0x00ff;
            dwin_send_e[6] = (tagE >>  8) & 0xff;           dwin_send_e[7] = tagE & 0xff;
            send_hexPGM(dwin_send_e, 8);
        }
        // if (statusMsg_P.babystep_v != tempMsg.babystep_v) {
        //     delay(10);
        //     statusMsg_P.babystep_v = tempMsg.babystep_v;
        //     char dwin_send_b[16] = {0};
        //     int tagB;
        //     addHeadMess(dwin_send_b);
        //     dwin_send_b[4] = BABYSTEP_NUM >> 8;              dwin_send_b[5] = BABYSTEP_NUM & 0xff;
        //     if(tempMsg.leveling_v>=0){
        //         tagB = (int)(tempMsg.babystep_v*100+0.5);
        //         sprintf(&dwin_send_b[6], "%d.%d%d", tagB/100, tagB/10%10, tagB%10);
        //     }else{
        //         tagB = -(int)(tempMsg.leveling_v*100-0.5);
        //         sprintf(&dwin_send_b[6], "-%d.%d%d", tagB/100, tagB/10%10, tagB%10);
        //     }
        //     dwin_send_b[2] = strlen(&dwin_send_b[6]) + 3;
        //     send_hexPGM(dwin_send_b, dwin_send_b[2]+3);
        // }
        if (statusMsg_P.leveling_v != tempMsg.leveling_v) {
            delay(10);
            statusMsg_P.leveling_v = tempMsg.leveling_v;
            char dwin_send_l[16] = {0}; int tagL;
            addHeadMess(dwin_send_l);
            dwin_send_l[4] = ZOFFSET_NUM >> 8;              dwin_send_l[5] = ZOFFSET_NUM & 0xff;
            
            if(tempMsg.leveling_v>=0){
                tagL = (int)(tempMsg.leveling_v*100+0.5);
                sprintf(&dwin_send_l[6], "%d.%d%d", tagL/100, tagL/10%10, tagL%10);
            }
            else{
                tagL = -(int)(tempMsg.leveling_v*100-0.5);
                sprintf(&dwin_send_l[6], "-%d.%d%d", tagL/100, tagL/10%10, tagL%10);
            }
            dwin_send_l[2] = strlen(&dwin_send_l[6]) + 3;
            send_hexPGM(dwin_send_l, dwin_send_l[2]+3);
        }
      #if 0
        if (old_language != infoSettings.language){
            old_language = infoSettings.language;
            char dwin_send_lan[64] = {0};
            addHeadMess(dwin_send_lan);
            dwin_send_lan[4] = INFO_1 >> 8;     dwin_send_lan[5] = INFO_1 & 0xff;
            // dwin_send_lan[2] = 23;
            // Init_send_str(&dwin_send_lan[6], 20);
            // send_hexPGM(dwin_send_lan, 26);
            // delay(10);
            sprintf_P(&dwin_send_lan[6], "%s", textSelect(LABEL_LANGUAGE));
            dwin_send_lan[2] = 43; //strlen(&dwin_send_lan[6]) + 3;
            send_hexPGM(dwin_send_lan, dwin_send_lan[2]+3);
            delay(10);

            memset(dwin_send_lan, 0, sizeof(dwin_send_lan));
            addHeadMess(dwin_send_lan);
            dwin_send_lan[4] = INFO_2 >> 8;     dwin_send_lan[5] = INFO_2 & 0xff;
            // dwin_send_lan[2] = 23;
            // Init_send_str(&dwin_send_lan[6], 20);
            // send_hexPGM(dwin_send_lan, 26);
            // delay(10);
            sprintf_P(&dwin_send_lan[6], "%s", textSelect(LABEL_POWER_FAILED));
            dwin_send_lan[2] = 43; //strlen(&dwin_send_lan[6]) + 3;
            send_hexPGM(dwin_send_lan, dwin_send_lan[2]+3);
        }
      #endif
    }

    // 普通或打印时每隔1s获取的数据
    static uint32_t now_time_ms = 0;
    uint32_t get_time = millis();
    if (get_time < (1000 + now_time_ms)) { // Refresh per 1 sec
        return;
    }
    now_time_ms = get_time;
    // SERIAL_ECHOLN("1s\n");
    // 打印时获取时间和坐标
    PORT_REDIRECT(SERIAL_BOTH);
    if(isPrinting()){ 
        send_time(getPrintTime());
        // 获取xyz坐标
        if (statusMsg_P.x != tempMsg.x) {
            delay(10);
            statusMsg_P.x = tempMsg.x;
            char dwin_send_x[16] = {0};
            memset(dwin_send_x, 0, sizeof(dwin_send_x));
            int tagX = (int)(tempMsg.x * 10);
            addHeadMess(dwin_send_x);
            dwin_send_x[4] = MOVE_X_TEXT >> 8;              dwin_send_x[5] = MOVE_X_TEXT & 0x00ff;
            sprintf_P(&dwin_send_x[6], "%d.%d", tagX/10, tagX%10);
            dwin_send_x[2] = 6+3;
            send_hexPGM(dwin_send_x, dwin_send_x[2]+3);
        }
        if (statusMsg_P.y != tempMsg.y) {
            delay(10);
            statusMsg_P.y = tempMsg.y;
            char dwin_send_y[16] = {0};
            memset(dwin_send_y, 0, sizeof(dwin_send_y));
            int tagY = (int)(tempMsg.y * 10);
            addHeadMess(dwin_send_y);
            dwin_send_y[4] = MOVE_Y_TEXT >> 8;              dwin_send_y[5] = MOVE_Y_TEXT & 0x00ff;
            sprintf_P(&dwin_send_y[6], "%d.%d", tagY/10, tagY%10);
            dwin_send_y[2] = 6+3;
            send_hexPGM(dwin_send_y, dwin_send_y[2]+3);
        }
        if (statusMsg_P.z != tempMsg.z) {
            delay(10);
            statusMsg_P.z = tempMsg.z;
            char dwin_send_z[16] = {0};
            memset(dwin_send_z, 0, sizeof(dwin_send_z));
            int tagZ = (int)(tempMsg.z * 10);
            addHeadMess(dwin_send_z);
            dwin_send_z[4] = MOVE_Z_TEXT >> 8;              dwin_send_z[5] = MOVE_Z_TEXT & 0x00ff;
            sprintf_P(&dwin_send_z[6], "%d.%d", tagZ/10, tagZ%10);
            dwin_send_z[2] = 6+3;
            send_hexPGM(dwin_send_z, dwin_send_z[2]+3);
        }
        // if (statusMsg_P.babystep_v != tempMsg.babystep_v) {
        //     delay(10);
        //     statusMsg_P.babystep_v = tempMsg.babystep_v;
        //     char dwin_send_b[16] = {0};
        //     int tagB = tempMsg.babystep_v*100;
        //     addHeadMess(dwin_send_b);
        //     dwin_send_b[4] = BABYSTEP_NUM >> 8;              dwin_send_b[5] = BABYSTEP_NUM & 0xff;
        //     sprintf(&dwin_send_b[6], "%d.%d%d", tagB/100, tagB/10%10, tagB%10);
        //     dwin_send_b[2] = 5 + 3;
        //     send_hexPGM(dwin_send_b, dwin_send_b[2]+3);
        // }
        if (statusMsg_P.progress != tempMsg.progress) {
            statusMsg_P.progress = tempMsg.progress;
            char dwin_send_p[12] = {0};
            addHeadMess(dwin_send_p);
            dwin_send_p[2] = 0x05;
            dwin_send_p[4] = PROGRESS_ADDR >> 8;        dwin_send_p[5] = PROGRESS_ADDR & 0xff;
            dwin_send_p[6] = tempMsg.progress >> 8;     dwin_send_p[7] = tempMsg.progress & 0xff;
            send_hexPGM(dwin_send_p, 8);
            delay(20);
            dwin_send_p[4] = PROGRESS_TEXT >> 8;        dwin_send_p[5] = PROGRESS_TEXT & 0xff;
            sprintf_P(&dwin_send_p[6] ,"%d%%", tempMsg.progress);
            dwin_send_p[2] = 4+3;
            send_hexPGM(dwin_send_p, 10);
        }
        if (statusMsg_P.e0 != tempMsg.e0) {
            delay(10);
            statusMsg_P.e0 = tempMsg.e0;
            char dwin_send_e[8] = {0};
            short tagE = (short)(tempMsg.e0);
            addHeadMess(dwin_send_e);
            dwin_send_e[2] = 5;
            dwin_send_e[4] = EXTRUDE_LEN >> 8;              dwin_send_e[5] = EXTRUDE_LEN & 0x00ff;
            dwin_send_e[6] = (tagE >>  8) & 0xff;           dwin_send_e[7] = tagE & 0xff;
            send_hexPGM(dwin_send_e, 8);
        }
    }

    // 获取当前温度
    if (statusMsg_P.actHotend != tempMsg.actHotend) {
        delay(10);
        statusMsg_P.actHotend = tempMsg.actHotend;
        char dwin_send_H[8] = {0};
        int actH = (int)tempMsg.actHotend;
        addHeadMess(dwin_send_H);
        dwin_send_H[2] = 0x05;
        dwin_send_H[4] = HEAT_HOTEND_CURRENT_TEMP >> 8; dwin_send_H[5] = HEAT_HOTEND_CURRENT_TEMP & 0x00ff;
        dwin_send_H[6] = actH>>8;                       dwin_send_H[7] = actH & 0xFF;
        send_hexPGM(dwin_send_H, 8);
    }
    if (statusMsg_P.actBed != tempMsg.actBed) {
        delay(10);
        statusMsg_P.actBed = tempMsg.actBed;
        char dwin_send_B[8] = {0};
        int actB = (int)tempMsg.actBed;
        addHeadMess(dwin_send_B);
        dwin_send_B[2] = 0x05;
        dwin_send_B[4] = HEAT_BED_CURRENT_TEMP >> 8;    dwin_send_B[5] = HEAT_BED_CURRENT_TEMP & 0x00ff;
        dwin_send_B[6] = actB>>8;                       dwin_send_B[7] = actB & 0xFF;
        send_hexPGM(dwin_send_B, 8);
    }

    // 获取目标温度
    if (statusMsg_P.tagHotend != tempMsg.tagHotend) {
        delay(10);
        statusMsg_P.tagHotend = tempMsg.tagHotend;
        char dwin_send_TH[8] = {0};
        int tagH = (int)tempMsg.tagHotend;
        addHeadMess(dwin_send_TH);
        dwin_send_TH[2] = 0x05;
        dwin_send_TH[4] = HEAT_HOTEND_TARGET_TEMP >> 8; dwin_send_TH[5] = HEAT_HOTEND_TARGET_TEMP & 0x00ff;
        dwin_send_TH[6] = tagH>>8;                      dwin_send_TH[7] = tagH & 0xFF;
        send_hexPGM(dwin_send_TH, 8);
    }
    if (statusMsg_P.tagBed != tempMsg.tagBed) {
        delay(10);
        statusMsg_P.tagBed = tempMsg.tagBed;
        char dwin_send_TB[8] = {0};
        int tagB = (int)tempMsg.tagBed;
        addHeadMess(dwin_send_TB);
        dwin_send_TB[2] = 0x05;
        dwin_send_TB[4] = HEAT_BED_TARGET_TEMP >> 8;    dwin_send_TB[5] = HEAT_BED_TARGET_TEMP & 0x00ff;
        dwin_send_TB[6] = tagB>>8;                      dwin_send_TB[7] = tagB & 0xFF;
        send_hexPGM(dwin_send_TB, 8);
    }
}

void usart_lcd_stop_printf(){
    TERN_( HAS_UDISK, udisk.abortUdiskPrint(); )
    ExtUI::stopPrint();
    // set_bed_leveling_enabled(false);
}

void resumeAndPauseIcon(bool is_pause)
{
  if(infoMenu.menu[infoMenu.cur] != menuPrinting) return;
  char send_Pause[8] = {0x5A, 0xA5, 0x05, 0x82, 0x2B, 0x00, 0x00, 0x01};
  char send_Resume[8] = {0x5A, 0xA5, 0x05, 0x82, 0x2B, 0x00, 0x00, 0x00};
  PORT_REDIRECT(SERIAL_BOTH);
  if(is_pause)
    send_hexPGM(send_Pause, 8);
  else
    send_hexPGM(send_Resume, 8);
}

#endif
