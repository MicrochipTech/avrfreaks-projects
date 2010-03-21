//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file draw.h

	Description:
		Implements basic drawing functionality.  This forms a hardware abstraction
		layer which requires a backend for rendering.
*/
//---------------------------------------------------------------------------

#ifndef __DRAW_H_
#define __DRAW_H_

#include "types.h"
#include "font.h"
#include "colorspace.h"
#include "display.h"

//---------------------------------------------------------------------------
typedef struct
{
	USHORT usX;
	USHORT usY;
	COLOR uColor;
} DRAW_POINT_STRUCT;

//---------------------------------------------------------------------------
typedef struct
{
	USHORT usX1;
	USHORT usX2;
	USHORT usY1;
	USHORT usY2;
	COLOR uColor;
} DRAW_LINE_STRUCT;
//---------------------------------------------------------------------------
typedef struct
{
	USHORT usLeft;
	USHORT usTop;
	USHORT usRight;
	USHORT usBottom;
	COLOR uLineColor;
	BOOL bFill;
	COLOR uFillColor;
} DRAW_RECTANGLE_STRUCT;
//---------------------------------------------------------------------------
typedef struct
{
	USHORT usX;
	USHORT usY;
	USHORT usRadius;
	COLOR uLineColor;
	BOOL bFill;
	COLOR uFillColor;
} DRAW_CIRCLE_STRUCT;
//---------------------------------------------------------------------------
typedef struct
{
	USHORT usX;
	USHORT usY;
	USHORT usHeight;
	USHORT usWidth;
	COLOR uColor;
} DRAW_ELLIPSE_STRUCT;
//---------------------------------------------------------------------------
typedef struct
{
	USHORT usX;
	USHORT usY;
	USHORT usWidth;
	USHORT usHeight;
	UCHAR ucBPP;
	UCHAR *pucData;
} DRAW_BITMAP_STRUCT;
//---------------------------------------------------------------------------
typedef struct
{
	USHORT usX;
	USHORT usY;
	USHORT usWidth;
	USHORT usHeight;
	COLOR uColor;
	UCHAR *pucData;
} DRAW_STAMP_STRUCT;	// monochrome stamp, bitpacked 8bpp
//---------------------------------------------------------------------------
typedef struct
{
	USHORT usX;
	USHORT usY;
	FONT_STRUCT *pstFont;
	UCHAR *pucString;
	COLOR uColor;
} DRAW_TEXT_STRUCT;
//---------------------------------------------------------------------------

void Draw_ClearScreen(DISPLAY_DRIVER_STRUCT *pstDisplay_);
void Draw_Point(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_POINT_STRUCT *pstPoint_);
void Draw_Line(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_LINE_STRUCT *pstLine_);
void Draw_Rectangle(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_RECTANGLE_STRUCT *pstRectangle_);
void Draw_Circle(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_CIRCLE_STRUCT *pstCircle_);
void Draw_Ellipse(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_ELLIPSE_STRUCT *pstEllipse_);
void Draw_Bitmap(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_BITMAP_STRUCT *pstBitmap_);
void Draw_Stamp(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_STAMP_STRUCT *pstStamp_);
void Draw_Text(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_TEXT_STRUCT *pstText_);
USHORT Draw_TextWidth(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_TEXT_STRUCT *pstText_);
#endif //__DRAW_H_
