#include "../../../../../inc/MarlinConfig.h"

#if defined(MD_FSMC_LCD) // FSMC on 100/144 pins SoCs

#include "stm32f4_fsmc.h"
#include "stm32f4xx_hal.h"

/* Timing configuration */
#define FSMC_ADDRESS_SETUP_TIME   13  // AddressSetupTime
#define FSMC_DATA_SETUP_TIME      13  // DataSetupTime

/**
 * FSMC LCD IO
 */
#define __ASM __asm
#define __STATIC_INLINE static inline

// __attribute__((always_inline)) __STATIC_INLINE void __DSB() {
//   __ASM volatile ("dsb 0xF":::"memory");
// }

TFTLCD_TypeDef *LCD; // LCD FSMC Control Address

SRAM_HandleTypeDef SRAMx;
DMA_HandleTypeDef DMAtx;
// LCD_CONTROLLER_TypeDef *TFT_FSMC::LCD;

static uint8_t fsmcInit = 0;


SRAM_HandleTypeDef hsram1;

/* FSMC initialization function */
void MX_FSMC_Init(void)
{
  FSMC_NORSRAM_TimingTypeDef Timing = {0};

  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FSMC_NORSRAM_DEVICE;
  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK1;
  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  hsram1.Init.PageSize = FSMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 1;
  Timing.AddressHoldTime = 1;
  Timing.DataSetupTime = 1;
  Timing.BusTurnAroundDuration = 1;
  Timing.CLKDivision = 1;
  Timing.DataLatency = 1;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK)
  {
    Error_Handler( );
  }

}

