
#pragma once

#ifdef __cplusplus
extern "C"{
#endif

void gd32_usb_device_cdc_init();
void gd32_usb_host_msc_init();
void gd32_usb_loop();
uint8_t gd32_cdc_is_connected();

void gd32_cdc_continue_transmit(void);
uint8_t gd32_cdc_resume_receive(void);

// extern uint8_t udiskMounted;

#ifdef __cplusplus
}
#endif