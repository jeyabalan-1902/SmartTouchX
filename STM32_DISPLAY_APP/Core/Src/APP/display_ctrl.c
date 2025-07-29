/*
 * display_ctrl.c
 *
 *  Created on: Jul 8, 2025
 *      Author: kjeyabalan
 */

#include <APP/display_ctrl.h>
#include <APP/uart_rec.h>
#include <APP/user_app.h>
#include <onwords_logo.h>

int current_menu = MENU_MAIN;
int current_selection = 0;
int current_device = 0;
int last_selection = -1;
uint8_t current_line = 0;
char display_buffer[MAX_DISPLAY_LINES][23];
int last_menu = -1;

int downbutton = 0, upbutton = 0, enter = 0;

// 0=OFF, 1=ON
int device_states[4] = {0, 0, 0, 0};
int last_device_states[4] = {-1, -1, -1, -1};

bool menu_drawn = false;
bool buttons_drawn = false;

typedef struct {
    int x, y, width, height;
    int button_id;
    char text[25];
} button_position_t;

button_position_t current_buttons[6];
int button_count = 0;

void Display_Handler(void *param)
{
	while(1)
	{
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

void drawStatusInfo(char* status, uint16_t color) {
    fillRect(MARGIN_X, TITLE_HEIGHT + 5, BUTTON_WIDTH, 12, BLACK);
    ST7735_WriteString(MARGIN_X + 2, TITLE_HEIGHT + 7, status, Font_7x10, color, BLACK);
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
		button_count = 2;

		drawSingleButton(MARGIN_X, start_y, BUTTON_WIDTH, BUTTON_HEIGHT,
						"MASTER CONTROL", (current_selection == 0), 0);
		drawSingleButton(MARGIN_X, start_y + BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT,
						"DEVICE LIST", (current_selection == 1), 1);

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


void setDeviceState(int device, int state) {
    GPIO_TypeDef* gpio_led_port;
    GPIO_TypeDef *gpio_relay_port;
    uint16_t gpio_led_pin, gpio_relay_pin;
    char deviceItem[15];

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

		safe_printf("DISPLAY: Device %d set to %s\n", device + 1, state ? "ON" : "OFF");
	}
}

void setAllDevicesState(int state) {
	const char *devices[] = {"device1", "device2", "device3", "device4"};
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

        safe_printf("DISPLAY: All devices set to %s\n", state ? "ON" : "OFF");
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

void showActionFeedback(char* message, uint16_t color) {
    fillRect(MARGIN_X, DISPLAY_HEIGHT - 15, BUTTON_WIDTH, 12, BLACK);
    ST7735_WriteString(MARGIN_X + 2, DISPLAY_HEIGHT - 13, message, Font_7x10, color, BLACK);
    HAL_Delay(800);
    fillRect(MARGIN_X, DISPLAY_HEIGHT - 15, BUTTON_WIDTH, 12, BLACK);
}


void handleNavigation(void) {
    int max_options;

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
        default:
            max_options = 2;
            break;
    }

    if (upbutton) {
        HAL_Delay(200);
        int old_selection = current_selection;
        current_selection = (current_selection - 1 + max_options) % max_options;
        upbutton = 0;

        // Quick highlight update instead of full redraw
        switch(current_menu) {
            case MENU_MAIN:
                updateButtonSelection(old_selection, current_selection);
                last_selection = current_selection;
                break;
            case MENU_TOTAL_CONTROL:
                updateButtonSelection(old_selection, current_selection);
                last_selection = current_selection;
                break;
            case MENU_SEPARATE_CONTROL:
                updateButtonSelection(old_selection, current_selection);
                last_selection = current_selection;
                break;
            case MENU_DEVICE_CONTROL:
                updateButtonSelection(old_selection, current_selection);
                last_selection = current_selection;
                break;
        }
    }

    if (downbutton) {
        HAL_Delay(200);
        int old_selection = current_selection;
        current_selection = (current_selection + 1) % max_options;
        downbutton = 0;

        switch(current_menu) {
            case MENU_MAIN:
                updateButtonSelection(old_selection, current_selection);
                last_selection = current_selection;
                break;
            case MENU_TOTAL_CONTROL:
                updateButtonSelection(old_selection, current_selection);
                last_selection = current_selection;
                break;
            case MENU_SEPARATE_CONTROL:
                updateButtonSelection(old_selection, current_selection);
                last_selection = current_selection;
                break;
            case MENU_DEVICE_CONTROL:
                updateButtonSelection(old_selection, current_selection);
                last_selection = current_selection;
                break;
        }
    }

    if (enter) {
        HAL_Delay(200);
        enter = 0;

        switch(current_menu) {
            case MENU_MAIN:
                if (current_selection == 0) {
                    current_menu = MENU_TOTAL_CONTROL;
                    current_selection = 0;
                    menu_drawn = false; // Force menu redraw
                    displayTotalControlMenu();
                } else if (current_selection == 1) {
                    current_menu = MENU_SEPARATE_CONTROL;
                    current_selection = 0;
                    menu_drawn = false; // Force menu redraw
                    displaySeparateControlMenu();
                }
                break;

            case MENU_TOTAL_CONTROL:
                if (current_selection == 0) {
                    setAllDevicesState(1);
                } else if (current_selection == 1) {
                    setAllDevicesState(0);
                } else if (current_selection == 2) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false; // Force menu redraw
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
                    menu_drawn = false; // Force menu redraw
                    displayDeviceControlMenu();
                } else if (current_selection == 4) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false; // Force menu redraw
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
                    menu_drawn = false; // Force menu redraw
                    displaySeparateControlMenu();
                }else if (current_selection == 3) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false; // Force menu redraw
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;
        }
    }
}


