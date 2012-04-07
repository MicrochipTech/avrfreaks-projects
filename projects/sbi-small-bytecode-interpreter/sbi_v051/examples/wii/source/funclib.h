/* ========================================================================== */
/*                                                                            */
/*   funclib.h                                                                */
/*   (c) 2012 Gi@cky98                                                        */
/*                                                                            */
/* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

#include <grrlib.h>

// RGBA Colors
#define GRRLIB_BLACK   0x000000FF
#define GRRLIB_MAROON  0x800000FF
#define GRRLIB_GREEN   0x008000FF
#define GRRLIB_OLIVE   0x808000FF
#define GRRLIB_NAVY    0x000080FF
#define GRRLIB_PURPLE  0x800080FF
#define GRRLIB_TEAL    0x008080FF
#define GRRLIB_GRAY    0x808080FF
#define GRRLIB_SILVER  0xC0C0C0FF
#define GRRLIB_RED     0xFF0000FF
#define GRRLIB_LIME    0x00FF00FF
#define GRRLIB_YELLOW  0xFFFF00FF
#define GRRLIB_BLUE    0x0000FFFF
#define GRRLIB_FUCHSIA 0xFF00FFFF
#define GRRLIB_AQUA    0x00FFFFFF
#define GRRLIB_WHITE   0xFFFFFFFF

#ifndef _FUNCLIB
	#define _FUNCLIB

	void debugn(byte n)
	{
		// Put here your debug code
		printf("DEBUG\t\t0x%02X\t\t%i\n", n, n);
		return;
	}
	
	void errorn(byte n)
	{
		// Put here your error printing code
		printf("ERROR\t\t0x%02X\t\t%i\n", n, n);
		return;
	}
	
	int grrinit(byte b[16])
	{
		GRRLIB_Init();
		return 0;
	}
	
	int clrscreen(byte b[16])
	{
		GRRLIB_FillScreen(GRRLIB_BLACK);    // Clear the screen
		return 0;
	}
	
	int drawline(byte b[16])
	{
		GRRLIB_Line(_getval(b[0], b[1]), _getval(b[2], b[3]), _getval(b[4], b[5]), _getval(b[6], b[7]), GRRLIB_WHITE);
		return 0;
	}
	
	int render(byte b[16])
	{
		GRRLIB_Render();
		return 0;
	}
	
	int grrexit(byte b[16])
	{
		GRRLIB_Exit();
		return 0;
	}
	
	void _inituserfunc(void)
	{
		// Put here your function library initialization code
		_sbifuncs[0] = &grrinit;
		_sbifuncs[1] = &clrscreen;
		_sbifuncs[2] = &drawline;
		_sbifuncs[3] = &render;
		_sbifuncs[4] = &grrexit;
	}
	
#endif
