
/*
 * display_spi_app.c
 *
 *  Created on: Jul 8, 2025
 *      Author: kjeyabalan
 */


#include "user_app.h"
#include "display_spi_app.h"
#include "onwords_logo.h"
#include "esp32_uart_app.h"
#include "RTC_app.h"

int current_menu = MENU_MAIN;
int current_selection = 0;
int current_device = 0;
int last_selection = -1;
int last_menu = -1;

int downbutton = 0, upbutton = 0, enter = 0;

// 0=OFF, 1=ON
int device_states[4] = {0, 0, 0, 0};
int last_device_states[4] = {-1, -1, -1, -1};
char selected_audio[20] = {0};
bool menu_drawn = false;
bool buttons_drawn = false;

// Clock settings variables
int temp_hour = 0, temp_minute = 0, temp_second = 0;
int temp_date = 1, temp_month = 1, temp_year = 25;
int temp_day = 1;
int current_alarm = 0; // 0 for Alarm A, 1 for Alarm B
int scroll_offset = 0;

typedef struct {
    int x, y, width, height;
    int button_id;
    char text[25];
} button_position_t;

button_position_t current_buttons[6];
int button_count = 0;

void Display_Handler(void *param)
{
	ButtonEvent_t evt;

	while(1)
	{
		if (xQueueReceive(btnEventQueue, &evt, 0) == pdPASS) {
		    if (evt == BTN_EVENT_UP) {
		        upbutton = 1;
		        downbutton = 0;
		        enter = 0;
		    }
		    else if (evt == BTN_EVENT_DOWN) {
		        upbutton = 0;
		        downbutton = 1;
		        enter = 0;
		    }
		    else if (evt == BTN_EVENT_ENTER) {
		        upbutton = 0;
		        downbutton = 0;
		        enter = 1;
		    }
		}
		Menu_Handler();
	}
}

void updateToDisplayMenu(void)
{
	if(current_menu == MENU_TOTAL_CONTROL)
	{
		last_selection = current_selection;
		displayTotalControlMenu();
	}
	else if(current_menu == MENU_SEPARATE_CONTROL)
	{
		last_selection = current_selection;
		displaySeparateControlMenu();
	}
	else if(current_menu == MENU_DEVICE_CONTROL)
	{
		last_selection = current_selection;
		displayDeviceControlMenu();
	}
	else if(current_menu == MENU_CLOCK_SETTINGS)
	{
		last_selection = current_selection;
		displayClockSettingsMenu();
	}
	else if(current_menu == MENU_SET_TIME)
	{
		last_selection = current_selection;
		displaySetTimeMenu();
	}
	else if(current_menu == MENU_SET_DATE)
	{
		last_selection = current_selection;
		displaySetDateMenu();
	}
	else if(current_menu == MENU_SET_ALARM)
	{
		last_selection = current_selection;
		displaySetAlarmMenu();
	}
	else if(current_menu == MENU_HOUR_SELECT)
	{
		last_selection = current_selection;
		displayHourSelectMenu();
	}
	else if(current_menu == MENU_MINUTE_SELECT)
	{
		last_selection = current_selection;
		displayMinuteSelectMenu();
	}
	else if(current_menu == MENU_SECOND_SELECT)
	{
		last_selection = current_selection;
		displaySecondSelectMenu();
	}
	else if(current_menu == MENU_DATE_SELECT)
	{
		last_selection = current_selection;
		displayDateSelectMenu();
	}
	else if(current_menu == MENU_MONTH_SELECT)
	{
		last_selection = current_selection;
		displayMonthSelectMenu();
	}
	else if(current_menu == MENU_YEAR_SELECT)
	{
		last_selection = current_selection;
		displayYearSelectMenu();
	}
	else if(current_menu == MENU_DAY_SELECT)
	{
		last_selection = current_selection;
		displayDaySelectMenu();
	}
}

void displayMainMenu(void)
{
	if (current_menu != last_menu || !menu_drawn)
	{
		ST7735_SetRotation(1);
		fillScreen(BLACK);
		drawTitleBar("HOME MENU");
		menu_drawn = true;
		buttons_drawn = false;
		last_menu = current_menu;
	}

	if (!buttons_drawn || last_selection != current_selection)
	{
		int start_y = TITLE_HEIGHT + 15;
		button_count = 3;

		drawSingleButton(MARGIN_X, start_y, BUTTON_WIDTH, BUTTON_HEIGHT,
						"MASTER CONTROL", (current_selection == 0), 0);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT,
						"DEVICE LIST", (current_selection == 1), 1);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 2, BUTTON_WIDTH, BUTTON_HEIGHT,
								"CLOCK SETTINGS", (current_selection == 2), 2);

		buttons_drawn = true;
	}

	last_selection = current_selection;
}


void displayTotalControlMenu(void)
{
	bool states_changed = false;
	syncDisplayDeviceStates();

	for (int i = 0; i < 4; i++)
	{
		if (device_states[i] != last_device_states[i])
		{
			states_changed = true;
			last_device_states[i] = device_states[i];
		}
	}

	if (current_menu != last_menu || !menu_drawn)
	{
		ST7735_SetRotation(1);
		fillScreen(BLACK);
		drawTitleBar("MASTER CONTROL");
		menu_drawn = true;
		buttons_drawn = false;
		last_menu = current_menu;
		states_changed = true;
	}

	if (states_changed)
	{
		int total_on = 0;
		for (int i = 0; i < 4; i++)
		{
			if (device_states[i]) total_on++;
		}
		updateDeviceCount(total_on);
	}

	if (!buttons_drawn)
	{
		int start_y = TITLE_HEIGHT + 25;
		button_count = 3;

		drawSingleButton(MARGIN_X, start_y, BUTTON_WIDTH, BUTTON_HEIGHT,
						"TOTAL ON", (current_selection == 0), 0);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT,
						"TOTAL OFF", (current_selection == 1), 1);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING*2, BUTTON_WIDTH, BUTTON_HEIGHT,
						"GO BACK", (current_selection == 2), 2);

		buttons_drawn = true;
	}
	else if (last_selection != current_selection)
	{
		updateButtonSelection(last_selection, current_selection);
	}

	last_selection = current_selection;
}

