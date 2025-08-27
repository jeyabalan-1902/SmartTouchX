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
char alarm_A_data[15];
char alarm_B_data[15];


char source[] = "RTC";

TaskHandle_t rtcTaskHandle = NULL;
QueueHandle_t rtcRequestQueue = NULL;
volatile uint8_t alarmEvent = 0;


typedef enum
{
	RTC_REQUEST_SET_TIME = 1,
	RTC_REQUEST_SET_DATE,
	RTC_REQUEST_SET_ALARM_A,
	RTC_REQUEST_SET_ALARM_B,
	RTC_REQUEST_ADD_ALARM,
	RTC_REQUEST_CLEAR_ALARM
}EXT_RTC_REC;

typedef struct {
    EXT_RTC_REC request_type;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t date;
    uint8_t month;
    uint8_t year;
    uint8_t day;
    uint8_t alarm_type;
    uint8_t deviceMask;
    uint8_t alarm_index;
} rtc_request_t;


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


typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t deviceMask;
    bool active;
    bool repeat_daily;
} alarm_entry_t;

static alarm_entry_t allAlarms[MAX_ALARMS] = {0};


static RTC_MACHINE rtcState = RTC_INIT;
static rtc_request_t pending_request = {0};
static bool has_pending_request = false;
static uint8_t alarmA_device_mask = 0;
static uint8_t alarmB_device_mask = 0;

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
		rtcAlive = true;
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
						    safe_printf("RTC: External Alarm A set request - %02d:%02d:%02d, mask=0x%02X\n",
						        pending_request.hour, pending_request.minute, pending_request.second, pending_request.deviceMask);

						    set_alarmA(pending_request.hour, pending_request.minute, pending_request.second);
						    alarmA_device_mask = pending_request.deviceMask;
						    break;

						case RTC_REQUEST_SET_ALARM_B:
						    safe_printf("RTC: External Alarm B set request - %02d:%02d:%02d, mask=0x%02X\n",
						        pending_request.hour, pending_request.minute, pending_request.second, pending_request.deviceMask);

						    set_alarmB(pending_request.hour, pending_request.minute, pending_request.second);
						    alarmB_device_mask = pending_request.deviceMask;
						    break;

						case RTC_REQUEST_ADD_ALARM:
						    if (pending_request.alarm_index < MAX_ALARMS) {
						        allAlarms[pending_request.alarm_index].hour       = pending_request.hour;
						        allAlarms[pending_request.alarm_index].minute     = pending_request.minute;
						        allAlarms[pending_request.alarm_index].second     = pending_request.second;
						        allAlarms[pending_request.alarm_index].deviceMask = pending_request.deviceMask;
						        allAlarms[pending_request.alarm_index].active     = true;

						        safe_printf("RTC: Added Alarm[%d] %02d:%02d:%02d mask=0x%02X\n",
						            pending_request.alarm_index,
						            pending_request.hour, pending_request.minute, pending_request.second,
						            pending_request.deviceMask);

						        program_next_alarms();
						    }
						    break;

						case RTC_REQUEST_CLEAR_ALARM:
						    if (pending_request.alarm_index < MAX_ALARMS) {
						        allAlarms[pending_request.alarm_index].active = false;

						        safe_printf("RTC: Cleared Alarm[%d]\n", pending_request.alarm_index);
						        program_next_alarms();
						    }
						    break;
					}
					has_pending_request = false;
				}
				rtcState = RTC_IDLE;
				break;

			case RTC_IDLE:
				if (ulTaskNotifyTake(pdTRUE, 0) > 0)
				{
					if (alarmEvent == 1 || alarmEvent == 2) {
					    handle_alarm_trigger(alarmEvent);  // pass 1=AlarmA, 2=AlarmB
					    alarmEvent = 0;
					}
//				    if (alarmEvent == 1)
//				    {
//				        for (int i = 0; i < 4; i++) {
//				            int state = (alarmA_device_mask >> i) & 0x01;
//				            setDeviceState(i, state);
//				        }
//				        safe_printf("Alarm A triggered - Applied mask=0x%02X\n", alarmA_device_mask);
//				    }
//				    else if (alarmEvent == 2)
//				    {
//				        for (int i = 0; i < 4; i++) {
//				            int state = (alarmB_device_mask >> i) & 0x01;
//				            setDeviceState(i, state);
//				        }
//				        safe_printf("Alarm B triggered - Applied mask=0x%02X\n", alarmB_device_mask);
//				    }
//				    alarmEvent = 0;
				}
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


