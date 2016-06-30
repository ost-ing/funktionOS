#ifndef DMA_C_H
#define DMA_C_H

#include "definitions.h"

typedef enum
{
	DMADirection_PeriphToMemory = 0x00,
	DMADirection_MemoryToPeriph = 0x01,
	DMADirection_MemoryToMemory = 0x02
}DMADirection;

typedef enum
{
	DMADataSize_Byte  = 0x00,
	DMADataSize_Short = 0x01,
	DMADataSize_Word  = 0x02
}DMADataSize;

typedef enum
{
	DMAPriority_Low 	= 0x00,
	DMAPriority_Medium  = 0x01,
	DMAPriority_High	= 0x02,
	DMAPriority_VHigh 	= 0x03
}DMAPriority;

typedef enum
{
	DMABurstSize_INCR1  = 0x00,
	DMABurstSize_INCR4  = 0x01,
	DMABurstSize_INCR8  = 0x02,
	DMABurstSize_INCR16 = 0x03,
}DMABurstSize;

typedef enum
{
	DMAFifoThreshold_Quater       = 0x00,
	DMAFifoThreshold_Half         = 0x01,
	DMAFifoThreshold_ThreeQuater  = 0x02,
	DMAFifoThreshold_Full		  = 0x03,
}DMAFifoThreshold;

typedef enum
{
	DMAChannel_0 = 0x00,
	DMAChannel_1 = 0x01,
	DMAChannel_2 = 0x02,
	DMAChannel_3 = 0x03,
	DMAChannel_4 = 0x04,
	DMAChannel_5 = 0x05,
	DMAChannel_6 = 0x06,
	DMAChannel_7 = 0x07,
}DMAChannel;

/*
typedef enum
{
	DMAInterrupt_TransferComplete		= DMA_SxCR_TCIE,
	DMAInterrupt_HalfTransferComplete	= DMA_SxCR_HTIE,
	DMAInterrupt_TransferError			= DMA_SxCR_TEIE,
	DMAInterrupt_DirectError			= DMA_IT_DME,
	DMAInterrupt_FIFOError				= (uint32_t)0x00000080,
}DMAInterrupt;
*/

typedef struct
{
	/* DMA_SxCR */
	bool PeriphFlow;
	bool PeriphIncrementOffset;
	bool PeriphIncrement;
	bool MemoryIncrement;
	bool CircularMode;
	bool DoubleBufferMode;
	bool DoubleCurrentTarget;

	bool HalfTransferInterrupt;
	bool TransferCompleteInterrupt;
	bool TransferErrorInterrupt;
	bool ReceiveCompleteInterrupt;
	bool OverUnderRunInterrupt;     // FIFO Error interrupt
	bool DirectErrorInterrupt;		// Direct Error interrupt

	DMAChannel Channel;
	DMAPriority Priority;
	DMADirection Direction;
	DMADataSize PeriphDataSize;
	DMADataSize MemoryDataSize;
	DMABurstSize MemoryBurstSize;
	DMABurstSize PeriphBurstSize;

	/*DMA FIFO*/
	DMAFifoThreshold FifoThreshold;
	bool DirectModeDisabled;
	bool FifoErrorInterrupt;
}DMAConfig;

typedef struct
{
	/* Mapping Indices */
	uint ControllerIndex;
	uint StreamIndex;

	DMA_TypeDef* DMAx;			// DMA Controller
	DMA_Stream_TypeDef* Stream;	// DMA Stream
	DMAConfig InitConfig;		//

	/* Address loading */
	uint Count;
	uint* PeriphBaseAddr;
	uint* Memory0BaseAddr;
	uint* Memory1BaseAddr;

	void (*OnHalfTransferComplete)(void);
	void (*OnTransferComplete)(void);
	void (*OnTransferError)(void);
	void (*OnReceiveComplete)(void);
	void (*OnFifoError)(void);
	void (*OnDirectError)(void);
}DMAInstance;

void dma_clock_enable(DMA_TypeDef* DMAx, bool enable);
void dma_init(DMAInstance* instance);
void dma_enable(DMAInstance *instance, bool enable);
uint dma_get_count(DMAInstance *instance);
void dma_write_buffer(DMAInstance *instance, uint* buffer, uint count);
void dma_set_address(DMAInstance *instance);
void dma_set_count(DMAInstance *instance);
#endif
