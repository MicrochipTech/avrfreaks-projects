/*
	FreeRTOS.org V5.2.0 - Copyright (C) 2003-2009 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS.org is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS.org; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS.org, without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

    ***************************************************************************
    ***************************************************************************
    *                                                                         *
    * SAVE TIME AND MONEY!  We can port FreeRTOS.org to your own hardware,    *
    * and even write all or part of your application on your behalf.          *
    * See http://www.OpenRTOS.com for details of the services we provide to   *
    * expedite your project.                                                  *
    *                                                                         *
    ***************************************************************************
    ***************************************************************************

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and 
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety 
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting, 
	licensing and training services.
*/

/*******************************************************************************
     FreeRTOS port to the CodeVisionAVR C compiler V2.04.0 or later
     by Pavel Haiduc, HP InfoTech s.r.l. www.hpinfotech.com
     V1.0, 16 March 2009
********************************************************************************/

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo file headers. */
#include "regtest.h"

/*
 * Test tasks that sets registers to known values, then checks to ensure the
 * values remain as expected.  Test 1 and test 2 use different values.
 */
static void prvRegisterCheck1( void *pvParameters );
static void prvRegisterCheck2( void *pvParameters );

/* Set to a non zero value should an error be found. */
portBASE_TYPE xRegTestError = pdFALSE;

/*-----------------------------------------------------------*/

