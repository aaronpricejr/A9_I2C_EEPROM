#ifndef MAIN_H
#define MAIN_H

#include "stm32l4xx_hal.h"
#include "eeprom.h"

/* CubeMX-generated pin aliases for NUCLEO-L4A6ZG */
#define B1_Pin                   GPIO_PIN_13
#define B1_GPIO_Port             GPIOC

#define LD2_Pin                  GPIO_PIN_7
#define LD3_Pin                  GPIO_PIN_14
#define LD2_GPIO_Port            GPIOB
#define LD3_GPIO_Port            GPIOB

#define USB_PowerSwitchOn_Pin    GPIO_PIN_6
#define USB_PowerSwitchOn_GPIO_Port GPIOG
#define USB_OverCurrent_Pin      GPIO_PIN_7
#define USB_OverCurrent_GPIO_Port   GPIOG

#define USB_SOF_Pin              GPIO_PIN_8
#define USB_VBUS_Pin             GPIO_PIN_9
#define USB_ID_Pin               GPIO_PIN_10
#define USB_DM_Pin               GPIO_PIN_11
#define USB_DP_Pin               GPIO_PIN_12
#define USB_SOF_GPIO_Port        GPIOA
#define USB_VBUS_GPIO_Port       GPIOA
#define USB_ID_GPIO_Port         GPIOA
#define USB_DM_GPIO_Port         GPIOA
#define USB_DP_GPIO_Port         GPIOA

#define STLK_RX_Pin              GPIO_PIN_8
#define STLK_TX_Pin              GPIO_PIN_9
#define STLK_RX_GPIO_Port        GPIOG
#define STLK_TX_GPIO_Port        GPIOG

/* EEPROM test values */
#define TEST_ADDR   0x1A3C
#define TEST_DATA   0xE7

/* Error handler forward declaration */
void Error_Handler(void);

#endif /* MAIN_H */
