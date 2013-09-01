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
	\file graphics.h
	\brief Graphics driver class declaration
*/

#ifndef __GRAPHICSX_H__
#define __GRAPHICSX_H__

#include "driver.h"
#include "draw.h"

//---------------------------------------------------------------------------
/*!
	Defines the base graphics driver class, which is inherited by all other
	graphics drivers.  Per-pixel rendering functions for all raster operations
	is provided by default.  These can be overridden with more efficient
	hardware-supported operations where available.
*/
class GraphicsDriver : public Driver
{
public:
//---------------------------------------------------------------------------
/*
	The base graphics driver does not implement the set of
	virtual methods inherited from the Driver class.  This
	is left to the actual hardware implementation.
*/
//---------------------------------------------------------------------------
	
	/*!
		\fn void DrawPixel(DrawPoint_t *pstPoint_)		
		Draw a single pixel to the display.
		\param pstPoint_ Structure containing the pixel data (color/location) 
						 to be written.
	*/
	virtual void DrawPixel(DrawPoint_t *pstPoint_) {};
	
	/*!
		\fn void ReadPixel(DrawPoint_t *pstPoint_)		
		Read a single pixel from the display.
		\param pstPoint_ Structure containing the pixel location of the pixel
					     to be read.  The color value will contain the value 
						 from the display when read.
	*/
	virtual void ReadPixel(DrawPoint_t *pstPoint_) {};
	
//---------------------------------------------------------------------------
/*
	Raster operations defined using per-pixel rendering.
	Can be overridden in inheriting classes.
*/
//---------------------------------------------------------------------------
	/*!
		\fn void ClearScreen()
		
		Clear the screen (initializes to all black pixels)    
	*/
	virtual void ClearScreen();
		
	/*!
		\fn void Point(DrawPoint_t *pstPoint_)
		Draw a pixel to the display.    
		\param pstPoint_ - pointer to the struct containing the pixel to draw
	*/	
	virtual void Point(DrawPoint_t *pstPoint_);
	
	/*!
		\fn void Line(DrawLine_t *pstLine_)
		Draw a line to the display using Bresenham's line drawing algorithm
		\param pstLine_ - pointer to the line structure
	*/	
	virtual void Line(DrawLine_t *pstLine_);
	
	/*!
		\fn void Rectangle(DrawRectangle_t *pstRectangle_)
		Draws a rectangle on the display
		\param pstRectangle_ - pointer to the rectangle struct
	*/	
	virtual void Rectangle(DrawRectangle_t *pstRectangle_);

	/*!
		\fn void Circle(DrawCircle_t *pstCircle_)
		Draw a circle to the display    
		\param pstCircle_ - pointer to the circle to draw
	*/	
	virtual void Circle(DrawCircle_t *pstCircle_);
	
	/*!
		\fn void Ellipse(DrawEllipse_t *pstEllipse_)
		Draw an ellipse to the display
		\param pstEllipse_ - pointer to the ellipse to draw on the display
	*/	
	virtual void Ellipse(DrawEllipse_t *pstEllipse_);
	
	/*!
		\fn void Bitmap(DrawBitmap_t *pstBitmap_)    
		Draw an RGB image on the display
		\param pstBitmap_ - pointer to the bitmap object to display
	*/	
	virtual void Bitmap(DrawBitmap_t *pstBitmap_);
	
	/*!
		\fn void Stamp(DrawStamp_t *pstStamp_)
		Draws a stamp (a 1-bit bitmap) on the display
		\param pstStamp_ - pointer to the stamp object to draw
	*/	
	virtual void Stamp(DrawStamp_t *pstStamp_);
	
	/*!
		\fn void Move(DrawMove_t *pstMove_);
		
		Move a the contents from one rectangle on screen to another rectangle,
		specified by the values of the input structure.

		\param pstMove_ - object describing the graphics movement operation
			(framebuffer operations only).
	*/	
	virtual void Move(DrawMove_t *pstMove_ );
	
	/*!
		\fn void TriangleWire(DrawPoly_t *pstPoly_);
		Draw a wireframe triangle to the display.
		\param pstPoly_ Pointer to the polygon to draw.		
	*/
	virtual void TriangleWire(DrawPoly_t *pstPoly_);
	
	/*!
		\fn void TriangleFill(DrawPoly_t *pstPoly_);
		Draw a filled triangle to the display.
		\param pstPoly_ Pointer to the polygon to draw.		
	*/
	virtual void TriangleFill(DrawPoly_t *pstPoly_);
	
	/*!
		\fn void Poly(DrawPoly_t *pstPoly_);    
		Draw a polygon on the specified display
		\param pstPoly_ - pointer to the polygon object to draw
	*/
	virtual void Polygon(DrawPoly_t *pstPoly_);

	/*!
		\fn void Text(DrawText_t *pstText_)    
		Draw a string of text to the display using a bitmap font
		\param pstText_ - pointer to the text object to render
	*/	
	virtual void Text(DrawText_t *pstText_);
	
    /*!
        \fn TextFX

        Render a string of text to the display with effects

        \param pstText_ - pointer to the text object to render
        \param pstFX_   - struct defining special text formatting to apply
     */
    void TextFX(DrawText_t *pstText_, TextFX_t *pstFX_);

	/*!
		\fn K_USHORT Draw_TextWidth(DisplayManager_t *pstDisplay_, DrawText_t *pstText_)    
		Returns the width of the text string (in pixels), as it would be drawn to
		the screen, without actually drawing it to the screen.
		\param pstText_ - text object to determine the rendering width
	*/	
	virtual K_USHORT TextWidth(DrawText_t *pstText_);

	/*!
		\fn void SetWindow(DrawWindow_t *pstWindow_);
		Set the drawable window of the screen    
		\param pstWindow_ - pointer to the window struct defining the drawable area
	*/
	void SetWindow( DrawWindow_t *pstWindow_ );
	
	/*!
		\fn void ClearWindow();
		Clear the window - resetting the boundaries to the entire drawable area of
		the screen.    
	*/
	void ClearWindow();
protected:

	K_USHORT m_usResX;
	K_USHORT m_usResY;

	K_USHORT m_usLeft;
	K_USHORT m_usTop;
	K_USHORT m_usRight;
	K_USHORT m_usBottom;
	
	K_UCHAR m_ucBPP;
};

#endif

