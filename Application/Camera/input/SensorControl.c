#include "SensorControl.h"
#include "Application/Camera/input/Sensors/Hcsr04.h"

#define SW_1 (1U << 4U)
#define SW_0 (1U << 0U)

void SensorControl_Init(void)
{
    sensorParameters.isFirstPictureToBeTaken = false;
    sensorParameters.isInputTrigered = false;
}

sensorParameters_t* returnSensorParameters(void)
{
    sensorParameters.isInputTrigered = HcSr04_isAnObjectDetected();
    sensorParameters.isFirstPictureToBeTaken = HcSr04_isFirstPictureToBeTaken();
    sensorParameters.shootMode = MODE_CONTINUOUSFOCUS_AND_SHOOT;
    return &sensorParameters;
}

void SensorControl_Control(bool isPictureTaked)
{
#if HC_SR04
    HcSr04_Control(isPictureTaked);
#endif
}