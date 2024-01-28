#ifndef __GPIO_PORT_B__
#define __GPIO_PORT_B__

#include <stdbool.h>
#include <stdint.h>

void GpioPortB_init(void);
void HalPortB_activatePin(uint8_t pin);
void HalPortB_deactivatePin(uint8_t pin);
bool GpioPortB_readPin(uint8_t pin);

#endif  /*__GPIO_PORT_B__*/