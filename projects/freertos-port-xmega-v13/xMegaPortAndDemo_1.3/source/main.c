//
// File: main.c
// 
// xmega demo program VG Feb,Mar 2011
//
// CPU:   atxmega32A4, atxmega128a1
//
//
// Version:
// 1.0.0		23.06.2010	initial Version
// 1.0.1        15.11.2010  FreeRTOS-port demo for 32A4 only.
// 1.1.0		13.02.2011	necessary changes for 128A1.
//							streamlined context switch.
//							interrupt nesting demonstrated in demo code.
// 1.2.0        12.03.2011  now compiles without error with configMAX_SYSCALL_INTERRUPT_PRIORITY=0.
//                          portTaskIncrementTick no more inlined. The inlined code used r17 and 
//                             this did not allow to use the option not to save r2-r17 in the context save/restore.
//                          more cosmetic changes.
// 1.2.1        27.11.2011  Memory checks to avoid all kinds of overflow or misconfiguration
//
#include "all.h"
#include "mem_check.h"

//extern u8 __data_start;
//extern u8 __data_end;
extern u8 __bss_start;
extern u8 __bss_end;
extern u8 __heap_start;
	

// task handles
xTaskHandle hUSARTC0_Send;
xTaskHandle hUSARTC0_Receive;

//xTaskHandle hUSARTC1_Send;
xTaskHandle hUSARTC1_Receive;

xTaskHandle hUSARTD0_Send;
//xTaskHandle hUSARTD0_Receive;

xTaskHandle hUSARTD1_Receive;

xTaskHandle hUSARTE0_Receive;

xTaskHandle hSwitchPrios;



extern void vApplicationIdleHook( void );

void vSwitchPrios(void *pvParameters);

u32 idleCnt;

//----------------------------------------------
//
void vApplicationIdleHook( void )
{
	
	idleCnt++;

	#if RUNTIME_CHECKS == 1
	checkAllStacks();
	#endif

	SLEEPMGR_PREPARE_SLEEP(SLEEP_SMODE_IDLE_gc);
	cpu_sleep();
	SLEEPMGR_DISABLE_SLEEP();
}


