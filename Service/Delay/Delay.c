/*
 * Delay.c
 *
 *  Created on: januar 2024
 *      Author: Alejandro Bauernfeind
 */


#include "Delay.h"

void Delay_us(uint32_t us)
{
    // Asumiendo un reloj de 16 MHz (ajusta seg�n tu configuraci�n)
    volatile uint32_t i, j;

    for (int i = 0; i < us; ++i) 
    {
        for (int j = 0; j < 16; ++j) 
        {
            // Esto proporcionar� aproximadamente un miliegundo de retraso
        }
    }
}
void Delay_ms(uint32_t ms)
{
    // 
    volatile uint32_t i, j;

    for (int i = 0; i < ms; ++i) 
    {
        for (int j = 0; j < 8341; ++j) 
        {
            // Esto proporcionar� aproximadamente un miliegundo de retraso
        }
    }
}