void displaySeparateControlMenu(void)
{
	bool states_changed = false;
	syncDisplayDeviceStates();

	for (int i = 0; i < 4; i++) {
		if (device_states[i] != last_device_states[i]) {
			states_changed = true;
			last_device_states[i] = device_states[i];
		}
	}

	if (current_menu != last_menu || !menu_drawn) {
		ST7735_SetRotation(1);
		fillScreen(BLACK);
		drawTitleBar("DEVICE LIST");
		menu_drawn = true;
		buttons_drawn = false;
		last_menu = current_menu;
		states_changed = true;
	}

	if (!buttons_drawn || states_changed) {
		int start_y = TITLE_HEIGHT + 10;
		button_count = 5;

		for (int i = 0; i < 4; i++)
		{
			char device_text[20];
			snprintf(device_text, sizeof(device_text), "DEVICE %d [%s]",
					i + 1, device_states[i] ? "ON" : "OFF");
			drawSingleButton(MARGIN_X, start_y + i * 20, BUTTON_WIDTH, 16,
						   device_text, (current_selection == i), i);
		}

		drawSingleButton(MARGIN_X, start_y + 4 * 20, BUTTON_WIDTH, 16,
					   "GO BACK", (current_selection == 4), 4);

		buttons_drawn = true;
	} else if (last_selection != current_selection) {
		updateButtonSelection(last_selection, current_selection);
	}

	last_selection = current_selection;
}

void displayDeviceControlMenu(void) {
	bool state_changed = false;
	syncDisplayDeviceStates();

	if (device_states[current_device] != last_device_states[current_device]) {
		state_changed = true;
		last_device_states[current_device] = device_states[current_device];
	}

	if (current_menu != last_menu || !menu_drawn) {
		ST7735_SetRotation(1);
		fillScreen(BLACK);

		char title[30];
		snprintf(title, sizeof(title), "DEVICE %d CONTROL", current_device + 1);
		drawTitleBar(title);

		menu_drawn = true;
		buttons_drawn = false;
		last_menu = current_menu;
		state_changed = true;
	}

	if (state_changed) {
		updateDeviceControlStatus(current_device, device_states[current_device]);
	}

	if (!buttons_drawn) {
		int start_y = TITLE_HEIGHT + 25;
		button_count = 4;

		drawSingleButton(MARGIN_X, start_y, BUTTON_WIDTH, BUTTON_HEIGHT,
						"TURN ON", (current_selection == 0), 0);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT,
						"TURN OFF", (current_selection == 1), 1);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING*2, BUTTON_WIDTH, BUTTON_HEIGHT,
						"GO BACK", (current_selection == 2), 2);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING*3, BUTTON_WIDTH, BUTTON_HEIGHT,
						"GO TO HOME", (current_selection == 3), 3);

		buttons_drawn = true;
	} else if (last_selection != current_selection) {
		updateButtonSelection(last_selection, current_selection);
	}

	last_selection = current_selection;
}

