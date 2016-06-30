#include "ILI9341.h"

 /*
 STM32F429 DISCOVERY LCD PIN CONNECTIONS: SPI5.
        LCD   BOARD     DISCOVERY BOARD 	
        SCK 	PF7       SPI clock 	      
        SDO   PF8 	    (MISO	Not used    
        SDI   PF9 	    (MOSI)
        CS    PC2       Chip S. SPI 	    
        D/C   PD13      Data/Command reg 	
        RESET PD12      Reset LCD. Not used on discovery board
*/

//LCD settings
#define ILI9341_WIDTH 				240
#define ILI9341_HEIGHT				320
#define ILI9341_PIXEL				76800


//Bits
//Transparent background, only for strings and chars
#define ILI9341_TRANSPARENT     0x80000000

//Commands
#define ILI9341_RESET				    0x01
#define ILI9341_SLEEP_OUT       0x11
#define ILI9341_GAMMA           0x26
#define ILI9341_DISPLAY_OFF     0x28
#define ILI9341_DISPLAY_ON      0x29
#define ILI9341_COLUMN_ADDR     0x2A
#define ILI9341_PAGE_ADDR			  0x2B
#define ILI9341_GRAM            0x2C
#define ILI9341_MAC             0x36
#define ILI9341_PIXEL_FORMAT    0x3A
#define ILI9341_WDB             0x51
#define ILI9341_WCD             0x53
#define ILI9341_RGB_INTERFACE   0xB0
#define ILI9341_FRC             0xB1
#define ILI9341_BPC             0xB5
#define ILI9341_DFC             0xB6
#define ILI9341_POWER1          0xC0
#define ILI9341_POWER2          0xC1
#define ILI9341_VCOM1           0xC5
#define ILI9341_VCOM2           0xC7
#define ILI9341_POWERA          0xCB
#define ILI9341_POWERB          0xCF
#define ILI9341_PGAMMA          0xE0
#define ILI9341_NGAMMA          0xE1
#define ILI9341_DTCA            0xE8
#define ILI9341_DTCB            0xEA
#define ILI9341_POWER_SEQ       0xED
#define ILI9341_3GAMMA_EN       0xF2
#define ILI9341_INTERFACE       0xF6
#define ILI9341_PRC             0xF7

/* ADAFRUIT DEFINITIONS */
#define ILI9341_NOP 0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID 0x04
#define ILI9341_RDDST 0x09
#define ILI9341_SLPIN 0x10
#define ILI9341_SLPOUT 0x11
#define ILI9341_PTLON 0x12
#define ILI9341_NORON 0x13
#define ILI9341_RDMODE 0x0A
#define ILI9341_RDMADCTL 0x0B
#define ILI9341_RDPIXFMT 0x0C
#define ILI9341_RDIMGFMT 0x0A
#define ILI9341_RDSELFDIAG 0x0F
#define ILI9341_INVOFF 0x20
#define ILI9341_INVON 0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON 0x29
#define ILI9341_CASET 0x2A
#define ILI9341_PASET 0x2B
#define ILI9341_RAMWR 0x2C
#define ILI9341_RAMRD 0x2E
#define ILI9341_PTLAR 0x30
#define ILI9341_MADCTL 0x36
#define ILI9341_PIXFMT 0x3A
#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR 0xB4
#define ILI9341_DFUNCTR 0xB6
#define ILI9341_PWCTR1 0xC0
#define ILI9341_PWCTR2 0xC1
#define ILI9341_PWCTR3 0xC2
#define ILI9341_PWCTR4 0xC3
#define ILI9341_PWCTR5 0xC4
#define ILI9341_VMCTR1 0xC5
#define ILI9341_VMCTR2 0xC7
#define ILI9341_RDID1 0xDA
#define ILI9341_RDID2 0xDB
#define ILI9341_RDID3 0xDC
#define ILI9341_RDID4 0xDD
#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1

#define ILI9341_MAX_BUFF_SIZE 64

/* Forward declarations */
static void ili9341_init_comms(void);
static void ili9341_init_driver(void);
static void ili9341_delay(volatile unsigned int delay);

static ILI9341Settings settings = {0};

// Pin stuff
static GPIO_TypeDef* GPIO_RST_WRX = GPIOD;
static int pinWRX = 13;
static int pinRST = 12;

