/*
 * onwords_logo.c
 *
 *  Created on: Jul 9, 2025
 *      Author: kjeyabalan
 */

#include <onwords_logo.h>

void displayCenteredOnwardsLogo(void) {
    // Set landscape orientation
    ST7735_SetRotation(1);

    // Clear screen with black background
    fillScreen(LOGO_BACKGROUND);

    // Calculate text dimensions
    // "on" = 2 characters × 16 pixels = 32 pixels wide
    int on_width = 2 * CHAR_WIDTH_16x26;

    // "wards" = 5 characters × 16 pixels = 80 pixels wide
    int wards_width = 5 * CHAR_WIDTH_16x26;

    // Total text width = "onwards" = 7 characters × 16 pixels = 112 pixels
    int total_width = 7 * CHAR_WIDTH_16x26;

    // Calculate center positions
    int center_x = DISPLAY_WIDTH_LANDSCAPE / 2;
    int center_y = DISPLAY_HEIGHT_LANDSCAPE / 2;

    // Calculate starting position to center the entire word "onwards"
    int start_x = center_x - (total_width / 2);
    int start_y = center_y - (CHAR_HEIGHT_16x26 / 2);

    // Draw "on" in red
    ST7735_WriteString(start_x, start_y, "on", Font_16x26, LOGO_ON_COLOR, LOGO_BACKGROUND);

    // Draw "wards" in white (continuing after "on")
    ST7735_WriteString(start_x + on_width, start_y, "words", Font_16x26, LOGO_WARDS_COLOR, LOGO_BACKGROUND);
}

// Alternative: Perfect pixel-level centering
void displayPerfectCenteredLogo(void) {
    ST7735_SetRotation(1);
    fillScreen(BLACK);

    // Manual calculation for perfect centering
    // Display center: 160/2 = 80 (x), 128/2 = 64 (y)
    // Text "onwards" = 7 chars × 16px = 112px wide
    // Text height = 26px

    int text_width = 112;
    int text_height = 26;

    int start_x = (160 - text_width) / 2;  // (160 - 112) / 2 = 24
    int start_y = (128 - text_height) / 2; // (128 - 26) / 2 = 51

    // Draw "on" (2 chars × 16px = 32px wide)
    ST7735_WriteString(start_x, start_y, "on", Font_16x26, RED, BLACK);

    // Draw "wards" (starts at start_x + 32)
    ST7735_WriteString(start_x + 32, start_y, "words", Font_16x26, WHITE, BLACK);
}

// Version with debug positioning info
void displayLogoWithDebug(void) {
    ST7735_SetRotation(1);
    fillScreen(BLACK);

    // Calculate positions
    int total_chars = 7;
    int char_width = 16;
    int char_height = 26;
    int total_width = total_chars * char_width;

    int center_x = 160 / 2;
    int center_y = 128 / 2;
    int start_x = center_x - (total_width / 2);
    int start_y = center_y - (char_height / 2);

    // Draw debug info (small font)
    char debug_info[50];
    sprintf(debug_info, "X:%d Y:%d", start_x, start_y);
    ST7735_WriteString(5, 5, debug_info, Font_7x10, YELLOW, BLACK);

    sprintf(debug_info, "W:%d H:%d", total_width, char_height);
    ST7735_WriteString(5, 15, debug_info, Font_7x10, YELLOW, BLACK);

    // Draw center crosshairs for reference
    drawLine(center_x - 10, center_y, center_x + 10, center_y, GRAY);
    drawLine(center_x, center_y - 10, center_x, center_y + 10, GRAY);

    // Draw the logo
    ST7735_WriteString(start_x, start_y, "on", Font_16x26, RED, BLACK);
    ST7735_WriteString(start_x + 32, start_y, "words", Font_16x26, WHITE, BLACK);
}

// Optimized version with exact positioning
void displayOnwardsLogoOptimized(void) {
    ST7735_SetRotation(1);
    fillScreen(BLACK);

    // Exact calculations for 160×128 display
    // Font_16x26: each character is 16px wide, 26px tall
    // "onwards" = 7 characters = 112px total width

    // Perfect center positioning
    int x_pos = 24;  // (160 - 112) / 2 = 24
    int y_pos = 51;  // (128 - 26) / 2 = 51

    // Draw "on" in red (positions 0-1, 32px wide)
    ST7735_WriteString(x_pos, y_pos, "on", Font_16x26, RED, BLACK);

    // Draw "wards" in white (positions 2-6, 80px wide)
    ST7735_WriteString(x_pos + 32, y_pos, "words", Font_16x26, WHITE, BLACK);
}

// Function with startup animation
void displayLogoWithAnimation(void) {
    ST7735_SetRotation(1);
    fillScreen(BLACK);

    int x_pos = 24;
    int y_pos = 51;

    // Animate "on" appearing first
    ST7735_WriteString(x_pos, y_pos, "on", Font_16x26, RED, BLACK);
    HAL_Delay(500);

    // Then animate "wards" appearing
    ST7735_WriteString(x_pos + 32, y_pos, "words", Font_16x26, WHITE, BLACK);
}

// Simple test function
void testCenteredLogo(void) {
    printf("Displaying Logo...\n");
    displayOnwardsLogoOptimized();
    HAL_Delay(3000);
    printf("Logo display complete\n");
}

// Integration with your menu system
void showStartupLogoAndMenu(void) {
    // Show logo first
    displayOnwardsLogoOptimized();
    HAL_Delay(3000);

    // Transition to menu
    fillScreen(BLACK);
    HAL_Delay(200);
}
