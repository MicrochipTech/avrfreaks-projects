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
	\file graphics.cpp
	\brief Generic graphics driver implementation
*/

#include "kerneltypes.h"
#include "graphics.h"
#include "draw.h"
#include "driver.h"
#include "colorspace.h"
#include "font.h"

#include <stdio.h>

//---------------------------------------------------------------------------
void GraphicsDriver::ClearScreen()
{
	DrawPoint_t stPoint;
	stPoint.uColor = COLOR_BLACK;

	for (stPoint.usX = 0; stPoint.usX < m_usResX; stPoint.usX++)
	{
		for (stPoint.usY = 0; stPoint.usY < m_usResY; stPoint.usY++)
		{
			// Pixel Write
			DrawPixel(&stPoint);
		}
	}
}
//---------------------------------------------------------------------------
void GraphicsDriver::Point(DrawPoint_t *pstPoint_)
{
    DrawPixel(pstPoint_);
}

//---------------------------------------------------------------------------
void GraphicsDriver::Line(DrawLine_t *pstLine_)
{
	// Bresenham Line drawing algorithm, adapted from:
	// www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html

	DrawPoint_t stPoint;
	K_SHORT sX1 = (K_SHORT)pstLine_->usX1;
	K_SHORT sX2 = (K_SHORT)pstLine_->usX2;
	K_SHORT sY1 = (K_SHORT)pstLine_->usY1;
	K_SHORT sY2 = (K_SHORT)pstLine_->usY2;
	K_SHORT sDeltaY = sY2 - sY1;
	K_SHORT sDeltaX = sX2 - sX1;
	K_CHAR cStepx, cStepy;
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
	DrawPixel(&stPoint);

	if (sDeltaX > sDeltaY)
	{
		K_SHORT sFraction = sDeltaY - (sDeltaX >> 1);

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
			DrawPixel(&stPoint);
		}
	}
	else
	{
		K_SHORT sFraction = sDeltaX - (sDeltaY >> 1);
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
			DrawPixel(&stPoint);
		}
	}
}

//---------------------------------------------------------------------------
void GraphicsDriver::Rectangle(DrawRectangle_t *pstRectangle_)
{
	DrawPoint_t stPoint;

	// if drawing a background fill color (optional)
	if (pstRectangle_->bFill == true)
	{
		stPoint.uColor = pstRectangle_->uFillColor;
		for (stPoint.usX = pstRectangle_->usLeft; stPoint.usX <= pstRectangle_->usRight; stPoint.usX++)
		{
			for (stPoint.usY = pstRectangle_->usTop; stPoint.usY <= pstRectangle_->usBottom; stPoint.usY++)
			{
				DrawPixel(&stPoint);
			}
		}
	}

	// Draw four orthogonal lines...
	stPoint.uColor = pstRectangle_->uLineColor;
	stPoint.usY = pstRectangle_->usTop;
	for (stPoint.usX = pstRectangle_->usLeft; stPoint.usX <= pstRectangle_->usRight; stPoint.usX++)
	{
		DrawPixel(&stPoint);
	}

	stPoint.usY = pstRectangle_->usBottom;
	for (stPoint.usX = pstRectangle_->usLeft; stPoint.usX <= pstRectangle_->usRight; stPoint.usX++)
	{
		DrawPixel(&stPoint);
	}

	stPoint.usX = pstRectangle_->usLeft;
	for (stPoint.usY = pstRectangle_->usTop; stPoint.usY <= pstRectangle_->usBottom; stPoint.usY++)
	{
		DrawPixel(&stPoint);
	}

	stPoint.usX = pstRectangle_->usRight;
	for (stPoint.usY = pstRectangle_->usTop; stPoint.usY <= pstRectangle_->usBottom; stPoint.usY++)
	{
		DrawPixel(&stPoint);
	}
}

