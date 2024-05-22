#include "../TSC_Menu.h"
#include "../../../../../feature/bedlevel/bedlevel.h"
#include "../../../../../feature/bedlevel/abl/abl.h"
#include "../Menu/menu.h"
#include "FeatureSettings.h"
#include "../../../../ultralcd.h"
//1 title, ITEM_PER_PAGE items (icon + label) 






/// @brief Draw a custom string instead of a string from the language files. Truncates the provided string and displays it
/// @param x X location on the display
/// @param y Y location on the display
/// @param text The string
/// @param pixelWidth The maximum width in pixels that the string should occupy
/// @return The rest of the string that wasn't displayed
const uint8_t* GUI_DispLenStringCustom(int16_t x, int16_t y, const char *text, uint16_t pixelWidth)
{       
    CHAR_INFO hinfo;
    uint16_t hcurPixelWidth = 0;
    size_t htextLength = strlen(text);
    
    // Allocate a buffer with uint8_t alignment
    uint8_t textBuffer[htextLength + 1]; // Add 1 for null terminator
    memcpy(textBuffer, text, htextLength + 1); // Copy the string along with the null terminator

    const uint8_t *p = textBuffer; // Use the aligned buffer as the pointer

    if(text == NULL) return NULL;
    
    while(hcurPixelWidth < pixelWidth && *p)
    {
        getCharacterInfo(p, &hinfo);
        if(hcurPixelWidth + hinfo.pixelWidth > pixelWidth) return p;
        GUI_DispOne(x, y, p);
        x += hinfo.pixelWidth;
        hcurPixelWidth += hinfo.pixelWidth;
        p += hinfo.bytes;
    }
    return p;
}






#ifdef USEOLDSTATUSSCREEN
MENUITEMS StatusItems = {
// title
LABEL_READY,
// icon                       label
 {{ICON_HEAT,                 LABEL_PREHEAT},
  {ICON_STATUSNOZZLE,         LABEL_NOZZLE},
  {ICON_STATUSBED,            LABEL_BED},
  // {ICON_STATUSFAN,            LABEL_BACKGROUND},
  // {ICON_STATUS_SPEED,         LABEL_BACKGROUND},
  #ifdef AUTO_BED_LEVELING_BILINEAR
    {ICON_LEVELING,             LABEL_LEVELING},
  #else
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
  #endif
  {ICON_MAINMENU,             LABEL_MAINMENU},  
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_PRINT,                LABEL_PRINT},}
};
#else
MENUITEMS StatusItems = {
// title
LABEL_BACKGROUND, //was LABEL_READY
// icon                       label
 {{ICON_HEAT,                 LABEL_PREHEAT},
  {ICON_STATUSNOZZLE,         LABEL_NOZZLE},
  {ICON_STATUSBED,            LABEL_BED},
  // {ICON_STATUSFAN,            LABEL_BACKGROUND},
  // {ICON_STATUS_SPEED,         LABEL_BACKGROUND},
  #ifdef AUTO_BED_LEVELING_BILINEAR
    {ICON_LEVELING,             LABEL_LEVELING},
  #else
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
  #endif
  {ICON_MAINMENU,             LABEL_MAINMENU},  
  {ICON_MORE,                 LABEL_FUNCTIONS}, // Custom functions menu, use more icon to not have to add more haha
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_PRINT,                LABEL_PRINT},}
};
#endif



const ITEM ToolItems[3] = {
// icon                       label
  {ICON_STATUSNOZZLE,         LABEL_BACKGROUND},
  {ICON_STATUSBED,            LABEL_BACKGROUND},
  {ICON_STATUSFAN,            LABEL_BACKGROUND},
//{ICON_HEAT_STATUS,          LABEL_BACKGROUND},
//{ICON_BED_STATUS,           LABEL_BACKGROUND},
//{ICON_FAN_STATUS,           LABEL_BACKGROUND},
};
// const ITEM SpeedItems[2] = {
// // icon                       label
//   {ICON_STATUS_SPEED,         LABEL_BACKGROUND},
//   {ICON_STATUS_FLOW,          LABEL_BACKGROUND},
// };

SCROLL     msgScroll;
static const uint8_t *msgbody;

