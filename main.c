#include <stdint.h>
#include <stdbool.h>
#include "tm4c_cmsis.h"

#include "Application/Camera/output/CameraControl.h"
#include "Application/Camera/input/SensorControl.h"
#include "Application/Visual/Pages/HandlePages.h"
#include "Service/Timebase/Timebase.h"
#include "Service/TouchPanel/Panel/ili9341.h"
#include "Service/TouchPanel/Touch/ili9341Touch.h"
#include "Service/Init/init.h"

#define SHIFT_CALIB (20U)

int main()
{
    initialise_registers();
    Timebase_init();
    ILI9341Touch_init();
    ILI9341_Init();
    CameraControl_init();
    HandlePages_init(); 
   
    
    while(true)
    {
        HandlePages_execute();
        SensorControl_Control();
        CameraControl_Control();
    }
}