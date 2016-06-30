#ifndef EXTI_C_H
#define EXTI_C_H

#include "definitions.h"

typedef enum
{
	EXTITrigger_Falling,
	EXTITrigger_Rising,
	EXTITrigger_FallingRising,
}EXTITrigger;


typedef struct EXTIInstance
{
	uint NVICPriority;
	GPIO_TypeDef* GPIOX;
	uint Pin;
	EXTITrigger Trigger;
	void (*OnExternalInterrupt[5])(struct EXTIInstance* instance);
	uint Mapping;
}EXTIInstance;

void exti_init(EXTIInstance* instance);

#endif
