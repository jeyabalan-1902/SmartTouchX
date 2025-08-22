/*
 * menu_UI.c
 *
 *  Created on: Aug 19, 2025
 *      Author: kjeyabalan
 */


#include "menu_UI.h"
#include "RTC_app.h"


// Clock settings variables
int temp_hour = 0, temp_minute = 0, temp_second = 0;
int temp_date = 1, temp_month = 1, temp_year = 2025;
int temp_day = 1;
int current_alarm = 0; // 0 for Alarm A, 1 for Alarm B

bool increment_mode = false;  // Flag to indicate we're in increment/decrement mode
int selected_field = -1;

typedef struct {
    int x, y, width, height;
    int button_id;
    char text[25];
} button_position_t;

button_position_t current_buttons[6];
int button_count = 0;

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

	get_time_date(timeData, dateData);
	ST7735_WriteString(4, DISPLAY_HEIGHT - 18, dateData, Font_7x10, WHITE, BLACK);

	get_time_date(timeData, dateData);
	ST7735_WriteString(DISPLAY_WIDTH/2 + 2, DISPLAY_HEIGHT - 18, timeData, Font_7x10, YELLOW, BLACK);

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
						"GO TO HOME", (current_selection == 2), 2);

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
    syncDisplayDeviceStates();

    if (current_menu != last_menu || !menu_drawn) {
        ST7735_SetRotation(1);
        fillScreen(BLACK);
        drawTitleBar("DEVICE LIST");
        menu_drawn = true;
        buttons_drawn = false;
        last_menu = current_menu;
    }

    int start_y = TITLE_HEIGHT + 10;
    button_count = 5;

    if (!buttons_drawn) {
        for (int i = 0; i < 4; i++) {
            last_device_states[i] = device_states[i];
            char device_text[20];
            snprintf(device_text, sizeof(device_text), "DEVICE %d [%s]",
                     i + 1, device_states[i] ? "ON" : "OFF");
            drawSingleButton(MARGIN_X, start_y + i * 20, BUTTON_WIDTH, 16,
                             device_text, (current_selection == i), i);
        }

        drawSingleButton(MARGIN_X, start_y + 4 * 20, BUTTON_WIDTH, 16,
                         "GO TO HOME", (current_selection == 4), 4);

        buttons_drawn = true;
    }
    else {
        for (int i = 0; i < 4; i++) {
            if (device_states[i] != last_device_states[i]) {
                last_device_states[i] = device_states[i];

                char device_text[20];
                snprintf(device_text, sizeof(device_text), "DEVICE %d [%s]",
                         i + 1, device_states[i] ? "ON" : "OFF");

                drawSingleButton(MARGIN_X, start_y + i * 20, BUTTON_WIDTH, 16,
                                 device_text, (current_selection == i), i);
            }
        }
        if (last_selection != current_selection) {
            updateButtonSelection(last_selection, current_selection);
        }
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
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 2, BUTTON_WIDTH, BUTTON_HEIGHT, "SET SCHEDULE", (current_selection == 2), 2);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 3, BUTTON_WIDTH, BUTTON_HEIGHT, "GO TO HOME", (current_selection == 3), 3);

        buttons_drawn = true;
    }

    last_selection = current_selection;
}

void alarm_A_setMenu(void)
{
	if (current_menu != last_menu || !menu_drawn)
	{
		ST7735_SetRotation(1);
		fillScreen(BLACK);
		drawTitleBar("SCHEDULE-A");
		menu_drawn = true;
		buttons_drawn = false;
		last_menu = current_menu;
	}

	if (!buttons_drawn || last_selection != current_selection)
	{
		int start_y = TITLE_HEIGHT + 15;
		button_count = 4;

		drawSingleButton(MARGIN_X, start_y, BUTTON_WIDTH, BUTTON_HEIGHT, "SET TIME", (current_selection == 0), 0);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT, "SELECT DEVICES", (current_selection == 1), 1);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 2, BUTTON_WIDTH, BUTTON_HEIGHT, "GO BACK", (current_selection == 2), 2);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 3, BUTTON_WIDTH, BUTTON_HEIGHT, "GO TO HOME", (current_selection == 3), 3);

		buttons_drawn = true;
	}

	last_selection = current_selection;
}

