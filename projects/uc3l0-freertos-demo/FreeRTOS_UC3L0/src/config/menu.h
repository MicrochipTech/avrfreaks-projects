/********************************************************************
	created:	2012/03/04
	created:	4:3:2012   3:13
	filename: 	FreeRTOS_UC3L0\src\config\menu.h
	file path:	FreeRTOS_UC3L0\src\config
	file base:	menu
	file ext:	h
	author:		Dustin Sanders
	
	purpose:	Header for the Main Menu Task
*********************************************************************/


#ifndef MENU_H_
#define MENU_H_

#include "print_funcs.h"
#include "serial.h"
#include "board.h"
#include "gloabals.h"
#include "adc.h"

/************************************************************************/
/* Main Menu Prototypes                                                 */
/************************************************************************/
//************************************
// Method:    vStartMainMenu
// FullName:  vStartMainMenu
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: unsigned portBASE_TYPE uxPriority
//************************************
portBASE_TYPE lMenuStartTerminal( unsigned portBASE_TYPE uxPriority );
//************************************
// Method:    vHandleMenus
// FullName:  vHandleMenus
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void * pvParameters
//************************************
void vMenuHandleTerminal(  void *pvParameters  );
//************************************
// Method:    xMainMenuRunning
// FullName:  xMainMenuRunning
// Access:    public 
// Returns:   portBASE_TYPE
// Qualifier:
// Parameter: void
//************************************
portBASE_TYPE lMenuTerminalRunning( void );   //How to check if its still running?

//portBASE_TYPE xMenuHandlePWM( int8_t *menuPtr );
 
//Prints lots of data 
portBASE_TYPE lMenuHandleData( U8 *menuPtr );

// Task to continuously print out data
// Parameters determine whats output, refresh rate and to where
//      (usart, usb, ethernet, can, i2c data buffer)
void vMenuHandleDataTask( void *pvParameters );

void vMenuHandleOSDisplay( unsigned long dev );

void vMenuHadleHistory( U8 *menuPtr );

#endif /* MENU_H_ */