//---------------------------------------------------------------------------
void GraphicsDriver::Circle(DrawCircle_t *pstCircle_)
{
	DrawPoint_t stPoint;
	K_SHORT sX;
	K_SHORT sY;
	K_ULONG ulRadSquare;

	K_ULONG ulXSquare;
	K_ULONG ulYSquare;

	// Get the radius squared value...
	ulRadSquare = (K_ULONG)pstCircle_->usRadius;
	ulRadSquare *= ulRadSquare;

	// Look at the upper-right quarter of the circle
	for (sX = 0; sX <= (K_SHORT)pstCircle_->usRadius; sX++)
	{
		ulXSquare = (K_ULONG)sX;
		ulXSquare *= ulXSquare;
		for (sY = 0; sY <= (K_SHORT)pstCircle_->usRadius; sY++)
		{
			ulYSquare = (K_ULONG)sY;
			ulYSquare *= ulYSquare;

			// if filled...
			if (pstCircle_->bFill == true)
			{
				stPoint.uColor = pstCircle_->uFillColor;
				if (ulXSquare + ulYSquare <= ulRadSquare)
				{
					// Draw the fill color at the appropriate locations (quadrature...)
					stPoint.usX = pstCircle_->usX + sX;
					stPoint.usY = pstCircle_->usY + sY;
					DrawPixel(&stPoint);
					stPoint.usX = pstCircle_->usX - sX;
					stPoint.usY = pstCircle_->usY + sY;
					DrawPixel(&stPoint);
					stPoint.usX = pstCircle_->usX + sX;
					stPoint.usY = pstCircle_->usY - sY;
					DrawPixel(&stPoint);
					stPoint.usX = pstCircle_->usX - sX;
					stPoint.usY = pstCircle_->usY - sY;
					DrawPixel(&stPoint);
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
				stPoint.usY = pstCircle_->usY + sY;
				DrawPixel(&stPoint);
				stPoint.usX = pstCircle_->usX - sX;
				stPoint.usY = pstCircle_->usY + sY;
				DrawPixel(&stPoint);
				stPoint.usX = pstCircle_->usX + sX;
				stPoint.usY = pstCircle_->usY - sY;
				DrawPixel(&stPoint);
				stPoint.usX = pstCircle_->usX - sX;
				stPoint.usY = pstCircle_->usY - sY;
				DrawPixel(&stPoint);
			}
		}
	}
}

//---------------------------------------------------------------------------
void GraphicsDriver::Ellipse(DrawEllipse_t *pstEllipse_)
{
	DrawPoint_t stPoint;
	K_SHORT sX;
	K_SHORT sY;
	K_ULONG ulRadius;
	K_ULONG ulHSquare;
	K_ULONG ulVSquare;
	K_ULONG ulXSquare;
	K_ULONG ulYSquare;

	ulHSquare = (K_ULONG)pstEllipse_->usWidth;
	ulHSquare *= ulHSquare;

	ulVSquare = (K_ULONG)pstEllipse_->usHeight;
	ulVSquare *= ulVSquare;

	ulRadius = ulHSquare * ulVSquare;

	for (sX = 0; sX <= (K_SHORT)pstEllipse_->usWidth; sX++)
	{
		ulXSquare = (K_ULONG)sX;
		ulXSquare *= ulXSquare;
		ulXSquare *= ulHSquare;

		for (sY = 0; sY <= (K_SHORT)pstEllipse_->usHeight; sY++)
		{
			ulYSquare = (K_ULONG)sY;
			ulYSquare *= ulYSquare;
			ulYSquare *= ulVSquare;

			if ((ulXSquare + ulYSquare) <= ulRadius)
			{
				// Draw the fill color at the appropriate locations (quadrature...)
				stPoint.usX = pstEllipse_->usX + sX;
				stPoint.usY = pstEllipse_->usY + sY;
				DrawPixel(&stPoint);
				stPoint.usX = pstEllipse_->usX - sX;
				stPoint.usY = pstEllipse_->usY + sY;
				DrawPixel(&stPoint);
				stPoint.usX = pstEllipse_->usX + sX;
				stPoint.usY = pstEllipse_->usY - sY;
				DrawPixel(&stPoint);
				stPoint.usX = pstEllipse_->usX - sX;
				stPoint.usY = pstEllipse_->usY - sY;
				DrawPixel(&stPoint);
			}
		}
	}
}

//---------------------------------------------------------------------------
void GraphicsDriver::Bitmap(DrawBitmap_t *pstBitmap_)
{
	K_USHORT usRow;
	K_USHORT usCol;

	K_USHORT usIndex;

	K_UCHAR ucRed = 0;
	K_UCHAR ucBlue = 0;
	K_UCHAR ucGreen = 0;

	DrawPoint_t stPoint;

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
					ucRed     = ((pstBitmap_->pucData[usIndex]) & 0xE0) << 1;
					ucGreen   = ((pstBitmap_->pucData[usIndex]) & 0x18) << 3;
					ucBlue    = ((pstBitmap_->pucData[usIndex]) & 0x07) << 5;
				}
					break;
				case 2:
				{
					K_USHORT usTemp;
					usTemp = pstBitmap_->pucData[usIndex];
					usTemp <<= 8;
					usTemp |= pstBitmap_->pucData[usIndex + 1];

					// 5:6:5, RGB
					ucRed     = (K_UCHAR)((usTemp >> 11) & 0x001F) << 3;
					ucGreen   = (K_UCHAR)((usTemp >> 5) & 0x003F)  << 2;
					ucBlue    = (K_UCHAR)(usTemp & 0x001F) << 3;
				}
					break;
				case 3:
				{
					K_ULONG ulTemp;
					ulTemp = pstBitmap_->pucData[usIndex];
					ulTemp <<= 8;
					ulTemp |= pstBitmap_->pucData[usIndex + 1];
					ulTemp <<= 8;
					ulTemp |= pstBitmap_->pucData[usIndex + 2];

					// 8:8:8 RGB
					ucRed    = (K_UCHAR)((ulTemp & 0x00FF0000) >> 16);
					ucGreen  = (K_UCHAR)((ulTemp & 0x0000FF00) >> 8);
					ucBlue   = (K_UCHAR)((ulTemp & 0x000000FF));
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
			DrawPixel(&stPoint);

			// Stamps are opaque, don't fill in the BG
			usIndex += m_ucBPP / 8;
		}
	}
}

