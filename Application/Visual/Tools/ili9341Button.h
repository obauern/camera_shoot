/**
 *	Button library for ILI9341 LCD controller
 * 	and STMPE811 Resistive touch screen controller
 *
 *	@author 	Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@link		http://stm32f4-discovery.com/2014/05/11-button-library-for-ili9341-lcd-and-stmpe811-touch-controller-on-stm32f429-discovery-board/
 *	@version 	v1.0
 *	@ide		Keil uVision
 *	@license	GNU GPL v3
 *	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 *	
 *	By default, library supports up to 10 buttons.
 *	If you want to use more of them, change line below in your defines.h file:
 *	
 *	#define TM_ILI9341_BUTTON_MAX_BUTTONS	10
 *	
 */
#ifndef TM_ILI9341_BUTTON_H
#define TM_ILI9341_BUTTON_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "Service/TouchPanel/Tools/fonts.h"
#include "Service/TouchPanel/Touch/ili9341Touch.h"

// Maximum number of buttons available
#define TM_ILI9341_BUTTON_MAX_BUTTONS	10

//Button flags
#define TM_BUTTON_FLAG_ENABLED			0x0001	// Button is enabled - internal flag
#define TM_BUTTON_FLAG_NOBORDER			0x0002	// No border used in
#define TM_BUTTON_FLAG_IMAGE			0x0004	// Use image for background
#define TM_BUTTON_FLAG_NOLABEL			0x0008	// Do not display button label
#define TM_BUTTON_FLAG_USED				0x8000	// Button is used - internal flag


typedef enum
{
    ERROR,
    SUCCESS
}ErrorStatus_t;

typedef void (*callbackFunction_t)();
/**
 * Button options struct
 *
 * Parameters:
 *	- uint16_t x: X coordinate of top left button corner
 *	- uint16_t y: Y coordinate of top left button corner
 *	- uint16_t width: button width in pixels
 *	- uint16_t height: button height in pixels
 *	- uint16_t background: 16bit background color
 *	- uint16_t borderColor: 16bit border color
 *	- uint16_t flags: Button flags
 *	- char *label: pointer to first character of label
 *	- FontDef *font: pointer to font structure
 *	- uint16_t color: label color
 *	- uint16_t *image: pointer to location at first color for pixel
 */
typedef struct 
{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	uint16_t background;
        uint16_t pressedBackground;
	uint16_t borderColor;
	uint16_t flags;
	char* label;
	FontDef* font;
	uint16_t color;
	uint16_t* image;
        callbackFunction_t callbackFunction;
        bool wasButtonPressed;
} TM_ILI9341_Button_t;

int8_t ili9341Button_Add(TM_ILI9341_Button_t* button);
ErrorStatus_t ili9341Button_Draw(uint8_t id);
void ili9341Button_DrawButtonWithBackgroundPressed(uint8_t id);
void ili9341Button_DrawAll(void);

/**
 * Enable button
 *
 * Parameters:
 * 	- uint8_t id: button id
 */
void ili9341Button_Enable(uint8_t id);

/**
 * Disable button
 *
 * Parameters:
 * 	- uint8_t id: button id
 */
void ili9341Button_Disable(uint8_t id);

/**
 * Delete all buttons in library
 *
 *
 */
void ili9341Button_DeleteAll(void);

/**
 * Delete button
 *
 * Parameters:
 * 	- uint8_t id: button id
 */
void ili9341Button_Delete(uint8_t id);

/**
 * Get pressed button
 *
 * Parameters:
 * 	- TM_STMPE811_TouchData *TouchData: pointer to TM_STMPE811_TouchData structure
 *
 * Returns button id on success, otherwise -1
 */
int8_t ili9341Button_Touch(TouchData_t* touchData);

#endif

