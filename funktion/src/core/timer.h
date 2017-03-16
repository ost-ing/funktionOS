#ifndef TIMER_C_H
#define TIMER_C_H

#include "definitions.h"

typedef enum
{
  TimerFunctionalState_Enabled  = 0,
  TimerFunctionalState_Disabled = 1,
}TimerFunctionalState;

typedef struct {
  /*CR1*/
  bool CEN;   //Counter enable
  bool UDIS;  //Update disable
  bool URS;   //Update request source
  bool OPM;   //One pulse mode
  bool ARPE;  //Auto reload preload enable

  /*CR2*/
  bool MMS_0;  //Master mode selection
  bool MMS_1;  //Master mode selection
  bool MMS_2;  //Master mode selection
        //Reset = 0b000,
        //Enable = 0b001,
        //Update = 0b010,

  /*TIMX_DIER*/
  bool UDE;   //Update DMA request enable
  bool UIE;  //Update interrupt enable

  /*TIMX_SR*/
  bool UIF;  //Update interrupt flag

  /*TIMX_EGR*/
  bool UG;  //Update generation

}TIMConfig;

typedef struct TimerInstance{
  uint _Mapping;
  uint PeripheralMapping;
  TIM_TypeDef* TIMX;
  void (*OnTimerInterrupt)(struct TimerInstance* instance);
}TimerInstance;

void timer_init(TimerInstance* instance, TIMConfig control);
void timer_deinit(TimerInstance* instance);
void timer_start(TimerInstance* instance);
void timer_stop(TimerInstance* instance);
void timer_overflow_interrupt_configure(TimerInstance* instance, TimerFunctionalState state, int priority);
void timer_overflow_interrupt_period(TimerInstance* instance, int period);

#endif
