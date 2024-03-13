#ifndef __KT404C_H
#define __KT404C_H

#include <avr/pgmspace.h>

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

struct checksum{
	unsigned char chk_sum_h;
	unsigned char chk_sum_l;
};

class QYMXFS {
  public:
	unsigned char last_cmd[6];
	HardwareSerial * m_serial;//串口句柄指针
	UART_HandleTypeDef UART_Handler; //UART句柄
	void serial(USART_TypeDef * serial,int bound);
	int available(void);
	void play(int no);//撥放指定音樂
	// void play(int dir,int no);//指定資料夾音樂
	void play(unsigned char volume,unsigned char no);//指定音樂按音量调节
	void stop(void);//暫停
	void start(void);//resum
	void stop_decode(void);//停止
	void voice_amp(bool opt);//聲音大小調整--增减
	void voice_amp(int n);//聲音大小調整--指定音量
	void device_sel(int dev);
	void reset(void);//模塊重置
  private:
	void cmd_write(unsigned char *cmd);//串口发送数据
	struct checksum  create_checksum(unsigned char cmd[]);//checksum 校验码
};

extern QYMXFS kt404c;

#endif
