#include "sdram.h"
#include "../core/core.h"

void sdram_configure(void)
{
	SDRAMTimingRegister timing = {0};
	SDRAMControlRegister control = {0};


	/* FMC SDRAM device initialization sequence --------------------------------*/
	/* Step 1 ----------------------------------------------------*/
	/* Timing configuration for 90 Mhz of SD clock frequency (180Mhz/2) */
	/* TMRD: 2 Clock cycles */
	/* 1 clock cycle = 1 / 90MHz = 11.1ns */
	timing.LoadModeRegisterToActive   = 2;
	/* TXSR: min=70ns (7x11.10ns) */
	timing.ExitSelfRefreshDelay = 7;
	/* TRAS: min=42ns (4x11.10ns) max=120k (ns) */
	timing.SelfRefreshTime = 4;
	/* TRC:  min=70 (7x11.10ns) */
	timing.RowCycleDelay = 7;
	/* TWR:  min=1+ 7ns (1+1x11.10ns) */
	timing.RecoveryDelay = 2;
	/* TRP:  20ns => 2x11.10ns */
	timing.RowPrechargeDelay = 2;
	/* TRCD: 20ns => 2x11.10ns */
	timing.RowCycleDelay = 2;

	control.Bank = SDRAMBankType_2;
	control.ColumnBits = SDRAMColumnBits_8bit;
	control.RowBits = SDRAMRowBits_12bit;
	control.BusWidth = SDRAMMemoryBusWidth_16bit;
	control.NumInternalBanks = SDRAMBankNumber_4Banks;
	control.CasLatency = SDRAMCasLatency_3cycle;
	control.WriteProtection = true;
	control.ClockPeriod = SDRAMClockPeriod_2xHCLK;
	control.ReadBurst = false;
	control.ReadPipe = SDRAMReadPipe_1cycle;

	sdram_init(&control, &timing);

	SDRAMCommandRegister command = {0};
	command.CommandMode = SDRAMCommandMode_ClockConfigEnable;
	//command.CommandTargetBank1 = true;
	command.CommandTargetBank2 = true;
	command.AutoRefreshNumber = 1;
	command.ModeRegisterDefinition = 0;
	sdram_command_init(&command);

	command.CommandMode = SDRAMCommandMode_AllBankPrecharge;
	//command.CommandTargetBank1 = true;
	command.CommandTargetBank2 = true;
	command.AutoRefreshNumber = 1;
	command.ModeRegisterDefinition = 0;
	sdram_command_init(&command);

	command.CommandMode = SDRAMCommandMode_AutoRefresh;
	//command.CommandTargetBank1 = true;
	command.CommandTargetBank2 = true;
	command.AutoRefreshNumber = 8;
	command.ModeRegisterDefinition = 0;
	sdram_command_init(&command);

	command.CommandMode = SDRAMCommandMode_LoadModeRegister;
	//command.CommandTargetBank1 = true;
	command.CommandTargetBank2 = true;
	command.AutoRefreshNumber = 1;
	command.ModeRegisterDefinition = 0x0231;
	sdram_command_init(&command);

	/* Set the refresh rate counter */
	/* (7.81 us x Freq) - 20 = (7.81 * 90MHz) - 20 = 683 */
	/* Set the device refresh counter */
	sdram_set_refresh(680);

	sdram_validate();

}

static void sdram_pins_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;

	// GPIO B
	int gpiobPins[] = { 5, 6 };
	gpio_init_af_pins(GPIOB, GPIOMode_AltFunction, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_NoPull, gpiobPins, 2, GPIOAFMapping_FSMC_FMC_SDIO_OTGHS);

	// GPIO C
	gpio_init_af(GPIOC, GPIOMode_AltFunction, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_NoPull, 0, GPIOAFMapping_FSMC_FMC_SDIO_OTGHS);

	// GPIO D
	int gpiodPins[] = { 0, 1, 8, 9, 10, 14, 15 };
	gpio_init_af_pins(GPIOD, GPIOMode_AltFunction, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_NoPull, gpiodPins, 7, GPIOAFMapping_FSMC_FMC_SDIO_OTGHS);

	// GPIO E
	int gpioePins[] = { 0, 1, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	gpio_init_af_pins(GPIOE, GPIOMode_AltFunction, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_NoPull, gpioePins, 11, GPIOAFMapping_FSMC_FMC_SDIO_OTGHS);

	// GPIO F
	int gpiofPins[] = { 0, 1, 2, 3, 4, 5, 11, 12, 13, 14, 15 };
	gpio_init_af_pins(GPIOF, GPIOMode_AltFunction, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_NoPull, gpiofPins, 12, GPIOAFMapping_FSMC_FMC_SDIO_OTGHS);

	// GPIO G
	int gpiogPins[] = { 0, 1, 4, 5, 8, 15 };
	gpio_init_af_pins(GPIOG, GPIOMode_AltFunction, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_NoPull, gpiogPins, 6, GPIOAFMapping_FSMC_FMC_SDIO_OTGHS);
}

