/*
 * clockRegister.c
 *
 *  Created on: januar 2024
 *      Author: Alejandro Bauernfeind
 */


#include "HAL/CLOCK/Systick.h"
   
#include "HAL/CLOCK/clockRegister.h"
   
#include "tm4c_cmsis.h"

#define NVIC_ST_CTRL_COUNT      (1<<16)
#define NVIC_ST_CTRL_CLK_SRC   (1<<2)
#define NVIC_ST_CTRL_INTEN     (1<<1)
#define NVIC_ST_CTRL_ENABLE    (1)
   


void SysTick_init(void)
{
    uint32_t systemFreq = ClockRegister_getSystemFrequency();
    // Deshabilitar el SysTick durante la configuración
    SYSTICK->STCTRL &= ~NVIC_ST_CTRL_ENABLE;
    
    // Systick configured to be triggered every 100ms
    SYSTICK->STLOAD = (systemFreq/10) - 1;
    
    // Reiniciar el valor del contador
    SYSTICK->STCURRENT = 0;
    
    // Configurar el control del SysTick
    SYSTICK->STCTRL |= NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_ENABLE;
}


