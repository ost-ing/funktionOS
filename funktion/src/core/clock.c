#include "clock.h"
#include "gpio.h"

static volatile ClockDetails Details = {0};
static volatile RunTime Runtime = {0};
void (*systick_update)(void);


/**
====================================================================
##### System, AHB and APB busses clocks configuration functions #####
===============================================================================
[..]
This section provide functions allowing to configure the System, AHB, APB1 and
APB2 busses clocks.
(#) Several clock sources can be used to drive the System clock (SYSCLK): HSI,
HSE and PLL.
The AHB clock (HCLK) is derived from System clock through configurable
prescaler and used to clock the CPU, memory and peripherals mapped
on AHB bus (DMA, GPIO...). APB1 (PCLK1) and APB2 (PCLK2) clocks are derived
from AHB clock through configurable prescalers and used to clock
the peripherals mapped on these busses. You can use
"RCC_GetClocksFreq()" function to retrieve the frequencies of these clocks.
-@- All the peripheral clocks are derived from the System clock (SYSCLK) except:
(+@) I2S: the I2S clock can be derived either from a specific PLL (PLLI2S) or
from an external clock mapped on the I2S_CKIN pin.
You have to use RCC_I2SCLKConfig() function to configure this clock.
(+@) RTC: the RTC clock can be derived either from the LSI, LSE or HSE clock
divided by 2 to 31. You have to use RCC_RTCCLKConfig() and RCC_RTCCLKCmd()
functions to configure this clock.
(+@) USB OTG FS, SDIO and RTC: USB OTG FS require a frequency equal to 48 MHz
to work correctly, while the SDIO require a frequency equal or lower than
to 48. This clock is derived of the main PLL through PLLQ divider.
(+@) IWDG clock which is always the LSI clock.
(#) For STM32F405xx/407xx and STM32F415xx/417xx devices, the maximum frequency
of the SYSCLK and HCLK is 168 MHz, PCLK2 84 MHz and PCLK1 42 MHz. Depending
on the device voltage range, the maximum frequency should be adapted accordingly:
+-------------------------------------------------------------------------------------+
| Latency | HCLK clock frequency (MHz) |
| |---------------------------------------------------------------------|
| | voltage range | voltage range | voltage range | voltage range |
| | 2.7 V - 3.6 V | 2.4 V - 2.7 V | 2.1 V - 2.4 V | 1.8 V - 2.1 V |
|---------------|----------------|----------------|-----------------|-----------------|
|0WS(1CPU cycle)|0 < HCLK <= 30 |0 < HCLK <= 24 |0 < HCLK <= 22 |0 < HCLK <= 20 |
|---------------|----------------|----------------|-----------------|-----------------|
|1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48 |22 < HCLK <= 44 |20 < HCLK <= 40 |
|---------------|----------------|----------------|-----------------|-----------------|
|2WS(3CPU cycle)|60 < HCLK <= 90 |48 < HCLK <= 72 |44 < HCLK <= 66 |40 < HCLK <= 60 |
|---------------|----------------|----------------|-----------------|-----------------|
|3WS(4CPU cycle)|90 < HCLK <= 120|72 < HCLK <= 96 |66 < HCLK <= 88 |60 < HCLK <= 80 |
|---------------|----------------|----------------|-----------------|-----------------|
|4WS(5CPU cycle)|120< HCLK <= 150|96 < HCLK <= 120|88 < HCLK <= 110 |80 < HCLK <= 100 |
|---------------|----------------|----------------|-----------------|-----------------|
|5WS(6CPU cycle)|150< HCLK <= 168|120< HCLK <= 144|110 < HCLK <= 132|100 < HCLK <= 120|
|---------------|----------------|----------------|-----------------|-----------------|
|6WS(7CPU cycle)| NA |144< HCLK <= 168|132 < HCLK <= 154|120 < HCLK <= 140|
|---------------|----------------|----------------|-----------------|-----------------|
|7WS(8CPU cycle)| NA | NA |154 < HCLK <= 168|140 < HCLK <= 160|
+---------------|----------------|----------------|-----------------|-----------------+
(#) For STM32F42xxx/43xxx devices, the maximum frequency of the SYSCLK and HCLK is 180 MHz,
PCLK2 90 MHz and PCLK1 45 MHz. Depending on the device voltage range, the maximum
frequency should be adapted accordingly:
+-------------------------------------------------------------------------------------+
| Latency | HCLK clock frequency (MHz) |
| |---------------------------------------------------------------------|
| | voltage range | voltage range | voltage range | voltage range |
| | 2.7 V - 3.6 V | 2.4 V - 2.7 V | 2.1 V - 2.4 V | 1.8 V - 2.1 V |
|---------------|----------------|----------------|-----------------|-----------------|
|0WS(1CPU cycle)|0 < HCLK <= 30 |0 < HCLK <= 24 |0 < HCLK <= 22 |0 < HCLK <= 20 |
|---------------|----------------|----------------|-----------------|-----------------|
|1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48 |22 < HCLK <= 44 |20 < HCLK <= 40 |
|---------------|----------------|----------------|-----------------|-----------------|
|2WS(3CPU cycle)|60 < HCLK <= 90 |48 < HCLK <= 72 |44 < HCLK <= 66 |40 < HCLK <= 60 |
|---------------|----------------|----------------|-----------------|-----------------|
|3WS(4CPU cycle)|90 < HCLK <= 120|72 < HCLK <= 96 |66 < HCLK <= 88 |60 < HCLK <= 80 |
|---------------|----------------|----------------|-----------------|-----------------|
|4WS(5CPU cycle)|120< HCLK <= 150|96 < HCLK <= 120|88 < HCLK <= 110 |80 < HCLK <= 100 |
|---------------|----------------|----------------|-----------------|-----------------|
|5WS(6CPU cycle)|120< HCLK <= 180|120< HCLK <= 144|110 < HCLK <= 132|100 < HCLK <= 120|
|---------------|----------------|----------------|-----------------|-----------------|
|6WS(7CPU cycle)| NA |144< HCLK <= 168|132 < HCLK <= 154|120 < HCLK <= 140|
|---------------|----------------|----------------|-----------------|-----------------|
|7WS(8CPU cycle)| NA |168< HCLK <= 180|154 < HCLK <= 176|140 < HCLK <= 160|
|---------------|----------------|----------------|-----------------|-----------------|
|8WS(9CPU cycle)| NA | NA |176 < HCLK <= 180|160 < HCLK <= 168|
+-------------------------------------------------------------------------------------+
(#) For STM32F401xx devices, the maximum frequency of the SYSCLK and HCLK is 84 MHz,
PCLK2 84 MHz and PCLK1 42 MHz. Depending on the device voltage range, the maximum
frequency should be adapted accordingly:
+-------------------------------------------------------------------------------------+
| Latency | HCLK clock frequency (MHz) |
| |---------------------------------------------------------------------|
| | voltage range | voltage range | voltage range | voltage range |
| | 2.7 V - 3.6 V | 2.4 V - 2.7 V | 2.1 V - 2.4 V | 1.8 V - 2.1 V |
|---------------|----------------|----------------|-----------------|-----------------|
|0WS(1CPU cycle)|0 < HCLK <= 30 |0 < HCLK <= 24 |0 < HCLK <= 22 |0 < HCLK <= 20 |
|---------------|----------------|----------------|-----------------|-----------------|
|1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48 |22 < HCLK <= 44 |20 < HCLK <= 40 |
|---------------|----------------|----------------|-----------------|-----------------|
|2WS(3CPU cycle)|60 < HCLK <= 84 |48 < HCLK <= 72 |44 < HCLK <= 66 |40 < HCLK <= 60 |
|---------------|----------------|----------------|-----------------|-----------------|
|3WS(4CPU cycle)| NA |72 < HCLK <= 84 |66 < HCLK <= 84 |60 < HCLK <= 80 |
|---------------|----------------|----------------|-----------------|-----------------|
|4WS(5CPU cycle)| NA | NA | NA |80 < HCLK <= 84 |
+-------------------------------------------------------------------------------------+
-@- On STM32F405xx/407xx and STM32F415xx/417xx devices:
(++) when VOS = '0', the maximum value of fHCLK = 144MHz.
(++) when VOS = '1', the maximum value of fHCLK = 168MHz.
[..]
On STM32F42xxx/43xxx devices:
(++) when VOS[1:0] = '0x01', the maximum value of fHCLK is 120MHz.
(++) when VOS[1:0] = '0x10', the maximum value of fHCLK is 144MHz.
(++) when VOS[1:0] = '0x11', the maximum value of f is 168MHz
[..]
On STM32F401x devices:
(++) when VOS[1:0] = '0x01', the maximum value of fHCLK is 64MHz.
(++) when VOS[1:0] = '0x10', the maximum value of fHCLK is 84MHz.
You can use PWR_MainRegulatorModeConfig() function to control VOS bits.
*/



