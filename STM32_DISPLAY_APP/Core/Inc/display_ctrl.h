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
#define DEVICE_CONTROL_OPTIONS 4


#define MAX_DISPLAY_LINES 8
#define LINE_HEIGHT       12
#define CHAR_WIDTH        7

#define BL_DEBUG_MSG_EN

extern uint8_t current_line;
extern char display_buffer[MAX_DISPLAY_LINES][23];
extern int current_menu;
extern int current_selection;
extern int current_device;
extern int last_selection;
extern int downbutton, upbutton, enter;
extern int device_states[4];


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

void drawSingleButton(int x, int y, int width, int height, char* text, int selected, int button_id);
void updateButtonSelection(int old_selection, int new_selection);
void drawTitleBar(char* title);

// Optimized update functions
void updateStatusInfo(char* status, uint16_t color);
void updateDeviceStatusText(int device_index, bool is_on);
void updateDeviceCount(int total_on);
void updateDeviceControlStatus(int device, bool is_on);

// Fast update functions
void fastUpdateDeviceList(void);
void fastUpdateTotalControl(void);
void fastUpdateDeviceControl(void);

// Device control functions
void setDeviceState(int device, int state);
void setAllDevicesState(int state);
void syncDisplayDeviceStates(void);
void Display_Handler(void *param);
void updateToDisplayMenu(void);
// GPIO interrupt callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);



void print_To_display(char *format,...);
void DisplayMessage(const char* message);
void ScrollDisplay(void);
void ClearDisplay(void);

#endif /* INC_DISPLAY_CTRL_H_ */
