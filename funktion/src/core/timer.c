#include "timer.h"
#include "gpio.h"
#include "clock.h"

#define TIMER_SUPPORTED_INSTANCES 8

static TimerInstance* Instances[TIMER_SUPPORTED_INSTANCES];

void timer_init(TimerInstance* instance, TIMConfig control)
{
	int mapping = 0;

	// Turn the clock on & get the mapping for the instance
	if (instance->TIMX == TIM1){
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
		mapping = 1;
	}else if (instance->TIMX == TIM2){
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
		mapping = 2;
	}else if (instance->TIMX == TIM3){
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
		mapping = 3;
	}else if (instance->TIMX == TIM4){
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
		mapping = 4;
	}else if (instance->TIMX == TIM5){
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
		mapping = 5;
	}else if (instance->TIMX == TIM6){
		RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
		mapping = 6;
	}else if (instance->TIMX == TIM7){
		RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
		mapping = 7;
	}else if (instance->TIMX == TIM8){
		RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
		mapping = 8;
	}else if (instance->TIMX == TIM9){
		RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
		mapping = 9;
	}else if (instance->TIMX == TIM10){
		RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
		mapping = 10;
	}else if (instance->TIMX == TIM11){
		RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
		mapping = 11;
	}else if (instance->TIMX == TIM12){
		RCC->APB1ENR |= RCC_APB1ENR_TIM12EN;
		mapping = 12;
	}else if (instance->TIMX == TIM13){
		RCC->APB1ENR |= RCC_APB1ENR_TIM13EN;
		mapping = 13;
	}else if (instance->TIMX == TIM14){
		RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
		mapping = 14;
	}

	// Ensure that the mapping is supported and its not already initialised
	if (!mapping || mapping > TIMER_SUPPORTED_INSTANCES)
		throw(InitialisationException, "The timer specified is not supported");

	// If already initialised, deinitialise it to allow for initialisation
	if (Instances[mapping-1] != NULL)
		timer_deinit(Instances[mapping-1]);

	// Set the pointer for the interrupt handlers
	instance->_Mapping = mapping-1;
	Instances[instance->_Mapping] = instance;

	// Initialise the timer
	Regset(&instance->TIMX->CR1, TIM_CR1_UDIS, control.UDIS);
	Regset(&instance->TIMX->CR1, TIM_CR1_URS, control.URS);
	Regset(&instance->TIMX->CR1, TIM_CR1_OPM, control.OPM);
	Regset(&instance->TIMX->CR1, TIM_CR1_ARPE, control.ARPE);
	Regset(&instance->TIMX->CR2, TIM_CR2_MMS_0, control.MMS_0);
	Regset(&instance->TIMX->CR2, TIM_CR2_MMS_1, control.MMS_1);
	Regset(&instance->TIMX->CR2, TIM_CR2_MMS_2, control.MMS_2);
	Regset(&instance->TIMX->DIER, TIM_DIER_UDE, control.UDE);
	Regset(&instance->TIMX->SR, TIM_SR_UIF, control.UIF);

	// Hack:
	NVIC_SetPriority(TIM6_DAC_IRQn, 3);
	NVIC_SetPriority(TIM7_IRQn, 3);
}

void timer_deinit(TimerInstance* instance)
{
	timer_stop(instance);
	instance->TIMX->CR1 = 0;
	instance->TIMX->CR2 = 0;
	instance->TIMX->DIER = 0;
	instance->TIMX->SR = 0;
	instance->OnTimerInterrupt = NULL;
	instance->PeripheralMapping = 0;
	Instances[instance->_Mapping] = NULL;
	instance->_Mapping = 0;

	//TODO: Disable NVIC IRQ
	//TODO: Disable CLK
}

void timer_start(TimerInstance* instance)
{
	Regset(&instance->TIMX->CR1, TIM_CR1_CEN, 1);
}

void timer_stop(TimerInstance* instance)
{
	Regset(&instance->TIMX->CR1, TIM_CR1_CEN, 0);
}

static IRQn_Type timer_get_irqn(TimerInstance* instance)
{
	if (instance->TIMX == TIM1)
		return TIM1_UP_TIM10_IRQn;
	else if (instance->TIMX == TIM2)
		return TIM2_IRQn;
	else if (instance->TIMX ==TIM3)
		return TIM3_IRQn;
	else if (instance->TIMX ==TIM4)
		return TIM4_IRQn;
	else if (instance->TIMX ==TIM5)
		return TIM5_IRQn;
	else if (instance->TIMX ==TIM6)
		return TIM6_DAC_IRQn;
	else if (instance->TIMX ==TIM7)
		return TIM7_IRQn;
	else if (instance->TIMX ==TIM8)
		return TIM8_UP_TIM13_IRQn;
	else if (instance->TIMX ==TIM9)
		return TIM1_BRK_TIM9_IRQn;
	else if (instance->TIMX ==TIM10)
		return TIM1_UP_TIM10_IRQn;
	else if (instance->TIMX ==TIM11)
		return TIM1_TRG_COM_TIM11_IRQn;
	else if (instance->TIMX ==TIM12)
		return TIM8_BRK_TIM12_IRQn;
	else if (instance->TIMX ==TIM13)
		return TIM8_UP_TIM13_IRQn;
	else if (instance->TIMX ==TIM14)
		return TIM8_TRG_COM_TIM14_IRQn;

	throw(InvalidOperationException, "Unsupported Timer detected. Cannot find ISQ");
	return -1;
}

void timer_overflow_interrupt_configure(TimerInstance* instance, TimerFunctionalState state, int priority)
{
	IRQn_Type type = timer_get_irqn(instance);

	if (state != TimerFunctionalState_Enabled)
	{
		Regset(&instance->TIMX->DIER, TIM_DIER_UIE, 1);
		NVIC_SetPriority(type, priority);
		NVIC_EnableIRQ(type);
	}
	else
	{
		Regset(&instance->TIMX->DIER, TIM_DIER_UIE, 0);
		NVIC_DisableIRQ(type);
	}
}

void timer_overflow_interrupt_period(TimerInstance* instance, int period)
{
	uint32_t apbclock,tempreg;
	ClockDetails details = clock_details();
	if ((instance->TIMX == TIM1)||(instance->TIMX == TIM8)||(instance->TIMX == TIM9)||(instance->TIMX == TIM10)||(instance->TIMX == TIM11))
		apbclock = details.APB2Frequency;
	else apbclock = details.APB1Frequency;
	if ((period <= 1456) && (apbclock > 1E6)){
		tempreg = ( period * (apbclock/1E6) ) - 1;
		instance->TIMX->ARR = tempreg;
		instance->TIMX->PSC = 1;
	}
	//&instance->TIMX->ARR = 0;
	//&instance->TIMX->PSC = 0;
}


/*Timer Interrupts*/

void TIM1_IRQHandler(void)
{
}

void TIM2_IRQHandler(void)
{
}

void TIM3_IRQHandler(void)
{
}

void TIM4_IRQHandler(void)
{
}

void TIM5_IRQHandler(void)
{
}

void TIM6_DAC_IRQHandler(void)
{
	if (TIM6->SR != 0)
	{
		TimerInstance* instance = Instances[6-1];
		if (instance->OnTimerInterrupt != NULL)
			instance->OnTimerInterrupt(instance);
		TIM6->SR = 0;
	}
}

void TIM7_IRQHandler(void)
{
}

void TIM8_IRQHandler(void)
{
}
