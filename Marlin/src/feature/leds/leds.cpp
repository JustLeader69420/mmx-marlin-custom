/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
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

/**
 * leds.cpp - Marlin RGB LED general support
 */

#include "../../inc/MarlinConfig.h"

#if HAS_COLOR_LEDS

#include "leds.h"

#if ENABLED(BLINKM)
  #include "blinkm.h"
#endif

#if ENABLED(PCA9632)
  #include "pca9632.h"
#endif

#if ENABLED(PCA9533)
  #include "pca9533.h"
#endif

#if ENABLED(LED_COLOR_PRESETS)
  const LEDColor LEDLights::defaultLEDColor = MakeLEDColor(
    LED_USER_PRESET_RED, LED_USER_PRESET_GREEN, LED_USER_PRESET_BLUE,
    LED_USER_PRESET_WHITE, LED_USER_PRESET_BRIGHTNESS
  );
#endif

#if EITHER(LED_CONTROL_MENU, PRINTER_EVENT_LEDS)
  LEDColor LEDLights::color;
  bool LEDLights::lights_on;
#endif

LEDLights leds;

void LEDLights::setup() {
  #if EITHER(RGB_LED, RGBW_LED)
    if (PWM_PIN(RGB_LED_R_PIN)) SET_PWM(RGB_LED_R_PIN); else SET_OUTPUT(RGB_LED_R_PIN);
    if (PWM_PIN(RGB_LED_G_PIN)) SET_PWM(RGB_LED_G_PIN); else SET_OUTPUT(RGB_LED_G_PIN);
    if (PWM_PIN(RGB_LED_B_PIN)) SET_PWM(RGB_LED_B_PIN); else SET_OUTPUT(RGB_LED_B_PIN);
    #if ENABLED(RGBW_LED)
      if (PWM_PIN(RGB_LED_W_PIN)) SET_PWM(RGB_LED_W_PIN); else SET_OUTPUT(RGB_LED_W_PIN);
    #endif
  #endif
  TERN_(NEOPIXEL_LED, neo.init());
  TERN_(PCA9533, PCA9533_init());
  TERN_(LED_USER_PRESET_STARTUP, set_default());
}

