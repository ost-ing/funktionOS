#include "dma.h"

#define DMA_SUPPORTED_CONTROLLERS 2
#define DMA_SUPPORTED_STREAMS 8

// An array of DMAInstance pointers. Like any pointer a reference must be kept.
// E.g. in the ILI9341 module; a globally declared 'static DMAInstance Instance' can be used and passed into the dma_init
// function to initialize DMA for a given controller/stream. Once initialized the pointer is added to this array
// for interrupt call back handling.
static DMAInstance* InitialisedDMA[DMA_SUPPORTED_CONTROLLERS][DMA_SUPPORTED_STREAMS] = {{0}};

void dma_clock_enable(DMA_TypeDef* DMAx, bool enable)
{
	if (DMAx == DMA1)
		//Regset(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN, enable);
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	else if (DMAx == DMA2)
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	//	Regset(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN, enable);
}

void dma_set_address(DMAInstance *instance)
{
	// Configure the Input/Output addresses
	instance->Stream->PAR  = (__IO uint32_t)instance->PeriphBaseAddr;
	instance->Stream->M0AR = (__IO uint32_t)instance->Memory0BaseAddr;
	instance->Stream->M1AR = (__IO uint32_t)instance->Memory1BaseAddr;
	instance->Stream->NDTR = (__IO uint32_t)instance->Count;
}

void dma_set_count(DMAInstance *instance)
{
	instance->Stream->NDTR = instance->Count;
}

void dma_init(DMAInstance* instance)
{
	DMA_TypeDef* DMAx = instance->DMAx;
	DMA_Stream_TypeDef* stream = instance->Stream;
	DMAConfig config = instance->InitConfig;

	// Add this instance to the array
	InitialisedDMA[instance->ControllerIndex-1][instance->StreamIndex] = instance;

	// Enable clock to the DMA Controller
	dma_clock_enable(DMAx, true);

	/*
	 * Ref. DM00031020 p.321
	 * Note: The following Stream initialization procedure is outlined.
	 * The order of the procedure matters.
	*/

	// Clear enable bit, wait for it to clear
	dma_enable(instance, false);
	while(stream->CR & DMA_SxCR_EN);

	// Set the address for the Periph & Memory
	dma_set_address(instance);

	// Configure the channel of the DMA Stream
	Regset(&stream->CR, DMA_SxCR_CHSEL_0, (config.Channel & 0x01));
	Regset(&stream->CR, DMA_SxCR_CHSEL_1, (config.Channel & 0x02));

	// Configure the Flow Controller
	Regset(&stream->CR, DMA_SxCR_PFCTRL, config.PeriphFlow);

	// Configure Stream Priority
	Regset(&stream->CR, DMA_SxCR_PL_0, (config.Priority & 0x01));
	Regset(&stream->CR, DMA_SxCR_PL_1, (config.Priority & 0x02));

	// Configure FIFO Control Register
	Regset(&stream->FCR, DMA_SxFCR_FTH_0, (config.FifoThreshold & 0x01));
	Regset(&stream->FCR, DMA_SxFCR_FTH_1, (config.FifoThreshold & 0x02));
	Regset(&stream->FCR, DMA_SxFCR_DMDIS, config.DirectModeDisabled);
	Regset(&stream->FCR, DMA_SxFCR_FEIE,  config.FifoErrorInterrupt);

	// Configure Direction
	Regset(&stream->CR, DMA_SxCR_DIR_0, (config.Direction & 0x01));
	Regset(&stream->CR, DMA_SxCR_DIR_1, (config.Direction & 0x02));

	// Configure Burst
	Regset(&stream->CR, DMA_SxCR_PBURST_0, (config.PeriphBurstSize & 0x01));
	Regset(&stream->CR, DMA_SxCR_PBURST_1, (config.PeriphBurstSize & 0x02));
	Regset(&stream->CR, DMA_SxCR_MBURST_0, (config.MemoryBurstSize & 0x01));
	Regset(&stream->CR, DMA_SxCR_MBURST_1, (config.MemoryBurstSize & 0x02));

	// Configure Memory address incrementing
	Regset(&stream->CR, DMA_SxCR_PINC, config.PeriphIncrement);
	Regset(&stream->CR, DMA_SxCR_MINC, config.MemoryIncrement);

	// Configure Data Widths, Circular and double buffering
	Regset(&stream->CR, DMA_SxCR_PSIZE_0, (config.PeriphDataSize & 0x01));
	Regset(&stream->CR, DMA_SxCR_PSIZE_1, (config.PeriphDataSize & 0x02));
	Regset(&stream->CR, DMA_SxCR_MSIZE_0, (config.MemoryDataSize & 0x01));
	Regset(&stream->CR, DMA_SxCR_MSIZE_1, (config.MemoryDataSize & 0x02));

	Regset(&stream->CR, DMA_SxCR_PINCOS, config.PeriphIncrementOffset);
	Regset(&stream->CR, DMA_SxCR_CIRC, config.CircularMode);
	Regset(&stream->CR, DMA_SxCR_DBM, config.DoubleBufferMode);
	Regset(&stream->CR, DMA_SxCR_CT, config.DoubleCurrentTarget);

	// Configure DMA transfer interrupts
	Regset(&stream->CR, DMA_SxCR_DMEIE, config.DirectErrorInterrupt);
	Regset(&stream->CR, DMA_SxCR_TEIE,  config.TransferErrorInterrupt);
	Regset(&stream->CR, DMA_SxCR_HTIE,  config.HalfTransferInterrupt);
	Regset(&stream->CR, DMA_SxCR_TCIE,  config.TransferCompleteInterrupt);

	// Configure NVIC
}

