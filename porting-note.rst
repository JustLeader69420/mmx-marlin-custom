#####################
porting note.
#####################


todo list
~~~~~~~~~~~~~~~~~~~~~

1. gpio led                 pass
#. gpio key xmin ymin zmin  pass
#. uart                     pass
#. stepper                  pass @ 1mm 10mm move test.
#. spi flash w25qxx         
#. spi sd card reader       pass
#. touch screen xpt2042     pass
#. fsmc lcd display.        pass. use other ui so far.
#. dma 
#. timer pwm buzzer
#. temp sensor              fix max/min temp bug.
#. runout sensor


gpio. f4
~~~~~~~~~~~~~~~~~~~~~


compatible with st parts:
    verified: gpio R/W


use EXTUI-example so far
~~~~~~~~~~~~~~~~~~~~~

123123
~~~~~~~~~~~~~~~~~~~~~


如何通过串口打印debug信息？？
~~~~~~~~~~~~~~~~~~~~~

也许是通过serial == host 之间这个来输出？


code excute sequence
###############################

main -- C:\Users\langgo\.platformio\packages\framework-arduinoststm32\cores\arduino\main.cpp
serialx.begin -- Marlin\src\HAL\STM32\MarlinSerial.h
hw-serial.begin -- C:\Users\langgo\.platformio\packages\framework-arduinoststm32\cores\arduino\HardwareSerial.cpp
uart_init -- C:\Users\langgo\.platformio\packages\framework-arduinoststm32\libraries\SrcWrapper\src\stm32\uart.c
HAL_Init setTimerIntPrio -- C:\Users\langgo\.platformio\packages\framework-arduinoststm32\libraries\SoftwareSerial\src\SoftwareSerial.cpp
tmc_stepper_swserial -- .pio\libdeps\langgo407ve\TMCStepper\src\source\TMC2208Stepper.cpp   setup param for init purpose.
sw_serial -- C:\Users\langgo\.platformio\packages\framework-arduinoststm32\libraries\SoftwareSerial\src\SoftwareSerial.cpp


stepper timer tim5
-------------------------------
tim cfg ARR = 2000. psc = 42,  timck = 84mhz
it freq = 84mhz / 42  = 2mhz  / 2k = 1khz


fsmc tft driver 
-------------------------------
1. fsmc rs cs backlight other data pins define & init.
#. fsmc clock init.
#. lcd rw via fsmc like lcd->ram = xxxx;
#. 今天调试时  相同代码情况下断点调试有部分内存会忽然变0的问题 断电重启后不复现  国产MCU 有时真坑。。。

bed level data
-------------------------------------
Bilinear Leveling Grid:
      0      1      2      3
 0 -1.082 -1.332 -1.700 -1.922
 1 -1.090 -1.408 -1.658 -1.905
 2 -1.127 -1.400 -1.652 -1.892
 3 -1.158 -1.441 -1.709 -1.926


是否编译支持BL的app？
-------------------------------------
 build_flags       = ${common_stm32.build_flags}
  -DVECT_TAB_OFFSET=0x0     //无loader，调试测试方便 
                            //并且; buildroot/share/PlatformIO/scripts/stm32_bootloader.py
  -DVECT_TAB_OFFSET=0x10000 //支持bootloader




usb serial 原装的CDC QUEUE解读笔记
-------------------------------------------

1. 首先分别初始化2个ring queue。分别给tx和rx用
2. 读对列和写队列区别是读可以读到r==w即下标相等，但写最多写到w=r-1否则就会丢失第一个字节
3. 入队时根据需要入队的大小分2种情况讨论（当前到末尾 和 是否有过圈后的一段）
4. 读取时，调用readBlock尽可能多的读取,但此时还没处理读指针rp
5. readblock返回buffer地址和size 需要自己memcpy去读出来。
6. 读完调用commitRead移动rp,读操作完成。
7. ########接收队列 rx queue ########
8. dequeue读取1byte。 peek只读取不出队
9. rx queue read仍然是分2种情况，最大可读取不过圈
10. 接收数据前用rxq_resvBlock准备好位置。重点是找到一个足够放maxpack大小的位置
11. 接收后仍然用commintBlock更新写位置