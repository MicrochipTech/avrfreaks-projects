/********************************************************************
	created:	2012/03/05
	created:	5:3:2012   18:40
	filename: 	C:\Users\Wedge\Dropbox\school\Thesis\Code\AVR\FreeRTOS_UC3L0\FreeRTOS_UC3L0\src\adc.h
	file path:	C:\Users\Wedge\Dropbox\school\Thesis\Code\AVR\FreeRTOS_UC3L0\FreeRTOS_UC3L0\src
	file base:	adc
	file ext:	h
	author:		Dustin Sanders
	
	purpose:	Header file for ADC Data Handler
*********************************************************************/
  
#ifndef ADC_H_
#define ADC_H_

//#include <asf.h>
#include "avr32/io.h"
#include "gloabals.h"



portBASE_TYPE lADCInit( void );

portBASE_TYPE lADC_PDCAInit( void );

portBASE_TYPE lADCStartHandler( unsigned portBASE_TYPE uxPriority );

void vADCHandler( void *pvParameters );

portBASE_TYPE lIsADCHandlerRunning( void );

void vADCTaskDelay( portBASE_TYPE uxDelayMS );

portBASE_TYPE lADCHandleMenu( U8 *menuPtr );

void vADCHandleDisplay ( unsigned long dev );

#endif /* ADC_H_ */