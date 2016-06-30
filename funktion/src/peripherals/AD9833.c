#include "AD9833.h"
#include "math.h"

// AD9833 constants
#define F_MCLK              25000000          // 25MHz clock
#define FREQ_DIV            268435456         // 2 to the power of 28
#define FREQ_MULTIPLIER	    10.73741824	      // (2^28)/25Mhz
#define FREQ0REG_CONSTANT   0x4000
#define FREQ1REG_CONSTANT   0x8000
#define FREQMAXLSB			16383

// AD9833 control register bit definitions
#define BIT_CB1      15		
#define BIT_CB2      14		
#define BIT_D28      13
#define BIT_HLD      12
#define BIT_FSELECT  11
#define BIT_PSELECT  10
#define BIT_RESET    8
#define BIT_SLEEP1   7
#define BIT_SLEEP12  6
#define BIT_OPBITN   5
#define BIT_DIV2     3
#define BIT_MODE     1

// Forward declarations
extern float roundf(float);
static void frequencysweep_run(TimerInstance* timer);

// Controlling the AD9833 in master mode. 
// Only require MOSI, SCLK and NSS
static SPIInstance SpiInstance = {0};
static SPIControlConfig Control = {0};

static AD9833Waveform Function = 0;
static uint Frequency = 0;
static uint frequencyRegLSB = 0x0000;
static uint frequencyRegMSB = 0x0000;
static uint controlRegister = 0x0000;
static uint controlRegisterReset = 0x0000;

static AD9833FreqSweepState SweepState = {0};
static TimerInstance* Timer = NULL;

void ad9833_init(TimerInstance* timer)
{
	Timer = timer;

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;

	//DM00071990 Page 77 SPI4
	SPIPins pins = {0};
	pins.NSS = 4;
	pins.SCLK = 2;
	pins.MISO = 5;
	pins.MOSI = 6;

	SpiInstance.GPIOX = GPIOE;
	SpiInstance.GPIOX_CS = GPIOE;
	SpiInstance.SPIX = SPI4;
	SpiInstance.Pins = pins;
	SpiInstance.AFLayout = 5; //SPI4 used here
	SpiInstance.ControllerIndex = 1;

	Control.BR_0 = 0;
	Control.BR_1 = 0;
	Control.BR_2 = 0; // 16Mhz/2
	Control.CPOL = 1;
	Control.CPHA = 0;
	Control.LSBFIRST = 0; //MSB first
	Control.BIDIMODE = 1;
	Control.BIDIOE = 1;
	Control.RXONLY = 0;
	Control.DFF = 1;
	Control.MSTR = 1;
	Control.SSOE = 0; //Slave select as output
	Control.SSM = 1; //

	spi_init (&SpiInstance, &Control);

	/* Default waveform */
	ad9833_changefunction(AD9833Waveform_Sine);
	ad9833_changefrequency(1000);
	ad9833_update();
}

void ad9833_update(void)
{
	spi_write_short(&SpiInstance, controlRegisterReset);
	spi_write_short(&SpiInstance, frequencyRegLSB);
	spi_write_short(&SpiInstance, frequencyRegMSB);
	spi_write_short(&SpiInstance, 0xC000);
	spi_write_short(&SpiInstance, controlRegister);
	for (int i = 0; i < 5000; i++);
}

void ad9833_changefunction(AD9833Waveform function)
{
	Function = function;

	switch (Function)
	{
	case AD9833Waveform_Sine:
		controlRegister = 0x0000;
		controlRegisterReset = controlRegister|1<<BIT_D28|1<<BIT_RESET;
		controlRegister = controlRegister|1<<BIT_D28;
		break;
	case AD9833Waveform_Square:
		controlRegister = 0x0000;
		controlRegisterReset = controlRegister|1<<BIT_D28|1<<BIT_OPBITN|1<<BIT_DIV2|1<<BIT_RESET;
		controlRegister = controlRegister|1<<BIT_D28|1<<BIT_OPBITN|1<<BIT_DIV2;
		break;
	case AD9833Waveform_Sawtooth:
		controlRegister = 0x0000;
		controlRegisterReset = controlRegister|1<<BIT_D28|1<<BIT_MODE|1<<BIT_RESET;
		controlRegister = controlRegister|1<<BIT_D28|1<<BIT_MODE;
		break;
	case AD9833Command_Reserved:
		break;
	}
}

