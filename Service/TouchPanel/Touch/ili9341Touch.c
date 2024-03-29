/*
 * ili9341_touch.c
 *
 *  Created on: 29 mars 2023
 *      Author: PC-MAGH
 */


#include "Service/TouchPanel/Touch/ili9341Touch.h"

#include "HAL/GPIO/GpioPortB.h"
#include "HAL/SPI/HalSpi.h"
#include "Service/Delay/Delay.h"

#include <stdlib.h>

#define READ_X_12BITS 0xD3
#define READ_Y_12BITS 0x93
   
#define READ_X_8BITS 0xD8
#define READ_Y_8BITS 0x98
   
#define READ_X_8BITS_NPWRDWN 0xDB
#define READ_Y_8BITS_NPWRDWN 0x9B

#define START_CMD   0x80

#define MAXIMUM_ADMISIBLE_PIXDIST (5U)

#define PROMEDY  (5U)
   
static void ILI9341_TouchSelect();
static void ILI9341_TouchUnselect();
static uint16_t spiReadWrite12Bit(uint8_t cmd);

static void calibrationCoordiantes(TouchData_t* touchData);
static bool proveMaximalPixelDistance(uint16_t value1, uint16_t value2);

static uint32_t avg_x = 0;
static uint32_t avg_y = 0;

void ILI9341Touch_init(void)
{
    ILI9341_TouchSelect();
    (void)HalSpi_write(START_CMD);
    (void)HalSpi_write(0x00);
    (void)HalSpi_write(0x00);
    ILI9341_TouchUnselect();
}

bool ILI9341Touch_isPressed(void)
{
    return GpioPortB_readPin(TOUCH_IRQ_Pin);
}

bool ILI9341Touch_getCoordinates(TouchData_t* touchData)
{

    avg_x = 0;
    avg_y = 0;
    uint16_t Promx[PROMEDY] = 0;
    uint16_t Promy[PROMEDY] = 0;
    uint8_t nsamples = 0;

    ILI9341_TouchSelect();
    
    for(uint8_t i = 0; i < PROMEDY; i++) 
    {
        if(true == ILI9341Touch_isPressed())
            break;

        (void)HalSpi_write(READ_X_8BITS);
        Promx[i] = HalSpi_write(0x00);
        HalSpi_write(0x00);
        HalSpi_write(READ_Y_8BITS);
        Promy[i] = HalSpi_write(0x00);
        HalSpi_write(0x00);
        
        if(i>=1)
        {
            if ((proveMaximalPixelDistance(Promx[i],Promx[i-1]) == true) || 
                (proveMaximalPixelDistance(Promy[i],Promy[i-1]) == true))
            {
                break;
            }
        }
        
        avg_x += Promx[i];
        avg_y += Promy[i];
        
        nsamples++;
    }
    
    ILI9341_TouchUnselect();
    
    if(nsamples < PROMEDY)
        return false;
    
    avg_x = avg_x/PROMEDY;
    avg_y = avg_y/PROMEDY;
    
    
    
    touchData->x = avg_x;
    touchData->y = avg_y;
    
    calibrationCoordiantes(touchData);
    
    return true;
}

/******************************************************/
/* Static functions */
/******************************************************/
static void ILI9341_TouchSelect() 
{
    HalSpi_changeFrequency(TOUCH_FREQ);
    Delay_ms(1);
    HalPortB_deactivatePin(TOUCH_CS_Pin);
}
   
static void ILI9341_TouchUnselect() 
{
    HalPortB_activatePin(TOUCH_CS_Pin);
}

static uint16_t spiReadWrite12Bit(uint8_t cmd)
{
    uint8_t coordinate[2];
    uint16_t retVal;

    HalSpi_write(cmd);
    coordinate[0] = HalSpi_write(0x00);
    coordinate[1] = HalSpi_write(0x00);
    
    retVal = coordinate[0] + (coordinate[1]<<4);
    return retVal;
}

static void calibrationCoordiantes(TouchData_t* touchData)
{
    int32_t xCal = 0;
    int32_t yCal = 0;
        
    xCal = (int32_t)(touchData->x)*-200;
    yCal = (int32_t)(touchData->y)*-280;
    
    xCal /= 93;
    yCal /= 98;
        
    xCal += 254;
    yCal += 351;
    
    touchData->x = (uint16_t)xCal;
    touchData->y = (uint16_t)yCal;
}

static bool proveMaximalPixelDistance(uint16_t value1, uint16_t value2)
{
    bool retVal = false;
    
    if (((abs(value1-value2))>MAXIMUM_ADMISIBLE_PIXDIST))
    {
        retVal = true;;
    }
    return retVal;
}
