/*
 * RTC_app.h
 *
 *  Created on: Aug 18, 2025
 *      Author: kjeyabalan
 */

#ifndef INC_APP_RTC_APP_H_
#define INC_APP_RTC_APP_H_

#include "main.h"
#include "user_app.h"


#define MAX_ALARMS  10

typedef struct
{
	uint8_t setAlarm_A;
	uint8_t setAlarm_B;
}selectAlarm;

extern RTC_HandleTypeDef hrtc;
extern selectAlarm deviceAlarm[DEVICE_COUNT];
extern char timeData[15];
extern char dateData[15];
extern char alarm_A_data[15];
extern char alarm_B_data[15];

bool rtc_request_clear_alarm(uint8_t index);
bool rtc_request_set_time(uint8_t hour, uint8_t minute, uint8_t second);
bool rtc_request_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t day);
bool rtc_request_set_alarm_a(uint8_t hour, uint8_t minute, uint8_t second, uint8_t device_mask);
bool rtc_request_set_alarm_b(uint8_t hour, uint8_t minute, uint8_t second, uint8_t device_mask);
bool rtc_request_add_alarm(uint8_t index, uint8_t hour, uint8_t minute, uint8_t second, uint8_t device_mask, bool repeat_daily);

void program_next_alarms(void);
void handle_alarm_trigger(uint8_t whichAlarm);

bool RTC_init(void);
void set_time (uint8_t hr, uint8_t min, uint8_t sec);
void set_date (uint8_t year, uint8_t month, uint8_t date, uint8_t day);
void set_alarm (uint8_t hr, uint8_t min, uint8_t sec);
void get_time_date(char *time, char *date);
void RTC_Task(void *param);

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);
void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc);
void set_alarmB(uint8_t hr, uint8_t min, uint8_t sec);
void set_alarmA(uint8_t hr, uint8_t min, uint8_t sec);

#endif /* INC_APP_RTC_APP_H_ */