void Menu_Handler(void) {
    switch(current_menu) {
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
    }

    handleNavigation();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_10) { // ENTER button
        downbutton = 0;
        upbutton = 0;
        enter = 1;
    }

    if (GPIO_Pin == GPIO_PIN_11) { // DOWN button
        downbutton = 1;
        upbutton = 0;
        enter = 0;
    }

    if (GPIO_Pin == GPIO_PIN_9) { // UP button
        downbutton = 0;
        upbutton = 1;
        enter = 0;
    }
}


void initializeMenu(void) {
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

void fastUpdateDeviceList(void) {
    if (current_menu == MENU_SEPARATE_CONTROL) {
        for (int i = 0; i < 4; i++) {
            if (device_states[i] != last_device_states[i]) {
                updateDeviceStatusText(i, device_states[i]);
                last_device_states[i] = device_states[i];
            }
        }
    }
}

// Fast status update for total control
void fastUpdateTotalControl(void) {
    if (current_menu == MENU_TOTAL_CONTROL) {
        int total_on = 0;
        for (int i = 0; i < 4; i++) {
            if (device_states[i]) total_on++;
        }
        updateDeviceCount(total_on);
    }
}

// Fast status update for device control
void fastUpdateDeviceControl(void) {
    if (current_menu == MENU_DEVICE_CONTROL) {
        updateDeviceControlStatus(current_device, device_states[current_device]);
    }
}

void ClearDisplay(void)
{
    fillScreen(BLACK);
    current_line = 0;

    // Redraw header
    ST7735_WriteString(5, 5, "-ONWORDS TOUCH BOARD-", Font_7x10, YELLOW, BLACK);
	ST7735_WriteString(5, 18, "    VERSION v3.0", Font_7x10, RED, BLACK);

    // Clear display buffer
    for(int i = 0; i < MAX_DISPLAY_LINES; i++) {
        memset(display_buffer[i], 0, 23);
    }
}

void ScrollDisplay(void)
{
    for(int i = 0; i < MAX_DISPLAY_LINES - 1; i++) {
        strcpy(display_buffer[i], display_buffer[i + 1]);
    }

    // Clear the last line
    memset(display_buffer[MAX_DISPLAY_LINES - 1], 0, 23);

    // Redraw all lines
    fillRect(0, 30, DISPLAY_WIDTH, MAX_DISPLAY_LINES * LINE_HEIGHT, BLACK);

    for(int i = 0; i < MAX_DISPLAY_LINES; i++) {
        if(strlen(display_buffer[i]) > 0) {
            ST7735_WriteString(5, 30 + (i * LINE_HEIGHT), display_buffer[i], Font_7x10, WHITE, BLACK);
        }
    }
}


void DisplayMessage(const char* message)
{
    if(!message) return;

    // If we've reached the bottom, scroll up
    if(current_line >= MAX_DISPLAY_LINES) {
        ClearDisplay();
        //current_line = MAX_DISPLAY_LINES - 1;
        current_line = 0;
    }

    strncpy(display_buffer[current_line], message, 22);
    display_buffer[current_line][22] = '\0';

    // Display the message
    ST7735_WriteString(5, 30 + (current_line * LINE_HEIGHT), display_buffer[current_line], Font_7x10, WHITE, BLACK);

    current_line++;
}

void print_To_display(char *format,...)
{
#ifdef BL_DEBUG_MSG_EN
	char str[80];

	/*Extract the the argument list using VA apis */
	va_list args;
	va_start(args, format);
	vsprintf(str, format,args);
	// Also display on screen
	DisplayMessage(str);
	va_end(args);
#else
    // If debug is disabled, still show on display
    char str[80];
    va_list args;
    va_start(args, format);
    vsprintf(str, format, args);
    DisplayMessage(str);
    va_end(args);
#endif
}

