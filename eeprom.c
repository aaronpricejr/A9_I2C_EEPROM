/*
 *******************************************************************************
 * @file           : eeprom.c
 * @brief          : EEPROM read/write via I2C1
 * project         : EE 329 S'26 AX
 * authors         : alexys
 * version         : 0.1
 * date            : May 18, 2026
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * Description: Provides EEPROM_init, EEPROM_write, EEPROM_read for a
 *              24-series I2C EEPROM (e.g. AT24C32) over I2C1.
 *              All signals are 3.3 V — no 5 V drive applied.
 *******************************************************************************
 * GPIO Wiring
 * |   Component   | GPIO Identifier | Connector Location | Config
 *-----------------------------------------------------------------------------
 * | EEPROM - SCL  | PB8             | CN10-13            | AF4 OD
 * | EEPROM - SDA  | PB9             | CN10-15            | AF4 OD
 *******************************************************************************
 * Version History
 *  Ver.|   Date     | Description
 *  ---------------------------------------------------------------------------
 *  0.1 | 2026-05-18 | Initial — init, write, read
 *******************************************************************************
 *
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */
#include "eeprom.h"

/* ── EEPROM_init ──────────────────────────────────────────────────────────
 * Configures PB6 (SCL) and PB7 (SDA) as open-drain AF4 (I2C1),
 * then brings up I2C1 at ~100 kHz using 16 MHz SYSCLK timing from CubeMX.
 * 3.3 V logic — no 5 V signals applied to STM32L4 pins.
 */
void EEPROM_init(void)
{
	/* 1. Clocks */
	RCC->AHB2ENR  |=  RCC_AHB2ENR_GPIOBEN;     // GPIOB: SCL / SDA
	RCC->APB1ENR1 |=  RCC_APB1ENR1_I2C1EN;     // I2C1 bus clock

	/* 2. MODER — PB8, PB9 → Alternate Function (0b10) */
	GPIOB->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
	GPIOB->MODER |=  (2u << GPIO_MODER_MODE8_Pos) |
	                 (2u << GPIO_MODER_MODE9_Pos);

	/* 3. OTYPER — open-drain (required for I2C) */
	GPIOB->OTYPER |=  (GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);

	/* 4. OSPEEDR — high speed */
	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9);
	GPIOB->OSPEEDR |=  (2u << GPIO_OSPEEDR_OSPEED8_Pos) |
	                   (2u << GPIO_OSPEEDR_OSPEED9_Pos);

	/* 5. PUPDR — no pull (external pull-ups on SCL/SDA to 3.3 V) */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);

	/* 6. AFR[1] — PB8, PB9 → AF4 (I2C1) */
	GPIOB->AFR[4] &= ~(0xFu << GPIO_AFRH_AFSEL8_Pos);
	GPIOB->AFR[4] |=  (0x4u << GPIO_AFRH_AFSEL8_Pos);

	GPIOB->AFR[4] &= ~(0xFu << GPIO_AFRH_AFSEL9_Pos);
	GPIOB->AFR[4] |=  (0x4u << GPIO_AFRH_AFSEL9_Pos);

    /* 7. Configure I2C1 */
    I2C1->CR1    &= ~I2C_CR1_PE;               // disable I2C for config
    I2C1->CR1    &= ~I2C_CR1_ANFOFF;           // enable analog filter
    I2C1->CR1    &= ~I2C_CR1_DNF;              // disable digital filter
    I2C1->TIMINGR = 0x00100D14;    				  // 100 kHz @ 4 MHz SYSCLK
    I2C1->CR2    |=  I2C_CR2_AUTOEND;          // auto STOP after last byte
    I2C1->CR2    &= ~I2C_CR2_ADD10;            // 7-bit address mode
    I2C1->CR1    |=  I2C_CR1_PE;               // enable I2C
}

/* ── EEPROM_write ─────────────────────────────────────────────────────────
 * Writes one byte to a 15-bit memory address.
 * Transaction: START | DevAddr+W | AddrHigh | AddrLow | Data | STOP
 * Caller must wait >= 5 ms after return before issuing a READ (tWR cycle).
 */
void EEPROM_write(uint16_t addr, uint8_t data)
{
    /* Build CR2: WRITE, 3 bytes (2 addr + 1 data), device address */
    I2C1->CR2 &= ~I2C_CR2_RD_WRN;                          // WRITE mode
    I2C1->CR2 &= ~I2C_CR2_NBYTES;
    I2C1->CR2 |=  (3u << I2C_CR2_NBYTES_Pos);              // 3 bytes total
    I2C1->CR2 &= ~I2C_CR2_SADD;
    I2C1->CR2 |=  (EEPROM_ADDRESS << (I2C_CR2_SADD_Pos + 1)); // 7-bit addr SHL 1
    I2C1->CR2 |=   I2C_CR2_START;                          // issue START

    /* Byte 1: memory address high byte */
    while (!(I2C1->ISR & I2C_ISR_TXIS)) ;
    I2C1->TXDR = (addr >> 8) & 0xFF;

    /* Byte 2: memory address low byte */
    while (!(I2C1->ISR & I2C_ISR_TXIS)) ;
    I2C1->TXDR = addr & 0xFF;

    /* Byte 3: data */
    while (!(I2C1->ISR & I2C_ISR_TXIS)) ;
    I2C1->TXDR = data;

    /* Wait for STOP (AUTOEND issues it after byte 3) */
    while (!(I2C1->ISR & I2C_ISR_STOPF)) ;
    I2C1->ICR |= I2C_ICR_STOPCF;                           // clear STOPF flag
}

uint8_t EEPROM_read(uint16_t addr)
{
    /* Phase 1: WRITE the 2-byte memory address (no AUTOEND for restart) */
    I2C1->CR2 &= ~I2C_CR2_AUTOEND;                         // manual STOP / restart
    I2C1->CR2 &= ~I2C_CR2_RD_WRN;                          // WRITE mode
    I2C1->CR2 &= ~I2C_CR2_NBYTES;
    I2C1->CR2 |=  (2u << I2C_CR2_NBYTES_Pos);              // 2 address bytes
    I2C1->CR2 &= ~I2C_CR2_SADD;
    I2C1->CR2 |=  (EEPROM_ADDRESS << (I2C_CR2_SADD_Pos + 1));
    I2C1->CR2 |=   I2C_CR2_START;

    /* Address high */
    while (!(I2C1->ISR & I2C_ISR_TXIS)) ;
    I2C1->TXDR = (addr >> 8) & 0xFF;

    /* Address low */
    while (!(I2C1->ISR & I2C_ISR_TXIS)) ;
    I2C1->TXDR = addr & 0xFF;

    /* Wait for address phase to complete */
    while (!(I2C1->ISR & I2C_ISR_TC)) ;

    /* Phase 2: READ 1 byte with repeated START */
    I2C1->CR2 |=  I2C_CR2_AUTOEND;                         // re-enable AUTOEND
    I2C1->CR2 |=  I2C_CR2_RD_WRN;                          // READ mode
    I2C1->CR2 &= ~I2C_CR2_NBYTES;
    I2C1->CR2 |=  (1u << I2C_CR2_NBYTES_Pos);              // 1 data byte
    I2C1->CR2 |=   I2C_CR2_START;                          // repeated START

    /* Wait for received byte */
    while (!(I2C1->ISR & I2C_ISR_RXNE)) ;
    uint8_t rxdata = (uint8_t)I2C1->RXDR;

    /* Wait for STOP */
    while (!(I2C1->ISR & I2C_ISR_STOPF)) ;
    I2C1->ICR |= I2C_ICR_STOPCF;

    return rxdata;
}
