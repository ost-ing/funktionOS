# FunktionOS

A lightweight HAL and operating system for the STM32Fx ARM Cortex M3 microcontroller.
FunktionOS was developed with the intent for it being used as the basis of a function generator application utilising a DDS chipset.
This software comes with no guarentees what-so-ever, it was developed as a side hobby project.

Additionally, the developer will need to modify the software to match the hardware GPIO layout.

To program the function generator an additional client software application has been written in C# for windows .NET. This application uses the serial port to program the function generator to change its waveform and periodicity.

You will require the CooCox IDE to develop this with the ARM GNU toolchain