void handleNavigation(void) {
    int max_options;
    char src_name[] = "DISPLAY";

    switch(current_menu) {
        case MENU_MAIN:
            max_options = MAIN_MENU_OPTIONS;
            break;
        case MENU_TOTAL_CONTROL:
            max_options = TOTAL_CONTROL_OPTIONS;
            break;
        case MENU_SEPARATE_CONTROL:
            max_options = SEPARATE_CONTROL_OPTIONS;
            break;
        case MENU_DEVICE_CONTROL:
            max_options = DEVICE_CONTROL_OPTIONS;
            break;
        case MENU_CLOCK_SETTINGS:
            max_options = CLOCK_SETTINGS_OPTIONS;
            break;
        case MENU_SET_TIME:
            max_options = SET_TIME_OPTIONS;
            break;
        case MENU_SET_DATE:
            max_options = SET_DATE_OPTIONS;
            break;
        case MENU_SET_ALARM:
            max_options = SET_ALARM_OPTIONS;
            break;
        case MENU_HOUR_SELECT:
            max_options = HOUR_SELECT_OPTIONS;
            break;
        case MENU_MINUTE_SELECT:
            max_options = MINUTE_SELECT_OPTIONS;
            break;
        case MENU_SECOND_SELECT:
            max_options = SECOND_SELECT_OPTIONS;
            break;
        case MENU_DATE_SELECT:
            max_options = DATE_SELECT_OPTIONS;
            break;
        case MENU_MONTH_SELECT:
            max_options = MONTH_SELECT_OPTIONS;
            break;
        case MENU_YEAR_SELECT:
            max_options = YEAR_SELECT_OPTIONS;
            break;
        case MENU_DAY_SELECT:
            max_options = DAY_SELECT_OPTIONS;
            break;
        default:
            max_options = 2;
            break;
    }

    if (upbutton) {
        HAL_Delay(200);
        int old_selection = current_selection;

        // Handle scrolling for time/date selection menus
        if (current_menu == MENU_HOUR_SELECT || current_menu == MENU_MINUTE_SELECT ||
            current_menu == MENU_SECOND_SELECT || current_menu == MENU_DATE_SELECT) {

            if (current_selection == 0 && scroll_offset > 0) {
                scroll_offset -= 2;
                buttons_drawn = false;
                return;
            }
        }

        current_selection = (current_selection - 1 + max_options) % max_options;
        upbutton = 0;

        // Quick highlight update for all menus
        updateButtonSelection(old_selection, current_selection);
        last_selection = current_selection;
    }

    if (downbutton) {
        HAL_Delay(200);
        int old_selection = current_selection;

        // Handle scrolling for time/date selection menus
        if (current_menu == MENU_HOUR_SELECT) {
            if (current_selection >= 23 && scroll_offset < 18) {
                scroll_offset += 2;
                buttons_drawn = false;
                return;
            }
        }
        else if (current_menu == MENU_MINUTE_SELECT || current_menu == MENU_SECOND_SELECT) {
            if (current_selection >= 59 && scroll_offset < 52) {
                scroll_offset += 2;
                buttons_drawn = false;
                return;
            }
        }
        else if (current_menu == MENU_DATE_SELECT) {
            if (current_selection >= 30 && scroll_offset < 23) {
                scroll_offset += 2;
                buttons_drawn = false;
                return;
            }
        }

        current_selection = (current_selection + 1) % max_options;
        downbutton = 0;

        updateButtonSelection(old_selection, current_selection);
        last_selection = current_selection;
    }

    if (enter) {
        HAL_Delay(200);
        enter = 0;

        switch(current_menu) {
            case MENU_MAIN:
                if (current_selection == 0) {
                    current_menu = MENU_TOTAL_CONTROL;
                    current_selection = 0;
                    menu_drawn = false;
                    displayTotalControlMenu();
                } else if (current_selection == 1) {
                    current_menu = MENU_SEPARATE_CONTROL;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySeparateControlMenu();
                } else if (current_selection == 2) {
                    current_menu = MENU_CLOCK_SETTINGS;
                    current_selection = 0;
                    menu_drawn = false;
                    displayClockSettingsMenu();
                }
                break;

            case MENU_CLOCK_SETTINGS:
                if (current_selection == 0) {
                    // Get current time first
                    get_time_date(timeData, dateData);
                    sscanf(timeData, "%d:%d:%d", &temp_hour, &temp_minute, &temp_second);
                    current_menu = MENU_SET_TIME;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetTimeMenu();
                } else if (current_selection == 1) {
                    // Get current date first
                    get_time_date(timeData, dateData);
                    sscanf(dateData, "%d-%d-%d", &temp_date, &temp_month, &temp_year);
                    current_menu = MENU_SET_DATE;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetDateMenu();
                } else if (current_selection == 2) {
                    current_menu = MENU_SET_ALARM;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetAlarmMenu();
                } else if (current_selection == 3) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;

            case MENU_SET_TIME:
                if (current_selection == 0) {
                    current_menu = MENU_HOUR_SELECT;
                    current_selection = temp_hour;
                    scroll_offset = 0;
                    menu_drawn = false;
                    displayHourSelectMenu();
                } else if (current_selection == 1) {
                    current_menu = MENU_MINUTE_SELECT;
                    current_selection = temp_minute;
                    scroll_offset = 0;
                    menu_drawn = false;
                    displayMinuteSelectMenu();
                } else if (current_selection == 2) {
                    current_menu = MENU_SECOND_SELECT;
                    current_selection = temp_second;
                    scroll_offset = 0;
                    menu_drawn = false;
                    displaySecondSelectMenu();
                } else if (current_selection == 3) {
                    current_menu = MENU_CLOCK_SETTINGS;
                    current_selection = 0;
                    menu_drawn = false;
                    displayClockSettingsMenu();
                } else if (current_selection == 4) {
                    // Apply time and go to main menu
                    set_time(temp_hour, temp_minute, temp_second);
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;

            case MENU_SET_DATE:
                if (current_selection == 0) {
                    current_menu = MENU_DATE_SELECT;
                    current_selection = temp_date;
                    scroll_offset = 0;
                    menu_drawn = false;
                    displayDateSelectMenu();
                } else if (current_selection == 1) {
                    current_menu = MENU_MONTH_SELECT;
                    current_selection = temp_month;
                    menu_drawn = false;
                    displayMonthSelectMenu();
                } else if (current_selection == 2) {
                    current_menu = MENU_YEAR_SELECT;
                    current_selection = temp_year - 25;
                    menu_drawn = false;
                    displayYearSelectMenu();
                } else if (current_selection == 3) {
                    current_menu = MENU_DAY_SELECT;
                    current_selection = temp_day;
                    menu_drawn = false;
                    displayDaySelectMenu();
                } else if (current_selection == 4) {
                    current_menu = MENU_CLOCK_SETTINGS;
                    current_selection = 0;
                    menu_drawn = false;
                    displayClockSettingsMenu();
                } else if (current_selection == 5) {
                    // Apply date and go to main menu
                    set_date(temp_year, temp_month, temp_date, temp_day);
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;

            case MENU_SET_ALARM:
                if (current_selection == 0) {
                    current_alarm = 0; // Alarm A
                    current_menu = MENU_SET_TIME; // Reuse set time menu for alarm
                    current_selection = 0;
                    temp_hour = 0; temp_minute = 0; temp_second = 0;
                    menu_drawn = false;
                    displaySetTimeMenu();
                } else if (current_selection == 1) {
                    current_alarm = 1; // Alarm B
                    current_menu = MENU_SET_TIME; // Reuse set time menu for alarm
                    current_selection = 0;
                    temp_hour = 0; temp_minute = 0; temp_second = 0;
                    menu_drawn = false;
                    displaySetTimeMenu();
                } else if (current_selection == 2) {
                    current_menu = MENU_CLOCK_SETTINGS;
                    current_selection = 0;
                    menu_drawn = false;
                    displayClockSettingsMenu();
                }
                break;

            case MENU_HOUR_SELECT:
                if (current_selection < 24) {
                    temp_hour = current_selection;
                    current_menu = MENU_SET_TIME;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetTimeMenu();
                } else if (current_selection == 24) {
                    current_menu = MENU_SET_TIME;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetTimeMenu();
                } else if (current_selection == 25) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;

            case MENU_MINUTE_SELECT:
                if (current_selection < 60) {
                    temp_minute = current_selection;
                    current_menu = MENU_SET_TIME;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetTimeMenu();
                } else if (current_selection == 60) {
                    current_menu = MENU_SET_TIME;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetTimeMenu();
                } else if (current_selection == 61) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;

            case MENU_SECOND_SELECT:
                if (current_selection < 60) {
                    temp_second = current_selection;
                    current_menu = MENU_SET_TIME;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetTimeMenu();
                } else if (current_selection == 60) {
                    current_menu = MENU_SET_TIME;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetTimeMenu();
                } else if (current_selection == 61) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;

            case MENU_DATE_SELECT:
                if (current_selection < 32 && current_selection > 0) {
                    temp_date = current_selection;
                    current_menu = MENU_SET_DATE;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetDateMenu();
                } else if (current_selection == 31) {
                    current_menu = MENU_SET_DATE;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetDateMenu();
                } else if (current_selection == 32) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;

            case MENU_MONTH_SELECT:
                if (current_selection < 12) {
                    temp_month = current_selection + 1;
                    current_menu = MENU_SET_DATE;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetDateMenu();
                } else if (current_selection == 12) {
                    current_menu = MENU_SET_DATE;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetDateMenu();
                } else if (current_selection == 13) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;

            case MENU_YEAR_SELECT:
                if (current_selection < 10) {
                    temp_year = 25 + current_selection;
                    current_menu = MENU_SET_DATE;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetDateMenu();
                } else if (current_selection == 10) {
                    current_menu = MENU_SET_DATE;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetDateMenu();
                } else if (current_selection == 11) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;

            case MENU_DAY_SELECT:
                if (current_selection < 7) {
                    temp_day = current_selection + 1;
                    current_menu = MENU_SET_DATE;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetDateMenu();
                } else if (current_selection == 7) {
                    current_menu = MENU_SET_DATE;
                    current_selection = 0;
                    menu_drawn = false;
                    displaySetDateMenu();
                } else if (current_selection == 8) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;

            case MENU_TOTAL_CONTROL:
                if (current_selection == 0) {
                    setAllDevicesState(1, src_name);
                } else if (current_selection == 1) {
                    setAllDevicesState(0, src_name);
                } else if (current_selection == 2) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;

            case MENU_SEPARATE_CONTROL:
                if (current_selection >= 0 && current_selection <= 3) {
                    current_device = current_selection;
                    current_menu = MENU_DEVICE_CONTROL;
                    current_selection = 0;
                    menu_drawn = false;
                    displayDeviceControlMenu();
                } else if (current_selection == 4) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;

            case MENU_DEVICE_CONTROL:
                if (current_selection == 0) {
                    setDeviceState(current_device, 1);
                } else if (current_selection == 1) {
                    setDeviceState(current_device, 0);
                } else if (current_selection == 2) {
                    current_menu = MENU_SEPARATE_CONTROL;
                    current_selection = current_device;
                    menu_drawn = false;
                    displaySeparateControlMenu();
                }else if (current_selection == 3) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;
        }
    }
}

void Menu_Handler(void)
{
    switch(current_menu)
    {
        case MENU_MAIN:
            displayMainMenu();
            break;
        case MENU_TOTAL_CONTROL:
            displayTotalControlMenu();
            break;
        case MENU_SEPARATE_CONTROL:
            displaySeparateControlMenu();
            break;
        case MENU_DEVICE_CONTROL:
            displayDeviceControlMenu();
            break;
        case MENU_CLOCK_SETTINGS:
            displayClockSettingsMenu();
            break;
        case MENU_SET_TIME:
            displaySetTimeMenu();
            break;
        case MENU_SET_DATE:
            displaySetDateMenu();
            break;
        case MENU_SET_ALARM:
            displaySetAlarmMenu();
            break;
        case MENU_HOUR_SELECT:
            displayHourSelectMenu();
            break;
        case MENU_MINUTE_SELECT:
            displayMinuteSelectMenu();
            break;
        case MENU_SECOND_SELECT:
            displaySecondSelectMenu();
            break;
        case MENU_DATE_SELECT:
            displayDateSelectMenu();
            break;
        case MENU_MONTH_SELECT:
            displayMonthSelectMenu();
            break;
        case MENU_YEAR_SELECT:
            displayYearSelectMenu();
            break;
        case MENU_DAY_SELECT:
            displayDaySelectMenu();
            break;
    }
    handleNavigation();
}


void displayClockSettingsMenu(void)
{
    if (current_menu != last_menu || !menu_drawn)
    {
        ST7735_SetRotation(1);
        fillScreen(BLACK);
        drawTitleBar("CLOCK SETTINGS");
        menu_drawn = true;
        buttons_drawn = false;
        last_menu = current_menu;
    }

    if (!buttons_drawn || last_selection != current_selection)
    {
        int start_y = TITLE_HEIGHT + 15;
        button_count = 4;

        drawSingleButton(MARGIN_X, start_y, BUTTON_WIDTH, BUTTON_HEIGHT, "SET TIME", (current_selection == 0), 0);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT, "SET DATE", (current_selection == 1), 1);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 2, BUTTON_WIDTH, BUTTON_HEIGHT, "SET ALARM", (current_selection == 2), 2);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 3, BUTTON_WIDTH, BUTTON_HEIGHT, "GO BACK", (current_selection == 3), 3);

        buttons_drawn = true;
    }

    last_selection = current_selection;
}

