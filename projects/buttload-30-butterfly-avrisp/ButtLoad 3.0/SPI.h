/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef SPI_H
#define SPI_H

	// INCLUDES:
	#include <avr/io.h>
	
	#include "GlobalMacros.h"
	#include "Main.h"
	
	// MACROS:
	#define SPI_SPIOFF()	     MACROS{ PRR |= (1 << PRSPI); }MACROE
	
	// PROTOTYPES:
	void    SPI_SPIInit(void);
	uint8_t SPI_SPITransmit(const uint8_t Data);

	
#endif
