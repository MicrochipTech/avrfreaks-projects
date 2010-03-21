//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file colorspace.h

	Description:
		Color space definitions, color definition macros, and common colors 
*/
//---------------------------------------------------------------------------

#ifndef __COLORSPACE_H_
#define __COLORSPACE_H_

#include "types.h"

//---------------------------------------------------------------------------
// Module configuration flags
#define DRAW_COLOR_24BIT    0
#define DRAW_COLOR_16BIT    1
#define DRAW_COLOR_8BIT     0
#define DRAW_COLOR_1BIT     0


#if DRAW_COLOR_1BIT
//---------------------------------------------------------------------------
// Off or On
	#define COLOR				UCHAR
	#define MAX_RED				(0x01)
	#define MAX_GREEN			(0x01)
	#define MAX_BLUE			(0x01)
	#define RGB_COLOR(R,G,B)	( (R | G | B) != 0 )
	#define BYTES_PER_PIXEL		(1)
//---------------------------------------------------------------------------
#elif DRAW_COLOR_8BIT
//---------------------------------------------------------------------------
//3R:2G:3B
	#define COLOR				UCHAR
	#define MAX_RED				(0x07)
	#define MAX_GREEN			(0x03)
	#define MAX_BLUE			(0x07)
	
	#define MASK_RED(R)			(((UCHAR)R & 0x07) << 5)
	#define MASK_BLUE(B)		((UCHAR)B & 0x07)
	#define MASK_GREEN(G)		(((UCHAR)G & 0x03) << 3)
	
	#define RGB_COLOR(R,G,B)	( MASK_BLUE(B) | MASK_GREEN(G) | MASK_RED(R) )
	#define BYTES_PER_PIXEL		(1)
//---------------------------------------------------------------------------
#elif DRAW_COLOR_16BIT
//---------------------------------------------------------------------------
//5R:6G:5B
	#define COLOR				USHORT
	#define MAX_RED				(0x001F)
	#define MAX_GREEN			(0x003F)
	#define MAX_BLUE			(0x001F)
	
	#define MASK_RED(R)			(((USHORT)R & 0x001F) << 11)
	#define MASK_BLUE(B)		((USHORT)B & 0x001F)
	#define MASK_GREEN(G)		(((USHORT)G & 0x003F) << 5)
	
	#define RGB_COLOR(R,G,B)	( MASK_BLUE(B) | MASK_GREEN(G) | MASK_RED(R) )
	#define BYTES_PER_PIXEL		(2)
//---------------------------------------------------------------------------
#elif DRAW_COLOR_24BIT
//---------------------------------------------------------------------------
//8R:8G:8B
	#define COLOR	ULONG
	#define MAX_RED				(0x00FF)
	#define MAX_GREEN			(0x00FF)
	#define MAX_BLUE			(0x00FF)
	
	#define MASK_RED(R)			((ULONG)R & 0x00FF)
	#define MASK_BLUE(B)		(((ULONG)B & 0x00FF) << 16)
	#define MASK_GREEN(G)		(((ULONG)G & 0x00FF) << 8)
	
	#define RGB_COLOR(R,G,B)	( MASK_BLUE(B) | MASK_GREEN(G) | MASK_RED(R) )
	
	#define BYTES_PER_PIXEL		(3)
//---------------------------------------------------------------------------
#else
	#error "Must define a color space for graphics framework"
#endif

//---------------------------------------------------------------------------
// Primary colors
//---------------------------------------------------------------------------
#define COLOR_RED		RGB_COLOR(MAX_RED, 		0, 				0)
#define COLOR_BLUE		RGB_COLOR(0, 			0, 				MAX_BLUE)
#define COLOR_GREEN		RGB_COLOR(0, 			MAX_GREEN,		0)
#define COLOR_YELLOW	RGB_COLOR(MAX_RED,		MAX_GREEN,		0)
#define COLOR_WHITE		RGB_COLOR(MAX_RED,		MAX_GREEN,		MAX_BLUE)
#define COLOR_BLACK		RGB_COLOR(0,			0,				0)
#define COLOR_GREY75	RGB_COLOR(MAX_RED*3/4,	MAX_GREEN*3/4,	MAX_BLUE*3/4)
#define COLOR_GREY50	RGB_COLOR(MAX_RED/2,	MAX_GREEN/2,	MAX_BLUE/2)
#define COLOR_GREY25	RGB_COLOR(MAX_RED/4,	MAX_GREEN/4,	MAX_BLUE/4)

#endif
