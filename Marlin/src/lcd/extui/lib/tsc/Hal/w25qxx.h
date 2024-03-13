#ifndef _W25QXX_H_
#define _W25QXX_H_

#include "spi.h"

#define CMD_WRITE_ENABLE   0x06
#define CMD_WRITE_DISABLE  0x04
#define CMD_READ_REGISTER1 0x05
#define CMD_READ_REGISTER2 0x35
#define CMD_PAGE_PROGRAM   0x02
#define CMD_SECTOR_ERASE   0x20
#define CMD_BLOCK_ERASE    0xD8
#define CMD_FLASH__BE      0xC7

#define CMD_READ_ID        0x9F
#define CMD_READ_DATA      0x03
#define CMD_FAST_READ_DATA 0x0B
#define W25QXX_DUMMY_BYTE  0xFF
#define W25QXX_SPI_PAGESIZE 0x100

#define W25QXX_PAGE_SIZE    (256)
#define W25QXX_SECTOR_SIZE  (4*1024)  //min erase unit
#define W25QXX_BLOCK_SIZE   (64*1024)

#define W25Q64_MARLIN_SETTING_ADDR  /*8MB*/ (8*1024*1024 - (3*W25QXX_SECTOR_SIZE) )
#define W25Q64_SETTING_ADDR  /*8MB*/ (8*1024*1024 - (2*W25QXX_SECTOR_SIZE) )
#define W25Q64_TSC_PARA_ADDR  /*8MB*/ (8*1024*1024 - W25QXX_SECTOR_SIZE)

extern SPIClass W25qxxSPI;

uint8_t W25Qxx_SPI_Read_Write_Byte(uint8_t data);
void W25Qxx_SPI_CS_Set(uint8_t level);
void W25Qxx_Init(void);

void W25Qxx_WriteEnable(void);
void W25Qxx_WaitForWriteEnd(void);
void W25Qxx_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25Qxx_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25Qxx_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void W25Qxx_EraseSector(uint32_t SectorAddr);
void W25Qxx_EraseBlock(uint32_t BlockAddr);
void W25Qxx_EraseBulk(void);
uint32_t W25Qxx_ReadID(void);

void w25qxx_spi_dma_init();
void w25qxx_spi_set_data_width_8_16(uint8_t data_size);
void w25qxx_spi_transferDMA(void *txbuf, void *rxbuf, uint16_t cnt);

void w25qxx_read_saved(uint32_t addr, uint8_t *data, uint16_t size);

#endif 