void alarm_B_setMenu(void)
{
	if (current_menu != last_menu || !menu_drawn)
	{
		ST7735_SetRotation(1);
		fillScreen(BLACK);
		drawTitleBar("SCHEDULE-B");
		menu_drawn = true;
		buttons_drawn = false;
		last_menu = current_menu;
	}

	if (!buttons_drawn || last_selection != current_selection)
	{
		int start_y = TITLE_HEIGHT + 15;
		button_count = 4;

		drawSingleButton(MARGIN_X, start_y, BUTTON_WIDTH, BUTTON_HEIGHT, "SET TIME", (current_selection == 0), 0);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT, "SELECT DEVICES", (current_selection == 1), 1);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 2, BUTTON_WIDTH, BUTTON_HEIGHT, "GO BACK", (current_selection == 2), 2);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 3, BUTTON_WIDTH, BUTTON_HEIGHT, "GO TO HOME", (current_selection == 3), 3);

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
        int start_y = TITLE_HEIGHT + 15;
        button_count = 4;

        char schedule1[30];
        char schedule2[30];
        snprintf(schedule1, sizeof(schedule1), "SCHEDULE-A[%s]", alarm_A_data);
        snprintf(schedule2, sizeof(schedule2), "SCHEDULE-B[%s]", alarm_B_data);

        drawSingleButton(MARGIN_X, start_y, BUTTON_WIDTH, BUTTON_HEIGHT, schedule1, (current_selection == 0), 0);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT, schedule2, (current_selection == 1), 1);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 2, BUTTON_WIDTH, BUTTON_HEIGHT, "GO BACK", (current_selection == 2), 2);
        drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING * 3, BUTTON_WIDTH, BUTTON_HEIGHT, "GO TO HOME", (current_selection == 3), 3);

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
        increment_mode = false;  // Reset increment mode when entering menu
        selected_field = -1;
    }

    if (!buttons_drawn || last_selection != current_selection)
    {
        int start_y = TITLE_HEIGHT + 10;
        button_count = 5;
        get_time_date(timeData, dateData);
        char time_display[30];
        snprintf(time_display, sizeof(time_display), "TIME: %s", timeData);
        updateStatusInfo(time_display, WHITE);

        // Show current values and indicate which field is being modified
        char hour_text[15], minute_text[15], second_text[15];

        if (increment_mode && selected_field == 0) {
            snprintf(hour_text, sizeof(hour_text), "HOUR [%02d]", temp_hour);
        } else {
            snprintf(hour_text, sizeof(hour_text), "HOUR (%02d)", temp_hour);
        }

        if (increment_mode && selected_field == 1) {
            snprintf(minute_text, sizeof(minute_text), "MIN [%02d]", temp_minute);
        } else {
            snprintf(minute_text, sizeof(minute_text), "MIN (%02d)", temp_minute);
        }

        if (increment_mode && selected_field == 2) {
            snprintf(second_text, sizeof(second_text), "SEC [%02d]", temp_second);
        } else {
            snprintf(second_text, sizeof(second_text), "SEC (%02d)", temp_second);
        }

        // Reduced button height and spacing to fit all buttons
        int button_height = 16;
        int button_spacing = 18;

        // Draw time field buttons with reduced spacing
        drawSingleButton(MARGIN_X, start_y + 15, BUTTON_WIDTH, button_height, hour_text, (current_selection == 0), 0);
        drawSingleButton(MARGIN_X, start_y + button_spacing + 15, BUTTON_WIDTH, button_height, minute_text, (current_selection == 1), 1);
        drawSingleButton(MARGIN_X, start_y + button_spacing * 2 + 15, BUTTON_WIDTH, button_height, second_text, (current_selection == 2), 2);

        // Draw APPLY and GO BACK buttons in parallel (side by side)
        int parallel_y = start_y + button_spacing * 3 + 20;
        int button_width_half = (BUTTON_WIDTH - 5) / 2; // Split width with small gap

        drawSingleButton(MARGIN_X, parallel_y, button_width_half, button_height, "APPLY", (current_selection == 3), 3);
        drawSingleButton(MARGIN_X + button_width_half + 5, parallel_y, button_width_half, button_height, "GO BACK", (current_selection == 4), 4);

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
        increment_mode = false;  // Reset increment mode when entering menu
        selected_field = -1;
    }

    if (!buttons_drawn || last_selection != current_selection)
    {
        int start_y = TITLE_HEIGHT + 10;
        button_count = 6;
        get_time_date(timeData, dateData);
        char date_display[30];
        snprintf(date_display, sizeof(date_display), "DATE: %s", dateData);
        updateStatusInfo(date_display, WHITE);

        // Show current values and indicate which field is being modified
        char date_text[15], month_text[15], year_text[15], day_text[15];

        if (increment_mode && selected_field == 0) {
            snprintf(date_text, sizeof(date_text), "DATE [%02d]", temp_date);
        } else {
            snprintf(date_text, sizeof(date_text), "DATE (%02d)", temp_date);
        }

        if (increment_mode && selected_field == 1) {
            snprintf(month_text, sizeof(month_text), "MON [%02d]", temp_month);
        } else {
            snprintf(month_text, sizeof(month_text), "MON (%02d)", temp_month);
        }

        if (increment_mode && selected_field == 2) {
            snprintf(year_text, sizeof(year_text), "YEAR [%02d]", temp_year);
        } else {
            snprintf(year_text, sizeof(year_text), "YEAR (%02d)", temp_year);
        }

        if (increment_mode && selected_field == 3) {
            snprintf(day_text, sizeof(day_text), "DAY [%02d]", temp_day);
        } else {
            snprintf(day_text, sizeof(day_text), "DAY (%02d)", temp_day);
        }

        // Reduced button height and spacing to fit all buttons
        int button_height = 14;
        int button_spacing = 16;

        // Draw date field buttons with reduced spacing
        drawSingleButton(MARGIN_X, start_y + 15, BUTTON_WIDTH, button_height, date_text, (current_selection == 0), 0);
        drawSingleButton(MARGIN_X, start_y + button_spacing + 15, BUTTON_WIDTH, button_height, month_text, (current_selection == 1), 1);
        drawSingleButton(MARGIN_X, start_y + button_spacing * 2 + 15, BUTTON_WIDTH, button_height, year_text, (current_selection == 2), 2);
        drawSingleButton(MARGIN_X, start_y + button_spacing * 3 + 15, BUTTON_WIDTH, button_height, day_text, (current_selection == 3), 3);

        // Draw APPLY and GO BACK buttons in parallel (side by side)
        int parallel_y = start_y + button_spacing * 4 + 20;
        int button_width_half = (BUTTON_WIDTH - 5) / 2; // Split width with small gap

        drawSingleButton(MARGIN_X, parallel_y, button_width_half, button_height, "APPLY", (current_selection == 4), 4);
        drawSingleButton(MARGIN_X + button_width_half + 5, parallel_y, button_width_half, button_height, "GO BACK", (current_selection == 5), 5);

        buttons_drawn = true;
    }

    last_selection = current_selection;
}


