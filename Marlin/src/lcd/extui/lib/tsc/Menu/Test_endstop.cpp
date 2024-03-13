#include "../TSC_Menu.h"

static MENUITEMS TestItems = {
// title
LABEL_BACKGROUND,
// icon                       label
 {{ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},}
  // {ICON_BACK,                 LABEL_BACK},}
};

#define E_Y_START   40
static const GUI_RECT TestEndstopRect[16] = {
  { (int16_t)20,  (int16_t)E_Y_START+0*BYTE_HEIGHT, (int16_t)239, (int16_t)E_Y_START+1*BYTE_HEIGHT },
  { (int16_t)240, (int16_t)E_Y_START+0*BYTE_HEIGHT, (int16_t)460, (int16_t)E_Y_START+1*BYTE_HEIGHT },
  { (int16_t)20,  (int16_t)E_Y_START+1*BYTE_HEIGHT, (int16_t)239, (int16_t)E_Y_START+2*BYTE_HEIGHT },
  { (int16_t)240, (int16_t)E_Y_START+1*BYTE_HEIGHT, (int16_t)460, (int16_t)E_Y_START+2*BYTE_HEIGHT },
  { (int16_t)20,  (int16_t)E_Y_START+2*BYTE_HEIGHT, (int16_t)239, (int16_t)E_Y_START+3*BYTE_HEIGHT },
  { (int16_t)240, (int16_t)E_Y_START+2*BYTE_HEIGHT, (int16_t)460, (int16_t)E_Y_START+3*BYTE_HEIGHT },
  { (int16_t)20,  (int16_t)E_Y_START+3*BYTE_HEIGHT, (int16_t)239, (int16_t)E_Y_START+4*BYTE_HEIGHT },
  { (int16_t)240, (int16_t)E_Y_START+3*BYTE_HEIGHT, (int16_t)460, (int16_t)E_Y_START+4*BYTE_HEIGHT },
  { (int16_t)20,  (int16_t)E_Y_START+4*BYTE_HEIGHT, (int16_t)239, (int16_t)E_Y_START+5*BYTE_HEIGHT },
  { (int16_t)240, (int16_t)E_Y_START+4*BYTE_HEIGHT, (int16_t)460, (int16_t)E_Y_START+5*BYTE_HEIGHT },
  { (int16_t)20,  (int16_t)E_Y_START+5*BYTE_HEIGHT, (int16_t)239, (int16_t)E_Y_START+6*BYTE_HEIGHT },
  { (int16_t)240, (int16_t)E_Y_START+5*BYTE_HEIGHT, (int16_t)460, (int16_t)E_Y_START+6*BYTE_HEIGHT },
  { (int16_t)20,  (int16_t)E_Y_START+6*BYTE_HEIGHT, (int16_t)239, (int16_t)E_Y_START+7*BYTE_HEIGHT },
  { (int16_t)240, (int16_t)E_Y_START+6*BYTE_HEIGHT, (int16_t)460, (int16_t)E_Y_START+7*BYTE_HEIGHT },
  { (int16_t)20,  (int16_t)E_Y_START+7*BYTE_HEIGHT, (int16_t)239, (int16_t)E_Y_START+8*BYTE_HEIGHT },
  { (int16_t)240, (int16_t)E_Y_START+7*BYTE_HEIGHT, (int16_t)460, (int16_t)E_Y_START+8*BYTE_HEIGHT },
};

