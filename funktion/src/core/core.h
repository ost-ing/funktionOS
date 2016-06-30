#ifndef COMMON_C_H
#define COMMON_C_H

/*
 * All core libraries are added into this singular header file.
 * Once added, the application or peripheral domains can easily
 * access all the core functionality by simply including "../core/core.h"
 */

/* Definitions include adds the Microcontroller Memory Abstraction Layer */
#include "definitions.h"
#include "utilities.h"
#include "clock.h"
#include "spi.h"
#include "gpio.h"
#include "dma.h"
#include "sdram.h"
#include "usart.h"
#include "serial.h"
#include "exti.h"
#include "timer.h"

#endif
