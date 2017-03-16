// A flat C SPI library for the STM32F4
// Written by Oliver Stenning
// Private use only.

#ifndef SPI_C_H
#define SPI_C_H

#include "definitions.h"

typedef struct
{
  uchar SCLK;
  uchar NSS;
  uchar MOSI;
  uchar MISO;
}SPIPins;

typedef struct
{
  uint ControllerIndex;
  int AFLayout;
  SPIPins Pins;
  SPI_TypeDef *SPIX;
  GPIO_TypeDef *GPIOX;
  GPIO_TypeDef *GPIOX_CS;
  bool UseManualSS;
  void (*OnReceiveNotEmpty)(unsigned char);
  void (*OnTransmitEmpty)(void);
}SPIInstance;

typedef struct
{
  /* CR1 */
  bool CPHA;    //Clock phase
  bool CPOL;    //Clock polarity
  bool MSTR;    //Master selection
  bool BR_0;    //Baud rate control
  bool BR_1;    //Baud rate control
  bool BR_2;    //Baud rate control
  bool LSBFIRST;//Frame format
  bool SSM;     //Software slave management
  bool RXONLY;  //Receive only
  bool DFF;     //Data frame format
  bool CRCNEXT; //Transmit CRC next
  bool CRCEN;   //Hardware CRC calculation enable
  bool BIDIOE;  //Output enable in bidirectional mode
  bool BIDIMODE;//Bidirectional data mode enable
  /* CR2 */
  bool RXDMAEN; //Rx buffer DMA enable
  bool TXDMAEN; //Tx buffer DMA enable
  bool SSOE;    //SS output enable
  bool FRF;     //Frame format
  bool ERRIE;   //Error interrupt enable
  bool RXNEIE;  //RX buffer not empty interrupt enable
  bool TXEIE;   //Tx buffer empty interrupt enable
}SPIControlConfig;

typedef enum
{
  SPILow = 0,
  SPIHigh
}SPILineState;

void spi_init(SPIInstance* spi, SPIControlConfig* control);
void spi_enable(SPIInstance* spi, bool state);
void spi_write_byte(SPIInstance* spi, unsigned char byte);
void spi_write_short(SPIInstance* spi, unsigned short data);
void spi_slave_select(SPIInstance* spi, SPILineState state);
uchar spi_read_poll(SPIInstance* spi);

#endif