void displaySetTimeMenu(void)
{
    if (current_menu != last_menu || !menu_drawn)
    {
        ST7735_SetRotation(1);
        fillScreen(BLACK);
        drawTitleBar("SET TIME");
        menu_drawn = true;
        buttons_drawn = false;
        last_menu = current_menu;
    }

    if (!buttons_drawn || last_selection != current_selection)
    {
        int start_y = TITLE_HEIGHT + 15;
        button_count = 5;

        char time_display[30];
        snprintf(time_display, sizeof(time_display), "TIME: %02d:%02d:%02d", temp_hour, temp_minute, temp_second);
        updateStatusInfo(time_display, WHITE);

        drawSingleButton(MARGIN_X, start_y + 15, BUTTON_WIDTH, BUTTON_HEIGHT, "HOUR", (current_selection == 0), 0);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING + 15, BUTTON_WIDTH, BUTTON_HEIGHT, "MINUTE", (current_selection == 1), 1);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 2 + 15, BUTTON_WIDTH, BUTTON_HEIGHT, "SECOND", (current_selection == 2), 2);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 3 + 15, BUTTON_WIDTH, BUTTON_HEIGHT, "GO BACK", (current_selection == 3), 3);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 4 + 15, BUTTON_WIDTH, BUTTON_HEIGHT, "GO TO MENU", (current_selection == 4), 4);

        buttons_drawn = true;
    }

    last_selection = current_selection;
}

void displaySetDateMenu(void)
{
    if (current_menu != last_menu || !menu_drawn)
    {
        ST7735_SetRotation(1);
        fillScreen(BLACK);
        drawTitleBar("SET DATE");
        menu_drawn = true;
        buttons_drawn = false;
        last_menu = current_menu;
    }

    if (!buttons_drawn || last_selection != current_selection)
    {
        int start_y = TITLE_HEIGHT + 15;
        button_count = 6;

        char date_display[30];
        snprintf(date_display, sizeof(date_display), "DATE: %02d-%02d-%02d", temp_date, temp_month, temp_year);
        updateStatusInfo(date_display, WHITE);

        drawSingleButton(MARGIN_X, start_y + 15, BUTTON_WIDTH, BUTTON_HEIGHT-5, "DATE", (current_selection == 0), 0);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING-5 + 15, BUTTON_WIDTH, BUTTON_HEIGHT-5, "MONTH", (current_selection == 1), 1);
        drawSingleButton(MARGIN_X, start_y + (BUTTON_SPACING-5) * 2 + 15, BUTTON_WIDTH, BUTTON_HEIGHT-5, "YEAR", (current_selection == 2), 2);
        drawSingleButton(MARGIN_X, start_y + (BUTTON_SPACING-5) * 3 + 15, BUTTON_WIDTH, BUTTON_HEIGHT-5, "DAY", (current_selection == 3), 3);
        drawSingleButton(MARGIN_X, start_y + (BUTTON_SPACING-5) * 4 + 15, BUTTON_WIDTH, BUTTON_HEIGHT-5, "GO BACK", (current_selection == 4), 4);
        drawSingleButton(MARGIN_X, start_y + (BUTTON_SPACING-5) * 5 + 15, BUTTON_WIDTH, BUTTON_HEIGHT-5, "GO TO MENU", (current_selection == 5), 5);

        buttons_drawn = true;
    }

    last_selection = current_selection;
}