void dma_enable(DMAInstance *instance, bool enable)
{
	// Ref. DM00031020 p.321
	// Note: DMA must be disabled before the peripheral is disabled

	Regset(&instance->Stream->CR, DMA_SxCR_EN, enable);
}

uint dma_get_count(DMAInstance *instance)
{
	return instance->Stream->NDTR;
}


/* IRQ Handlers */

typedef struct { uint TCIF; uint HTIF; uint TEIF; uint DMEIF; uint FEIF; } DMAInterruptFlags;

static void dma_handle_interrupt(DMAInstance* instance, DMAInterruptFlags statusFlags, DMAInterruptFlags clearFlags)
{
	// Get the status register, Stream 0-3 uses LISR, Stream 4-7 uses HISR
	volatile uint32_t* statusRegister = NULL;
	volatile uint32_t* clearRegister = NULL;

/*	if (instance->StreamIndex < 4)
	{
		statusRegister = &instance->DMAx->LISR;
		clearRegister = &instance->DMAx->LIFCR;
	}
	else
	{
		statusRegister = &instance->DMAx->HISR;
		clearRegister = &instance->DMAx->HIFCR;
	}
*/
	statusRegister = &DMA2->HISR;
	clearRegister = &DMA2->HIFCR;

	// Mask out the status register, attaining which interrupt fired
	bool isTransferComplete = *statusRegister & statusFlags.TCIF;
	bool isHalfTransfer     = *statusRegister & statusFlags.HTIF;
	bool isTransferError    = *statusRegister & statusFlags.TEIF;
	bool isDirectError      = *statusRegister & statusFlags.DMEIF;
	bool isFifoError		= *statusRegister & statusFlags.FEIF;

	// Clear the interrupt flags
	if (isTransferComplete)
		*clearRegister |= clearFlags.TCIF;
	if (isHalfTransfer)
		*clearRegister |= clearFlags.HTIF;
	if (isTransferError)
		*clearRegister |= clearFlags.TEIF;
	if (isDirectError)
		*clearRegister |= clearFlags.DMEIF;
	if (isFifoError)
		*clearRegister |= clearFlags.FEIF;

	// Call back to the invoking module using this DMA Channel
	if (isTransferComplete && instance->OnTransferComplete)
		instance->OnTransferComplete();
	if (isHalfTransfer && instance->OnHalfTransferComplete)
		instance->OnHalfTransferComplete();
	if (isTransferError && instance->OnTransferError)
		instance->OnTransferError();
	if (isDirectError && instance->OnDirectError)
		instance->OnDirectError();
	if (isFifoError && instance->OnFifoError)
		instance->OnFifoError();
}

/* DMA1 IRQ Handlers */

void DMA1_Stream0_IRQHandler(void)
{
	DMAInstance* instance = InitialisedDMA[0][0];
	DMAInterruptFlags statusFlags = { DMA_LISR_TCIF0, DMA_LISR_HTIF0, DMA_LISR_TEIF0, DMA_LISR_DMEIF0, DMA_LISR_FEIF0 };
	DMAInterruptFlags clearFlags  = { DMA_LIFCR_CTCIF0, DMA_LIFCR_CHTIF0, DMA_LIFCR_CTEIF0, DMA_LIFCR_CDMEIF0, DMA_LIFCR_CFEIF0 };
	dma_handle_interrupt(instance, statusFlags, clearFlags);
}

void DMA1_Stream1_IRQHandler(void)
{
	DMAInstance* instance = InitialisedDMA[0][1];
	DMAInterruptFlags statusFlags = { DMA_LISR_TCIF1, DMA_LISR_HTIF1, DMA_LISR_TEIF1, DMA_LISR_DMEIF1, DMA_LISR_FEIF1 };
	DMAInterruptFlags clearFlags  = { DMA_LIFCR_CTCIF1, DMA_LIFCR_CHTIF1, DMA_LIFCR_CTEIF1, DMA_LIFCR_CDMEIF1, DMA_LIFCR_CFEIF1 };
	dma_handle_interrupt(instance, statusFlags, clearFlags);
}