void ad9833_changefrequency(uint frequency)
{
	Frequency = frequency;

	uint frequencyRegister = 0;
	float frequencyRegisterF = 0;
	frequencyRegisterF = roundf(Frequency * FREQ_MULTIPLIER);
	frequencyRegister = (uint)frequencyRegisterF;

	if (frequencyRegister > FREQMAXLSB)
	{
		uint LSB=0,MSB=0;
		LSB = frequencyRegister & 0x00003fff;	    //14-bit LSB extracted from the 28-bit frequency data
		MSB = frequencyRegister & 0x0fffC000;	    //14-bit MSB extracted from the 28-bit frequency data
		MSB = MSB >> 14;						    //realigning the bit position for serial data transfer
		frequencyRegLSB = LSB + FREQ0REG_CONSTANT;	//constant selection can be broken into a function when needed
		frequencyRegMSB = MSB + FREQ0REG_CONSTANT;
	}
	else
	{
		frequencyRegLSB = frequencyRegisterF + FREQ0REG_CONSTANT;
		frequencyRegMSB = FREQ0REG_CONSTANT;
	}
}

void ad9833_changephase(int phase)
{

}

void ad9833_stop(void)
{
	ad9833_frequencysweep_stop();
	// TODO: Stop the AD9833 itself from outputting the signal.
}

void ad9833_frequencysweep_stop(void)
{
	// If the Frequency Sweep is active, stop it.
	if (SweepState.IsPlaying)
	{
		// Disable timer and interrupt
		timer_stop(Timer);
		// Clear the sweep state
		SweepState = (const AD9833FreqSweepState){ 0 };
	}
}

/*
 * Frequency sweep will use a timer to periodically change the frequency. Still under construction.
 */
void ad9833_frequencysweep_begin(AD9833FreqSweep mode, uint lowerFreq, uint upperFreq, uint timeStep, uint freqStep)
{
	if (mode == AD9833FreqSweep_Reserved)
		throw(InvalidArgumentException, "Invalid frequency sweep mode for AD9833");

	// Set the Sweep State
	SweepState.Mode = mode;
	SweepState.LowerFreq = lowerFreq;
	SweepState.UpperFreq = upperFreq;
	SweepState.TimeStep = timeStep;
	SweepState.FreqStep = freqStep;
	SweepState.IsPlaying = true;

	// Enable timer and interrupt
	TIMConfig timerSettings = {0};
	timerSettings.ARPE = true;		//Auto reload pre-load enabled
	timerSettings.OPM = false;		//One pulse mode is disabled
	timerSettings.UIE = true;		//Interrupt enabled

	// Begin the timer
	Timer->OnTimerInterrupt = &frequencysweep_run;
	timer_init(Timer, timerSettings);
	timer_overflow_interrupt_period(Timer, timeStep);
	timer_overflow_interrupt_configure(Timer, TimerFunctionalState_Disabled, 3);
	timer_start(Timer);

	/* Timer will begin interrupting to set the output frequency */
}

static void frequencysweep_run(TimerInstance* timer)
{
	if (!SweepState.IsPlaying)
		return;

	if (SweepState.Mode == AD9833FreqSweep_RiseFall)
	{
		if (Frequency >= SweepState.UpperFreq)
			SweepState.Polarity = 0;
		else if (Frequency <= SweepState.LowerFreq)
			SweepState.Polarity = 1;
	}
	else if (SweepState.Mode == AD9833FreqSweep_Rise)
	{
		SweepState.Polarity = 1;
		if (Frequency >= SweepState.UpperFreq)
			Frequency = SweepState.LowerFreq;
	}
	else if (SweepState.Mode == AD9833FreqSweep_Fall)
	{
		SweepState.Polarity = 0;
		if (Frequency <= SweepState.LowerFreq)
			Frequency = SweepState.UpperFreq;
	}

	// Update the frequency
	uint freq = Frequency + (SweepState.Polarity ? SweepState.FreqStep : -1*SweepState.FreqStep);

	// Update the AD9833
	ad9833_changefunction(Function);
	ad9833_changefrequency(freq);
	ad9833_update();

	// Restart the timer, to generate the interrupt again
	timer_overflow_interrupt_period(Timer, SweepState.TimeStep * 1000);
	timer_overflow_interrupt_configure(Timer, TimerFunctionalState_Disabled, 3);
	timer_start(Timer);

	/* Timer will begin interrupting to set the output frequency */
}


