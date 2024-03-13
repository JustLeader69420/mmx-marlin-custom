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
#pragma once

/**
 * STM32F407VET6 with RAMPS-like shield
 * 'Black' STM32F407VET6 board - https://www.stm32duino.com/viewtopic.php?t=485
 * Shield - https://github.com/jmz52/Hardware
 */

#if NOT_TARGET(STM32F4, STM32F4xx)
  #error "Oops! Select an STM32F4 board in 'Tools > Board.'"
#elif HOTENDS > 2 || E_STEPPERS > 2
  #error "Black STM32F4VET6 supports up to 2 hotends / E-steppers."
#endif

#ifndef BOARD_INFO_NAME
  #define BOARD_INFO_NAME "MD langgo STM32F4XX"
#endif

#define DEFAULT_MACHINE_NAME "MD D301 STM32F407ZG"

//#define I2C_EEPROM
#define SRAM_EEPROM_EMULATION
#define MARLIN_EEPROM_SIZE                0x2000  // 8KB

//
// Servos
//
// #define SERVO0_PIN                          PC6
// #define SERVO1_PIN                          PC7
#define CALIB_PIN          PB1

//
// Z Probe must be this pins
//
#define Z_MIN_PROBE_PIN    PB0

//
// Limit Switches
//
#define X_MIN_PIN                           PE10
// #define X_MAX_PIN                           PA15

#define Y_MIN_PIN                           PE11
// #define Y_MAX_PIN                           PD12

#ifdef Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN
  #define Z_MIN_PIN          PE12
#else
  #define Z_MIN_PIN          PE12
#endif
// #define Z_MIN_PIN                           PG0
// #define Z_MAX_PIN                           PD15

#define LED_RED_PIN               PE13

//
// Steppers
//
#define X_ENABLE_PIN       PD3
#define X_STEP_PIN         PD2
#define X_DIR_PIN          PC10

#define Y_ENABLE_PIN       PC12
#define Y_STEP_PIN         PA10
#define Y_DIR_PIN          PA9

#define Z_ENABLE_PIN       PC9
#define Z_STEP_PIN         PG8
#define Z_DIR_PIN          PG7

#define E0_ENABLE_PIN      PD13
#define E0_STEP_PIN        PD12
#define E0_DIR_PIN         PD11

#define Z2_ENABLE_PIN      PG5
#define Z2_STEP_PIN        PG4
#define Z2_DIR_PIN         PG3

/**
 * TMC2208/TMC2209 stepper drivers
 */
#if HAS_TMC220x
  //
  // Software serial ASYNC half duplex?
  //
  #define X_SERIAL_TX_PIN  PD6
  #define X_SERIAL_RX_PIN  PD6

  #define Y_SERIAL_TX_PIN  PC11
  #define Y_SERIAL_RX_PIN  PC11

  #define Z_SERIAL_TX_PIN  PA8
  #define Z_SERIAL_RX_PIN  PA8

  #define E0_SERIAL_TX_PIN PG2
  #define E0_SERIAL_RX_PIN PG2

  #define Z2_SERIAL_TX_PIN PG6
  #define Z2_SERIAL_RX_PIN PG6

  // Reduce baud rate to improve software serial reliability
  #define TMC_BAUD_RATE 19200
#endif

//
// Filament Sensor
//
#ifndef FIL_RUNOUT_PIN
  #define FIL_RUNOUT_PIN   PG1
#endif

//
// Power Supply Control
//
#ifndef PS_ON_PIN
  #define PS_ON_PIN        PA1
#endif

//
// Temperature Sensors
//
#define TEMP_0_PIN                          PC4   // T0
#define TEMP_BED_PIN                        PC5   // TB

#ifndef TEMP_CHAMBER_PIN
  #define TEMP_CHAMBER_PIN                  PC3   // TC
#endif

//
// Heaters / Fans
//
#define HEATER_0_PIN       PC6
#define HEATER_BED_PIN     PC8

#define FAN_PIN            PC7


//
// USB connect control
//
#define USB_CONNECT_PIN    PA15
#define USB_CONNECT_INVERTING false

#define SD_DETECT_PIN      PE3
#define BEEPER_PIN         PE15

#define NEOPIXEL_PIN       PB9

//
// Misc. Functions
//
#define LED_PIN                             PE13
#define LED1_PIN                             PE14
#define LED2_PIN                             PE15
// #define KILL_PIN                            PB1

//
// LCD / Controller
//
//#define SD_DETECT_PIN                     PC5
//#define SD_DETECT_PIN                     PA8   // SDIO SD_DETECT_PIN, external SDIO card reader only


/**
 * Note: Alfawise screens use various TFT controllers. Supported screens
 * are based on the ILI9341, ILI9328 and ST7798V. Define init sequences for
 * other screens in u8g_dev_tft_320x240_upscale_from_128x64.cpp
 *
 * If the screen stays white, disable 'LCD_RESET_PIN' to let the bootloader
 * init the screen.
 *
 * Setting an 'LCD_RESET_PIN' may cause a flicker when entering the LCD menu
 * because Marlin uses the reset as a failsafe to revive a glitchy LCD.
 */

#define LCD_BACKLIGHT_PIN  PG10
#define FSMC_CS_PIN        PD7   // FSMC_NE1
#define FSMC_RS_PIN        PE2   // A23 Register. Only one address needed

#define LCD_USE_DMA_FSMC   // Use DMA transfers to send data to the TFT
#define FSMC_DMA_DEV       DMA1
#define FSMC_DMA_CHANNEL   DMA_CH4  //fixme in F4

#define DOGLCD_MOSI        -1  // Prevent auto-define by Conditionals_post.h
#define DOGLCD_SCK         -1

// #define LCD_PINS_RS                         PE15
// #define LCD_PINS_ENABLE                     PD8
// #define LCD_PINS_D4                         PE10
// #define LCD_PINS_D5                         PE12
// #define LCD_PINS_D6                         PD1
// #define LCD_PINS_D7                         PE8
// #define BTN_ENC                             PD9
// #define BTN_EN1                             PD4
// #define BTN_EN2                             PD13



//
// Onboard SD support
//

#define HAS_ONBOARD_SD

#define SDIO_D0_PIN                         PC8
#define SDIO_D1_PIN                         PC9
#define SDIO_D2_PIN                         PC10
#define SDIO_D3_PIN                         PC11
#define SDIO_CK_PIN                         PC12
#define SDIO_CMD_PIN                        PD2

#ifndef SDCARD_CONNECTION
  #define SDCARD_CONNECTION              ONBOARD
#endif

#if SD_CONNECTION_IS(ONBOARD)
  // #define SDIO_SUPPORT                            // Use SDIO for onboard SD

  #ifndef SDIO_SUPPORT
    #define SOFTWARE_SPI                          // Use soft SPI for onboard SD
    #define SDSS                     SDIO_D3_PIN  //A4
    #define SCK_PIN                  SDIO_CK_PIN  //A5
    #define MISO_PIN                 SDIO_D0_PIN  //A6
    #define MOSI_PIN                SDIO_CMD_PIN  //A7
  #endif
#endif
