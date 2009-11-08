//*****************************************************************************
//
//  File........: Record.c
//
//  Author(s)...: J.Landowski
//
//  Target(s)...: ATmega169p
//
//  Compiler....: AVR-GCC 4.3.2; avr-libc 1.4.6
//
//  Description.: Data logger (recorder)
//
//  Revisions...: 0.0
//
//  YYYYMMDD - VER. - COMMENT                           - SIGN.
//
//  20090718 - 1.0  - Created                           - J.Landowski
// 
//*****************************************************************************

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.h"
#include "usart.h"
#include "ADC.h"
#include "eeprom169.h"
#include "dataflash.h"
#include "RTC.h"
#include "record.h"

uint8_t ff = 0xFF;

#ifdef USE_DFLASH
#  define  NVMemLimit		524288L  // DATAFLASH
#  define  resetNVMem       
#  define  NVMemRead(a)		DF_read(a)
#  define  NVMemWrite(a,b)	DF_write(a,b)
#  define  NVMemFlush		DF_flush()
#else // use the EEprom
#  define  NVMemLimit      512     // EEPROM
#  define  resetNVMem      clearEEprom() 
#  define  NVMemRead(a)    EEPROM_read(a)
#  define  NVMemWrite(a,b) EEPROM_write(a,b)
#  define  NVMemFlush	  
#endif

volatile uint8_t  gRecording = FALSE;	// Are we in record mode?
volatile int16_t  gRecordTimer = 0;		// Sample interval counter

// This variable determines the recording interval. This is settable
// via the Menu system under the Record->Interval entry.

volatile int16_t gRecordInterval = 30; // Data recording interval in seconds

static uint32_t dataAddress    = 0;	 // Address in NV memory to write to


// record a header to mark a logging session (12 bytes)
void recordDataHeader()
{
	unsigned int adr;
	unsigned char buffer[16];

	dataAddress = 0;    // new recording sessions overwrite previous data
	gNewSession = TRUE; // flag the start of a new session

	resetNVMem;  // Clear EEPROM, a no-op for DataFlash
	
	// mark record start
	buffer[0]  = 165;  // 0xA5
	buffer[1]  = 165;	
	// date, time, and sample interval. Set the RTC date & time
	// before recording (if time is relevant to your use)
	buffer[2]  = gYEAR;
	buffer[3]  = gMONTH;
	buffer[4]  = gDAY;
	buffer[5]  = gHOUR;
	buffer[6]  = gMINUTE;
	buffer[7]  = gSECOND;
	buffer[8] = (gRecordInterval & 0xFF);
	buffer[9] = (gRecordInterval >> 8);
	// record end
	buffer[10] = 165;
	buffer[11] = 165;	

	// write header to NV memory
	for(adr=0; adr < 12; adr++)
	{
		NVMemWrite((uint32_t)(dataAddress+adr), buffer[adr]);
	}
	// update NV memory address
	dataAddress += 12;

	// Mark the end of the record. Temporary, to be overwritten
	// on next write so don't update the address pointer.
	NVMemWrite(dataAddress, 0xff);
	NVMemWrite(dataAddress+1, 0xff);

	DF_CS_inactive;  // Disable the DataFlash to save power
}

// Record some data to the NV memory memory (6 bytes)
void recordData()
{
	unsigned int adr;
	unsigned int tmp;
	unsigned char buffer[6];

	if( dataAddress > NVMemLimit-8 ) // stop recording when memory is full
		return;

	// read light level
    ADC_init(LIGHT_SENSOR);       // Init the ADC to measure light
    tmp = ADC_read();
	buffer[0] = tmp & 0xFF;
	buffer[1] = tmp >> 8;

	// read temperature 
    ADC_init(TEMPERATURE_SENSOR); // Init the ADC to measure temp
    tmp = ADC_read();
	buffer[2] = tmp & 0xFF;
	buffer[3] = tmp >> 8;

	// read voltage 
    ADC_init(VOLTAGE_SENSOR);     // Init the ADC to measure voltage
    tmp = ADC_read();
	buffer[4] = tmp & 0xFF;
	buffer[5] = tmp >> 8;

	// write to DataFlash
	for(adr=0; adr < 6; adr++)
	{
		NVMemWrite( (uint32_t)(dataAddress+adr), buffer[adr]);
	}
	// update DataFlash address
	dataAddress += 6;

	// Mark the end of the record. Temporary to be overwritten on
	// at next write so don't update the address pointer.
	NVMemWrite(dataAddress, 0xff);
	NVMemWrite(dataAddress+1, 0xff);

	DF_CS_inactive;  // Disable the DataFlash to save power
}

// Dump the contents of the memory store to the USART (serial port).
void memDump()
{
	uint32_t adr;
	unsigned char buffer = 0;
	unsigned char pByte = 0;

	// 12: Baud rate =  9600bps for 1MHZ Clk,U2X=1
	//  8: Baud rate = 14400bps for 1MHZ Clk,U2X=1
	// 12: Baud rate = 19200bps for 2MHZ Clk,U2X=1
	//  8: Baud rate = 57600bps for 4MHZ Clk,U2X=1
	//  8: Baud rate = 115200bps for 8MHZ Clk,U2X=1
	// Set the system clock in the main initialization routine (main.c).

	USART_Init(12); // --> 9600 BAUD for default 1Mhz CPU clock

	Delay(10);

	for(adr = 0; adr < NVMemLimit; adr++)
	{
		buffer = NVMemRead(adr);
		Usart_Tx(buffer);
		if( pByte == 0xff && buffer == 0xff)
		{
			break;
		}
		pByte = buffer;
	}

	Usart_Tx(0); // Make sure the buffers flushed
	Usart_Tx(0);

	DF_CS_inactive;  // Disable the DataFlash to save power
}

// Flush the dataflash buffer to the flash. A no-op if EEPROM.
void memFlush()
{
	NVMemFlush;
	DF_CS_inactive;  // Disable the DataFlash to save power
}
