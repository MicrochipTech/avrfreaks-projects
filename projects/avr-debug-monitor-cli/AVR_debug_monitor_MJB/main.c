/*________________________________________________________________________________________*\

  File:        main.c
  Author:      M.J.Bauer
  Originated:  Feb 2007

  Project:     Stand-alone 'AVR Debug Monitor' with command line user interface (CLI).
               This version is customized for the ATmega128.

  This module comprises the mainline, R/T kernel and background tasks.
  If the "task scheduler" is too primitive for the target application,
  drop in the kernel and task scheduler routines from 'ALERT' or 'BERT'.
  (Ref:  www.hotkey.net.au/~mjbauer)

\*________________________________________________________________________________________*/

#include  "system.h"
#include  "periph.h"
#include  "cli.h"


// Functions in main module...
void  kernel( void );
void  doBackgroundTasks( void );

// Globals...
bool    b5msecTaskReq;
bool    b50mSecTaskReq;
bool    b500msecTaskReq;
uint16  gwDebugFlags;


short  main( void )
{
	initMCUports();
	initMCUtimers();
	init_UART0();
	ENABLE_GLOBAL_IRQ;
	NEW_LINE;
	CmndVersion(1, NULL);
    kernel();

    return ( 1 );
}


void  kernel( void )
{
	while ( 1 )
	{
		ServicePortInput();
		doBackgroundTasks();
	}
}


void  doBackgroundTasks( void )
{
	if ( b5msecTaskReq )        // Do 5mSec periodic task(s)...
	{
//		buttonInputScan();      // TODO
//		__watchdog_reset();     // TODO
		b5msecTaskReq = 0;
	}
	if ( b50mSecTaskReq )       // Do 50mSec periodic task(s)...
	{
		// TODO: call 50mSec tasks (if any)
		b50mSecTaskReq = 0;
	}
	if ( b500msecTaskReq )      // Do 500mSec periodic task(s)...
	{
		// TODO: call 500mSec tasks (if any)
		HEARTBEAT_LED_TOGL;
		b500msecTaskReq = 0;
	}
}


// end
