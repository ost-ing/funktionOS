#include "usart.h"
#include "clock.h"

/*
 * U(S)ARTx   TX     RX     TX     RX     TX     RX     APB
  USART1     PA9   PA10   PB6   PB7           2
  USART2     PA2   PA3   PD5   PD6           1
  USART3     PB10   PB11   PC10   PC11   PD8   PD9   1
  UART4     PA0   PA1   PC10   PC11           1
  UART5     PC12   PD2                   1
  USART6     PC6   PC7   PG14   PG9           2
  UART7     PE8   PE7   PF7   PF6           1
  UART8     PE1   PE0                   1
 *
 */

#define USART_SUPPORTED_INSTANCES 3

USARTInstance* Instances[USART_SUPPORTED_INSTANCES];

static void usart_clock_init(USART_TypeDef* usart)
{
  if (usart == USART1)
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
  else if (usart == USART2)
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
  else if (usart == USART3)
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
  else if (usart == USART6)
    RCC->APB1ENR |= RCC_APB2ENR_USART6EN;
}

static IRQn_Type usart_get_irqn(USART_TypeDef* usart)
{
  if (usart == USART1)
    return USART1_IRQn;
  else if (usart == USART2)
    return USART2_IRQn;
  else if (usart == USART3)
    return USART3_IRQn;
  else if (usart == USART6)
    return USART6_IRQn;

  throw(InvalidOperationException, "Unsupported USART provided, unable to find IRQn");
  return -1;
}

static void usart_baud_init(USARTInstance* instance)
{
  ClockDetails details = clock_details();

  uint32_t fractionaldivider, apbclock, tmpreg, integerdivider = 0;

  if ((instance->USARTX == USART1) || (instance->USARTX == USART6))
    // APB2
    apbclock = details.APB2Frequency;
  else
    // APB1
    apbclock = details.APB1Frequency;

  if (apbclock == 0)
    apbclock = CLOCK_HSI;

  /* Determine the integer part */
  if ((instance->USARTX->CR1 & USART_CR1_OVER8) != 0)
  {
    /* Integer part computing in case Oversampling mode is 8 Samples */
    integerdivider = ((25 * apbclock) / (2 * (instance->BaudRate)));
  }
  else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
  {
    /* Integer part computing in case Oversampling mode is 16 Samples */
    integerdivider = ((25 * apbclock) / (4 * (instance->BaudRate)));
  }
  tmpreg = (integerdivider / 100) << 4;

  /* Determine the fractional part */
  fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

  /* Implement the fractional part in the register */
  if ((instance->USARTX->CR1 & USART_CR1_OVER8) != 0)
  {
    tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
  }
  else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
  {
    tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
  }

  /* Write to USART BRR register */
  instance->USARTX->BRR = (uint16_t)tmpreg;
}

