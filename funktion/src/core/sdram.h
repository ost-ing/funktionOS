#ifndef SDRAM_C_H
#define SDRAM_C_H

#include "definitions.h"

#define SDRAM_BANK1_START_ADDR (uint32_t)0xC0000000
#define SDRAM_BANK2_START_ADDR (uint32_t)0xD0000000

/* SDRAM Start Address begins at SDRAM Bank 2 */
#define SDRAM_START_ADDR SDRAM_BANK2_START_ADDR

/* SDRAM Maximum memory width 8MB */
#define SDRAM_MEMORY_WIDTH (uint32_t)0x800000

typedef enum{
  SDRAMColumnBits_8bit  = 0,
  SDRAMColumnBits_9bit  = 1,
  SDRAMColumnBits_10bit = 2,
  SDRAMColumnBits_11bit = 3
}SDRAMColumnBits;

typedef enum{
  SDRAMRowBits_11bit = 0,
  SDRAMRowBits_12bit = 1,
  SDRAMRowBits_13bit = 2
}SDRAMRowBits;

typedef enum{
  SDRAMMemoryBusWidth_8bit  = 0,
  SDRAMMemoryBusWidth_16bit = 1,
  SDRAMMemoryBusWidth_32bit = 2,
}SDRAMMemoryBusWidth;

typedef enum{
  SDRAMCasLatency_1cycle = 1,
  SDRAMCasLatency_2cycle = 2,
  SDRAMCasLatency_3cycle = 3,
}SDRAMCasLatency;

typedef enum{
  SDRAMClockPeriod_Disabled = 0,
  SDRAMClockPeriod_2xHCLK   = 2,
  SDRAMClockPeriod_3xHCLK   = 3
}SDRAMClockPeriod;

typedef enum{
  SDRAMReadPipe_NoDelay = 0,
  SDRAMReadPipe_1cycle  = 1,
  SDRAMReadPipe_2cycle  = 2,
}SDRAMReadPipe;

typedef enum{
  SDRAMBankNumber_2Banks = 0,
  SDRAMBankNumber_4Banks = 1
}SDRAMBankNumber;

typedef enum{
  SDRAMBankType_1 = 0,
  SDRAMBankType_2 = 1
}SDRAMBankType;

typedef enum{
  SDRAMCommandMode_Normal       = 0,
  SDRAMCommandMode_ClockConfigEnable  = 1,
  SDRAMCommandMode_AllBankPrecharge   = 2,
  SDRAMCommandMode_AutoRefresh    = 3,
  SDRAMCommandMode_LoadModeRegister  = 4,
  SDRAMCommandMode_SelfRefresh    = 5,
  SDRAMCommandMode_PowerDown      = 6
}SDRAMCommandMode;

typedef struct{
  SDRAMBankType Bank;
  SDRAMColumnBits ColumnBits;
  SDRAMRowBits RowBits;
  SDRAMMemoryBusWidth BusWidth;
  SDRAMBankNumber NumInternalBanks;
  SDRAMCasLatency CasLatency;
  bool WriteProtection;
  SDRAMClockPeriod ClockPeriod;
  bool ReadBurst;
  SDRAMReadPipe ReadPipe;
}SDRAMControlRegister;

typedef struct{
  uchar LoadModeRegisterToActive;
  uchar ExitSelfRefreshDelay;
  uchar SelfRefreshTime;
  uchar RowCycleDelay;
  uchar RecoveryDelay;
  uchar RowPrechargeDelay;
  uchar RowToColumnDelay;
}SDRAMTimingRegister;

typedef struct{
  SDRAMCommandMode CommandMode;
  bool CommandTargetBank2;
  bool CommandTargetBank1;
  uchar AutoRefreshNumber;
  ushort ModeRegisterDefinition;
}SDRAMCommandRegister;

void sdram_configure(void);
void sdram_init(SDRAMControlRegister* control, SDRAMTimingRegister* timing);
void sdram_command_init(SDRAMCommandRegister* command);
bool sdram_validate(void);
void sdram_set_refresh(uint32_t count);
#endif
