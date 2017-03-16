#include "gpio.h"
#include "spi.h"

/*
Reference:
    STM32F4 Reference Manual. DM00031020
    Page 266.
Summary:
    Each general-purpose I/O port has:
  - four 32-bit configuration registers (GPIOx_MODER, GPIOx_OTYPER, GPIOx_OSPEEDR and GPIOx_PUPDR),
  -  two 32-bit data registers (GPIOx_IDR and GPIOx_ODR),
  -  a 32-bit set/reset register (GPIOx_BSRR),
  -  a 32-bit locking register (GPIOx_LCKR)
  -  two 32-bit alternate function selection register (GPIOx_AFRH and GPIOx_AFRL).
*/

static void gpio_clock_init(GPIO_TypeDef* GPIOX)
{
  if (GPIOX == GPIOA)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  else if (GPIOX == GPIOB)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
  else if (GPIOX == GPIOC)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
  else if (GPIOX == GPIOD)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
  else if (GPIOX == GPIOE)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
  else if (GPIOX == GPIOF)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
  else if (GPIOX == GPIOG)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
  else if (GPIOX == GPIOH)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
  else if (GPIOX == GPIOK)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;
}


// 16 pins are mapped to with 2 bits to each pin.

void gpio_init_af_pins(GPIO_TypeDef *GPIOX, GPIOMode mode, GPIOOutputType outputType, GPIOSpeed speed, GPIOResistor resistor, int* pins, uint pinCount, GPIOAFMapping af)
{
  for (int i = 0; i < pinCount; i++)
  {
    int pin = pins[i];
    gpio_init(GPIOX, mode, outputType, speed, resistor, pin);
    gpio_set_af(GPIOX, pin, af);
  }
}

void gpio_init_af(GPIO_TypeDef *GPIOX, GPIOMode mode, GPIOOutputType outputType, GPIOSpeed speed, GPIOResistor resistor, unsigned char pin, GPIOAFMapping af)
{
  gpio_init(GPIOX, mode, outputType, speed, resistor, pin);
  gpio_set_af(GPIOX, pin, af);
}

void gpio_init(GPIO_TypeDef *GPIOX, GPIOMode mode, GPIOOutputType outputType, GPIOSpeed speed, GPIOResistor resistor, unsigned char pin)
{
  // Clock configuration
  gpio_clock_init(GPIOX);

  int bit = pin * 2; // 2 bits per pin

  GPIOX->MODER &= ~(mode << bit);
  GPIOX->MODER |= mode << bit;

  GPIOX->OSPEEDR &= ~(speed << bit);
  GPIOX->OSPEEDR |= speed << bit;

  GPIOX->PUPDR &= ~(resistor << bit);
  GPIOX->PUPDR |= resistor << bit;

  GPIOX->OTYPER &= ~(outputType << pin);
  GPIOX->OTYPER |= outputType << pin;
}

void gpio_init_input(GPIO_TypeDef *GPIOX, GPIOMode mode, GPIOResistor resistor, unsigned char pin)
{
  if (mode != GPIOMode_Input && mode != GPIOMode_AltFunction)
    throw(InvalidArgumentException, "The GPIOMode must be either Input or Alternate function");

  gpio_init(GPIOX, mode, GPIOOutputType_PushPull, GPIOSpeed_2MHz, resistor, pin);
}


/*
    AF#
    0 RTC_50Hz, MCO, TAMPER, SWJ, TRACE
    1 TIM1, TIM2
    2 TIM3, TIM4, TIM5
    3 TIM8, TIM9, TIM10, TIM11
    4 I2C1, I2C2, I2C3,
    5 SPI1, SPI2/I2S2
    6 SPI3/I2S3
    7 IS23ext, USART1, USART2, USART3,
    8 UART4, UART5, USART6
    9 CAN1, CAN2, TIM12, TIM13, TIM14
   10 OTG_FS, OTG_HS
   11 ETHERNET
   12 FSMC, OTG HS, SDIO
   13 DCMI
   14
   15 EVENTOUT
*/

void gpio_set_af(GPIO_TypeDef* GPIOX, uint16_t pin, GPIOAFMapping afpin)
{
  // Pulled from stm32f4 stdlib

  uint32_t temp = 0x00;
  uint32_t temp_2 = 0x00;

  temp = ((uint32_t)(afpin) << ((uint32_t)((uint32_t)pin & (uint32_t)0x07) * 4));
  GPIOX->AFR [pin >> 0x03] &= ~((uint32_t)0xF << ((uint32_t)((uint32_t)pin & (uint32_t)0x07) * 4));
  temp_2 = GPIOX->AFR [pin >> 0x03] | temp;
  GPIOX->AFR [pin >> 0x03] = temp_2;
}

bool gpio_read_pin(GPIO_TypeDef* GPIOX, uint pin)  //Daniyal Noor: maybe this should be uchar!
{
  return ((GPIOX->IDR & (1 << pin)) ? true : false);
}

uint gpio_read(GPIO_TypeDef *GPIOX)
{
  return GPIOX->IDR;
}

void gpio_write(GPIO_TypeDef* GPIOX, int pin, bool state)
{
  GPIOX->ODR = state << pin;
}

uint16_t gpio_port_source(GPIO_TypeDef* GPIOx)
{
  uint16_t portsource = 0;

  if (GPIOx == GPIOA)
    portsource = 0x00;
  else if (GPIOx == GPIOB)
    portsource = 0x01;
  else if (GPIOx == GPIOC)
    portsource = 0x02;
  else if (GPIOx == GPIOD)
    portsource = 0x03;
  else if (GPIOx == GPIOE)
    portsource = 0x04;
  else if (GPIOx == GPIOF)
    portsource = 0x05;
  else if (GPIOx == GPIOG)
    portsource = 0x06;
  else if (GPIOx == GPIOH)
    portsource = 0x07;
  else if (GPIOx == GPIOI)
    portsource = 0x08;
  else if (GPIOx == GPIOJ)
    portsource = 0x09;
  else if (GPIOx == GPIOK)
    portsource = 0x0A;

  /* Return portsource */
  return portsource;
}

uint16_t gpio_pin_source(uchar pin)
{
  uint16_t pinsource = 0;

  /* Get pinsource */
  while (pin > 1) {
    /* Increase pinsource */
    pinsource++;
    /* Shift right */
    pin >>= 1;
  }

  /* Return source */
  return pinsource;
}
