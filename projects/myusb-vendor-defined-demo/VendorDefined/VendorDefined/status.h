/*
	Status.h
	
	Functions for logging program status to the serial port, to
	be used for debugging pruposes etc.
	
	2008-03-21, P.Harvey-Smith.
	
*/

#include "AVRPort.h"

#ifndef __STATUS_DEFINES__
#define __STATUS_DEFINES__

#ifdef SERIAL_STATUS
#define log(format,...) printf_P(PSTR(format),##__VA_ARGS__)
#else
#define log(format,...) 
#endif

void cls(void);
void Init_Serial(void);
void DecodeRequest(USB_HIDReport_Data_t *AVRIORequest);


#endif
