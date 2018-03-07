#include "serial.h"
#include <stdio.h>
#include <string.h>

/*
  The DNOS protocol is a simple binary based serial protocol which features unique bytes for
  the framing of the structure and a 2 byte XOR digest.
  As the framing bytes (STX and ETX) are unique (they are not to be present within the data-frame)
  any bytes that do exist inside the data-frame that match STX or ETX are to be delimited with the
  delimiter byte (DLM). Such as if a data-frame is: [0x12,0x34,0x55,STX,0x11,ETX,0x01] it will then
  become: [0x12,0x34,0x55,DLM,STX,0x11,DLM,ETX,0x01] this way the parsing protocol is then simplified.

    DNOS Protocol structure:
     1b SYNC
     1b STX
     1b Command
     1b Sub-command
     2b length   [0 = LSB, 1 = MSB]
     nb data frame
     2b XOR based checksum  (from command to end of data-frame)
     1b ETX

     Note: Can write to the console using windows terminal command: 'echo hello > COMx'
*/

#define ERROR   -1
#define STX   0xFD
#define ETX   0xFE
#define DLM   0xFC
#define SYN   0xFA
#define ACK    0xEF
#define NAK    0xEE

#define SUPPORTED_INSTANCES 2
#define RX_MIN_SIZE         8

static void(*InputHandler)(void*, SysInput);
static SerialInstance* GlobalInstance;
static SerialInstance* Instances[SUPPORTED_INSTANCES];
static uint MapIndex = 0;

static void serial_readbyte_isr(USARTInstance* instance, uchar byte);
static void serial_parse_buffer(SerialInstance* instance);
static void increment_index(uint* index);

void serial_init(SerialInstance* instance, bool isGlobal)
{
  if (instance->_Mapping != 0 || Instances[instance->_Mapping] != NULL)
    throw(AlreadyInitException, "Serial Instance already initialised");
  if (MapIndex > SUPPORTED_INSTANCES)
    throw(InitialisationException, "Maximum number of serial instances established");

  Instances[MapIndex] = instance;
  instance->_Mapping = MapIndex;

  if (isGlobal || GlobalInstance == NULL)
    serial_set_global(instance);

  // Configure GPIO
  gpio_init_af(instance->GPIOX, GPIOMode_AltFunction, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_PullUp, instance->TxPin, GPIOAFMapping_USART1_3);
  gpio_init_af(instance->GPIOX, GPIOMode_AltFunction, GPIOOutputType_PushPull, GPIOSpeed_80MHz_100MHz, GPIOResistor_PullUp, instance->RxPin, GPIOAFMapping_USART1_3);

  USARTInstance Usart = {0};
  Usart.PeripheralMapping = MapIndex;
  Usart.Control1.UsartEnable = true;
  Usart.Control1.TransmitEnable = true;
  Usart.Control1.ReceiveEnable = true;
  Usart.Control1.ReceiveNotEmptyInterruptEnable = true;
  Usart.Control1.WordLength = USARTWordLength_8bit;
  Usart.Control2.StopBits   = USARTStopBits_1;
  Usart.BaudRate = instance->Baudrate;
  Usart.USARTX = instance->USARTX;
  Usart.InterruptPriority = instance->Priority;
  Usart.OnReceiveNotEmpty = &serial_readbyte_isr;
  instance->_Usart = Usart;
  usart_init(&instance->_Usart);

  MapIndex++;
}

// Sets the input handler for the serial module. When data is decoded the input handler
// processes the command and passes then passes the message to the appropriate module.
void serial_input_handler(void(*callback)(void*, SysInput))
{
  InputHandler = callback;
}

// Sets the default serial instance for all calls not specifying a specific SerialInstance.
void serial_set_global(SerialInstance* instance)
{
  if (instance == NULL)
    throw(MissingPointerException, "Null instance cannot be used to set the global serial port");
  GlobalInstance = instance;
}

// Prints a line of ascii characters on the serial port for the specified SerialInstance.
void serial_instance_println(SerialInstance* instance, char* string)
{
  serial_instance_print(instance, string);
  usart_write(&instance->_Usart, '\r');
  usart_write(&instance->_Usart, '\n');
}

// Prints a line of ascii characters on the serial port with the global serial port.
void serial_println(char* string)
{
  serial_instance_println(GlobalInstance, string);
}