//---------------------------------------------------------------------------
void GraphicsDriver::Stamp(DrawStamp_t *pstStamp_)
{
	K_USHORT usRow;
	K_USHORT usCol;
	K_USHORT usShift;
	K_USHORT usIndex;
	DrawPoint_t stPoint;

	usIndex = 0;
	for (usRow = pstStamp_->usY; usRow < (pstStamp_->usY + pstStamp_->usHeight); usRow++)
	{
		usShift = 0x80;
		for (usCol = pstStamp_->usX; usCol < (pstStamp_->usX + pstStamp_->usWidth); usCol++)
		{
			// If the packed bit in the bitmap is a "1", draw the color.
			if (pstStamp_->pucData[usIndex] & usShift)
			{
				stPoint.usX = usCol;
				stPoint.usY = usRow;
				stPoint.uColor = pstStamp_->uColor;
				DrawPixel(&stPoint);
			}
			// Stamps are opaque, don't fill in the BG

			// Shift to the next bit in the field
			usShift >>= 1;

			// Rollover - next bit in the bitmap.
			// This obviously works best for stamps that are multiples of 8x8
			if (usShift == 0)
			{
				usShift = 0x80;
				usIndex++;
			}
		}
	}
}

//----------------------------------------------------------------------------
void GraphicsDriver::Move( DrawMove_t *pstMove_ )
{
	DrawPoint_t stPoint;
	K_LONG sX;
	K_LONG sY;
	K_LONG sXInc = 0;
	K_LONG sYInc = 0;

	K_BOOL bLeftToRight = false;
	K_BOOL bTopToBottom = false;

	if (pstMove_->usSrcX > pstMove_->usDstX)
	{
		bLeftToRight = true;
	}
	if (pstMove_->usSrcY > pstMove_->usDstY)
	{
		bTopToBottom = true;
	}

	if (bLeftToRight)
	{
		sXInc++;
	}
	else
	{
		sXInc--;
		pstMove_->usSrcX += pstMove_->usCopyWidth - 1;
		pstMove_->usDstX += pstMove_->usCopyWidth - 1;
	}

	if (bTopToBottom)
	{
		sYInc++;
	}
	else
	{
		sYInc--;
		pstMove_->usSrcY += pstMove_->usCopyHeight - 1;
		pstMove_->usDstY += pstMove_->usCopyHeight - 1;
	}

	// Hideously inefficient memory move...
	for (sX = 0; sX < pstMove_->usCopyWidth; sX++)
	{
		for (sY = 0; sY < pstMove_->usCopyHeight; sY++)
		{
			// Read from source (value read into the point struct)
			stPoint.usY = (K_USHORT)((K_LONG)pstMove_->usSrcY + ((K_LONG)sY * sYInc));
			stPoint.usX = (K_USHORT)((K_LONG)pstMove_->usSrcX + ((K_LONG)sX * sXInc));
			ReadPixel(&stPoint);

			// Copy to dest
			stPoint.usY = (K_USHORT)((K_LONG)pstMove_->usDstY + ((K_LONG)sY * sYInc));
			stPoint.usX = (K_USHORT)((K_LONG)pstMove_->usDstX + ((K_LONG)sX * sXInc));
			DrawPixel(&stPoint);
		}
	}
}

