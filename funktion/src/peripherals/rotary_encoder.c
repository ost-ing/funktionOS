#include "rotary_encoder.h"
#include "../core/core.h"
#include <stdio.h>

#define ROTARY_ENCODER_INSTANCES  5
#define ROTARY_ENCODER_MIN_MS     5

static void signal_isr(EXTIInstance* instance);
static void button_isr(EXTIInstance* instance);
static RotaryEncoder* Instances[ROTARY_ENCODER_INSTANCES];

void rotary_encoder_init(RotaryEncoder* instance)
{
  if (instance->Mapping > ROTARY_ENCODER_INSTANCES)
    throw(InitialisationException, "RotaryEncoder Instance beyond allowed allocated count");
  if (Instances[instance->Mapping] != NULL)
    throw(AlreadyInitException, "RotaryEncoder Instance at mapping already initialised");

  Instances[instance->Mapping] = instance;

  /* Signal line 1 external interrupt -- Clock line */
  instance->_SignalEXTI.GPIOX = instance->SignalAGPIOX;
  instance->_SignalEXTI.NVICPriority = 3;
  instance->_SignalEXTI.OnExternalInterrupt[0] = &signal_isr;
  instance->_SignalEXTI.Pin = instance->SignalAPin;
  instance->_SignalEXTI.Mapping = instance->Mapping;
  instance->_SignalEXTI.Trigger = EXTITrigger_FallingRising;
  exti_init(&instance->_SignalEXTI);

  /* Signal line 2 -- State line */
  gpio_init(GPIOB, GPIOMode_Input, GPIOOutputType_PushPull, GPIOSpeed_2MHz, GPIOResistor_NoPull, 1);

  /* Push button external interrupt */
  instance->_ButtonEXTI.GPIOX = instance->ButtonGPIOX;
  instance->_ButtonEXTI.NVICPriority = 3;
  instance->_ButtonEXTI.OnExternalInterrupt[0] = &button_isr;
  instance->_ButtonEXTI.Pin = instance->ButtonPin;
  instance->_ButtonEXTI.Mapping = instance->Mapping;
  instance->_ButtonEXTI.Trigger = EXTITrigger_Rising;
  exti_init(&instance->_ButtonEXTI);

  instance->LastA = 1;
}

static void calculate(RotaryEncoder* instance)
{
  bool signalA = gpio_read_pin(instance->_SignalEXTI.GPIOX, instance->_SignalEXTI.Pin);
  bool signalB = gpio_read_pin(instance->SignalBGPIOX, instance->SignalBPin);

  /* Check difference */
  if (signalA != instance->LastA) {
    instance->LastA = signalA;

    if (instance->LastA == 0) {
      if (signalB == true) {
        instance->Value++;
        instance->Incremented = true;
      } else {
        instance->Value--;
        instance->Incremented = false;
      }
    }
  }
}

static void signal_isr(EXTIInstance* instance)
{
  // Retrieve the Encoder via mapping
  RotaryEncoder* encoder = Instances[instance->Mapping];
  if (encoder == NULL)
    throw(MissingPointerException, "Cannot find RotaryEncoder Instance from mapping");

  ulong ms = runtime_milliseconds();
  ulong dt = ms - encoder->_DebounceTime;

/*  char dst[32];
  char val[sizeof(ulong)];
  sprintf(val, "%d", dt);

  strcat(dst, "RE: Last Interrupt ms");
  strcat(dst, val);

  console_println(dst);
*/
  encoder->_DebounceTime = ms;
  if (dt <= ROTARY_ENCODER_MIN_MS){
    if (dt == 0)
      return;
    return;
  }

  calculate(encoder);

  encoder->OnRotaryEncoderAdjust();
}

static void button_isr(EXTIInstance* instance)
{
  RotaryEncoder* encoder = Instances[instance->Mapping];
  if (encoder == NULL)
    throw(MissingPointerException, "Cannot find RotaryEncoder Instance from mapping");

  ulong ms = runtime_milliseconds();
  ulong dt = ms - encoder->_DebounceTime;
  if (dt <= ROTARY_ENCODER_MIN_MS)
    return;

  encoder->OnRotaryEncoderPressed();
}
