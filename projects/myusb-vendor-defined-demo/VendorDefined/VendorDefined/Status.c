/*
	Status.c
	
	Functions for logging program status to the serial port, to
	be used for debugging pruposes etc.
	
	2008-03-21, P.Harvey-Smith.
	
*/

#include "status.h"
#include "AVRPort.h"
#include <stdio.h>

#ifdef SERIAL_STATUS

void DecodeRequest(USB_HIDReport_Data_t *AVRIORequest);
static int StdioSerial_TxByte(char DataByte, FILE *Stream);

static FILE SerialOut = FDEV_SETUP_STREAM(StdioSerial_TxByte,NULL,_FDEV_SETUP_WRITE);

int StdioSerial_TxByte(char DataByte, FILE *Stream)
{
	#ifdef COOKED_SERIAL	
		if((DataByte=='\r') || (DataByte=='\n'))
		{
			Serial_TxByte('\r');
			Serial_TxByte('\n');
		}
		else
	#endif
	
	Serial_TxByte(DataByte);
	
	return 0;
}

void cls(void)
{
	log(ESC_ERASE_DISPLAY);
	log(ESC_CURSOR_POS(0,0));
}

void Init_Serial(void)
{
	Serial_Init(38400);
	stdout=&SerialOut;
	
	cls();
	
	log("stdio initialised\n");
	log("MyUSB AVRPort Demo, compiled at %s on %s\n",__TIME__,__DATE__);
}

void DecodeRequest(USB_HIDReport_Data_t *AVRIORequest)
{
	uint8_t	Reg;		// Register to write
	uint8_t	PortNo;		// Port to write
	
	log("IORequest={ReportID:%2.2X, Cmd:%2.2X, Dest:%2.2X, Data:%2.2X, Addr:%4.4X}\n", 		\
	               AVRIORequest->ReportID,AVRIORequest->CmdByte,AVRIORequest->DestByte,	\
				   AVRIORequest->DataByte,AVRIORequest->Address);

	switch (AVRIORequest->DestByte)
	{
		case DestPort : 
		
			PortNo=(AVRIORequest->CmdByte & PortMask);			// Set port in 
			Reg=(AVRIORequest->CmdByte & RegMask) >> RegShift;	// Get Reg, IN, DDR, PORT

			if(AVRIORequest->CmdByte & WriteMask) 	
				log("Write: ");
			else
				log("Read:  ");

			switch (Reg)
			{
				case 0x00	: log("PIN%c  ",PortNo+65); break;
				case 0x01	: log("DDR%c  ",PortNo+65); break;
				case 0x02	: log("PORT%c ",PortNo+65); break;
				case 0x03	: log("UNDEF%c",PortNo+65); break;
			}
	
			if(PortValid(PortNo))
				log("Port valid\n");
			else
				log("Port invalid\n");
		
			log("PortNo=%d, Reg=%d\n",PortNo,Reg);
			
			break;
		
		case DestEEPROM :
			if(AVRIORequest->CmdByte & WriteMask) 
				log("EEPROM Write %2.2X at address %4.4X\n",AVRIORequest->DataByte,AVRIORequest->Address);
			else
				log("EEPROM read from address %4.4X\n",AVRIORequest->Address);
			break;
			
		case DestRAM :
			if(AVRIORequest->CmdByte & WriteMask) 
				log("RAM Write %2.2X at address %4.4X\n",AVRIORequest->DataByte,AVRIORequest->Address);
			else
				log("RAM read from address %4.4X\n",AVRIORequest->Address);
			break;
	}
}

#else
void cls(void) {};
void Init_Serial(void) {} ;
void DecodeRequest(USB_HIDReport_Data_t *AVRIORequest) {};

#endif
