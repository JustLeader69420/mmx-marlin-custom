#ifndef _PRINT_H_
#define _PRINT_H_

extern bool enter_by_icon;
extern bool can_print_flag;
extern uint8_t auto_close_flag;

void menuPrintFromSource(void);
void menuPrint(void);
void menuCallBackChoseFileList(void);
void menuChoseFileList(void);
void menuWhetherToPtint();
void menuDeleteFile();

#endif