void usart_init(USARTInstance* instance)
{
  int mapping = 0;
  USART_TypeDef* usart = instance->USARTX;

  if (usart == USART1)
    mapping = 0;
  else if (usart == USART2)
    mapping = 1;
  else if (usart == USART3)
    mapping = 2;

  if (Instances[mapping] != NULL)
    throw(AlreadyInitException, "The USART has already been allocated");

  Instances[mapping] = instance;

  USARTControl1Register* control1 = &(instance->Control1);
  USARTControl2Register* control2 = &(instance->Control2);
  USARTControl3Register* control3 = &(instance->Control3);

  // 0. Enable Peripheral Bus Clock to USARTx
  usart_clock_init(usart);

  // 1. Enable Usart
  Regset(&usart->CR1, USART_CR1_UE, control1->UsartEnable);

  // 2. Define the word length
  Regset(&usart->CR1, USART_CR1_M, control1->WordLength);

  // 3. Define the stop bits
  Regset(&usart->CR2, USART_CR2_STOP_0, (control2->StopBits & 0x01));
  Regset(&usart->CR2, USART_CR2_STOP_1, (control2->StopBits & 0x02));

  // 4. Enable DMA if required
  Regset(&usart->CR3, USART_CR3_DMAR, (control3->DMAReceiverEnable & 0x01));
  Regset(&usart->CR3, USART_CR3_DMAT, (control3->DMATransmitEnable & 0x01));

  // 5. Set the baud rate
  //  usart->BRR = 0x1A0;   /* Assumes 8MHz APB1 */
  usart_baud_init(instance);

  // Not apart of Initialisation Sequence outlined in specification
  // Control Register 1
  Regset(&usart->CR1, USART_CR1_SBK, control1->SendBreak);
  Regset(&usart->CR1, USART_CR1_RWU, control1->RxWakeupMethod);
  Regset(&usart->CR1, USART_CR1_IDLEIE, control1->IdleInterruptEnable);
  Regset(&usart->CR1, USART_CR1_RXNEIE, control1->ReceiveNotEmptyInterruptEnable);
  Regset(&usart->CR1, USART_CR1_TCIE, control1->TransmitCompleteInterruptEnable);
  Regset(&usart->CR1, USART_CR1_TXEIE, control1->TransmitCompleteInterruptEnable);
  Regset(&usart->CR1, USART_CR1_PEIE, control1->ParityErrorInteruptEnable);
  Regset(&usart->CR1, USART_CR1_PS, control1->ParityType);
  Regset(&usart->CR1, USART_CR1_PCE, control1->ParityControlEnable);
  Regset(&usart->CR1, USART_CR1_WAKE, control1->WakeMethod);
  Regset(&usart->CR1, USART_CR1_OVER8, control1->OversampleMode);

  // Control Register 2
  Regset(&usart->CR2, (USART_CR2_ADD & 0x01), (control2->NodeAddress & 0x01));
  Regset(&usart->CR2, (USART_CR2_ADD & 0x02), (control2->NodeAddress & 0x02));
  Regset(&usart->CR2, (USART_CR2_ADD & 0x04), (control2->NodeAddress & 0x04));
  Regset(&usart->CR2, USART_CR2_LBDL, (control2->BreakDetection & 0x01));
  Regset(&usart->CR2, USART_CR2_LBDIE, (control2->LinBreakDetectInterruptEnable & 0x01));
  Regset(&usart->CR2, USART_CR2_LBCL, (control2->LastBitClockPulseEnable & 0x01));
  Regset(&usart->CR2, USART_CR2_LBDIE, (control2->LinBreakDetectInterruptEnable & 0x01));
  Regset(&usart->CR2, USART_CR2_CPHA, (control2->ClockPhasePulse & 0x01));
  Regset(&usart->CR2, USART_CR2_CPOL, (control2->ClockPolarity & 0x01));
  Regset(&usart->CR2, USART_CR2_LINEN, (control2->LinModeEnable & 0x01));

  // Control Register 3
  Regset(&usart->CR3, USART_CR3_EIE, (control3->ErrorInterruptEnable & 0x01));
  Regset(&usart->CR3, USART_CR3_IREN, (control3->IrDAModeEnable & 0x01));
  Regset(&usart->CR3, USART_CR3_IRLP, (control3->IrDALowPowerMode & 0x01));
  Regset(&usart->CR3, USART_CR3_HDSEL, (control3->HalfDuplexMode & 0x01));
  Regset(&usart->CR3, USART_CR3_NACK, (control3->SmartcardNAKEnable & 0x01));
  Regset(&usart->CR3, USART_CR3_SCEN, (control3->SmartcardModeEnable & 0x01));
  Regset(&usart->CR3, USART_CR3_RTSE, (control3->RTSEnable & 0x01));
  Regset(&usart->CR3, USART_CR3_CTSE, (control3->CTSEnable & 0x01));
  Regset(&usart->CR3, USART_CR3_CTSIE, (control3->CTSInterruptEnable & 0x01));
  Regset(&usart->CR3, USART_CR3_ONEBIT, (control3->SampleBit & 0x01));

  if (control1->ReceiveNotEmptyInterruptEnable)
  {
    IRQn_Type type = usart_get_irqn(instance->USARTX);
    NVIC_SetPriority(type, instance->InterruptPriority);
    NVIC_EnableIRQ(type);
  }

  // 6. Set the Tx/Rx Enabled
  Regset(&usart->CR1, USART_CR1_TE, control1->TransmitEnable);
  Regset(&usart->CR1, USART_CR1_RE, control1->ReceiveEnable);
}

void usart_write(USARTInstance* instance, uchar byte)
{
  if (instance == NULL)
    throw(MissingPointerException, "Cannot write to USART as the instance is null or uninitialised");
  while(!(instance->USARTX->SR & USART_SR_TXE));
  instance->USARTX->DR = byte;
}

uchar usart_read_poll(USARTInstance* instance)
{
  return instance->USARTX->DR;
}

static void usart_validate_errors(USARTInstance* instance)
{
  if (instance->USARTX->SR & USART_SR_NE)
    throw(InvalidOperationException, "Noise error flag");
  if (instance->USARTX->SR & USART_SR_FE)
    throw(InvalidOperationException, "Framing error flag");
  if (instance->USARTX->SR & USART_SR_PE)
    throw(InvalidOperationException, "Parity error flag");
  if (instance->USARTX->SR & USART_SR_ORE)
    throw(InvalidOperationException, "OVERRUN");
}

static void usart_handle_isr(USARTInstance* instance)
{
  usart_validate_errors(instance);

  if (instance->OnReceiveNotEmpty == NULL)
    return;

  while (instance->USARTX->SR & USART_SR_RXNE)
  {
    int val = instance->USARTX->DR;
    instance->OnReceiveNotEmpty(instance, val);
  }
}

void USART3_IRQHandler(void)
{
  USARTInstance* instance = Instances[2];
  if (instance == NULL)
    throw(MissingPointerException, "Could not find USART instance");

  usart_handle_isr(instance);
}
