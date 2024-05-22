#include "Hal/STM32_Flash.h"
#include "Menu/Settings.h"
#include "Hal/w25qxx.h"

#define SAVE_DEVICE_EXT_FLASH 1

#define PARA_SIZE 256  //bytes
#define TSC_SIGN  0x20200707 // DO NOT MODIFY
// TODO: Decipher and consider modifying:
#define PARA_SIGN 0x20200708 // If a new setting parameter is added, modify here and initialize the initial value in the "infoSettingsReset()" function

extern uint32_t TSC_Para[7];        //
extern SETTINGS infoSettings;  //


// W25Q64_SETTING_ADDR  
// W25Q64_TSC_PARA_ADDR
void w25qxx_write_saved(uint8_t *data, uint16_t size)
{
  uint32_t save_addr = W25Q64_SETTING_ADDR;
  W25Qxx_EraseSector(save_addr);
  W25Qxx_WriteBuffer(data, save_addr, size);
}

void w25qxx_read_saved(uint8_t *data, uint16_t size)
{
  uint32_t save_addr = W25Q64_SETTING_ADDR;
  W25Qxx_ReadBuffer(data, save_addr, size);
}

void wordToByte(uint32_t word, uint8_t *bytes)  //
{
  uint8_t len = 4;
  uint8_t i = 0;
  for(i = 0; i < len; i++)
  {
    bytes[i] = (word >> 24) & 0xFF;
    word <<= 8;
  }
}

uint32_t byteToWord(uint8_t *bytes, uint8_t len)
{
  uint32_t word = 0;
  uint8_t i = 0;
  for(i = 0; i < len; i++)
  {
    word <<= 8;
    word |= bytes[i];
  }
  return word;
}

// Read settings parameter if exist, or reset settings parameter
// return value: whether the touch screen calibration parameter exists
bool readStoredPara(void)
{
  bool paraExist = true;
  uint8_t data[PARA_SIZE];
  uint32_t index = 0;
  uint32_t sign = 0;
#if SAVE_DEVICE_EXT_FLASH
  w25qxx_read_saved(data, PARA_SIZE);
#else
  STM32_FlashRead(data, PARA_SIZE);
#endif
  sign = byteToWord(data + (index += 4), 4);
  if(sign != TSC_SIGN) paraExist = false;    // If the touch screen calibration parameter does not exist
  else
  {
    for(uint32_t i=0; i<sizeof(TSC_Para)/sizeof(TSC_Para[0]); i++)
    {
      TSC_Para[i] = byteToWord(data + (index += 4), 4);
    }
  }
    
  sign = byteToWord(data + (index += 4), 4);
  if(sign != PARA_SIGN) // If the settings parameter is illegal, reset settings parameter
  {
    infoSettingsReset();
  }
  else
  {
    infoSettings.baudrate           = byteToWord(data + (index += 4), 4);
    infoSettings.language           = byteToWord(data + (index += 4), 4);
    infoSettings.mode               = byteToWord(data + (index += 4), 4);
    infoSettings.runout             = byteToWord(data + (index += 4), 4);
    infoSettings.rotate_ui          = byteToWord(data + (index += 4), 4);
    infoSettings.bg_color           = byteToWord(data + (index += 4), 4);
    infoSettings.font_color         = byteToWord(data + (index += 4), 4);
    infoSettings.silent             = byteToWord(data + (index += 4), 4);
    infoSettings.auto_off           = byteToWord(data + (index += 4), 4);
    infoSettings.tsccalibnextboot   = byteToWord(data + (index += 4), 4);

  }
  
  return paraExist;
}

void storePara(void)
{
  uint8_t  data[PARA_SIZE]; 
  uint32_t index = 0;
  
  wordToByte(TSC_SIGN, data + (index += 4));
  for(uint32_t i=0; i<sizeof(TSC_Para)/sizeof(TSC_Para[0]); i++)
  {
    wordToByte(TSC_Para[i], data + (index += 4));
  }
  wordToByte(PARA_SIGN, data + (index += 4));
  wordToByte(infoSettings.baudrate,           data + (index += 4));
  wordToByte(infoSettings.language,           data + (index += 4));
  wordToByte(infoSettings.mode,               data + (index += 4));
  wordToByte(infoSettings.runout,             data + (index += 4));
  wordToByte(infoSettings.rotate_ui,          data + (index += 4));
  wordToByte(infoSettings.bg_color,           data + (index += 4));
  wordToByte(infoSettings.font_color,         data + (index += 4));
  wordToByte(infoSettings.silent,             data + (index += 4));
  wordToByte(infoSettings.auto_off,           data + (index += 4));
  wordToByte(infoSettings.tsccalibnextboot,   data + (index += 4));
  
#if SAVE_DEVICE_EXT_FLASH
  w25qxx_write_saved(data, PARA_SIZE);
#else
  STM32_FlashWrite(data, PARA_SIZE);
#endif
}


