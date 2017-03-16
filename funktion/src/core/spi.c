#include "spi.h"
#include "gpio.h"

#define SPI_SUPPORTED_CONTROLLERS 6

/* configure pins used by SPI1
 * PA5 = SCK
 * PA6 = MISO
 * PA7 = MOSI
 */

/*
SPI Initialisation Procedure for MASTER MODE
1. Select the BR[2:0] bits to define the serial clock baud rate (see SPI_CR1 register).
2. Select the CPOL and CPHA bits to define one of the four relationships between the data transfer and the serial clock (see Figure 248).
   This step is not required when the TI mode is selected.
3. Set the DFF bit to define 8- or 16-bit data frame format
4. Configure the LSBFIRST bit in the SPI_CR1 register to define the frame format. This step is not required when the TI mode is selected.
5. If the NSS pin is required in input mode, in hardware mode, connect the NSS pin to a high-level signal during the complete byte transmit sequence.
   In NSS software mode, set the SSM and SSI bits in the SPI_CR1 register.
   If the NSS pin is required in output mode, the SSOE bit only should be set. This step is not required when the TI mode is selected.
6. Set the FRF bit in SPI_CR2 to select the TI protocol for serial communications.
7. The MSTR and SPE bits must be set (they remain set only if the NSS pin is connected to a high-level signal).
*/

SPIInstance* InitialisedSPI[SPI_SUPPORTED_CONTROLLERS];

static void spi_clock_enable(SPI_TypeDef* SPIX)
{
  if (SPIX == SPI1)
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  else if (SPIX == SPI2)
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
  else if (SPIX == SPI3)
    RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
  else if (SPIX == SPI4)
    RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
  else if (SPIX == SPI5)
    RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
  else if (SPIX == SPI6)
    RCC->APB2ENR |= RCC_APB2ENR_SPI6EN;
}

void spi_init(SPIInstance* instance, SPIControlConfig* control)
{
  if (InitialisedSPI[instance->ControllerIndex-1] != NULL)
    throw(AlreadyInitException, "The SPI port is already initialised");

  InitialisedSPI[instance->ControllerIndex-1] = instance;

    // Configure the pins GPIO
  gpio_init(instance->GPIOX_CS, GPIOMode_Output, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResister_PullDown, instance->Pins.NSS);       // TODO: Make in/out configurable
  gpio_init(instance->GPIOX, GPIOMode_AltFunction, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResister_PullDown, instance->Pins.MOSI);    // Output
  gpio_init(instance->GPIOX, GPIOMode_AltFunction, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResister_PullDown, instance->Pins.SCLK);    // Output
  gpio_init(instance->GPIOX, GPIOMode_AltFunction, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResister_PullDown, instance->Pins.MISO);    // Input

    // Configure the pins AF. Re-map the GPIO to the AF.
  gpio_set_af(instance->GPIOX, instance->Pins.SCLK, instance->AFLayout);  // Enable SCLK
  gpio_set_af(instance->GPIOX, instance->Pins.MOSI, instance->AFLayout);  // Enable MOSI
  gpio_set_af(instance->GPIOX, instance->Pins.MISO, instance->AFLayout);  // Enable MISO

  // Enable clock
  spi_clock_enable(instance->SPIX);

  /* Ref: RM0090 pg. 869 */
  /* Note: The initialization process for SPI is described below. The order of the operations matter. */

  /* SPI Control Register #1 Configuration */

  // Configure Baud Rate
  Regset(&instance->SPIX->CR1, SPI_CR1_BR_0, control->BR_0);
  Regset(&instance->SPIX->CR1, SPI_CR1_BR_1, control->BR_1);
  Regset(&instance->SPIX->CR1, SPI_CR1_BR_2, control->BR_2);

  // Configure waveform
  Regset(&instance->SPIX->CR1, SPI_CR1_CPHA, control->CPHA);
  Regset(&instance->SPIX->CR1, SPI_CR1_CPOL, control->CPOL);

  // Configure dataframe format (8 or 16bit)
  Regset(&instance->SPIX->CR1, SPI_CR1_DFF, control->DFF);

  // Configure LSB or MSB first
  Regset(&instance->SPIX->CR1, SPI_CR1_LSBFIRST, control->LSBFIRST);

  // Configure NSS
  Regset(&instance->SPIX->CR1, SPI_CR1_SSM, control->SSM);
  if (control->SSM)
    Regset(&instance->SPIX->CR1, SPI_CR1_SSI, true);

  // Configure TI Protocol
  Regset(&instance->SPIX->CR2, SPI_CR2_FRF, control->FRF);

  // Configure Master or Slave
  Regset(&instance->SPIX->CR1, SPI_CR1_MSTR, control->MSTR);

  // Left over CR1 (Not in the initialization sequence outlined)
  Regset(&instance->SPIX->CR1, SPI_CR1_RXONLY, control->RXONLY);
  Regset(&instance->SPIX->CR1, SPI_CR1_CRCNEXT,  control->CRCNEXT);
  Regset(&instance->SPIX->CR1, SPI_CR1_CRCEN, control->CRCEN);
  Regset(&instance->SPIX->CR1, SPI_CR1_BIDIOE, control->BIDIOE);
  Regset(&instance->SPIX->CR1, SPI_CR1_BIDIMODE, control->BIDIMODE);

  // Left over CR2 (Not in the initialization sequence outlined)
  Regset(&instance->SPIX->CR2, SPI_CR2_TXEIE, control->TXEIE);
  Regset(&instance->SPIX->CR2, SPI_CR2_RXNEIE, control->RXNEIE);
  Regset(&instance->SPIX->CR2, SPI_CR2_ERRIE, control->ERRIE);
  Regset(&instance->SPIX->CR2, SPI_CR2_SSOE, control->SSOE);
  Regset(&instance->SPIX->CR2, SPI_CR2_TXDMAEN, control->TXDMAEN);
  Regset(&instance->SPIX->CR2, SPI_CR2_RXDMAEN, control->RXDMAEN);

    // Need to start the SPI in an off state (high ss)
  spi_slave_select(instance, SPIHigh);

  // Enable SPI
  Regset(&instance->SPIX->CR1, SPI_CR1_SPE, true);
}

