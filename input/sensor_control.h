#ifndef __SENSOR_CONTROL_H__
#define __SENSOR_CONTROL_H__

#include <stdint.h>
#include <stdbool.h>
#include "tm4c_cmsis.h"
#include "TM4C123GH6PM.h"
#include "HC_SR04.h"

bool isInputTriggered(void);
void sensorControl(void);

#endif /*__SENSOR_CONTROL_H__*/