//---------------------------------------------------------------------------
void GraphicsDriver::Text(DrawText_t *pstText_)
{
	K_USHORT usX, usY;
	K_USHORT usStartX;
	K_USHORT usStartY;
	K_USHORT usCharOffsetX;
	K_USHORT usCharIndex = 0;
	K_UCHAR *pucData = (K_UCHAR*)pstText_->pstFont->pucFontData;
	DrawPoint_t stPoint;

	// set the color for this element.
	stPoint.uColor = pstText_->uColor;

	usCharOffsetX = 0;

	// Draw every character in the string, one at a time
	while (pstText_->pcString[usCharIndex] != 0)
	{
		K_USHORT usOffset = 0;
		
		K_UCHAR ucWidth;
		K_UCHAR ucHeight;
		K_UCHAR ucVOffset;
		K_UCHAR ucBitmask;
		
		// Read the glyphs from memory until we arrive at the one we wish to print
		for (usX = 0; usX < pstText_->pcString[usCharIndex]; usX++)
		{
			// Glyphs are variable-sized for efficiency - to look up a particular 
			// glyph, we must traverse all preceding glyphs in the list
			ucWidth  = Font_ReadByte(usOffset, pucData);
			ucHeight = Font_ReadByte(usOffset + 1, pucData);

			// Adjust the offset to point to the next glyph
			usOffset += ((((K_USHORT)ucWidth + 7) >> 3) * (K_USHORT)ucHeight) 
						+ (sizeof(Glyph_t) - 1);		
		}
	
		// Header information:  glyph size and vertical offset
		ucWidth   = Font_ReadByte(usOffset++, pucData);
		ucHeight  = Font_ReadByte(usOffset++, pucData);
		ucVOffset = Font_ReadByte(usOffset++, pucData);

		usStartY = pstText_->usTop + (K_USHORT)ucVOffset;
		usStartX = pstText_->usLeft;
		
		// Draw the font from left->right, top->bottom
		for (	usY = usStartY;
				usY < usStartY + (K_USHORT)ucHeight;
				usY++ )
		{
			K_UCHAR ucTempChar = Font_ReadByte(usOffset, pucData);
			ucBitmask = 0x80;
			
			for (	usX = usCharOffsetX + usStartX;
					usX < usCharOffsetX + usStartX + (K_USHORT)ucWidth;
					usX++ )
			{				
				if (!ucBitmask)
				{
					ucBitmask = 0x80;
					usOffset++;
					ucTempChar = Font_ReadByte(usOffset, pucData);
				}
				
				if (ucTempChar & ucBitmask)
				{
					// Update the location
					stPoint.usX = usX;
					stPoint.usY = usY;

					// Draw the point.
					DrawPixel(&stPoint);
				}
				
				ucBitmask >>= 1;
			}

			usOffset++;
		}

		// Next character
		usCharIndex++;
		usCharOffsetX += (K_USHORT)ucWidth + 1;
	}
}

