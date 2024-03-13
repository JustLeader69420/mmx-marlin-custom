#ifndef _PRIBT_LIST
#define _PRIBT_LIST

#define LIST_LINE 6

extern char dwinCurPage;

void send_text_msg(void);
void get_sdcard_list(void);
uint16_t read_filenum(void);
bool lcdEnterDir(const char *nextdir);
void lcdExitDir();
void select_file(int i);

#endif