static SPIInstance SPIHandle = {0};
static SPIControlConfig Control = {0};
static DMAInstance DMAHandle = {0};
volatile static uchar TxBuffer[ILI9341_MAX_BUFF_SIZE] = {0};

void ili9341_init(void)
{
	ili9341_init_comms();
	ili9341_init_driver();
}

static void ili9341_dma_transfer_complete(void)
{
//	spi_slave_select(&SPIHandle, SPIHigh);
	dma_enable(&DMAHandle, false);
}

static void ili9341_dma_transfer_error(void)
{
}

static void ili9341_dma_transfer_half(void)
{
}

static void ili9341_spi_transmit_complete(void)
{
}

void ili9341_init_comms(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	RCC->APB2ENR |= RCC_APB2ENR_SPI5EN; 	    // Alternate Function Clock Enable

	// Initialize the WRX and RST pins
	gpio_init(GPIO_RST_WRX, GPIOMode_Output, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_NoPull, pinWRX);
	gpio_init(GPIO_RST_WRX, GPIOMode_Output, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_NoPull, pinRST);
	
	// Configure the SPI for the LCD
	SPIPins pins = {0};
	pins.NSS  = 2;  
	pins.SCLK = 7;	
	pins.MISO = 8;	
	pins.MOSI = 9;

	// Configure SPI Registers
	SPIHandle.ControllerIndex = 5;
	SPIHandle.GPIOX = GPIOF;     	// CLK/MISO/MOSI GPIOF
	SPIHandle.GPIOX_CS = GPIOC;  	// Slave Select GPIOC
	SPIHandle.SPIX = SPI5;       	// SPI port 5
	SPIHandle.Pins = pins;
	SPIHandle.AFLayout = 5;      	// Use SPI5 for AF.
	SPIHandle.OnTransmitEmpty = &ili9341_spi_transmit_complete;
	Control.MSTR = true;
	Control.SSM  = true;
	Control.DFF  = false;
	Control.BIDIMODE = true; 	// 1 line, bidirectional
	Control.BIDIOE = true;		// Output only mode
	Control.TXDMAEN = true;		// ENABLE DMA FOR TX
	//Control.TXEIE = true;		// Enable SPI TX complete interrupt


	// Initialize DMA
	DMAHandle.DMAx = DMA2;
	DMAHandle.Stream = DMA2_Stream4;
	DMAHandle.ControllerIndex = 2;
	DMAHandle.StreamIndex = 4;
	DMAHandle.Memory0BaseAddr = (uint*) &TxBuffer;
	DMAHandle.PeriphBaseAddr = (uint*) &(SPI5->DR);
	DMAHandle.InitConfig.Channel = DMAChannel_2;
	DMAHandle.InitConfig.Direction = DMADirection_MemoryToPeriph;
	DMAHandle.InitConfig.PeriphIncrement = false;
	DMAHandle.InitConfig.MemoryIncrement = false;
	DMAHandle.InitConfig.PeriphDataSize = DMADataSize_Byte;
	DMAHandle.InitConfig.MemoryDataSize = DMADataSize_Byte;
	DMAHandle.InitConfig.CircularMode = false;
	DMAHandle.InitConfig.Priority = DMAPriority_VHigh;
	DMAHandle.InitConfig.DirectModeDisabled = true;	// Dont use FIFO
	DMAHandle.InitConfig.PeriphBurstSize = DMABurstSize_INCR16;
	DMAHandle.InitConfig.MemoryBurstSize = DMABurstSize_INCR16;
	DMAHandle.InitConfig.FifoThreshold = DMAFifoThreshold_Full;

	DMAHandle.InitConfig.TransferCompleteInterrupt = true;
	DMAHandle.InitConfig.TransferErrorInterrupt = true;
	DMAHandle.InitConfig.HalfTransferInterrupt = true;

	DMAHandle.OnTransferComplete = &ili9341_dma_transfer_complete;
	DMAHandle.OnTransferError = &ili9341_dma_transfer_error;
	DMAHandle.OnHalfTransferComplete = &ili9341_dma_transfer_half;

	dma_init(&DMAHandle);

	//NVIC_SetPriority(SPI5_IRQn, 6);
	//NVIC_SetPriority(DMA2_Stream4_IRQn, 2);
	NVIC_EnableIRQ(DMA2_Stream4_IRQn);
	//NVIC_EnableIRQ(SPI5_IRQn);

	spi_init(&SPIHandle, &Control);

	spi_slave_select(&SPIHandle, SPILow);
}