// The following is a SysInput handler - This method gets called by an input manager when data is parsed via serial comms.
void ad9833_input_handler(void* sender, SysInput input)
{
	if (input.Type != SysInput_Serial)
		throw(InvalidOperationException, "Unknown or unsupported System input method for AD9833");

	// A pointer to the instance of serial port that sent the data ;)
	// SerialInstance* serial = (SerialInstance*)sender;

	// The subcommand tells the AD9833 what to do
	AD9833Command command = (AD9833Command) input.Subcommand;

	if (command == AD9833Command_Waveform)
	{
		// Change waveform
		if (input.Length != sizeof(AD9833Waveform))
			throw(IndexOutOfRangeException, "Invalid data length for changing the waveform for AD9833");

		AD9833Waveform waveform = (AD9833Waveform) input.Dataframe[0];

		if (waveform == AD9833Waveform_Reserved)
			throw(InvalidOperationException, "Invalid waveform specified for AD9833");

		ad9833_changefunction(waveform);
		ad9833_changefrequency(Frequency);
		ad9833_update();
	}
	else if (command == AD9833Command_Stop)
	{
		// Stop the AD9833 and all activity.
		if (input.Length != 0)
			throw(IndexOutOfRangeException, "Invalid data length for the stop command of the AD9833");
		ad9833_stop();
	}
	else if (command == AD9833Command_Frequency)
	{
		// Change frequency
		if (input.Length != sizeof(uint))
			throw(IndexOutOfRangeException, "Invalid data length for changing the frequency for AD9833");

		uint freq = *((uint*)&input.Dataframe[0]);

		ad9833_changefunction(Function);
		ad9833_changefrequency(freq);
		ad9833_update();
	}
	else if (command == AD9833Command_Phase)
	{
		// Change phase
	}
	else if (command == AD9833Command_FreqSweep_Begin)
	{
		// FreqSweep + lower freq + upper freq + timestep(mS) + freqstep(hz)
		if (input.Length != (sizeof(AD9833FreqSweep) + sizeof(uint)*4))
			throw(IndexOutOfRangeException, "Invalid data length for the frequency sweep command of AD9833");

		uint index = 0;
		AD9833FreqSweep waveform = (AD9833FreqSweep) input.Dataframe[index];
		index += sizeof(AD9833FreqSweep);
		uint lowerFreq = *((uint*)&input.Dataframe[index]);
		index += sizeof(uint);
		uint upperFreq = *((uint*)&input.Dataframe[index]);
		index += sizeof(uint);
		uint timeStep = *((uint*)&input.Dataframe[index]);
		index += sizeof(uint);
		uint freqStep = *((uint*)&input.Dataframe[index]);

		// Case Study: the following are equal.
		// uint test1 = utilities_chararr_to_uint(&input.Dataframe[index]);
		// uint test2 = *((uint*)&input.Dataframe[index]);

		ad9833_frequencysweep_begin(waveform, lowerFreq, upperFreq, timeStep, freqStep);
	}
	else if (command == AD9833Command_FreqSweep_Stop)
	{
		// Stop the AD9833's frequency sweep feature.
		if (input.Length != 0)
			throw(IndexOutOfRangeException, "Invalid data length for the stop command of the AD9833");
		ad9833_frequencysweep_stop();
	}
	else
	{
		throw(InvalidOperationException, "Unknown or unsupported AD9833Command");
	}
}
