#ifndef USART_C_H
#define USART_C_H

#include "definitions.h"

typedef enum{
  USARTWordLength_8bit = 0,
  USARTWordLength_9bit = 1
}USARTWordLength;

typedef enum{
  USARTWakeMethod_IdleLine    = 0,
  USARTWakeMethod_AddressMark = 1
}USARTWakeMethod;

typedef enum{
  USARTReceiverWakeMethod_Active = 0,
  USARTReceiverWakeMethod_Mute   = 1
}USARTReceiverWakeMethod;

typedef enum{
  USARTParityType_Even = 0,
  USARTParityType_Odd  = 1
}USARTParityType;

typedef enum{
  USARTOversampleMode_16 = 0,
  USARTOversampleMode_8  = 1
}USARTOversampleMode;

typedef enum{
  USARTLinBreakDetection_10bit = 0,
  USARTLinBreakDetection_11bit = 1
}USARTLinBreakDetection;

typedef enum{
  USARTStopBits_1     = 0,
  USARTStopBits_Half     = 1,
  USARTStopBits_2     = 2,
  USARTStopBits_3Quater = 3
}USARTStopBits;

typedef struct{
  bool SendBreak;
  bool ReceiveEnable;
  bool TransmitEnable;
  bool IdleInterruptEnable;
  bool ReceiveNotEmptyInterruptEnable;
  bool TransmitCompleteInterruptEnable;
  bool ParityErrorInteruptEnable;
  bool ParityControlEnable;
  USARTParityType ParityType;
  USARTWakeMethod WakeMethod;
  USARTReceiverWakeMethod RxWakeupMethod;
  USARTWordLength WordLength;
  USARTOversampleMode OversampleMode;
  bool UsartEnable;
}USARTControl1Register;

typedef struct{
  bool LinModeEnable;
  USARTStopBits StopBits;
  bool ClockEnable;
  bool ClockPolarity;
  bool ClockPhasePulse;
  bool LastBitClockPulseEnable;
  bool LinBreakDetectInterruptEnable;
  USARTLinBreakDetection BreakDetection;
  uchar NodeAddress;
}USARTControl2Register;

typedef struct{
  bool ErrorInterruptEnable;
  bool IrDAModeEnable;
  bool IrDALowPowerMode;
  bool HalfDuplexMode;
  bool SmartcardNAKEnable;
  bool SmartcardModeEnable;
  bool DMAReceiverEnable;
  bool DMATransmitEnable;
  bool RTSEnable;
  bool CTSEnable;
  bool CTSInterruptEnable;
  bool SampleBit;
}USARTControl3Register;

typedef struct USARTInstance{
  USART_TypeDef* USARTX;
  uint32_t BaudRate;
  USARTControl1Register Control1;
  USARTControl2Register Control2;
  USARTControl3Register Control3;
  void (*OnReceiveNotEmpty)(struct USARTInstance*, uchar);
  uint PeripheralMapping;
  int InterruptPriority;
}USARTInstance;

void usart_init(USARTInstance* instance);
void usart_write(USARTInstance* instance, uchar byte);
uchar usart_read_poll(USARTInstance* instance);

#endif
