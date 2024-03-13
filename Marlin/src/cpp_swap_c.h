#ifndef __CPPSWAPC_H_
#define __CPPSWAPC_H_

#include "HAL/STM32/watchdog.h"
// #include "stdio.h"

void FeedDog(){
    extern "C"
    {
        HAL_watchdog_refresh();
    }
}
#endif
