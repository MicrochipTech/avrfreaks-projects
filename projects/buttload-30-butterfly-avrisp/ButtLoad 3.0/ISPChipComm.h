/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef ISPCHIPCOMM_H
#define ISPCHIPCOMM_H

	// INCLUDES:
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	
	#include "AVRISPCommandBytes.h"
	#include "AVRISPCommandInterpreter.h"
	#include "Delay.h"
	#include "LCD_Driver.h"
	#include "Main.h"
	#include "USI.h"
	
	// MACROS AND DEFINES:
	#define ISPCC_PROG_MODE_PAGE              0x01
	#define ISPCC_PROG_MODE_PAGEDONE          0x80
	#define ISPCC_POLL_MODE_AVR               0x03
	
	#define ISPCC_POLL_BUSYFLAG               0x01
	
	#define ISPCC_PAGE_POLLTYPE_MASK          0x70
	#define ISPCC_PAGE_POLLTYPE_MASKSHIFT     3
	#define ISPCC_PAGE_POLLTYPE_WAIT          (1 << 4)
	
	#define ISPCC_WORD_POLLTYPE_MASK          0x0E
	#define ISPCC_WORD_POLLTYPE_MASKSHIFT     0
	#define ISPCC_WORD_POLLTYPE_WAIT          (1 << 1)
	
	#define ISPCC_POLLTYPE_MASK               0x0E
	#define ISPCC_POLLTYPE_WAIT               (1 << 1)
	#define ISPCC_POLLTYPE_DATA               (1 << 2)
	#define ISPCC_POLLTYPE_READY              (1 << 3)
	
	#define ISPCC_HIGH_BYTE_READ              (1 << 3)
	#define ISPCC_LOW_BYTE_READ               (0 << 3)
	#define ISPCC_HIGH_BYTE_WRITE             (1 << 3)
	#define ISPCC_LOW_BYTE_WRITE              (0 << 3)
	
	#define ISPCC_NO_FAULT                    0
	#define ISPCC_FAULT_NOERASE               1
	#define ISPCC_FAULT_NODATATYPE            2
	#define ISPCC_FAULT_TIMEOUT               3
	
	#define ISPCC_COMM_TIMEOUT                0x02D0 // 100ms timeout at 7372800Hz, 1024 prescaler
	
	// EXTERNAL VARIABLES:
	extern const char SyncErrorMessage[] PROGMEM;
	#define ProgrammingFault                  GPIOR2 // Psudo-variable, GPIO register for speed
	
	// PROTOTYPES:
	void ISPCC_EnterChipProgrammingMode(void);
	void ISPCC_ProgramChip(void);
	
	#if defined(INC_FROM_ISPCC)
	  static void ISPCC_PollForProgComplete(const uint8_t PollData, uint16_t PollAddr);
	#endif

#endif
