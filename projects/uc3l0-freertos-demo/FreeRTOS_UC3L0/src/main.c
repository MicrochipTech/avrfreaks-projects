/*****************************************************************************
 *
 * \file
 *
 * \brief FreeRTOS Real Time Kernel example.
 *
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 *
 * Main. c also creates a task called "Check".  This only executes every three
 * seconds but has the highest priority so is guaranteed to get processor time.
 * Its main function is to check that all the other tasks are still operational.
 * Each task that does not flash an LED maintains a unique count that is
 * incremented each time the task successfully completes its function.  Should
 * any error occur within such a task the count is permanently halted.  The
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have
 * changed all the tasks are still executing error free, and the check task
 * toggles an LED.  Should any task contain an error at any time the LED toggle
 * will stop.
 *
 * The LED flash and communications test tasks do not maintain a count.
 *
 *****************************************************************************/

/*
    FreeRTOS V7.0.0 - Copyright (C) 2011 Real Time Engineers Ltd.


    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Environment header files. */
#include <asf.h>
#include "conf_clock.h"
#include "board.h"

/* Scheduler header files. */
#include "FreeRTOS.h"
#include "task.h"

/* App file headers. */
#include "serial.h"
#include "gloabals.h"
#include "pwmctrl.h"
#include "adc.h"
#include "menu.h"

/************************************************************************/
/* FreeRTOS Task Defines                                                */
/************************************************************************/

/*! \name Priority definitions for the tasks
 * Some tasks just use the idle priority.
 */
//! @{
#define mainMENU_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1)
#define mainADC_TASK_PRIORITY       ( tskIDLE_PRIORITY + 2)
#define mainCHECK_TASK_PRIORITY     ( tskIDLE_PRIORITY + 4 )
//! @}

//! LED that is toggled by the check task.  The check task periodically checks
//! that all the other tasks are operating without error.  If no errors are found
//! the LED is toggled.  If an error is found at any time the LED toggles faster.
#define mainCHECK_TASK_LED        ( LED1 )  

//! LED that is set upon error.
//#define mainERROR_LED             ( LED1 ) 

//! The period between executions of the check task.
#define mainCHECK_PERIOD          ( ( portTickType ) 10000 / portTICK_RATE_MS  )

//! If an error is detected in a task, the vErrorChecks task will enter in an
//! infinite loop flashing the LED at this rate.
#define mainERROR_FLASH_RATE      ( (portTickType) 500 / portTICK_RATE_MS )

// TODO: Make 2 buffers for each? when comes time to avg setup reload with
//      with that buffer. 


/************************************************************************/
/* Hardware Inits                                                       */
/************************************************************************/

void init_hw( void );

/************************************************************************/
/* Main Task Functions                                                  */
/************************************************************************/

/*
 * The task that executes at the highest priority and calls
 * prvCheckOtherTasksAreStillRunning().  See the description at the top
 * of the file.
 */
static void vErrorChecks( void *pvParameters );

/*
 * Checks that all the demo application tasks are still executing without error
 * - as described at the top of the file.
 */
static portBASE_TYPE prvCheckOtherTasksAreStillRunning( void );

/*
 * A task that exercises the memory allocator.
 */
//static void vMemCheckTask( void *pvParameters );

/*
 * Called by the check task following the detection of an error to set the
 * LEDs into a state that shows an error has beeen found.
 */
static void prvIndicateError( void );

/*-----------------------------------------------------------*/

int main( void )
{
    // Setup Clocks 
	sysclk_init();  
	
	// Make sure watchdog is asleep in his bed. Maybe use him later
	wdt_disable(); 
	
	portDBG_TRACE( CLEARSCR "Starting FreeRTOS, Please Wait..." );
	
	//Init Stuff
	init_hw();
	portDBG_TRACE( "...Board Ready." ); 
	     
	if(lPWMInit()) 
	{ 
	    portDBG_TRACE( "PWM FAILED" ); 
	}
	else
	{
		portDBG_TRACE( "...PWM Ready." );
	}		
		
		
	/* Main Tasks  */

	lMenuStartTerminal(mainMENU_TASK_PRIORITY);
	
	lADCStartHandler(mainADC_TASK_PRIORITY);

	/* Create Error Checking Task */
	if( xTaskCreate(
		vErrorChecks
		,  (const signed portCHAR *)"ErCk"
		,  configMINIMAL_STACK_SIZE
		,  NULL
		,  mainCHECK_TASK_PRIORITY
		,  NULL ) != pdPASS)
	{
	    portDBG_TRACE( CRLF CL_RED BOLD 
		    "!!!--vErrorChecks Task Failed to start--!!!" NORMAL );		
	}
    portDBG_TRACE( "...vErrorChecks Started." );
	portDBG_TRACE( CRLF CL_GREEN BOLD "That's it, Punch it Chewie!!" NORMAL );
	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was insufficient memory to create the idle
	task. */
    //portDBG_TRACE( CL_RED BOLD "He's Dead Jim!" CRLF );
	return 0;
}
/*-----------------------------------------------------------*/

/*!
 * \brief The task function for the "Check" task.
 */
static void vErrorChecks( void *pvParameters )
{

	/* The parameters are not used.  Prevent compiler warnings. */
	( void ) pvParameters;

	/* Cycle for ever, delaying then checking all the other tasks are still
	operating without error. */

	for( ;; )
	{
        vTaskDelay( mainCHECK_PERIOD );
		/* Check all other tasks are still operating without error.
		Check that vMemCheckTask did increment the counter. */
		if( ( prvCheckOtherTasksAreStillRunning() != pdFALSE ) )
		{
			/* An error has occurred in one of the tasks.
			Don't go any further and flash the LED faster to give visual
			feedback of the error. */
			prvIndicateError();
		}
		else
		{
			/* Toggle the LED if everything is okay. */
			LED_Toggle( mainCHECK_TASK_LED );
		}
	}
}
/*-----------------------------------------------------------*/


/*!
 * \brief Checks that all the demo application tasks are still executing without error.
 */
static portBASE_TYPE prvCheckOtherTasksAreStillRunning( void )
{
    static portBASE_TYPE xErrorHasOccurred = pdFALSE;

    // ADC is the only one working right now
    // Use as template
// 	if( lIsADCHandlerRunning() )
// 	{
// 		xErrorHasOccurred = pdTRUE;
// 	}
    // Add My functions here.
	
	//portDBG_TRACE( "I would normally check shit, but I'm not.");

	return ( xErrorHasOccurred );
}
/*-----------------------------------------------------------*/

static void prvIndicateError( void )
{
	/* The check task has found an error in one of the other tasks.
	Set the LEDs to a state that indicates this. */
	//LED_On(mainERROR_LED);
    portENTER_CRITICAL()
	// Enter loop until button is pressed which will call WDT to reset MCU
	while( gpio_local_get_pin_value(GPIO_PUSH_BUTTON_0) )
	{
			LED_Toggle( mainCHECK_TASK_LED );
			vTaskDelay( mainERROR_FLASH_RATE );
	}
    RESET_AVR 
}  

void init_hw( void )
{
   	/*	A little House Cleaning	 */
    // Turn off the LEDs
	LED_Off ((U32) -1); 
	// Enable SW0
	gpio_configure_pin (GPIO_PUSH_BUTTON_0, (GPIO_DIR_INPUT | GPIO_PULL_UP));	

}
