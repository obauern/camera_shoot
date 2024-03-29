#ifndef __CAMERA_CONTROL_H__
#define __CAMERA_CONTROL_H__

#include <stdint.h>
#include <stdbool.h>
#include "tm4c_cmsis.h"
#include "TM4C123GH6PM.h"
#include "Application/Camera/input/SensorControl.h"

typedef enum CameraControlMode_TAG
{
    CAMERA_MODE_SENSOR,
    CAMERA_MODE_MANUAL,
    CAMERA_MODE_DEACTIVATED
}CameraControlMode_t;

void CameraControl_init(void);
void CameraControl_triggerManualPicture(void);
void CameraControl_Control(void);
bool CameraControl_isPictureTaken(void);
void CameraControl_setMode(CameraControlMode_t);

#endif /*__CAMERA_CONTROL_H__*/