//---------------------------------------------------------------------------
void GraphicsDriver::TextFX(DrawText_t *pstText_, TextFX_t *pstFX_ )
{
    K_USHORT usX, usY;
    K_USHORT usPartialX = 0;
    K_USHORT usPartialY = 0;
    K_USHORT usStartX;
    K_USHORT usStartY;
    K_USHORT usCharOffsetX;
    K_USHORT usCharIndex = 0;
    K_UCHAR *pucData = (K_UCHAR*)pstText_->pstFont->pucFontData;
    DrawPoint_t stPoint;

    // set the color for this element.
    stPoint.uColor = pstText_->uColor;

    usCharOffsetX = 0;

    // Draw every character in the string, one at a time
    while (pstText_->pcString[usCharIndex] != 0)
    {
        K_USHORT usOffset = 0;

        K_UCHAR ucWidth;
        K_UCHAR ucHeight;
        K_UCHAR ucVOffset;
        K_UCHAR ucBitmask;

        // Read the glyphs from memory until we arrive at the one we wish to print
        for (usX = 0; usX < pstText_->pcString[usCharIndex]; usX++)
        {
            // Glyphs are variable-sized for efficiency - to look up a particular
            // glyph, we must traverse all preceding glyphs in the list
            ucWidth  = Font_ReadByte(usOffset, pucData);
            ucHeight = Font_ReadByte(usOffset + 1, pucData);

            // Adjust the offset to point to the next glyph
            usOffset += ((((K_USHORT)ucWidth + 7) >> 3) * (K_USHORT)ucHeight)
                        + (sizeof(Glyph_t) - 1);
        }

        // Header information:  glyph size and vertical offset
        ucWidth   = Font_ReadByte(usOffset++, pucData);
        ucHeight  = Font_ReadByte(usOffset++, pucData);
        ucVOffset = Font_ReadByte(usOffset++, pucData);

        usStartY = pstText_->usTop + (K_USHORT)ucVOffset;
        usStartX = pstText_->usLeft;

        // Draw the font from left->right, top->bottom
        for (	usY = usStartY;
                usY < usStartY + (K_USHORT)ucHeight;
                usY++ )
        {
            K_UCHAR ucTempChar = Font_ReadByte(usOffset, pucData);
            ucBitmask = 0x80;
            usPartialY = 0;
            usPartialX = 0;

            K_USHORT usTempPartialX = 0;

            for (	usX = usCharOffsetX + usStartX;
                    usX < usCharOffsetX + usStartX + (K_USHORT)ucWidth;
                    usX++ )
            {
                K_USHORT usTempPartialY = 0;
                usPartialY = 0;
                if (!ucBitmask)
                {
                    ucBitmask = 0x80;
                    usOffset++;
                    ucTempChar = Font_ReadByte(usOffset, pucData);
                }

                if ((ucTempChar & ucBitmask) || (pstFX_->ucFlags & TEXTFX_FLAG_OPAQUE_BG))
                {
                    // usX and usY represent the untransformed data...
                    // we need usStartX, usStartY, usDeltaX, usDeltaY to proceed.
                    K_USHORT usDeltaX = (usX - pstText_->usLeft);
                    K_USHORT usDeltaY = (usY - pstText_->usTop);

                    // Compute "unadjusted" pixels for normal or scaled
                    K_USHORT usRawX, usRawY;

                    if (pstFX_->ucFlags & TEXTFX_FLAG_SCALE_X)
                    {
                        usRawX = usStartX + (((usDeltaX * pstFX_->usScaleX100))/100);
                        usTempPartialX = pstFX_->usScaleX100;
                    }
                    else
                    {
                        usRawX = usX;
                        usTempPartialX = 100;
                    }
                    usTempPartialX += usPartialX;

                    if (pstFX_->ucFlags & TEXTFX_FLAG_SCALE_Y)
                    {
                        usRawY = usStartY + (((usDeltaY * pstFX_->usScaleY100))/100);
                        usTempPartialY = pstFX_->usScaleY100;
                    }
                    else
                    {
                        usRawY = usY;
                        usTempPartialY = 100;
                    }
                    usTempPartialY += usPartialY;

                    K_USHORT usBLAH = usTempPartialX;

                    if (!(ucTempChar & ucBitmask))
                    {
                        stPoint.uColor = pstFX_->uBGColor;
                    }
                    else
                    {
                        stPoint.uColor = pstText_->uColor;
                    }

                    //!ToDo - Add rotation

                    stPoint.usX = usRawX;
                    while (usTempPartialX >= 50)
                    {
                        stPoint.usY = usRawY;
                        usBLAH = usTempPartialY;
                        while (usBLAH >= 50)
                        {
                            DrawPixel(&stPoint);
                            stPoint.usY++;
                            if (usBLAH >= 100)
                            {
                                usBLAH -= 100;
                            }
                            else
                            {
                                usBLAH = 0;
                            }
                        }
                        stPoint.usX++;
                        if (usTempPartialX >= 100)
                        {
                            usTempPartialX -= 100;
                        }
                        else
                        {
                            usTempPartialX = 0;
                        }
                    }

                    usPartialX = (usTempPartialX % 100);
                    usPartialY = (usTempPartialY % 100);
                }

                ucBitmask >>= 1;
            }

            usOffset++;
        }

        // Next character
        usCharIndex++;
        usCharOffsetX += (K_USHORT)ucWidth + 1;
    }
}

