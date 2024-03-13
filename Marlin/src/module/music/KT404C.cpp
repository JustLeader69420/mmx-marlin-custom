#include "KT404C.h"
#include "../../MarlinCore.h"

#ifdef NEW_BOARD
/*******************************************/
QYMXFS kt404c;
#define USART6_TX	PG14
#define USART6_RX	PG9

const PinMap MY_PinMap_UART_TX[] = {
  {PG_14, USART6,  STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF8_USART6)},
  {NC,    NP,    0}
};

const PinMap MY_PinMap_UART_RX[] = {
  {PG_9,  USART6,  STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF8_USART6)},
  {NC,    NP,    0}
};
/*******************************************/
/*  初始化串口
 *  @param serial: 串口号
 *  @param bound: 波特率
 */
void QYMXFS::serial(USART_TypeDef * serial,int bound) {
  #if 0
	switch(serial) {
		case 1:
			m_serial=&Serial;
		break;
		case 2:
			m_serial=&Serial1;			
		break;
		case 3:
			m_serial=&Serial2;					
		break;
		case 4:
			m_serial=&Serial3;				
		break;
	}
	m_serial->begin(baudrat);
  #else
	UART_Handler.Instance=serial;					    //USART号
	UART_Handler.Init.BaudRate=bound;				    //波特率
	UART_Handler.Init.WordLength=UART_WORDLENGTH_8B;   	//字长为8位数据格式
	UART_Handler.Init.StopBits=UART_STOPBITS_1;	    	//一个停止位
	UART_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验
	UART_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   	//无硬件流控
	UART_Handler.Init.Mode=UART_MODE_TX_RX;		    	//模式：收发

	__HAL_RCC_USART6_CLK_ENABLE();
	pinmap_pinout(digitalPinToPinName(USART6_TX), MY_PinMap_UART_TX);
    pinmap_pinout(digitalPinToPinName(USART6_RX), MY_PinMap_UART_RX);
	
	HAL_UART_Init(&UART_Handler);					    //init

  #endif
}
int QYMXFS::available(void) {
	return m_serial->available();
}
//UART底层初始化，
//会被HAL_UART_Init()调用
//huart:串口句柄
// void HAL_UART_MspInit(UART_HandleTypeDef *huart)
// {
//     //GPIO¶Ë¿ÚÉèÖÃ
// 	GPIO_InitTypeDef GPIO_Initure;
	
// 	if(huart->Instance==USART6)					//Èç¹ûÊÇ´®¿Ú1£¬½øÐÐ´®¿Ú1 MSP³õÊ¼»¯
// 	{
// 		__HAL_RCC_GPIOG_CLK_ENABLE();			//Ê¹ÄÜGPIOAÊ±ÖÓ
// 		__HAL_RCC_USART6_CLK_ENABLE();			//Ê¹ÄÜUSART1Ê±ÖÓ
	
// 		GPIO_Initure.Pin=GPIO_PIN_9|GPIO_PIN_14;			//PG9:Rx  PG14:Tx
// 		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//¸´ÓÃÍÆÍìÊä³ö
// 		GPIO_Initure.Pull=GPIO_PULLUP;			//ÉÏÀ­
// 		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//¸ßËÙ
// 		GPIO_Initure.Alternate=GPIO_AF8_USART6;	//¸´ÓÃÎªUSART1
// 		HAL_GPIO_Init(GPIOG, &GPIO_Initure);	   	//³õÊ¼»¯PA9
// 	}
// }

/* 指定音乐播放
 * @param dir: 目录
 * @param no: 第no首
 */
void QYMXFS::play(int no){
	unsigned char  cmd[]={0xFF,0x06,0x03,0x00,0x00,no};
	cmd_write(cmd);
}
// void QYMXFS::play(int dir,int no) {
// 	unsigned char cmd[]={0xFF,0x06,0x0F,0x01,dir,no};
// 	cmd_write(cmd);
// 	// delay(40);
// }
/* 
 * 带音量调节播放
 * @param volume: 音量
 */
void QYMXFS::play(unsigned char volume,unsigned char no){
	unsigned char cmd[]={0xFF,0x06,0x22,0x00,volume,no};
	cmd_write(cmd);
}

// 暂停
void QYMXFS::stop() {
	unsigned char cmd[]={ 0xFF, 0x06, 0x0E, 0x00, 0x00, 0x00};
	cmd_write(cmd);		
}
// 开始
void QYMXFS::start() {
	unsigned char cmd[]={ 0xFF, 0x06, 0x0D, 0x00, 0x00, 0x00};
	cmd_write(cmd);	
}
// 停止播放
void QYMXFS::stop_decode() {
	unsigned char cmd[]={ 0xff, 0x06, 0x16, 0x00, 0x00, 0x00};
	cmd_write(cmd);
}

/*
 * 音量调节
 * @param opt: 
 * 				true 加
 * 				false 减
 * @param n: 音量
 */
void QYMXFS::voice_amp(bool opt){
	if(opt) {
		unsigned char cmd[]={0xFF,0x06,0x04,0x00,0x00,0x00};	// 音量加
		cmd_write(cmd);			
	} else {
		unsigned char cmd[]={0xFF,0x06,0x05,0x00,0x00,0x01};	// 音量减
		cmd_write(cmd);			
	}
}
void QYMXFS::voice_amp(int n){
	unsigned char cmd[]={0xFF,0x06,0x06,0x00,0x00,n};
	cmd_write(cmd);
}

/*
 * 选择播放音乐的磁盘
 * @param dev: 磁盘
 * 				0x01 udisk
 * 				0x02 sd
 * 				0x04 flash
 */
void QYMXFS::device_sel(int dev) {
	unsigned char cmd[]={0xff,0x06,0x09,0x00,0x00,dev};
	cmd_write(cmd);
}

// 模块复位
void QYMXFS::reset(){
	unsigned char cmd[]={0xFF,0x06,0x0C,0x00,0x00,0x00};
	cmd_write(cmd);
}

// 写指令
void QYMXFS::cmd_write(unsigned char *cmd) {
  #if 0

	unsigned int sum=0;
	struct checksum m_check=create_checksum(cmd);
		
	m_serial->write(0x7E);
	m_serial->write(cmd,6);
	m_serial->write(m_check.chk_sum_h);
	m_serial->write(m_check.chk_sum_l);
	m_serial->write(0xEF);
	
  #else

	unsigned char *p = cmd;
	uint8_t q[10] = {0};
	// checksum m_check=create_checksum(cmd);
	uint8_t i = 1;

	q[0] = 0x7E;
	i = 1;
	while(i != 7){
		q[i] = *cmd;
		cmd++;i++;
	}
	// q[7] = m_check.chk_sum_h;
	// q[8] = m_check.chk_sum_l;
	// q[9] = 0xEF;
	q[i] = 0xEF;
	
	HAL_UART_Transmit(&UART_Handler, q, 8, 0xffff);
	
  #endif
	delay(20);
}
// 为指令加校验码
struct checksum  QYMXFS::create_checksum(unsigned char cmd[]) { //cmd length:8 byte
 	unsigned int sum=0;
 	struct checksum mchk;
	for(int i=0;i<=5;i++){
		sum+=cmd[i];
	}
	sum=((0xFFFF-sum)+1);
	mchk.chk_sum_h=sum>>8;
	mchk.chk_sum_l=sum&0x00ff;
	return mchk;
}
#endif
