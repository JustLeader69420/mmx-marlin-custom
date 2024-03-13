#include "../../../../../inc/MarlinConfig.h"
#include "xpt2046.h"

#if ENABLED(NEW_BOARD)
  #define TOUCH_CS_PIN     PG0
  #define TOUCH_SCK_PIN    PA5
  #define TOUCH_MOSI_PIN   PA7
  #define TOUCH_MISO_PIN   PA6
  #define TOUCH_INT_PIN    PF15 // PenIRQ coming from XPT2046
  #define ERR_RANGE 50
  #define READ_TIMES 5
  #define LOST_VAL 1
#else
  #define TOUCH_CS_PIN     PB6
  #define TOUCH_SCK_PIN    PB3
  #define TOUCH_MOSI_PIN   PB5
  #define TOUCH_MISO_PIN   PB4
  #define TOUCH_INT_PIN    PB7 // PenIRQ coming from XPT2046
  #define ERR_RANGE 50
  #define READ_TIMES 5
  #define LOST_VAL 1
#endif

/***************************************** XPT2046 SPI ģʽ�ײ���ֲ�Ľӿ�********************************************/
//XPT2046 SPI��� - ʹ��ģ��SPI
_SW_SPI xpt2046;

//Ƭѡ
static void Set_CS(uint8_t level)
{
  SW_SPI_CS_Set(&xpt2046, level);
}

//��д����
static uint8_t Read_Write_Byte(uint8_t TxData)
{		
  return SW_SPI_Read_Write(&xpt2046, TxData);			    
}

#if 0
uint8_t TP_Init(void)
{
  #if ENABLED(NEW_BOARD)

    GPIO_InitTypeDef GPIO_Initure;
    
    {
      __HAL_RCC_GPIOA_CLK_ENABLE();
      __HAL_RCC_GPIOF_CLK_ENABLE();
      __HAL_RCC_GPIOG_CLK_ENABLE();
      
      GPIO_Initure.Pin=GPIO_PIN_6;
      GPIO_Initure.Mode=GPIO_MODE_INPUT;
      GPIO_Initure.Pull=GPIO_PULLUP;
      GPIO_Initure.Speed=GPIO_SPEED_HIGH;
      HAL_GPIO_Init(GPIOA,&GPIO_Initure);     // PA6
      
      GPIO_Initure.Pin=GPIO_PIN_15;
      HAL_GPIO_Init(GPIOF,&GPIO_Initure);     // PF15
          
      GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
      GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_7;
      HAL_GPIO_Init(GPIOA,&GPIO_Initure);     // PA5,7
      
      GPIO_Initure.Pin=GPIO_PIN_0;
      HAL_GPIO_Init(GPIOG,&GPIO_Initure);     // PG0
    }
  
  #else

    GPIO_InitTypeDef GPIO_Initure;
      
    {
      __HAL_RCC_GPIOB_CLK_ENABLE();
      
      GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_4;
      GPIO_Initure.Mode=GPIO_MODE_INPUT;
      GPIO_Initure.Pull=GPIO_PULLUP;
      GPIO_Initure.Speed=GPIO_SPEED_HIGH;
      HAL_GPIO_Init(GPIOB,&GPIO_Initure);     // PA6
          
      GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
      GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6;
      HAL_GPIO_Init(GPIOB,&GPIO_Initure);     // PG0
    }

  #endif
	return 1; 									 
}
#endif

//XPT2046 SPI�ͱ��жϳ�ʼ��
void XPT2046_Init(void)
{
  // TP_Init();
  //PA15-TPEN
  SET_INPUT(TOUCH_INT_PIN);

  SW_SPI_Config(&xpt2046, _SPI_MODE3, 8, // 8bit
  TOUCH_CS_PIN,     //CS
  TOUCH_SCK_PIN,    //SCK
  TOUCH_MISO_PIN,   //MISO
  TOUCH_MOSI_PIN    //MOSI
  );
  Set_CS(1);
}


uint8_t XPT2046_Read_Pen(void)
{
  return READ(TOUCH_INT_PIN);
}

uint16_t XPT2046_Read_AD(uint8_t CMD)
{
  uint16_t ADNum;
  Set_CS(0);

  Read_Write_Byte(CMD);
  ADNum=Read_Write_Byte(0xff);
  ADNum= ((ADNum)<<8) | Read_Write_Byte(0xff);
  ADNum >>= 4;

  Set_CS(1);
  return ADNum;
}

uint16_t XPT2046_Average_AD(uint8_t CMD)
{
  uint16_t i, j;
  uint16_t read_buf[READ_TIMES];
  uint16_t sum = 0,temp = 0;

  for(i=0; i<READ_TIMES; i++)
  {
    read_buf[i] = XPT2046_Read_AD(CMD);
  } 		    
  for(i=0; i<READ_TIMES-1; i++)
  {
    for(j=i+1; j<READ_TIMES; j++)
    {
      if(read_buf[i] > read_buf[j])
      {
        temp = read_buf[i];
        read_buf[i] = read_buf[j];
        read_buf[j] = temp;
      }
    }
  }

  for(i=LOST_VAL; i < READ_TIMES-LOST_VAL; i++)
  {
    sum += read_buf[i];
  }

  temp = sum/(READ_TIMES-2*LOST_VAL);
  return temp;   
} 

uint16_t XPT2046_Repeated_Compare_AD(uint8_t CMD) 
{
  uint16_t num1, num2;
  num1 = XPT2046_Average_AD(CMD);
  num2 = XPT2046_Average_AD(CMD);

  if(((num2 <= num1) && (num1 < num2 + ERR_RANGE)) || ((num1 <= num2) && (num2 < num1 + ERR_RANGE)))
  {
    return (num1+num2)/2;
  }
  else return 0;	  
}

