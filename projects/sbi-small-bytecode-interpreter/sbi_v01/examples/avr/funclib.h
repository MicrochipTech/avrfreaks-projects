/* ========================================================================== */
/*                                                                            */
/*   funclib.h                                                                */
/*   (c) 2012 Gi@cky98                                                        */
/*                                                                            */
/* ========================================================================== */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

#ifndef _FUNCLIB
	#define _FUNCLIB

	void debugn(int n)
	{
		SPrint("Debug :: ");
		SPrintN(n);
		SPrint("\n");
		return;
	}
	
	int l_delay(byte p1, byte p2, byte p3)
	{
		while (p1--) _delay_ms(1);
		return 0;
	}
	
	int l_out(byte p1, byte p2, byte p3)
	{
		PORTA=p1;
		return 0;
	}
	
	void _inituserfunc(void)
	{
		_sbifuncs[0] = &l_delay;
		_sbifuncs[1] = &l_out;
	}
	
#endif