//----------------------------------------------------------------------------
K_USHORT GraphicsDriver::TextWidth(DrawText_t *pstText_)
{
    K_USHORT usCharOffsetX;
	K_USHORT usCharIndex = 0;
	K_USHORT usX;
	K_UCHAR *pucData = (K_UCHAR*)pstText_->pstFont->pucFontData;

	usCharOffsetX = 0;

	// Draw every character in the string, one at a time
	while (pstText_->pcString[usCharIndex] != 0)
	{
		K_USHORT usOffset = 0;
		
		K_UCHAR ucWidth;
		K_UCHAR ucHeight;
		
		// Read the glyphs from memory until we arrive at the one we wish to print
		for (usX = 0; usX < pstText_->pcString[usCharIndex]; usX++)
		{
			// Glyphs are variable-sized for efficiency - to look up a particular 
			// glyph, we must traverse all preceding glyphs in the list
			ucWidth  = Font_ReadByte(usOffset, pucData);
			ucHeight = Font_ReadByte(usOffset + 1, pucData);

			// Adjust the offset to point to the next glyph
			usOffset += ((((K_USHORT)ucWidth + 7) >> 3) * (K_USHORT)ucHeight) 
						+ (sizeof(Glyph_t) - 1);		
		}
	
		// Header information:  glyph size and vertical offset
		ucWidth   = Font_ReadByte(usOffset, pucData);
		usOffset += (sizeof(Glyph_t) - 1);
	
		// Next character
		usCharIndex++;
		usCharOffsetX += (K_USHORT)ucWidth + 1;
	}
	
	return usCharOffsetX;
}

