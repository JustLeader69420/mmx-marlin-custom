#pragma once

#include "usbh_core.h"

void MX_USB_HOST_Init(void);
void MX_USB_HOST_Process(void);
void MSC_MenuProcess(void);
short mountUdisk(void);
short umountUdisk(void);
HOST_StateTypeDef MX_USB_GET_State(void);
uint8_t usb_is_ready(void);

// extern uint8_t udiskMounted;
