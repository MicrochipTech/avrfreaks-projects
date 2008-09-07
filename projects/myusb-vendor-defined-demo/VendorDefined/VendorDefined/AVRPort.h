/*

	Vendor defined HID demonstration, based on Keyboard HID examples
	from the MyUSB source.

	Defines and data structures for the AVRPort demo.
*/

/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _HID_H_
#define _HID_H_
	/* Configuration options */

	/* Uncomment OUTPUT_ON_CONTROL to send all output packets via the control endpoint (0) */
//		#define OUTPUT_ON_CONTROL

	/* Uncomment SERIAL_STATUS to allow the serial port debugging code to be compiled in */ 
		#define SERIAL_STATUS			
	/* Uncomment COOKED_SERIAL to allow translation of /n to /r/n for windows based terminals */
		#define COOKED_SERIAL	
		
		
	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <util/delay.h>

		#include "Descriptors.h"
		#include "RingBuff.h"

		#include <MyUSB/Common/ButtLoadTag.h>         	// PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/USB/USB.h>            	// USB Functionality
		#include <MyUSB/Drivers/Board/LEDs.h>     		// LEDs driver
		#include <MyUSB/Drivers/Board/HWB.h>            // Hardware Button driver
		#include <MyUSB/Scheduler/Scheduler.h>        	// Simple scheduler for task management
		#include <MyUSB/Drivers/Misc/TerminalCodes.h> 	// ANSI terminal codes
		#include <MyUSB/Drivers/AT90USBXXX/Serial.h>	// Serial port IO

	/* Task Definitions: */
		TASK(USB_HID_Report);

	/* Type Defines: */
		typedef struct
		{
			uint8_t 	ReportID;						// USB report ID
			uint8_t 	CmdByte;						// Command Byte
			uint8_t 	DestByte;						// Destination byte see below
			uint8_t 	DataByte;						// Databyte, to be read or written
			uint16_t	Address;						// EEPROM / RAM address
		} USB_HIDReport_Data_t;
		
		typedef struct
		{
			uint8_t 	ReportID;						// USB report ID
			uint8_t 	CmdByte;						// Command Byte
			uint16_t	EESize;							// Size of EEPROM in bytes
			uint16_t	RAMSize;						// Size of RAM in bytes
		} USB_HIDFeature_Data_t;
		
	/* Event Handlers: */
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_ConfigurationChanged);
		HANDLES_EVENT(USB_UnhandledControlPacket);

	/* USB HID requests */
		#define GET_REPORT		0X01
		#define GET_IDLE		0X02
		#define GET_PROTOCOL	0x03
		#define SET_REPORT		0X09
		#define SET_IDLE		0X0A
		#define SET_PROTOCOL	0x0B

	/* USB HID Report IDs, used in GET/SET_REPORT */
		#define REPORT_ID_INPUT		0x01
		#define REPORT_ID_OUTPUT	0x02
		#define REPORT_ID_FEATURE	0x03
	
	/* CmdByte breakdown 
	   
		When Reading/Writing a port (DestByte=DestPort).
		
		uWRRPPPP where
	   
		u		= unused in input, set to zero, used to flag error on reply
		W		= Read/Write bit, 0=Read, 1=Write
		RR		= Port reg to access, 00=DDR, 01=PIN, 10=PORT, 11=EEPROM
		PPPP	= Port to access, 0=A, 1=B etc
	*/
	
		#define WriteMask		0x40		// R/W bit
		#define RegMask			0x30		// Reg no bits
		#define PortMask		0x0F		// Port mask
	
		#define RegPIN			0x00		// PIN reg 
		#define RegDDR			0x10		// DDR reg
		#define RegPORT			0x20		// PORT reg
		#define RegShift		4			// Right shift this many bits to make reg absolute
		#define MaxReg			0x02		// Max register offset, 0=IN, 1=DDR, 2=OUT
	
		#define PortBase		0x20		// Port A base address.
		#define PortSize		0x03		// Each port takes 3 addresses, pin,ddr,port
	
		#define ErrorFlag		0x80		// If CmdByte has this bit set then there was an IOError
	
		#define ErrNoError		0x00		// No error
		#define ErrPortInvalid	0x01		// Invalid port specified.
		#define ErrRegInvalid	0x02		// Invalid register specified
		#define ErrDestInvalid	0x03		// Invalid destination
		#define ErrAddrInvalid	0x04		// Invalid Address, for EEPROM/RAM
		
	/* CmdByte breakdown 
	   
		When Reading/Writing EEPROM or RAM
	
		u		= unused in input, set to zero, used to flag error on reply
		W		= Read/Write bit, 0=Read, 1=Write
		CCCCCC	= Operation to perform.
		
	*/
		
		#define OPMask			0x3F		// Operation mask, to extract op code from command byte
		#define ReadOrWrite		0x00		// Read or write value, W in CmdByte determines r or w
		#define GetAddressMax	0x01		// Get Max value for address
	
	/* Destination codings */
	
		#define DestPort		0x00		// Data is for a port read/write, CMD byte as above
		#define DestEEPROM		0x01		// Data is for EEPRROM 
		#define DestRAM			0x02		// Data is for RAM

		#if (defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB646__) || \
			 defined(__AVR_AT90USB1287__) || defined(__AVR_AT90USB647__))

			#define NoValidPorts 6
			#define ValidPortInit 	{0,1,2,3,4,5}	// Port A..F

		#elif (defined(__AVR_AT90USB162__) || defined(__AVR_AT90USB82__))

			#define NoValidPorts 3
			#define ValidPortInit 	{1,2,3}			// Port b,c,d
		#else
		#error Not supported on this AVR
		#endif	


	/* Functions */
	
	uint8_t ProcessRequest(USB_HIDReport_Data_t *AVRIORequest);
	void DoIO(USB_HIDReport_Data_t 	*IORequest);
	uint8_t PortValid(uint8_t	Port);
#endif
