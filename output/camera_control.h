#ifndef __CAMERA_CONTROL_H__
#define __CAMERA_CONTROL_H__

#include <stdint.h>
#include <stdbool.h>
#include "tm4c_cmsis.h"
#include "TM4C123GH6PM.h"


void camera_control(bool isInputTriggered);
bool isPictureTaked(void);

#endif /*__CAMERA_CONTROL_H__*/