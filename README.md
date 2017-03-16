# FunktionOS

A lightweight HAL and operating system for the STM32Fx ARM Cortex M3 microcontroller written in C.
Written for the ARM STM Discovery development board using the CooCox IDE and ARM GNU Toolchain.

## Intent

FunktionOS was developed with the intent for it being used as the basis of a function generator application utilising a DDS chipset.
Along the way I began writing modules for 2D GUI rendering for TFT screens and input peripherals such as Rotary Encoders to control the function generator.

The function generator can either be used with onboard peripherals such as the TFT screen and GUI rendering elements such as the Number picker or with the .NET
application via the serial port. Currently the project is in the state of using the serial port, but it can be configured to use the LCD instead.

## Supported hardware abstractions

- Direct Memory Access (DMA)
- External Interrupts (EXTI)
- General Purpose I/O (GPIO)
- Serial (USART/UART)
- Serial Peripheral Interface (SPI)
- Timers

## Supported peripherals

- DDS Chipset AD9833
- TFT screen ILI9341
- Rotary Encoders
- Logging via the serial port

## 2D Rendering elements

- Number picker (used for picking frequency)
- Side Menu
- Geometric shapes

## .NET client

To program the function generator an additional client software application has been written in C# for windows .NET.
This application uses the serial port to program the function generator to change its waveform and periodicity.

## Repository structure

`./funktion/` - The funktion OS

`./client/` - The .NET desktop client for controlling the function generator

# How

The easiest way to run and debug this code is using the STMDiscovery development board with CooCox IDE along with the ARM GNU C Toolchain. Don't use Keil.
If you have some other development board, make sure the peripherals are supported and mapped to the correct GPIO layouts.
