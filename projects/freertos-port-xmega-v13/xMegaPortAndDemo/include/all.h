// file: all.h

#ifndef ALL_H
#define ALL_H


#define SERIAL_SIMULATION

//gcc
#include "avr_compiler.h"

//driver
#include "pmic_driver.h"
#include "tc_driver.h"
#include "clksys_driver.h"

//FreeRTOS 
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "StackMacros.h"

//own
#include "mydefs.h"
#include "initializations.h"
#include "usarttasks.h"
#include "baudprecalc.h"

//#include <stdlib.h>
//#include <string.h>
//#include <avr/eeprom.h>
//#include <avr/wdt.h>

#endif

