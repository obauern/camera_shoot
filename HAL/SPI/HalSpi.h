/*
 * HalSpi.h
 *
 *  Created on: 29 mars 2023
 *      Author: PC-MAGH
 */

#ifndef HAL_SPI_H_
#define HAL_SPI_H_

#include <stdint.h>

typedef enum
{
    SCREEN_FREQ,
    TOUCH_FREQ
}componentFrequency_t;

void HalSpi_init(void);
void HalSpi_changeFrequency(componentFrequency_t componentFrequency);
void HalSpi_testFrequency(uint8_t freq);
uint8_t HalSpi_write(uint8_t data);
uint8_t HalSpi_read(void); 

#endif /* HAL_SPI_H_ */
