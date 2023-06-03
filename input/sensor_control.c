#include "sensor_control.h"
#include "HC_SR04.h"

#define SW_1 (1U << 4U)
#define SW_0 (1U << 0U)

bool isInputTriggered(void)
{
    /*return GPIO_PORTF_AHB_DATA_BITS_R[SW_1] == 0x00;*/
    return isAnObjectDetected();
}

void sensorControl(void)
{
#if HC_SR04
    HcSr04Control();
#endif
}