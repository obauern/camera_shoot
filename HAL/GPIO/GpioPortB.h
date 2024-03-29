#ifndef __GPIO_PORT_B__
#define __GPIO_PORT_B__

#include <stdbool.h>
#include <stdint.h>

#define TOUCH_CS_Pin (1U << 0U)
#define TOUCH_IRQ_Pin (1U << 1U)

void GpioPortB_init(void);
void HalPortB_activatePin(uint8_t pin);
void HalPortB_deactivatePin(uint8_t pin);
bool GpioPortB_readPin(uint8_t pin);

#endif  /*__GPIO_PORT_B__*/