/*
 * halPortA.c
 *
 *  Created on: januar 2024
 *      Author: Alejandro Bauernfeind
 */


#include "GpioPortA.h"
#include "tm4c_cmsis.h"

#define SYSCTL_BASE     0x400FE000

#define GPIOHBCTL    0x06C
#define RCGCGPIO_OFFSET 0x608

#define SYSCTL_GPIOHBCTL_PORTA (1U<<0U)

#define SYSCTL_RCGCGPIO (*((volatile uint32_t *)(SYSCTL_BASE + RCGCGPIO_OFFSET)))
#define SYSCTL_GPIOHBCTL_R (*((volatile uint32_t *)(SYSCTL_BASE + GPIOHBCTL)))

#define CLOCK_PORTA_AVAILABLE (1U << 0U)
#define PA2_OUTPUT (1U << 2U)
#define PA3_OUTPUT (1U << 3U)
#define PA4_INTPUT (~(1U << 4U))
#define PA5_OUTPUT (1U << 5U)
#define PA6_OUTPUT (1U << 6U)
#define PA7_OUTPUT (1U << 7U)

#define PA2_AFSEL PA2_OUTPUT
#define PA4_AFSEL (1U<<4U)
#define PA5_AFSEL PA5_OUTPUT

#define PA2_SSI0CLK (2U << 8U)
#define PA4_SSI0RX (2U << 16U)
#define PA5_SSI0TX (2U << 20U)

#define PA2_DEN  (1U << 2U)
#define PA3_DEN  (1U << 3U)
#define PA4_DEN  (1U << 4U)
#define PA5_DEN  (1U << 5U)
#define PA6_DEN  (1U << 6U)
#define PA7_DEN  (1U << 7U)

#define PA2_PUR  (1U << 2U)
#define PA3_PUR  (1U << 3U)
#define PA4_PUR  (1U << 4U)
#define PA5_PUR  (1U << 5U)

void GpioPortA_init(void)
{
    // Habilita el puerto A para los pines SPI
    SYSCTL_RCGCGPIO |= CLOCK_PORTA_AVAILABLE;           // Habilitar el reloj para el puerto A (RCGCGPIO)
    while (!(SYSCTL_RCGCGPIO & CLOCK_PORTA_AVAILABLE)); // Esperar hasta que el reloj del puerto A esté listo (PRGPIO)

    SYSCTL_GPIOHBCTL_R &= ~SYSCTL_GPIOHBCTL_PORTA;
    GPIOA->DIR |= PA2_OUTPUT | PA3_OUTPUT | PA5_OUTPUT | PA6_OUTPUT | PA7_OUTPUT;
    GPIOA->DIR &= PA4_INTPUT;
    
    GPIOA->AFSEL = 0x00;
    GPIOA->AFSEL |= PA2_AFSEL | PA4_AFSEL | PA5_AFSEL;
    
    GPIOA->PCTL &= 0xFF0000FF;
    GPIOA->PCTL |= PA2_SSI0CLK | PA4_SSI0RX | PA5_SSI0TX;
    
    GPIOA->DEN = 0x00;
    GPIOA->DEN |= PA2_DEN | PA3_DEN | PA4_DEN | PA5_DEN | PA6_DEN | PA7_DEN;
    
    GPIOA->PUR = 0x00;
    GPIOA->PUR |= PA2_PUR | PA3_PUR | PA4_PUR | PA5_PUR;
}

void GpioPortA_activatePin(uint8_t pin)
{
    GPIOA->DATA_Bits[pin] = 0xFFFFFFFF;
}

void GpioPortA_deactivatePin(uint8_t pin)
{
    GPIOA->DATA_Bits[pin] = 0;
}