void LCD_FSMCInit(uint8_t cs, uint8_t rs)
{
    if (fsmcInit)
        return;
    fsmcInit = 1;

    #if 0

        // TFT_IO::lcd_init_rgb();
        TFT_FSMC::Init();
        TFT_IO::InitTFT();
        TFT_FSMC::testRGB();

    #else

        uint32_t controllerAddress;

      #if PIN_EXISTS(TFT_RESET)
        OUT_WRITE(TFT_RESET_PIN, HIGH);
        HAL_Delay(100);
      #endif

      // #if PIN_EXISTS(TFT_BACKLIGHT)
      //   OUT_WRITE(TFT_BACKLIGHT_PIN, HIGH);
      // #endif

        FSMC_NORSRAM_TimingTypeDef Timing, ExtTiming;

        uint32_t NSBank = (uint32_t)pinmap_peripheral(digitalPinToPinName(TFT_CS_PIN), PinMap_FSMC_CS);

        SRAMx.Instance = FSMC_NORSRAM_DEVICE;
        SRAMx.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
        /* SRAMx.Init */
        SRAMx.Init.NSBank = NSBank;
        SRAMx.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
        SRAMx.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
        SRAMx.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
        SRAMx.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
        SRAMx.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
        SRAMx.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
        SRAMx.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
        SRAMx.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
        SRAMx.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
        SRAMx.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
        SRAMx.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
        SRAMx.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
      #ifdef STM32F4xx
        SRAMx.Init.PageSize = FSMC_PAGE_SIZE_NONE;
      #endif
        /* Read Timing - relatively slow to ensure ID information is correctly read from TFT controller */
        /* Can be decreases from 15-15-24 to 4-4-8 with risk of stability loss */
        Timing.AddressSetupTime = 15;
        Timing.AddressHoldTime = 0;
        Timing.DataSetupTime = 12;
        Timing.BusTurnAroundDuration = 0;
        Timing.CLKDivision = 4;
        Timing.DataLatency = 17;
        Timing.AccessMode = FSMC_ACCESS_MODE_A;
        /* Write Timing */
        /* Can be decreases from 8-15-8 to 0-0-1 with risk of stability loss */
        ExtTiming.AddressSetupTime = 8;
        ExtTiming.AddressHoldTime = 0;
        ExtTiming.DataSetupTime = 8;
        ExtTiming.BusTurnAroundDuration = 0;
        ExtTiming.CLKDivision = 16;
        ExtTiming.DataLatency = 17;
        ExtTiming.AccessMode = FSMC_ACCESS_MODE_A;

        __HAL_RCC_FSMC_CLK_ENABLE();

        for (uint16_t i = 0; PinMap_FSMC[i].pin != NC; i++)
            pinmap_pinout(PinMap_FSMC[i].pin, PinMap_FSMC);
        pinmap_pinout(digitalPinToPinName(TFT_CS_PIN), PinMap_FSMC_CS);
        pinmap_pinout(digitalPinToPinName(TFT_RS_PIN), PinMap_FSMC_RS);

        controllerAddress = FSMC_BANK1_1;
      #ifdef PF0
        switch (NSBank)
        {
            case FSMC_NORSRAM_BANK2:
                controllerAddress = FSMC_BANK1_2;
                break;
            case FSMC_NORSRAM_BANK3:
                controllerAddress = FSMC_BANK1_3;
                break;
            case FSMC_NORSRAM_BANK4:
                controllerAddress = FSMC_BANK1_4;
                break;
        }
      #endif

        controllerAddress |= (uint32_t)pinmap_peripheral(digitalPinToPinName(TFT_RS_PIN), PinMap_FSMC_RS);

        //
        #if ENABLED(ST32_SHIP)
          HAL_SRAM_Init(&SRAMx, &Timing, &ExtTiming);
        #elif ENABLED(USE_GD32)
          HAL_SRAM_Init(&SRAMx, &Timing, NULL);
          // MX_FSMC_Init();
        #endif
        // SRAMx.Instance->BTCR[0] = FSMC_BCR1_WREN | FSMC_BCR_MTYP_SRAM  | FSMC_BCR_MWID_16BITS | FSMC_BCR_MBKEN;   //BCR1
        // SRAMx.Instance->BTCR[1] = (FSMC_DATA_SETUP_TIME << 8) | FSMC_ADDRESS_SETUP_TIME;  //BTR1

        __HAL_RCC_DMA2_CLK_ENABLE();

      #ifdef STM32F1xx
        DMAtx.Instance = DMA2_Channel1;
      #elif defined(STM32F4xx)
        DMAtx.Instance = DMA2_Stream0;
        DMAtx.Init.Channel = DMA_CHANNEL_0;
        DMAtx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        DMAtx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
        DMAtx.Init.MemBurst = DMA_MBURST_SINGLE;
        DMAtx.Init.PeriphBurst = DMA_PBURST_SINGLE;
      #endif
        DMAtx.Init.Direction = DMA_MEMORY_TO_MEMORY;
        DMAtx.Init.MemInc = DMA_MINC_DISABLE;
        DMAtx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        DMAtx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        DMAtx.Init.Mode = DMA_NORMAL;
        DMAtx.Init.Priority = DMA_PRIORITY_HIGH;

        LCD = (TFTLCD_TypeDef *)controllerAddress;

    #endif
}
#if 0
void LCD_WriteData(uint16_t data) {
  // TFT_FSMC::LCD->RAM = data;
  TFT_FSMC::WriteData(data);
  __DSB();
}

void LCD_WriteReg(uint16_t Reg) {
  // TFT_FSMC::LCD->REG = Reg;
  TFT_FSMC::WriteReg(Reg);
  __DSB();
}

uint16_t LCD_ReadData(void)
{
  // volatile uint16_t ram;
  // ram = TFT_FSMC::LCD->RAM;	
  // return ram;
}

void LCD_WriteMultiple(uint16_t color, uint32_t cnt)
{
  TFT_FSMC::WriteMultiple(color, cnt);
}
#else
void LCD_WriteData(uint16_t data)
{
    LCD->RAM = data;
    // TFT_FSMC::WriteData(data);
    // __DSB();
}

void LCD_WriteReg(uint16_t Reg)
{
    LCD->REG = Reg;
    // TFT_FSMC::WriteReg(Reg);
    // __DSB();
}

uint16_t LCD_ReadData(void)
{
    volatile uint16_t ram;
    ram = LCD->RAM;
    return ram;
}

