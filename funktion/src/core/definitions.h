#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/* System Include */
#include "stm32f429xx.h"
#include <stddef.h>
#include <stdbool.h>
#include "libraries/exceptions4c/e4c_lite.h"

/* Defined exceptions */
E4C_DECLARE_EXCEPTION(InvalidArgumentException);
E4C_DECLARE_EXCEPTION(InitialisationException);
E4C_DECLARE_EXCEPTION(AlreadyInitException);
E4C_DECLARE_EXCEPTION(IndexOutOfRangeException);
E4C_DECLARE_EXCEPTION(MissingPointerException);
E4C_DECLARE_EXCEPTION(InvalidOperationException);

/* Data-typedef's */
typedef unsigned char	uchar;
typedef unsigned int	uint;
typedef unsigned short	ushort;
typedef unsigned long	ulong;
typedef unsigned char   InputCommand;

/* Enumerations */
typedef enum
{
	SysInput_Serial      = 0x00,
	SysInput_USB         = 0x01,
	SysInput_Bluetooth   = 0x02,
	SysInput_BluetoothLE = 0x03,
}SysInputType;

/* Structures */
typedef struct
{
	void* Value1;
	void* Value2;
	uint  Size1;
	uint  Size2;
}PointerPair;

#define SYS_INPUT_DATAFRAME_SIZE 32

typedef struct
{
	SysInputType Type;
	InputCommand Command;
	uchar Subcommand;
	ushort Length;
	uchar Dataframe[SYS_INPUT_DATAFRAME_SIZE];
}SysInput;


// Sets the register values bit-field either high or low
void Regset(volatile uint32_t* reg,
	    	unsigned int bitfield,
		    uint set);
													

#endif