void DMA1_Stream2_IRQHandler(void)
{
	DMAInstance* instance = InitialisedDMA[0][2];
	DMAInterruptFlags statusFlags = { DMA_LISR_TCIF2, DMA_LISR_HTIF2, DMA_LISR_TEIF2, DMA_LISR_DMEIF2, DMA_LISR_FEIF2 };
	DMAInterruptFlags clearFlags  = { DMA_LIFCR_CTCIF2, DMA_LIFCR_CHTIF2, DMA_LIFCR_CTEIF2, DMA_LIFCR_CDMEIF2, DMA_LIFCR_CFEIF2 };
	dma_handle_interrupt(instance, statusFlags, clearFlags);
}

void DMA1_Stream3_IRQHandler(void)
{
	DMAInstance* instance = InitialisedDMA[0][3];
	DMAInterruptFlags statusFlags = { DMA_LISR_TCIF3, DMA_LISR_HTIF3, DMA_LISR_TEIF3, DMA_LISR_DMEIF3, DMA_LISR_FEIF3 };
	DMAInterruptFlags clearFlags  = { DMA_LIFCR_CTCIF3, DMA_LIFCR_CHTIF3, DMA_LIFCR_CTEIF3, DMA_LIFCR_CDMEIF3, DMA_LIFCR_CFEIF3 };
	dma_handle_interrupt(instance, statusFlags, clearFlags);
}

void DMA1_Stream4_IRQHandler(void)
{
	DMAInstance* instance = InitialisedDMA[0][4];
	DMAInterruptFlags statusFlags = { DMA_HISR_TCIF4, DMA_HISR_HTIF4, DMA_HISR_TEIF4, DMA_HISR_DMEIF4, DMA_HISR_FEIF4 };
	DMAInterruptFlags clearFlags  = { DMA_HIFCR_CTCIF4, DMA_HIFCR_CHTIF4, DMA_HIFCR_CTEIF4, DMA_HIFCR_CDMEIF4, DMA_HIFCR_CFEIF4 };
	dma_handle_interrupt(instance, statusFlags, clearFlags);
}

void DMA1_Stream5_IRQHandler(void)
{
	DMAInstance* instance = InitialisedDMA[0][5];
	DMAInterruptFlags statusFlags = { DMA_HISR_TCIF5, DMA_HISR_HTIF5, DMA_HISR_TEIF5, DMA_HISR_DMEIF5, DMA_HISR_FEIF5 };
	DMAInterruptFlags clearFlags  = { DMA_HIFCR_CTCIF5, DMA_HIFCR_CHTIF5, DMA_HIFCR_CTEIF5, DMA_HIFCR_CDMEIF5, DMA_HIFCR_CFEIF5 };
	dma_handle_interrupt(instance, statusFlags, clearFlags);
}

void DMA1_Stream6_IRQHandler(void)
{
	DMAInstance* instance = InitialisedDMA[0][6];
	DMAInterruptFlags statusFlags = { DMA_HISR_TCIF6, DMA_HISR_HTIF6, DMA_HISR_TEIF6, DMA_HISR_DMEIF6, DMA_HISR_FEIF6 };
	DMAInterruptFlags clearFlags  = { DMA_HIFCR_CTCIF6, DMA_HIFCR_CHTIF6, DMA_HIFCR_CTEIF6, DMA_HIFCR_CDMEIF6, DMA_HIFCR_CFEIF6 };
	dma_handle_interrupt(instance, statusFlags, clearFlags);
}

void DMA1_Stream7_IRQHandler(void)
{
	DMAInstance* instance = InitialisedDMA[0][7];
	DMAInterruptFlags statusFlags = { DMA_HISR_TCIF7, DMA_HISR_HTIF7, DMA_HISR_TEIF7, DMA_HISR_DMEIF7, DMA_HISR_FEIF7 };
	DMAInterruptFlags clearFlags  = { DMA_HIFCR_CTCIF7, DMA_HIFCR_CHTIF7, DMA_HIFCR_CTEIF7, DMA_HIFCR_CDMEIF7, DMA_HIFCR_CFEIF7 };
	dma_handle_interrupt(instance, statusFlags, clearFlags);
}


/* DMA1 IRQ Handlers */
void DMA2_Stream0_IRQHandler(void)
{

}

void DMA2_Stream1_IRQHandler(void)
{

}

void DMA2_Stream2_IRQHandler(void)
{

}

void DMA2_Stream3_IRQHandler(void)
{

}

void DMA2_Stream4_IRQHandler(void)
{
	DMAInstance* instance = InitialisedDMA[1][4];
	DMAInterruptFlags statusFlags = { DMA_HISR_TCIF4, DMA_HISR_HTIF4, DMA_HISR_TEIF4, DMA_HISR_DMEIF4, DMA_HISR_FEIF4 };
	DMAInterruptFlags clearFlags  = { DMA_HIFCR_CTCIF4, DMA_HIFCR_CHTIF4, DMA_HIFCR_CTEIF4, DMA_HIFCR_CDMEIF4, DMA_HIFCR_CFEIF4 };
	dma_handle_interrupt(instance, statusFlags, clearFlags);
}

void DMA2_Stream5_IRQHandler(void)
{

}

void DMA2_Stream6_IRQHandler(void)
{

}

void DMA2_Stream7_IRQHandler(void)
{

}
