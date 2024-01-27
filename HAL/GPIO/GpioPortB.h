#ifndef __GPIO_PORT_B__
#define __GPIO_PORT_B__

#include <stdbool.h>
#include <stdint.h>

void GpioPortB_init(void);
bool GpioPortB_readPin(uint8_t pin);

#endif  /*__GPIO_PORT_B__*/