/*
 * display_ctrl.c
 *
 *  Created on: Jul 8, 2025
 *      Author: kjeyabalan
 */

#include "display_ctrl.h"

int current_menu = MENU_MAIN;
int current_selection = 0;
int current_device = 0;
int last_selection = -1;

int downbutton = 0, upbutton = 0, enter = 0;

volatile int global_device_states[4] = {0, 0, 0, 0};// 0=OFF, 1=ON
int device_states[4] = {0, 0, 0, 0};

void drawCleanButton(int x, int y, int width, int height, char* text, int selected) {
    if (selected) {
        drawRoundRect(x-1, y-1, width+2, height+2, 3, WHITE);
        fillRoundRect(x, y, width, height, 3, GREEN);
        ST7735_WriteString(x+5, y+4, text, Font_7x10, BLACK, GREEN);
    } else {
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


void displayMainMenu(void) {
    if (last_selection != current_selection || last_selection == -1) {
        ST7735_SetRotation(1);
        fillScreen(BLACK);

        drawTitleBar("HOME MENU");

        int start_y = TITLE_HEIGHT + 15;
        drawCleanButton(MARGIN_X, start_y, BUTTON_WIDTH, BUTTON_HEIGHT,
                       "GROUP CONTROL", (current_selection == 0));
        drawCleanButton(MARGIN_X, start_y + BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT,
                       "DEVICE LIST", (current_selection == 1));

        last_selection = current_selection;
    }
}

void displayTotalControlMenu(void) {
    if (last_selection != current_selection || last_selection == -1) {
    	syncDisplayDeviceStates();
        ST7735_SetRotation(1);
        fillScreen(BLACK);

        drawTitleBar("GROUP CONTROL");

        int total_on = 0;
        for (int i = 0; i < 4; i++) {
            if (device_states[i]) total_on++;
        }

        char status[25];
        snprintf(status, sizeof(status), "Active: %d/4 devices", total_on);
        drawStatusInfo(status, CYAN);

        int start_y = TITLE_HEIGHT + 25;
        drawCleanButton(MARGIN_X, start_y, BUTTON_WIDTH, BUTTON_HEIGHT,
                       "TOTAL ON", (current_selection == 0));
        drawCleanButton(MARGIN_X, start_y + BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT,
                       "TOTAL OFF", (current_selection == 1));
        drawCleanButton(MARGIN_X, start_y + BUTTON_SPACING*2, BUTTON_WIDTH, BUTTON_HEIGHT,
                       "GO BACK", (current_selection == 2));

        last_selection = current_selection;
    }
}

void displaySeparateControlMenu(void) {
    if (last_selection != current_selection || last_selection == -1) {
    	syncDisplayDeviceStates();
        ST7735_SetRotation(1);
        fillScreen(BLACK);

        drawTitleBar("DEVICE LIST");
        int start_y = TITLE_HEIGHT + 10;
        char device_text[20];

        for (int i = 0; i < 4; i++) {
            snprintf(device_text, sizeof(device_text), "DEVICE %d [%s]",
                    i + 1, device_states[i] ? "ON" : "OFF");
            drawCleanButton(MARGIN_X, start_y + i * 20, BUTTON_WIDTH, 16,
                           device_text, (current_selection == i));
        }
        drawCleanButton(MARGIN_X, start_y + 4 * 20, BUTTON_WIDTH, 16,
                       "GO BACK", (current_selection == 4));

        last_selection = current_selection;
    }
}

void displayDeviceControlMenu(void) {
    if (last_selection != current_selection || last_selection == -1) {
    	syncDisplayDeviceStates();
        ST7735_SetRotation(1);
        fillScreen(BLACK);

        char title[30];
        snprintf(title, sizeof(title), "DEVICE %d CONTROL", current_device + 1);
        drawTitleBar(title);

        char status[25];
        snprintf(status, sizeof(status), "Status: %s",
                device_states[current_device] ? "ON" : "OFF");
        uint16_t status_color = device_states[current_device] ? GREEN : RED;
        drawStatusInfo(status, status_color);

        int start_y = TITLE_HEIGHT + 25;
        drawCleanButton(MARGIN_X, start_y, BUTTON_WIDTH, BUTTON_HEIGHT,
                       "TURN ON", (current_selection == 0));
        drawCleanButton(MARGIN_X, start_y + BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT,
                       "TURN OFF", (current_selection == 1));
        drawCleanButton(MARGIN_X, start_y + BUTTON_SPACING*2, BUTTON_WIDTH, BUTTON_HEIGHT,
                       "GO BACK", (current_selection == 2));

        last_selection = current_selection;
    }
}



void setDeviceState(int device, int state) {
    GPIO_TypeDef* gpio_port;
    uint16_t gpio_pin;
    char deviceItem[15];

    switch(device) {
        case 0:
            gpio_port = TOUCH_LED1_GPIO_Port;
            gpio_pin = TOUCH_LED1_Pin;
            sprintf(deviceItem, "device1");
            break;
        case 1:
            gpio_port = TOUCH_LED2_GPIO_Port;
            gpio_pin = TOUCH_LED2_Pin;
            sprintf(deviceItem, "device2");
            break;
        case 2:
            gpio_port = TOUCH_LED3_GPIO_Port;
            gpio_pin = TOUCH_LED3_Pin;
            sprintf(deviceItem, "device3");
            break;
        case 3:
            gpio_port = TOUCH_LED4_GPIO_Port;
            gpio_pin = TOUCH_LED4_Pin;
            sprintf(deviceItem, "device4");
            break;
        default:
            return;
    }

    if (xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
		if (state) {
			HAL_GPIO_WritePin(gpio_port, gpio_pin, GPIO_PIN_SET);
			global_device_states[device] = 1;
			device_states[device] = 1;
		} else {
			HAL_GPIO_WritePin(gpio_port, gpio_pin, GPIO_PIN_RESET);
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

		printf("Display: Device %d set to %s\n", device + 1, state ? "ON" : "OFF");
	}
}

void setAllDevicesState(int state) {
	const char *devices[] = {"device1", "device2", "device3", "device4"};
	cJSON *resp = cJSON_CreateObject();
    if (xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        for (int i = 0; i < 4; i++) {
            GPIO_TypeDef* gpio_port;
            uint16_t gpio_pin;

            switch(i) {
                case 0:
                    gpio_port = TOUCH_LED1_GPIO_Port;
                    gpio_pin = TOUCH_LED1_Pin;
                    break;
                case 1:
                    gpio_port = TOUCH_LED2_GPIO_Port;
                    gpio_pin = TOUCH_LED2_Pin;
                    break;
                case 2:
                    gpio_port = TOUCH_LED3_GPIO_Port;
                    gpio_pin = TOUCH_LED3_Pin;
                    break;
                case 3:
                    gpio_port = TOUCH_LED4_GPIO_Port;
                    gpio_pin = TOUCH_LED4_Pin;
                    break;
            }

            if (state) {
                HAL_GPIO_WritePin(gpio_port, gpio_pin, GPIO_PIN_SET);
                global_device_states[i] = 1;
                device_states[i] = 1;
                cJSON_AddNumberToObject(resp, devices[i], device_states[i]);
            } else {
                HAL_GPIO_WritePin(gpio_port, gpio_pin, GPIO_PIN_RESET);
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

        printf("Display: All devices set to %s\n", state ? "ON" : "OFF");
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
        current_selection = (current_selection - 1 + max_options) % max_options;
        upbutton = 0;

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
    }

    if (downbutton) {
        HAL_Delay(200);
        current_selection = (current_selection + 1) % max_options;
        downbutton = 0;

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
    }

    if (enter) {
        HAL_Delay(200);
        enter = 0;

        switch(current_menu) {
            case MENU_MAIN:
                if (current_selection == 0) {
                    current_menu = MENU_TOTAL_CONTROL;
                    current_selection = 0;
                    last_selection = -1;
                    displayTotalControlMenu();
                } else if (current_selection == 1) {
                    current_menu = MENU_SEPARATE_CONTROL;
                    current_selection = 0;
                    last_selection = -1;
                    displaySeparateControlMenu();
                }
                break;

            case MENU_TOTAL_CONTROL:
                if (current_selection == 0) {
                    setAllDevicesState(1);
                    showActionFeedback("All Devices ON", GREEN);
                    last_selection = -1;
                    displayTotalControlMenu();
                } else if (current_selection == 1) {
                    setAllDevicesState(0);
                    showActionFeedback("All Devices OFF", RED);
                    last_selection = -1;
                    displayTotalControlMenu();
                } else if (current_selection == 2) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    last_selection = -1;
                    displayMainMenu();
                }
                break;

            case MENU_SEPARATE_CONTROL:
                if (current_selection >= 0 && current_selection <= 3) {
                    current_device = current_selection;
                    current_menu = MENU_DEVICE_CONTROL;
                    current_selection = 0;
                    last_selection = -1;
                    displayDeviceControlMenu();
                } else if (current_selection == 4) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    last_selection = -1;
                    displayMainMenu();
                }
                break;

            case MENU_DEVICE_CONTROL:
                if (current_selection == 0) {
                    setDeviceState(current_device, 1);
                    char msg[25];
                    snprintf(msg, sizeof(msg), "Device %d ON", current_device + 1);
                    showActionFeedback(msg, GREEN);
                    last_selection = -1;
                    displayDeviceControlMenu();
                } else if (current_selection == 1) {
                    setDeviceState(current_device, 0);
                    char msg[25];
                    snprintf(msg, sizeof(msg), "Device %d OFF", current_device + 1);
                    showActionFeedback(msg, RED);
                    last_selection = -1;
                    displayDeviceControlMenu();
                } else if (current_selection == 2) {
                    current_menu = MENU_SEPARATE_CONTROL;
                    current_selection = current_device;
                    last_selection = -1;
                    displaySeparateControlMenu();
                }
                break;
        }
    }
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

void Menu_Handler(void) {
    handleNavigation();
}



void initializeMenu(void) {
	printf("display_init\n\r");
    current_menu = MENU_MAIN;
    current_selection = 0;
    current_device = 0;
    last_selection = -1;

    for (int i = 0; i < 4; i++) {
        device_states[i] = 0;
        setDeviceState(i, 0);
    }
    displayMainMenu();
}
