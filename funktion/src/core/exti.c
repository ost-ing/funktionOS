#include "exti.h"
#include "gpio.h"

#define EXTI_INSTANCE_COUNT 7

static EXTIInstance* Instances[EXTI_INSTANCE_COUNT] = {0};

void exti_init(EXTIInstance* instance)
{
	uint pin = instance->Pin;

//	if (EXTI->IMR & pin || EXTI->EMR & pin)
//		throw_exception("GPIO EXTI already in use", 0, CallbackException);

	IRQn_Type irq;
	uint instanceMap = pin;

	if (pin == 0)
		irq = EXTI0_IRQn;
	else if (pin == 1)
		irq = EXTI1_IRQn;
	else if (pin == 2)
		irq = EXTI2_IRQn;
	else if (pin == 3)
		irq = EXTI3_IRQn;
	else if (pin == 4)
		irq = EXTI4_IRQn;
	else if (pin >= 5 && pin <= 9){
		irq = EXTI9_5_IRQn;
		instanceMap = 5;
	}
	else if (pin >= 10 && pin <= 15){
		irq = EXTI15_10_IRQn;
		instanceMap = 6;
	}
	else
		throw(InvalidOperationException, "GPIO EXTI invalid pin");

	if (Instances[instanceMap] != NULL)
		throw(AlreadyInitException, "GPIO EXTI already initialised");

	Instances[instanceMap] = instance;

	/* Initialize the GPIO pin */

	GPIOResistor resistor = 0;
	if (instance->Trigger == EXTITrigger_Falling)
		resistor = GPIOResistor_PullUp;
	else if (instance->Trigger == EXTITrigger_Rising)
		resistor = GPIOResister_PullDown;
	else
		resistor = GPIOResistor_NoPull;

	gpio_init(instance->GPIOX, GPIOMode_Input, GPIOOutputType_PushPull, GPIOSpeed_2MHz, resistor, instance->Pin);

	/* Enable SYSCFG clock */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* Connect proper GPIO to SYSCFG */
	uchar portsource = gpio_port_source(instance->GPIOX);
	uchar pinsource = pin;// gpio_pin_source(pin);
	uint32_t tmp = 0x00;
	tmp = ((uint32_t)0x0F) << (0x04 * (pinsource & (uint8_t)0x03));
	SYSCFG->EXTICR[pinsource >> 2] &= ~tmp;
	SYSCFG->EXTICR[pinsource >> 2] |= (((uint32_t)portsource) << (0x04 * (pinsource & (uint8_t)0x03)));

	/* Clear first */
	EXTI->IMR &= ~ (1 << pin);
	EXTI->EMR &= ~ (1 << pin);

	/* Select interrupt mode */
	EXTI->IMR |= (1 << pin);

	/* Clear first */
	EXTI->FTSR &= ~(1 << pin);
	EXTI->RTSR &= ~(1 << pin);

	/* Select edge */
	if (instance->Trigger == EXTITrigger_Falling)
		EXTI->FTSR |= (1 << pin);
    else if (instance->Trigger == EXTITrigger_Rising)
		EXTI->RTSR |= (1 << pin);
	else {
		EXTI->FTSR |= (1 << pin);
		EXTI->RTSR |= (1 << pin);
	}

	/* Add to NVIC */
	NVIC_SetPriority(irq, instance->NVICPriority);
	NVIC_EnableIRQ(irq);
}

void exti_deinit(EXTIInstance* instance)
{
	uint pin = instance->Pin;
	uint instanceMap = pin;
	uint irq = 0;

	if (pin == 0)
		irq = EXTI0_IRQn;
	else if (pin == 1)
		irq = EXTI1_IRQn;
	else if (pin == 2)
		irq = EXTI2_IRQn;
	else if (pin == 3)
		irq = EXTI3_IRQn;
	else if (pin == 4)
		irq = EXTI4_IRQn;
	else if (pin >= 5 && pin <= 9){
		irq = EXTI9_5_IRQn;
		instanceMap = 5;
	}
	else if (pin >= 10 && pin <= 15){
		irq = EXTI15_10_IRQn;
		instanceMap = 6;
	}
	else
		throw(InvalidOperationException, "GPIO EXTI deinit invalid pin");

	if (Instances[instanceMap] == NULL)
		throw(InitialisationException, "EXTI not initialised");

	EXTI->IMR &= ~(1 << pin);
	EXTI->EMR &= ~(1 << pin);
	EXTI->FTSR &= ~(1 << pin);
	EXTI->RTSR &= ~(1 << pin);

	NVIC_DisableIRQ(irq);

	Instances[instanceMap] = NULL;
}

