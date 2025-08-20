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

// External request structure
typedef struct {
    uint8_t request_type;  // 1=SET_TIME, 2=SET_DATE, 3=SET_ALARM_A, 4=SET_ALARM_B
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t date;
    uint8_t month;
    uint8_t year;
    uint8_t day;
    uint8_t alarm_type;  // 0=Alarm A, 1=Alarm B
} rtc_request_t;

// Queue for external requests
QueueHandle_t rtcRequestQueue = NULL;

// Request types
#define RTC_REQUEST_SET_TIME    1
#define RTC_REQUEST_SET_DATE    2
#define RTC_REQUEST_SET_ALARM_A 3
#define RTC_REQUEST_SET_ALARM_B 4

typedef enum
{
	RTC_INIT,
	RTC_SET_TIME,
	RTC_SET_DATE,
	RTC_SET_ALARM,
	RTC_GET_DATE_TIME,
	RTC_IDLE,
	RTC_PROCESS_EXTERNAL_REQUEST
}RTC_MACHINE;

static RTC_MACHINE rtcState = RTC_INIT;

static rtc_request_t pending_request = {0};
static bool has_pending_request = false;

void RTC_Task(void *param)
{
	rtcTaskHandle = xTaskGetCurrentTaskHandle();
	TickType_t xLastWakeTime = xTaskGetTickCount();

	rtcRequestQueue = xQueueCreate(5, sizeof(rtc_request_t));
	if (rtcRequestQueue == NULL) {
		safe_printf("Failed to create RTC request queue\n");
		return;
	}

	while(1)
	{
		rtc_request_t request;
		if (xQueueReceive(rtcRequestQueue, &request, 0) == pdPASS) {
			pending_request = request;
			has_pending_request = true;
			if (rtcState == RTC_IDLE) {
				rtcState = RTC_PROCESS_EXTERNAL_REQUEST;
			}
		}

		switch(rtcState)
		{
			case RTC_INIT:
				int init = RTC_init();
				rtcState = init == 1 ? RTC_GET_DATE_TIME : RTC_SET_TIME;
				break;

			case RTC_SET_TIME:
				set_time(16, 44, 0);
				rtcState = RTC_SET_DATE;
				break;

			case RTC_SET_DATE:
				set_date(25, 8, 19, 2);
				rtcState = RTC_SET_ALARM;
				break;

			case RTC_SET_ALARM:
				set_alarmA(16, 45, 0);
				set_alarmB(16, 46, 0);
				rtcState = RTC_GET_DATE_TIME;
				break;

			case RTC_GET_DATE_TIME:
				get_time_date(timeData, dateData);
				safe_printf("%s\n",timeData);
				safe_printf("%s\n",dateData);
				rtcState = RTC_IDLE;
				break;

			case RTC_PROCESS_EXTERNAL_REQUEST:
				if (has_pending_request) {
					switch(pending_request.request_type) {
						case RTC_REQUEST_SET_TIME:
							safe_printf("RTC: External time set request - %02d:%02d:%02d\n",
								pending_request.hour, pending_request.minute, pending_request.second);
							set_time(pending_request.hour, pending_request.minute, pending_request.second);
							break;

						case RTC_REQUEST_SET_DATE:
							safe_printf("RTC: External date set request - %02d-%02d-%02d (Day:%d)\n",
								pending_request.date, pending_request.month, pending_request.year, pending_request.day);
							set_date(pending_request.year, pending_request.month, pending_request.date, pending_request.day);
							break;

						case RTC_REQUEST_SET_ALARM_A:
							safe_printf("RTC: External Alarm A set request - %02d:%02d:%02d\n",
								pending_request.hour, pending_request.minute, pending_request.second);
							set_alarmA(pending_request.hour, pending_request.minute, pending_request.second);
							break;

						case RTC_REQUEST_SET_ALARM_B:
							safe_printf("RTC: External Alarm B set request - %02d:%02d:%02d\n",
								pending_request.hour, pending_request.minute, pending_request.second);
							set_alarmB(pending_request.hour, pending_request.minute, pending_request.second);
							break;
					}
					has_pending_request = false;
				}
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
					alarmEvent = 0;
				}
				if (has_pending_request) {
					rtcState = RTC_PROCESS_EXTERNAL_REQUEST;
				}
				break;
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
	}
}


bool rtc_request_set_time(uint8_t hour, uint8_t minute, uint8_t second)
{
	if (rtcRequestQueue == NULL) return false;

	rtc_request_t request = {
		.request_type = RTC_REQUEST_SET_TIME,
		.hour = hour,
		.minute = minute,
		.second = second
	};

	BaseType_t result = xQueueSend(rtcRequestQueue, &request, pdMS_TO_TICKS(100));
	return (result == pdPASS);
}


bool rtc_request_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t day)
{
	if (rtcRequestQueue == NULL) return false;

	rtc_request_t request = {
		.request_type = RTC_REQUEST_SET_DATE,
		.year = year,
		.month = month,
		.date = date,
		.day = day
	};

	BaseType_t result = xQueueSend(rtcRequestQueue, &request, pdMS_TO_TICKS(100));
	return (result == pdPASS);
}


bool rtc_request_set_alarm_a(uint8_t hour, uint8_t minute, uint8_t second)
{
	if (rtcRequestQueue == NULL) return false;

	rtc_request_t request = {
		.request_type = RTC_REQUEST_SET_ALARM_A,
		.hour = hour,
		.minute = minute,
		.second = second
	};

	BaseType_t result = xQueueSend(rtcRequestQueue, &request, pdMS_TO_TICKS(100));
	return (result == pdPASS);
}


bool rtc_request_set_alarm_b(uint8_t hour, uint8_t minute, uint8_t second)
{
	if (rtcRequestQueue == NULL) return false;

	rtc_request_t request = {
		.request_type = RTC_REQUEST_SET_ALARM_B,
		.hour = hour,
		.minute = minute,
		.second = second
	};

	BaseType_t result = xQueueSend(rtcRequestQueue, &request, pdMS_TO_TICKS(100));
	return (result == pdPASS);
}


void set_time_external(uint8_t hr, uint8_t min, uint8_t sec)
{
	rtc_request_set_time(hr, min, sec);
}

void set_date_external(uint8_t year, uint8_t month, uint8_t date, uint8_t day)
{
	rtc_request_set_date(year, month, date, day);
}

void set_alarmA_external(uint8_t hr, uint8_t min, uint8_t sec)
{
	rtc_request_set_alarm_a(hr, min, sec);
}

void set_alarmB_external(uint8_t hr, uint8_t min, uint8_t sec)
{
	rtc_request_set_alarm_b(hr, min, sec);
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
