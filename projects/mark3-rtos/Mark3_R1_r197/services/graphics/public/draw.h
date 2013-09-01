/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
    \file draw.h
    \brief Raster graphics APIs
    Description:
        Implements basic drawing functionality.  This forms a hardware abstraction
        layer which requires a backend for rendering.
*/
//---------------------------------------------------------------------------

#ifndef __DRAW_H_
#define __DRAW_H_

#include "kerneltypes.h"
#include "font.h"
#include "colorspace.h"

//---------------------------------------------------------------------------
// Event definitions for 2D hardware accelerated graphics functions
typedef enum
{
    //--[Mandatory for a display driver]-------------------------------------
    DISPLAY_EVENT_SET_PIXEL = 0x00,    
    DISPLAY_EVENT_GET_PIXEL,
	
    //--[Optional if supported in hardware]----------------------------------
    DISPLAY_EVENT_CLEAR,
    DISPLAY_EVENT_LINE,
    DISPLAY_EVENT_RECTANGLE,
    DISPLAY_EVENT_CIRCLE,
    DISPLAY_EVENT_ELLIPSE,
    DISPLAY_EVENT_BITMAP,
    DISPLAY_EVENT_STAMP,
    DISPLAY_EVENT_TEXT,    
    DISPLAY_EVENT_MOVE,
	DISPLAY_EVENT_POLY
} DisplayEvent_t;

//---------------------------------------------------------------------------
/*!
    \brief Defines a pixel
*/
typedef struct
{
    K_USHORT usX;       //!< X coordinate of the pixel
    K_USHORT usY;       //!< Y coordinate of the pixel
    COLOR uColor;       //!< Color of the pixel
} DrawPoint_t;

//---------------------------------------------------------------------------
/*!
    \brief Defines a simple line
*/
typedef struct
{
    K_USHORT usX1;      //!< Starting X coordinate
    K_USHORT usX2;      //!< Ending X coordinate
    K_USHORT usY1;      //!< Starting Y Coordinate
    K_USHORT usY2;      //!< Ending Y coordinate
    COLOR uColor;       //!< Color of the pixel
} DrawLine_t;
//---------------------------------------------------------------------------
/*!
    \brief Defines a rectangle
*/
typedef struct
{
    K_USHORT usLeft;    //!< Leftmost pixel of the rectangle
    K_USHORT usTop;     //!< Topmost pixel of the rectangle
    K_USHORT usRight;   //!< Rightmost pixel of the rectangle
    K_USHORT usBottom;  //!< Bottom pixel of the rectangle
    COLOR uLineColor;   //!< Color of the line
    K_BOOL bFill;       //!< Whether or not to floodfill the interior
    COLOR uFillColor;   //!< Color of the interior of the rectangle
} DrawRectangle_t;
//---------------------------------------------------------------------------
/*!
    \brief Defines a circle
*/
typedef struct
{
    K_USHORT usX;       //!< Center X pixel
    K_USHORT usY;       //!< Center Y pixel
    K_USHORT usRadius;  //!< Radius in pixels
    COLOR uLineColor;   //!< Color of the circle perimeter
    K_BOOL bFill;       //!< Whether or not to fill the interior of the circle
    COLOR uFillColor;   //!< Fill color for the circle
} DrawCircle_t;
//---------------------------------------------------------------------------
/*!
    \brief Defines a ellipse
*/
typedef struct
{
    K_USHORT usX;       //!< Center X pixel
    K_USHORT usY;       //!< Center Y pixel
    K_USHORT usHeight;  //!< Height of the ellipse
    K_USHORT usWidth;   //!< Width of the ellipse
    COLOR uColor;       //!< Color of the ellipse perimeter
} DrawEllipse_t;
//---------------------------------------------------------------------------
/*!
    \brief Defines a bitmap
*/
typedef struct
{
    K_USHORT usX;       //!< Leftmost pixel
    K_USHORT usY;       //!< Uppermost pixel
    K_USHORT usWidth;   //!< Width of the bitmap in pixels
    K_USHORT usHeight;  //!< Height of the bitmap in pixels
    K_UCHAR ucBPP;      //!< Bits-per-pixel
    K_UCHAR *pucData;   //!< Pixel data pointer
} DrawBitmap_t;
//---------------------------------------------------------------------------
/*!
    \brief Defines a 1-bit 2D bitmap of arbitrary resolution
*/
typedef struct
{
    K_USHORT usX;       //!< Leftmost pixel
    K_USHORT usY;       //!< Uppermost pixel
    K_USHORT usWidth;   //!< Width of the stamp
    K_USHORT usHeight;  //!< Height of the stamp
    COLOR uColor;       //!< Color of the stamp
    K_UCHAR *pucData;   //!< Pointer to the stamp data
} DrawStamp_t;    // monochrome stamp, bitpacked 8bpp

