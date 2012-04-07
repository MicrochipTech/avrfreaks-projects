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
	
	void errorn(int n)
	{
		printf("ERROR\t\t0x%02X\t\t%i\n", n, n);
		return;
	}
	
	int myfunc(byte b[16])
	{
		printf("Custom user function, parameters: %i, %i, %i, %i, %i, %i, %i, %i\n", _getval(b[0], b[1]), _getval(b[2], b[3]), _getval(b[4], b[5]), _getval(b[6], b[7]), _getval(b[8], b[9]), _getval(b[10], b[11]), _getval(b[12], b[13]), _getval(b[14], b[15]));
		return 0;
	}
	
	int msgbox(byte b[16])
	{
		char message[512];
		char buf[4];
		strcpy(message, "First parameter: ");
		itoa(_getval(b[0], b[1]), buf, 10);
		strcat(message, (const char*)buf);
		strcat(message, "\nSecond parameter: ");
		itoa(_getval(b[2], b[3]), buf, 10);
		strcat(message, (const char*)buf);
		strcat(message, "\nThird parameter: ");
		itoa(_getval(b[4], b[5]), buf, 10);
		strcat(message, (const char*)buf);
		MessageBox(0, (const char*)message, "SBI Runtime", 1);
		return 0;
	}
	
	int getnum(byte b[16])
	{
		int n;
		printf("Enter a number: ");
		scanf("%i", &n);
		_setval(b[0], b[1], (byte)n);
		return 0;
	}
	
	void _inituserfunc(void)
	{
		_sbifuncs[0] = &myfunc;
		_sbifuncs[1] = &msgbox;
		_sbifuncs[2] = &getnum;
	}
	
#endif