/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 180000000
 *            HCLK(Hz)                       = 180000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 8000000
 *            PLL_M                          = 8
 *            PLL_N                          = 360
 *            PLL_P                          = 2
 *            PLL_Q                          = 7
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 5
 *         The LTDC Clock is configured as follow :
 *            PLLSAIN                        = 192
 *            PLLSAIR                        = 4
 *            PLLSAIDivR                     = 8
 * @param  None
 * @retval None
 *
 * COPYRIGHT(c) 2014 STMicroelectronics
 */

void clock_configure(void)
{
  Regset(&PWR->CR, PWR_CR_VOS_1, 1);
  Regset(&PWR->CR, PWR_CR_VOS_1, 0);

  //clock_mco2_init();

  RCCControl rc = {0};
  RCCConfig cfgr = {0};
  RCCPLLConfig pll = {0};

  //cfgr.ClockOutput2Type = MCOType_SYSCLK;
  //cfgr.MCO1Divider = MCOPrescaler_Div4;
  //cfgr.MCO2Divider = MCOPrescaler_Div2;

  cfgr.AHBDivider  = AHBPrescaler_Div1;
  cfgr.APB1Divider = APBPrescaler_Div4;
  cfgr.APB2Divider = APBPrescaler_Div2;
  cfgr.ClkSwitch = ClockSwitch_PLL;

  cfgr.ClockDetails.AHBDivider  = 1;
  cfgr.ClockDetails.APB1Divider = 4;
  cfgr.ClockDetails.APB2Divider = 2;

  // Core Clock = (((HSI | HSE) / PLLM) * PLL_N) / PLL_P

  //HSE = 8MHz
  pll.M = 8; // 1MHz
  pll.N = 360;
  pll.P = PLLMainFactor_Div2;
  pll.Q = 7;
  pll.Source = PLLSource_HSE;

//  rc.HSIEnable = true;

  rc.HSEEnable = true;
  rc.HSEBypass = true;
  rc.SecuritySysEnable = true;
  rc.PLLEnable = true;

  clock_init(&rc, &cfgr, &pll, false);
}

