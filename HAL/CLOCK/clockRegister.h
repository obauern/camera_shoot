/*
 * ili9341_touch.h
 *
 *  Created on: 29 mars 2023
 *      Author: PC-MAGH
 */

#ifndef CLOCK_REGISTER_H_
#define CLOCK_REGISTER_H_
   
#include <stdint.h>

void ClockRegister_init(void);
uint32_t ClockRegister_getSystemFrequency(void);

#endif /* CLOCK_REGISTER_H_ */
