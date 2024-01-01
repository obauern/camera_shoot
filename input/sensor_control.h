#ifndef __SENSOR_CONTROL_H__
#define __SENSOR_CONTROL_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tm4c_cmsis.h"
#include "TM4C123GH6PM.h"
#include "HC_SR04.h"

typedef enum
{
  MODE_FOCUS_AND_SHOOT,
  MODE_CONTINUOUSFOCUS_AND_SHOOT
} shootMode_t;

typedef struct
{
    bool isInputTrigered;
    bool isFirstPictureToBeTaken;
    shootMode_t shootMode;
}sensorParameters_t;

static sensorParameters_t sensorParameters;

void initSensorParameters(void);
sensorParameters_t* returnSensorParameters(void);
void sensorControl(bool isPictureTaked);

#endif /*__SENSOR_CONTROL_H__*/