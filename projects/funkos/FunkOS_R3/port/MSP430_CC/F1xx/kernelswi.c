//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  kernelswi.c

	Description:
		Utility functions for kernel software interrupt functionality
*/
#include "types.h"

#include <msp430.h>

//---------------------------------------------------------------------------
#define SWI0_VECTOR_ADDR		(0xFFE8)
extern void SWI0(void);

//---------------------------------------------------------------------------
void KernelSWI_Config(void)
{
  *(volatile USHORT *)(SWI0_VECTOR_ADDR) = *(USHORT*)SWI0;
  
  // Set P1IFG to true
  P1DIR &= ~0x01;  
}

//---------------------------------------------------------------------------
void KernelSWI_Start(void)
{
    P1IE |= 0x01;
}

//---------------------------------------------------------------------------
void KernelSWI_Stop(void)
{
    P1IE &= ~0x01;
}

//---------------------------------------------------------------------------
void KernelSWI_Clear(void)
{
    P1IFG &= ~0x01;
}

//---------------------------------------------------------------------------
void KernelSWI_Trigger(void)
{
    P1IFG |= 0x01;
}
