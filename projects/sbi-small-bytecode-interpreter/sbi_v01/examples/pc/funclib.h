/* ========================================================================== */
/*                                                                            */
/*   funclib.h                                                                */
/*   (c) 2012 Gi@cky98                                                        */
/*                                                                            */
/* ========================================================================== */

#include <stdio.h>

#include <string.h>

#include <windows.h>

#ifndef _FUNCLIB
	#define _FUNCLIB

	void debugn(int n)
	{
		printf("DEBUG\t\t0x%02X\t\t%i\n", n, n);
		return;
	}
	
	int myfunc(byte p1, byte p2, byte p3)
	{
		printf("Custom user function, parameters: %i, %i, %i\n", p1, p2, p3);
		return 0;
	}
	
	int msgbox(byte p1, byte p2, byte p3)
	{
		char message[512];
		char buf[4];
		strcpy(message, "First parameter: ");
		itoa(p1, buf, 10);
		strcat(message, (const char*)buf);
		strcat(message, "\nSecond parameter: ");
		itoa(p2, buf, 10);
		strcat(message, (const char*)buf);
		strcat(message, "\nThird parameter: ");
		itoa(p3, buf, 10);
		strcat(message, (const char*)buf);
		MessageBox(0, (const char*)message, "SBI Runtime", 1);
		return 0;
	}
	
	void _inituserfunc(void)
	{
		_sbifuncs[0] = &myfunc;
		_sbifuncs[1] = &msgbox;
	}
	
#endif