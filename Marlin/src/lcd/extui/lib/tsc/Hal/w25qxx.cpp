#include "../../../../../inc/MarlinConfig.h"
#include "w25qxx.h"
#include "rtt.h"
#include "../../../../tft_io/tft_io.h"

/*************************** W25Qxx SPI ģʽ�ײ���ֲ�Ľӿ� ***************************/
//#define W25QXX_SPIX     _SPI3
//#define W25Qxx_SPEED   0


// mosi, uint8_t miso, uint8_t sclk, uint8_t ssel SPI2 in F4
#if ENABLED(NEW_BOARD)
  #define W25QXX_SPIX SPI3
  #define W25QXX_MOSI_PIN PB5
  #define W25QXX_MISO_PIN PB4
  #define W25QXX_SCLK_PIN PB3
  #define W25QXX_CS_PIN   PA15
#else
  #define W25QXX_SPIX SPI2
  #define W25QXX_MOSI_PIN PC3
  #define W25QXX_MISO_PIN PC2
  #define W25QXX_SCLK_PIN PB13
  #define W25QXX_CS_PIN   PB12
#endif

SPIClass W25qxxSPI(W25QXX_MOSI_PIN, W25QXX_MISO_PIN, W25QXX_SCLK_PIN, W25QXX_CS_PIN);  //when assign _spi instance?
SPI_HandleTypeDef SPI3_Handler;                     //SPI句柄

void W25Qxx_SPI_CS_Set(uint8_t level)
{
  WRITE(W25QXX_CS_PIN, level);
}

uint8_t W25Qxx_SPI_Read_Write_Byte(uint8_t data)
{
  #if ENABLED(NEW_BOARD)
    uint8_t Rxdata;
    HAL_SPI_TransmitReceive(&SPI3_Handler,&data,&Rxdata,1, 1000);       
 	  return Rxdata;
  #else
    return W25qxxSPI.transfer(data);
  #endif
}

void W25Qxx_SPI_Read_Buf(uint8_t* buf, uint16_t nbyte) 
{
  #if ENABLED(NEW_BOARD)
    for(int i=0; i<nbyte; i++){
      buf[i] = W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
    }
  #else
    // W25qxxSPI.dmaTransfer(0, const_cast<uint8_t*>(buf), nbyte);
    uint8_t *dummy = (uint8_t*)0x08000000;
    W25qxxSPI.transfer(dummy, buf, nbyte);
  #endif
}

void W25Qxx_SPI_Write_Buf(uint8_t* pBuffer, uint32_t len)
{
  #if ENABLED(NEW_BOARD)
    for(int i=0; i<len; i++){
      W25Qxx_SPI_Read_Write_Byte(pBuffer[i]);
    }
  #else
    W25qxxSPI.transfer(pBuffer, len);
  #endif
}
#if ENABLED(NEW_BOARD)
  void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
  {
      GPIO_InitTypeDef GPIO_Initure;
      
      __HAL_RCC_GPIOB_CLK_ENABLE();       //Ê¹ÄÜGPIOBÊ±ÖÓ
      __HAL_RCC_SPI3_CLK_ENABLE();        //Ê¹ÄÜSPI1Ê±ÖÓ
      
      //PB3,4,5
      GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
      GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //¸´ÓÃÍÆÍìÊä³ö
      GPIO_Initure.Pull=GPIO_PULLUP;                  //ÉÏÀ­
      GPIO_Initure.Speed=GPIO_SPEED_HIGH;             //¿ìËÙ            
      GPIO_Initure.Alternate=GPIO_AF6_SPI3;           //¸´ÓÃÎªSPI1
      HAL_GPIO_Init(GPIOB,&GPIO_Initure);
  }
  void SPI3_Init(void){
    SPI3_Handler.Instance = W25QXX_SPIX;                //SPI3
    SPI3_Handler.Init.Mode=SPI_MODE_MASTER;             //主模式
    SPI3_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //双线模式
    SPI3_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //8bit 帧
    SPI3_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;    //空闲状态为高电平
    SPI3_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI3_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS 信号由硬件（NSS管脚）还是软件 
    SPI3_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//定义波特率预分频的值:波特率预分频值为256
    SPI3_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //指定数据传输从MSB位还是LSB 位开始:数据传输从 MSB 位开始
    SPI3_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //关闭TI模式
    SPI3_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//关闭CRC校验
    SPI3_Handler.Init.CRCPolynomial=7;                  //CRC值计算的多项式

    __HAL_RCC_SPI3_CLK_ENABLE();
    pinmap_pinout(digitalPinToPinName(W25QXX_MOSI_PIN), PinMap_SPI_MOSI);
    pinmap_pinout(digitalPinToPinName(W25QXX_MISO_PIN), PinMap_SPI_MISO);
    pinmap_pinout(digitalPinToPinName(W25QXX_SCLK_PIN), PinMap_SPI_SCLK);

    HAL_SPI_Init(&SPI3_Handler);//³õÊ¼»¯
    
    __HAL_SPI_ENABLE(&SPI3_Handler);                    //使能SPI1
  }
  void SPI3_SetSpeed(uint8_t SPI_BaudRatePrescaler)
  {
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//ÅÐ¶ÏÓÐÐ§ÐÔ
    __HAL_SPI_DISABLE(&SPI3_Handler);            //¹Ø±ÕSPI
    SPI3_Handler.Instance->CR1&=0XFFC7;          //Î»3-5ÇåÁã£¬ÓÃÀ´ÉèÖÃ²¨ÌØÂÊ
    SPI3_Handler.Instance->CR1|=SPI_BaudRatePrescaler;//ÉèÖÃSPIËÙ¶È
    __HAL_SPI_ENABLE(&SPI3_Handler);             //Ê¹ÄÜSPI
  }
