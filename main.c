#include <stdint.h>
#include <stdbool.h>
#include "tm4c_cmsis.h"
#include "Service/Init/init.h"
#include "Application/Camera/output/CameraControl.h"
#include "Application/Camera/input/SensorControl.h"

int main()
{
    initialise_values();
    
    while(true)
    {
        SensorControl_Control(CameraControl_IsPictureTaken());
        CameraControl_Control(returnSensorParameters());
    }
}