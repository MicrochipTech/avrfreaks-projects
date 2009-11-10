/*
 *
 * Name         :  pcd8544.c
 *
 * Description  :  This is a driver for the PCD8544 graphic LCD.
 *                 Based on the code written by Sylvain Bissonette
 *                 This driver is buffered in 504 bytes memory be sure
 *                 that your MCU having bigger memory
 *
 * Author       :  Fandi Gunawan <fandigunawan@gmail.com>
 * Website      :  http://fandigunawan.wordpress.com
 *
 * Credit       :  Sylvain Bissonette (2003)
 *
 * License      :  GPL v. 3
 *
 * Compiler     :  WinAVR, GCC for AVR platform
 *                 Tested version :
 *                 - 20070525 (avr-libc 1.4)
 *                 - 20071221 (avr-libc 1.6)
 *                 - 20081225 tested by Jakub Lasinski
 *                 - other version please contact me if you find out it is working
 * Compiler note:  Please be aware of using older/newer version since WinAVR
 *                 is under extensive development. Please compile with parameter -O1
 *
 * History      :
 * Version 0.2.6 (March 14, 2009) additional optimization by Jakub Lasinski
 * + Optimization using Memset and Memcpy
 * + Bug fix and sample program reviewed
 * + Commented <stdio.h>
 * Version 0.2.5 (December 25, 2008) x-mas version :)
 * + Boundary check is added (reported by starlino on Dec 20, 2008)
 * + Return value is added, it will definitely useful for error checking
 * Version 0.2.4 (March 5, 2008)
 * + Multiplier was added to LcdBars to scale the bars
 * Version 0.2.3 (February 29, 2008)
 * + Rolled back LcdFStr function because of serious bug
 * + Stable release
 * Version 0.2.2 (February 27, 2008)
 * + Optimizing LcdFStr function
 * Version 0.2.1 (January 2, 2008)
 * + Clean up codes
 * + All settings were migrated to pcd8544.h
 * + Using _BV() instead of << to make a better readable code
 * Version 0.2 (December 11-14, 2007)
 * + Bug fixed in LcdLine() and LcdStr()
 * + Adding new routine
 *    - LcdFStr()
 *    - LcdSingleBar()
 *    - LcdBars()
 *    - LcdRect()
 *    - LcdImage()
 * + PROGMEM used instead of using.data section
 * Version 0.1 (December 3, 2007)
 * + First stable driver
 *
 * Note         :
 * Font will be displayed this way (16x6)
 * 1 2 3 4 5 6 7 8 9 0 1 2 3 4
 * 2
 * 3
 * 4
 * 5
 * 6
 *
 * Contributor : 
 * + Jakub Lasinski
 */

//#include <stdio.h>
#include <avr/io.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "pcd8544.h"


/* Function prototypes */

static void LcdSend    ( byte data, LcdCmdData cd );
static void Delay      ( void );

/* Global variables */

/* Cache buffer in SRAM 84*48 bits or 504 bytes */
static byte  LcdCache [ LCD_CACHE_SIZE ];

/* Cache index */
static int   LcdCacheIdx;

/* Lower part of water mark */
static int   LoWaterMark;

/* Higher part of water mark */
static int   HiWaterMark;

/* Variable to decide whether update Lcd Cache is active/nonactive */
static bool  UpdateLcd;

/*
 * Name         :  LcdInit
 * Description  :  Performs MCU SPI & LCD controller initialization.
 * Argument(s)  :  None.
 * Return value :  None.
 */
