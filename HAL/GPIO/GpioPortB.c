#include "GpioPortB.h"

#include "TM4C123GH6PM.h"
#include "tm4c_cmsis.h"

#define PB0_OUTPUT (1U << 0U)
#define PB1_INPUT  (~(1U << 1U))

#define PB2_INPUT (~(1U << 2U))
#define PB3_OUTPUT (1U << 3U)

#define PB2_DEN   (1U << 2U)
#define PB3_DEN   (1U << 3U)

#define PB2_AFSEL (1U << 2U)

#define PB2_T3CCP0 (7U << 8U)

#define PB2_GPIO_PIN  (1U << 2U)

#define CLOCK_PORTB_AVAILABLE (1U << 1U)

static void pb2T3CCP0Pb3Trigger(void);
static void initChipSelectAndTouchIrq(void);

void GpioPortB_init(void)
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;   /*gpio clock gate for port B*/
    //while (!(SYSCTL_RCGCGPIO_R & CLOCK_PORTB_AVAILABLE));
    SYSCTL_GPIOHBCTL_R |= SYSCTL_GPIOHBCTL_PORTB; /*activates the ahb*/
    
    pb2T3CCP0Pb3Trigger();
    initChipSelectAndTouchIrq();
}

void HalPortB_activatePin(uint8_t pin)
{
    GPIOB_HS->DATA_Bits[pin] = 0xFFFFFFFF;
}

void HalPortB_deactivatePin(uint8_t pin)
{
    GPIOB_HS->DATA_Bits[pin] = 0;
}

bool GpioPortB_readPin(uint8_t pin)
{
    bool retVal = false;
    
    if ((GPIOB_HS->DATA_Bits[pin] & pin) == 0U)
    {
      retVal = false;
    }
    else
    {
      retVal = true;
    }
    return retVal;
}

/*-----------INTERNAL FUNCTIONS-------------*/

static void pb2T3CCP0Pb3Trigger(void)
{
    GPIOB_HS->DIR &= PB2_INPUT; 
    GPIOB_HS->DIR |= PB3_OUTPUT;
    
    GPIOB_HS->DEN |= PB2_DEN | PB3_DEN;  
    
    GPIOB_HS->AFSEL |= PB2_AFSEL; /*Special function in PB2*/
    
    GPIOB_HS->PCTL &= ~0x00000F00;
    GPIOB_HS->PCTL |= PB2_T3CCP0;
}

static void initChipSelectAndTouchIrq(void)
{
      SYSCTL_GPIOHBCTL_R |= SYSCTL_GPIOHBCTL_PORTB;
    // Configura pines para SSI0 (Puedes ajustar según tu necesidad)

    GPIOB_HS->DIR |= PB0_OUTPUT;
    GPIOB_HS->DIR &= PB1_INPUT;  // PB0(output): Chip Select (Touch) PB1(input): Touch Irq


    GPIOB_HS->DEN |= (TOUCH_CS_Pin | TOUCH_IRQ_Pin); /* PB0 y PB1 digital pins*/
    

    GPIOB_HS->PUR |= (TOUCH_CS_Pin | TOUCH_IRQ_Pin); /* PULL-UP both pins */
}


