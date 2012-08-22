// File: main.c
// 
// xmega demo program VG Nov 2010
//
// CPU:   xmega32A4
// Clock: 14.7 Mhz (Crystal)
//
//
// Version:
// 1.0.0		23.06.2010	initial Version
// 1.0.1        15.11.2010  FreeRTOS-port demo
//
#include "all.h"

// task handles
xTaskHandle hUSARTC0_Send;
xTaskHandle hUSARTE0_Receive;

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName );

typedef void tskTCB;
extern volatile tskTCB * volatile pxCurrentTCB;

int main(void)
{
	unsigned portBASE_TYPE txPriority;
	unsigned portBASE_TYPE rxPriority;

	//init clock
	vInitClock();

	// all usart preparation including port pin setup
	initUsarts();

	// prepare the queues that hold the data that is ready to be sent out
	initChannelQueues();

	//Init port settings (other than serial)
	vPortPreparation();

	// Create the tasks
	// Tx has to have a higher prio, it otherwise does not get enough cpu time 
	// when using the higher baudrates on the receiving side.
	txPriority = tskIDLE_PRIORITY+2;
	rxPriority = tskIDLE_PRIORITY+1;

	xTaskCreate( vUSARTC0_Send,   ( signed char * ) "uc0s", configMINIMAL_STACK_SIZE+50, NULL, txPriority, &hUSARTC0_Send );
	xTaskCreate( vUSARTE0_Receive,( signed char * ) "ue0r", configMINIMAL_STACK_SIZE+50, NULL, rxPriority, &hUSARTE0_Receive );


	// for serial data simulation
#ifdef SERIAL_SIMULATION
	setupTimer1Interrupt();
#endif

	vTaskStartScheduler();

	return 0;
}


// to report kernel detected stack overflows
//
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
	/* Just to stop compiler warnings. */
	(void) pxTask;
	(void) pcTaskName;

	error(ERR_STACK_OVERFLOW, ( xTaskHandle )pxCurrentTCB);

}

void error(u8 errCode, xTaskHandle *pxTask)
{
	u8 a;

	/* Just to stop compiler warnings. */
	(void) pxTask;

	if(errCode==ERR_STACK_OVERFLOW)
		a = 1;
	else if(errCode==ERR_QUEUE_SEND_FAILED)
		a = 2;
    else
		a = 3;

// TODO from here:
//
// - log the occurance of any error in flash mem (inc counter)
// - restart from main


}



//---- end of file main.c -------------------------------------------------------


