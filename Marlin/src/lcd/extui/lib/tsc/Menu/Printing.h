#ifndef _PRINTING_H_
#define _PRINTING_H_

/**
  * Maximum hotend temperature of automatic shut down after printing.
  * When enable automatic shutdown(Auto Power), when the hotend temperature is higher than this value
  * turn on the fan to cool down, wait for the hotend temperature to be lower than this value, then turn off the power automatically
  */
#define AUTO_SHUT_DOWN_MAXTEMP 50

#if ENABLED(HAS_UDISK)
  // extern bool UDiskPrint;
  // extern bool UDiskPausePrint;
  // extern bool UDiskStopPrint;
  extern bool UDiskPrintFinish;
#endif
#if ENABLED(USART_LCD)
  extern uint8_t printPaused;
  extern uint8_t printPaused2;
#endif

bool isPrinting(void);
bool isPrintInfoMenu(void);
bool isPaused(void);
bool setPrintPause(bool is_pause);
#if 0//ENABLED(HAS_UDISK)
  bool setUDiskPrintPause();
#endif

uint32_t getPrintTime(void);
uint8_t getPrintProgress(void);
void send_time(uint32_t pt);

void menuPrinting(void);
void menuStopPrinting(void);
void menuShutDown(void);

void resumeToPause(bool is_pause);

#endif