void spi_slave_select(SPIInstance* spi, SPILineState state)
{
  bool isSSM = (spi->SPIX->CR1 & SPI_CR1_SSM) != false;

  if (isSSM)
  {
    if (state == SPILow)
      spi->GPIOX_CS->BSRRH |= 1 << spi->Pins.NSS;
    else if (state == SPIHigh)
      spi->GPIOX_CS->BSRRL |= 1 << spi->Pins.NSS;
  }
}

unsigned char spi_read_poll(SPIInstance* spi)
{
  spi_slave_select(spi, SPILow);
  while ((!(spi->SPIX->SR & (1 << 0))));   // Wait RXNE (Recieve not empty (we have data))
  uchar val = spi->SPIX->DR;
  spi_slave_select(spi, SPIHigh);
  return val;
}


void spi_write_short(SPIInstance* spi, unsigned short data)
{
  /* The CLK will begin to fire once the shift register (SPIx->DR) is filled */
  spi_slave_select(spi, SPILow);
  while (!(spi->SPIX->SR & (1 << 1)));
  spi->SPIX->DR = data;
  spi_slave_select(spi, SPIHigh);
}

void spi_write_byte(SPIInstance* spi, unsigned char byte)
{
  /* The CLK will begin to fire once the shift register (SPIx->DR) is filled */
  spi_slave_select(spi, SPILow);
  while (!(spi->SPIX->SR & (1 << 1)));
  spi->SPIX->DR = byte;
  spi_slave_select(spi, SPIHigh);
}

// IRQ Handlers

void spi_handle_interrupt(SPIInstance *instance)
{
  bool isTransmitEmpty = (instance->SPIX->SR & SPI_SR_TXE) != false;
  bool isReceieveNotEmpty = (instance->SPIX->SR & SPI_SR_RXNE) != false;

  if (isTransmitEmpty && instance->OnTransmitEmpty)
    instance->OnTransmitEmpty();

  if (isReceieveNotEmpty && instance->OnReceiveNotEmpty){
    unsigned char val = instance->SPIX->DR;
    instance->OnReceiveNotEmpty(val);
  }
}

void SPI1_IRQHandler(void)
{
  SPIInstance* instance = InitialisedSPI[0];
  spi_handle_interrupt(instance);
}
//...
void SPI5_IRQHandler(void)
{
  SPIInstance* instance = InitialisedSPI[5-1];
  spi_handle_interrupt(instance);
}
