#include "../TSC_Menu.h"
// #include <EEPROM.h>
#include "stm32f4xx_hal.h"
// #include <stm32_eeprom.h>
// #include "../../../../src/HAL/STM32_F4_F7/eeprom_emul.h"


// #define EEPROM_START_ADDRESS (0x8000000UL + (STM32_FLASH_SIZE) * 1024UL - (EEPROM_PAGE_SIZE) * 2UL)
//#define SIGN_ADDRESS (0x08080000 - 0x800 - 0x800)
#define SIGN_ADDRESS (0x080E0000) //last sector (128KB) to save user parameters

void STM32_FlashRead(uint8_t *data, uint32_t len)
{
  uint32_t i = 0;
  for(i = 0; i < len; i++)
  {
    data[i] = *(volatile uint8_t*)(SIGN_ADDRESS + i);
  }
}

void STM32_FlashWrite(uint8_t *data, uint32_t len)
{
#if 0
  uint32_t i = 0;
  FLASH_Unlock();
  FLASH_ErasePage(SIGN_ADDRESS);
  for(i = 0; i < len; i += 2)
  {
    uint16_t data16 = data[i] | (data[min(i+1, len-1)] << 8);  //stm32f10x needs to write at least 16 bits at a time
    FLASH_ProgramHalfWord(SIGN_ADDRESS + i, data16);
  }
  FLASH_Lock();

#else
  HAL_FLASH_Unlock();
  // HAL_FLASHEx_erase
  // HAL_FLASHEx_Erase( SIGN_ADDRESS);

  HAL_FLASH_Lock();
#endif
}