void LcdInit ( void )
{
    /* Pull-up on reset pin. */
    LCD_PORT |= _BV ( LCD_RST_PIN );

    /* Set output bits on LCD Port. */
    LCD_DDR |= _BV( LCD_RST_PIN ) | _BV( LCD_DC_PIN ) | _BV( LCD_CE_PIN ) | _BV( SPI_MOSI_PIN ) | _BV( SPI_CLK_PIN );

    Delay();

    /* Toggle display reset pin. */
    LCD_PORT &= ~( _BV( LCD_RST_PIN ) );
    Delay();
    LCD_PORT |= _BV ( LCD_RST_PIN );

    /* Enable SPI port:
    * No interrupt, MSBit first, Master mode, CPOL->0, CPHA->0, Clk/4
    */
    SPCR = 0x50;

    /* Disable LCD controller */
    LCD_PORT |= _BV( LCD_CE_PIN );

    LcdSend( 0x21, LCD_CMD ); /* LCD Extended Commands. */
    LcdSend( 0xC8, LCD_CMD ); /* Set LCD Vop (Contrast).*/
    LcdSend( 0x06, LCD_CMD ); /* Set Temp coefficent. */
    LcdSend( 0x13, LCD_CMD ); /* LCD bias mode 1:48. */
    LcdSend( 0x20, LCD_CMD ); /* LCD Standard Commands,Horizontal addressing mode */
    LcdSend( 0x0C, LCD_CMD ); /* LCD in normal mode. */

    /* Reset watermark pointers to empty */
    LoWaterMark = LCD_CACHE_SIZE;
    HiWaterMark = 0;

    /* Clear display on first time use */
    LcdClear();
    LcdUpdate();
}

/*
 * Name         :  LcdContrast
 * Description  :  Set display contrast.
 * Argument(s)  :  contrast -> Contrast value from 0x00 to 0x7F.
 * Return value :  None.
 */
void LcdContrast ( byte contrast )
{
    /* LCD Extended Commands. */
    LcdSend( 0x21, LCD_CMD );

    /* Set LCD contrast level. */
    LcdSend( 0x80 | contrast, LCD_CMD );

    /* LCD Standard Commands, horizontal addressing mode. */
    LcdSend( 0x20, LCD_CMD );
}

/*
 * Name         :  LcdClear
 * Description  :  Clears the display. LcdUpdate must be called next.
 * Argument(s)  :  None.
 * Return value :  None.
 * Note         :  Based on Sylvain Bissonette's code
 */
void LcdClear ( void )
{
// Removed in version 0.2.6, March 14 2009
// Optimized by Jakub Lasinski
//    int i;
//
//    /* Set 0x00 to all LcdCache's contents */
//    for ( i = 0; i < LCD_CACHE_SIZE; i++ )
//    {
//        LcdCache[ i ] = 0x00;
//    }
	memset(LcdCache,0x00,LCD_CACHE_SIZE); //Sugestion - its faster and its 10 bytes less in program mem
    /* Reset watermark pointers to full */
    LoWaterMark = 0;
    HiWaterMark = LCD_CACHE_SIZE - 1;

    /* Set update flag to be true */
    UpdateLcd = TRUE;
}

/*
 * Name         :  LcdGotoXYFont
 * Description  :  Sets cursor location to xy location corresponding to basic
 *                 font size.
 * Argument(s)  :  x, y -> Coordinate for new cursor position. Range: 1,1 .. 14,6
 * Return value :  see return value in pcd8544.h
 * Note         :  Based on Sylvain Bissonette's code
 */
byte LcdGotoXYFont ( byte x, byte y )
{
    /* Boundary check, slow down the speed but will guarantee this code wont fail */
    /* Version 0.2.5 - Fixed on Dec 25, 2008 (XMAS) */
    if( x > 14)
        return OUT_OF_BORDER;
    if( y > 6)
        return OUT_OF_BORDER;
    /*  Calculate index. It is defined as address within 504 bytes memory */

    LcdCacheIdx = ( x - 1 ) * 6 + ( y - 1 ) * 84;
    return OK;
}

/*
 * Name         :  LcdChr
 * Description  :  Displays a character at current cursor location and
 *                 increment cursor location.
 * Argument(s)  :  size -> Font size. See enum in pcd8544.h.
 *                 ch   -> Character to write.
 * Return value :  see pcd8544.h about return value
 */
