#ifndef AD9833_C_H
#define AD9833_C_H

#include "../core/core.h"

typedef enum
{
  AD9833Command_Reserved      = 0x00,
  AD9833Command_Stop          = 0x01,
  AD9833Command_Waveform      = 0x02,
  AD9833Command_Frequency     = 0x03,
  AD9833Command_Phase         = 0x04,
  AD9833Command_FreqSweep_Begin  = 0x05,
  AD9833Command_FreqSweep_Stop   = 0x06,
}AD9833Command;

typedef enum
{
  AD9833Waveform_Reserved = 0x00,
  AD9833Waveform_Sine     = 0x01,
  AD9833Waveform_Square   = 0x02,
  AD9833Waveform_Sawtooth = 0x03,
}AD9833Waveform;

typedef enum
{
  AD9833FreqSweep_Reserved = 0x00,
  AD9833FreqSweep_RiseFall = 0x01,
  AD9833FreqSweep_Rise     = 0x02,
  AD9833FreqSweep_Fall     = 0x03,
}AD9833FreqSweep;

typedef struct
{
  AD9833FreqSweep Mode;
  uint LowerFreq;
  uint UpperFreq;
  uint TimeStep;
  uint FreqStep;
  bool IsPlaying;
  bool Polarity;
}AD9833FreqSweepState;

void ad9833_init(TimerInstance* timer);
void ad9833_update(void);
void ad9833_changefunction(AD9833Waveform function);
void ad9833_changefrequency(uint frequency);
void ad9833_changephase(int phase);
void ad9833_frequencysweep_begin(AD9833FreqSweep mode, uint lowerFreq, uint upperFreq, uint timeStep, uint freqStep);
void ad9833_frequencysweep_stop(void);
void ad9833_input_handler(void* sender, SysInput input);
void ad9833_stop(void);

#endif
