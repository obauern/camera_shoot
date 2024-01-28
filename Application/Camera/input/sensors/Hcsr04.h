#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include <stdbool.h>

void HcSr04_Control(bool isPictureTaked);
bool HcSr04_isAnObjectDetected(void);
bool HcSr04_isFirstPictureToBeTaken(void);

#endif  /*__HC_SR04_H__*/