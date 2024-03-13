#ifndef LCD_SHOW_ADDR
#define LCD_SHOW_ADDR

#define SEND_HEAD1  0x5A
#define SEND_HEAD2  0xA5
#define SEND_WRITE  0x82
#define SEND_READ   0x83


// preheat
#define PREHEAT_HEAT_ADDR			(0x4000)
#define PREHEAT_BOTH_ADDR			(0x1150)
#define PREHEAT_BOTH_TEXT			(0x1130)
#define PREHEAT_PLA_H				(0x1152)
#define PREHEAT_PLA_B				(0x1154)
#define PREHEAT_ABS_H				(0x1156)
#define PREHEAT_ABS_B				(0x1158)
#define PREHEAT_TPU_H				(0x115a)
#define PREHEAT_TPU_B				(0x115c)
// heat
#define HEAT_HOTEND_ADDR			(0x4002)
#define HEAT_BED_ADDR				(0x4004)
#define HEAT_HOTEND_CURRENT_TEMP	(0x10F0)
#define HEAT_HOTEND_TARGET_TEMP		(0x10F4)
#define HEAT_BED_CURRENT_TEMP	    (0x10F6)
#define HEAT_BED_TARGET_TEMP	    (0x10FA)
#define HEAT_INCREMENT				(0x1276)
#define HEAT_INCREMENT_TEXT		    (0x1240)
#define HEAT_START_ADDR_H			(0x1278)
#define HEAT_START_TEXT_H			(0x1250)
#define HEAT_START_ADDR_B			(0x1500)
#define HEAT_START_TEXT_B			(0x1520)
// move
#define MOVE_ADDR					(0x4006)
#define MOVE_INCREMENT				(0x1706)
#define MOVE_INCREMENT_TEXT		    (0x1740)
#define MOVE_X_TEXT                 (0x2BC0)
#define MOVE_Y_TEXT                 (0x2CD0)
#define MOVE_Z_TEXT                 (0x2DE0)

// home
#define HOME_ADDR					(0x4008)
// Extrude
#define EXTRUDE_ADDR				(0x400a)
#define EXTRUDE_LEN					(0x190a)
#define EXTRUDE_SPEED				(0x1906)
#define EXTRUDE_INCREMENT			(0x1908)
#define EXTRUDE_INCREMENT_TEXT	    (0x1950)

// Fan
#define FAN_SHOW_PIC				(0x1A04)
#define FAN_ADDR					(0x400C)
#define FAN_LV						(0x1A06)
#define FAN_LV_TEXT					(0x1A40)
#define FAN_SPEED_NUM				(0x1A0A)

//zoffset
#define ZOFFSET_ADDR				(0x4010)
#define ZOFFSET_INCREMENT			(0x1C06)
#define ZOFFSET_INCREMENT_TEXT      (0x1C40)
#define ZOFFSET_NUM					(0x1C0A)
#define ZOFFSET_LEN					(0x1C70)

// PrintChoose
#define PC_ADDR						(0x4012)

/// PrintList
#define PL_ADDR						(0x4014)
#define PL_HEAD_TEXT                (0x2600)
#define LIST_VP_1                   (0x2000)
#define LIST_VP_2                   (0x2100)
#define LIST_VP_3                   (0x2200)
#define LIST_VP_4                   (0x2300)
#define LIST_VP_5                   (0x2400)
#define LIST_VP_6                   (0x2500)
#define LIST_COLOR_1                (0x8003)
#define LIST_COLOR_2                (0x8013)
#define LIST_COLOR_3                (0x8023)
#define LIST_COLOR_4                (0x8033)
#define LIST_COLOR_5                (0x8043)
#define LIST_COLOR_6                (0x8053)
#define LIST_NUM_VP(X)              LIST_VP_##X
#define LIST_NUM_COLOR(X)           LIST_COLOR_##X

#define DWIN_COLOR_BLACK            (0x0000)
#define DWIN_COLOR_WHITE            (0xFFFF)
#define DWIN_COLOR_ORANGE           (0xFC08)
#define DWIN_COLOR_RED              (0xF800)

// printing
#define PROGRESS_ADDR               (0x2BBC)
#define PROGRESS_TEXT               (0x2B90)
#define PRING_TIME                  (0x2BA0)
#define PRINTING_HEAD_TEXT          (0x2700)
#define PRINTING_TIME_FLAG          (0x2BBE)
#define PRINTING_PAUSE              (0x2B00)

// babystep
#define BABYSTEP_NUM                (0x2C20)

// Info
#define INFO_TITEL                  (0x3000)
#define INFO_CONFIRM                (0x3100)
#define INFO_BACK                   (0x3110)
#define INFO_WARNING                (0x3200)
#define INFO_MESSAGE                (0x3300)

#define INFO_TITEL_1                (0x8060)
#define INFO_CONFIRM_1              (0x8070)
#define INFO_BACK_1                 (0x8080)
#define INFO_MESSAGE_1              (0x8090)

void addHeadMess(char *str);
void Updata_usart_lcd(void);
void usart_lcd_stop_printf();
void resumeAndPauseIcon(bool is_pause);

#endif