byte LcdChr ( LcdFontSize size, byte ch )
{
    byte i, c;
    byte b1, b2;
    int  tmpIdx;

    if ( LcdCacheIdx < LoWaterMark )
    {
        /* Update low marker. */
        LoWaterMark = LcdCacheIdx;
    }

    if ( (ch < 0x20) || (ch > 0x7b) )
    {
        /* Convert to a printable character. */
        ch = 92;
    }

    if ( size == FONT_1X )
    {
        for ( i = 0; i < 5; i++ )
        {
            /* Copy lookup table from Flash ROM to LcdCache */
            LcdCache[LcdCacheIdx++] = pgm_read_byte(&( FontLookup[ ch - 32 ][ i ] ) ) << 1;
        }
    }
    else if ( size == FONT_2X )
    {
        tmpIdx = LcdCacheIdx - 84;

        if ( tmpIdx < LoWaterMark )
        {
            LoWaterMark = tmpIdx;
        }

        if ( tmpIdx < 0 ) return OUT_OF_BORDER;

        for ( i = 0; i < 5; i++ )
        {
            /* Copy lookup table from Flash ROM to temporary c */
            c = pgm_read_byte(&(FontLookup[ch - 32][i])) << 1;
            /* Enlarge image */
            /* First part */
            b1 =  (c & 0x01) * 3;
            b1 |= (c & 0x02) * 6;
            b1 |= (c & 0x04) * 12;
            b1 |= (c & 0x08) * 24;

            c >>= 4;
            /* Second part */
            b2 =  (c & 0x01) * 3;
            b2 |= (c & 0x02) * 6;
            b2 |= (c & 0x04) * 12;
            b2 |= (c & 0x08) * 24;

            /* Copy two parts into LcdCache */
            LcdCache[tmpIdx++] = b1;
            LcdCache[tmpIdx++] = b1;
            LcdCache[tmpIdx + 82] = b2;
            LcdCache[tmpIdx + 83] = b2;
        }

        /* Update x cursor position. */
        /* Version 0.2.5 - Possible bug fixed on Dec 25,2008 */
        LcdCacheIdx = (LcdCacheIdx + 11) % LCD_CACHE_SIZE;
    }

    if ( LcdCacheIdx > HiWaterMark )
    {
        /* Update high marker. */
        HiWaterMark = LcdCacheIdx;
    }

    /* Horizontal gap between characters. */
    /* Version 0.2.5 - Possible bug fixed on Dec 25,2008 */
    LcdCache[LcdCacheIdx] = 0x00;
    /* At index number LCD_CACHE_SIZE - 1, wrap to 0 */
    if(LcdCacheIdx == (LCD_CACHE_SIZE - 1) )
    {
        LcdCacheIdx = 0;
        return OK_WITH_WRAP;
    }
    /* Otherwise just increment the index */
    LcdCacheIdx++;
    return OK;
}

/*
 * Name         :  LcdStr
 * Description  :  Displays a character at current cursor location and increment
 *                 cursor location according to font size. This function is
 *                 dedicated to print string laid in SRAM
 * Argument(s)  :  size      -> Font size. See enum.
 *                 dataArray -> Array contained string of char to be written
 *                              into cache.
 * Return value :  see return value on pcd8544.h
 */
byte LcdStr ( LcdFontSize size, byte dataArray[] )
{
    byte tmpIdx=0;
    byte response;
    while( dataArray[ tmpIdx ] != '\0' )
	{
        /* Send char */
		response = LcdChr( size, dataArray[ tmpIdx ] );
        /* Just in case OUT_OF_BORDER occured */
        /* Dont worry if the signal == OK_WITH_WRAP, the string will
        be wrapped to starting point */
        if( response == OUT_OF_BORDER)
            return OUT_OF_BORDER;
        /* Increase index */
		tmpIdx++;
	}
    return OK;
}

/*
 * Name         :  LcdFStr
 * Description  :  Displays a characters at current cursor location and increment
 *                 cursor location according to font size. This function is
 *                 dedicated to print string laid in Flash ROM
 * Argument(s)  :  size    -> Font size. See enum.
 *                 dataPtr -> Pointer contained string of char to be written
 *                            into cache.
 * Return value :  see return value on pcd8544.h
 * Example      :  LcdFStr(FONT_1X, PSTR("Hello World"));
 *                 LcdFStr(FONT_1X, &name_of_string_as_array);
 */
