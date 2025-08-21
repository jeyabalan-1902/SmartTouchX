/*
 * RTC_app.h
 *
 *  Created on: Aug 18, 2025
 *      Author: kjeyabalan
 */

#ifndef INC_APP_RTC_APP_H_
#define INC_APP_RTC_APP_H_

#include "main.h"

extern RTC_HandleTypeDef hrtc;

extern char timeData[15];
extern char dateData[15];
extern char alarm_A_data[15];
extern char alarm_B_data[15];


bool rtc_request_set_time(uint8_t hour, uint8_t minute, uint8_t second);
bool rtc_request_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t day);
bool rtc_request_set_alarm_a(uint8_t hour, uint8_t minute, uint8_t second);
bool rtc_request_set_alarm_b(uint8_t hour, uint8_t minute, uint8_t second);

void set_time_external(uint8_t hr, uint8_t min, uint8_t sec);
void set_date_external(uint8_t year, uint8_t month, uint8_t date, uint8_t day);
void set_alarmA_external(uint8_t hr, uint8_t min, uint8_t sec);
void set_alarmB_external(uint8_t hr, uint8_t min, uint8_t sec);

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
