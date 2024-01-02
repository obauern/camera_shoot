#include <stdint.h>
#include <stdbool.h>
#include "tm4c_cmsis.h"
#include "init.h"
#include "camera_control.h"
#include "sensor_control.h"

int main()
{
    initialise_values();
    
    while(true)
    {
        sensorControl(isPictureTaken());
        camera_control(returnSensorParameters());
    }
}