//---------------------------------------------------------------------------
/*!
    \brief Defines a bitmap-rendered string
*/
typedef struct
{
    K_USHORT usLeft;           //!< Leftmost pixel of the text
    K_USHORT usTop;           //!< Uppermost pixel of the text
    COLOR uColor;           //!< Color of the text
    Font_t *pstFont;  //!< Pointer to the font used to render the text
    const K_CHAR *pcString;     //!< ASCII String to render
} DrawText_t;

//---------------------------------------------------------------------------
#define TEXTFX_FLAG_OPAQUE_BG   (0x01)  //!< Use an opaque BG
#define TEXTFX_FLAG_ROTATE      (0x02)  //!< Apply text rotation
#define TEXTFX_FLAG_SCALE_X     (0x04)  //!< Scale the text horizontally
#define TEXTFX_FLAG_SCALE_Y     (0x08)  //!< Scale the text vertically

//---------------------------------------------------------------------------
typedef struct
{
    K_UCHAR ucFlags;        //!< Text effects applied
    COLOR uBGColor;         //!< Background color for opaque backgrounds
    K_USHORT usRotateDeg;   //!< Rotation in degrees
    K_USHORT usScaleX100;   //!< Scaling factor, fixed point modulo 100
    K_USHORT usScaleY100;   //!< Scaling factor, fixed point modulo 100
} TextFX_t;

//---------------------------------------------------------------------------
/*!
    \brief Defines the active window - establishes boundaries for drawing on
    the current display.  Only pixels drawn inside the surface boundaries are
    rendered to the output
*/
typedef struct
{
    K_USHORT usLeft;        //!< Left boundary
    K_USHORT usRight;       //!< Right boundary
    K_USHORT usTop;         //!< Upper boundary
    K_USHORT usBottom;      //!< Bottom boundary
} DrawWindow_t;

//---------------------------------------------------------------------------
/*!
    \brief Simple 2D copy/paste.  Moves a bitmap specified by the given source
    coordinates on-surface to the destination coordinates.
*/
typedef struct
{
    K_USHORT usSrcX;        //!< Source X pixel (leftmost)
    K_USHORT usSrcY;        //!< Source Y pixel (topmost)
    K_USHORT usDstX;        //!< Destination X pixel (leftmost)
    K_USHORT usDstY;        //!< Destination Y pixel (topmost)
    K_USHORT usCopyHeight;  //!< Number of rows to copy 
    K_USHORT usCopyWidth;   //!< Number of columns to copy
} DrawMove_t;

//---------------------------------------------------------------------------
/*!
    \brief Specifies a single 2D point.  When used in arrays, this provides
    a way to draw vector paths, which form the basis of the polygon data
    structures.
*/
typedef struct
{
    K_USHORT usX;
    K_USHORT usY;
} DrawVector_t;

//---------------------------------------------------------------------------
/*!
    \brief Defines the structure of an arbitrary polygon.  Can be used to 
    specify the 
*/
typedef struct
{
    K_USHORT     usNumPoints;   //!< Number of points in the polygon
    COLOR        uColor;        //!< Color to use for lines/fill
    K_BOOL       bFill;         //!< Display as wireframe or filled
    DrawVector_t *pstVector;    //!< Vector points making the polygon
} DrawPoly_t;

#endif //__DRAW_H_