bool rtc_request_set_alarm_a(uint8_t hour, uint8_t minute, uint8_t second, uint8_t device_mask)
{
    if (rtcRequestQueue == NULL) return false;

    rtc_request_t request = {
        .request_type = RTC_REQUEST_SET_ALARM_A,
        .hour = hour,
        .minute = minute,
        .second = second,
        .deviceMask = device_mask
    };

    snprintf(alarm_A_data, sizeof(alarm_A_data), "%02d:%02d", hour, minute);

    BaseType_t result = xQueueSend(rtcRequestQueue, &request, pdMS_TO_TICKS(100));
    return (result == pdPASS);
}


bool rtc_request_set_alarm_b(uint8_t hour, uint8_t minute, uint8_t second, uint8_t device_mask)
{
	if (rtcRequestQueue == NULL) return false;

	rtc_request_t request = {
		.request_type = RTC_REQUEST_SET_ALARM_B,
		.hour = hour,
		.minute = minute,
		.second = second,
		.deviceMask = device_mask
	};
	snprintf(alarm_B_data, sizeof(alarm_B_data), "%02d:%02d", hour, minute);
	BaseType_t result = xQueueSend(rtcRequestQueue, &request, pdMS_TO_TICKS(100));
	return (result == pdPASS);
}


bool rtc_request_add_alarm(uint8_t index, uint8_t hour, uint8_t minute, uint8_t second, uint8_t device_mask, bool repeat_daily)
{
    if (rtcRequestQueue == NULL || index >= MAX_ALARMS) return false;

    rtc_request_t request = {
        .request_type = RTC_REQUEST_ADD_ALARM,
        .hour = hour,
        .minute = minute,
        .second = second,
        .deviceMask = device_mask,
        .alarm_index = index,
        .alarm_type = repeat_daily ? 1 : 0
    };

    return (xQueueSend(rtcRequestQueue, &request, pdMS_TO_TICKS(100)) == pdPASS);
}

bool rtc_request_clear_alarm(uint8_t index)
{
    if (rtcRequestQueue == NULL || index >= MAX_ALARMS) return false;

    rtc_request_t request = {
        .request_type = RTC_REQUEST_CLEAR_ALARM,
        .alarm_index  = index
    };

    BaseType_t result = xQueueSend(rtcRequestQueue, &request, pdMS_TO_TICKS(100));
    return (result == pdPASS);
}



void program_next_alarms(void)
{
    RTC_TimeTypeDef gTime;
    RTC_DateTypeDef gDate;
    HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

    int nextA = -1, nextB = -1;
    uint32_t minDiffA = UINT32_MAX, minDiffB = UINT32_MAX;

    uint32_t nowSec = gTime.Hours*3600 + gTime.Minutes*60 + gTime.Seconds;

    for (int i = 0; i < MAX_ALARMS; i++) {
        if (!allAlarms[i].active) continue;

        uint32_t alarmSec = allAlarms[i].hour*3600 + allAlarms[i].minute*60 + allAlarms[i].second;
        uint32_t diff = (alarmSec >= nowSec) ? (alarmSec - nowSec) : (86400 - nowSec + alarmSec);

        if (diff < minDiffA) {
            minDiffB = minDiffA; nextB = nextA;
            minDiffA = diff; nextA = i;
        } else if (diff < minDiffB) {
            minDiffB = diff; nextB = i;
        }
    }

    if (nextA != -1) {
        set_alarmA(allAlarms[nextA].hour, allAlarms[nextA].minute, allAlarms[nextA].second);
    }

    if (nextB != -1) {
        set_alarmB(allAlarms[nextB].hour, allAlarms[nextB].minute, allAlarms[nextB].second);
    }
}

void handle_alarm_trigger(uint8_t whichAlarm)
{
    RTC_TimeTypeDef gTime;
    RTC_DateTypeDef gDate;
    HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

    uint8_t hr  = gTime.Hours;
    uint8_t min = gTime.Minutes;
    uint8_t sec = gTime.Seconds;

    for (int i = 0; i < MAX_ALARMS; i++) {
        if (allAlarms[i].active &&
            allAlarms[i].hour == hr &&
            allAlarms[i].minute == min &&
            allAlarms[i].second == sec) {

            for (int d = 0; d < 4; d++) {
                int state = (allAlarms[i].deviceMask >> d) & 0x01;
                setDeviceState(d, state);
            }

            safe_printf("Alarm[%d] %s triggered at %02d:%02d:%02d mask=0x%02X (from HW %c)\n",
                        i,
                        allAlarms[i].repeat_daily ? "Daily" : "One-shot",
                        hr, min, sec,
                        allAlarms[i].deviceMask,
                        whichAlarm == 1 ? 'A' : 'B');

            if (!allAlarms[i].repeat_daily) {
                allAlarms[i].active = false;
            }
        }
    }
    program_next_alarms();
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
