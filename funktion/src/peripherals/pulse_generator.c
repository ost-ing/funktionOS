#include "pulse_generator.h"

#define PULSE_INSTANCE_MAX 1
#define PULSE_SERIAL_INDEX 0

static PulseInstance* Instances[PULSE_INSTANCE_MAX];
static uint MapIndex = 0;

static void execute_timer_callback(TimerInstance* timer);
static void execute(PulseInstance* instance);

void pulse_generator_init(PulseInstance* instance)
{
    gpio_init(instance->GPIOX, GPIOMode_Output, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_NoPull, instance->Pin);

    // Ensure not already initialised and instance is OK
    if (Instances[MapIndex] != NULL)
      throw(AlreadyInitException, "This pulse generator is already initialised");
    if (instance->Timer == NULL)
      throw(MissingPointerException, "The timer for the pulse generator cannot be NULL");

    instance->_Mapping = MapIndex;
    Instances[MapIndex++] = instance;

    // Initialise the timer
    TIMConfig timerSettings = {0};
    timerSettings.ARPE = 1;    //Auto reload pre-load enabled
    timerSettings.OPM = 0;    //One pulse mode is disabled
    timerSettings.URS = 0;    //Generate interrupt&DMA only when overflow happens
    timerSettings.UDIS = 0;    //Update enable
    timerSettings.MMS_0 = 0;  //Not using the master mode
    timerSettings.MMS_1 = 0;
    timerSettings.MMS_2 = 0;
    timerSettings.UDE = 0;    //DMA request disabled
    timerSettings.UIE = 1;    //Interrupt enbaled
    timerSettings.UG = 0;    //No update generation

    // Set the callback for the timer
    instance->Timer->OnTimerInterrupt = &execute_timer_callback;
    timer_init(instance->Timer, timerSettings);
}

void pulse_generator_run(PulseInstance* instance)
{
  if (instance->SequenceMap.Size <= 0)
    throw(InvalidArgumentException, "Cannot run pulse generator as no sequence has been defined");

  // Stop the timer (if its already being used)
  timer_stop(instance->Timer);

  // Configure timer and pulse gen
  instance->Timer->OnTimerInterrupt = &execute_timer_callback;
  instance->Timer->PeripheralMapping = instance->_Mapping;
  instance->SequenceMap.Index = 0;
  instance->_IsRunning = true;

  // Execute the first element of the sequence map which will start the timer
  execute(instance);
}

void pulse_generator_stop(PulseInstance* instance)
{
  instance->_IsRunning = false;
  timer_stop(instance->Timer);
  instance->GPIOX->ODR &= ~(1 << instance->Pin);
}

void pulse_generator_configure(PulseInstance* instance, uint sequenceLength, bool states[], uint micros[])
{
  // Stop the pulse generator if its already running
  pulse_generator_stop(instance);

  // Configure the sequence map
  instance->SequenceMap.Index = 0;
  instance->SequenceMap.Size = sequenceLength;

  // Copy the array/pointers contents into the instances sequence-map
  for (int i = 0; i < sequenceLength; i++)
  {
    instance->SequenceMap.Micros[i] = micros[i];
    instance->SequenceMap.States[i] = states[i];
  }
}

// The following is a SysInput handler - This method gets called by an input manager when data is parsed via serial comms.
void pulse_generator_input_handler(void* sender, SysInput input)
{
  if (input.Type != SysInput_Serial)
    throw(InvalidOperationException, "Unknown or unsupported System input method for AD9833");

  // A pointer to the instance of serial port that sent the data ;)
  // SerialInstance* serial = (SerialInstance*)sender;

  // The subcommand tells the Pulse Generator what to do
  PulseCommand command = (PulseCommand) input.Subcommand;

  PulseInstance* instance = Instances[PULSE_SERIAL_INDEX];

  if (instance == NULL)
    throw(InvalidOperationException, "Pulse Generator Instance missing! Is it initialised?");

  if (command == PulseCommand_Start)
  {
    if (input.Length != 1)
      throw(IndexOutOfRangeException, "Invalid data length for the start command of the pulse generator");

    instance->_IsLooping = (input.Dataframe[0] & PulseFlag_Looping) != 0;

    pulse_generator_run(instance);
  }
  else if (command == PulseCommand_Stop)
  {
    if (input.Length != 0)
      throw(IndexOutOfRangeException, "Invalid data length for the stop command of the pulse generator");

    pulse_generator_stop(instance);
  }
  else if (command == PulseCommand_Configure)
  {
    // Seq. Length + 1*StateArray[Seq. Length] + 4*MicroSecondArray[Seq. Length]
    uint sequenceLength = *((uint*)&input.Dataframe[0]);

    // Ensure the total length of the sequence does not exceed the Pulse Generators buffer maximum size
    if (sequenceLength > PULSE_SEQUENCE_MAXSIZE)
      throw(IndexOutOfRangeException, "The sequence length must not exceed the maximum buffer size of the pulse generator");

    // Ensure the input is equal to Length Prefix + Boolean Sequences + Microsecond Sequences
    if (input.Length != (sizeof(uint) + (sequenceLength*sizeof(uchar)) + (sequenceLength*sizeof(uint))))
      throw(IndexOutOfRangeException, "Invalid data length for the configure command of the pulse generator");

    // Get pointers to the states array and microsecond array
    bool* states = (bool*)&input.Dataframe[sizeof(uint)];
    uint* micros = (uint*)&input.Dataframe[sizeof(uint) + sequenceLength];

    // Configure the pulse generator
    pulse_generator_configure(instance, sequenceLength, states, micros);
  }
  else
  {
    throw(InvalidOperationException, "Unknown or unsupported Pulse Generator subcommand");
  }
}

static void execute_timer_callback(TimerInstance* timer)
{
  PulseInstance* instance = Instances[timer->PeripheralMapping];

  // Ensure everything is valid
  if (instance == NULL)
    throw(MissingPointerException, "Could not find the PulseGenerator for the given timer mapping.");

  execute(instance);
}

static void execute(PulseInstance* instance)
{
  if (!instance->_IsRunning)
    return;

  // Check if we are at the end of the sequence frame.
  if (instance->SequenceMap.Index >= instance->SequenceMap.Size)
  {
    if (!instance->_IsLooping)
    {
      pulse_generator_stop(instance);
      return;
    }
    instance->SequenceMap.Index = 0;
  }

  // Get the current state & the number of microseconds for it
  bool state = instance->SequenceMap.States[instance->SequenceMap.Index];
  uint micros = instance->SequenceMap.Micros[instance->SequenceMap.Index];
  instance->SequenceMap.Index++;

  // Set the state of the pin
  instance->GPIOX->ODR = state
      ? (instance->GPIOX->ODR |= 1 << instance->Pin)
      : (instance->GPIOX->ODR &= ~(1 << instance->Pin));

  // Reinstate the timer for the next interval
  timer_overflow_interrupt_period(instance->Timer, micros);
  timer_overflow_interrupt_configure(instance->Timer, TimerFunctionalState_Disabled, 3);
  timer_start(instance->Timer);
}
