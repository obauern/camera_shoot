#include "sensor_control.h"
#include "HC_SR04.h"

#define SW_1 (1U << 4U)
#define SW_0 (1U << 0U)

bool isInputTriggered(void)
{
    return isAnObjectDetected();
}

void sensorControl(bool isPictureTaked)
{
#if HC_SR04
    HcSr04Control(isPictureTaked);
#endif
}