//----------------------------------------------------------------------------
void GraphicsDriver::TriangleWire(DrawPoly_t *pstPoly_)
{
    DrawLine_t stLine;

    stLine.uColor = pstPoly_->uColor;

    stLine.usX1 = pstPoly_->pstVector[0].usX;
    stLine.usY1 = pstPoly_->pstVector[0].usY;
    stLine.usX2 = pstPoly_->pstVector[1].usX;
    stLine.usY2 = pstPoly_->pstVector[1].usY;
    Line(&stLine);

    stLine.usX1 = pstPoly_->pstVector[1].usX;
    stLine.usY1 = pstPoly_->pstVector[1].usY;
    stLine.usX2 = pstPoly_->pstVector[2].usX;
    stLine.usY2 = pstPoly_->pstVector[2].usY;
    Line(&stLine);

    stLine.usX1 = pstPoly_->pstVector[2].usX;
    stLine.usY1 = pstPoly_->pstVector[2].usY;
    stLine.usX2 = pstPoly_->pstVector[0].usX;
    stLine.usY2 = pstPoly_->pstVector[0].usY;
    Line(&stLine);
}
//----------------------------------------------------------------------------
void GraphicsDriver::TriangleFill(DrawPoly_t *pstPoly_)
{
    // Drawing a raster-filled triangle:
    K_UCHAR ucMaxEdge = 0;
    K_UCHAR ucMinEdge1 = 0, ucMinEdge2 = 0;
    K_SHORT sMax = 0;
    K_SHORT sTemp;

    K_SHORT sDeltaX1, sDeltaX2;
    K_SHORT sDeltaY1, sDeltaY2;
    K_CHAR cStepX1, cStepX2;
    K_CHAR cStepY;
    K_SHORT sX1, sX2, sX3, sY1, sY2, sY3;
    K_SHORT sTempX1, sTempY1, sTempX2, sTempY2;
    K_SHORT sFraction1;
    K_SHORT sFraction2;
    K_SHORT i;
    DrawPoint_t stPoint;

    // Figure out which line segment is the longest
    sTemp = (K_SHORT)pstPoly_->pstVector[0].usY - (K_SHORT)pstPoly_->pstVector[1].usY;
    if( sTemp < 0 )    { sTemp = -sTemp; }
    if( sTemp > sMax ) { sMax  = sTemp; ucMaxEdge = 0; ucMinEdge1 = 1; ucMinEdge2 = 2;}

    sTemp = (K_SHORT)pstPoly_->pstVector[1].usY - (K_SHORT)pstPoly_->pstVector[2].usY;
    if( sTemp < 0 )    { sTemp = -sTemp; }
    if( sTemp > sMax ) { sMax  = sTemp; ucMaxEdge = 1; ucMinEdge1 = 2; ucMinEdge2 = 0; }

    sTemp = (K_SHORT)pstPoly_->pstVector[2].usY - (K_SHORT)pstPoly_->pstVector[0].usY;
    if( sTemp < 0 )    { sTemp = -sTemp; }
    if( sTemp > sMax ) { sMax  = sTemp; ucMaxEdge = 2; ucMinEdge1 = 0; ucMinEdge2 = 1;}

    // Label the vectors and copy into temporary signed buffers
    sX1 = (K_SHORT)pstPoly_->pstVector[ucMaxEdge].usX;
    sX2 = (K_SHORT)pstPoly_->pstVector[ucMinEdge1].usX;
    sX3 = (K_SHORT)pstPoly_->pstVector[ucMinEdge2].usX;

    sY1 = (K_SHORT)pstPoly_->pstVector[ucMaxEdge].usY;
    sY2 = (K_SHORT)pstPoly_->pstVector[ucMinEdge1].usY;
    sY3 = (K_SHORT)pstPoly_->pstVector[ucMinEdge2].usY;

    // Figure out whether or not we're drawing up-down or down-up
    sDeltaY1 = sY1 - sY2;
    if (sDeltaY1 < 0) { cStepY = -1; sDeltaY1 = -sDeltaY1; } else { cStepY = 1; }

    sDeltaX1 = sX1 - sX2;
    if (sDeltaX1 < 0) { cStepX1 = -1; sDeltaX1 = -sDeltaX1; } else { cStepX1 = 1; }

    sDeltaY2 = sY1 - sY3;
    if (sDeltaY2 < 0) { cStepY = -1; sDeltaY2 = -sDeltaY2; } else { cStepY = 1; }

    sDeltaX2 = sX1 - sX3;
    if (sDeltaX2 < 0) { cStepX2 = -1; sDeltaX2 = -sDeltaX2; } else { cStepX2 = 1; }

    sDeltaX1 <<=1;
    sDeltaX2 <<=1;
    sDeltaY1 <<=1;
    sDeltaY2 <<=1;

    sFraction1 = sDeltaX1;// - (sDeltaY1 >> 1);
    sFraction2 = sDeltaX2;// - (sDeltaY2 >> 1);

    sTempY1 = sY1;
    sTempY2 = sY1;
    sTempX1 = sX1;
    sTempX2 = sX1;

    stPoint.uColor = pstPoly_->uColor;

    if( sDeltaY2 != 0 )
    {
        while (sTempY2 != sY3)
        {
            stPoint.usY = sTempY2;
            if( sTempX1 < sTempX2 ) {
                for( i = sTempX1; i <= sTempX2; i++) {
                    stPoint.usX = i;
                    Point(&stPoint);
                }
            } else {
                for( i = sTempX2; i <= sTempX1; i++ ) {
                    stPoint.usX = i;
                    Point(&stPoint);
                }
            }

            while (sFraction2 >= sDeltaY2)
            {
                sTempX2 -= cStepX2;
                sFraction2 -= sDeltaY2;
            }
            sTempY2 -= cStepY;
            sFraction2 += sDeltaX2;

            while (sFraction1 >= sDeltaY1)
            {
                sTempX1 -= cStepX1;
                sFraction1 -= sDeltaY1;
            }
            sTempY1 -= cStepY;
            sFraction1 += sDeltaX1;
        }
    }

    sDeltaY2 = sY3 - sY2;
    sDeltaX2 = sX3 - sX2;

    if (sDeltaX2 < 0) { cStepX2 = -1; sDeltaX2 = -sDeltaX2; } else { cStepX2 = 1; }
    if (sDeltaY2 < 0) { cStepY = -1;  sDeltaY2 = -sDeltaY2; } else { cStepY  = 1; }

    sDeltaX2 <<=1;
    sDeltaY2 <<=1;

    sFraction2 = sDeltaX2; // - (sDeltaY2 >> 1);

    sTempY2 = sY3;
    sTempX2 = sX3;

    if( sDeltaY2 != 0)
    {
        while (sTempY2 != sY2)
        {
            stPoint.usY = sTempY2;
            if( sTempX1 < sTempX2 ) {
                for( i = sTempX1; i <= sTempX2; i++) {
                    stPoint.usX = i;
                    Point(&stPoint);
                }
            } else {
                for( i = sTempX2; i <= sTempX1; i++ ) {
                    stPoint.usX = i;
                    Point(&stPoint);
                }
            }

            while (sFraction2 >= sDeltaY2)
            {
                sTempX2 -= cStepX2;
                sFraction2 -= sDeltaY2;
            }
            sTempY2 -= cStepY;
            sFraction2 += sDeltaX2;

            while (sFraction1 >= sDeltaY1)
            {
                sTempX1 -= cStepX1;
                sFraction1 -= sDeltaY1;
            }
            sTempY1 -= cStepY;
            sFraction1 += sDeltaX1;
        }
    }
}

