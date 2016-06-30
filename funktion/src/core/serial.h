#ifndef SERIAL_C_H
#define SERIAL_C_H

#include "../core/core.h"
#define RX_BUFFER_SIZE 64

typedef struct
{
	/* Configuration Variables */
	uint Baudrate;
	uint TxPin;
	uint RxPin;
	USART_TypeDef* USARTX;
	GPIO_TypeDef* GPIOX;
	int Priority;

	/* Private Variables */
	USARTInstance _Usart;
	uint _Mapping;
	uint _BufferIndex;
	bool _LastByteSync;
	uchar _ReceiveBuffer[RX_BUFFER_SIZE];
}SerialInstance;



void serial_init(SerialInstance* instance, bool isGlobal);

void serial_set_global(SerialInstance* instance);
void serial_input_handler(void(*callback)(void*, SysInput));

void serial_instance_write(SerialInstance* instance, uchar byte);
void serial_instance_print(SerialInstance* instance, char* string);
void serial_instance_println(SerialInstance* instance, char* string);

void serial_write(uchar byte);
void serial_print(char* string);
void serial_println(char* string);

#endif
