/*
 * display_app.c
 *
 *  Created on: Jun 23, 2025
 *      Author: kjeyabalan
 */

#include "display_app.h"

uint8_t current_line = 0;
char display_buffer[MAX_DISPLAY_LINES][23];

void InitializeDisplay(void)
{
	printf("Application started\n");
	printf("Touch application running\n");
    ST7735_Init(0);
    fillScreen(BLACK);
    ST7735_SetRotation(1);
    // Clear display buffer
    for(int i = 0; i < MAX_DISPLAY_LINES; i++) {
        memset(display_buffer[i], 0, 23);
    }
    current_line = 0;

    // Display header
    ST7735_WriteString(5, 5, "-ONWORDS TOUCH BOARD-", Font_7x10, YELLOW, BLACK);
    ST7735_WriteString(5, 18, "    VERSION v3.0", Font_7x10, RED, BLACK);
    printf("Touch application running\n");

    HAL_Delay(1000);

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
