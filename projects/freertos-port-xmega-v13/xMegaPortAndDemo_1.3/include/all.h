//
// file: all.h
//
// xmega demo program VG Feb,Mar,Oct 2011
//
// CPU:   atxmega32A4, atxmega128a1
//
// Version: 1.2.1
//
#ifndef ALL_H
#define ALL_H

//gcc
#include "avr_compiler.h"

//driver
#include "pmic_driver.h"
#include "tc_driver.h"
#include "clksys_driver.h"
#include "sleepConfig.h"
#include "port_driver.h"

//FreeRTOS 
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "StackMacros.h"

//own
#include "mydefs.h"
#include "initializations.h"
#include "taskPrototypes.h"
#include "baudprecalc.h"
#include "errorHandlers.h"
#include "utils.h"

#endif // ALL_H