void displaySetAlarmMenu(void)
{
    if (current_menu != last_menu || !menu_drawn)
    {
        ST7735_SetRotation(1);
        fillScreen(BLACK);
        drawTitleBar("SET ALARM");
        menu_drawn = true;
        buttons_drawn = false;
        last_menu = current_menu;
    }

    if (!buttons_drawn || last_selection != current_selection)
    {
        int start_y = TITLE_HEIGHT + 25;
        button_count = 3;

        drawSingleButton(MARGIN_X, start_y, BUTTON_WIDTH, BUTTON_HEIGHT, "ALARM A", (current_selection == 0), 0);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT, "ALARM B", (current_selection == 1), 1);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 2, BUTTON_WIDTH, BUTTON_HEIGHT, "GO BACK", (current_selection == 2), 2);

        buttons_drawn = true;
    }

    last_selection = current_selection;
}

void displayHourSelectMenu(void)
{
    if (current_menu != last_menu || !menu_drawn)
    {
        ST7735_SetRotation(1);
        fillScreen(BLACK);
        drawTitleBar("SELECT HOUR");
        menu_drawn = true;
        buttons_drawn = false;
        last_menu = current_menu;
        scroll_offset = 0;
    }

    if (!buttons_drawn || last_selection != current_selection)
    {
        int start_y = TITLE_HEIGHT + 10;
        int col_width = (DISPLAY_WIDTH - 2 * MARGIN_X - 10) / 2;
        int button_height = 16;
        int spacing = 18;

        // Clear the content area
        fillRect(0, start_y, DISPLAY_WIDTH, DISPLAY_HEIGHT - start_y, BLACK);

        button_count = 26; // 24 hours + go back + go to menu

        // Calculate visible range
        int visible_buttons = 6; // 3 rows, 2 columns
        int start_idx = scroll_offset;
        int end_idx = start_idx + visible_buttons;
        if (end_idx > 24) end_idx = 24;

        int btn_id = 0;

        // Draw hour buttons in 2-column layout
        for (int i = start_idx; i < end_idx; i += 2)
        {
            int row = (i - start_idx) / 2;
            int y_pos = start_y + row * spacing;

            // Left column
            char hour_text[10];
            snprintf(hour_text, sizeof(hour_text), "%02d", i);
            drawSingleButton(MARGIN_X, y_pos, col_width, button_height, hour_text,
                           (current_selection == i), btn_id++);

            // Right column (if exists)
            if (i + 1 < 24)
            {
                snprintf(hour_text, sizeof(hour_text), "%02d", i + 1);
                drawSingleButton(MARGIN_X + col_width + 10, y_pos, col_width, button_height, hour_text,
                               (current_selection == i + 1), btn_id++);
            }
        }

        // Draw control buttons at bottom
        int control_y = start_y + 3 * spacing + 10;
        drawSingleButton(MARGIN_X, control_y, col_width, button_height, "GO BACK",
                        (current_selection == 24), 24);
        drawSingleButton(MARGIN_X + col_width + 10, control_y, col_width, button_height, "GO TO MENU",
                        (current_selection == 25), 25);

        buttons_drawn = true;
    }

    last_selection = current_selection;
}

void displayMinuteSelectMenu(void)
{
    if (current_menu != last_menu || !menu_drawn)
    {
        ST7735_SetRotation(1);
        fillScreen(BLACK);
        drawTitleBar("SELECT MINUTE");
        menu_drawn = true;
        buttons_drawn = false;
        last_menu = current_menu;
        scroll_offset = 0;
    }

    if (!buttons_drawn || last_selection != current_selection)
    {
        int start_y = TITLE_HEIGHT + 10;
        int col_width = (DISPLAY_WIDTH - 2 * MARGIN_X - 10) / 2;
        int button_height = 14;
        int spacing = 16;

        fillRect(0, start_y, DISPLAY_WIDTH, DISPLAY_HEIGHT - start_y, BLACK);

        button_count = 62; // 60 minutes + go back + go to menu

        int visible_buttons = 8; // 4 rows, 2 columns
        int start_idx = scroll_offset;
        int end_idx = start_idx + visible_buttons;
        if (end_idx > 60) end_idx = 60;

        int btn_id = 0;

        for (int i = start_idx; i < end_idx; i += 2)
        {
            int row = (i - start_idx) / 2;
            int y_pos = start_y + row * spacing;

            char minute_text[10];
            snprintf(minute_text, sizeof(minute_text), "%02d", i);
            drawSingleButton(MARGIN_X, y_pos, col_width, button_height, minute_text,
                           (current_selection == i), btn_id++);

            if (i + 1 < 60)
            {
                snprintf(minute_text, sizeof(minute_text), "%02d", i + 1);
                drawSingleButton(MARGIN_X + col_width + 10, y_pos, col_width, button_height, minute_text,
                               (current_selection == i + 1), btn_id++);
            }
        }

        int control_y = start_y + 4 * spacing + 5;
        drawSingleButton(MARGIN_X, control_y, col_width, button_height, "GO BACK",
                        (current_selection == 60), 60);
        drawSingleButton(MARGIN_X + col_width + 10, control_y, col_width, button_height, "GO TO MENU",
                        (current_selection == 61), 61);

        buttons_drawn = true;
    }

    last_selection = current_selection;
}

