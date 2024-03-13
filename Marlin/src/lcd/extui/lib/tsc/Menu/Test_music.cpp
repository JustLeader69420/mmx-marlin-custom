#include "../TSC_Menu.h"

#if ENABLED(NEW_BOARD)

#include "../../../../../module/music/KT404C.h"
//1 title, ITEM_PER_PAGE items
MENUITEMS musicItems = {
// title
LABEL_PERCENTAGE_SPEED,
// icon                       label
 {{ICON_DEC,                  LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_INC,                  LABEL_INC},
  {ICON_START,                LABEL_HEAT},
  {ICON_Z_DEC,                LABEL_Z_DEC},
  {ICON_Z_INC,                LABEL_Z_INC},
  {ICON_BACK,                 LABEL_BACK},}
};
static const ITEM itemMusicStartStop[2] = {
// icon                       label
  {ICON_STOP,                 LABEL_STOP},
  {ICON_START,                LABEL_HEAT},
};

static unsigned char item_music_num = 0;
static int item_music_volume = 30;
static uint8_t item_music_icon = 0;
#define MAX_MUSIC_NUM 3
static const GUI_RECT TestCurrentRect_0 = { (int16_t)121, (int16_t) 75, (int16_t)361, (int16_t)100 };
static const GUI_RECT TestCurrentRect_1 = { (int16_t)121, (int16_t)101, (int16_t)361, (int16_t)126 };
static const GUI_RECT TestCurrentRect_2 = { (int16_t)121, (int16_t)127, (int16_t)361, (int16_t)152 };
static const GUI_RECT TestCurrentRect_3 = { (int16_t)121, (int16_t)153, (int16_t)361, (int16_t)178 };

/********************************************************/
void redrawTestMusic(void){
  char tempstr[32];
  char tempstr2[64];
  uint8_t i = 0, j = 0, k = 0;

  GUI_SetColor(GANTRYLBL_COLOR);

  memset(tempstr, 0, sizeof(tempstr));
  memset(tempstr2, 0, sizeof(tempstr2));
  if(item_music_volume%2){
    for(i=0; i<(item_music_volume/2); i++)
      tempstr[i] = '|';
    tempstr[i] = '*';
  }else{
    for(i=0; i<(item_music_volume/2); i++)
      tempstr[i] = '|';
  }
  GUI_ClearPrect(&TestCurrentRect_0);
  // motors_v = (motors_num/GETTIME*3300)/(1024*21*4);
  sprintf_P(tempstr2, "volume: %s", tempstr);
  GUI_DispStringInPrect(&TestCurrentRect_0,(uint8_t *)tempstr2);

  memset(tempstr, 0, sizeof(tempstr));
  // memset(tempstr2, 0, sizeof(tempstr2));
  GUI_ClearPrect(&TestCurrentRect_1);
  sprintf_P(tempstr, "%d.mp3", item_music_num);
  GUI_DispStringInPrect(&TestCurrentRect_1,(uint8_t *)tempstr);
}
// 音乐
void menuCallBackMusic(void)
{
  KEY_VALUES  key_num = menuKeyGetValue();
  bool music_flag = false;
  switch(key_num)
  {
    case KEY_ICON_0:
      item_music_volume--;
      if (item_music_volume<0) item_music_volume = 0;
      kt404c.voice_amp(item_music_volume);
      music_flag = true;
      break;
      
    case KEY_ICON_3:
      item_music_volume++;
      kt404c.voice_amp(item_music_volume);
      music_flag = true;
      break;
      
    case KEY_ICON_4:
      item_music_icon = (item_music_icon+1) % 2;
      musicItems.items[key_num] = itemMusicStartStop[item_music_icon];
      menuDrawItem(&musicItems.items[key_num], key_num);
      break;
    
    case KEY_ICON_5:
      item_music_num = (item_music_num-1)%MAX_MUSIC_NUM;
      kt404c.play(item_music_num);
      music_flag = true;
      break;
    case KEY_ICON_6:
      item_music_num = (item_music_num+1)%MAX_MUSIC_NUM;
      kt404c.play(item_music_num);
      music_flag = true;
      break;
    case KEY_ICON_7:
      infoMenu.cur--;
      break;
    default:break;
  }
  if(music_flag == true)
  {
    music_flag = false;
    redrawTestMusic();
  }
}

void menuTestM(void)
{
  menuDrawPage(&musicItems);
  redrawTestMusic();
  menuSetFrontCallBack(menuCallBackMusic);
}

#endif