/* IRQ Handlers */

void EXTI0_IRQHandler(void)
{
	if (EXTI->PR & (EXTI_PR_PR0))
	{
		EXTI->PR = EXTI_PR_PR0;
		Instances[0]->OnExternalInterrupt[0](Instances[0]);
	}
}

void EXTI1_IRQHandler(void)
{
	if (EXTI->PR & (EXTI_PR_PR1))
	{
		EXTI->PR = EXTI_PR_PR1;
		Instances[1]->OnExternalInterrupt[0](Instances[1]);
	}
}

void EXTI2_IRQHandler(void)
{
	if (EXTI->PR & (EXTI_PR_PR2))
	{
		EXTI->PR = EXTI_PR_PR2;
		Instances[2]->OnExternalInterrupt[0](Instances[2]);
	}
}

void EXTI3_IRQHandler(void)
{
	if (EXTI->PR & (EXTI_PR_PR3))
	{
		EXTI->PR = EXTI_PR_PR3;
		Instances[3]->OnExternalInterrupt[0](Instances[3]);
	}
}

void EXTI4_IRQHandler(void)
{
	if (EXTI->PR & (EXTI_PR_PR4))
	{
		EXTI->PR = EXTI_PR_PR4;
		Instances[4]->OnExternalInterrupt[0](Instances[4]);
	}
}

void EXTI9_5_IRQHandler(void)
{
	EXTIInstance* instance = Instances[5];

	if (EXTI->PR & (EXTI_PR_PR5))
	{
		EXTI->PR = EXTI_PR_PR5;
		if (instance->OnExternalInterrupt[0] != NULL)
			instance->OnExternalInterrupt[0](Instances[5]);
	}
	if (EXTI->PR & (EXTI_PR_PR6))
	{
		EXTI->PR = EXTI_PR_PR6;
		if (instance->OnExternalInterrupt[1] != NULL)
			instance->OnExternalInterrupt[1](Instances[5]);
	}
	if (EXTI->PR & (EXTI_PR_PR7))
	{
		EXTI->PR = EXTI_PR_PR7;
		if (instance->OnExternalInterrupt[2] != NULL)
			instance->OnExternalInterrupt[2](Instances[5]);
	}
	if (EXTI->PR & (EXTI_PR_PR8))
	{
		EXTI->PR = EXTI_PR_PR8;
		if (instance->OnExternalInterrupt[3] != NULL)
			instance->OnExternalInterrupt[3](Instances[5]);
	}
	if (EXTI->PR & (EXTI_PR_PR9))
	{
		EXTI->PR = EXTI_PR_PR9;
		if (instance->OnExternalInterrupt[4] != NULL)
			instance->OnExternalInterrupt[4](Instances[5]);
	}
}

void EXTI15_10_IRQHandler(void)
{
	EXTIInstance* instance = Instances[6];

	if (EXTI->PR & (EXTI_PR_PR10))
	{
		EXTI->PR = EXTI_PR_PR10;
		if (instance->OnExternalInterrupt[0] != NULL)
			instance->OnExternalInterrupt[0](Instances[6]);
	}
	if (EXTI->PR & (EXTI_PR_PR11))
	{
		EXTI->PR = EXTI_PR_PR11;
		if (instance->OnExternalInterrupt[1] != NULL)
			instance->OnExternalInterrupt[1](Instances[5]);
	}
	if (EXTI->PR & (EXTI_PR_PR12))
	{
		EXTI->PR = EXTI_PR_PR12;
		if (instance->OnExternalInterrupt[2] != NULL)
			instance->OnExternalInterrupt[2](Instances[5]);
	}
	if (EXTI->PR & (EXTI_PR_PR13))
	{
		EXTI->PR = EXTI_PR_PR13;
		if (instance->OnExternalInterrupt[3] != NULL)
			instance->OnExternalInterrupt[3](Instances[5]);
	}
	if (EXTI->PR & (EXTI_PR_PR14))
	{
		EXTI->PR = EXTI_PR_PR14;
		if (instance->OnExternalInterrupt[4] != NULL)
			instance->OnExternalInterrupt[4](Instances[5]);
	}
	if (EXTI->PR & (EXTI_PR_PR15))
	{
		EXTI->PR = EXTI_PR_PR15;
		if (instance->OnExternalInterrupt[5] != NULL)
			instance->OnExternalInterrupt[5](Instances[5]);
	}
}