void sdram_command_init(SDRAMCommandRegister* command)
{
	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);

	uint32_t reg = 0;
	reg |= (((command->CommandMode) & 0x07) << 0);
	reg |= (((command->CommandTargetBank2) & 0x01) << 3);
	reg |= (((command->CommandTargetBank1) & 0x01) << 4);
	reg |= ((((command->AutoRefreshNumber)-1) & 0x0F) << 5);
	reg |= (((command->ModeRegisterDefinition) & 0x1FFF) << 9);
	FMC_Bank5_6->SDCMR = reg;

	for (int i = 0; i < 999; i++);

	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);
}

void sdram_set_refresh(uint32_t count)
{
	FMC_Bank5_6->SDRTR |= (count<<1);

	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);

	for (int i = 0; i < 999; i++);
}

void sdram_init(SDRAMControlRegister* control, SDRAMTimingRegister* timing)
{
	RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;

	// Maximum Frequency for the FMC is 90MHz as per page 22 of the datasheet.

	sdram_pins_init();

	SDRAMBankType bank = control->Bank;

	/* Control configuration register */
	if (bank == SDRAMBankType_1)
	{
		// 1. Set the clock period
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_SDCLK_0, control->ClockPeriod & 0x01);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_SDCLK_1, control->ClockPeriod & 0x02);

		// 2. Set Read Burst
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_RBURST, control->ReadBurst);

		// 3. Set Read Pipe
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_RPIPE_0, control->ReadPipe & 0x01);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_RPIPE_1, control->ReadPipe & 0x02);

		// 4. Control memory device features
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_NC_0, control->ColumnBits & 0x01);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_NC_1, control->ColumnBits & 0x02);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_NR_0, control->RowBits & 0x01);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_NR_1, control->RowBits & 0x02);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_CAS_0, control->CasLatency & 0x01);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_CAS_1, control->CasLatency & 0x02);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_WP, control->WriteProtection);
	}
	else
	{
		// 1. Set the clock period
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_SDCLK_0, control->ClockPeriod & 0x01);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_SDCLK_1, control->ClockPeriod & 0x02);

		// 2. Set Read Burst
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_RBURST, control->ReadBurst);

		// 3. Set Read Pipe
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_RPIPE_0, control->ReadPipe & 0x01);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_1], FMC_SDCR1_RPIPE_1, control->ReadPipe & 0x02);

		// 4. Control memory device features
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_2], FMC_SDCR2_NC_0, control->ColumnBits & 0x01);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_2], FMC_SDCR2_NC_1, control->ColumnBits & 0x02);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_2], FMC_SDCR2_NR_0, control->RowBits & 0x01);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_2], FMC_SDCR2_NR_1, control->RowBits & 0x02);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_2], FMC_SDCR2_CAS_0, control->CasLatency & 0x01);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_2], FMC_SDCR2_CAS_1, control->CasLatency & 0x02);
		Regset(&FMC_Bank5_6->SDCR[SDRAMBankType_2], FMC_SDCR2_WP, control->WriteProtection);
	}

	/* Timing configuration register */
	if (bank == SDRAMBankType_1)
	{
		uint32_t reg = 0;
		reg |= (((timing->LoadModeRegisterToActive - 1)  & 0x0F) << 0);
		reg |= (((timing->ExitSelfRefreshDelay - 1) & 0x0F) << 4);
		reg |= (((timing->SelfRefreshTime - 1) & 0x0F) << 8);
		reg |= (((timing->RowCycleDelay - 1) & 0x0F) << 12);
		reg |= (((timing->RecoveryDelay - 1) & 0x0F) << 16);
		reg |= (((timing->RowPrechargeDelay - 1) & 0x0F) << 20);
		reg |= (((timing->RowToColumnDelay - 1) & 0x0F) << 24);
		FMC_Bank5_6->SDTR[SDRAMBankType_1] = reg;
	}
	else
	{
		uint32_t reg = 0;
		reg |= (((timing->LoadModeRegisterToActive - 1)  & 0x0F) << 0);
		reg |= (((timing->ExitSelfRefreshDelay - 1) & 0x0F) << 4);
		reg |= (((timing->SelfRefreshTime - 1) & 0x0F) << 8);
		reg |= (((timing->RecoveryDelay - 1) & 0x0F) << 16);

		// Required to set these bits in SDTR1.
		uint32_t reqReg = 0;
		reqReg |= (((timing->RowCycleDelay - 1) & 0x0F) << 12);		//TRC
		reqReg |= (((timing->RowPrechargeDelay - 1) & 0x0F) << 20); //TRP

		FMC_Bank5_6->SDTR[SDRAMBankType_1] = reqReg;
		FMC_Bank5_6->SDTR[SDRAMBankType_2] = reg;
	}
}

bool sdram_validate(void)
{
	// Write byte

	while (1)
	{
		uint32_t* p = (uint32_t*)(SDRAM_START_ADDR + 0x50);

		*p = 0x18;
	}

	//*(__IO uint8_t *) (SDRAM_START_ADDR + 0x50) = 0x18;

	// Read byte
	//uint8_t b =  (*(__IO uint8_t *) (SDRAM_START_ADDR + 0x50));

	/*uint32_t b = *p;

	if (b != (uint32_t)0x18)
	{
		while(1);
	}
*/}



