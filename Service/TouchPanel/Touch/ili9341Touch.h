/*
 * ili9341_touch.h
 *
 *  Created on: 29 mars 2023
 *      Author: PC-MAGH
 */

#ifndef ILI9341_TOUCH_H_
#define ILI9341_TOUCH_H_


#include <stdbool.h>
#include <stdint.h>

/*** Redefine if necessary ***/

// Warning! Use SPI bus with < 1.3 Mbit speed, better ~650 Kbit to be save.

// change depending on screen orientation
#define ILI9341_TOUCH_SCALE_X 240
#define ILI9341_TOUCH_SCALE_Y 320

// to calibrate uncomment UART_Printf line in ili9341_touch.c
#define ILI9341_TOUCH_MIN_RAW_X 1500
#define ILI9341_TOUCH_MAX_RAW_X 31000
#define ILI9341_TOUCH_MIN_RAW_Y 3276
#define ILI9341_TOUCH_MAX_RAW_Y 30110

typedef struct
{
    uint16_t x;
    uint16_t y;
}TouchData_t;

// call before initializing any SPI devices
void ILI9341Touch_init(void);   

bool ILI9341Touch_isPressed(void);
bool ILI9341Touch_getCoordinates(TouchData_t* touchData);

#endif /* ILI9341_TOUCH_H_ */
