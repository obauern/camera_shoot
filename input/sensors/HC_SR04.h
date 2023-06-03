#include "TM4C123GH6PM.h"
#include <stdint.h>
#include <stdbool.h>
#include "tm4c_cmsis.h"

void initGpioForHCSr04(void);
void initTimersForHCSr04(void);
void HcSr04Control(bool isPictureTaked);
bool isAnObjectDetected(void);