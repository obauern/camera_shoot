#include <stdint.h>
#include <stdbool.h>
#include "tm4c_cmsis.h"

#include "Application/Camera/output/CameraControl.h"
#include "Application/Camera/input/SensorControl.h"
#include "Service/TouchPanel/Panel/ili9341.h"
#include "Service/TouchPanel/Touch/ili9341Touch.h"
#include "Service/Init/init.h"

int main()
{
    initialise_registers();
    ILI9341Touch_init();
    ILI9341_Init();
    CameraControl_init();
    
    while(true)
    {
        SensorControl_Control();
        CameraControl_Control();
    }
}