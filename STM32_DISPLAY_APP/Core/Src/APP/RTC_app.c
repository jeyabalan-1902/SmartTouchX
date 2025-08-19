/*
 * RTC_app.c
 *
 *  Created on: Aug 18, 2025
 *      Author: kjeyabalan
 *
 */

#include <stdbool.h>
#include "RTC_app.h"
#include "user_app.h"
#include "display_spi_app.h"

char timeData[15];
char dateData[15];
char source[] = "RTC";

TaskHandle_t rtcTaskHandle = NULL;
volatile uint8_t alarmEvent = 0;

typedef enum
{
	RTC_INIT,
	RTC_SET_TIME,
	RTC_SET_DATE,
	RTC_SET_ALARM,
	RTC_GET_DATE_TIME,
	RTC_IDLE
}RTC_MACHINE;

static RTC_MACHINE rtcState = RTC_INIT;



void RTC_Task(void *param)
{
	rtcTaskHandle = xTaskGetCurrentTaskHandle();
	TickType_t xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		switch(rtcState)
		{
			case RTC_INIT:
				int init = RTC_init();
				rtcState = init == 1 ? RTC_SET_ALARM : RTC_SET_TIME;
				//rtcState = RTC_SET_TIME;
				break;

			case RTC_SET_TIME:
				set_time(11, 56, 0);
				rtcState = RTC_SET_DATE;
				break;

			case RTC_SET_DATE:
				set_date(25, 8, 19, 2);
				rtcState = RTC_SET_ALARM;
				break;

			case RTC_SET_ALARM:
				set_alarmA(12, 55, 0);
				set_alarmB(12, 56, 0);
				rtcState = RTC_GET_DATE_TIME;
				break;

			case RTC_GET_DATE_TIME:
				get_time_date(timeData, dateData);
				safe_printf("%s\n",timeData);
				safe_printf("%s\n",dateData);
				rtcState = RTC_IDLE;
				break;

			case RTC_IDLE:
			    if (ulTaskNotifyTake(pdTRUE, 0) > 0)
			    {
			        if (alarmEvent == 1)
			        {
			            setAllDevicesState(1, source);
			            safe_printf("Alarm A triggered Devices ON\n");
			        }
			        else if (alarmEvent == 2)
			        {
			            setAllDevicesState(0, source);
			            safe_printf("Alarm B triggered Devices OFF\n");
			        }
			        alarmEvent = 0; // reset
			    }

			    get_time_date(timeData, dateData);
			    safe_printf("%s\n", timeData);
			    safe_printf("%s\n", dateData);
			    rtcState = RTC_IDLE;
			    break;
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
	}
}



bool RTC_init(void)
{
	if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x2345)
	{
		  return false;
	}
	return true;
}


void set_time (uint8_t hr, uint8_t min, uint8_t sec)
{
	RTC_TimeTypeDef sTime = {0};

	sTime.Hours = hr;
	sTime.Minutes = min;
	sTime.Seconds = sec;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
}


void set_date (uint8_t year, uint8_t month, uint8_t date, uint8_t day)  // monday = 1
{

	RTC_DateTypeDef sDate = {0};
	sDate.WeekDay = day;
	sDate.Month = month;
	sDate.Date = date;
	sDate.Year = year;
	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x2345);  // backup register
}


void set_alarmA(uint8_t hr, uint8_t min, uint8_t sec)
{
    RTC_AlarmTypeDef sAlarm = {0};
    sAlarm.AlarmTime.Hours = hr;
    sAlarm.AlarmTime.Minutes = min;
    sAlarm.AlarmTime.Seconds = sec;
    sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY; // daily
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.Alarm = RTC_ALARM_A;

    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
}


void set_alarmB(uint8_t hr, uint8_t min, uint8_t sec)
{
    RTC_AlarmTypeDef sAlarm = {0};
    sAlarm.AlarmTime.Hours = hr;
    sAlarm.AlarmTime.Minutes = min;
    sAlarm.AlarmTime.Seconds = sec;
    sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY; // daily
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.Alarm = RTC_ALARM_B;

    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
}


void get_time_date(char *time, char *date)
{
  RTC_DateTypeDef gDate;
  RTC_TimeTypeDef gTime;
  HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
  sprintf((char*)time,"%02d:%02d:%02d",gTime.Hours, gTime.Minutes, gTime.Seconds);
  sprintf((char*)date,"%02d-%02d-%2d",gDate.Date, gDate.Month, 2000 + gDate.Year);
}


void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    alarmEvent = 1;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(rtcTaskHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
{
    alarmEvent = 2;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(rtcTaskHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