void clock_init(RCCControl* rc, RCCConfig* cfgr, RCCPLLConfig* pllcfgr, bool overDriveEnable)
{
  Details.APB1Divider = cfgr->ClockDetails.APB1Divider;
  Details.APB2Divider = cfgr->ClockDetails.APB2Divider;
  Details.AHBDivider = cfgr->ClockDetails.AHBDivider;

  uint freq = 0;

  if (cfgr->ClkSwitch == ClockSwitch_HSI)
  {
    freq = CLOCK_HSI;
  }
  else if (cfgr->ClkSwitch == ClockSwitch_HSE)
  {
    freq = CLOCK_HSE;
  }
  else if (cfgr->ClkSwitch == ClockSwitch_PLL)
  {
    //Core Clock = (((HSI | HSE) / PLLM) * PLL_N) / PLL_P
    if (pllcfgr->Source == PLLSource_HSI)
      freq = CLOCK_HSI;
    else if (pllcfgr->Source == PLLSource_HSE)
      freq = CLOCK_HSE;
    freq /= pllcfgr->M;
    freq *= pllcfgr->N;
    freq /= ((pllcfgr->P+1)*2);
  }

  Details.AHBFrequency = freq / Details.AHBDivider;
  Details.APB1Frequency = Details.AHBFrequency / Details.APB1Divider;
  Details.APB2Frequency = Details.AHBFrequency / Details.APB2Divider;

  if (Details.AHBFrequency > 180E6)
    throw(InvalidOperationException, "AHB Frequency may not exceed 180MHz");
  if (Details.APB1Frequency > 45E6)
    throw(InvalidOperationException, "APB1 Frequency may not exceed 45MHz");
  if (Details.APB2Frequency > 90E6)
    throw(InvalidOperationException, "APB2 Frequency may not exceed 90MHz");

  // Configure the AHB Prescaler
  Regset(&RCC->CFGR, RCC_CFGR_HPRE_0, (cfgr->AHBDivider) & 0x01);
  Regset(&RCC->CFGR, RCC_CFGR_HPRE_1, (cfgr->AHBDivider) & 0x02);
  Regset(&RCC->CFGR, RCC_CFGR_HPRE_2, (cfgr->AHBDivider) & 0x04);
  Regset(&RCC->CFGR, RCC_CFGR_HPRE_3, (cfgr->AHBDivider) & 0x08);

  // Configure the APB1 Low Speed, Prescaler
  Regset(&RCC->CFGR, RCC_CFGR_PPRE1_0, (cfgr->APB1Divider) & 0x01);
  Regset(&RCC->CFGR, RCC_CFGR_PPRE1_1, (cfgr->APB1Divider) & 0x02);
  Regset(&RCC->CFGR, RCC_CFGR_PPRE1_2, (cfgr->APB1Divider) & 0x04);

  // Configure the APB2 High Speed, Prescaler
  Regset(&RCC->CFGR, RCC_CFGR_PPRE2_0, (cfgr->APB2Divider) & 0x01);
  Regset(&RCC->CFGR, RCC_CFGR_PPRE2_1, (cfgr->APB2Divider) & 0x02);
  Regset(&RCC->CFGR, RCC_CFGR_PPRE2_2, (cfgr->APB2Divider) & 0x04);

  // Configure the Microcontroller clock output 1
  Regset(&RCC->CFGR, RCC_CFGR_MCO1_0, (cfgr->ClockOutput1Type) & 0x01);
  Regset(&RCC->CFGR, RCC_CFGR_MCO1_1, (cfgr->ClockOutput1Type) & 0x02);

  Regset(&RCC->CFGR, RCC_CFGR_MCO1PRE_0, (cfgr->MCO1Divider) & 0x01);
  Regset(&RCC->CFGR, RCC_CFGR_MCO1PRE_1, (cfgr->MCO1Divider) & 0x02);
  Regset(&RCC->CFGR, RCC_CFGR_MCO1PRE_2, (cfgr->MCO1Divider) & 0x04);

  // Configure the Microcontroller clock output 2
  Regset(&RCC->CFGR, RCC_CFGR_MCO2_0, (cfgr->ClockOutput2Type) & 0x01);
  Regset(&RCC->CFGR, RCC_CFGR_MCO2_1, (cfgr->ClockOutput2Type) & 0x02);

  Regset(&RCC->CFGR, RCC_CFGR_MCO2PRE_0, (cfgr->MCO2Divider) & 0x01);
  Regset(&RCC->CFGR, RCC_CFGR_MCO2PRE_1, (cfgr->MCO2Divider) & 0x02);
  Regset(&RCC->CFGR, RCC_CFGR_MCO2PRE_2, (cfgr->MCO2Divider) & 0x04);

  /* Configure RCC_PLLCFGR */

  // Core Clock = (((HSI | HSE) / PLLM) * PLL_N) / PLL_P
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC, pllcfgr->Source);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLM_0, (pllcfgr->M) & 0x01);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLM_1, (pllcfgr->M) & 0x02);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLM_2, (pllcfgr->M) & 0x04);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLM_3, (pllcfgr->M) & 0x08);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLM_4, (pllcfgr->M) & 0x10);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLM_5, (pllcfgr->M) & 0x20);

  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLN_0, (pllcfgr->N) & 0x0001);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLN_1, (pllcfgr->N) & 0x0002);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLN_2, (pllcfgr->N) & 0x0004);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLN_3, (pllcfgr->N) & 0x0008);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLN_4, (pllcfgr->N) & 0x0010);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLN_5, (pllcfgr->N) & 0x0020);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLN_6, (pllcfgr->N) & 0x0040);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLN_7, (pllcfgr->N) & 0x0080); // ?
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLN_8, (pllcfgr->N) & 0x0100);

  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLP_0, (pllcfgr->P) & 0x01);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLP_1, (pllcfgr->P) & 0x02);

  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_0, (pllcfgr->Q) & 0x01);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_1, (pllcfgr->Q) & 0x02);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_2, (pllcfgr->Q) & 0x04);
  Regset(&RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_3, (pllcfgr->Q) & 0x08);

  /* Configure RCC_CR */
  Regset(&RCC->CR, RCC_CR_HSEBYP, rc->HSEBypass);
  Regset(&RCC->CR, RCC_CR_CSSON,  rc->SecuritySysEnable);

  // Enable the HSI
  Regset(&RCC->CR, RCC_CR_HSION, rc->HSIEnable);
  if (rc->HSIEnable)
    while(!(RCC->CR & RCC_CR_HSIRDY));

  // Enable the HSE
  Regset(&RCC->CR, RCC_CR_HSEON, rc->HSEEnable);
  if (rc->HSEEnable)
    while(!(RCC->CR & RCC_CR_HSERDY));

  // Enable the PLL
  Regset(&RCC->CR, RCC_CR_PLLON, rc->PLLEnable);
  if (rc->PLLEnable)
    while(!(RCC->CR & RCC_CR_PLLRDY));

  // Enable the PLL I2S
  Regset(&RCC->CR, RCC_CR_PLLI2SON, rc->PLLI2SEnable);
  if (rc->PLLI2SEnable)
    while(!(RCC->CR & RCC_CR_PLLI2SRDY));

  // Page 124. Activating Over-drive

  if (overDriveEnable)
  {
    // Freezes? who cares for the moment we dont need it.

    PWR->CR |= PWR_CR_ODEN;
    while (!(PWR->CSR & PWR_CSR_ODRDY));

    if (rc->PLLEnable)
      while(!(RCC->CR & RCC_CR_PLLRDY));
  }

  // Page 81. Increasing the CPU Frequency and FLASH Latency
  // The following assumes a voltage range of 2.7v - 3.6v.

  int hclk = Details.AHBFrequency;
  int waitStates = 0;

  if (hclk > 0 && hclk <= 30E6)
    waitStates = 0;
  else if (hclk > 30E6 && hclk <= 60E6)
    waitStates = 1;
  else if (hclk > 60E6 && hclk <= 90E6)
    waitStates = 2;
  else if (hclk > 90E6 && hclk <= 120E6)
    waitStates = 3;
  else if (hclk > 120E6 && hclk <= 150E6)
    waitStates = 4;
  else if (hclk > 150E6 && hclk <= 180E6)
    waitStates = 5;

  FLASH->ACR |= waitStates << 0;

  // Configure Clock Switch
  Regset(&RCC->CFGR, RCC_CFGR_SW_0, (cfgr->ClkSwitch) & 0x01);
  Regset(&RCC->CFGR, RCC_CFGR_SW_1, (cfgr->ClkSwitch) & 0x02);

  systick_init();
}

