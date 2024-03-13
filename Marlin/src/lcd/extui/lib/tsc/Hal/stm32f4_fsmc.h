#pragma once

#include <stdint.h>
#include "pinmap.h"
#include "../../../src/HAL/STM32/tft/tft_fsmc.h"
#include "../../../../tft_io/tft_io.h"


/*
 * Register bit definitions
 */

/* NOR/PSRAM chip-select control registers */

#define FSMC_BCR_CBURSTRW_BIT           19
#define FSMC_BCR_ASYNCWAIT_BIT          15
#define FSMC_BCR_EXTMOD_BIT             14
#define FSMC_BCR_WAITEN_BIT             13
#define FSMC_BCR_WREN_BIT               12
#define FSMC_BCR_WAITCFG_BIT            11
#define FSMC_BCR_WRAPMOD_BIT            10
#define FSMC_BCR_WAITPOL_BIT            9
#define FSMC_BCR_BURSTEN_BIT            8
#define FSMC_BCR_FACCEN_BIT             6
#define FSMC_BCR_MUXEN_BIT              1
#define FSMC_BCR_MBKEN_BIT              0

#define FSMC_BCR_CBURSTRW               (1U << FSMC_BCR_CBURSTRW_BIT)
#define FSMC_BCR_ASYNCWAIT              (1U << FSMC_BCR_ASYNCWAIT_BIT)
#define FSMC_BCR_EXTMOD                 (1U << FSMC_BCR_EXTMOD_BIT)
#define FSMC_BCR_WAITEN                 (1U << FSMC_BCR_WAITEN_BIT)
#define FSMC_BCR_WREN                   (1U << FSMC_BCR_WREN_BIT)
#define FSMC_BCR_WAITCFG                (1U << FSMC_BCR_WAITCFG_BIT)
#define FSMC_BCR_WRAPMOD                (1U << FSMC_BCR_WRAPMOD_BIT)
#define FSMC_BCR_WAITPOL                (1U << FSMC_BCR_WAITPOL_BIT)
#define FSMC_BCR_BURSTEN                (1U << FSMC_BCR_BURSTEN_BIT)
#define FSMC_BCR_FACCEN                 (1U << FSMC_BCR_FACCEN_BIT)
#define FSMC_BCR_MWID                   (0x3 << 4)
#define FSMC_BCR_MWID_8BITS             (0x0 << 4)
#define FSMC_BCR_MWID_16BITS            (0x1 << 4)
#define FSMC_BCR_MTYP                   (0x3 << 2)
#define FSMC_BCR_MTYP_SRAM              (0x0 << 2)
#define FSMC_BCR_MTYP_PSRAM             (0x1 << 2)
#define FSMC_BCR_MTYP_NOR_FLASH         (0x2 << 2)
#define FSMC_BCR_MUXEN                  (1U << FSMC_BCR_MUXEN_BIT)
#define FSMC_BCR_MBKEN                  (1U << FSMC_BCR_MBKEN_BIT)

/* SRAM/NOR-Flash chip-select timing registers */

#define FSMC_BTR_ACCMOD                 (0x3 << 28)
#define FSMC_BTR_ACCMOD_A               (0x0 << 28)
#define FSMC_BTR_ACCMOD_B               (0x1 << 28)
#define FSMC_BTR_ACCMOD_C               (0x2 << 28)
#define FSMC_BTR_ACCMOD_D               (0x3 << 28)
#define FSMC_BTR_DATLAT                 (0xF << 24)
#define FSMC_BTR_CLKDIV                 (0xF << 20)
#define FSMC_BTR_BUSTURN                (0xF << 16)
#define FSMC_BTR_DATAST                 (0xFF << 8)
#define FSMC_BTR_ADDHLD                 (0xF << 4)
#define FSMC_BTR_ADDSET                 0xF

/* SRAM/NOR-Flash write timing registers */

#define FSMC_BWTR_ACCMOD                 (0x3 << 28)
#define FSMC_BWTR_ACCMOD_A               (0x0 << 28)
#define FSMC_BWTR_ACCMOD_B               (0x1 << 28)
#define FSMC_BWTR_ACCMOD_C               (0x2 << 28)
#define FSMC_BWTR_ACCMOD_D               (0x3 << 28)
#define FSMC_BWTR_DATLAT                 (0xF << 24)
#define FSMC_BWTR_CLKDIV                 (0xF << 20)
#define FSMC_BWTR_DATAST                 (0xFF << 8)
#define FSMC_BWTR_ADDHLD                 (0xF << 4)
#define FSMC_BWTR_ADDSET                 0xF

// extern LCD_CONTROLLER_TypeDef *TFT_FSMC::LCD;


typedef struct {
  volatile uint16_t REG;
  volatile uint16_t RAM;
} TFTLCD_TypeDef;

extern TFTLCD_TypeDef *LCD;  // LCD FSMC Control Address

void LCD_FSMCInit(uint8_t cs, uint8_t rs);
void LCD_WriteData(uint16_t RegValue);
void LCD_WriteReg(uint16_t Reg);
uint32_t LCD_ReadData(uint16_t RegValue, uint8_t ReadSize);

//unused.
void LCD_WriteMultiple(uint16_t data, uint32_t count);
void LCD_WriteSequence(uint16_t *Data, uint16_t count);
// void LCD_WriteSequence(uint16_t *data, uint16_t length);
// void LCD_WriteSequence_Async(uint16_t *data, uint16_t length);
// void LCD_WaitSequence_Async();

