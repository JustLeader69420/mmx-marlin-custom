/**
  ******************************************************************************
  * @file    USB_Host/MSC_Standalone/Src/file_operations.c
  * @author  MCD Application Team
  * @brief   Write/read file on the disk.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright � 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
// #include "main.h"
// #include "usb_printf.h"
#include "ff.h"
// #include "log.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// extern FATFS USBHFatFS;
FIL MyFile;
FRESULT res;
uint32_t bytesWritten;
uint8_t rtext[200];
// uint8_t wtext[] = "USB Host Library : Mass Storage Example";

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Files operations: Read/Write and compare
  * @param  None
  * @retval None
  */
void MSC_File_Operations(void)
{
  uint16_t bytesread;

  // LOGI("INFO : FatFs Initialized \n");
  uint32_t fs_cluster;
  // FATFS* pfatfs = &USBH_fatfs;
  // if (FR_OK == f_getfree("0:", &fs_cluster, &pfatfs) ) {
  //   LOGI("disk free space %d clusters ", fs_cluster);
  //   LOGI("disk cluster size=%d ", pfatfs->csize * 512);
  // }

  // if(f_open(&MyFile, "0:USBHost.txt",FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
  // {
  //   LOGE("Cannot Open 'USBHost.txt' file \n");
  // }
  // else
  // {
  //   LOGI("INFO : 'USBHost.txt' opened for write  \n");
  //   res= f_write (&MyFile, wtext, sizeof(wtext), (void *)&bytesWritten);
  //   f_close(&MyFile);

  //   if((bytesWritten == 0) || (res != FR_OK)) /*EOF or Error*/
  //   {
  //     LOGE("Cannot Write on the  'USBHost.txt' file \n");
  //   }
  //   else
  //   {
  //     if(f_open(&MyFile, "0:USBHost.txt", FA_READ) != FR_OK)
  //     {
  //       LOGE("Cannot Open 'USBHost.txt' file for read.\n");
  //     }
  //     else
  //     {
  //       LOGI("INFO : Text written on the 'USBHost.txt' file \n");

  //       res = f_read(&MyFile, rtext, sizeof(rtext), (void *)&bytesread);

  //       if((bytesread == 0) || (res != FR_OK)) /*EOF or Error*/
  //       {
  //         LOGE("Cannot Read from the  'USBHost.txt' file \n");
  //       }
  //       else
  //       {
  //         LOGI("Read Text : \n");
  //         LOGW("%s",(char *)rtext);
  //         LOGW("\n");
  //       }
  //       f_close(&MyFile);
  //     }
  //     /* Compare read data with the expected data */
  //     if((bytesread == bytesWritten))
  //     {
  //       LOGI("INFO : FatFs data compare SUCCES");
  //       LOGI("\n");
  //     }
  //     else
  //     {
  //       LOGE("FatFs data compare ERROR");
  //       LOGE("\n");
  //     }
  //   }
  // }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