void ili9341_init_driver(void)
{
	gpio_write(GPIO_RST_WRX, pinRST, true);

	ili9341_send_command(ILI9341_RESET);
	ili9341_delay(2000000);

	ili9341_send_command(ILI9341_POWERA);
	ili9341_send_data(0x39);
	ili9341_send_data(0x2C);
	ili9341_send_data(0x00);
	ili9341_send_data(0x34);
	ili9341_send_data(0x02);
	ili9341_send_command(ILI9341_POWERB);
	ili9341_send_data(0x00);
	ili9341_send_data(0xC1);
	ili9341_send_data(0x30);
	ili9341_send_command(ILI9341_DTCA);
	ili9341_send_data(0x85);
	ili9341_send_data(0x00);
	ili9341_send_data(0x78);
	ili9341_send_command(ILI9341_DTCB);
	ili9341_send_data(0x00);
	ili9341_send_data(0x00);
	ili9341_send_command(ILI9341_POWER_SEQ);
	ili9341_send_data(0x64);
	ili9341_send_data(0x03);
	ili9341_send_data(0x12);
	ili9341_send_data(0x81);
	ili9341_send_command(ILI9341_PRC);
	ili9341_send_data(0x20);
	ili9341_send_command(ILI9341_POWER1);
	ili9341_send_data(0x23);
	ili9341_send_command(ILI9341_POWER2);
	ili9341_send_data(0x10);
	ili9341_send_command(ILI9341_VCOM1);
	ili9341_send_data(0x3E);
	ili9341_send_data(0x28);
	ili9341_send_command(ILI9341_VCOM2);
	ili9341_send_data(0x86);
	ili9341_send_command(ILI9341_MAC);
	ili9341_send_data(0x48);
	ili9341_send_command(ILI9341_PIXEL_FORMAT);
	ili9341_send_data(0x55);

	ili9341_send_command(ILI9341_FRC);            // Frame Rate Control
	ili9341_send_data(0x00);                      // Division ratio 1
	ili9341_send_data(0x18);                      // 16 clocks per line
	ili9341_send_command(ILI9341_DFC);
	ili9341_send_data(0x08);
	ili9341_send_data(0x82);
	ili9341_send_data(0x27);
	ili9341_send_command(ILI9341_3GAMMA_EN);
	ili9341_send_data(0x00);
	ili9341_send_command(ILI9341_COLUMN_ADDR);    // Column Address set
	ili9341_send_data(0x00);                      //
	ili9341_send_data(0x00);                      // Start column = 0
	ili9341_send_data(0x00);                      //
	ili9341_send_data(0xEF);                      // End column = 239
	ili9341_send_command(ILI9341_PAGE_ADDR);      // Column Address set
	ili9341_send_data(0x00);                      //
	ili9341_send_data(0x00);                      // Start page = 0
	ili9341_send_data(0x01);                      //
	ili9341_send_data(0x3F);                      // End page = 319
	ili9341_send_command(ILI9341_GAMMA);          // Gamma Set
	ili9341_send_data(0x01);                      // Gamma curve 1 (G2.2)
	ili9341_send_command(ILI9341_PGAMMA);
	ili9341_send_data(0x0F);
	ili9341_send_data(0x31);
	ili9341_send_data(0x2B);
	ili9341_send_data(0x0C);
	ili9341_send_data(0x0E);
	ili9341_send_data(0x08);
	ili9341_send_data(0x4E);
	ili9341_send_data(0xF1);
	ili9341_send_data(0x37);
	ili9341_send_data(0x07);
	ili9341_send_data(0x10);
	ili9341_send_data(0x03);
	ili9341_send_data(0x0E);
	ili9341_send_data(0x09);
	ili9341_send_data(0x00);
	ili9341_send_command(ILI9341_NGAMMA);
	ili9341_send_data(0x00);
	ili9341_send_data(0x0E);
	ili9341_send_data(0x14);
	ili9341_send_data(0x03);
	ili9341_send_data(0x11);
	ili9341_send_data(0x07);
	ili9341_send_data(0x31);
	ili9341_send_data(0xC1);
	ili9341_send_data(0x48);
	ili9341_send_data(0x08);
	ili9341_send_data(0x0F);
	ili9341_send_data(0x0C);
	ili9341_send_data(0x31);
	ili9341_send_data(0x36);
	ili9341_send_data(0x0F);
	ili9341_send_command(ILI9341_SLEEP_OUT);

	ili9341_delay(1000000);

	ili9341_send_command(ILI9341_DISPLAY_ON);
	ili9341_send_command(ILI9341_GRAM);
}


