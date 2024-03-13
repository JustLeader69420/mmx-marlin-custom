/**
 * Marlin 3D Printer Firmware
 *
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 * Copyright (c) 2016 Bob Cousins bobcousins42@googlemail.com
 * Copyright (c) 2015-2016 Nico Tonnhofer wurstnase.reprap@gmail.com
 * Copyright (c) 2017 Victor Perez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#if defined(ARDUINO_ARCH_STM32) && !defined(STM32GENERIC)

#include "HAL.h"
#include "usb_serial.h"

#include "../../inc/MarlinConfig.h"
#include "../shared/Delay.h"

#if ENABLED(SRAM_EEPROM_EMULATION)
  #if STM32F7xx
    #include <stm32f7xx_ll_pwr.h>
  #elif STM32F4xx
    #include <stm32f4xx_ll_pwr.h>
  #else
    #error "SRAM_EEPROM_EMULATION is currently only supported for STM32F4xx and STM32F7xx"
  #endif
#endif

// ------------------------
// Public Variables
// ------------------------

uint16_t HAL_adc_result;

// ------------------------
// Public functions
// ------------------------

// Needed for DELAY_NS() / DELAY_US() on CORTEX-M7
#if (defined(__arm__) || defined(__thumb__)) && __CORTEX_M == 7
  // HAL pre-initialization task
  // Force the preinit function to run between the premain() and main() function
  // of the STM32 arduino core
  __attribute__((constructor (102)))
  void HAL_preinit() {
    enableCycleCounter();
  }
#endif

// HAL initialization task
void HAL_init() {
  FastIO_init();

  #if ENABLED(SDSUPPORT) && DISABLED(SDIO_SUPPORT) && (defined(SDSS) && SDSS != -1)
    OUT_WRITE(SDSS, HIGH); // Try to set SDSS inactive before any other SPI users start up
  #endif

  #if PIN_EXISTS(LED)
    OUT_WRITE(LED_PIN, LOW); //test pass
  #endif
  #if PIN_EXISTS(CAP_TOUCH)
    SET_INPUT(CAP_TOUCH_PIN);
  #endif

  #if ENABLED(SRAM_EEPROM_EMULATION)
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();           // Enable access to backup SRAM
    __HAL_RCC_BKPSRAM_CLK_ENABLE();
    LL_PWR_EnableBkUpRegulator();         // Enable backup regulator
    while (!LL_PWR_IsActiveFlag_BRR());   // Wait until backup regulator is initialized
  #endif

  SetTimerInterruptPriorities();

  TERN_(EMERGENCY_PARSER, USB_Hook_init());
}

void HAL_clear_reset_source() { __HAL_RCC_CLEAR_RESET_FLAGS(); }

uint8_t HAL_get_reset_source() {
  return
    #ifdef RCC_FLAG_IWDGRST // Some sources may not exist...
      RESET != __HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)  ? RST_WATCHDOG :
    #endif
    #ifdef RCC_FLAG_IWDG1RST
      RESET != __HAL_RCC_GET_FLAG(RCC_FLAG_IWDG1RST) ? RST_WATCHDOG :
    #endif
    #ifdef RCC_FLAG_IWDG2RST
      RESET != __HAL_RCC_GET_FLAG(RCC_FLAG_IWDG2RST) ? RST_WATCHDOG :
    #endif
    #ifdef RCC_FLAG_SFTRST
      RESET != __HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)   ? RST_SOFTWARE :
    #endif
    #ifdef RCC_FLAG_PINRST
      RESET != __HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)   ? RST_EXTERNAL :
    #endif
    #ifdef RCC_FLAG_PORRST
      RESET != __HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)   ? RST_POWER_ON :
    #endif
    0
  ;
}

void _delay_ms(const int delay_ms) { delay(delay_ms); }

extern "C" {
  extern unsigned int _ebss; // end of bss section
}

// ------------------------
// ADC
// ------------------------

// TODO: Make sure this doesn't cause any delay
void HAL_adc_start_conversion(const uint8_t adc_pin) {
  #if ENABLED(NEW_BOARD)
    HAL_adc_result = Get_Adc(adc_pin);
  #else
    HAL_adc_result = analogRead(adc_pin);
  #endif
}
uint16_t HAL_adc_get_result() { return HAL_adc_result; }

// Reset the system (to initiate a firmware flash)
void flashFirmware(const int16_t) { NVIC_SystemReset(); }

// Maple Compatibility
systickCallback_t systick_user_callback;
void systick_attach_callback(systickCallback_t cb) { systick_user_callback = cb; }
void HAL_SYSTICK_Callback() { if (systick_user_callback) systick_user_callback(); }

/******************new board(PRT_MC)********************/
  #if ENABLED(NEW_BOARD)
    ADC_HandleTypeDef ADC_Handler;//ADC 句柄
    // #define HOTONE_TEMP_PIN ADC_Handler[0]
    uint8_t get_AdcPinNum(const uint8_t adc_pin){
      switch (adc_pin)
      {
        case PF5: return 1;
          break;

        case PF6: return 0;
          break;

        case PF7: return 2;
          break;

        case PF8: return 3;
          break;

        case PF9: return 4;
          break;

        default: break;
      }
      return 0;
    }
    //初始化ADC 
    //ch: ADC_channels   
    //通道值  0~16 取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16 
    void MY_ADC_Configuration(ADC_TypeDef * adc_num){
      ADC_Handler.Instance=adc_num; 
      ADC_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;   //4 分频，ADCCLK=PCLK2/4=90/4=22.5MHZ 
      ADC_Handler.Init.Resolution=ADC_RESOLUTION_10B;  //12位模式 
      ADC_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;  //右对齐 
      ADC_Handler.Init.ScanConvMode=DISABLE;        //非扫描模式 
      ADC_Handler.Init.EOCSelection=DISABLE;        //关闭 EOC 中断 
      ADC_Handler.Init.ContinuousConvMode=DISABLE;    //关闭连续转换 
      ADC_Handler.Init.NbrOfConversion=1;    //1 个转换在规则序列中 也就是只转换规则序列 1   
      ADC_Handler.Init.DiscontinuousConvMode=DISABLE;    //禁止不连续采样模式 
      ADC_Handler.Init.NbrOfDiscConversion=0;        //不连续采样通道数为 0 
      ADC_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;  //软件触发 
      ADC_Handler.Init.ExternalTrigConvEdge= ADC_EXTERNALTRIGCONVEDGE_NONE;//使用软件触发 
      ADC_Handler.Init.DMAContinuousRequests=DISABLE;         //关闭DMA请求

      HAL_ADC_Init(&ADC_Handler);                           //初始化   
    }
    void MY_ADC3_Init(const uint8_t adc_pin){
      __HAL_RCC_ADC3_CLK_ENABLE();
      pinmap_pinout(digitalPinToPinName(adc_pin), PinMap_ADC_1);
    }
    void MY_ADC_Init_P(const uint8_t adc_pin){
      uint8_t num = 0;
      GPIO_InitTypeDef GPIO_Initure;
      __HAL_RCC_ADC3_CLK_ENABLE();            //Ê¹ÄÜADC3Ê±ÖÓ
      __HAL_RCC_GPIOF_CLK_ENABLE();						//¿ªÆôGPIOFÊ±ÖÓ
      num = get_AdcPinNum(adc_pin);

      ADC_Handler.Instance=ADC3; 
      ADC_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;   //4 分频，ADCCLK=PCLK2/4=90/4=22.5MHZ 
      ADC_Handler.Init.Resolution=ADC_RESOLUTION_10B;  //12位模式 
      ADC_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;  //右对齐 
      ADC_Handler.Init.ScanConvMode=DISABLE;        //非扫描模式 
      ADC_Handler.Init.EOCSelection=DISABLE;        //关闭 EOC 中断 
      ADC_Handler.Init.ContinuousConvMode=DISABLE;    //关闭连续转换 
      ADC_Handler.Init.NbrOfConversion=1;    //1 个转换在规则序列中 也就是只转换规则序列 1   
      ADC_Handler.Init.DiscontinuousConvMode=DISABLE;    //禁止不连续采样模式 
      ADC_Handler.Init.NbrOfDiscConversion=0;        //不连续采样通道数为 0 
      ADC_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;  //软件触发 
      ADC_Handler.Init.ExternalTrigConvEdge= ADC_EXTERNALTRIGCONVEDGE_NONE;//使用软件触发 
      ADC_Handler.Init.DMAContinuousRequests=DISABLE;         //关闭DMA请求

      // __HAL_RCC_ADC3_CLK_ENABLE();
      // pinmap_pinout(digitalPinToPinName(adc_pin), PinMap_ADC_1);

      HAL_ADC_Init(&ADC_Handler);                           //初始化

    
      GPIO_Initure.Pin=GPIO_PIN_9;            //PF9
      GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //Ä£Äâ
      GPIO_Initure.Pull=GPIO_NOPULL;          //²»´øÉÏÏÂÀ­
      HAL_GPIO_Init(GPIOF,&GPIO_Initure);
    }
    //ADC 底层驱动，引脚配置，时钟使能 
    //此函数会被HAL_ADC_Init()调用 
    //hadc:ADC句柄 
    // void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) 
    // {
    //   GPIO_InitTypeDef GPIO_Initure; 
    //   __HAL_RCC_ADC1_CLK_ENABLE();    //使能 ADC1时钟 
    //   __HAL_RCC_GPIOA_CLK_ENABLE();    //开启 GPIOA时钟 
    //   GPIO_Initure.Pin=GPIO_PIN_5;        //PA5 
    //   GPIO_Initure.Mode=GPIO_MODE_ANALOG;  //模拟 
    //   GPIO_Initure.Pull=GPIO_NOPULL;      //不带上下拉 
    //   HAL_GPIO_Init(GPIOA,&GPIO_Initure); 
    // }

    // 获取ADC的通道
    uint32_t get_adc_channel(PinName pin, const my_PinMap_ADC *map){
      while (map->pin != NC) {
        if (map->pin == pin) {
          return map->channel;
        }
        map++;
      }
      return 0;
    }
    //获得ADC 值 
    //ch: 通道值  0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16 
    //返回值:转换结果 
    uint16_t Get_Adc(const uint8_t adc_pin){
      uint8_t num = 0;
      num = get_AdcPinNum(adc_pin);

      ADC_ChannelConfTypeDef ADC_ChanConf;
      ADC_ChanConf.Channel= get_adc_channel(digitalPinToPinName(adc_pin), ADC_Channel);          //通道
      ADC_ChanConf.Rank=1;                                    //第1 个序列，序列1
      ADC_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES;     //采样时间
      ADC_ChanConf.Offset=0;
      HAL_ADC_ConfigChannel(&ADC_Handler, &ADC_ChanConf);//通道配置 
      HAL_ADC_Start(&ADC_Handler);                       //开启 ADC 
      HAL_ADC_PollForConversion(&ADC_Handler, 10);       //轮询转换 
      return (uint16_t)HAL_ADC_GetValue(&ADC_Handler);   //返回最近一次ADC1规则组的转换结果 
    }
  #endif

#endif // ARDUINO_ARCH_STM32 && !STM32GENERIC
