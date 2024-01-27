#include "GpioPortB.h"

#include "TM4C123GH6PM.h"
#include "tm4c_cmsis.h"

#define PB2_INPUT (~(1U << 2U))
#define PB3_OUTPUT (1U << 3U)

#define PB2_DEN   (1U << 2U)
#define PB3_DEN   (1U << 2U)

#define PB2_AFSEL (1U << 2U)

#define PB2_T3CCP0 (7U << 8U)

static void pb2T3CCP0Pb3Trigger(void);

void GpioPortB_init(void)
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;   /*gpio clock gate for port B*/
    SYSCTL_GPIOHBCTL_R |= SYSCTL_GPIOHBCTL_PORTB; /*activates the ahb*/
    
    pb2T3CCP0Pb3Trigger();
}

static void pb2T3CCP0Pb3Trigger(void)
{
    GPIOB_HS->DIR &= PB2_INPUT; 
    GPIOB_HS->DIR |= PB3_OUTPUT;
    
    GPIOB_HS->DEN |= PB2_DEN | PB3_DEN;  
    
    GPIOB_HS->AFSEL |= PB2_AFSEL; /*Special function in PB2*/
    
    GPIOB_HS->PCTL &= ~0x00000F00;
    GPIOB_HS->PCTL |= PB2_T3CCP0;
}
