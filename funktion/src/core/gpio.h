#ifndef GPIO_C_H
#define GPIO_C_H

#include "definitions.h"

typedef enum{
  GPIOMode_Input       = 0,
  GPIOMode_Output,
  GPIOMode_AltFunction,
  GPIOMode_Analog,
}GPIOMode;

typedef enum{
  GPIOSpeed_2MHz       = 0,
  GPIOSpeed_25MHz      = 1,
  GPIOSpeed_50MHz      = 2,
  GPIOSpeed_80MHz_100MHz  = 3,
}GPIOSpeed;

typedef enum{
  GPIOResistor_NoPull     = 0,
  GPIOResistor_PullUp     = 1,
  GPIOResister_PullDown   = 2,
}GPIOResistor;

typedef enum {
  GPIOOutputType_PushPull  = 0,
  GPIOOutputType_OpenDrain = 1,
}GPIOOutputType;

typedef enum {
  GPIOAFMapping_System         = 0,
  GPIOAFMapping_TIM1_2         = 1,
  GPIOAFMapping_TIM3_5         = 2,
  GPIOAFMapping_TIM8_11        = 3,
  GPIOAFMapping_I2C1_3         = 4,
  GPIOAFMapping_SPI1_6         = 5,
  GPIOAFMapping_SPI2_3_SAI1    = 6,
  GPIOAFMapping_USART1_3       = 7,
  GPIOAFMapping_USART4_6       = 8,
  GPIOAFMapping_CAN1_2_TIM12_14  = 9,
  GPIOAFMapping_OTGFS_OTGHS      = 10,
  GPIOAFMapping_ETH              = 11,
  GPIOAFMapping_FSMC_FMC_SDIO_OTGHS = 12,
  GPIOAFMapping_DCMI           = 13,
  GPIOAFMapping_LTDC           = 14,
  GPIOAFMapping_EventOut       = 15,
}GPIOAFMapping;

void gpio_init(GPIO_TypeDef *GPIOX, GPIOMode mode, GPIOOutputType outputType, GPIOSpeed speed, GPIOResistor resistor, unsigned char pin);
void gpio_init_af(GPIO_TypeDef *GPIOX, GPIOMode mode, GPIOOutputType outputType, GPIOSpeed speed, GPIOResistor resistor, unsigned char pin, GPIOAFMapping af);
void gpio_init_input(GPIO_TypeDef *GPIOX, GPIOMode mode, GPIOResistor resistor, unsigned char pin);
void gpio_init_af_pins(GPIO_TypeDef *GPIOX, GPIOMode mode, GPIOOutputType outputType, GPIOSpeed speed, GPIOResistor resistor, int* pins, uint pinCount, GPIOAFMapping af);
void gpio_set_af(GPIO_TypeDef* GPIOX, unsigned short pin, GPIOAFMapping afpin);
void gpio_write(GPIO_TypeDef* GPIOX, int pin, bool state);
uint gpio_read(GPIO_TypeDef *GPIOX);
bool gpio_read_pin(GPIO_TypeDef* GPIOX, uint pin);
uint16_t gpio_port_source(GPIO_TypeDef* GPIOx);
uint16_t gpio_pin_source(uchar pin);
#endif
