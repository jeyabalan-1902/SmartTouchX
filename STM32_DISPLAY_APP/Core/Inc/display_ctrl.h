/*
 * display_ctrl.h
 *
 *  Created on: Jul 8, 2025
 *      Author: kjeyabalan
 */

#ifndef INC_DISPLAY_CTRL_H_
#define INC_DISPLAY_CTRL_H_

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "user_app.h"
#include "stm32f4xx_hal.h"
#include "ST7735.h"
#include "fonts.h"
#include "GFX_FUNCTIONS.h"


// Display dimensions for 1.8" ST7735
#define DISPLAY_WIDTH 160
#define DISPLAY_HEIGHT 128
#define BUTTON_WIDTH 140
#define BUTTON_HEIGHT 18
#define BUTTON_SPACING 22
#define MARGIN_X 10
#define TITLE_HEIGHT 15

// Menu state enumeration
typedef enum {
    MENU_MAIN = 0,
    MENU_TOTAL_CONTROL,
    MENU_SEPARATE_CONTROL,
    MENU_DEVICE_CONTROL
} menu_state_t;


// Menu configuration
#define MAIN_MENU_OPTIONS 2
#define TOTAL_CONTROL_OPTIONS 3
#define SEPARATE_CONTROL_OPTIONS 5
#define DEVICE_CONTROL_OPTIONS 3

// External variables
extern int current_menu;
extern int current_selection;
extern int current_device;
extern int last_selection;
extern int downbutton, upbutton, enter;
extern int device_states[4];
extern volatile int global_device_states[4];

// Function prototypes

// Core menu functions
void Menu_Handler(void);
void initializeMenu(void);
void handleNavigation(void);

// Menu display functions
void displayMainMenu(void);
void displayTotalControlMenu(void);
void displaySeparateControlMenu(void);
void displayDeviceControlMenu(void);

// Button drawing functions
void drawButton(int x, int y, int width, int height, char* text, int selected);

// Device control functions
void setDeviceState(int device, int state);
void setAllDevicesState(int state);
void syncDisplayDeviceStates(void);
// GPIO interrupt callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

// Legacy functions (for backward compatibility if needed)
void Test_Button_unselect(void);
void Test_Button_select(void);
void Page2_Button_unselect(void);
void Page2_Button_select(void);
void back_Button_unselect(void);
void back_Button_select(void);
void ledon_Button_unselect(void);
void ledon_Button_select(void);
void ledoff_Button_unselect(void);
void ledoff_Button_select(void);
void HomeMenu(void);
void TestMenu(void);
void Page2Menu(void);


#endif /* INC_DISPLAY_CTRL_H_ */