const char* SpeedID[2] = {"Sp.", "Flow"};
// text position rectangles for Live icons 
//icon 0
const GUI_POINT pointID[4] = {
  {1*ICON_WIDTH+0*SPACE_X+START_X - 2*(BYTE_WIDTH/2),                   TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0},
  {2*ICON_WIDTH+1*SPACE_X+START_X - 2*(BYTE_WIDTH/2),                   TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0},
  {3*ICON_WIDTH+2*SPACE_X+START_X - 2*(BYTE_WIDTH/2),                   TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0},
  {4*ICON_WIDTH+3*SPACE_X+START_X - 2*(BYTE_WIDTH/2),                   TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0},
};
const GUI_RECT rectB[4] = {
  {START_X + 0 * ICON_WIDTH + 0 * SPACE_X + SSICON_INFO_X,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
   START_X + 1 * ICON_WIDTH + 0 * SPACE_X - SSICON_INFO_X,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
  
  {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + SSICON_INFO_X,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
   START_X + 2 * ICON_WIDTH + 1 * SPACE_X - SSICON_INFO_X,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
  
  {START_X + 2 * ICON_WIDTH + 2 * SPACE_X + SSICON_INFO_X,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
   START_X + 3 * ICON_WIDTH + 2 * SPACE_X - SSICON_INFO_X,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
  
  {START_X + 3 * ICON_WIDTH + 3 * SPACE_X + SSICON_INFO_X,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
   START_X + 4 * ICON_WIDTH + 3 * SPACE_X - SSICON_INFO_X,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
};

//info rectangle          
const GUI_RECT RectInfo = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X,  TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y,
                           START_X + 3 * ICON_WIDTH + 2 * SPACE_X,  TITLE_END_Y +  2 * ICON_HEIGHT + 1 * SPACE_Y};

const  GUI_RECT msgRect ={START_X + 1 * ICON_WIDTH + 2 * SPACE_X + 2,   TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + STATUS_MSG_BODY_YOFFSET - 7,
                          START_X + 3 * ICON_WIDTH + 1 * SPACE_X - 2,   TITLE_END_Y +  2 * ICON_HEIGHT + 1 * SPACE_Y - STATUS_MSG_BODY_BOTTOM - 7};

const GUI_RECT RecGantry = {START_X+(SPACE_Y-2*STATUS_GANTRY_YOFFSET)/2, 1*ICON_HEIGHT+0*SPACE_Y+TITLE_END_Y+STATUS_GANTRY_YOFFSET,
                            4*ICON_WIDTH+3*SPACE_X+START_X-(SPACE_Y-2*STATUS_GANTRY_YOFFSET)/2, 1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y-STATUS_GANTRY_YOFFSET+1};

const GUI_Circle Gantry_Circle_pointID[2] = {
  {RecGantry.x0,RecGantry.y0+(RecGantry.y1-RecGantry.y0)/2,(RecGantry.y1-RecGantry.y0)/2},
  {RecGantry.x1,RecGantry.y0+(RecGantry.y1-RecGantry.y0)/2,(RecGantry.y1-RecGantry.y0)/2},
};

STATUS_MSG statusMsg;

void statusMsg_GetCurMsg(STATUS_MSG *msg)
{
  msg->curTool = ExtUI::getActiveTool();
  msg->actBed = ExtUI::getActualTemp_celsius(ExtUI::BED);
  msg->tagBed = ExtUI::getTargetTemp_celsius(ExtUI::BED),
  msg->actHotend = ExtUI::getActualTemp_celsius(msg->curTool),
  msg->tagHotend = ExtUI::getTargetTemp_celsius(msg->curTool);
  msg->fanPercent = ExtUI::getTargetFan_percent(ExtUI::FAN0);
  msg->feedRatePercent = ExtUI::getFeedrate_percent();
  msg->x = ExtUI::getAxisPosition_mm(ExtUI::X),
  msg->y = ExtUI::getAxisPosition_mm(ExtUI::Y),
  msg->z = ExtUI::getAxisPosition_mm(ExtUI::Z);
  msg->e0 = ExtUI::getAxisPosition_mm(ExtUI::E0);
  msg->babystep_v = getBabyStepZAxisTotalMM();
  TERN_(LEVELING_OFFSET, msg->leveling_v = getLevelingOffset();)
  msg->progress = getPrintProgress();
}

#define STATUS_START_Y (TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0)

#define TOOL_VAL_SEPARATOR_X     (START_X + 1 * ICON_WIDTH + 1 * SPACE_X + (ICON_WIDTH-BYTE_WIDTH)/2)
#define BED_VAL_SEPARATOR_X      (START_X + 2 * ICON_WIDTH + 2 * SPACE_X + (ICON_WIDTH-BYTE_WIDTH)/2)
#define FAN_VAL_SEPARATOR_X      (START_X + 2 * ICON_WIDTH + 2 * SPACE_X + (ICON_WIDTH-BYTE_WIDTH*4)/2 + BYTE_WIDTH*3)
#define FEEDRATE_VAL_SEPARATOR_X (START_X + 3 * ICON_WIDTH + 3 * SPACE_X + (ICON_WIDTH-BYTE_WIDTH*4)/2 + BYTE_WIDTH*3)

static void redrawToolAct(void)
{
  GUI_DispDec(TOOL_VAL_SEPARATOR_X - 3*BYTE_WIDTH, STATUS_START_Y, (int16_t)statusMsg.actHotend, 3, RIGHT);
}
static void redrawToolTag(void)
{
  GUI_DispDec(TOOL_VAL_SEPARATOR_X + BYTE_WIDTH+1, STATUS_START_Y, (int16_t)statusMsg.tagHotend, 3, LEFT);
}

static void redrawBedAct(void)
{
  GUI_DispDec(BED_VAL_SEPARATOR_X - 3*BYTE_WIDTH, STATUS_START_Y, (int16_t)statusMsg.actBed, 3, RIGHT);
}
static void redrawBedTag(void)
{
  GUI_DispDec(BED_VAL_SEPARATOR_X + BYTE_WIDTH+1, STATUS_START_Y, (int16_t)statusMsg.tagBed, 3, LEFT);
}

static void redrawFan(void)
{
  GUI_DispDec(FAN_VAL_SEPARATOR_X - 3*BYTE_WIDTH, STATUS_START_Y, (int16_t)statusMsg.fanPercent, 3, RIGHT);
}

static void redrawFeedRate(void)
{
  GUI_DispDec(FEEDRATE_VAL_SEPARATOR_X - 3*BYTE_WIDTH, STATUS_START_Y, (int16_t)statusMsg.feedRatePercent, 3, RIGHT);
}

static void redrawPosition(void)
{
  /*char tempstr[100];
  GUI_SetColor(0x2945);
  GUI_SetBkColor(GANTRYLBL_BKCOLOR);
  GUI_FillCircle(Gantry_Circle_pointID[0].x0-1,Gantry_Circle_pointID[0].y0,Gantry_Circle_pointID[0].r);
  GUI_FillCircle(Gantry_Circle_pointID[1].x0+1,Gantry_Circle_pointID[1].y0,Gantry_Circle_pointID[1].r);
  GUI_SetColor(GANTRYLBL_BKCOLOR);
  GUI_FillCircle(Gantry_Circle_pointID[0].x0,Gantry_Circle_pointID[0].y0,Gantry_Circle_pointID[0].r);
  GUI_FillCircle(Gantry_Circle_pointID[1].x0,Gantry_Circle_pointID[1].y0,Gantry_Circle_pointID[1].r);
  GUI_SetColor(GANTRYLBL_COLOR);
  GUI_ClearPrect(&RecGantry);
  // sprintf_P(tempstr, "X: %.2f   Y: %.2f   Z: %.2f", statusMsg.x, statusMsg.y, statusMsg.z);
  sprintf_P(tempstr, "X: %d.%d   Y: %d.%d   Z: %d.%d", (int)(statusMsg.x),(int)(statusMsg.x*10)%10, 
                                                       (int)(statusMsg.y),(int)(statusMsg.y*10)%10,
                                                       (int)(statusMsg.z),(int)(statusMsg.z*10)%10);
  GUI_DispStringInPrect(&RecGantry,(uint8_t *)tempstr);
  GUI_RestoreColorDefault();*/    // 20210617，去除显示xyz，用来显示label
}

static void drawStatus(void)
{
  statusMsg_GetCurMsg(&statusMsg);

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_SetColor(HEADING_COLOR);

  GUI_DispStringRight(pointID[1].x, pointID[1].y, (uint8_t *)heatDisplayID[statusMsg.curTool+1]); // Ext label
  GUI_DispStringRight(pointID[2].x, pointID[2].y, (uint8_t *)heatDisplayID[0]); // Bed label
  // GUI_DispStringRight(pointID[2].x, pointID[2].y, (uint8_t *)fanID[0]); // Fan label
  // GUI_DispStringRight(pointID[3].x, pointID[3].y, (uint8_t *)SpeedID[0]); // Speed / Flow label

  GUI_SetColor(VAL_COLOR);
  GUI_SetBkColor(WHITE);

  GUI_ClearPrect(&rectB[1]);
  GUI_DispString(TOOL_VAL_SEPARATOR_X, STATUS_START_Y, (uint8_t *)"/"); // Ext value     // I assume this displays the separators on the temperature values
  redrawToolAct();
  redrawToolTag();
  GUI_ClearPrect(&rectB[2]);
  GUI_DispString(BED_VAL_SEPARATOR_X, STATUS_START_Y, (uint8_t *)"/"); // Bed value     // I assume this displays the separators on the temperature values
  redrawBedAct();
  redrawBedTag();
  // GUI_ClearPrect(&rectB[2]);
  // GUI_DispString(FAN_VAL_SEPARATOR_X, STATUS_START_Y, (uint8_t *)"%"); // Fan speed percent
  // redrawFan();
  // GUI_ClearPrect(&rectB[3]);
  // GUI_DispString(FEEDRATE_VAL_SEPARATOR_X, STATUS_START_Y, (uint8_t *)"%"); // Feedrate percent
  // redrawFeedRate();

  /*redrawPosition();*/ // 20210617

  GUI_RestoreColorDefault();
}

void loopStatusCheck(void)
{
  STATUS_MSG tempMsg;
  statusMsg_GetCurMsg(&tempMsg);

  // Position & actual temperature refresh per 1 sec
  static uint32_t nowTime_ms = 0;
  if (millis() - nowTime_ms > 1000) { // Refresh per 1 sec
    nowTime_ms = millis();
    // Refresh position
    /*if (statusMsg.x != tempMsg.x || statusMsg.y != tempMsg.y || statusMsg.z != tempMsg.z){
      statusMsg.x = tempMsg.x;
      statusMsg.y = tempMsg.y;
      statusMsg.z = tempMsg.z;
      redrawPosition();
    }*/ // 20210617

    GUI_SetColor(VAL_COLOR);
    GUI_SetBkColor(WHITE);
    // Refresh temperature
    //  if (tool != tempTool)
    if (statusMsg.actHotend != tempMsg.actHotend) {
      statusMsg.actHotend = tempMsg.actHotend;
      redrawToolAct();
    }
    
    if (statusMsg.actBed != tempMsg.actBed) {
      statusMsg.actBed = tempMsg.actBed;
      redrawBedAct();
    }
  }
  // Target temperature & fan speed & feedrate refresh in real time
  GUI_SetColor(VAL_COLOR);
  GUI_SetBkColor(WHITE);
  if (statusMsg.tagHotend != tempMsg.tagHotend) {
    statusMsg.tagHotend = tempMsg.tagHotend;
    redrawToolTag();
  }
  if (statusMsg.tagBed != tempMsg.tagBed) {
    statusMsg.tagBed = tempMsg.tagBed;
    redrawBedTag();
  }

  GUI_RestoreColorDefault();
}


static bool msgNeedUpdate = true;
void statusScreen_setMsg(const uint8_t *title, const uint8_t *msg)
{
  msgbody = msg;
  msgNeedUpdate = true;
}

void redrawStatusMsg(void)
{
  #ifdef USEOLDSTATUSSCREEN
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_ClearPrect(&msgRect); 

  GUI_SetColor(0x8C51);
  GUI_FillCircle(msgRect.x0-1,(msgRect.y1-msgRect.y0)/2 + msgRect.y0,(msgRect.y1-msgRect.y0)/2); 
  GUI_FillCircle(msgRect.x1+1,(msgRect.y1-msgRect.y0)/2 + msgRect.y0,(msgRect.y1-msgRect.y0)/2); 
  GUI_SetColor(INFOMSG_BKCOLOR);
  GUI_FillCircle(msgRect.x0,(msgRect.y1-msgRect.y0)/2 + msgRect.y0,(msgRect.y1-msgRect.y0)/2); 
  GUI_FillCircle(msgRect.x1,(msgRect.y1-msgRect.y0)/2 + msgRect.y0,(msgRect.y1-msgRect.y0)/2); 
  GUI_SetColor(INFOMSG_COLOR);

  Scroll_CreatePara(&msgScroll, (uint8_t *)msgbody, &msgRect);
  GUI_SetBkColor(BK_COLOR);
#else



char* part1 = (char *)textSelect(LABEL_READY); 
char* part2 = " | ";
#ifdef SHOWINFOSTRING
char* part3 = (char *)textSelect(LABEL_SCREEN_INFO);
char* part4 = ": ";
#endif
char* part5 = (char *)msgbody;
duration_t part6secs = MarlinUI::remaining_time;
char part6[32]; // Unlikely to get this big, but just to be sure
part6secs.toString(part6);



// Calculate the size required for the result array
size_t size = strlen(part1) + strlen(part2); // add ready message and separator
#ifdef SHOWINFOSTRING
size += strlen(part3) + strlen(part4); // add info string and separator
#endif
#ifdef SHOWREMAININGTIME
size += strlen(part6) + strlen(part2); // add remaining time and separator
#endif
size += strlen(part5) + 1;  // msgbody and +1 for the null terminator

// Declare the result array with the calculated size
char result[size];

//get_remaining_time()

strcpy(result, part1);    // Copy part1 into result
strcat(result, part2);    // Concatenate part2 to result
#ifdef SHOWINFOSTRING
strcat(result, part3);    // Concatenate part3 to result
strcat(result, part4);    // Concatenate part4 to result
#endif
strcat(result, part5);    // Concatenate part5 to result

// Call the function to display the text
uint16_t start_y = (TITLE_END_Y - 10 - BYTE_HEIGHT) / 2;
GUI_FillRectColor(10, (TITLE_END_Y - 10 - BYTE_HEIGHT) / 2, LCD_WIDTH_PIXEL-10, start_y+BYTE_HEIGHT, TITLE_COLOR);     // Likely clearing space
GUI_SetTextMode(GUI_TEXTMODE_TRANS);
GUI_DispLenStringCustom(10, start_y, result, LCD_WIDTH_PIXEL-20);
GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
#endif

}

void drawStatusScreenMsg(void)
{
  #ifdef USEOLDSTATUSSCREEN
  // GUI_SetBkColor(0x4B0D);
  // GUI_ClearPrect(&RectInfo);
  // GUI_Clear_RCRect(RectInfo.x0, RectInfo.y0, RectInfo.x1, RectInfo.y1, 20);
  const ITEM itemStatus = {ICON_DEC,LABEL_BACKGROUND};
  GUI_SetBkColor(BLACK);
  menuDrawItem(&itemStatus, 5);
  menuDrawItem(&itemStatus, 6);
  GUI_SetBkColor(MD_GRAY);
  GUI_ClearRect(RectInfo.x0+20,RectInfo.y0,RectInfo.x1-20,RectInfo.y1);
  GUI_DispString(RectInfo.x0 + STATUS_MSG_TITLE_XOFFSET,RectInfo.y0 + STATUS_MSG_ICON_YOFFSET, textSelect(LABEL_STATUS_INFO));
  #endif
  redrawStatusMsg();
}



void scrollMsg(void){
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_SetColor(INFOMSG_COLOR);
  Scroll_DispString(&msgScroll, CENTER);
  GUI_RestoreColorDefault();
}

void menuCallStatus(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  switch (key_num)
  {
    case KEY_ICON_0:
      infoMenu.menu[++infoMenu.cur] = menuPreheat;
      break;
    case KEY_ICON_1:
      heatSetCurHeaterIndex(statusMsg.curTool+1); // Tool, Nozzle
      infoMenu.menu[++infoMenu.cur] = menuHeat;
      break;
    case KEY_ICON_2:
      heatSetCurHeaterIndex(0); // Bed
      infoMenu.menu[++infoMenu.cur] = menuHeat;
      break;
    case KEY_ICON_3:
      // infoMenu.menu[++infoMenu.cur] = menuSpeed;
      #ifdef AUTO_BED_LEVELING_BILINEAR
        // infoMenu.menu[++infoMenu.cur] = menuAutoLeveling;
        infoMenu.menu[++infoMenu.cur] = menuPopup_ABL;
      #else
        //infoMenu.menu[++infoMenu.cur] = menuManualLeveling;
      #endif
      break;
    case KEY_ICON_4:
      infoMenu.menu[++infoMenu.cur] = menuMain;
      break;
    case KEY_ICON_5:
      infoMenu.menu[++infoMenu.cur] = menuFunctionsmenu;
      break;
    case KEY_ICON_7:
     #ifdef HAS_UDISK
      infoMenu.menu[++infoMenu.cur] = menuChooseStorage;
     #else
      if(z_values[1][1] != 0){
        planner.leveling_active = true;
        set_bed_leveling_enabled(true);
      }
      enter_by_icon = true;
      infoMenu.menu[++infoMenu.cur] = menuPrint;
     #endif
      break;

    default:break;
  }
  if (msgNeedUpdate) {
    msgNeedUpdate = false;
    redrawStatusMsg();
  }
  scrollMsg();
  loopStatusCheck();  
}


void menuStatus()
{
  menuDrawPage(&StatusItems);
  drawStatus();
  drawStatusScreenMsg();
  msgNeedUpdate = false;
  TERN_( HAS_UDISK, udisk.init(); )
  menuSetFrontCallBack(menuCallStatus);
}