byte LcdFStr ( LcdFontSize size, const byte *dataPtr )
{
    byte c;
    byte response;
    for ( c = pgm_read_byte( dataPtr ); c; ++dataPtr, c = pgm_read_byte( dataPtr ) )
    {
        /* Put char */
        response = LcdChr( size, c );
        if(response == OUT_OF_BORDER)
            return OUT_OF_BORDER;
    }
	/* Fixed by Jakub Lasinski. Version 0.2.6, March 14, 2009 */
    return OK;
}

/*
 * Name         :  LcdPixel
 * Description  :  Displays a pixel at given absolute (x, y) location.
 * Argument(s)  :  x, y -> Absolute pixel coordinates
 *                 mode -> Off, On or Xor. See enum in pcd8544.h.
 * Return value :  see return value on pcd8544.h
 * Note         :  Based on Sylvain Bissonette's code
 */
byte LcdPixel ( byte x, byte y, LcdPixelMode mode )
{
    word  index;
    byte  offset;
    byte  data;

    /* Prevent from getting out of border */
    if ( x > LCD_X_RES ) return OUT_OF_BORDER;
    if ( y > LCD_Y_RES ) return OUT_OF_BORDER;

    /* Recalculating index and offset */
    index = ( ( y / 8 ) * 84 ) + x;
    offset  = y - ( ( y / 8 ) * 8 );

    data = LcdCache[ index ];

    /* Bit processing */

	/* Clear mode */
    if ( mode == PIXEL_OFF )
    {
        data &= ( ~( 0x01 << offset ) );
    }

    /* On mode */
    else if ( mode == PIXEL_ON )
    {
        data |= ( 0x01 << offset );
    }

    /* Xor mode */
    else if ( mode  == PIXEL_XOR )
    {
        data ^= ( 0x01 << offset );
    }

    /* Final result copied to cache */
    LcdCache[ index ] = data;

    if ( index < LoWaterMark )
    {
        /*  Update low marker. */
        LoWaterMark = index;
    }

    if ( index > HiWaterMark )
    {
        /*  Update high marker. */
        HiWaterMark = index;
    }
    return OK;
}

/*
 * Name         :  LcdLine
 * Description  :  Draws a line between two points on the display.
 * Argument(s)  :  x1, y1 -> Absolute pixel coordinates for line origin.
 *                 x2, y2 -> Absolute pixel coordinates for line end.
 *                 mode   -> Off, On or Xor. See enum in pcd8544.h.
 * Return value :  see return value on pcd8544.h
 */
byte LcdLine ( byte x1, byte x2, byte y1, byte y2, LcdPixelMode mode )
{
    int dx, dy, stepx, stepy, fraction;
    byte response;

    /* Calculate differential form */
    /* dy   y2 - y1 */
    /* -- = ------- */
    /* dx   x2 - x1 */

    /* Take differences */
    dy = y2 - y1;
    dx = x2 - x1;

    /* dy is negative */
    if ( dy < 0 )
    {
        dy    = -dy;
        stepy = -1;
    }
    else
    {
        stepy = 1;
    }

    /* dx is negative */
    if ( dx < 0 )
    {
        dx    = -dx;
        stepx = -1;
    }
    else
    {
        stepx = 1;
    }

    dx <<= 1;
    dy <<= 1;

    /* Draw initial position */
    response = LcdPixel( x1, y1, mode );
    if(response)
        return response;

    /* Draw next positions until end */
    if ( dx > dy )
    {
        /* Take fraction */
        fraction = dy - ( dx >> 1);
        while ( x1 != x2 )
        {
            if ( fraction >= 0 )
            {
                y1 += stepy;
                fraction -= dx;
            }
            x1 += stepx;
            fraction += dy;

            /* Draw calculated point */
            response = LcdPixel( x1, y1, mode );
            if(response)
                return response;

        }
    }
    else
    {
        /* Take fraction */
        fraction = dx - ( dy >> 1);
        while ( y1 != y2 )
        {
            if ( fraction >= 0 )
            {
                x1 += stepx;
                fraction -= dy;
            }
            y1 += stepy;
            fraction += dx;

            /* Draw calculated point */
            response = LcdPixel( x1, y1, mode );
            if(response)
                return response;
        }
    }

    /* Set update flag to be true */
    UpdateLcd = TRUE;
    return OK;
}