void displaySecondSelectMenu(void)
{
    // Similar to minute select but for seconds (0-59)
    if (current_menu != last_menu || !menu_drawn)
    {
        ST7735_SetRotation(1);
        fillScreen(BLACK);
        drawTitleBar("SELECT SECOND");
        menu_drawn = true;
        buttons_drawn = false;
        last_menu = current_menu;
        scroll_offset = 0;
    }

    if (!buttons_drawn || last_selection != current_selection)
    {
        int start_y = TITLE_HEIGHT + 10;
        int col_width = (DISPLAY_WIDTH - 2 * MARGIN_X - 10) / 2;
        int button_height = 14;
        int spacing = 16;

        fillRect(0, start_y, DISPLAY_WIDTH, DISPLAY_HEIGHT - start_y, BLACK);

        button_count = 62; // 60 seconds + go back + go to menu

        int visible_buttons = 8;
        int start_idx = scroll_offset;
        int end_idx = start_idx + visible_buttons;
        if (end_idx > 60) end_idx = 60;

        int btn_id = 0;

        for (int i = start_idx; i < end_idx; i += 2)
        {
            int row = (i - start_idx) / 2;
            int y_pos = start_y + row * spacing;

            char second_text[10];
            snprintf(second_text, sizeof(second_text), "%02d", i);
            drawSingleButton(MARGIN_X, y_pos, col_width, button_height, second_text,
                           (current_selection == i), btn_id++);

            if (i + 1 < 60)
            {
                snprintf(second_text, sizeof(second_text), "%02d", i + 1);
                drawSingleButton(MARGIN_X + col_width + 10, y_pos, col_width, button_height, second_text,
                               (current_selection == i + 1), btn_id++);
            }
        }

        int control_y = start_y + 4 * spacing + 5;
        drawSingleButton(MARGIN_X, control_y, col_width, button_height, "GO BACK",
                        (current_selection == 60), 60);
        drawSingleButton(MARGIN_X + col_width + 10, control_y, col_width, button_height, "GO TO MENU",
                        (current_selection == 61), 61);

        buttons_drawn = true;
    }

    last_selection = current_selection;
}

void displayDateSelectMenu(void)
{
    if (current_menu != last_menu || !menu_drawn)
    {
        ST7735_SetRotation(1);
        fillScreen(BLACK);
        drawTitleBar("SELECT DATE");
        menu_drawn = true;
        buttons_drawn = false;
        last_menu = current_menu;
        scroll_offset = 0;
    }

    if (!buttons_drawn || last_selection != current_selection)
    {
        int start_y = TITLE_HEIGHT + 10;
        int col_width = (DISPLAY_WIDTH - 2 * MARGIN_X - 10) / 2;
        int button_height = 14;
        int spacing = 16;

        fillRect(0, start_y, DISPLAY_WIDTH, DISPLAY_HEIGHT - start_y, BLACK);

        button_count = 33; // 31 dates + go back + go to menu

        int visible_buttons = 8;
        int start_idx = scroll_offset + 1; // Start from 1
        int end_idx = start_idx + visible_buttons;
        if (end_idx > 31) end_idx = 31;

        int btn_id = 0;

        for (int i = start_idx; i < end_idx; i += 2)
        {
            int row = (i - start_idx) / 2;
            int y_pos = start_y + row * spacing;

            char date_text[10];
            snprintf(date_text, sizeof(date_text), "%02d", i);
            drawSingleButton(MARGIN_X, y_pos, col_width, button_height, date_text,
                           (current_selection == i), btn_id++);

            if (i + 1 <= 31)
            {
                snprintf(date_text, sizeof(date_text), "%02d", i + 1);
                drawSingleButton(MARGIN_X + col_width + 10, y_pos, col_width, button_height, date_text,
                               (current_selection == i + 1), btn_id++);
            }
        }

        int control_y = start_y + 4 * spacing + 5;
        drawSingleButton(MARGIN_X, control_y, col_width, button_height, "GO BACK",
                        (current_selection == 31), 31);
        drawSingleButton(MARGIN_X + col_width + 10, control_y, col_width, button_height, "GO TO MENU",
                        (current_selection == 32), 32);

        buttons_drawn = true;
    }

    last_selection = current_selection;
}

