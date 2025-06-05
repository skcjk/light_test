/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.h
  * @brief   This file contains all the function prototypes for
  *          the rtc.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
void Date_write_BKP(RTC_HandleTypeDef *  hrtc,RTC_DateTypeDef * Date);
void Date_read_BKP(RTC_HandleTypeDef *  hrtc);
/* USER CODE END Includes */

extern RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN Private defines */
typedef struct 
{
	uint8_t  Seconds;
	uint8_t  Minutes;
	uint8_t  Hours;
	uint8_t  Date;
	uint8_t  WeekDay;
	uint8_t  Month;
	uint16_t Year;
}BKTime;

HAL_StatusTypeDef UISet_Time(uint8_t hour,uint8_t min,uint8_t sec);
HAL_StatusTypeDef UISet_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week);
int16_t CharToDec(char *str, uint8_t cnt);
/* USER CODE END Private defines */

void MX_RTC_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */

