
#pragma once

#ifdef __cplusplus
extern "C"{
#endif

void common_usb_fs_rcc_gpio_ll_init();
void common_usb_hs_rcc_gpio_ll_init();
void common_usb_fs_hs_nvic_init();

// void xx32_usb_device_cdc_init();
// void xx32_usb_host_msc_init();
// void gd32_usb_loop();
// uint8_t gd32_cdc_is_connected();

// void gd32_cdc_continue_transmit(void);
// uint8_t gd32_cdc_resume_receive(void);

#ifdef __cplusplus
}
#endif