// Prints a series of ascii characters on the serial port for the specified SerialInstance.
void serial_instance_print(SerialInstance* instance, char* string)
{
  while (*string) // Unsafe & exploitable
  {
    usart_write(&instance->_Usart, *string++);
  }
}

// Prints a series of ascii characters on the serial port with the global serial port.
void serial_print(char* string)
{
  serial_instance_print(GlobalInstance, string);
}

// Writes a byte on the serial port for the given SerialInstance.
void serial_instance_write(SerialInstance* instance, uchar byte)
{
  usart_write(&instance->_Usart, byte);
}

void serial_write(uchar byte)
{
  serial_instance_write(GlobalInstance, byte);
}

static void increment_index(uint* index)
{
  (*index)++;
  if (*index > RX_BUFFER_SIZE-1)
    (*index) = 0;
}

static bool validate_checksum(ushort expected, InputCommand command, uchar subcommand, ushort length, uchar dataframe[])
{
  ushort calc = 0x0000;
  calc ^= (uchar) command;
  calc ^= subcommand;
  calc ^= length;
  for (int i = 0; i < length; i++)
    calc ^= dataframe[i];

  return expected == calc;
}

static void serial_readbyte_isr(USARTInstance* instance, uchar byte)
{
  SerialInstance* serial = Instances[instance->PeripheralMapping];

  if (serial == NULL)
    throw(MissingPointerException, "Could not locate the SerialInstance from UsartInstance");

  // If the last byte we received was a SYNC byte and the current byte is
  // an STX, we have found ourselves a start-of-frame. We can reposition the index to 0.
  if (serial->_LastByteSync && byte == STX)
    serial->_BufferIndex = 0;

  serial->_ReceiveBuffer[serial->_BufferIndex] = byte;
  increment_index(&serial->_BufferIndex);

  // Set the flag that this byte is a SYNC byte for alignment of the data
  serial->_LastByteSync = (byte == SYN);

  // Only attempt to parse the buffer if we have reached the protocols minimum size requirements
  if (serial->_BufferIndex >= RX_MIN_SIZE)
  {
    serial_parse_buffer(serial);
  }
}


static void serial_parse_buffer(SerialInstance* instance)
{
  uint index = 0;

  if (instance->_ReceiveBuffer[index] != STX)
    return;

  increment_index(&index);

  InputCommand command = (InputCommand)instance->_ReceiveBuffer[index];
  increment_index(&index);

  // Extract out the sub-command
  uchar subcommand = instance->_ReceiveBuffer[index];
  increment_index(&index);

  // Extract out the length 1 (LSB)
  uchar length1 = instance->_ReceiveBuffer[index];
  increment_index(&index);

  // Extract out the length 2 (MSB)
  uchar length2 = instance->_ReceiveBuffer[index];
  increment_index(&index);

  ushort length = length1 | (length2 << 8);

  // Only continue if our Buffer is large enough
  if (instance->_BufferIndex - RX_MIN_SIZE < length)
    return;

  // Extract out the data-frame
  uchar dataframe[length];
  for (int i = 0; i < length; i++)
  {
    dataframe[i] = instance->_ReceiveBuffer[index];
    increment_index(&index);
  }

  // Extract out the checksum
  uchar crc1 = instance->_ReceiveBuffer[index];
  increment_index(&index);
  uchar crc2 = instance->_ReceiveBuffer[index];
  increment_index(&index);

  ushort checksum = crc1 | (crc2 << 8);

  if (!validate_checksum(checksum, command, subcommand, length, dataframe))
  {
    // Unexpected / bad data
    serial_instance_write(instance, NAK);
    return;
  }
  // Finally, we should have an ETX
  if (instance->_ReceiveBuffer[index] != ETX)
  {
    serial_instance_write(instance, NAK);
    return;
  }

  // Build output
  SysInput input = {0};
  input.Type = SysInput_Serial;
  input.Command = command;
  input.Subcommand = subcommand;
  input.Length = length;

  for (int i = 0; i < length; i++)
    input.Dataframe[i] = dataframe[i];

  // Send the acknowledgment byte back
  serial_instance_write(instance, ACK);

  // Scramble the frame
  instance->_ReceiveBuffer[0] = 0x00;
  instance->_ReceiveBuffer[index] = 0x00;

  // Handle the input. TODO: Could put this in the application domain
  if (InputHandler != NULL)
    InputHandler((void*)instance, input);
}