void vStartRegTestTasks( void )
{
    signed portCHAR temp1[]="Reg1";
    signed portCHAR temp2[]="Reg2";
    
	xTaskCreate( prvRegisterCheck1, temp1, configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( prvRegisterCheck2, temp2, configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );		
}
/*-----------------------------------------------------------*/

portBASE_TYPE xAreRegTestTasksStillRunning( void )
{
portBASE_TYPE xReturn;

	/* If a register was found to contain an unexpected value then the
	xRegTestError variable would have been set to a non zero value. */
	if( xRegTestError == pdFALSE )
	{
		xReturn = pdTRUE;
	}
	else
	{
		xReturn = pdFALSE;
	}
	
	return xReturn;
}
/*-----------------------------------------------------------*/

static void prvRegisterCheck1( void *pvParameters )
{
	for( ;; )
	{
    #asm
		LDI	r31, 5		
		MOV	r0, r31
		LDI	r31, 6
		MOV	r1, r31
		LDI	r31, 7
		MOV	r2, r31
		LDI	r31, 8
		MOV	r3, r31
		LDI	r31, 9
		MOV	r4, r31
		LDI	r31, 10
		MOV	r5, r31
		LDI	r31, 11
		MOV	r6, r31
		LDI	r31, 12
		MOV	r7, r31
		LDI	r31, 13
		MOV	r8, r31
		LDI	r31, 14
		MOV	r9, r31
		LDI	r31, 15
		MOV	r10, r31
		LDI	r31, 16
		MOV	r11, r31
		LDI	r31, 17
		MOV	r12, r31
		LDI	r31, 18
		MOV	r13, r31
		LDI	r31, 19
		MOV	r14, r31
		LDI	r31, 20
		MOV	r15, r31
		LDI	r16, 21
		LDI	r17, 22
		LDI	r18, 23
		LDI	r19, 24
		LDI	r20, 25
		LDI	r21, 26
		LDI	r22, 27
		LDI	r23, 28
		LDI	r24, 29
		LDI	r25, 30
		LDI	r26, 31
		LDI	r27, 32
		LDI	r30, 33

		LDI	r31, 5
		CPSE r31, r0
		STS	_xRegTestError, r0
		LDI	r31, 6
		CPSE r31, r1
		STS	_xRegTestError, r0
		LDI	r31, 7
		CPSE r31, r2
		STS	_xRegTestError, r0
		LDI	r31, 8
		CPSE r31, r3
		STS	_xRegTestError, r0
		LDI	r31, 9
		CPSE r31, r4
		STS	_xRegTestError, r0
		LDI	r31, 10
		CPSE r31, r5
		STS	_xRegTestError, r0
		LDI	r31, 11
		CPSE r31, r6
		STS	_xRegTestError, r0
		LDI	r31, 12
		CPSE r31, r7
		STS	_xRegTestError, r0
		LDI	r31, 13
		CPSE r31, r8
		STS	_xRegTestError, r0
		LDI	r31, 14
		CPSE r31, r9
		STS	_xRegTestError, r0
		LDI	r31, 15
		CPSE r31, r10
		STS	_xRegTestError, r0
		LDI	r31, 16
		CPSE r31, r11
		STS	_xRegTestError, r0
		LDI	r31, 17
		CPSE r31, r12
		STS	_xRegTestError, r0
		LDI	r31, 18
		CPSE r31, r13
		STS	_xRegTestError, r0
		LDI	r31, 19
		CPSE r31, r14
		STS	_xRegTestError, r0
		LDI	r31, 20
		CPSE r31, r15
		STS	_xRegTestError, r0
		LDI	r31, 21
		CPSE r31, r16
		STS	_xRegTestError, r0
		LDI	r31, 22
		CPSE r31, r17
		STS	_xRegTestError, r0
		LDI	r31, 23
		CPSE r31, r18
		STS	_xRegTestError, r0
		LDI	r31, 24
		CPSE r31, r19
		STS	_xRegTestError, r0
		LDI	r31, 25
		CPSE r31, r20
		STS	_xRegTestError, r0
		LDI	r31, 26
		CPSE r31, r21
		STS	_xRegTestError, r0
		LDI	r31, 27
		CPSE r31, r22
		STS	_xRegTestError, r0
		LDI	r31, 28
		CPSE r31, r23
		STS	_xRegTestError, r0
		LDI	r31, 29
		CPSE r31, r24
		STS	_xRegTestError, r0
		LDI	r31, 30
		CPSE r31, r25
		STS	_xRegTestError, r0
		LDI	r31, 31
		CPSE r31, r26
		STS	_xRegTestError, r0
		LDI	r31, 32
		CPSE r31, r27
		STS	_xRegTestError, r0
		LDI	r31, 33
		CPSE r31, r30
		STS	_xRegTestError, r0
    #endasm
	}
}
/*-----------------------------------------------------------*/

static void prvRegisterCheck2( void *pvParameters )
{
	for( ;; )
	{
    #asm
		LDI	r31, 1		
		MOV	r0, r31
		LDI	r31, 2
		MOV	r1, r31
		LDI	r31, 3
		MOV	r2, r31
		LDI	r31, 4
		MOV	r3, r31
		LDI	r31, 5
		MOV	r4, r31
		LDI	r31, 6
		MOV	r5, r31
		LDI	r31, 7
		MOV	r6, r31
		LDI	r31, 8
		MOV	r7, r31
		LDI	r31, 9
		MOV	r8, r31
		LDI	r31, 10
		MOV	r9, r31
		LDI	r31, 11
		MOV	r10, r31
		LDI	r31, 12
		MOV	r11, r31
		LDI	r31, 13
		MOV	r12, r31
		LDI	r31, 14
		MOV	r13, r31
		LDI	r31, 15
		MOV	r14, r31
		LDI	r31, 16
		MOV	r15, r31
		LDI	r16, 17
		LDI	r17, 18
		LDI	r18, 19
		LDI	r19, 20
		LDI	r20, 21
		LDI	r21, 22
		LDI	r22, 23
		LDI	r23, 24
		LDI	r24, 25
		LDI	r25, 26
		LDI	r26, 27
		LDI	r27, 28
		LDI	r30, 29

		LDI	r31, 1
		CPSE r31, r0
		STS	_xRegTestError, r0
		LDI	r31, 2
		CPSE r31, r1
		STS	_xRegTestError, r0
		LDI	r31, 3
		CPSE r31, r2
		STS	_xRegTestError, r0
		LDI	r31, 4
		CPSE r31, r3
		STS	_xRegTestError, r0
		LDI	r31, 5
		CPSE r31, r4
		STS	_xRegTestError, r0
		LDI	r31, 6
		CPSE r31, r5
		STS	_xRegTestError, r0
		LDI	r31, 7
		CPSE r31, r6
		STS	_xRegTestError, r0
		LDI	r31, 8
		CPSE r31, r7
		STS	_xRegTestError, r0
		LDI	r31, 9
		CPSE r31, r8
		STS	_xRegTestError, r0
		LDI	r31, 10
		CPSE r31, r9
		STS	_xRegTestError, r0
		LDI	r31, 11
		CPSE r31, r10
		STS	_xRegTestError, r0
		LDI	r31, 12
		CPSE r31, r11
		STS	_xRegTestError, r0
		LDI	r31, 13
		CPSE r31, r12
		STS	_xRegTestError, r0
		LDI	r31, 14
		CPSE r31, r13
		STS	_xRegTestError, r0
		LDI	r31, 15
		CPSE r31, r14
		STS	_xRegTestError, r0
		LDI	r31, 16
		CPSE r31, r15
		STS	_xRegTestError, r0
		LDI	r31, 17
		CPSE r31, r16
		STS	_xRegTestError, r0
		LDI	r31, 18
		CPSE r31, r17
		STS	_xRegTestError, r0
		LDI	r31, 19
		CPSE r31, r18
		STS	_xRegTestError, r0
		LDI	r31, 20
		CPSE r31, r19
		STS	_xRegTestError, r0
		LDI	r31, 21
		CPSE r31, r20
		STS	_xRegTestError, r0
		LDI	r31, 22
		CPSE r31, r21
		STS	_xRegTestError, r0
		LDI	r31, 23
		CPSE r31, r22
		STS	_xRegTestError, r0
		LDI	r31, 24
		CPSE r31, r23
		STS	_xRegTestError, r0
		LDI	r31, 25
		CPSE r31, r24
		STS	_xRegTestError, r0
		LDI	r31, 26
		CPSE r31, r25
		STS	_xRegTestError, r0
		LDI	r31, 27
		CPSE r31, r26
		STS	_xRegTestError, r0
		LDI	r31, 28
		CPSE r31, r27
		STS	_xRegTestError, r0
		LDI	r31, 29
		CPSE r31, r30
		STS	_xRegTestError, r0
    #endasm
	}
}

