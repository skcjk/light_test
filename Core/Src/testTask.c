#include "testTask.h"
#include <math.h>
#include "SDTask.h"
#include "cmsis_os.h"
#include "usart.h"
#include "rtc.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern osMutexId printMutexHandle;
extern rxStruct rx1S;
extern rxStruct rx2S;
extern uint8_t rx1Mutex;
extern uint8_t rx2Mutex;
extern uint8_t debugMode;
extern osMessageQId sdCmdQueueHandle;
extern osPoolId  sdCmdQueuePoolHandle;
extern RTC_TimeTypeDef RTC_TimeStruct;  
extern RTC_DateTypeDef RTC_DateStruct;
extern uint8_t aRx2Buffer;	

static sdStruct *sdSForQueue;

const char *preset_data = {
    "00_01_02_03_04_05_06_07_08_09_0a_0b_0c_0d_0e_0f_10_11_12_13_14_15_16_17_18_19_1a_1b_1c_1d_1e_1f_20_21_22_23_24_25_26_27_28_29_2a_2b_2c_2d_2e_2f_30"
};

void TestTask(void *argument){
    HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRx2Buffer, 1);
    while (1)
    {
        osDelay(1000);
        if (debugMode==1) continue;
        HAL_RTC_GetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);
        Date_write_BKP(&hrtc,&RTC_DateStruct);  // 更新备份寄存器中的日期信息,调用HAL_RTC_GetTime后会清空天数计数器，所以必须将日期保存至备份区

        // 将发送的数据以JSON格式写入sd卡，使用RTC时间
        sdSForQueue = osPoolAlloc(sdCmdQueuePoolHandle);
        if (sdSForQueue != NULL) {
            sdSForQueue->sd_cmd = SD_WRITE;
            snprintf((char *)sdSForQueue->rx_buf, SD_BUF_LEN,
            "{\"source_data\":\"%s\",\"year\":%04d,\"month\":%02d,\"day\":%02d,\"hour\":%02d,\"minute\":%02d,\"second\":%02d}\r\n",
            preset_data,
            RTC_DateStruct.Year + 2000,
            RTC_DateStruct.Month,
            RTC_DateStruct.Date,
            RTC_TimeStruct.Hours,
            RTC_TimeStruct.Minutes,
            RTC_TimeStruct.Seconds);
            if (osOK != osMessagePut(sdCmdQueueHandle, (uint32_t)sdSForQueue, osWaitForever)) {
            osPoolFree(sdCmdQueuePoolHandle, sdSForQueue); 
            }
        }

        osDelay(3000);
        osMutexWait(printMutexHandle, osWaitForever);
        printf("%s", preset_data);
        osMutexRelease(printMutexHandle);
        osDelay(100);
        // 将usart2收到的数据以JSON格式写入sd卡，使用RTC时间
        sdSForQueue = osPoolAlloc(sdCmdQueuePoolHandle);
        if (sdSForQueue != NULL) {
            sdSForQueue->sd_cmd = SD_WRITE;
            rx2Mutex = 1; // 锁定接收缓冲区，防止其他任务修改
            snprintf((char *)sdSForQueue->rx_buf, SD_BUF_LEN,
            "{\"tx1_rx2_data\":\"%s\",\"year\":%04d,\"month\":%02d,\"day\":%02d,\"hour\":%02d,\"minute\":%02d,\"second\":%02d}\r\n",
            rx2S.rx_buf,
            RTC_DateStruct.Year + 2000,
            RTC_DateStruct.Month,
            RTC_DateStruct.Date,
            RTC_TimeStruct.Hours,
            RTC_TimeStruct.Minutes,
            RTC_TimeStruct.Seconds);
            memset(rx2S.rx_buf, 0x00, RX_BUF_LEN);
            rx2S.data_length = 0; // 清空接收缓冲区
            rx2Mutex = 0; // 解锁接收缓冲区
            if (osOK != osMessagePut(sdCmdQueueHandle, (uint32_t)sdSForQueue, osWaitForever)) {
                osPoolFree(sdCmdQueuePoolHandle, sdSForQueue); 
            }
        }
        // 将usart1收到的数据以JSON格式写入sd卡，使用RTC时间
        sdSForQueue = osPoolAlloc(sdCmdQueuePoolHandle);
        if (sdSForQueue != NULL) {
            sdSForQueue->sd_cmd = SD_WRITE;
            rx1Mutex = 1; // 锁定接收缓冲区，防止其他任务修改
            snprintf((char *)sdSForQueue->rx_buf, SD_BUF_LEN,
            "{\"tx1_rx1_data\":\"%s\",\"year\":%04d,\"month\":%02d,\"day\":%02d,\"hour\":%02d,\"minute\":%02d,\"second\":%02d}\r\n",
            rx1S.rx_buf,
            RTC_DateStruct.Year + 2000,
            RTC_DateStruct.Month,
            RTC_DateStruct.Date,
            RTC_TimeStruct.Hours,
            RTC_TimeStruct.Minutes,
            RTC_TimeStruct.Seconds);
            memset(rx1S.rx_buf, 0x00, RX_BUF_LEN);
            rx1S.data_length = 0; // 清空接收缓冲区
            rx1Mutex = 0; // 解锁接收缓冲区
            if (osOK != osMessagePut(sdCmdQueueHandle, (uint32_t)sdSForQueue, osWaitForever)) {
                osPoolFree(sdCmdQueuePoolHandle, sdSForQueue); 
            }
        }

        osDelay(3000);
        osMutexWait(printMutexHandle, osWaitForever);
        HAL_UART_Transmit(&huart2, (const uint8_t *)preset_data, strlen(preset_data), 0xFF);
        while (huart2.gState != HAL_UART_STATE_READY) {
            osDelay(1); // 等待UART状态变为READY
        }
        osMutexRelease(printMutexHandle);
        osDelay(100);
        // 将usart1收到的数据以JSON格式写入sd卡，使用RTC时间
        sdSForQueue = osPoolAlloc(sdCmdQueuePoolHandle);
        if (sdSForQueue != NULL) {
            sdSForQueue->sd_cmd = SD_WRITE;
            rx1Mutex = 1; // 锁定接收缓冲区，防止其他任务修改
            snprintf((char *)sdSForQueue->rx_buf, SD_BUF_LEN,
            "{\"tx2_rx1_data\":\"%s\",\"year\":%04d,\"month\":%02d,\"day\":%02d,\"hour\":%02d,\"minute\":%02d,\"second\":%02d}\r\n",
            rx1S.rx_buf,
            RTC_DateStruct.Year + 2000,
            RTC_DateStruct.Month,
            RTC_DateStruct.Date,
            RTC_TimeStruct.Hours,
            RTC_TimeStruct.Minutes,
            RTC_TimeStruct.Seconds);
            memset(rx1S.rx_buf, 0x00, RX_BUF_LEN);
            rx1S.data_length = 0; // 清空接收缓冲区
            rx1Mutex = 0; // 解锁接收缓冲区
            if (osOK != osMessagePut(sdCmdQueueHandle, (uint32_t)sdSForQueue, osWaitForever)) {
                osPoolFree(sdCmdQueuePoolHandle, sdSForQueue); 
            }
        }
        // 将usart2收到的数据以JSON格式写入sd卡，使用RTC时间
        sdSForQueue = osPoolAlloc(sdCmdQueuePoolHandle);
        if (sdSForQueue != NULL) {
            sdSForQueue->sd_cmd = SD_WRITE;
            rx2Mutex = 1; // 锁定接收缓冲区，防止其他任务修改
            snprintf((char *)sdSForQueue->rx_buf, SD_BUF_LEN,
            "{\"tx2_rx2_data\":\"%s\",\"year\":%04d,\"month\":%02d,\"day\":%02d,\"hour\":%02d,\"minute\":%02d,\"second\":%02d}\r\n",
            rx2S.rx_buf,
            RTC_DateStruct.Year + 2000,
            RTC_DateStruct.Month,
            RTC_DateStruct.Date,
            RTC_TimeStruct.Hours,
            RTC_TimeStruct.Minutes,
            RTC_TimeStruct.Seconds);
            memset(rx2S.rx_buf, 0x00, RX_BUF_LEN);
            rx2S.data_length = 0; // 清空接收缓冲区
            rx2Mutex = 0; // 解锁接收缓冲区
            if (osOK != osMessagePut(sdCmdQueueHandle, (uint32_t)sdSForQueue, osWaitForever)) {
                osPoolFree(sdCmdQueuePoolHandle, sdSForQueue); 
            }
        }

    }
    
}
