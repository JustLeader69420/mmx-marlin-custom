#include "usb_common_init.h"
#include "stm32_usbh_conf.h"
#include "usbh_msc.h"
#include "stm32_def.h"
#include "log.h"
#include "fatfs.h"
// #include "../inc/MarlinConfig.h"
#include "../lcd/extui/ui_api.h"
#include "../lcd/extui/lib/tsc/TSC_Menu.h"
#include "../feature/powerloss.h"
#include "udiskPrint.h"

#ifdef HAS_UDISK
// PCD_HandleTypeDef hpcd_USB_OTG_FS;
// HCD_HandleTypeDef hhcd_USB_OTG_HS;
// extern PCD_HandleTypeDef hpcd_USB_OTG_HS;
extern HCD_HandleTypeDef hhcd_USB_OTG_FS;   //fs as msc

/** Status of the application. */
typedef enum {
  APPLICATION_IDLE = 0,
  APPLICATION_START,
  APPLICATION_READY,
  APPLICATION_DISCONNECT
}ApplicationTypeDef;

typedef enum {
  MSC_DEMO_START = 0,
  MSC_DEMO_FILE_OPERATIONS,
  MSC_DEMO_EXPLORER,
  MSC_DEMO_STOP,
}MSC_Demo_State;

MSC_Demo_State msc_demo;
MSC_LUNTypeDef msc_info;
USBH_HandleTypeDef hUsbHostFS;
// FATFS USBH_fatfs;

ApplicationTypeDef Appli_state = APPLICATION_IDLE;
#if 0
void MSC_MenuProcess(void)
{
  switch(msc_demo)
  {
    case MSC_DEMO_START:
      if(Appli_state == APPLICATION_READY)
      {
        msc_demo = MSC_DEMO_FILE_OPERATIONS;
      }
    break;

    case MSC_DEMO_FILE_OPERATIONS:
      /* Read and Write File Here */
      if(Appli_state == APPLICATION_READY)
      {
          msc_demo = MSC_DEMO_EXPLORER;
          // MSC_File_Operations();
      //   USBH_MSC_GetLUNInfo(&hUsbHostFS, 0, &msc_info);
      //   LOGI("usb disk vid %s", msc_info.inquiry.vendor_id);
      //   LOGI("usb disk pid %s", msc_info.inquiry.product_id);
      //   LOGI("usb disk cap block nbr %d ", msc_info.capacity.block_nbr);
      //   LOGI("usb disk cap block size %d ", msc_info.capacity.block_size);
      //   uint64_t total_size = (uint64_t)msc_info.capacity.block_size * msc_info.capacity.block_nbr;
      //   uint32_t size_kb = total_size / 1024;
      //   uint32_t size_mb = size_kb / 1024;
      //   uint32_t size_gb = size_mb / 1024;
      //   LOGI("usb disk cap size %d MB, %d GB", size_mb, size_gb);
      //   LOGI("usb disk cap size %d kBytes", size_kb);

        /* Prevent debounce effect for user key */
      //   HAL_Delay(400);
      }
    break;

    case MSC_DEMO_EXPLORER:
      /* Display disk content */
      if(Appli_state == APPLICATION_READY)
      {
        Explore_Disk("0:/", 1);
        msc_demo = MSC_DEMO_STOP;

        /* Prevent debounce effect for user key */
      //   HAL_Delay(400);
      }
    break;
    
    case MSC_DEMO_STOP:
      /* Display disk content */
          //stop here.
    break;

    default: break;
  }

  if(Appli_state == APPLICATION_DISCONNECT)
  {
    Appli_state = APPLICATION_IDLE;
    msc_demo = MSC_DEMO_STOP;
  }
}
#endif

static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

void MX_USB_HOST_Init(void)
{
  /* Init host Library, add supported class and start the library. */
  if (USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostFS, USBH_MSC_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_Start(&hUsbHostFS) != USBH_OK)
  {
    Error_Handler();
  }
}


