//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  kernelwdt.h
	
	Description:
		AVR XMEGA WDT Header
*/
//--------------------------------------------------------------------------- 
#ifndef __KERNELWDT_H_
#define __KERNELWDT_H_

void KernelWDT_Config(void);
void KernelWDT_Start(void);
void KernelWDT_Stop(void);
void KernelWDT_Kick(void);

#endif //__KERNELWDT_H_
