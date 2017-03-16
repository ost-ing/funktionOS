#ifndef PULSE_GENERATOR_C_H
#define PULSE_GENERATOR_C_H

#include "../core/core.h"

#define PULSE_SEQUENCE_MAXSIZE 16

typedef enum
{
  PulseCommand_Reserved  = 0x00,
  PulseCommand_Start     = 0x01,
  PulseCommand_Stop      = 0x02,
  PulseCommand_Configure = 0x03,
}PulseCommand;

typedef enum
{
  PulseFlag_Reserved = 0x00,
  PulseFlag_Looping  = 0x01,
}PulseFlag;

typedef struct
{
  bool States[PULSE_SEQUENCE_MAXSIZE];
  uint Micros[PULSE_SEQUENCE_MAXSIZE];
  uint Index;
  uint Size;
}PulseSequenceMap;

typedef struct
{
  /* Configuration variables */
  GPIO_TypeDef* GPIOX;
  uchar Pin;
  PulseSequenceMap SequenceMap;
  TimerInstance* Timer;

  /* State variables */
  bool _IsRunning;
  bool _IsLooping;
  uint _Mapping;
}PulseInstance;

void pulse_generator_init(PulseInstance* instance);
void pulse_generator_run(PulseInstance* instance);
void pulse_generator_configure(PulseInstance* instance, uint sequenceLength, bool states[], uint micros[]);
void pulse_generator_stop(PulseInstance* instance);
void pulse_generator_input_handler(void* sender, SysInput input);

#endif
