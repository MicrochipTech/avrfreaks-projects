/*

	Vendor defined HID demonstration, based on Keyboard HID examples
	from the MyUSB source.
  
	2008-03-10, P.Harvey-Smith.
  
	2008-04-03, P.Harvey-Smith, changed to work with the new LED code in 
		MyUSB 1.3.2.
		
	2008-05-29, P.Harvey-Smith, updated for renamed/changed calls in 
		MyUSB 1.5.0 Beta.
		
	2008-09-07, P.Harvey-Smith, updated for MyUSB-1.5.2.
		
*/

/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
 */

#include "AVRPort.h"
#include "board/EEprom.h"
#include "status.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB Vendor defined HID App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);


// Valid IO ports
uint8_t ValidPorts[NoValidPorts] = ValidPortInit;

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask		, TaskStatus: TASK_STOP },
	{ Task: USB_HID_Report	, TaskStatus: TASK_STOP },
};


int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	LEDs_Init();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
	
	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialise the serial port */
	Init_Serial();

	/* Initialise the EEProm */
	InitEE();

	/* Initialize USB Subsystem */
	USB_Init();

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_Connect)
{	
	/* Start USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);

	/* Red/green to indicate USB enumerating */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED4);
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Stop running HID reporting and USB management tasks */
	Scheduler_SetTaskMode(USB_HID_Report, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Double red to indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup HID Report (to host) Endpoint */
	Endpoint_ConfigureEndpoint(HID_EPNUM_IN, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, HID_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

#ifndef OUTPUT_ON_CONTROL
	/* Setup HID Report (from host) Endpoint */
	Endpoint_ConfigureEndpoint(HID_EPNUM_OUT, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_OUT, HID_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);
#endif

	/* Double green to indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);
  
	/* Start joystick reporting task */
	Scheduler_SetTaskMode(USB_HID_Report, TASK_RUN);
}

void HandshakeControlIN(void)
{
	log("HandshakeControlIN\n");
	
	// Flag that we have recieved all the data
	while (!(Endpoint_IsSetupOUTReceived() ));
	Endpoint_ClearSetupOUT();

//	while (!(Endpoint_Setup_Out_IsReceived()));
//	Endpoint_Setup_Out_Clear();

	// Transmit zero length packet to ACK the data
	Endpoint_ClearSetupIN();
	while (!(Endpoint_IsSetupINReady()));


//	Endpoint_Setup_In_Clear();
//	while (!(Endpoint_Setup_In_IsReady()));

}

EVENT_HANDLER(USB_UnhandledControlPacket)
{	
	uint8_t					ReportID;	// Report ID
	uint8_t					Handled;	// Did we handle this packet ?
	uint16_t				wValue;		// wValue, wIndex, wLen from control packet
	uint16_t				wIndex;		// See USB HID documentation for details
	uint16_t				wLen;
	USB_HIDFeature_Data_t	Feature;	// Temp feture structure for get/set feature
	USB_HIDReport_Data_t 	IORequest;	// Temp IORequest, for output reports.

	// Get data about the Packet.
	wValue=Endpoint_Read_Word();	
	wIndex=Endpoint_Read_Word();
	wLen=Endpoint_Read_Word();
	Endpoint_ClearSetupReceived();		// Flag that we have the packet header

	Handled=false;						// Did we handle the request, assume no....
	ReportID=(wValue & 0xFF00) >> 8;	// Extract report ID, for GET/SET report

	log("USB_UnhandledControlPacket:Request=%2.2X, Type=%2.2X\n",bRequest,bmRequestType);
	log("wValue=%4.4X, wIndex=%4.4X, wLen=%4.4X, ReportID=%2.2X\n",wValue,wIndex,wLen,ReportID);	
	log("Endpoint_BytesInEndpoint()=%d\n",Endpoint_BytesInEndpoint());
	log("Data:");
	
	switch (bRequest)
	{
		case SET_REPORT	: 
			// If there is any data wait for it.
			if(wLen>0)
				while(Endpoint_BytesInEndpoint()==0) ;

			log("Set report\n");
			
			switch (ReportID)
			{
				case REPORT_ID_OUTPUT	:
					// Read the report
				
					IORequest.ReportID=Endpoint_Read_Byte();
					IORequest.CmdByte=Endpoint_Read_Byte();
					IORequest.DestByte=Endpoint_Read_Byte();
					IORequest.DataByte=Endpoint_Read_Byte();
					IORequest.Address=Endpoint_Read_Word();
					
					HandshakeControlIN();
					
					DoIO(&IORequest);
					break;
					
				case REPORT_ID_FEATURE 	:
	log("Current endpoint : %d\n",Endpoint_GetCurrentEndpoint());
	log("Endpoint_BytesInEndpoint()=%d\n",Endpoint_BytesInEndpoint());
					// Read the feture request
					Feature.ReportID=Endpoint_Read_Byte();
	log("Endpoint_BytesInEndpoint()=%d\n",Endpoint_BytesInEndpoint());
					Feature.CmdByte=Endpoint_Read_Byte();
	log("Endpoint_BytesInEndpoint()=%d\n",Endpoint_BytesInEndpoint());
					Feature.EESize=Endpoint_Read_Word();
	log("Endpoint_BytesInEndpoint()=%d\n",Endpoint_BytesInEndpoint());
					Feature.RAMSize=Endpoint_Read_Word();
	log("Endpoint_BytesInEndpoint()=%d\n",Endpoint_BytesInEndpoint());
					
					HandshakeControlIN();
					
					if(Feature.CmdByte==0x01) 
						cls();
						
					break;
			
				default :
					HandshakeControlIN();
					break;
			}

			
			Handled=true;
			break;
		
		case GET_REPORT :
			log("Get report\n");
			switch (ReportID)
			{
				case REPORT_ID_INPUT :
					log("Input report ID not supported (YET)\n");
					break;
				case REPORT_ID_FEATURE :
					log("Sending feature report\n");
					Endpoint_Write_Byte(0x01);
					Endpoint_Write_Byte(0x00);
					Endpoint_Write_Word(E2END);
					Endpoint_Write_Word(RAMEND);
					Handled=true;
					break;
			}

			// Posible to replace with HandshakeControlIN() ??? 2008-05-27 PHS
			Endpoint_ClearSetupIN();
			while (!(Endpoint_IsSetupINReady()));
				
			while (!(Endpoint_IsSetupOUTReceived()));
			Endpoint_ClearSetupOUT();

			break;
	}
	

	if (!Handled)
	{
		Endpoint_StallTransaction();
		log("Not Handled\n");
	}
	else
	{
		log("Handled\n");
	}
}


TASK(USB_HID_Report)
{
	USB_HIDReport_Data_t 	IORequest;

	/* Check for a press of the HWB button, if so clear the output screen */
	if(HWB_GetStatus())		
	{
		cls();
	}

	/* Check if the USB System is connected to a Host */
	if (USB_IsConnected)
	{
		/* Select the HID Report (from host) Endpoint */
		Endpoint_SelectEndpoint(HID_EPNUM_OUT);

		/* Check if HID Endpoint Ready for Read/Write */
		if (Endpoint_ReadWriteAllowed())
		{
			/* We have a request waiting */
			if(Endpoint_BytesInEndpoint()!=0)
			{
				log("Endpoint_BytesInEndpoint()=%d\n",Endpoint_BytesInEndpoint());
				
				/* Read a request if one available */
				Endpoint_Read_Stream(&IORequest,sizeof(IORequest));
				
				/* Handshake the IN Endpoint */
				Endpoint_ClearCurrentBank();

				/* Process the IO request */
				DoIO(&IORequest);
			}
		}
	}
}

/* Process an IO request, and send the reply, this was split from the USB_HID_Report task */
/* above so that it could also be called in response to a control channel write as is the */
/* case where the host OS does not support Out Inturuupt pipes, e.g. Original Win98       */
void DoIO(USB_HIDReport_Data_t 	*IORequest)
{
	uint8_t					IOError;

	/* Process IO request, IOError nonzero if error, invalid port etc */
	IOError=ProcessRequest(IORequest);

	/* Error processing IO, set the error flag for the host */
	if(IOError!=ErrNoError)
	{
		IORequest->CmdByte |= ErrorFlag;		// Flag error to host
		IORequest->DataByte=IOError;
	}
	/* Select the HID report (to host) Endpoint */
	Endpoint_SelectEndpoint(HID_EPNUM_IN);

	/* Wait for host to be reay for response */
	while (!Endpoint_ReadWriteAllowed()) ;

				
	/* Send response to host */
				
	log("Sending response ReportID=%2.2X\n",IORequest->ReportID);
	DecodeRequest(IORequest);

	Endpoint_Write_Stream(IORequest,sizeof(USB_HIDReport_Data_t));
								
	/* Handshake the IN Endpoint - send the data to the host */
	Endpoint_ClearCurrentBank();
}

/* Check for a valid IO port, uses ValidPorts array defined above */
/* returns true if valid, false if not valid */
uint8_t PortValid(uint8_t	Port)
{
	uint8_t	Idx;
	uint8_t	Valid = false;
	
	for(Idx=0;Idx<NoValidPorts;Idx++)
		if(ValidPorts[Idx]==Port)
			Valid=true;
			
	return Valid;
}

/* Read or write a port, note this depends on the fact that on the current USB AVRs */
/* The Port registers have a base at PortBase, and are ascending from there if this */
/* code where ported to a different AVR, then it may have to be adjusted for it's   */
/* register layout !                                                                */
uint8_t DoDestPort(USB_HIDReport_Data_t *AVRIORequest)
{
	uint8_t	Reg;			// Register to write, PINx, DDRx, PORTx
	uint8_t	*Port;			// Pointer to port IO address to write
	uint8_t	PortNo;			// Port number
	
	DecodeRequest(AVRIORequest);
	
	/* Extract the port number and register from the Command byte */
	PortNo=(AVRIORequest->CmdByte & PortMask);
	Reg=(AVRIORequest->CmdByte & RegMask) >> RegShift;			// Get Reg, IN, DDR, PORT

	AVRIORequest->ReportID=HID_REPORT_PORT_IN;					// Setup reply report
	
		/* If the port and register are valid, attempt the operation */
	if(PortValid(PortNo) && (Reg<=MaxReg))
	{
		Port=(uint8_t *)(PortNo*PortSize)+PortBase;			// Get address of port
		Port=Port+Reg;											// Add reg offset
		
		if(AVRIORequest->CmdByte & WriteMask)
			// This is a write, write the value
			*Port=AVRIORequest->DataByte;
		else
			// This is a read request
			AVRIORequest->DataByte=*Port;

		return ErrNoError;	// Pretend no error (for now)
	}
	else
	{
		if (Reg<=MaxReg)
			return ErrPortInvalid;
		else
			return ErrRegInvalid;
	}
}


/* Carry out a read or write to RAM, note this can crash the host if writing ! */
uint8_t DoDestRAM(USB_HIDReport_Data_t *AVRIORequest)
{
	uint8_t	*Addr;									// Pointer to address to write
	
	Addr=(uint8_t *)AVRIORequest->Address;			// Get pointer to RAM to access

	AVRIORequest->ReportID=HID_REPORT_PORT_IN;		// Setup reply report
	
	if (AVRIORequest->Address<=RAMEND)				// Make sure Address is valid
	{
		if (AVRIORequest->CmdByte & WriteMask)		
			*Addr=AVRIORequest->DataByte;			// This is a write
		else
			AVRIORequest->DataByte=*Addr;			// This is a read
		
		return ErrNoError;							// No error
	}
	else
	{
		return ErrAddrInvalid;						// Error invalid address
	}
}

/* Carry out a read or write to EEPROM */
uint8_t DoDestEEPROM(USB_HIDReport_Data_t *AVRIORequest)
{
	AVRIORequest->ReportID=HID_REPORT_PORT_IN;		// Setup reply report

	if (AVRIORequest->Address<=E2END)				// Check address valid
	{
		if (AVRIORequest->CmdByte & WriteMask)		// Check for read or write
			WriteEE(AVRIORequest->Address,AVRIORequest->DataByte);  // Write it
		else
			AVRIORequest->DataByte=ReadEE(AVRIORequest->Address);   // Read it
		
		return ErrNoError;
	}
	else
	{
		return ErrAddrInvalid;						// Flag invalid address
	}
}

/* Determine what we should be accessing, Ports, EEPROM or RAM, and call the */
/* Apropreate handler for each */
uint8_t ProcessRequest(USB_HIDReport_Data_t *AVRIORequest)
{
	switch (AVRIORequest->DestByte)
	{
		case DestPort	: return DoDestPort(AVRIORequest); break;
		case DestEEPROM	: return DoDestEEPROM(AVRIORequest); break;
		case DestRAM	: return DoDestRAM(AVRIORequest); break;
	}
	
	return ErrDestInvalid;
}
