#include "init.h"

#define TIMER_0 (1U << 0)

inline static void initialiseAdc(void);
inline static void initialiseTimer0Interrupt(void);
inline static void initialiseGpio(void);

void initialise_values(void)
{
    initialiseGpio();
    initialiseTimer0Interrupt();
    initialiseAdc();
    initGpioForHCSr04();
    initTimersForHCSr04();
}

/*----------------static functions------------------------*/

inline static void initialiseGpio(void)
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;   /*gpio clock gate for port F*/
    
    SYSCTL_GPIOHBCTL_R |= SYSCTL_GPIOHBCTL_PORTF; /*activates the ahb*/
    
    GPIO_PORTF_AHB_DIR_R = 0x0EU; /*direction output for led*/
    
    GPIO_PORTF_AHB_AFSEL_R |= 0x00U;
    
    GPIO_PORTF_AHB_PUR_R |= 0x10U; /*pull up for the input*/
      
    GPIO_PORTF_AHB_DEN_R |= 0x1EU;  /*it makes the pins digital*/ 
}

inline static void initialiseTimer0Interrupt(void)
{
    SYSCTL_RCGCTIMER_R |= TIMER_0; /*clock gating for timer0*/
    TIMER0_CTL_R &= ~(1 << 0); /*deactivate timer0*/
    TIMER0_CFG_R = 0x00000000;
    TIMER0_TAMR_R |= 0x1;
    TIMER0_TAMR_R &= ~(1<<4);/*counting down*/
    TIMER0_TAILR_R = 0x00F42400; /*16Mhz, one second*/
    
    TIMER0->IMR |= (1<<0);
    NVIC_EnableIRQ(Timer0A_IRQn);
}

inline static void initialiseAdc(void)
{
    SYSCTL_RCGCADC_R |= (1<<1U); /*Activates ADC1*/
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;  /*gpio clock gate for port E*/
    GPIOE->DIR &= ~(1<<1U); /*PE1 input*/
    GPIOE->AFSEL |= (1<<1U); /*PE1 as alternate function*/
    GPIOE->DEN &= ~(1<<1U); /*Disable digital for PE1*/
    GPIOE->AMSEL |= (1<<1U); /*PE1 Analog modus*/
    
    /*Sample Sequencer configuration*/
    ADC1->ACTSS &= ~(1<<1U); /*DISABLE SS3*/
    ADC1->EMUX |= (0x0F<<12U); /*Continuous convertion*/
    ADC1->SSMUX3 = 2; /*PE1 is input 2 in the adc table*/

}



