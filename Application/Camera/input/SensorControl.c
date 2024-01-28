#include "SensorControl.h"
#include "Application/Camera/input/Sensors/Hcsr04.h"

#define SW_1 (1U << 4U)
#define SW_0 (1U << 0U)

void initSensorParameters(void)
{
    sensorParameters.isFirstPictureToBeTaken = false;
    sensorParameters.isInputTrigered = false;
}

sensorParameters_t* returnSensorParameters(void)
{
    sensorParameters.isInputTrigered = isAnObjectDetected();
    sensorParameters.isFirstPictureToBeTaken = isFirstPictureToBeTaken();
    sensorParameters.shootMode = MODE_CONTINUOUSFOCUS_AND_SHOOT;
    return &sensorParameters;
}

void sensorControl(bool isPictureTaked)
{
#if HC_SR04
    HcSr04Control(isPictureTaked);
#endif
}