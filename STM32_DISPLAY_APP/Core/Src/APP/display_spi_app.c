
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
#include "menu_UI.h"

int current_menu = MENU_MAIN;
int current_selection = 0;
int current_device = 0;
int last_selection = -1;
int last_menu = -1;

int downbutton = 0, upbutton = 0, enter = 0;

// 0=OFF, 1=ON
int device_states[4] = {0, 0, 0, 0};
int last_device_states[4] = {-1, -1, -1, -1};

bool menu_drawn = false;
bool buttons_drawn = false;



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
        case MENU_SET_SCHEDULE_A:
            alarm_A_setMenu();
            break;
        case MENU_SET_SCHEDULE_B:
        	alarm_B_setMenu();
        	break;
    }
    handleNavigation();
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

    // Initialize clock temp variables
    temp_hour = 0;
    temp_minute = 0;
    temp_second = 0;
    temp_date = 1;
    temp_month = 1;
    temp_year = 25;
    temp_day = 1;

    for (int i = 0; i < 4; i++) {
        device_states[i] = 0;
        last_device_states[i] = -1;
        setDeviceState(i, 0);
    }
    displayMainMenu();
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

}


void handleNavigation(void) {
    int max_options;
    char src_name[] = "DISPLAY";

    // Handle increment/decrement mode first
    if (increment_mode) {
        handleIncrementDecrement();

        // Handle enter button in increment mode (exit increment mode)
        if (enter) {
            HAL_Delay(200);
            enter = 0;
            increment_mode = false;
            selected_field = -1;
            buttons_drawn = false;
            last_selection = -1;
        }
        return; // Don't process normal navigation in increment mode
    }

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
        case MENU_SET_SCHEDULE_A:
        	max_options = SET_SCHEDULE_A;
        	break;
        case MENU_SET_SCHEDULE_B:
        	max_options = SET_SCHEDULE_B;
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
        updateButtonSelection(old_selection, current_selection);
        last_selection = current_selection;
    }

    if (downbutton) {
        HAL_Delay(200);
        int old_selection = current_selection;
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
                if (current_selection >= 0 && current_selection <= 2) {
                    // Enter increment/decrement mode for time fields
                    increment_mode = true;
                    selected_field = current_selection;
                    buttons_drawn = false;
                    last_selection = -1;
                } else if (current_selection == 3) {
                	current_selection = 0;
					menu_drawn = false;
                	if (current_alarm == 1) {
						rtc_request_set_alarm_a(temp_hour, temp_minute, temp_second);
						current_menu = MENU_SET_SCHEDULE_A;
						alarm_A_setMenu();
					} else if (current_alarm == 2) {
						rtc_request_set_alarm_b(temp_hour, temp_minute, temp_second);
						current_menu = MENU_SET_SCHEDULE_B;
						alarm_B_setMenu();
					} else {
						rtc_request_set_time(temp_hour, temp_minute, temp_second);
						current_menu = MENU_CLOCK_SETTINGS;
						displayClockSettingsMenu();
					}
                } else if (current_selection == 4) {
                    current_selection = 0;
                    menu_drawn = false;
                    if (current_alarm == 1) {
						current_menu = MENU_SET_SCHEDULE_A;
						alarm_A_setMenu();
					} else if (current_alarm == 2) {
						current_menu = MENU_SET_SCHEDULE_B;
						alarm_B_setMenu();
					} else {
						current_menu = MENU_CLOCK_SETTINGS;;
						displayClockSettingsMenu();
					}
                }
                break;

            case MENU_SET_DATE:
                if (current_selection >= 0 && current_selection <= 3) {
                    // Enter increment/decrement mode for date fields
                    increment_mode = true;
                    selected_field = current_selection;
                    buttons_drawn = false;
                    last_selection = -1;
                } else if (current_selection == 4) {
                	rtc_request_set_date(temp_year, temp_month, temp_date, temp_day);
                    current_menu = MENU_CLOCK_SETTINGS;
                    current_selection = 0;
                    menu_drawn = false;
                    displayClockSettingsMenu();
                } else if (current_selection == 5) {
                    current_menu = MENU_CLOCK_SETTINGS;
                    current_selection = 0;
                    menu_drawn = false;
                    displayClockSettingsMenu();
                }
                break;

            case MENU_SET_ALARM:
                if (current_selection == 0) {
                    current_menu = MENU_SET_SCHEDULE_A;
                    current_selection = 0;
                    menu_drawn = false;
                    alarm_A_setMenu();
                } else if (current_selection == 1) {
                    current_menu = MENU_SET_SCHEDULE_B;
                    current_selection = 0;
                    menu_drawn = false;
                    alarm_B_setMenu();
                } else if (current_selection == 2) {
                    current_menu = MENU_CLOCK_SETTINGS;
                    current_selection = 0;
                    menu_drawn = false;
                    displayClockSettingsMenu();
                } else if (current_selection == 3) {
                    current_menu = MENU_MAIN;
                    current_selection = 0;
                    menu_drawn = false;
                    displayOnwardsLogoOptimized();
                    HAL_Delay(600);
                    displayMainMenu();
                }
                break;

            case MENU_SET_SCHEDULE_A:
            	if(current_selection == 0){
            		current_menu = MENU_SET_TIME;
            		current_selection = 0;
            		menu_drawn = false;
            		current_alarm = 1;
            		temp_hour = 0; temp_minute = 0; temp_second = 0;
            		displaySetTimeMenu();
            	} else if(current_selection == 1){
            		current_menu = MENU_SELECT_DEVICES;
            		current_selection = 0;
            		menu_drawn = false;
            	} else if(current_selection == 2){
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

            case MENU_SET_SCHEDULE_B:
				if(current_selection == 0){
					current_menu = MENU_SET_TIME;
					current_selection = 0;
					menu_drawn = false;
					current_alarm = 2;
					temp_hour = 0; temp_minute = 0; temp_second = 0;
					displaySetTimeMenu();
				} else if(current_selection == 1){
					current_menu = MENU_SELECT_DEVICES;
					current_selection = 0;
					menu_drawn = false;
				} else if(current_selection == 2){
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
                } else if (current_selection == 3) {
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




