#ifndef __CAMERA_CONTROL_H__
#define __CAMERA_CONTROL_H__

#include <stdint.h>
#include <stdbool.h>
#include "tm4c_cmsis.h"
#include "TM4C123GH6PM.h"
#include "Application/Camera/input/SensorControl.h"


void CameraControl_Control(sensorParameters_t* sensorParametersPtr);
bool CameraControl_IsPictureTaken(void);

#endif /*__CAMERA_CONTROL_H__*/