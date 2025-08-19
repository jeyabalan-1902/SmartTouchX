/*
 * display_ctrl.h
 *
 *  Created on: Jul 8, 2025
 *      Author: kjeyabalan
 */

#ifndef INC_APP_DISPLAY_SPI_APP_H_
#define INC_APP_DISPLAY_SPI_APP_H_


#include <stdio.h>
#include <string.h>
#include "main.h"
#include "stm32f4xx_hal.h"

#include "fonts.h"
#include "GFX_FUNCTIONS.h"
#include "ST7735.h"


// Display dimensions for 1.8" ST7735
#define DISPLAY_WIDTH  160
#define DISPLAY_HEIGHT 128
#define BUTTON_WIDTH   140
#define BUTTON_HEIGHT  18
#define BUTTON_SPACING 22
#define MARGIN_X       10
#define TITLE_HEIGHT   15

// Menu state enumeration
typedef enum {
    MENU_MAIN = 0,
    MENU_TOTAL_CONTROL,
    MENU_SEPARATE_CONTROL,
    MENU_DEVICE_CONTROL,
	MENU_AUDIO_MENU,
	MENU_AUDIO_ACTION,
	MENU_CLOCK_SETTINGS,
	MENU_SET_TIME,
	MENU_SET_DATE,
	MENU_SET_ALARM,
	MENU_HOUR_SELECT,
	MENU_MINUTE_SELECT,
	MENU_SECOND_SELECT,
	MENU_DATE_SELECT,
	MENU_MONTH_SELECT,
	MENU_YEAR_SELECT,
	MENU_DAY_SELECT
} menu_state_t;

typedef enum {
    BTN_EVENT_UP,
    BTN_EVENT_DOWN,
    BTN_EVENT_ENTER
} ButtonEvent_t;

// Menu configuration
#define MAIN_MENU_OPTIONS        3
#define TOTAL_CONTROL_OPTIONS    3
#define SEPARATE_CONTROL_OPTIONS 5
#define DEVICE_CONTROL_OPTIONS   4
#define CLOCK_SETTINGS_OPTIONS   4
#define SET_TIME_OPTIONS         5
#define SET_DATE_OPTIONS         5
#define SET_ALARM_OPTIONS        5
#define HOUR_SELECT_OPTIONS      6
#define MINUTE_SELECT_OPTIONS    10
#define SECOND_SELECT_OPTIONS    10
#define DATE_SELECT_OPTIONS      10
#define MONTH_SELECT_OPTIONS     14
#define YEAR_SELECT_OPTIONS      10
#define DAY_SELECT_OPTIONS       9


extern int current_menu;
extern int current_selection;
extern int current_device;
extern int last_selection;
extern int downbutton, upbutton, enter;
extern int device_states[4];


// Core menu functions
void Menu_Handler(void);
void initializeMenu(void);
void handleNavigation(void);

// Menu display functions
void displayMainMenu(void);
void displayTotalControlMenu(void);
void displaySeparateControlMenu(void);
void displayDeviceControlMenu(void);
void displayAudioMenu(void);
void displayAudioActionPage(void);
// Button drawing functions
void drawButton(int x, int y, int width, int height, char* text, int selected);

void drawSingleButton(int x, int y, int width, int height, char* text, int selected, int button_id);
void updateButtonSelection(int old_selection, int new_selection);
void drawTitleBar(char* title);

// Optimized update functions
void updateStatusInfo(char* status, uint16_t color);
void updateDeviceStatusText(int device_index, bool is_on);
void updateDeviceCount(int total_on);
void updateDeviceControlStatus(int device, bool is_on);

// Device control functions
void setDeviceState(int device, int state);
void setAllDevicesState(int state, char *source);
void syncDisplayDeviceStates(void);
void Display_Handler(void *param);
void updateToDisplayMenu(void);


void displayDaySelectMenu(void);
void displayYearSelectMenu(void);
void displayMonthSelectMenu(void);
void displayDateSelectMenu(void);
void displaySecondSelectMenu(void);
void displayMinuteSelectMenu(void);
void displayHourSelectMenu(void);
void displaySetTimeMenu(void);
void displaySetDateMenu(void);
void displaySetAlarmMenu(void);
void displayClockSettingsMenu(void);




#endif /* INC_APP_DISPLAY_SPI_APP_H_ */