void clock_mco1_init(void)
{
  gpio_init(GPIOA, GPIOMode_AltFunction, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_NoPull, 8);
}

void clock_mco2_init(void)
{
  gpio_init(GPIOC, GPIOMode_AltFunction, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_NoPull, 9);
}

void systick_init(void)
{
  // By default, the STM32F4 uses a 16MHz High speed internal clock (HSI).
  // Interrupt every 1mS.
  while (SysTick_Config(Details.AHBFrequency / 1000) != 0);
}

void runtime_reset(void)
{
  RunTime rt = {0};
  Runtime = rt;
}

RunTime runtime_get(void)
{
  return Runtime;
}

ulong runtime_milliseconds(void)
{
  ulong total = 0;
  total += Runtime.milliseconds;
  total += (Runtime.seconds * 1000);
  total += (Runtime.minutes * 60 * 1000);
  total += (Runtime.hours * 60 * 60 * 1000);
  total += (Runtime.days * 24 * 60 * 60 * 1000);
  return total;
}

ClockDetails clock_details(void)
{
  return Details;
}

void systick_subscribe(void(*callback)(void))
{
  systick_update = callback;
}

// Calculate the running time.
static void calculate_runtime(void)
{
  Runtime.milliseconds ++;

  if (Runtime.milliseconds >= 1E3)
  {
    Runtime.milliseconds = 0;
    Runtime.seconds ++;
  }
  if (Runtime.seconds >= 60)
  {
    Runtime.seconds = 0;
    Runtime.minutes ++;
  }
  if (Runtime.minutes >= 60)
  {
    Runtime.minutes = 0;
    Runtime.hours ++;
  }
}


// Interrupts ever 1mS.
void SysTick_Handler(void)
{
  // Calculate running time.
  // This feature can be used for accurately determining delay periods
  // Or the amount of time the system has been running for
  calculate_runtime();

  if (systick_update != NULL)
    systick_update();
}

void WWDG_IRQHandler(void)
{
}

void UsageFault_Handler(void)
{
}
