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
GPIOB->AFR[1] &= ~(0xFu << GPIO_AFRH_AFSEL8_Pos);
GPIOB->AFR[1] |=  (0x4u << GPIO_AFRH_AFSEL8_Pos);

GPIOB->AFR[1] &= ~(0xFu << GPIO_AFRH_AFSEL9_Pos);
GPIOB->AFR[1] |=  (0x4u << GPIO_AFRH_AFSEL9_Pos);
