#ifndef __GPIO_PORT_F__
#define __GPIO_PORT_F__

#include <stdint.h>

void GpioPortF_init(void);
void GpioPortF_activatePin(uint8_t pin);
void GpioPortF_deactivatePin(uint8_t pin);

#endif  /*__GPIO_PORT_F__*/