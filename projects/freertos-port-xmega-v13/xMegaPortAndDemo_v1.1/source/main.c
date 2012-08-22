//
// File: main.c
// 
// xmega demo program VG Feb 2011
//
// CPU:   atxmega32A4, atxmega128a1
//
//
// Version:
// 1.0.0		23.06.2010	initial Version
// 1.0.1        15.11.2010  FreeRTOS-port demo
// 1.1.0		13.02.2011	necessary changes for 128A1
//							streamlined context switch
//							interrupt nesting demonstrated in demo code
//
#include "all.h"

// task handles
xTaskHandle hUSARTC0_Send;
xTaskHandle hUSARTC1_Receive;
xTaskHandle hUSARTE0_Receive;
xTaskHandle hSwitchPrios;

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName );
void vSwitchPrios(void *pvParameters);

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
	txPriority = tskIDLE_PRIORITY+1;
	rxPriority = tskIDLE_PRIORITY+1;

	xTaskCreate( vUSARTC0_Send,   ( signed char * ) "uc0s", configMINIMAL_STACK_SIZE, NULL, txPriority, &hUSARTC0_Send );
	xTaskCreate( vUSARTE0_Receive,( signed char * ) "ue0r", configMINIMAL_STACK_SIZE, NULL, rxPriority, &hUSARTE0_Receive );
	xTaskCreate( vUSARTC1_Receive,( signed char * ) "uc1r", configMINIMAL_STACK_SIZE, NULL, rxPriority, &hUSARTC1_Receive );
	xTaskCreate( vSwitchPrios,	  ( signed char * ) "spri", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &hSwitchPrios );


	// for serial data simulation
#ifdef SERIAL_SIMULATION
	setupTimerCC1Interrupt();
	setupTimerCE0Interrupt();
#endif

#if 1
	setupTimerCD0Interrupt(); // to test the medium level nesting ints
	setupTimerCD1Interrupt(); // to test the high level nesting ints
#endif

	vTaskStartScheduler();

	return 0;
}

//===================
// This task switches the prios of the other task.
// It is used only for test purposes, to stress this 
// demo code.
//
void vSwitchPrios(void *pvParameters)
{
	// Just to stop compiler warnings.
	(void) pvParameters;

	const portTickType xDelay = 50 / portTICK_RATE_MS;

    for( ;; )
    {
        vTaskDelay( xDelay );
		vTaskPrioritySet( hUSARTC0_Send, 	tskIDLE_PRIORITY + 1 );
		vTaskPrioritySet( hUSARTE0_Receive, tskIDLE_PRIORITY + 1 );
		vTaskPrioritySet( hUSARTC1_Receive, tskIDLE_PRIORITY + 1 );

        vTaskDelay( xDelay );
		vTaskPrioritySet( hUSARTC0_Send, 	tskIDLE_PRIORITY + 2 );
		vTaskPrioritySet( hUSARTE0_Receive, tskIDLE_PRIORITY + 1 );
		vTaskPrioritySet( hUSARTC1_Receive, tskIDLE_PRIORITY + 1 );

        vTaskDelay( xDelay );
		vTaskPrioritySet( hUSARTC0_Send, 	tskIDLE_PRIORITY + 1 );
		vTaskPrioritySet( hUSARTE0_Receive, tskIDLE_PRIORITY + 2 );
		vTaskPrioritySet( hUSARTC1_Receive, tskIDLE_PRIORITY + 1 );

        vTaskDelay( xDelay );
		vTaskPrioritySet( hUSARTC0_Send, 	tskIDLE_PRIORITY + 1 );
		vTaskPrioritySet( hUSARTE0_Receive, tskIDLE_PRIORITY + 1 );
		vTaskPrioritySet( hUSARTC1_Receive, tskIDLE_PRIORITY + 2 );

        vTaskDelay( xDelay );
		vTaskPrioritySet( hUSARTC0_Send, 	tskIDLE_PRIORITY + 2 );
		vTaskPrioritySet( hUSARTE0_Receive, tskIDLE_PRIORITY + 2 );
		vTaskPrioritySet( hUSARTC1_Receive, tskIDLE_PRIORITY + 1 );

        vTaskDelay( xDelay );
		vTaskPrioritySet( hUSARTC0_Send, 	tskIDLE_PRIORITY + 1 );
		vTaskPrioritySet( hUSARTE0_Receive, tskIDLE_PRIORITY + 2 );
		vTaskPrioritySet( hUSARTC1_Receive, tskIDLE_PRIORITY + 2 );
    }

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


