#include "GpioPortF.h"

#include "TM4C123GH6PM.h"
#include "tm4c_cmsis.h"

#define PF1_OUTPUT (1U << 1U)
#define PF2_OUTPUT (1U << 2U)
#define PF3_OUTPUT (1U << 3U)
#define PF4_INPUT  (~(1U << 4U))

#define NO_AFSEL (0x00)

#define PF4_PULLUP (1U << 4U)

#define PF1_DEN (1U << 1U)
#define PF2_DEN (1U << 2U)
#define PF3_DEN (1U << 3U)
#define PF4_DEN (1U << 4U)

void GpioPortF_init(void)
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;   /*gpio clock gate for port F*/
    
    SYSCTL_GPIOHBCTL_R |= SYSCTL_GPIOHBCTL_PORTF; /*activates the ahb*/
    
    GPIO_PORTF_AHB_DIR_R |= PF1_OUTPUT | PF2_OUTPUT | PF3_OUTPUT;
    GPIO_PORTF_AHB_DIR_R &= PF4_INPUT;
    
    GPIO_PORTF_AHB_AFSEL_R = NO_AFSEL;
    
    GPIO_PORTF_AHB_PUR_R |= PF4_PULLUP; /*pull up for the input*/
    
    GPIO_PORTF_AHB_DEN_R |= PF1_DEN | PF2_DEN | PF3_DEN | PF4_DEN;
}

void GpioPortF_activatePin(uint8_t pin)
{
    GPIOF_HS->DATA_Bits[pin] = 0xFFFFFFFF;
}

void GpioPortF_deactivatePin(uint8_t pin)
{
    GPIOF_HS->DATA_Bits[pin] = 0;
}