/*
 * Name         :  LcdSingleBar
 * Description  :  Display single bar.
 * Argument(s)  :  baseX  -> absolute x axis coordinate
 *                 baseY  -> absolute y axis coordinate
 *				   height -> height of bar (in pixel)
 *				   width  -> width of bar (in pixel)
 *				   mode   -> Off, On or Xor. See enum in pcd8544.h.
 * Return value :  see return value on pcd8544.h
 */
byte LcdSingleBar ( byte baseX, byte baseY, byte height, byte width, LcdPixelMode mode )
{
	byte tmpIdxX,tmpIdxY,tmp;

    byte response;

    /* Checking border */
	if ( ( baseX > LCD_X_RES ) || ( baseY > LCD_Y_RES ) ) return OUT_OF_BORDER;

	if ( height > baseY )
		tmp = 0;
	else
		tmp = baseY - height;

    /* Draw lines */
	for ( tmpIdxY = tmp; tmpIdxY < baseY; tmpIdxY++ )
	{
		for ( tmpIdxX = baseX; tmpIdxX < (baseX + width); tmpIdxX++ )
        {
			response = LcdPixel( tmpIdxX, tmpIdxY, mode );
            if(response)
                return response;

        }
	}

    /* Set update flag to be true */
	UpdateLcd = TRUE;
    return OK;
}

/*
 * Name         :  LcdBars
 * Description  :  Display multiple bars.
 * Argument(s)  :  data[] -> data which want to be plotted
 *                 numbBars  -> number of bars want to be plotted
 *				   width  -> width of bar (in pixel)
 * Return value :  see return value on pcd8544.h
 * Note         :  Please check EMPTY_SPACE_BARS, BAR_X, BAR_Y in pcd8544.h
 */
byte LcdBars ( byte data[], byte numbBars, byte width, byte multiplier )
{
	byte b;
	byte tmpIdx = 0;
    byte response;

	for ( b = 0;  b < numbBars ; b++ )
	{
        /* Preventing from out of border (LCD_X_RES) */
		if ( tmpIdx > LCD_X_RES ) return OUT_OF_BORDER;

		/* Calculate x axis */
		tmpIdx = ((width + EMPTY_SPACE_BARS) * b) + BAR_X;

		/* Draw single bar */
		response = LcdSingleBar( tmpIdx, BAR_Y, data[ b ] * multiplier, width, PIXEL_ON);
        if(response == OUT_OF_BORDER)
            return response;
	}

	/* Set update flag to be true */
	UpdateLcd = TRUE;
    return OK;

}
/*
 * Name         :  LcdRect
 * Description  :  Display a rectangle.
 * Argument(s)  :  x1   -> absolute first x axis coordinate
 *                 y1   -> absolute first y axis coordinate
 *				   x2   -> absolute second x axis coordinate
 *				   y2   -> absolute second y axis coordinate
 *				   mode -> Off, On or Xor. See enum in pcd8544.h.
 * Return value :  see return value on pcd8544.h.
 */