//----------------------------------------------------------------------------
void GraphicsDriver::Polygon(DrawPoly_t *pstPoly_)
{
    K_USHORT i,j,k;
    K_BOOL bState = false;

    DrawPoly_t   stTempPoly;
    DrawVector_t astTempVec[3];

    if (pstPoly_->usNumPoints < 3)
    {
        return;
    }

    stTempPoly.uColor = pstPoly_->uColor;
    stTempPoly.bFill = pstPoly_->bFill;
    stTempPoly.pstVector = astTempVec;
    stTempPoly.usNumPoints = 3;

    astTempVec[0].usX = pstPoly_->pstVector[0].usX;
    astTempVec[1].usX = pstPoly_->pstVector[1].usX;
    astTempVec[0].usY = pstPoly_->pstVector[0].usY;
    astTempVec[1].usY = pstPoly_->pstVector[1].usY;

    j = 2;
    astTempVec[2].usX = pstPoly_->pstVector[pstPoly_->usNumPoints - 1].usX;
    astTempVec[2].usY = pstPoly_->pstVector[pstPoly_->usNumPoints - 1].usY;

    k = pstPoly_->usNumPoints - 2;

    if( pstPoly_->bFill )
    {
        TriangleFill(&stTempPoly);
    }
    else
    {
        TriangleWire(&stTempPoly);
    }

    // Filled polygon/wireframe polygon using triangle decomp.
    for(i = 0; i < pstPoly_->usNumPoints - 3; i++)
    {
        astTempVec[0].usX = astTempVec[1].usX;
        astTempVec[1].usX = astTempVec[2].usX;
        astTempVec[0].usY = astTempVec[1].usY;
        astTempVec[1].usY = astTempVec[2].usY;

        if( !bState )
        {
            bState = true;
            astTempVec[2].usX = pstPoly_->pstVector[j].usX;
            astTempVec[2].usY = pstPoly_->pstVector[j].usY;
            j++;
        }
        else
        {
            bState = false;
            astTempVec[2].usX = pstPoly_->pstVector[k].usX;
            astTempVec[2].usY = pstPoly_->pstVector[k].usY;
            k--;
        }
        if( pstPoly_->bFill )
        {
            TriangleFill(&stTempPoly);
        }
        else
        {
            TriangleWire(&stTempPoly);
        }
    }
}

//----------------------------------------------------------------------------
void GraphicsDriver::SetWindow(DrawWindow_t *pstWindow_)
{
	if ((pstWindow_->usLeft <= pstWindow_->usRight) &&
		(pstWindow_->usRight < m_usResX) &&
		(pstWindow_->usLeft < m_usResX))
	{
		m_usLeft = pstWindow_->usLeft;
		m_usRight = pstWindow_->usRight;
	}
    
    if ((pstWindow_->usTop <= pstWindow_->usBottom) &&
		(pstWindow_->usTop < m_usTop)  &&
		(pstWindow_->usBottom < m_usBottom))
	{
	    m_usTop = pstWindow_->usTop;    
	    m_usBottom = pstWindow_->usBottom;
	}
    
}

//----------------------------------------------------------------------------
void GraphicsDriver::ClearWindow()
{
    m_usLeft = 0;
    m_usTop = 0;
    m_usRight = m_usResX - 1;
    m_usBottom = m_usResY - 1;
}
