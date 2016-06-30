#include "../core/core.h"
#include "../peripherals/AD9833.h"
#include "../peripherals/pulse_generator.h"
#include "input_manager.h"

volatile int LedState = 0;
volatile int timerintcounter = 0;

void flash_light_test(void);
void flash_light(TimerInstance* instance);
void pulse_test(void);

static SerialInstance Serial = {
	.Baudrate = 19200,
	.GPIOX    = GPIOB,
    .USARTX   = USART3,
    .TxPin    = 10,
    .RxPin    = 11,
    .Priority = 0,
};

static TimerInstance Timer = {
	.TIMX = TIM6,
	.OnTimerInterrupt = NULL,
};

static PulseInstance Pulse = {
	.GPIOX       = GPIOG,
	.Pin         = 8,
	.SequenceMap = {0},
	.Timer       = &Timer,
};

int main(void)
{
    try
	{
		// Configure the clock
		clock_configure();

		// Initialise the Serial
		serial_init(&Serial, true);
		serial_input_handler(&input_manager_process);
		serial_println("Console initialised");

		//throw(RuntimeException, "BLAH");

		// Initialise the AD9833
	/*	ad9833_init(&Timer);
		ad9833_changefunction(AD9833Waveform_Sine);
		ad9833_changefrequency(100);
		ad9833_update();
		serial_println("AD9833 initialised");
*/
		// Initialise the Pulse Generator -- under construction.
		pulse_generator_init(&Pulse);
		serial_println("Pulse Generator initialised");

		// Flash light test -- works :D
		//flash_light_test();

		// Pulse generator test -- under construction.
		//pulse_test();
	}
	catch(RuntimeException)
	{
		serial_println(E4C_EXCEPTION.message);
	}

	while (1);
}

/////////////////////////////////////////////////////////////////

uchar pin=13;
uint count = 0;
void flash_light_test(void)
{
	/*Timer Test*/
	GPIO_TypeDef *GPIOX = GPIOG;

	gpio_init(GPIOX, GPIOMode_Output, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_NoPull, pin);

	TIMConfig TimControl = {0};
	TimControl.ARPE = 1;	//Auto reload pre-load enabled
	TimControl.OPM = 0;		//One pulse mode is disabled
	TimControl.URS = 0;		//Generate interrupt&DMA only when overflow happens
	TimControl.UDIS = 0;	//Update enable
	TimControl.MMS_0 = 0;	//Not using the master mode
	TimControl.MMS_1 = 0;
	TimControl.MMS_2 = 0;
	TimControl.UDE = 0;		//DMA request disabled
	TimControl.UIE = 1;		//Interrupt enbaled
	TimControl.UG = 0;		//No update generation

	// Set the callback for the timer
	Timer.OnTimerInterrupt = &flash_light;

	timer_init(&Timer, TimControl);
	timer_overflow_interrupt_period(&Timer, 7); //this function take in microseconds and max is 1456us at the moment.
	timer_overflow_interrupt_configure(&Timer, TimerFunctionalState_Disabled, 1);
	timer_start(&Timer);

	//Put the scope on PG13 and you will be able to see the generated waves.
}

void flash_light(TimerInstance* instance)
{
	// ISR callback
	GPIOG->ODR ^= 1 << pin;

	// Test that it stops
	if (count++ > 1000000){
		timer_stop(instance);
		count = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void pulse_test(void)
{
	bool states[10] = { 0, 1, 0,  1,  0,  1,  0,   1,   0 };
	uint micros[10] = {12,25,50,100,200,400,800,1000,1200 };

	pulse_generator_configure(&Pulse, 9, states, micros);

	// dont run here, we run from the console application
	//pulse_generator_run(&Pulse);
}

