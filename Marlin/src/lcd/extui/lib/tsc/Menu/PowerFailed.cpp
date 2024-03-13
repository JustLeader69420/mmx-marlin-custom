#include "../TSC_Menu.h"
#include "../../../../../feature/powerloss.h"

void menuCallBackPowerFailed(void)
{
  uint16_t key_num = KEY_GetValue(2, doubleBtnRect);
  switch(key_num)
  {
    case KEY_POPUP_CONFIRM:
      storeCmd("M1000");
      //recovery.resume();
      infoMenu.cur--;
      break;
    
    case KEY_POPUP_CANCEL:
     #ifdef HAS_UDISK
      if(sd_or_udisk){
        if(udisk.usbIsReady())
          f_unlink(recovery.filename1);
      }
      else
     #endif
        card.removeJobRecoveryFile();
      infoMenu.cur--;
      break;
  }
  if(!IS_SD_INSERTED() TERN_(HAS_UDISK, && !plr_flag))
  {
    infoMenu.cur--;
  }
}

void menuPowerFailed()
{
  //GUI_Clear(BK_COLOR);
  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_POWER_FAILED), (uint8_t *)recovery.info.sd_filename, textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
    
  menuSetFrontCallBack(menuCallBackPowerFailed);
}
