#include "input_manager.h"
#include "../peripherals/AD9833.h"
#include "../peripherals/pulse_generator.h"

/* The number of supported commands */
#define SUPPORTED_COMMANDS 2

/* Input command definitions. Note: 0x00-0x1F are Operating System reserved */
#define AD9833_SERIAL_COMMAND    0x20  /* AD9833 command */
#define PULSEG_SERIAL_COMMAND    0x21  /* Pulse generator command */

/* Input commands are defined for lookup */
static InputCommand InputCommands[SUPPORTED_COMMANDS] = {
  AD9833_SERIAL_COMMAND,
  PULSEG_SERIAL_COMMAND,
};

/* Input Handlers are defined for lookup */
static void(*InputHandlers[SUPPORTED_COMMANDS])(void*, SysInput) = {
  &ad9833_input_handler,
  &pulse_generator_input_handler,
};

// For the given SysInput, finds the relevent command and function pointer
// for callback.
void input_manager_process(void* sender, SysInput input)
{
  // Find the Command
  int index = -1;
  for (int i = 0; i < SUPPORTED_COMMANDS; i++)
  {
    if (InputCommands[i] == input.Command)
    {
      index = i;
      break;
    }
  }

  // Unknown or unsupported Command
  if (index == -1)
    return;

  // Callout to the handler
  InputHandlers[index](sender, input);
}
