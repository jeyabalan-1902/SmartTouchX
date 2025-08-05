/*
 * onwords_logo.h
 *
 *  Created on: Jul 9, 2025
 *      Author: kjeyabalan
 */

#ifndef INC_DISPLAY_ONWORDS_LOGO_H_
#define INC_DISPLAY_ONWORDS_LOGO_H_

#include <APP/user_app.h>
#include <DISPLAY/GFX_FUNCTIONS.h>
#include <DISPLAY/ST7735.h>
#include <math.h>


#define DISPLAY_WIDTH_LANDSCAPE 160
#define DISPLAY_HEIGHT_LANDSCAPE 128

// Font_16x26 character dimensions
#define CHAR_WIDTH_16x26 16
#define CHAR_HEIGHT_16x26 26

// Colors for the logo
#define LOGO_ON_COLOR RED
#define LOGO_WARDS_COLOR WHITE
#define LOGO_BACKGROUND BLACK

void displayCenteredOnwardsLogo(void);
void displayOnwardsLogoOptimized(void);
void displayLogoWithAnimation(void);
void showStartupLogoAndMenu(void);

#endif /* INC_DISPLAY_ONWORDS_LOGO_H_ */
