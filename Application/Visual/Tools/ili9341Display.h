
#ifndef ILI9341_DISPLAY_H
#define ILI9341_DISPLAY_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "Service/TouchPanel/Tools/fonts.h"
#include "Service/TouchPanel/Touch/ili9341Touch.h"

// Maximum number of displays available
#define ILI9341_DISPLAY_MAX_DISPLAYS	10

typedef enum
{
    DISP_ERROR,
    DISP_SUCCESS
}DispErrorStatus_t;

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
	uint16_t borderColor;
	uint32_t* variable;
	FontDef* font;
	uint16_t color;
        bool used;
} ILI9341_Display_t;


int8_t ili9341Display_Add(ILI9341_Display_t* display);

DispErrorStatus_t ili9341Display_Draw(uint8_t id);

void ili9341Display_DrawAll(void);

void ili9341Display_DeleteAll(void);

void ili9341Display_Delete(uint8_t id);

/**
 * Get pressed button
 *
 * Parameters:
 * 	- TM_STMPE811_TouchData *TouchData: pointer to TM_STMPE811_TouchData structure
 *
 * Returns button id on success, otherwise -1
 */
int8_t ILI9341Display_update(void);

#endif

