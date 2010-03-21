//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  kerneltimer.c

	Description:
		Utility functions for kernel timer functionality
*/
#include "types.h"
#include "kernelcfg.h"

#include <msp430.h>

//---------------------------------------------------------------------------
#define TIMERA0_ISR_VECTOR		(0xFFEC)
extern void Tick(void);

//---------------------------------------------------------------------------
void KernelTimer_Config(void)
{	
    *(volatile USHORT *)(TIMERA0_ISR_VECTOR) = *(USHORT*)Tick;
    
    TACTL = 0;
    TACTL |= TASSEL_1;
    TACTL |= TACLR;
    TACCR0 = SYSTEM_FREQ / TICK_FREQ;
}

//---------------------------------------------------------------------------
void KernelTimer_Start(void)
{
	TACTL |= MC_1;  
    TACCTL0 |= CCIE;
    TACTL |= TAIE;
}

//---------------------------------------------------------------------------
void KernelTimer_Stop(void)
{
    TACCTL0 &= ~CCIE;
    TACTL &= ~TAIE;
}

void Timer_ISR