byte LcdRect ( byte x1, byte x2, byte y1, byte y2, LcdPixelMode mode )
{
	byte tmpIdxX,tmpIdxY;
    byte response;

	/* Checking border */
	if ( ( x1 > LCD_X_RES ) ||  ( x2 > LCD_X_RES ) || ( y1 > LCD_Y_RES ) || ( y2 > LCD_Y_RES ) )
		/* If out of border then return */
		return OUT_OF_BORDER;

	if ( ( x2 > x1 ) && ( y2 > y1 ) )
	{
		for ( tmpIdxY = y1; tmpIdxY < y2; tmpIdxY++ )
		{
			/* Draw line horizontally */
			for ( tmpIdxX = x1; tmpIdxX < x2; tmpIdxX++ )
            {
				/* Draw a pixel */
				response = LcdPixel( tmpIdxX, tmpIdxY, mode );
                if(response)
                    return response;
            }
		}

		/* Set update flag to be true */
		UpdateLcd = TRUE;
	}
    return OK;
}
/*
 * Name         :  LcdImage
 * Description  :  Image mode display routine.
 * Argument(s)  :  Address of image in hexes
 * Return value :  None.
 * Example      :  LcdImage(&sample_image_declared_as_array);
 */
void LcdImage ( const byte *imageData )
{
	/* Initialize cache index to 0 */
//	LcdCacheIdx = 0;
//	/* While within cache range */
//    for ( LcdCacheIdx = 0; LcdCacheIdx < LCD_CACHE_SIZE; LcdCacheIdx++ )
//    {
//		/* Copy data from pointer to cache buffer */
//        LcdCache[LcdCacheIdx] = pgm_read_byte( imageData++ );
//    }
	/* optimized by Jakub Lasinski, version 0.2.6, March 14, 2009 */
    memcpy_P(LcdCache,imageData,LCD_CACHE_SIZE);	//Same as aboeve - 6 bytes less and faster instruction
	/* Reset watermark pointers to be full */
    LoWaterMark = 0;
    HiWaterMark = LCD_CACHE_SIZE - 1;

	/* Set update flag to be true */
    UpdateLcd = TRUE;
}

/*
 * Name         :  LcdUpdate
 * Description  :  Copies the LCD cache into the device RAM.
 * Argument(s)  :  None.
 * Return value :  None.
 */
void LcdUpdate ( void )
{
    int i;

    if ( LoWaterMark < 0 )
        LoWaterMark = 0;
    else if ( LoWaterMark >= LCD_CACHE_SIZE )
        LoWaterMark = LCD_CACHE_SIZE - 1;

    if ( HiWaterMark < 0 )
        HiWaterMark = 0;
    else if ( HiWaterMark >= LCD_CACHE_SIZE )
        HiWaterMark = LCD_CACHE_SIZE - 1;

    /*  Set base address according to LoWaterMark. */
    LcdSend( 0x80 | ( LoWaterMark % LCD_X_RES ), LCD_CMD );
    LcdSend( 0x40 | ( LoWaterMark / LCD_X_RES ), LCD_CMD );

    /*  Serialize the display buffer. */
    for ( i = LoWaterMark; i <= HiWaterMark; i++ )
    {
        LcdSend( LcdCache[ i ], LCD_DATA );
    }

    /*  Reset watermark pointers. */
    LoWaterMark = LCD_CACHE_SIZE - 1;
    HiWaterMark = 0;

    /* Set update flag to be true */
	UpdateLcd = FALSE;
}

/*
 * Name         :  LcdSend
 * Description  :  Sends data to display controller.
 * Argument(s)  :  data -> Data to be sent
 *                 cd   -> Command or data (see enum in pcd8544.h)
 * Return value :  None.
 */
static void LcdSend ( byte data, LcdCmdData cd )
{
    /*  Enable display controller (active low). */
    LCD_PORT &= ~( _BV( LCD_CE_PIN ) );

    if ( cd == LCD_DATA )
    {
        LCD_PORT |= _BV( LCD_DC_PIN );
    }
    else
    {
        LCD_PORT &= ~( _BV( LCD_DC_PIN ) );
    }

    /*  Send data to display controller. */
    SPDR = data;

    /*  Wait until Tx register empty. */
    while ( (SPSR & 0x80) != 0x80 );


    /* Disable display controller. */
    LCD_PORT |= _BV( LCD_CE_PIN );
}

/*
 * Name         :  Delay
 * Description  :  Uncalibrated delay for LCD init routine.
 * Argument(s)  :  None.
 * Return value :  None.
 */
static void Delay ( void )
{
    int i;

    for ( i = -32000; i < 32000; i++ );
}