void displayMonthSelectMenu(void)
{
    if (current_menu != last_menu || !menu_drawn)
    {
        ST7735_SetRotation(1);
        fillScreen(BLACK);
        drawTitleBar("SELECT MONTH");
        menu_drawn = true;
        buttons_drawn = false;
        last_menu = current_menu;
    }

    if (!buttons_drawn || last_selection != current_selection)
    {
        int start_y = TITLE_HEIGHT + 10;
        int col_width = (DISPLAY_WIDTH - 2 * MARGIN_X - 10) / 2;
        int button_height = 16;
        int spacing = 18;

        fillRect(0, start_y, DISPLAY_WIDTH, DISPLAY_HEIGHT - start_y, BLACK);

        button_count = 14; // 12 months + go back + go to menu

        const char* months[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                               "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

        for (int i = 0; i < 12; i += 2)
        {
            int row = i / 2;
            int y_pos = start_y + row * spacing;

            drawSingleButton(MARGIN_X, y_pos, col_width, button_height, (char*)months[i],
                           (current_selection == i + 1), i);

            if (i + 1 < 12)
            {
                drawSingleButton(MARGIN_X + col_width + 10, y_pos, col_width, button_height, (char*)months[i + 1],
                               (current_selection == i + 2), i + 1);
            }
        }

        int control_y = start_y + 6 * spacing + 5;
        drawSingleButton(MARGIN_X, control_y, col_width, button_height, "GO BACK",
                        (current_selection == 12), 12);
        drawSingleButton(MARGIN_X + col_width + 10, control_y, col_width, button_height, "GO TO MENU",
                        (current_selection == 13), 13);

        buttons_drawn = true;
    }

    last_selection = current_selection;
}

void displayYearSelectMenu(void)
{
    if (current_menu != last_menu || !menu_drawn)
    {
        ST7735_SetRotation(1);
        fillScreen(BLACK);
        drawTitleBar("SELECT YEAR");
        menu_drawn = true;
        buttons_drawn = false;
        last_menu = current_menu;
    }

    if (!buttons_drawn || last_selection != current_selection)
    {
        int start_y = TITLE_HEIGHT + 15;
        button_count = 12; // 10 years + go back + go to menu

        // Show years 2025-2034 (25-34 in 2-digit format)
        for (int i = 0; i < 10; i++)
        {
            char year_text[10];
            snprintf(year_text, sizeof(year_text), "20%02d", 25 + i);
            drawSingleButton(MARGIN_X, start_y + i * 12, BUTTON_WIDTH, 10, year_text,
                           (current_selection == 25 + i), i);
        }

        drawSingleButton(MARGIN_X, start_y + 10 * 12, BUTTON_WIDTH, 10, "GO BACK",
                        (current_selection == 10), 10);
        drawSingleButton(MARGIN_X, start_y + 11 * 12, BUTTON_WIDTH, 10, "GO TO MENU",
                        (current_selection == 11), 11);

        buttons_drawn = true;
    }

    last_selection = current_selection;
}

void displayDaySelectMenu(void)
{
    if (current_menu != last_menu || !menu_drawn)
    {
        ST7735_SetRotation(1);
        fillScreen(BLACK);
        drawTitleBar("SELECT DAY");
        menu_drawn = true;
        buttons_drawn = false;
        last_menu = current_menu;
    }

    if (!buttons_drawn || last_selection != current_selection)
    {
        int start_y = TITLE_HEIGHT + 15;
        button_count = 9; // 7 days + go back + go to menu

        const char* days[] = {"MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY",
                             "FRIDAY", "SATURDAY", "SUNDAY"};

        for (int i = 0; i < 7; i++)
        {
            drawSingleButton(MARGIN_X, start_y + i * 15, BUTTON_WIDTH, 13, (char*)days[i],
                           (current_selection == i + 1), i);
        }

        drawSingleButton(MARGIN_X, start_y + 7 * 15, BUTTON_WIDTH, 13, "GO BACK",
                        (current_selection == 7), 7);
        drawSingleButton(MARGIN_X, start_y + 8 * 15, BUTTON_WIDTH, 13, "GO TO MENU",
                        (current_selection == 8), 8);

        buttons_drawn = true;
    }

    last_selection = current_selection;
}


void setDeviceState(int device, int state) {
    GPIO_TypeDef* gpio_led_port;
    GPIO_TypeDef *gpio_relay_port;
    uint16_t gpio_led_pin, gpio_relay_pin;
    char deviceItem[15];
    char msg[128];
    switch(device) {
        case 0:
            gpio_led_port = TOUCH_LED1_GPIO_Port;
            gpio_relay_port = L_RELAY_1_GPIO_Port;
            gpio_led_pin = TOUCH_LED1_Pin;
            gpio_relay_pin = L_RELAY_1_Pin;
            sprintf(deviceItem, "device1");
            break;
        case 1:
            gpio_led_port = TOUCH_LED2_GPIO_Port;
            gpio_relay_port = L_RELAY_2_GPIO_Port;
            gpio_led_pin = TOUCH_LED2_Pin;
            gpio_relay_pin = L_RELAY_2_Pin;
            sprintf(deviceItem, "device2");
            break;
        case 2:
            gpio_led_port = TOUCH_LED3_GPIO_Port;
            gpio_relay_port = L_RELAY_3_GPIO_Port;
            gpio_led_pin = TOUCH_LED3_Pin;
            gpio_relay_pin = L_RELAY_3_Pin;
            sprintf(deviceItem, "device3");
            break;
        case 3:
            gpio_led_port = TOUCH_LED4_GPIO_Port;
            gpio_relay_port = L_RELAY_4_GPIO_Port;
            gpio_led_pin = TOUCH_LED4_Pin;
            gpio_relay_pin = L_RELAY_4_Pin;
            sprintf(deviceItem, "device4");
            break;
        default:
            return;
    }

    if (xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
		if (state) {
			HAL_GPIO_WritePin(gpio_led_port, gpio_led_pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(gpio_relay_port, gpio_relay_pin, GPIO_PIN_SET);
			global_device_states[device] = 1;
			device_states[device] = 1;
		} else {
			HAL_GPIO_WritePin(gpio_led_port, gpio_led_pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(gpio_relay_port, gpio_relay_pin, GPIO_PIN_RESET);
			global_device_states[device] = 0;
			device_states[device] = 0;
		}

		xSemaphoreGive(deviceStateMutex);

		cJSON *resp = cJSON_CreateObject();
		cJSON_AddNumberToObject(resp, deviceItem, device_states[device]);
		char *respStr = cJSON_PrintUnformatted(resp);
		if (respStr)
		{
			HAL_UART_Transmit(&huart3, (uint8_t *)respStr, strlen(respStr), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
			free(respStr);
		}
		cJSON_Delete(resp);

		snprintf(msg, sizeof(msg), "Device %d turned %s\n", device + 1, state ? "ON" : "OFF");
		//HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

		safe_printf("DISPLAY: Device %d set to %s\n",  device + 1, state ? "ON" : "OFF");
	}
}

void setAllDevicesState(int state, char *source) {
	const char *devices[] = {"device1", "device2", "device3", "device4"};
	char msg[128];
	cJSON *resp = cJSON_CreateObject();
    if (xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        for (int i = 0; i < 4; i++) {
        	GPIO_TypeDef* gpio_led_port;
        	GPIO_TypeDef *gpio_relay_port;
			uint16_t gpio_led_pin, gpio_relay_pin;

            switch(i) {
                case 0:
                	gpio_led_port = TOUCH_LED1_GPIO_Port;
					gpio_relay_port = L_RELAY_1_GPIO_Port;
					gpio_led_pin = TOUCH_LED1_Pin;
					gpio_relay_pin = L_RELAY_1_Pin;
                    break;
                case 1:
                	gpio_led_port = TOUCH_LED2_GPIO_Port;
					gpio_relay_port = L_RELAY_2_GPIO_Port;
					gpio_led_pin = TOUCH_LED2_Pin;
					gpio_relay_pin = L_RELAY_2_Pin;
                    break;
                case 2:
                	gpio_led_port = TOUCH_LED3_GPIO_Port;
					gpio_relay_port = L_RELAY_3_GPIO_Port;
					gpio_led_pin = TOUCH_LED3_Pin;
					gpio_relay_pin = L_RELAY_3_Pin;
                    break;
                case 3:
                	gpio_led_port = TOUCH_LED4_GPIO_Port;
					gpio_relay_port = L_RELAY_4_GPIO_Port;
					gpio_led_pin = TOUCH_LED4_Pin;
					gpio_relay_pin = L_RELAY_4_Pin;
                    break;
            }

            if (state) {
            	HAL_GPIO_WritePin(gpio_led_port, gpio_led_pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(gpio_relay_port, gpio_relay_pin, GPIO_PIN_SET);
                global_device_states[i] = 1;
                device_states[i] = 1;
                cJSON_AddNumberToObject(resp, devices[i], device_states[i]);
            } else {
            	HAL_GPIO_WritePin(gpio_led_port, gpio_led_pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(gpio_relay_port, gpio_relay_pin, GPIO_PIN_RESET);
                global_device_states[i] = 0;
                device_states[i] = 0;
                cJSON_AddNumberToObject(resp, devices[i], device_states[i]);
            }
        }

        xSemaphoreGive(deviceStateMutex);

        char *respStr = cJSON_PrintUnformatted(resp);
		if (respStr)
		{
			HAL_UART_Transmit(&huart3, (uint8_t *)respStr, strlen(respStr), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
			free(respStr);
		}
		cJSON_Delete(resp);
		snprintf(msg, sizeof(msg),"All devices turned %s\n", state ? "ON" : "OFF");
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        safe_printf("%s: All devices set to %s\n", source, state ? "ON" : "OFF");
    }
}

void syncDisplayDeviceStates(void) {
    if (xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        for (int i = 0; i < 4; i++) {
            device_states[i] = global_device_states[i];
        }
        xSemaphoreGive(deviceStateMutex);
    }
}

void initializeMenu(void)
{
    safe_printf("display_init\n\r");
    current_menu = MENU_MAIN;
    current_selection = 0;
    current_device = 0;
    last_selection = -1;
    last_menu = -1;
    menu_drawn = false;
    buttons_drawn = false;

    for (int i = 0; i < 4; i++) {
        device_states[i] = 0;
        last_device_states[i] = -1;

        setDeviceState(i, 0);
    }
    displayMainMenu();
}



void drawSingleButton(int x, int y, int width, int height, char* text, int selected, int button_id)
{
    fillRect(x-1, y-1, width+2, height+2, BLACK);

    if (selected)
    {
        drawRoundRect(x-1, y-1, width+2, height+2, 3, WHITE);
        fillRoundRect(x, y, width, height, 3, GREEN);
        ST7735_WriteString(x+5, y+4, text, Font_7x10, BLACK, GREEN);
    }
    else
    {
        drawRoundRect(x-1, y-1, width+2, height+2, 3, WHITE);
        fillRoundRect(x, y, width, height, 3, GRAY);
        ST7735_WriteString(x+5, y+4, text, Font_7x10, WHITE, GRAY);
    }

    // Store button position AND TEXT for future updates
    if (button_id < 6)
    {
        current_buttons[button_id].x = x;
        current_buttons[button_id].y = y;
        current_buttons[button_id].width = width;
        current_buttons[button_id].height = height;
        current_buttons[button_id].button_id = button_id;
        strncpy(current_buttons[button_id].text, text, 24);
        current_buttons[button_id].text[24] = '\0';
    }
}

void updateButtonSelection(int old_selection, int new_selection)

{
    // Update old button (unselect)
    if (old_selection >= 0 && old_selection < button_count)
    {
        button_position_t *old_btn = &current_buttons[old_selection];

        // Clear and redraw unselected button with text
        fillRect(old_btn->x-1, old_btn->y-1, old_btn->width+2, old_btn->height+2, BLACK);
        drawRoundRect(old_btn->x-1, old_btn->y-1, old_btn->width+2, old_btn->height+2, 3, WHITE);
        fillRoundRect(old_btn->x, old_btn->y, old_btn->width, old_btn->height, 3, GRAY);
        ST7735_WriteString(old_btn->x+5, old_btn->y+4, old_btn->text, Font_7x10, WHITE, GRAY);
    }

    // Update new button (select)
    if (new_selection >= 0 && new_selection < button_count)
    {
        button_position_t *new_btn = &current_buttons[new_selection];

        // Clear and redraw selected button with text
        fillRect(new_btn->x-1, new_btn->y-1, new_btn->width+2, new_btn->height+2, BLACK);
        drawRoundRect(new_btn->x-1, new_btn->y-1, new_btn->width+2, new_btn->height+2, 3, WHITE);
        fillRoundRect(new_btn->x, new_btn->y, new_btn->width, new_btn->height, 3, GREEN);
        ST7735_WriteString(new_btn->x+5, new_btn->y+4, new_btn->text, Font_7x10, BLACK, GREEN);
    }
}

void updateDeviceStatusText(int device_index, bool is_on)
{
    int text_x = MARGIN_X + 5;
    int text_y = TITLE_HEIGHT + 10 + device_index * 20 + 4;
    fillRect(text_x + 70, text_y, 40, 10, GRAY);
    char status[8];
    snprintf(status, sizeof(status), "[%s]", is_on ? "ON" : "OFF");
    ST7735_WriteString(text_x + 70, text_y, status, Font_7x10, WHITE, GRAY);
}

void updateStatusInfo(char* status, uint16_t color)
{
    fillRect(MARGIN_X, TITLE_HEIGHT + 5, BUTTON_WIDTH, 12, BLACK);
    ST7735_WriteString(MARGIN_X + 2, TITLE_HEIGHT + 7, status, Font_7x10, color, BLACK);
}

void updateDeviceCount(int total_on)
{
    char status[35];
    if(total_on > 0)
    {
    	snprintf(status, sizeof(status), "Status: %d/4 devices", total_on);
    	updateStatusInfo(status, GREEN);
    }
    else
    {
    	snprintf(status, sizeof(status), "Status: %d/4 devices", total_on);
		updateStatusInfo(status, RED);
    }
}

void updateDeviceControlStatus(int device, bool is_on)
{
    char status[25];
    snprintf(status, sizeof(status), "Status: %s", is_on ? "ON" : "OFF");
    uint16_t status_color = is_on ? GREEN : RED;
    updateStatusInfo(status, status_color);
}

void drawCleanButton(int x, int y, int width, int height, char* text, int selected)
{
    if (selected)
    {
        drawRoundRect(x-1, y-1, width+2, height+2, 3, WHITE);
        fillRoundRect(x, y, width, height, 3, GREEN);
        ST7735_WriteString(x+5, y+4, text, Font_7x10, BLACK, GREEN);
    }
    else
    {
        drawRoundRect(x-1, y-1, width+2, height+2, 3, WHITE);
        fillRoundRect(x, y, width, height, 3, GRAY);
        ST7735_WriteString(x+5, y+4, text, Font_7x10, WHITE, GRAY);
    }
}

void drawTitleBar(char* title) {
    fillRect(0, 0, DISPLAY_WIDTH, TITLE_HEIGHT, BLUE);
    int title_length = strlen(title);
    int title_width = title_length * 7;
    int center_x = (DISPLAY_WIDTH - title_width) / 2;

    if (center_x < 2) center_x = 2;

    ST7735_WriteString(center_x, 2, title, Font_7x10, WHITE, BLUE);

    drawLine(0, TITLE_HEIGHT, DISPLAY_WIDTH, TITLE_HEIGHT, WHITE);
}