void handleIncrementDecrement(void)
{
    if (!increment_mode) return;

    if (upbutton) {
        HAL_Delay(200);
        upbutton = 0;

        if (current_menu == MENU_SET_TIME) {
            switch(selected_field) {
                case 0: // Hour
                    temp_hour = (temp_hour + 1) % 24;
                    break;
                case 1: // Minute
                    temp_minute = (temp_minute + 1) % 60;
                    break;
                case 2: // Second
                    temp_second = (temp_second + 1) % 60;
                    break;
            }
        }
        else if (current_menu == MENU_SET_DATE) {
            switch(selected_field) {
                case 0: // Date
                    temp_date = (temp_date % 31) + 1;
                    break;
                case 1: // Month
                    temp_month = (temp_month % 12) + 1;
                    break;
                case 2: // Year
					temp_year = (temp_year - 25 + 1) % 10 + 25;
					break;
                case 3: // Day
                    temp_day = (temp_day % 7) + 1;
                    break;
            }
        }

        // Force redraw to show updated values
        buttons_drawn = false;
        last_selection = -1;
    }

    if (downbutton) {
        HAL_Delay(200);
        downbutton = 0;

        if (current_menu == MENU_SET_TIME) {
            switch(selected_field) {
                case 0: // Hour
                    temp_hour = (temp_hour - 1 + 24) % 24;
                    break;
                case 1: // Minute
                    temp_minute = (temp_minute - 1 + 60) % 60;
                    break;
                case 2: // Second
                    temp_second = (temp_second - 1 + 60) % 60;
                    break;
            }
        }
        else if (current_menu == MENU_SET_DATE) {
            switch(selected_field) {
                case 0: // Date
                    temp_date = temp_date == 1 ? 31 : temp_date - 1;
                    break;
                case 1: // Month
                    temp_month = temp_month == 1 ? 12 : temp_month - 1;
                    break;
                case 2: // Year
					temp_year = temp_year == 25 ? 34 : temp_year - 1;
					break;
                case 3: // Day
                    temp_day = temp_day == 1 ? 7 : temp_day - 1;
                    break;
            }
        }

        buttons_drawn = false;
        last_selection = -1;
    }
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


void ST7735_WriteCenteredTimeDate(uint16_t y, const char* dateStr, const char* timeStr, FontDef font, uint16_t color, uint16_t bgcolor)
{
    char buffer[40];
    sprintf(buffer, "%s  %s", dateStr, timeStr);
    uint16_t textWidth = strlen(buffer) * font.width;
    uint16_t x = (DISPLAY_WIDTH - textWidth) / 2;
    ST7735_WriteString(x, y, buffer, font, color, bgcolor);
}