void LEDLights::set_color(const LEDColor &incol
  #if ENABLED(NEOPIXEL_LED)
    , bool isSequence/*=false*/
  #endif
) {

  #if ENABLED(NEOPIXEL_LED)

    const uint32_t neocolor = LEDColorWhite() == incol
                            ? neo.Color(NEO_WHITE)
                            : neo.Color(incol.r, incol.g, incol.b, incol.w);
    static uint16_t nextLed = 0;

    #ifdef NEOPIXEL_BKGD_LED_INDEX
      if (NEOPIXEL_BKGD_LED_INDEX == nextLed) {
        if (++nextLed >= neo.pixels()) nextLed = 0;
        return;
      }
    #endif

    neo.set_brightness(incol.i);

    if (isSequence) {
      neo.set_pixel_color(nextLed, neocolor);
      neo.show();
      if (++nextLed >= neo.pixels()) nextLed = 0;
      return;
    }

    neo.set_color(neocolor);

  #endif

  #if ENABLED(BLINKM)

    // This variant uses i2c to send the RGB components to the device.
    blinkm_set_led_color(incol);

  #endif

  #if EITHER(RGB_LED, RGBW_LED)

    // This variant uses 3-4 separate pins for the RGB(W) components.
    // If the pins can do PWM then their intensity will be set.
    #define UPDATE_RGBW(C,c) do {                       \
      if (PWM_PIN(RGB_LED_##C##_PIN))                   \
        analogWrite(pin_t(RGB_LED_##C##_PIN), incol.c); \
      else                                              \
        WRITE(RGB_LED_##C##_PIN, incol.c ? HIGH : LOW); \
    }while(0)
    UPDATE_RGBW(R,r); UPDATE_RGBW(G,g); UPDATE_RGBW(B,b);
    #if ENABLED(RGBW_LED)
      UPDATE_RGBW(W,w);
    #endif

  #endif

  // Update I2C LED driver
  TERN_(PCA9632, PCA9632_set_led_color(incol));
  TERN_(PCA9533, PCA9533_set_rgb(incol.r, incol.g, incol.b));

  #if EITHER(LED_CONTROL_MENU, PRINTER_EVENT_LEDS)
    // Don't update the color when OFF
    lights_on = !incol.is_off();
    if (lights_on) color = incol;
  #endif
}

#if ENABLED(LED_CONTROL_MENU)
  void LEDLights::toggle() { if (lights_on) set_off(); else update(); }
#endif

#ifdef LED_BACKLIGHT_TIMEOUT

  millis_t LEDLights::led_off_time; // = 0

  void LEDLights::update_timeout(const bool power_on) {
    const millis_t ms = millis();
    if (power_on)
      reset_timeout(ms);
    else if (ELAPSED(ms, led_off_time))
      set_off();
  }

#endif

#if ENABLED(NEOPIXEL2_SEPARATE)

  #if ENABLED(NEO2_COLOR_PRESETS)
    const LEDColor LEDLights2::defaultLEDColor = MakeLEDColor(
      NEO2_USER_PRESET_RED, NEO2_USER_PRESET_GREEN, NEO2_USER_PRESET_BLUE,
      NEO2_USER_PRESET_WHITE, NEO2_USER_PRESET_BRIGHTNESS
    );
  #endif

  #if ENABLED(LED_CONTROL_MENU)
    LEDColor LEDLights2::color;
    bool LEDLights2::lights_on;
  #endif

  LEDLights2 leds2;

  void LEDLights2::setup() {
    neo2.init();
    TERN_(NEO2_USER_PRESET_STARTUP, set_default());
  }

  void LEDLights2::set_color(const LEDColor &incol) {
    const uint32_t neocolor = LEDColorWhite() == incol
                            ? neo2.Color(NEO2_WHITE)
                            : neo2.Color(incol.r, incol.g, incol.b, incol.w);
    neo2.set_brightness(incol.i);
    neo2.set_color(neocolor);

    #if ENABLED(LED_CONTROL_MENU)
      // Don't update the color when OFF
      lights_on = !incol.is_off();
      if (lights_on) color = incol;
    #endif
  }

  #if ENABLED(LED_CONTROL_MENU)
    void LEDLights2::toggle() { if (lights_on) set_off(); else update(); }
  #endif

#endif  // NEOPIXEL2_SEPARATE

  #if ENABLED(R_B_LED)
    #include "leds.h"
    LEDLights3 led3;
  #endif

  #if ENABLED(WS2812_LED)
    #include "leds.h"
  
    LEDLights4 led4;
    static bool the_led4_unlock = true;
    static int leds4_mode = AUTO_MODE;

    void LEDLights4::setup()
    {
      OUT_WRITE(WS2812_LED_PIN, LOW);
      __HAL_RCC_TIM13_CLK_ENABLE();       // 使能TIM13的时钟
      TIM13->CNT = 0;                     // counter register计数寄存器清零
      TIM13->PSC = 1 - 1;                 // prescaler定时器的预分频，注意要减一
      // TIM3->DIER |= 1<<0;
      TIM13->SR = (uint16_t)~(1 << 0);    // status register配置状态寄存器
    }
    
    // total 2.5us, run in 400Khz
    // #define NEOPIXEL_T0H_US 0.35  // Neopixel code 0 high level hold time in us
    // #define NEOPIXEL_T1H_US 2.15  // Neopixel code 1 high level hold time in us
    #define NEOPIXEL_T0H_US 0.3  // Neopixel code 0 high level hold time in us
    #define NEOPIXEL_T1H_US 0.6  // Neopixel code 1 high level hold time in us
    #define NEOPIXEL_CYCLE  6.0
    #define LED4_NUM EXTRUDERS
    void WS2812_Send_DAT(uint32_t *ws2812_dat)
    {
      if(the_led4_unlock){
        the_led4_unlock = false;
        uint16_t led_num;
        int8_t bit;
        uint16_t cycle = (HAL_RCC_GetPCLK1Freq()/1000000) * (NEOPIXEL_CYCLE) * 2 - 1;  // Neopixel frequency
        uint16_t code_0_tim_h_cnt = cycle * (NEOPIXEL_T0H_US / (NEOPIXEL_CYCLE));  // Code 0, High level hold time,
        uint16_t code_1_tim_h_cnt = cycle * (NEOPIXEL_T1H_US / (NEOPIXEL_CYCLE));
        uint32_t color = 0;

        __disable_irq();  // Disable interrupt, avoid disturbing the timing of WS2812
        TIM13->ARR = cycle;
        TIM13->CR1 |= 0x01;   // 使能定时器
        for (led_num=0; led_num < LED4_NUM; led_num++)
        {
          color = ws2812_dat[led_num] & 0xffffff;
          while (!(TIM13->SR & 0x01));
          TIM13->SR = 0;
         #if 0
          for (bit = 0; bit < 24; bit++)
          {
            TIM13->CNT = 0;
            WRITE(WS2812_LED_PIN, HIGH);  // WS2812 required very high speed, so "GPIO_SetLevel(LED_COLOR_PIN, 1)" not applicable
            if (((color>>bit) & 0x01))
            {
              while (TIM13->CNT < code_1_tim_h_cnt);
            }
            else
            {
              while (TIM13->CNT < code_0_tim_h_cnt);
            }
            WRITE(WS2812_LED_PIN, LOW);
            while (!TIM13->SR);
            TIM13->SR = 0;
          }
         #else
          for (bit = 23; bit >= 0; bit--)
          {
            TIM13->CNT = 0;
            WRITE(WS2812_LED_PIN, HIGH);  // WS2812 required very high speed, so "GPIO_SetLevel(LED_COLOR_PIN, 1)" not applicable
            if (color & (1 << bit))
            {
              while (TIM13->CNT < code_1_tim_h_cnt);
            }
            else
            {
              while (TIM13->CNT < code_0_tim_h_cnt);
            }
            WRITE(WS2812_LED_PIN, LOW);
            while (!(TIM13->SR & 0x01));
            // while(TIM13->CNT < (cycle-1));
            TIM13->SR = 0;
          }
         #endif
        }
        TIM13->CR1 &= ~0x01;
        __enable_irq();  // Enable interrupt
        the_led4_unlock = true;
      }
    }
    
    void LEDLights4::set_color(uint32_t color)
    {
      uint32_t color_str[LED4_NUM] = {0};
      for(uint16_t i=0; i<LED4_NUM; i++){
        color_str[i] = color;
      }
      WS2812_Send_DAT(color_str);
    }
    void LEDLights4::set_color(uint32_t *color)
    {
      WS2812_Send_DAT(color);
    }
    uint32_t LEDLights4::get_temperature_color(float temp){
      uint32_t color = 0;
      uint8_t t_red = 0;
      uint8_t t_green = 0;
      uint8_t t_blue = 0;
      // 30摄氏度及以下显示蓝色
      // 31~60 温度越高，红色越深
      // 61~180 温度越高，蓝色越浅
      // 181~220 温度越高，绿色越深，红->橙->黄
      // 221~270 温度越高，绿色蓝色越深，黄->白
      // 大于270 显示白色
      // G R B : <<16 <<8 <<0
      if(temp<=30){ color = 0x0000ff; }
      else if(temp>30 && temp<=60){
        t_red = (uint8_t)((temp-30)*8.5);   // 255/30=8.5 => 0xff/30
        color = 0x0000ff | (t_red<<8);
      }
      else if(temp>60 && temp<=180){
        t_blue = (uint8_t)(0xff-(temp-60)*2.125);  //255/120=2.125 => ff/120
        color = 0x00ff00 | (t_blue);
      }
      else if(temp>180 && temp<=270){
        t_green = (uint8_t)((temp-180)*2.5);  // 200/80=2.5

        if(temp>220 && temp<=270){
          t_blue = (uint8_t)((temp-220)*5.1); // 255/50=5.1
        }else{
          t_blue = 0;
        }

        color = 0x00ff00 | (t_green<<16) | (t_blue);
      }
      // else if(temp>220 && temp<=270){
      //   t_green = (uint8_t)(0xff-(temp-120)*4.25);
      //   color = 0x00ff00 | (t_green<<16);
      // }
      else if(temp>270){ color =  0xffffff; }

      return color;
    }
    // 模式更替
    void LEDLights4::set_led_mode(uint32_t mode){
      if(mode > AUTO_MODE) return;
      leds4_mode = mode;
    }
    int LEDLights4::get_led_mode(){
        return leds4_mode;
    }
  #endif // W2812_LED
  
#endif  // HAS_COLOR_LEDS