enum endstopstates
{
  #if HAS_X_MIN
    X_MIN_E,
  #endif
  #if HAS_X2_MIN
    X2_MIN_E,
  #endif
  #if HAS_X_MAX
    X_MAX_E,
  #endif
  #if HAS_X2_MAX
    X2_MAX_E,
  #endif
  #if HAS_Y_MIN
    Y_MIN_E,
  #endif
  #if HAS_Y2_MIN
    Y2_MIN_E,
  #endif
  #if HAS_Y_MAX
    Y_MAX_E,
  #endif
  #if HAS_Y2_MAX
    Y2_MAX_E,
  #endif
  #if HAS_Z_MIN
    Z_MIN_E,
  #endif
  #if HAS_Z2_MIN
    Z2_MIN_E,
  #endif
  #if HAS_Z3_MIN
    Z3_MIN_E,
  #endif
  #if HAS_Z4_MIN
    Z4_MIN_E,
  #endif
  #if HAS_Z_MAX
    Z_MAX_E,
  #endif
  #if HAS_Z2_MAX
    Z2_MAX_E,
  #endif
  #if HAS_Z3_MAX
    Z3_MAX_E,
  #endif
  #if HAS_Z4_MAX
    Z4_MAX_E,
  #endif
  #if HAS_CUSTOM_PROBE_PIN
    PROBE_PIN_E,
  #endif
  #if HAS_FILAMENT_SENSOR
    #if NUM_RUNOUT_SENSORS == 1
      FIL_RUNOUT_E,
    #else
    #endif
  #endif
};
/********************************************************/
static bool report_endstop_states() {
  // TERN_(BLTOUCH, bltouch._set_SW_mode());
  #define ES_REPORT(S) return (READ(S##_PIN) != S##_ENDSTOP_INVERTING)
  #if HAS_X_MIN
    ES_REPORT(X_MIN);
  #endif
  #if HAS_X2_MIN
    ES_REPORT(X2_MIN);
  #endif
  #if HAS_X_MAX
    ES_REPORT(X_MAX);
  #endif
  #if HAS_X2_MAX
    ES_REPORT(X2_MAX);
  #endif
  #if HAS_Y_MIN
    ES_REPORT(Y_MIN);
  #endif
  #if HAS_Y2_MIN
    ES_REPORT(Y2_MIN);
  #endif
  #if HAS_Y_MAX
    ES_REPORT(Y_MAX);
  #endif
  #if HAS_Y2_MAX
    ES_REPORT(Y2_MAX);
  #endif
  #if HAS_Z_MIN
    ES_REPORT(Z_MIN);
  #endif
  #if HAS_Z2_MIN
    ES_REPORT(Z2_MIN);
  #endif
  #if HAS_Z3_MIN
    ES_REPORT(Z3_MIN);
  #endif
  #if HAS_Z4_MIN
    ES_REPORT(Z4_MIN);
  #endif
  #if HAS_Z_MAX
    ES_REPORT(Z_MAX);
  #endif
  #if HAS_Z2_MAX
    ES_REPORT(Z2_MAX);
  #endif
  #if HAS_Z3_MAX
    ES_REPORT(Z3_MAX);
  #endif
  #if HAS_Z4_MAX
    ES_REPORT(Z4_MAX);
  #endif
  #if HAS_CUSTOM_PROBE_PIN
    // return (READ(Z_MIN_PROBE_PIN) != Z_MIN_PROBE_ENDSTOP_INVERTING, PSTR(STR_Z_PROBE));
    ES_REPORT(Z_MIN_PROBE);
  #endif
  #if HAS_FILAMENT_SENSOR
    #if NUM_RUNOUT_SENSORS == 1
      return (READ(FIL_RUNOUT_PIN) != FIL_RUNOUT_STATE, PSTR(STR_FILAMENT_RUNOUT_SENSOR));
    #else
    #endif
  #endif

  // TERN_(BLTOUCH, bltouch._reset_SW_mode());
  // TERN_(JOYSTICK_DEBUG, joystick.report());
}
void drawEndstopStates(uint8_t item)
{
  switch(item){
    #if HAS_X_MIN
      case X_MIN_E:
        // GUI_DispStringInPrect(&TestEndstopRect[X_MIN_E], "X_MIN");
        break;
    #endif
    #if HAS_X2_MIN
      case X2_MIN_E:break;
    #endif
    #if HAS_X_MAX
      case X_MAX_E:break;
    #endif
    #if HAS_X2_MAX
      case X2_MAX_E:break;
    #endif
    #if HAS_Y_MIN
      case Y_MIN_E:break;
    #endif
    #if HAS_Y2_MIN
      case Y2_MIN_E:break;
    #endif
    #if HAS_Y_MAX
      case Y_MAX_E:break;
    #endif
    #if HAS_Y2_MAX
      case Y2_MAX_E:break;
    #endif
    #if HAS_Z_MIN
      case Z_MIN_E:break;
    #endif
    #if HAS_Z2_MIN
      case Z2_MIN_E:break;
    #endif
    #if HAS_Z3_MIN
      case Z3_MIN_E:break;
    #endif
    #if HAS_Z4_MIN
      case Z4_MIN_E:break;
    #endif
    #if HAS_Z_MAX
      case Z_MAX_E:break;
    #endif
    #if HAS_Z2_MAX
      case Z2_MAX_E:break;
    #endif
    #if HAS_Z3_MAX
      case Z3_MAX_E:break;
    #endif
    #if HAS_Z4_MAX
      case Z4_MAX_E:break;
    #endif
    #if HAS_CUSTOM_PROBE_PIN
      case PROBE_PIN_E:break;
    #endif
    #if HAS_FILAMENT_SENSOR
      #if NUM_RUNOUT_SENSORS == 1
        case FIL_RUNOUT_E:break;
      #else
      #endif
    #endif

    default:break;
  }
}
void menuCallBackEndstopStates()
{
  #define ES_REPORT(S) (READ(S##_PIN) != S##_ENDSTOP_INVERTING)
  
  static uint32_t nowTime_ms = 0;
  KEY_VALUES key_num = menuKeyGetValue();

  if (key_num != KEY_IDLE)
  {
    infoMenu.cur--;
  }
  if (millis() - nowTime_ms > 1000) 
  {
    nowTime_ms = millis();
    
    #if HAS_X_MIN
      if(ES_REPORT(X_MIN)){
        GUI_DispStringInPrect(&TestEndstopRect[X_MIN_E], (const uint8_t*)"Xin   T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[X_MIN_E], (const uint8_t*)"Xin   O");
      }
    #endif
    #if HAS_X2_MIN
      if(ES_REPORT(X2_MIN)){
        GUI_DispStringInPrect(&TestEndstopRect[X2_MIN_E], (const uint8_t*)"Xin2  T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[X2_MIN_E], (const uint8_t*)"Xin2  O");
      }
    #endif
    #if HAS_X_MAX
      if(ES_REPORT(X_MAX)){
        GUI_DispStringInPrect(&TestEndstopRect[X_MAX_E], (const uint8_t*)"Xax   T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[X_MAX_E], (const uint8_t*)"Xax   O");
      }
    #endif
    #if HAS_X2_MAX
      if(ES_REPORT(X2_MAX)){
        GUI_DispStringInPrect(&TestEndstopRect[X2_MAX_E], (const uint8_t*)"Xax2  T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[X2_MAX_E], (const uint8_t*)"Xax2  O");
      }
    #endif
    #if HAS_Y_MIN
      if(ES_REPORT(Y_MIN)){
        GUI_DispStringInPrect(&TestEndstopRect[Y_MIN_E], (const uint8_t*)"Yin   T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[Y_MIN_E], (const uint8_t*)"Yin   O");
      }
    #endif
    #if HAS_Y2_MIN
      if(ES_REPORT(Y2_MIN)){
        GUI_DispStringInPrect(&TestEndstopRect[Y2_MIN_E], (const uint8_t*)"Yin2  T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[Y2_MIN_E], (const uint8_t*)"Yin2  O");
      }
    #endif
    #if HAS_Y_MAX
      if(ES_REPORT(Y_MAX)){
        GUI_DispStringInPrect(&TestEndstopRect[Y_MAX_E], (const uint8_t*)"Yax   T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[Y_MAX_E], (const uint8_t*)"Yax   O");
      }
    #endif
    #if HAS_Y2_MAX
      if(ES_REPORT(Y2_MAX)){
        GUI_DispStringInPrect(&TestEndstopRect[Y2_MAX_E], (const uint8_t*)"Yax2  T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[Y2_MAX_E], (const uint8_t*)"Yax2  O");
      }
    #endif
    #if HAS_Z_MIN
      if(ES_REPORT(Z_MIN)){
        GUI_DispStringInPrect(&TestEndstopRect[Z_MIN_E], (const uint8_t*)"Zin   T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[Z_MIN_E], (const uint8_t*)"Zin   O");
      }
    #endif
    #if HAS_Z2_MIN
      if(ES_REPORT(Z2_MIN)){
        GUI_DispStringInPrect(&TestEndstopRect[Z2_MIN_E], (const uint8_t*)"Zin2  T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[Z2_MIN_E], (const uint8_t*)"Zin2  O");
      }
    #endif
    #if HAS_Z3_MIN
      if(ES_REPORT(Z3_MIN)){
        GUI_DispStringInPrect(&TestEndstopRect[Z3_MIN_E], (const uint8_t*)"Zin3  T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[Z3_MIN_E], (const uint8_t*)"Zin3  O");
      }
    #endif
    #if HAS_Z4_MIN
      if(ES_REPORT(Z4_MIN)){
        GUI_DispStringInPrect(&TestEndstopRect[Z4_MIN_E], (const uint8_t*)"Zin4  T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[Z4_MIN_E], (const uint8_t*)"Zin4  O");
      }
    #endif
    #if HAS_Z_MAX
      if(ES_REPORT(Z_MAX)){
        GUI_DispStringInPrect(&TestEndstopRect[Z_MAX_E], (const uint8_t*)"Zax   T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[Z_MAX_E], (const uint8_t*)"Zax   O");
      }
    #endif
    #if HAS_Z2_MAX
      if(ES_REPORT(Z2_MAX)){
        GUI_DispStringInPrect(&TestEndstopRect[Z2_MAX_E], (const uint8_t*)"Zax2  T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[Z2_MAX_E], (const uint8_t*)"Zax2  O");
      }
    #endif
    #if HAS_Z3_MAX
      if(ES_REPORT(Z3_MAX)){
        GUI_DispStringInPrect(&TestEndstopRect[Z3_MAX_E], (const uint8_t*)"Zax3  T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[Z3_MAX_E], (const uint8_t*)"Zax3  O");
      }
    #endif
    #if HAS_Z4_MAX
      if(ES_REPORT(Z4_MAX)){
        GUI_DispStringInPrect(&TestEndstopRect[Z4_MAX_E], (const uint8_t*)"Zax4  T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[Z4_MAX_E], (const uint8_t*)"Zax4  O");
      }
    #endif
    #if HAS_CUSTOM_PROBE_PIN
      // return (READ(Z_MIN_PROBE_PIN) != Z_MIN_PROBE_ENDSTOP_INVERTING, PSTR(STR_Z_PROBE));
      if(ES_REPORT(Z_MIN_PROBE)){
        GUI_DispStringInPrect(&TestEndstopRect[PROBE_PIN_E], (const uint8_t*)"ZPin  T");
      }
      else{
        GUI_DispStringInPrect(&TestEndstopRect[PROBE_PIN_E], (const uint8_t*)"ZPin  O");
      }
    #endif
    #if HAS_FILAMENT_SENSOR
      #if NUM_RUNOUT_SENSORS == 1
        if (READ(FIL_RUNOUT_PIN) != FIL_RUNOUT_STATE){
          GUI_DispStringInPrect(&TestEndstopRect[FIL_RUNOUT_E], (const uint8_t*)"FRin  T");
        }
        else{
          GUI_DispStringInPrect(&TestEndstopRect[FIL_RUNOUT_E], (const uint8_t*)"FRin  O");
        }
      #else
      #endif
    #endif
  }
}
void menuEndstopStates(){
  menuDrawPage(&TestItems);
  menuSetFrontCallBack(menuCallBackEndstopStates);
}
