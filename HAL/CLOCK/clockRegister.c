/*
 * clockRegister.c
 *
 *  Created on: januar 2024
 *      Author: Alejandro Bauernfeind
 */


#include "HAL/CLOCK/clockRegister.h"
#include "tm4c_cmsis.h"

#define SYSCTL_BASE     0x400FE000

#define PLLSTAT_OFFSET 0x168
#define PLLFREQ0        0x160

#define SYSCTL_PLLSTAT (*((volatile uint32_t *)(SYSCTL_BASE + PLLSTAT_OFFSET)))
#define SYSCTL_PLLFREQ0 (*((volatile uint32_t *)(SYSCTL_BASE + PLLFREQ0)))

#define CLOCK_DISABLE (1U << 0U)
#define CLOCK_ENABLE  (~(1U << 0U))
#define MAIN_CLOCK   (~(0x3 << 4U))
#define PLL_BYPASS   (~(1U << 11U))
#define USE_SYS_DIV  (1U << 22U)
#define PLL_POWER_ON (~(1U << 13U))
#define CLEAR_XTAL   (~(0x1F << 6U))
#define XTAL_16MHZ   (0x15 << 6U)
#define CLEAR_SYSDIV (~(0xF << 23U))
#define SYSCTL_SYSDIV_3 (0x2 << 23U)

#define PLL_LOCKED (1U<<6U)

#define NO_USER_RCC2 (~(1U << 31U))

typedef struct clockControl_TAG
{
   uint32_t sysFrequency;
}clockControl_t;

static clockControl_t clockControl;

static void systemClockTo66MHz(void);

void ClockRegister_init(void)
{
    systemClockTo66MHz();
}

static void systemClockTo66MHz(void)
{
    // Deshabilitar el oscilador principal mientras se realiza la configuración
    SYSCTL->RCC |= CLOCK_DISABLE;  // MOSCDIS
    
    SYSCTL->RCC2 &= NO_USER_RCC2;
    
    // Seleccionar el oscilador principal como fuente (el externo)
    SYSCTL->RCC &= MAIN_CLOCK;  // Limpiar los bits OSCSRC
    
    SYSCTL->RCC |= USE_SYS_DIV;
    
    SYSCTL->RCC &= PLL_POWER_ON; //Clear the pwrdown pll
  
    // Configurar el valor del cristal a 16 MHz
    SYSCTL->RCC &= CLEAR_XTAL;   // Limpiar los bits XTAL
    SYSCTL->RCC |= XTAL_16MHZ;    // Configurar XTAL a 0x15 (16 MHz)

    SYSCTL->RCC &= CLEAR_SYSDIV;   // Limpia sysdiv
    SYSCTL->RCC |= SYSCTL_SYSDIV_3; //Sysdiv en 3
    
    while(!(SYSCTL->RCC & PLL_LOCKED)); //wait for the pll to lock
          
    SYSCTL->RCC &= PLL_BYPASS; //The pll is enabled by clearing the bypass
    
    // Habilitar el oscilador principal mientras se realiza la configuración
    SYSCTL->RCC &= CLOCK_ENABLE;  // MOSCDIS
    
    clockControl.sysFrequency = 66000000U;
}

uint32_t ClockRegister_getSystemFrequency(void)
{
    return clockControl.sysFrequency;
}
