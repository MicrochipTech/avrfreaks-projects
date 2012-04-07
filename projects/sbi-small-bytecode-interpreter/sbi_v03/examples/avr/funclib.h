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
	
	int l_delay(byte b[16])
	{
		byte p = _getval(b[0], b[1]);
		while (p--) _delay_ms(1);
		return 0;
	}
	
	int l_out(byte b[16])
	{
		if (_getval(b[0], b[1])>0)
			PORTA = 0x01;
		else
			PORTA = 0x00;
		return 0;
	}
	
	int l_in(byte b[16])
	{
		DDRA=0x0F;
		if (PINA & 0x10)
		{
			_setval(b[0], b[1], 1);
		}			
		else
		{
			_setval(b[0], b[1], 0);
		}
		return 0;
	}
	
	void _inituserfunc(void)
	{
		_sbifuncs[0] = &l_delay;
		_sbifuncs[1] = &l_out;
		_sbifuncs[2] = &l_in;
	}
	
#endif