//----------------------------------------------
//
int main(void)
{
	unsigned portBASE_TYPE txPriority,rxPriority;

	// knowing the reason for a reset may be of value
	// if you want to log/track sporadically  occurring
	// problems

	// software reset ?
	//
	if( RST.STATUS & RST_SRF_bm )
	{
		// reset this bit
		RST.STATUS = RST_SRF_bm;
	}
	// power on reset ?
	else if( RST.STATUS & RST_PORF_bm)
	{
		// reset this bit
		RST.STATUS = RST_PORF_bm;
	}
	// debugger reset ?
	else if( RST.STATUS & RST_PDIRF_bm)
	{
		// reset this bit
		RST.STATUS = RST_PDIRF_bm;
	}
	// external reset ?
	else if( RST.STATUS & RST_EXTRF_bm)
	{
		// reset this bit
		RST.STATUS = RST_EXTRF_bm;
	}

	size_t bytesLeft0 = xPortGetFreeHeapSize();
	// consistency check
	if(bytesLeft0 != configTOTAL_HEAP_SIZE)
	{
		error(ERR_LOW_HEAP_SPACE);
	}
	
	/*
	// try to understand the memory layout!
	// these values can help you
	//
	u8* be = (u8*)&__bss_start;  // the rtos heap is located within .bss !
	u8* hs = (u8*)&__bss_end;
	u8* bs = (u8*)&__heap_start; // this heap is not used by the FreeRTOS ! (since malloc() is not used)
	*/

	// There must be a minimum stack size to get the initializations done 
	// until the RTOS is running.
	// bss_end depends on configTOTAL_HEAP_SIZE. Decreasing configTOTAL_HEAP_SIZE gives
	// you room for the global stack.
	//
	s16 freeSpaceOnGlobalStack = SP - (uint16_t) &__bss_end;

	// this value is a guess. It is double checked later on.
	#define MINIMUM_STACK_NEEDED 60
	if( freeSpaceOnGlobalStack < MINIMUM_STACK_NEEDED )
	{
		error(ERR_HEAP_TOO_LARGE); // decrease configTOTAL_HEAP_SIZE !
	}

	// how much stack do we have used up to now?
	u16 minimumStackLeft_before = get_mem_unused();
	// this should be nearly identical to freeSpaceOnGlobalStack
	// (so this is another consistency check)
	if( freeSpaceOnGlobalStack - minimumStackLeft_before > 10 )
	{
		error(ERR_HEAP_TOO_LARGE); // decrease configTOTAL_HEAP_SIZE !
	}
	//Init clock
	vInitClock();

	// all usart preparation including port pin setup
	initUsarts();

	// prepare the queues that hold the data that is ready to be sent out
	initChannelQueues();

	//Init port settings (other than serial)
	vPortPreparation();

	// Unused hardware modules are disabled to save power.
	disableUnusedModules();
	// Create the tasks
	txPriority = tskIDLE_PRIORITY+1;
	rxPriority = tskIDLE_PRIORITY+1;

	xTaskCreate( vUSARTE0_Receive,( signed char * ) "ue0r", configMINIMAL_STACK_SIZE, NULL, rxPriority, &hUSARTE0_Receive );
	xTaskCreate( vUSARTC1_Receive,( signed char * ) "uc1r", configMINIMAL_STACK_SIZE, NULL, rxPriority, &hUSARTC1_Receive );
	xTaskCreate( vUSARTC0_Send,   ( signed char * ) "uc0s", configMINIMAL_STACK_SIZE, NULL, txPriority, &hUSARTC0_Send );
	xTaskCreate( vSwitchPrios,	  ( signed char * ) "spri", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &hSwitchPrios );
	//TODO check return codes

	// for serial data simulation
	// two timers are used to simulate serial input
#if SERIAL_SIMULATION_C1 == 1
	setupTimerCC1Interrupt();
#endif
#if SERIAL_SIMULATION_E0 == 1
	setupTimerCE0Interrupt();
#endif

#if NESTING_TEST == 1
	setupTimerCD0Interrupt(); // to test the medium level nesting ints
	setupTimerCD1Interrupt(); // to test the high level nesting ints
#endif

	// Check if there was more stack used than guessed (see above: MINIMUM_STACK_NEEDED).
	// minimumStackLeft is the number of bytes between bss-end and global (non-rtos) stack.
	// It is the minimum free stack that we had until now.
	// It must be >0 to ensure that the allocated data in .bss is not
	// touched during initialization of the tasks. 
	// If this error occurs the rtos heap size must be reduced. (The rtos stack
	// is located in .bss , but before the rtos scheduler is starting, the global 
	// stack at the top of the ram is used.)
	u16 minimumStackLeft_after = get_mem_unused();
	if( minimumStackLeft_after == 0)
	{
		error(ERR_LOW_GLOBAL_STACK_SPACE);
	}
		
	// Check the # of free bytes on the rtos-heap that may still be allocated by pvPortMalloc().
	// (the rtos heap size is specified in FreeRTOSConfig.h)
	size_t bytesLeft3 = xPortGetFreeHeapSize();

	// There must be a minimum of free rtos-heap space left
	// to successfully create the idle task in vTaskStartScheduler().
	#define MIN_STACKSIZE_TO_CREATE_IDLE_TASK 37 + configMINIMAL_STACK_SIZE
	if(bytesLeft3 < ( MIN_STACKSIZE_TO_CREATE_IDLE_TASK ) )
	{
		// It will not be possible to create the idle task.
		// If we don't stop execution here, it will stop immediately after 
		// because of other runtime stack checks.
		//		
		error(ERR_LOW_HEAP_SPACE);
	}

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
		checkAllStacks();

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




//---- end of file main.c -------------------------------------------------------


