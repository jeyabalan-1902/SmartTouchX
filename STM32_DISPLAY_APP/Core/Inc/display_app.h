/*
 * display_app.h
 *
 *  Created on: Jun 23, 2025
 *      Author: kjeyabalan
 */

#ifndef INC_DISPLAY_APP_H_
#define INC_DISPLAY_APP_H_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "main.h"
#include "user_app.h"
#include "ST7735.h"
#include "fonts.h"
#include "GFX_FUNCTIONS.h"

#define DISPLAY_WIDTH     160
#define DISPLAY_HEIGHT    128
#define MAX_DISPLAY_LINES 8
#define LINE_HEIGHT       12
#define CHAR_WIDTH        7

#define BL_DEBUG_MSG_EN

extern uint8_t current_line;
extern char display_buffer[MAX_DISPLAY_LINES][23];

void DisplayMessage(const char* message);
void ScrollDisplay(void);
void ClearDisplay(void);
void InitializeDisplay(void);
void print_To_display(char *format,...);

#endif /* INC_DISPLAY_APP_H_ */
