//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file draw.c

	Description:
		Implements basic drawing functionality.  This forms a hardware abstraction
		layer which requires a backend for rendering.
*/
//---------------------------------------------------------------------------

#include "types.h"
#include "draw.h"
#include "display.h"	//! display.h contains the reference to the hardware interface
#include "driver.h"
#include "colorspace.h"

//---------------------------------------------------------------------------
/*!
	Clear the screen (initializes to all black pixels)	
	\fn void Draw_ClearScreen(DISPLAY_DRIVER_STRUCT *pstDisplay_)
	\param pstDisplay_ - Pointer to the display driver	
*/
//---------------------------------------------------------------------------
void Draw_ClearScreen(DISPLAY_DRIVER_STRUCT *pstDisplay_)
{
	if (pstDisplay_->usAccelerationFlags & ACCELERATED_CLEAR)
	{
		Driver_Control(pstDisplay_->pstDriver, DISPLAY_EVENT_CLEAR, NULL);
	}
	else
	{
		USHORT usX;
		USHORT usY;
		DRAW_POINT_STRUCT stPoint;
		stPoint.uColor = COLOR_BLACK;

		//!! Add Mutex Protection
		for (stPoint.usX = 0; stPoint.usX < pstDisplay_->usResX; stPoint.usX++)
		{
			for (stPoint.usY = 0; stPoint.usY < pstDisplay_->usResY; stPoint.usY++)
			{
				// Pixel Write
				pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
			}
		}
	}
}
//---------------------------------------------------------------------------
/*!
	Draw a pixel to the display.
	\fn void Draw_Point(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_POINT_STRUCT *pstPoint_)
	\param pstDisplay_ - pointer to the display driver
	\param pstPoint_ - pointer to the struct containing the pixel to draw
*/
//---------------------------------------------------------------------------
void Draw_Point(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_POINT_STRUCT *pstPoint_)
{
	pstDisplay_->pfDrawPixel(pstDisplay_, pstPoint_);
}
//---------------------------------------------------------------------------
/*!
	Draw a line to the display using Bresenham's line drawing algorithm
	\fn void Draw_Line(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_LINE_STRUCT *pstLine_)
	\param pstDisplay_ - pointer to the display driver
	\param pstLine_ - pointer to the line structure
*/
//---------------------------------------------------------------------------
void Draw_Line(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_LINE_STRUCT *pstLine_)
{
	if (pstDisplay_->usAccelerationFlags & ACCELERATED_LINE)
	{
		Driver_Control(pstDisplay_->pstDriver, DISPLAY_EVENT_LINE, pstLine_);
	}
	else
	{
		// Bresenham Line drawing algorithm, adapted from:
		// www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html

		DRAW_POINT_STRUCT stPoint;
		SHORT sX1 = (SHORT)pstLine_->usX1;
		SHORT sX2 = (SHORT)pstLine_->usX2;
		SHORT sY1 = (SHORT)pstLine_->usY1;
		SHORT sY2 = (SHORT)pstLine_->usY2;
		SHORT sDeltaY = sY2 - sY1;
        SHORT sDeltaX = sX2 - sX1;
        CHAR cStepx, cStepy;
		stPoint.uColor = pstLine_->uColor;

        if (sDeltaY < 0)
		{
			sDeltaY = -sDeltaY;
			cStepy = -1;
		}
		else
		{
			cStepy = 1;
		}

        if (sDeltaX < 0)
		{
			sDeltaX = -sDeltaX;
			cStepx = -1;
		}
		else
		{
			cStepx = 1;
		}

		// Scale by a factor of 2 in each direction
        sDeltaY <<= 1;
        sDeltaX <<= 1;

		stPoint.usX = sX1;
		stPoint.usY = sY1;
		pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);

        if (sDeltaX > sDeltaY)
		{
            SHORT sFraction = sDeltaY - (sDeltaX >> 1);

            while (sX1 != sX2)
			{
                if (sFraction >= 0)
				{
                    sY1 += cStepy;
                    sFraction -= sDeltaX;
                }
                sX1 += cStepx;
                sFraction += sDeltaY;
				
				stPoint.usX = sX1;
				stPoint.usY = sY1;
				pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
            }
        } 
		else 
		{
            SHORT sFraction = sDeltaX - (sDeltaY >> 1);
            while (sY1 != sY2)
			{
                if (sFraction >= 0) 
				{
                    sX1 += cStepx;
                    sFraction -= sDeltaY;
                }
                sY1 += cStepy;
                sFraction += sDeltaX;
				
                stPoint.usX = sX1;
				stPoint.usY = sY1;
				pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
            }
        }
	}
}
//---------------------------------------------------------------------------
/*!
	Draws a rectangle on the display
	\fn void Draw_Rectangle(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_RECTANGLE_STRUCT *pstRectangle_)
	\param pstDisplay_ - pointer to the display driver
	\param pstRectangle_ - pointer to the rectangle struct
*/
//---------------------------------------------------------------------------
void Draw_Rectangle(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_RECTANGLE_STRUCT *pstRectangle_)
{
	if (pstDisplay_->usAccelerationFlags & ACCELERATED_RECTANGLE)
	{
		Driver_Control(pstDisplay_->pstDriver, DISPLAY_EVENT_RECTANGLE, pstRectangle_);
	}
	else
	{
		DRAW_POINT_STRUCT stPoint;
		USHORT usX;
		USHORT usY;
		
		// if drawing a background fill color (optional)
		if (pstRectangle_->bFill == TRUE)
		{
			stPoint.uColor = pstRectangle_->uFillColor;
			for (stPoint.usX = pstRectangle_->usLeft; stPoint.usX <= pstRectangle_->usRight; stPoint.usX++)
			{
				for (stPoint.usY = pstRectangle_->usTop; stPoint.usY <= pstRectangle_->usBottom; stPoint.usY++)
				{
					pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
				}
			}
		}
		
		// Draw four orthogonal lines...
		stPoint.uColor = pstRectangle_->uLineColor;
		stPoint.usY = pstRectangle_->usTop;
		for (stPoint.usX = pstRectangle_->usLeft; stPoint.usX <= pstRectangle_->usRight; stPoint.usX++)
		{
			pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
		}
		
		stPoint.usY = pstRectangle_->usBottom;
		for (stPoint.usX = pstRectangle_->usLeft; stPoint.usX <= pstRectangle_->usRight; stPoint.usX++)
		{
			pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
		}
		
		stPoint.usX = pstRectangle_->usLeft;
		for (stPoint.usY = pstRectangle_->usTop; stPoint.usY <= pstRectangle_->usBottom; stPoint.usY++)
		{
			pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
		}
		
		stPoint.usX = pstRectangle_->usRight;
		for (stPoint.usY = pstRectangle_->usTop; stPoint.usY <= pstRectangle_->usBottom; stPoint.usY++)
		{
			pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
		}
	}
}
//---------------------------------------------------------------------------
/*!
	Draw a circle to the display
	\fn void Draw_Circle(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_CIRCLE_STRUCT *pstCircle_)
	\param pstDisplay_ - pointer to the display
	\param pstCircle_ - pointer to the circle to draw
*/
//---------------------------------------------------------------------------
void Draw_Circle(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_CIRCLE_STRUCT *pstCircle_)
{
	if (pstDisplay_->usAccelerationFlags & ACCELERATED_CIRCLE)
	{
		Driver_Control(pstDisplay_->pstDriver, DISPLAY_EVENT_CIRCLE, pstCircle_);
	}
	else
	{
		DRAW_POINT_STRUCT stPoint;
		SHORT sX;
		SHORT sY;
		ULONG ulRadSquare;
		ULONG ulSquare;
		ULONG ulXSquare;
		ULONG ulYSquare;
		
		// Get the radius squared value...
		ulRadSquare = (ULONG)pstCircle_->usRadius;
		ulRadSquare *= ulRadSquare;
		
		// Look at the upper-right quarter of the circle
		for (sX = 0; sX <= (SHORT)pstCircle_->usRadius; sX++)
		{
			ulXSquare = (ULONG)sX;
			ulXSquare *= ulXSquare;
			for (sY = 0; sY <= (SHORT)pstCircle_->usRadius; sY++)
			{
				ulYSquare = (ULONG)sY;
				ulYSquare *= ulYSquare;
				
				// if filled...
				if (pstCircle_->bFill == TRUE)
				{
					stPoint.uColor = pstCircle_->uFillColor;					
					if (ulXSquare + ulYSquare <= ulRadSquare)
					{
						// Draw the fill color at the appropriate locations (quadrature...)
						stPoint.usX = pstCircle_->usX + sX;
						stPoint.usY = pstCircle_->usX + sY;
						pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
						stPoint.usX = pstCircle_->usX - sX;
						stPoint.usY = pstCircle_->usX + sY;
						pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
						stPoint.usX = pstCircle_->usX + sX;
						stPoint.usY = pstCircle_->usX - sY;
						pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
						stPoint.usX = pstCircle_->usX - sX;
						stPoint.usY = pstCircle_->usX - sY;
						pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
					}
				}
				// Check for edge...
				if (
                    ((ulXSquare + ulYSquare) >= (ulRadSquare-pstCircle_->usRadius)) &&
                    ((ulXSquare + ulYSquare) <= (ulRadSquare+pstCircle_->usRadius))
                   )
				{
					stPoint.uColor = pstCircle_->uLineColor;

					// Draw the fill color at the appropriate locations (quadrature...)
					stPoint.usX = pstCircle_->usX + sX;
					stPoint.usY = pstCircle_->usX + sY;
					pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
					stPoint.usX = pstCircle_->usX - sX;
					stPoint.usY = pstCircle_->usX + sY;
					pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
					stPoint.usX = pstCircle_->usX + sX;
					stPoint.usY = pstCircle_->usX - sY;
					pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
					stPoint.usX = pstCircle_->usX - sX;
					stPoint.usY = pstCircle_->usX - sY;
					pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);

				}
			}
		}
	}
}
//---------------------------------------------------------------------------
/*!
	Draw an ellipse to the display
	\fn void Draw_Ellipse(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_ELLIPSE_STRUCT *pstEllipse_)
	\param pstDisplay_ - pointer to the display driver
	\param pstEllipse_ - pointer to the ellipse to draw on the display
*/
//---------------------------------------------------------------------------
void Draw_Ellipse(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_ELLIPSE_STRUCT *pstEllipse_)
{
	if (pstDisplay_->usAccelerationFlags & ACCELERATED_ELLPISE)
	{
		Driver_Control(pstDisplay_->pstDriver, DISPLAY_EVENT_ELLIPSE, NULL);
	}
	else
	{
		DRAW_POINT_STRUCT stPoint;
		SHORT sX;
		SHORT sY;
		ULONG ulRadius;
		ULONG ulHSquare;
		ULONG ulVSquare;
		ULONG ulXSquare;
		ULONG ulYSquare;
		
		ulHSquare = (ULONG)pstEllipse_->usWidth;
		ulHSquare *= ulHSquare;
		
		ulVSquare = (ULONG)pstEllipse_->usHeight;
		ulVSquare *= ulVSquare;
		
		ulRadius = ulHSquare * ulVSquare;
		
		for (sX = 0; sX <= (SHORT)pstEllipse_->usWidth; sX++)
		{
			ulXSquare = (ULONG)sX;
			ulXSquare *= ulXSquare;
			ulXSquare *= ulHSquare;
			
			for (sY = 0; sY <= (SHORT)pstEllipse_->usHeight; sY++)
			{
				ulYSquare = (ULONG)sY;
				ulYSquare *= ulYSquare;
				ulYSquare *= ulVSquare;
				
				if ((ulXSquare + ulYSquare) <= ulRadius)
				{
                    // Draw the fill color at the appropriate locations (quadrature...)
                    stPoint.usX = pstEllipse_->usX + sX;
                    stPoint.usY = pstEllipse_->usX + sY;
                    pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
                    stPoint.usX = pstEllipse_->usX - sX;
                    stPoint.usY = pstEllipse_->usX + sY;
                    pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
                    stPoint.usX = pstEllipse_->usX + sX;
                    stPoint.usY = pstEllipse_->usX - sY;
                    pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
                    stPoint.usX = pstEllipse_->usX - sX;
                    stPoint.usY = pstEllipse_->usX - sY;
                    pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
                }
			}
		}
	}
}
//---------------------------------------------------------------------------
/*! 
	Draw an RGB image on the display 
	\fn void Draw_Bitmap(DISPLAY_DRIVER_STRUCT *pstDriver_, DRAW_BITMAP_STRUCT *pstBitmap_)
	\param pstDriver_ - pointer to the display driver
	\param pstBitmap_ - pointer to the bitmap object to display
*/
//---------------------------------------------------------------------------
void Draw_Bitmap(DISPLAY_DRIVER_STRUCT *pstDriver_, DRAW_BITMAP_STRUCT *pstBitmap_)
{
	if (pstDriver_->usAccelerationFlags & ACCELERATED_BITMAP)
	{
		Driver_Control(pstDriver_->pstDriver, DISPLAY_EVENT_BITMAP, pstBitmap_);
	}
	else
	{
		USHORT usRow;
		USHORT usCol;
		USHORT usShift;
		USHORT usIndex;
		
		UCHAR ucRed;
		UCHAR ucBlue;
		UCHAR ucGreen;
		
		DRAW_POINT_STRUCT stPoint;
		
		usIndex = 0;
		for (usRow = pstBitmap_->usY; usRow < (pstBitmap_->usY + pstBitmap_->usHeight); usRow++)
		{
			for (usCol = pstBitmap_->usX; usCol < (pstBitmap_->usX + pstBitmap_->usWidth); usCol++)
			{
				stPoint.usX = usCol;
				stPoint.usY = usRow;
				
				// Build the color based on the bitmap value...  This algorithm
				// is slow, but it automatically converts any 8/16/24 bit bitmap into the
				// current colorspace defined...
				switch(pstBitmap_->ucBPP)
				{
					case 1:
					{
						// 3:2:3, RGB
						ucRed 	= ((pstBitmap_->pucData[usIndex]) & 0xE0) << 1;
						ucGreen = ((pstBitmap_->pucData[usIndex]) & 0x18) << 3;
						ucBlue = ((pstBitmap_->pucData[usIndex]) & 0x07) << 5;
					}
						break;
					case 2:
					{
						USHORT usTemp;
						usTemp = pstBitmap_->pucData[usIndex];
						usTemp <<= 8;
						usTemp |= pstBitmap_->pucData[usIndex + 1];
						
						// 5:6:5, RGB
						ucRed 	= (UCHAR)((usTemp >> 11) & 0x001F) << 3;
						ucGreen = (UCHAR)((usTemp >> 5) & 0x003F)  << 2;
						ucBlue = (UCHAR)(usTemp & 0x001F) << 3;
					}
						break;
					case 3:
					{
						ULONG ulTemp;
						ulTemp = pstBitmap_->pucData[usIndex];
						ulTemp <<= 8;
						ulTemp |= pstBitmap_->pucData[usIndex + 1];
						ulTemp <<= 8;
						ulTemp |= pstBitmap_->pucData[usIndex + 2];
						
						// 8:8:8 RGB
						ucRed 	= (UCHAR)((ulTemp & 0x00FF0000) >> 16);
						ucGreen = (UCHAR)((ulTemp & 0x0000FF00) >> 8);
						ucBlue	= (UCHAR)((ulTemp & 0x000000FF));
					}
						break;
					default:
						break;
				}
				
				// Convert the R,G,B values into the correct colorspace for display
#if DRAW_COLOR_2BIT
				//1-bit
				ucRed >>= 7;
				ucGreen >>= 7;
				ucBlue >>= 7;
#elif DRAW_COLOR_8BIT
				//3:2:3 R:G:B
				ucRed >>= 5;
				ucGreen >>= 6;
				ucBlue >>= 5;
#elif DRAW_COLOR_16BIT
				//5:6:5 R:G:B
				ucRed >>= 3;
				ucGreen >>= 2;
				ucBlue >>= 3;
#elif DRAW_COLOR_24BIT
				// No conversion required
#endif
				// Build the color.
				stPoint.uColor = RGB_COLOR(ucRed,ucGreen,ucBlue);

				// Draw the point.
				pstDriver_->pfDrawPixel(pstDriver_, &stPoint);

				// Stamps are opaque, don't fill in the BG
				usIndex += pstDriver_->ucBPP;
			}
		}
	}
}
//---------------------------------------------------------------------------
/*!
	Draws a stamp (a 1-bit bitmap) on the display 
	\fn void Draw_Stamp(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_STAMP_STRUCT *pstStamp_)
	\param pstDisplay_ - pointer to the display driver
	\param pstStamp_ - pointer to the stamp object to draw
*/	
//---------------------------------------------------------------------------
void Draw_Stamp(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_STAMP_STRUCT *pstStamp_)
{
	if (pstDisplay_->usAccelerationFlags & ACCELERATED_BITMAP)
	{
		Driver_Control(pstDisplay_->pstDriver, DISPLAY_EVENT_STAMP, pstStamp_);
	}
	else
	{
		USHORT usRow;
		USHORT usCol;
		USHORT usShift;
		USHORT usIndex;
		DRAW_POINT_STRUCT stPoint;

		usIndex = 0;
		for (usRow = pstStamp_->usY; usRow < (pstStamp_->usY + pstStamp_->usHeight); usRow++)
		{
			usShift = 1;
			for (usCol = pstStamp_->usX; usCol < (pstStamp_->usX + pstStamp_->usWidth); usCol++)
			{
				// If the packed bit in the bitmap is a "1", draw the color.
				if (pstStamp_->pucData[usIndex] & usShift)
				{
					stPoint.usX = usCol;
					stPoint.usY = usRow;
					stPoint.uColor = pstStamp_->uColor;
					pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
				}
				// Stamps are opaque, don't fill in the BG
				
				// Shift to the next bit in the field
				usShift <<= 1;
				
				// Rollover - next bit in the bitmap.
				// This obviously works best for stamps that are multiples of 8x8
				if (usShift == 0x100)
				{
					usShift = 1;
					usIndex++;
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
/*!
	Draw a string of text to the display using a bitmap font
	\fn void Draw_Text(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_TEXT_STRUCT *pstText_)
	\param pstDisplay_ - pointer to the display driver
	\param pstText_ - pointer to the text object to render
*/
//---------------------------------------------------------------------------
void Draw_Text(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_TEXT_STRUCT *pstText_)
{
	if (pstDisplay_->usAccelerationFlags & ACCELERATED_TEXT)
	{
		Driver_Control(pstDisplay_->pstDriver, DISPLAY_EVENT_TEXT, NULL);
	}
	else
	{
        USHORT usX, usY;
        USHORT usCharOffsetX;
        USHORT usCharOffsetY;
        USHORT usArrayIndex = 0;
        USHORT usCharIndex = 0;

        UCHAR ucWidthBytes;
        UCHAR ucBitIndex;
        UCHAR ucTempOffsetX;

		DRAW_POINT_STRUCT stPoint;

        // set the color for this element.
        stPoint.uColor = pstText_->uColor;

        // The width of each character.
        ucWidthBytes = (pstText_->pstFont->ucFontWidth / 8);

        // Draw each character
        usCharOffsetX = 0;

        while (pstText_->pucString[usCharIndex] != 0)
        {
            // Get the address of the char in the font table...
            usArrayIndex = (((USHORT)(ucWidthBytes + 1)
                            * (USHORT)(pstText_->pstFont->ucFontHeight)) + 2)
                            * (USHORT)(pstText_->pucString[usCharIndex] - pstText_->pstFont->ucStartChar);

            ucBitIndex = 0;

            // Header information:  vertical offset and font width.
            usCharOffsetY = Font_ReadByte(usArrayIndex, pstText_->pstFont->pucCharData);
            usArrayIndex++;
            ucTempOffsetX = Font_ReadByte(usArrayIndex, pstText_->pstFont->pucCharData);
            usArrayIndex++;

            // Draw the font from left->right, top->bottom
            for (usY = usCharOffsetY + pstText_->usY;
                    usY < usCharOffsetY + pstText_->usY + pstText_->pstFont->ucFontHeight;
                    usY++)
            {
                for (usX = usCharOffsetX + pstText_->usX;
                        usX <= usCharOffsetX + pstText_->usX + pstText_->pstFont->ucFontWidth;
                        usX++)
                {
                    // Pixel is ON
                    UCHAR ucTempChar = Font_ReadByte(usArrayIndex, pstText_->pstFont->pucCharData);
                    if (ucTempChar & (1 << ucBitIndex))
                    {
                        // Update the location
                        stPoint.usX = usX;
                        stPoint.usY = usY;

                        // Draw the point.
                        pstDisplay_->pfDrawPixel(pstDisplay_, &stPoint);
                    }

                    // Go to the next index.
                    ucBitIndex++;
                    if (ucBitIndex >= 8)
                    {
                        ucBitIndex = 0;
                        usArrayIndex++;
                    }
                }
                // Go through the next index anyways
                if (ucBitIndex != 0)
                {
                    usArrayIndex++;
                    ucBitIndex = 0;
                }
            }

            // Next character
            usCharIndex++;
            usCharOffsetX += ucTempOffsetX + 1;
        }
	}
}
//----------------------------------------------------------------------------
/*!
	Returns the width of the text string (in pixels), as it would be drawn to
	the screen, without actually drawing it to the screen.
	
	\fn USHORT Draw_TextWidth(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_TEXT_STRUCT *pstText_)
	\param pstDisplay_ - pointer to the display driver
	\param pstText_ - text object to determine the rendering width 
*/
//----------------------------------------------------------------------------
USHORT Draw_TextWidth(DISPLAY_DRIVER_STRUCT *pstDisplay_, DRAW_TEXT_STRUCT *pstText_)
{
    USHORT usReturn = 0;
    USHORT usX, usY;

    USHORT usCharOffsetX;
    USHORT usCharOffsetY;
    USHORT usArrayIndex = 0;
    USHORT usCharIndex = 0;
    UCHAR ucWidthBytes;
    UCHAR ucTempOffsetX;

    DRAW_POINT_STRUCT stPoint;

    // set the color for this element.
    stPoint.uColor = pstText_->uColor;

    // The width of each character.
    ucWidthBytes = (pstText_->pstFont->ucFontWidth / 8);

    // Draw each character
    usCharOffsetX = 0;

    while (pstText_->pucString[usCharIndex] != 0)
    {
        // Get the address of the char in the font table...
        usArrayIndex = (((USHORT)(ucWidthBytes + 1)
                        * (USHORT)(pstText_->pstFont->ucFontHeight)) + 2)
                        * (USHORT)(pstText_->pucString[usCharIndex] - pstText_->pstFont->ucStartChar);

        // Header information:  vertical offset and font width.
        usCharOffsetY = Font_ReadByte(usArrayIndex, pstText_->pstFont->pucCharData);
        usArrayIndex++;
        ucTempOffsetX = Font_ReadByte(usArrayIndex, pstText_->pstFont->pucCharData);
        usArrayIndex++;

        // Next character
        usCharIndex++;
        usReturn += ucTempOffsetX + 1;
    }
    return usReturn;
}