#endif
volatile uint32_t w25qid;
void W25Qxx_Init(void)
{
  #if ENABLED(NEW_BOARD)

    w25qxx_spi_dma_init();
    uint8_t temp;
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOG_CLK_ENABLE();           // 使能时钟
    
    //W25QXX_CS_PIN PA15
    GPIO_Initure.Pin=GPIO_PIN_15;           //11脚
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //堆挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //快速         
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);     //初始化
    // SET_OUTPUT(W25QXX_CS_PIN);
    
    W25Qxx_SPI_CS_Set(1);                   //SPI FLASH不选中
    SPI3_Init();		   			                //初始化SPI
    SPI3_SetSpeed(SPI_BAUDRATEPRESCALER_2); //ÉèÖÃÎª21MÊ±ÖÓ,¸ßËÙÄ£Ê½
    // W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
    w25qid = W25Qxx_ReadID();
    SERIAL_ECHOPAIR("w25qid=", w25qid);

  #else

    w25qxx_spi_dma_init();
    W25qxxSPI.begin();
    W25qxxSPI.setClockDivider(2);
    SET_OUTPUT(W25QXX_CS_PIN);
    W25Qxx_SPI_CS_Set(1);
    w25qid = W25Qxx_ReadID();
    rtt.printf("w25qxx id = %6x" , w25qid);

  #endif
}
/*************************************************************************************/


// дʹ��
void W25Qxx_WriteEnable(void)
{
  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_WRITE_ENABLE);
  W25Qxx_SPI_CS_Set(1);
}
//�ȴ�W25Qxx����
void W25Qxx_WaitForWriteEnd(void)
{
  uint8_t flashstatus = 0;

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_REGISTER1);
  do
  {
    flashstatus = W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
  }
  while (flashstatus & 0x01); 
  W25Qxx_SPI_CS_Set(1);
}

