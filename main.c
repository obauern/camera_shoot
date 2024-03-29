#include <stdint.h>
#include <stdbool.h>
#include "tm4c_cmsis.h"

#include "Application/Camera/output/CameraControl.h"
#include "Application/Camera/input/SensorControl.h"
#include "Application/Visual/Pages/HandlePages.h"
#include "Service/TouchPanel/Panel/ili9341.h"
#include "Service/TouchPanel/Touch/ili9341Touch.h"
#include "Service/Init/init.h"

#define SHIFT_CALIB (20U)

int main()
{
    initialise_registers();
    ILI9341Touch_init();
    ILI9341_Init();
    CameraControl_init();
    HandlePages_init(); 
    
     //ILI9341_DrawImage((ILI9341_WIDTH - 240)/2,(ILI9341_HEIGHT-240)/2,240,240,
    //  (const uint16_t*)test_img_240x240);
    ILI9341_FillScreen(ILI9341_WHITE);
    /*4 puntos para calibracion*/
    ILI9341_DrawPixel(SHIFT_CALIB, SHIFT_CALIB, ILI9341_RED);
    ILI9341_DrawPixel(ILI9341_WIDTH - SHIFT_CALIB, SHIFT_CALIB, ILI9341_BLUE);
    ILI9341_DrawPixel(SHIFT_CALIB, ILI9341_HEIGHT - SHIFT_CALIB, ILI9341_GREEN);
    ILI9341_DrawPixel(ILI9341_WIDTH - SHIFT_CALIB, ILI9341_HEIGHT - SHIFT_CALIB, ILI9341_BLACK);
    
    
    while(true)
    {
        HandlePages_execute();
        SensorControl_Control();
        CameraControl_Control();
    }
}