#ifndef EEPROM_H
#define EEPROM_H

#include "stm32l4xx_hal.h"
#include <stdint.h>

#define EEPROM_ADDRESS      0x50        // 7-bit base address (A2A1A0 = 000)
#define EEPROM_WRITE_DELAY  5           // ms — required after every write

void    EEPROM_init(void);
void    EEPROM_write(uint16_t addr, uint8_t data);
uint8_t EEPROM_read(uint16_t addr);

#endif /* EEPROM_H */