//��ҳд
void W25Qxx_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  W25Qxx_WriteEnable();
  W25Qxx_SPI_CS_Set(0);
	
  W25Qxx_SPI_Read_Write_Byte(CMD_PAGE_PROGRAM);
  W25Qxx_SPI_Read_Write_Byte((WriteAddr & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((WriteAddr & 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(WriteAddr & 0xFF);
	
  W25Qxx_SPI_Write_Buf(pBuffer, NumByteToWrite);
  
  W25Qxx_SPI_CS_Set(1);
  W25Qxx_WaitForWriteEnd();
}

//��FLASH�з���buffer������

/*
 * @param pBuffer  write buffer
 * @param WriteAddr  the start address
 * @param NumByteToWrite  the size of buffer
 */
void W25Qxx_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % W25QXX_SPI_PAGESIZE;//������һҳ���ĸ���ַ��ʼд����
  count = W25QXX_SPI_PAGESIZE - Addr;//������ҳ������д��������
  NumOfPage =  NumByteToWrite / W25QXX_SPI_PAGESIZE;//����Ҫ��������ݳ��ȿ���д����ҳ
  NumOfSingle = NumByteToWrite % W25QXX_SPI_PAGESIZE;//����������ҳ�����⣬������д�����ݳ���

  if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      W25Qxx_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      while(NumOfPage--)
      {
        W25Qxx_WritePage(pBuffer, WriteAddr, W25QXX_SPI_PAGESIZE);
        WriteAddr +=  W25QXX_SPI_PAGESIZE;
        pBuffer += W25QXX_SPI_PAGESIZE;
      }
      W25Qxx_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;

        W25Qxx_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        W25Qxx_WritePage(pBuffer, WriteAddr, temp);
      }
      else
      {
        W25Qxx_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / W25QXX_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % W25QXX_SPI_PAGESIZE;

      W25Qxx_WritePage(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        W25Qxx_WritePage(pBuffer, WriteAddr, W25QXX_SPI_PAGESIZE);
        WriteAddr +=  W25QXX_SPI_PAGESIZE;
        pBuffer += W25QXX_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        W25Qxx_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/*��FLASH�ж�����*/
void W25Qxx_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  W25Qxx_SPI_CS_Set(0);

  W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);

  W25Qxx_SPI_Read_Write_Byte((ReadAddr & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((ReadAddr& 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(ReadAddr & 0xFF);

  W25Qxx_SPI_Read_Buf(pBuffer, NumByteToRead);
	
  W25Qxx_SPI_CS_Set(1);
}

//��ID
uint32_t W25Qxx_ReadID(void)
{
  uint32_t Temp = 0;

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_ID);

  Temp = W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
  Temp = (Temp<<8) | W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
  Temp = (Temp<<8) | W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);

  W25Qxx_SPI_CS_Set(1);

  return Temp;
}

//檫除扇区
void W25Qxx_EraseSector(uint32_t SectorAddr)
{
  W25Qxx_WriteEnable();

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_SECTOR_ERASE);
  W25Qxx_SPI_Read_Write_Byte((SectorAddr & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((SectorAddr & 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(SectorAddr & 0xFF);
  W25Qxx_SPI_CS_Set(1);

  W25Qxx_WaitForWriteEnd();
}

//檫除块
void W25Qxx_EraseBlock(uint32_t BlockAddr)
{
  W25Qxx_WriteEnable();
	
  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_BLOCK_ERASE);
  W25Qxx_SPI_Read_Write_Byte((BlockAddr & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((BlockAddr & 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(BlockAddr & 0xFF);
  W25Qxx_SPI_CS_Set(1);

  W25Qxx_WaitForWriteEnd();
}

//ȫƬ����
void W25Qxx_EraseBulk(void)
{
  W25Qxx_WriteEnable();

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_FLASH__BE);
  W25Qxx_SPI_CS_Set(1);

  W25Qxx_WaitForWriteEnd();
}



SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_tx;
DMA_HandleTypeDef hdma_spi2_rx;

void w25qxx_spi_set_data_width_8_16(uint8_t data_size)
{
  W25QXX_SPIX->CR1 &= ~SPI_CR1_SPE;
  if (data_size == 16) {
    W25QXX_SPIX->CR1 |= SPI_CR1_DFF;
  }
  else {
    W25QXX_SPIX->CR1 &= ~SPI_CR1_DFF;
  }
  W25QXX_SPIX->CR1 |= SPI_CR1_SPE;
}

void w25qxx_spi_dma_init()
{
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* SPI2_TX Init */
  #if ENABLED(NEW_BOARD)
  
    hdma_spi2_tx.Instance = DMA1_Stream5;
    hdma_spi2_tx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi2_tx.Init.MemInc = DMA_MINC_DISABLE;
    hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_spi2_tx.Init.Mode = DMA_NORMAL;
    hdma_spi2_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_spi2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_spi2_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_spi2_tx.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_spi2_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
    if (HAL_DMA_Init(&hdma_spi2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&SPI3_Handler, hdmatx, hdma_spi2_tx);

    /* SPI2_RX Init */
    hdma_spi2_rx.Instance = DMA1_Stream2;
    hdma_spi2_rx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi2_rx.Init.MemInc = DMA_MINC_DISABLE;
    hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_spi2_rx.Init.Mode = DMA_NORMAL;
    hdma_spi2_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_spi2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_spi2_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_spi2_rx.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_spi2_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
    
    if (HAL_DMA_Init(&hdma_spi2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&SPI3_Handler, hdmarx, hdma_spi2_rx);

  #else

    hdma_spi2_tx.Instance = DMA1_Stream4;
    hdma_spi2_tx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi2_tx.Init.MemInc = DMA_MINC_DISABLE;
    hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_spi2_tx.Init.Mode = DMA_NORMAL;
    hdma_spi2_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_spi2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_spi2_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_spi2_tx.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_spi2_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
    if (HAL_DMA_Init(&hdma_spi2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&hspi2, hdmatx, hdma_spi2_tx);

    /* SPI2_RX Init */
    hdma_spi2_rx.Instance = DMA1_Stream3;
    hdma_spi2_rx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi2_rx.Init.MemInc = DMA_MINC_DISABLE;
    hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_spi2_rx.Init.Mode = DMA_NORMAL;
    hdma_spi2_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_spi2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_spi2_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_spi2_rx.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_spi2_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
    
    if (HAL_DMA_Init(&hdma_spi2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&hspi2, hdmarx, hdma_spi2_rx);

  #endif
}

void w25qxx_spi_transferDMA(void *txbuf, void *rxbuf, uint16_t cnt)
{
  const uint16_t dummy = 0xffff;
  if (txbuf == NULL) {
    txbuf = (uint8_t*)&dummy;
  }

  /* Enable Rx DMA Request */
  SET_BIT(W25QXX_SPIX->CR2, SPI_CR2_RXDMAEN);
  SET_BIT(W25QXX_SPIX->CR2, SPI_CR2_TXDMAEN);

  __HAL_DMA_CLEAR_FLAG(&hdma_spi2_tx, __HAL_DMA_GET_FE_FLAG_INDEX(&hdma_spi2_tx));
  __HAL_DMA_CLEAR_FLAG(&hdma_spi2_rx, __HAL_DMA_GET_FE_FLAG_INDEX(&hdma_spi2_rx));

  __HAL_DMA_CLEAR_FLAG(&hdma_spi2_tx, __HAL_DMA_GET_TC_FLAG_INDEX(&hdma_spi2_tx));
  __HAL_DMA_CLEAR_FLAG(&hdma_spi2_rx, __HAL_DMA_GET_TC_FLAG_INDEX(&hdma_spi2_rx));

  //cfg dma. before
  // HAL_DMA_Start(&hdma_spi2_rx, (uint32_t)&(W25QXX_SPIX->DR), (uint32_t)rxbuf, cnt); //use rx
  // HAL_DMA_Start(&hdma_spi2_tx,  (uint32_t)txbuf, (uint32_t)&(W25QXX_SPIX->DR), cnt);

  // __HAL_DMA_DISABLE(&hdma_spi2_rx);
  // __HAL_DMA_DISABLE(&hdma_spi2_tx);

  hdma_spi2_rx.Instance->NDTR = cnt;  //periph to mem DR->LCD.RAM
  hdma_spi2_rx.Instance->PAR = (uint32_t)&(W25QXX_SPIX->DR);
  hdma_spi2_rx.Instance->M0AR = (uint32_t)rxbuf;

  hdma_spi2_tx.Instance->NDTR = cnt;  //mem to periph, tx dummy clocks
  hdma_spi2_tx.Instance->PAR = (uint32_t)&(W25QXX_SPIX->DR);
  hdma_spi2_tx.Instance->M0AR = (uint32_t)txbuf;

  __HAL_DMA_ENABLE(&hdma_spi2_rx);
  __HAL_DMA_ENABLE(&hdma_spi2_tx);

  while ( !__HAL_DMA_GET_FLAG(&hdma_spi2_rx, __HAL_DMA_GET_TC_FLAG_INDEX(&hdma_spi2_rx) ));
  while ( !__HAL_DMA_GET_FLAG(&hdma_spi2_tx, __HAL_DMA_GET_TC_FLAG_INDEX(&hdma_spi2_tx) ));

  CLEAR_BIT(W25QXX_SPIX->CR2, SPI_CR2_TXDMAEN);
  CLEAR_BIT(W25QXX_SPIX->CR2, SPI_CR2_RXDMAEN);

  __HAL_DMA_DISABLE(&hdma_spi2_tx);
  __HAL_DMA_DISABLE(&hdma_spi2_rx);
}


