#ifndef CLOCK_C_H
#define CLOCK_C_H

#include "definitions.h"

#define CLOCK_HSI 16E6
#define CLOCK_HSE  8E6

typedef enum{
  MCOType_SYSCLK = 0x00,
  MCOType_PLLI2S = 0x01,
  MCOType_HSE    = 0x02,
  MCOType_PLL    = 0x03
}MCOType;

typedef enum{
  MCOPrescaler_Div1 = 0x00,
  MCOPrescaler_Div2 = 0x04,
  MCOPrescaler_Div3 = 0x05,
  MCOPrescaler_Div4 = 0x06,
  MCOPrescaler_Div5 = 0x07,
}MCOPrescaler;

typedef enum{
  APBPrescaler_Div1  = 0x00,
  APBPrescaler_Div2  = 0x04,
  APBPrescaler_Div4  = 0x05,
  APBPrescaler_Div8  = 0x06,
  APBPrescaler_Div16 = 0x07,
}APBPrescaler;

typedef enum{
  AHBPrescaler_Div1   = 0x00,
  AHBPrescaler_Div2   = 0x08,
  AHBPrescaler_Div4   = 0x09,
  AHBPrescaler_Div8   = 0x0A,
  AHBPrescaler_Div16  = 0x0B,
  AHBPrescaler_Div64  = 0x0C,
  AHBPrescaler_Div128 = 0x0D,
  AHBPrescaler_Div256 = 0x0E,
  AHBPrescaler_Div512 = 0x0F,
}AHBPrescaler;

typedef enum{
  ClockSwitch_HSI = 0x00,
  ClockSwitch_HSE = 0x01,
  ClockSwitch_PLL = 0x02,
}ClockSwitch;

typedef enum{
  PLLMainFactor_Div2 = 0x00,
  PLLMainFactor_Div4 = 0x01,
  PLLMainFactor_Div6 = 0x02,
  PLLMainFactor_Div8 = 0x03,
}PLLMainFactor;

typedef enum{
  PLLSource_HSI = 0,
  PLLSource_HSE = 1
}PLLSource;

typedef struct{
  uint Q;
  uint M;
  uint N;
  PLLMainFactor P;
  PLLSource Source;
}RCCPLLConfig;

typedef struct{
  uint AHBDivider;
  uint APB2Divider;
  uint APB1Divider;
  uint AHBFrequency;
  uint APB1Frequency;
  uint APB2Frequency;
}ClockDetails;

typedef struct{
  ClockSwitch ClkSwitch;
  AHBPrescaler AHBDivider;
  APBPrescaler APB1Divider;
  APBPrescaler APB2Divider;
  MCOType ClockOutput1Type;
  MCOType ClockOutput2Type;
  MCOPrescaler MCO1Divider;
  MCOPrescaler MCO2Divider;
  bool I2SSource;
  ClockDetails ClockDetails;
}RCCConfig;

typedef struct{
  bool HSIEnable;
  bool HSEEnable;
  bool HSEBypass;
  bool SecuritySysEnable;
  bool PLLEnable;
  bool PLLI2SEnable;
}RCCControl;

typedef struct{
  int days;
  int hours;
  int minutes;
  int seconds;
  int milliseconds;
}RunTime;

ulong runtime_milliseconds(void);
RunTime runtime_get(void);
ClockDetails clock_details(void);
void clock_configure(void);
void clock_init(RCCControl* rc, RCCConfig* cfgr, RCCPLLConfig* pllcfgr, bool overDriveEnable);
void systick_init(void);
void systick_subscribe(void(*callback)(void));
void runtime_reset(void);

#endif
