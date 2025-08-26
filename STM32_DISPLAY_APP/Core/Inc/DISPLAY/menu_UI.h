/*
 * menu_UI.h
 *
 *  Created on: Aug 19, 2025
 *      Author: kjeyabalan
 */

#ifndef INC_DISPLAY_MENU_UI_H_
#define INC_DISPLAY_MENU_UI_H_


#include "display_spi_app.h"

extern int current_menu;
extern int current_selection;
extern int current_device;
extern int last_selection;
extern int downbutton, upbutton, enter;
extern int device_states[4];

extern int temp_hour, temp_minute, temp_second;
extern int temp_date, temp_month, temp_year, temp_day;
extern uint8_t temp_device1, temp_device2, temp_device3, temp_device4;
extern int current_alarm; // 0 for Alarm A, 1 for Alarm B
extern int scroll_offset;

extern bool increment_mode;
extern int selected_field;


void displayMainMenu(void);
void displayTotalControlMenu(void);
void displaySeparateControlMenu(void);
void displayDeviceControlMenu(void);
void displaySetTimeMenu(void);
void displaySetDateMenu(void);
void displaySetAlarmMenu(void);
void displayClockSettingsMenu(void);
void handleIncrementDecrement(void);
void alarm_A_setMenu(void);
void alarm_B_setMenu(void);
void displayDeviceSelectForAlarm(void);
void displayAlarmSetTimeMenu(void);
void ST7735_WriteCenteredTimeDate(uint16_t y, const char* dateStr, const char* timeStr, FontDef font, uint16_t color, uint16_t bgcolor);
void drawButton(int x, int y, int width, int height, char* text, int selected);
void drawSingleButton(int x, int y, int width, int height, char* text, int selected, int button_id);
void updateButtonSelection(int old_selection, int new_selection);
void drawTitleBar(char* title);

#endif /* INC_DISPLAY_MENU_UI_H_ */