/*
 * Background task
 */
void MX_USB_HOST_Process(void)
{
  /* USB Host Background task */
  USBH_Process(&hUsbHostFS);
}
HOST_StateTypeDef MX_USB_GET_State(void){
  return hUsbHostFS.gState;
}
uint8_t usb_is_ready(void){
  return (USBH_MSC_IsReady(&hUsbHostFS));
}

/*
 * user callback definition
 * USBH_HandleTypeDef->pUser callback function
 * you can set some hint here
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
  switch(id)
  {
    case HOST_USER_SELECT_CONFIGURATION:
      // usb_printf("usb device select config. \r\n");
      break;

    // disconnection
    case HOST_USER_DISCONNECTION:
      Appli_state = APPLICATION_DISCONNECT;
      if (f_mount(NULL, "", 0) != FR_OK) 
      {
        // usb_printf("ERROR: deinit fatfs fail.");
        // LOGI("mount deinit fail.");
      }
      FATFS_UnLinkDriver(USBHPath);
      TERN_(EXTENSIBLE_UI, ExtUI::onUsbRemoved());
      if(infoMenu.menu[infoMenu.cur] == menuPrintUdisk) infoMenu.cur--;
      udisk.ResetPin();
      // LOGI("usbh disconnect");
      // usb_printf("usb device disconnected. \r\n");
      break;
    
    case HOST_USER_RESET_ENUM:
      f_mount(NULL, "", 0);
      FATFS_UnLinkDriver(USBHPath);
      break;

    // active usb
    case HOST_USER_CLASS_ACTIVE:
      Appli_state = APPLICATION_READY;
      FATFS_LinkDriver(&USBH_Driver, USBHPath);
      if (f_mount(&USBHFatFS, "", 0) != FR_OK) {}
      TERN_(EXTENSIBLE_UI, ExtUI::onUsbInserted());
      TERN_(POWER_LOSS_RECOVERY, recovery.check_u());

      // LOGI("usb class active. ");
      break;

    // connection usb
    case HOST_USER_CONNECTION:
      Appli_state = APPLICATION_START;
      // LOGI("usbh connect");
      // extern const Diskio_drvTypeDef  USBH_Driver;
      // FATFS_LinkDriver(&USBH_Driver, USBHPath);
      // LOGI("usbh udisk path:%s", USBHPath);
      // FRESULT res = f_mkfs(usbh_disk_path, FM_FAT32, 4096, 
      //                     working_buffer, 
      //                     sizeof(working_buffer));

      // LOGI("format fs result:%d", res);

      // if (f_mount(&USBHFatFS, "", 0) != FR_OK) {
      //   // usb_printf("ERROR: mount usb fatfs fail");
      //   // LOGE("usbh connect mount fail");
      // }

      // TERN_(EXTENSIBLE_UI, ExtUI::onUsbInserted());
      //DONT just operate fs here!! usbh fsm should run to switch 
      //correct state!!!
      // extern FRESULT Explore_Disk(char *path, uint8_t recu_level);
      // Explore_Disk(USBHPath, 1);
      break;

    default:
      break;
  }
  /* USER CODE END CALL_BACK_1 */
}

// void stm32_usbh_msc_init()
// {
//     // common_usb_fs_rcc_gpio_ll_init();
//     // common_usb_fs_hs_nvic_init();
//     MX_USB_HOST_Init();
//     MX_FATFS_Init();
// }

#if 1//defined(USE_GD32)

#ifdef __cplusplus
extern "C"{
#endif
// irq 
void OTG_FS_IRQHandler()
{
    HAL_HCD_IRQHandler(&hhcd_USB_OTG_FS);
}

// void OTG_HS_IRQHandler(void)
// {
//     HAL_PCD_IRQHandler(&hhcd_USB_OTG_HS);
// }
#ifdef __cplusplus
}
#endif
#endif

#endif // HAS_UDISK
