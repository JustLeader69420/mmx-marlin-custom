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
 * Platform-independent Arduino functions for SPI EEPROM.
 * Enable USE_SHARED_EEPROM if not supplied by the framework.
 */

#include "../../inc/MarlinConfig.h"

#if ENABLED(W25QXX_SPI_EEPROM)

#include "eeprom_if.h"
#include "rtt.h"

#define EEPROM_RW_DEBUG 0

#if 1 //ENABLED(USE_SHARED_EEPROM)

#define MARLIN_EEPROM_SIZE  (W25QXX_SECTOR_SIZE)  //4k
static uint8_t ram_eeprom[MARLIN_EEPROM_SIZE] __attribute__((aligned(4))) = {0};
static bool eeprom_data_written = false;
static uint8_t ram_write[MARLIN_EEPROM_SIZE] __attribute__((aligned(4)));
static uint8_t ram_read[MARLIN_EEPROM_SIZE] __attribute__((aligned(4)));
uint32_t sector_addr = W25Q64_MARLIN_SETTING_ADDR;
void eeprom_init() {

#if EEPROM_RW_DEBUG
  for (int i=0; i<MARLIN_EEPROM_SIZE; ++i) {
    ram_eeprom[i] = i & 0xff;
  }
  memcpy(ram_write, ram_eeprom, sizeof(ram_write)); //because spi will recover buffer???

  W25Qxx_EraseSector(sector_addr);
  W25Qxx_WriteBuffer(ram_write, W25Q64_MARLIN_SETTING_ADDR, sizeof(ram_eeprom));

  W25Qxx_ReadBuffer(ram_read, W25Q64_MARLIN_SETTING_ADDR, sizeof(ram_read));
  if ( memcmp(ram_read, ram_eeprom, sizeof(ram_eeprom)) ) {
    rtt.printf("fuck. eeprom read back not match.");
  }
  else {
    rtt.printf("nice. eeprom read back match. test success");
  }
  
#endif
  W25Qxx_ReadBuffer(ram_eeprom, W25Q64_MARLIN_SETTING_ADDR, sizeof(ram_eeprom));
}

void eeprom_finish() {
  if ( eeprom_data_written ) {
    W25Qxx_EraseSector(W25Q64_MARLIN_SETTING_ADDR);
    W25Qxx_WriteBuffer(ram_eeprom, W25Q64_MARLIN_SETTING_ADDR, sizeof(ram_eeprom));
    eeprom_data_written = false;
  }
}
uint8_t eeprom_read_byte(uint8_t* pos) {
  return ram_eeprom[(int)pos];
}

void eeprom_write_byte(uint8_t* pos, uint8_t value) {
  if (ram_eeprom[(int)pos] != value) {
    ram_eeprom[(int)pos] = value;
    eeprom_data_written = true;
  }
}

#endif // USE_SHARED_EEPROM
#endif // 
