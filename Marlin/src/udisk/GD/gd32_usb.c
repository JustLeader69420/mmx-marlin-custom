#if 0//defined(USE_GD32)

// #include <gd32f4xx.h>
#include "drv_usb_hw.h"
#include "drv_usb_core.h"
#include "gdusbd_core.h"
#include "gdusbh_core.h"
#include "gdusbh_usr.h"
#include "usbh_msc_core.h"
#include "cdc_acm_core.h"
#include "drv_usbh_int.h"
#include "drv_usbd_int.h"
// #include "rtt.h"
// #include "log.h"
#include "stm32yyxx_ll_pwr.h"   //use stm32 common rcc & nvic
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"
#include "gd32_cdc_queue.h"
#include "usb_common_init.h"


usbh_host usb_host_msc;     
usb_core_driver usbh_core;  //usb fs as host
usb_core_driver cdc_acm;    //usb hs as device
usb_cdc_handler *pcdc;




void gd32_usb_device_cdc_init()
{
    common_usb_hs_rcc_gpio_ll_init();
    //usb device cdc on USB_HS
    usbd_init (&cdc_acm,
              USB_CORE_ENUM_HS,
              &cdc_desc,
              &cdc_class);
}

void gd32_usb_host_msc_init()
{
    common_usb_fs_rcc_gpio_ll_init();
    /* register device class */
    usbh_class_register(&usb_host_msc, &usbh_msc);

    //usb host msc on USB_FS
    usbh_init(&usb_host_msc,
              &usbh_core,
              USB_CORE_ENUM_FS,
              &usr_cb);

    common_usb_fs_hs_nvic_init();

    MX_FATFS_Init();
}
#if 0 
//du
void cdc_write(usb_dev *udev, uint8_t *buf, uint32_t len)
{
  usbd_ep_send (udev, CDC_DATA_IN_EP, buf, len);
}

void cdc_read(usb_dev *udev, uint8_t *buf)
{
  // usbd_ep_recev(udev, CDC_DATA_OUT_EP, buf, USB_CDC_DATA_PACKET_SIZE);
  usb_cdc_handler *cdc = (usb_cdc_handler *)udev->dev.class_data[CDC_COM_INTERFACE];
  memcpy(buf, (uint8_t*)cdc->data, cdc->receive_length);
}

uint8_t gd32_cdc_resume_receive(void)
{

  /*
   * TS: main and IRQ threads can't pass it at same time, because
   * IRQ may occur only if receivePended is true. So it is thread-safe!
   */
  // if (!receivePended) {

  uint8_t *block = CDC_ReceiveQueue_ReserveBlock(&ReceiveQueue);
  if (block != NULL) {
    // receivePended = 1;
    /* Set new buffer */
    // USBD_CDC_SetRxBuffer(&hUSBD_Device_CDC, block);
    // USBD_CDC_ReceivePacket(&hUSBD_Device_CDC);
    cdc_read(&cdc_acm, block);
    return 1;
  }
  // }
  return 0;
}

void gd32_cdc_continue_transmit(void)
{
  uint16_t size;
  uint8_t *buffer;
  
  /*
   * TS: This method can be called both in the main thread
   * (via USBSerial::write) and in the IRQ stream (via USBD_CDC_Transferred),
   * BUT the main thread cannot pass this condition while waiting for a IRQ!
   * This is not possible because TxState is not zero while waiting for data
   * transfer ending! The IRQ thread is uninterrupted, since its priority
   * is higher than that of the main thread. So this method is thread safe.
   */
  buffer = CDC_TransmitQueue_ReadBlock(&TransmitQueue, &size);
  if (size > 0) {
    cdc_write(&cdc_acm, buffer, size);
    // transmitStart = HAL_GetTick();
    // USBD_CDC_SetTxBuffer(&hUSBD_Device_CDC, buffer, size);
    /*
      * size never exceed PMA buffer and USBD_CDC_TransmitPacket make full
      * copy of block in PMA, so no need to worry about buffer damage
      */
    // USBD_CDC_TransmitPacket(&hUSBD_Device_CDC);
  }
}

void cdc_usr_tx (usb_dev *udev)
{
    usb_cdc_handler *cdc = (usb_cdc_handler *)udev->dev.class_data[CDC_COM_INTERFACE];

    if (0U != cdc->receive_length) {
        cdc->packet_sent = 0U;

        //将接收到的数据包存入rx-queue
        /* It always contains required amount of free space for writing */
        CDC_ReceiveQueue_CommitBlock(&ReceiveQueue, cdc->receive_length);
        uint8_t *block = CDC_ReceiveQueue_ReserveBlock(&ReceiveQueue);
        if (block != NULL) {
          cdc_read(&cdc_acm, block);  //read data stored in cdc->data in rx stage
        }

        // usbd_ep_send (udev, CDC_DATA_IN_EP, (uint8_t*)(cdc->data), cdc->receive_length);
        // demo时的回环
        cdc->receive_length = 0U;
    }

    //检查tx-queue非空 则说明有数据需要发送
    gd32_cdc_continue_transmit();
    // cdc->packet_sent = 0U;
    // usbd_ep_send (udev, CDC_DATA_IN_EP, (uint8_t*)(cdc->data), cdc->receive_length);
    
}

void cdc_usr_rx (usb_dev *udev)
{
    usb_cdc_handler *cdc = (usb_cdc_handler *)udev->dev.class_data[CDC_COM_INTERFACE];

    cdc->packet_receive = 0U;
    cdc->packet_sent = 0U;

    usbd_ep_recev(udev, CDC_DATA_OUT_EP, (uint8_t*)(cdc->data), USB_CDC_DATA_PACKET_SIZE);
}


uint8_t gd32_cdc_is_connected()
{
  return USBD_CONFIGURED == cdc_acm.dev.cur_status;
}
#endif



// volatile uint8_t receivePended = 1;

void gd32_usb_loop()
{
    usbh_core_task(&usb_host_msc);
#if 0
    if (USBD_CONFIGURED == cdc_acm.dev.cur_status) {
      if (0U == cdc_acm_check_ready(&cdc_acm)) {
        cdc_usr_rx(&cdc_acm);
      }
      else {
        cdc_usr_tx(&cdc_acm);
      }
        // if (0U == cdc_acm_check_ready(&cdc_acm)) {  //init or rx 1 pack && 1 pack pend to tx
            // cdc_acm_data_receive(&cdc_acm);
        // } else {
            // cdc_acm_data_send(&cdc_acm);
        // }
    }
#endif
}

// irq 
void OTG_FS_IRQHandler(void)
{
    usbh_isr(&usbh_core);
}
/* du
void OTG_HS_IRQHandler(void)
{
    usbd_isr(&cdc_acm);
}
*/

#endif