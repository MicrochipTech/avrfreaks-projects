/* ========================================================================== */
/*                                                                            */
/*   funclib.h                                                                */
/*   (c) 2012 Gi@cky98                                                        */
/*                                                                            */
/* ========================================================================== */

#include <stdio.h>

#ifndef _FUNCLIB
	#define _FUNCLIB

	void debugn(byte n)
	{
		// Put here your debug code
		
		// For windows, for example, use:
		//printf("DEBUG\t\t0x%02X\t\t%i\n", n, n);
		
		// For AVR, for example, use:
		//uart_sendstring("DEBUG ");
		//uart_sendnumber(n);
		//uart_sendstring("\n");
		return;
	}
	
	void errorn(byte n)
	{
		// Put here your error printing code
		
		// For windows, for example, use:
		//printf("ERROR\t\t0x%02X\t\t%i\n", n, n);
		
		// For AVR, for example, use:
		//uart_sendstring("ERROR ");
		//uart_sendnumber(n);
		//uart_sendstring("\n");
		return;
	}
	
	/*int myfunc(byte b[16])
	{
		printf("Custom user function, parameters: %i, %i, %i\n", p1, p2, p3);
	}*/
	
	void _inituserfunc(void)
	{
		// Put here your function library initialization code
		// es: _sbifuncs[0] = &myfunc;
	}
	
#endif
