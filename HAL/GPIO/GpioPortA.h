/*
 * halPortA.h
 *
 *  Created on: 29 mars 2023
 *      Author: Alejandro Bauernfeind
 */

#ifndef HAL_PORT_A_H_
#define HAL_PORT_A_H_

#include <stdint.h>

void initPortA(void);
void GpioPortA_activatePin(uint8_t pin);
void GpioPortA_deactivatePin(uint8_t pin);

#endif /* HAL_PORT_A_H_ */