static void ili9341_delay(volatile unsigned int delay) 
{
	for (; delay != 0; delay--); 
}

void ili9341_set_cursor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) 
{
	ili9341_send_command(ILI9341_COLUMN_ADDR);

	uint8_t buff1[] = { (x1 >> 8), (x1 & 0xFF),
			            (x2 >> 8), (x2 & 0xFF)
	                  };

	ili9341_send_data(buff1[0]);
	ili9341_send_data(buff1[1]);
	ili9341_send_data(buff1[2]);
	ili9341_send_data(buff1[3]);

	ili9341_send_command(ILI9341_PAGE_ADDR);

	uint buff2[] = { (y1 >> 8), (y1 & 0xFF),
					 (y2 >> 8), (y2 & 0xFF)
				   };

	ili9341_send_data(buff2[0]);
	ili9341_send_data(buff2[1]);
	ili9341_send_data(buff2[2]);
	ili9341_send_data(buff2[3]);
//	ili9341_send_data_buffer(buff2,4);
}

void ili9341_rotate(ILI9341Orientation orientation)
{
	ili9341_send_command(ILI9341_MAC);
	ili9341_send_data((uchar)orientation);
	settings.Orientation = orientation;

	if (orientation == ILI9341_Orientation_Portrait1 || orientation == ILI9341_Orientation_Portrait2)
	{
		settings.Height = ILI9341_HEIGHT;
		settings.Width = ILI9341_WIDTH;
	}
	else
	{
		settings.Height = ILI9341_WIDTH;
		settings.Width = ILI9341_HEIGHT;
	}
}


static void ili9341_send_dma(uint length)
{
	DMAHandle.Count = length;
	dma_set_count(&DMAHandle);

	// Enable the DMA Stream, beginning transmission from the buffer to the PeriphBaseAddr
	dma_enable(&DMAHandle, true);
}

void ili9341_send_data_buffer(int* data, uint length)
{
	if (length > ILI9341_MAX_BUFF_SIZE)
		throw(IndexOutOfRangeException, "ILI9341 send buffer overflow guard exception");

	for (int i = 0; i < length; i++)
		TxBuffer[i] = data[i];

	gpio_write(GPIO_RST_WRX, pinWRX, true);
	ili9341_send_dma(length);
}

void ili9341_send_data(uint8_t data)
{
	TxBuffer[0] = data;
	gpio_write(GPIO_RST_WRX, pinWRX, true);
	ili9341_send_dma(1);
}

void ili9341_send_command_buffer(uint8_t* command, uint length)
{
	if (length > ILI9341_MAX_BUFF_SIZE)
		throw(IndexOutOfRangeException, "TxBuffer overflow guard exception");

	for (int i = 0; i < length; i++)
		TxBuffer[i] = command[i];

	gpio_write(GPIO_RST_WRX, pinWRX, false);
	ili9341_send_dma(length);
}

void ili9341_send_command(uint8_t command)
{
	TxBuffer[0] = command;
	gpio_write(GPIO_RST_WRX, pinWRX, false);
	ili9341_send_dma(1);
}


void ili9341_fill(uint32_t color) 
{
	unsigned int n, i, j;
	i = color >> 8;
	j = color & 0xFF;
	
	ili9341_set_cursor(0, 0, settings.Width - 1, settings.Height - 1);
	ili9341_send_command(ILI9341_GRAM);

	for (n = 0; n < ILI9341_PIXEL; n ++)
	{
		ili9341_send_data(i);
		ili9341_send_data(j);
	}

	/*
	for (n = 0; n < ILI9341_PIXEL; n += 8)
	{
		uint ij[] = {i,j, i,j, i, j, i, j, i, j, i, j, i, j, i, j };

		ili9341_send_data_buffer(ij, 16);
	}
	*/
}

void ili9341_draw_pixel(uint16_t x, uint16_t y, uint32_t color) 
{
	if (x < 0 || x >= settings.Width || y < 0 || y >= settings.Height)
		return;

	ili9341_set_cursor(x, y, x, y);
	ili9341_send_command(ILI9341_GRAM);
	int ij[] = {(color >> 8), (color & 0xFF)};
	ili9341_send_data_buffer(ij, 2);
}