void LCD_WriteMultiple(uint16_t color, uint32_t count)
{
    static uint16_t Data;
    uint16_t tran_cnt;
    Data = color; //in case color changed outside.
    // TFT_FSMC::WriteMultiple(color, cnt);
    // while (count > 0) {
    //   dma_setup_transfer(FSMC_DMA_DEV, FSMC_DMA_CHANNEL, &color, DMA_SIZE_16BITS, &TFTLCD->RAM, DMA_SIZE_16BITS, DMA_MEM_2_MEM);
    //   dma_set_num_transfers(FSMC_DMA_DEV, FSMC_DMA_CHANNEL, count > 65535 ? 65535 : count);
    //   dma_clear_isr_bits(FSMC_DMA_DEV, FSMC_DMA_CHANNEL);
    //   dma_enable(FSMC_DMA_DEV, FSMC_DMA_CHANNEL);

    //   while ((dma_get_isr_bits(FSMC_DMA_DEV, FSMC_DMA_CHANNEL) & 0x0A) == 0) {};
    //   dma_disable(FSMC_DMA_DEV, FSMC_DMA_CHANNEL);

    //   count = count > 65535 ? count - 65535 : 0;
    // }
#if 1
    while (count > 0)
    {
        tran_cnt = count > 0xffff ? 0xffff : count;
        count -= tran_cnt;

        DMAtx.Init.PeriphInc = DMA_PINC_DISABLE;
        DMAtx.Init.MemInc = DMA_MINC_DISABLE;   //lcd auto inc write pointer
        HAL_DMA_Init(&DMAtx);

        __HAL_DMA_CLEAR_FLAG(&DMAtx, __HAL_DMA_GET_TC_FLAG_INDEX(&DMAtx));
        __HAL_DMA_CLEAR_FLAG(&DMAtx, __HAL_DMA_GET_TE_FLAG_INDEX(&DMAtx));

    #ifdef STM32F1xx
            DMAtx.Instance->CNDTR = tran_cnt;
            DMAtx.Instance->CPAR = (uint32_t)Data;
            DMAtx.Instance->CMAR = (uint32_t) & (LCD->RAM);
    #elif defined(STM32F4xx)
            DMAtx.Instance->NDTR = tran_cnt;
            DMAtx.Instance->PAR = (uint32_t) & Data;
            DMAtx.Instance->M0AR = (uint32_t) & (LCD->RAM);
    #endif
        __HAL_DMA_ENABLE(&DMAtx);

        // auto dma_tcif = __HAL_DMA_GET_TC_FLAG_INDEX(&DMAtx);
        while( !__HAL_DMA_GET_FLAG(&DMAtx, __HAL_DMA_GET_TC_FLAG_INDEX(&DMAtx)) );
    }

#endif
    // for (int h=0; h<320; ++h) {
    //     for (int w=0; w<480; ++w) {
    //         LCD->RAM = color;
    //     }
    //     Data++;
    //     Data--;
    // }

    // for (int p=0; p<count; ++p) {
    //     LCD->RAM = color;
    //     Data++;
    //     Data--;
    // }
    // Data++;

}

void LCD_WriteSequence(uint16_t *Data, uint16_t count)
{
    uint16_t tran_cnt;
    while (count > 0)
    {
        tran_cnt = count > 0xffff ? 0xffff : count;
        count -= tran_cnt;

        DMAtx.Init.PeriphInc = DMA_PINC_ENABLE;
        DMAtx.Init.MemInc = DMA_MINC_DISABLE;   //lcd auto inc write pointer
        HAL_DMA_Init(&DMAtx);

        __HAL_DMA_CLEAR_FLAG(&DMAtx, __HAL_DMA_GET_TC_FLAG_INDEX(&DMAtx));
        __HAL_DMA_CLEAR_FLAG(&DMAtx, __HAL_DMA_GET_TE_FLAG_INDEX(&DMAtx));

    #ifdef STM32F1xx
            DMAtx.Instance->CNDTR = tran_cnt;
            DMAtx.Instance->CPAR = (uint32_t)Data;
            DMAtx.Instance->CMAR = (uint32_t) & (LCD->RAM);
    #elif defined(STM32F4xx)
            DMAtx.Instance->NDTR = tran_cnt;
            DMAtx.Instance->PAR = (uint32_t) & Data;
            DMAtx.Instance->M0AR = (uint32_t) & (LCD->RAM);
    #endif
        __HAL_DMA_ENABLE(&DMAtx);

        // auto dma_tcif = __HAL_DMA_GET_TC_FLAG_INDEX(&DMAtx);
        while( !__HAL_DMA_GET_FLAG(&DMAtx, __HAL_DMA_GET_TC_FLAG_INDEX(&DMAtx)) );
    }
}

#endif

#endif // MD_FSMC_LCD
