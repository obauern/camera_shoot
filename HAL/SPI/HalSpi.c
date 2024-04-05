/*
 * HalSpi.c
 *
 *  Created on: januar 2024
 *      Author: Alejandro Bauernfeind
 */


#include "HalSpi.h"
#include "tm4c_cmsis.h"

#define SSI0_BASE_ADDR  0x40008000  // Dirección base del SSI0
#define SYSCTL_BASE     0x400FE000
   
#define RCGCSSI_OFFSET 0x61C
#define PRSSI_OFFSET   0xA1C
   
#define SSI_CR0_OFFSET 0x000
#define SSI_CR1_OFFSET 0x004
#define SSI_DR_OFFSET  0x008
#define SSI_SR_OFFSET  0x00C
#define SSI_CPSR_OFFSET 0x010
#define SSI_CC_OFFSET  0xFC8

#define SYSCTL_RCGCSSI (*((volatile uint32_t *)(SYSCTL_BASE + RCGCSSI_OFFSET)))
#define SYSCTL_PRSSI (*((volatile uint32_t *)(SYSCTL_BASE + PRSSI_OFFSET)))

#define SSI0_CR0 (*((volatile uint32_t *)(SSI0_BASE_ADDR + SSI_CR0_OFFSET)))
#define SSI0_CR1 (*((volatile uint32_t *)(SSI0_BASE_ADDR + SSI_CR1_OFFSET)))
#define SSI0_DR  (*((volatile uint32_t *)(SSI0_BASE_ADDR + SSI_DR_OFFSET)))
#define SSI0_SR  (*((volatile uint32_t *)(SSI0_BASE_ADDR + SSI_SR_OFFSET)))
#define SSI0_CPSR (*((volatile uint32_t *)(SSI0_BASE_ADDR + SSI_CPSR_OFFSET)))
#define SSI0_CC  (*((volatile uint32_t *)(SSI0_BASE_ADDR + SSI_CC_OFFSET)))

#define ENABLE_CLOCK_SSI0 (1U<<0U)
#define ENABLE_SSI0 (1U << 1U)
#define DISABLE_SSIO (~ENABLE_SSI0)

#define MASTER_CR1 (0U)

#define SSI0_8BITS (7U)
#define SSI0_SPI (~(3U << 4U))

#define CPSDVSR_SCREEN (6)//(2U)
#define CPSDVSR_TOUCH (67U)

#define CLOCK_POLARITY_HIGH (1U<<6U)
#define SPH_SECOND_CLOCK_EDGE_TRANSITION (1U << 7U)
#define SCR_0 (~(0xFF<<8U))

#define TRANSMIT_BUFFER_FULL (0x02)
#define RECEIVE_BUFFER_EMTPY (0x04)

typedef struct
{
    componentFrequency_t actualFreq;
}halSpiVariables_t;

static halSpiVariables_t halSpiVariables;
   
void HalSpi_init(void)
{
    SYSCTL_RCGCSSI |= ENABLE_CLOCK_SSI0;  // RCGCSSI
    while (!(SYSCTL_PRSSI & 0x01)); // PRSSI
    // Habilita el módulo SSI0
    SSI0_CR1 &= DISABLE_SSIO;  // Deshabilita el SSI mientras se configura
    
    // Datos de 8 bits, formato SPI
    SSI0_CR0 |= SSI0_8BITS;
    SSI0_CR0 &= SSI0_SPI;

    SSI0_CR0 |= CLOCK_POLARITY_HIGH;
    //SSI0_CR0 = 0x000F;   // Datos de 16 bits, formato SPI, fase 0, polaridad 0
    SSI0_CR1 = MASTER_CR1; // Configura como maestro

    // Configura la velocidad de transferencia
    // Frecuencia del sistema / (CPSDVSR * (1 + SCR))
    SSI0_CPSR = CPSDVSR_TOUCH;     
    halSpiVariables.actualFreq = TOUCH_FREQ;
    SSI0_CR0 &= SCR_0; 
    
    SSI0_CR0 |= SPH_SECOND_CLOCK_EDGE_TRANSITION; //SPH 1

    // Habilita el SSI
    SSI0_CR1 |= ENABLE_SSI0;     // Habilita el SSI
}

void HalSpi_changeFrequency(componentFrequency_t componentFrequency)
{
    if(componentFrequency != halSpiVariables.actualFreq)
    {
        halSpiVariables.actualFreq = componentFrequency;
        
        SSI0_CR1 &= DISABLE_SSIO;  // Deshabilita el SSI mientras se configura
        
        if (componentFrequency == SCREEN_FREQ)
        {
            SSI0_CPSR = CPSDVSR_SCREEN;      
        }
        else if (componentFrequency == TOUCH_FREQ)
        {
            SSI0_CPSR = CPSDVSR_TOUCH;    //under 1MHz
        }
        
        SSI0_CR1 |= ENABLE_SSI0; //se vuelve a habilitar el spi
        
        while (SSI0_SR & (1U << 4U)); /*espera a que se habilite el spi*/
    }
}

void HalSpi_testFrequency(uint8_t freq)
{
    SSI0_CR1 &= DISABLE_SSIO;  // Deshabilita el SSI mientras se configura
    
    SSI0_CPSR = freq;
    
    SSI0_CR1 |= ENABLE_SSI0; //se vuelve a habilitar el spi
    while (SSI0_SR & (1U << 4U)); /*espera a que se habilite el spi*/
}

uint8_t HalSpi_write(uint8_t data)
{
    while ((SSI0_SR & TRANSMIT_BUFFER_FULL) == 0);
    // Envía datos al registro de datos
    SSI0_DR = data;
    while ((SSI0_SR & RECEIVE_BUFFER_EMTPY) == 0);
    return (uint8_t)SSI0_DR;
}

uint8_t HalSpi_read(void)
{
    while (SSI0_SR & (1U << 4U)); /*espera a que se habilite el spi*/
    
    HalSpi_write(0x00); /*to trigger the read*/
    // Espera a que el buffer de recepción tenga algo
    while ((SSI0_SR & RECEIVE_BUFFER_EMTPY) == 0);
    // Lee datos desde el registro de datos
    return (uint8_t)SSI0_DR;
}