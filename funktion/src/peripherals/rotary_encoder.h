#ifndef ROTARY_ENCODER_C_H
#define ROTARY_ENCODER_C_H

#include "../core/core.h"

typedef struct
{
	/* Configuration Variables */
	GPIO_TypeDef* SignalAGPIOX;
	GPIO_TypeDef* SignalBGPIOX;
	GPIO_TypeDef* ButtonGPIOX;
	uint SignalBPin;
	uint SignalAPin;
	uint ButtonPin;
	void (*OnRotaryEncoderAdjust)(void);
	void (*OnRotaryEncoderPressed)(void);
	uint Mapping;

	/* Private Variables */
	EXTIInstance _SignalEXTI;
	EXTIInstance _ButtonEXTI;
	ulong _DebounceTime;

	/* State Variables */
	uint Count;
	uint Delta;
	uint Absolute;
	uint LastA;
	int Value;
	bool Incremented;
}RotaryEncoder;

void rotary_encoder_init(RotaryEncoder* instance);

#endif
