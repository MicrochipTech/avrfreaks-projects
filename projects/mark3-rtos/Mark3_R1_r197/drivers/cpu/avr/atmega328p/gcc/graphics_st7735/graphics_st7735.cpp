/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2013 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
    \file graphics_st7735.cpp
    
    \brief Graphics driver implementation on ST7735 hardware    
*/
/*************************************************** 
  Note:  This module is based off of 3rd party code, 
	 see the license below!
*************************************************** 
  This is a library for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
 
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "graphics_st7735.h"
#include "kerneltypes.h"
#include "thread.h"
#include "colorspace.h"

#include <avr/pgmspace.h>
#include <avr/io.h>


// SPI Defines
#define SPI_CLK_DIV_4			(0x00)
#define SPI_CLK_MASK			(0x03)
#define SPI_CLK_MASK_2			(0x01)

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80
static const uint8_t PROGMEM
  Bcmd[] = {                  // Initialization commands for 7735B screens
    18,                       // 18 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    ST7735_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
      0x05,                   //     16-bit color
      10,                     //     10 ms delay
    ST7735_FRMCTR1, 3+DELAY,  //  4: Frame rate control, 3 args + delay:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms delay
    ST7735_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    ST7735_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    ST7735_PWCTR1 , 2+DELAY,  //  8: Power control, 2 args + delay:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms delay
    ST7735_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    ST7735_VMCTR1 , 2+DELAY,  // 11: Power control, 2 args + delay:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms delay
    ST7735_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ST7735_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1,16+DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms delay
    ST7735_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 2
      0x00, 0x81,             //     XEND = 129
    ST7735_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 1
      0x00, 0x81,             //     XEND = 160
    ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
      255 },                  //     255 = 500 ms delay

  Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

  Rcmd2green[] = {            // Init for 7735R, part 2 (green tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 0
      0x00, 0x7F+0x02,        //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //     XSTART = 0
      0x00, 0x9F+0x01 },      //     XEND = 159
  Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F },           //     XEND = 159

  Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay


//---------------------------------------------------------------------------
// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void GraphicsST7735::CommandList(const K_UCHAR *pucData_)
{
    K_UCHAR ucNumCommands = pgm_read_byte(pucData_++);      // Number of commands to follow
    while (ucNumCommands--)                                 // For each command...
    {
        WriteCommand(pgm_read_byte(pucData_++));            // Read, issue command

        K_UCHAR     ucNumArgs = pgm_read_byte(pucData_++);  // Number of args to follow
        K_USHORT    usMs      = ucNumArgs & DELAY;          // If hibit set, delay follows args

        ucNumArgs &= ~DELAY;                                // Mask out delay bit

        while(ucNumArgs--)                                  // For each argument...
        {
            WriteData(pgm_read_byte(pucData_++));           // Read, issue argument
        }

        if(usMs)                                            // If there's a delay for this command
        {
            usMs = pgm_read_byte(pucData_++);               // Read post-command delay time (ms)
            if(usMs == 255)                                 // If 255, delay for 500 ms
            {
                usMs = 500;
            }
        }
    }
}

#if USE_HW_SPI
//---------------------------------------------------------------------------
#define TFT_SPI_WRITE(x) \
{ \
    TFT_SPI_SPDR = (x); \
    while( !(TFT_SPI_SPSR & (1 << TFT_SPI_SPIF) ) ) { } \
}
#else
//---------------------------------------------------------------------------
#define TFT_SPI_WRITE(x)	\
{ \
    K_UCHAR ucMask = 0x80; \
    while (ucMask) \
    { \
        if (ucMask & x) \
        { \
            TFT_SPI_MOSI_PORT |= TFT_SPI_MOSI_PIN; \
        } \
        else \
        { \
            TFT_SPI_MOSI_PORT &= ~TFT_SPI_MOSI_PIN; \
        } \
        TFT_SPI_SCLK_OUT |= TFT_SPI_SCLK_PIN; \
        TFT_SPI_SCLK_OUT &= ~TFT_SPI_SCLK_PIN; \
        ucMask >>= 1; \
    } \
}

#endif

//---------------------------------------------------------------------------
void GraphicsST7735::WriteCommand( K_UCHAR ucX_ ) 
{
    TFT_CD_PORT &= ~TFT_CD_PIN;
    TFT_CS_PORT &= ~TFT_CS_PIN;
	TFT_SPI_WRITE(ucX_);
    TFT_CS_PORT |= TFT_CS_PIN;
}

//---------------------------------------------------------------------------
void GraphicsST7735::WriteData( K_UCHAR ucX_ ) 
{
    TFT_CD_PORT |= TFT_CD_PIN;
    TFT_CS_PORT &= ~TFT_CS_PIN;
	TFT_SPI_WRITE(ucX_);
    TFT_CS_PORT |= TFT_CS_PIN;
} 

//---------------------------------------------------------------------------
void GraphicsST7735::Init()
{
    m_ucColStart = 0;
    m_ucRowStart = 0;

    m_usResX = ST7735_TFTWIDTH;
    m_usResY = ST7735_TFTHEIGHT;
    m_ucBPP = 16;

    TFT_CD_DIR |= TFT_CD_PIN;
    TFT_CS_DIR |= TFT_CS_PIN;
    TFT_CD_PORT &= ~TFT_CD_PIN;
    TFT_CS_PORT &= ~TFT_CS_PIN;

#if USE_HW_SPI
    // SPI Slave-select ourput high; guarantee we're the master on the bus, and
    // that the SPI HW behaves as expected
    SPI_SS_PORT |= SPI_SS_PIN;
    SPI_SS_DIR  |= SPI_SS_PIN;

    // Configure SPI as Master, MSB First, and Enable
    TFT_SPI_SPCR  =  (1 << MSTR); 	    // Master-mode
	TFT_SPI_SPCR |=  (1 << SPE);		// Enable.
	
	TFT_SPI_SPSR = (1 << SPI2X);		// Double-speed SPI, for faster writes
	
    // Implicit - Mode0 -> We  cleared the SPCR earlier, so will already be 0
#endif
    TFT_SPI_SCLK_DIR |= TFT_SPI_SCLK_PIN;
    TFT_SPI_MOSI_DIR |= TFT_SPI_MOSI_PIN;
    TFT_SPI_SCLK_PORT &= ~TFT_SPI_SCLK_PIN;
    TFT_SPI_MOSI_PORT &= ~TFT_SPI_MOSI_PIN;
    TFT_CS_PORT &= ~TFT_CS_PIN;

#if TFT_RST_PIN
    if (TFT_RST_PIN)
    {
        TFT_RST_DIR  |= TFT_RST_PIN;
        TFT_RST_PORT |= TFT_RST_PIN;
        Thread::Sleep(500);

        TFT_RST_PORT &= ~TFT_RST_PIN;
        Thread::Sleep(500);

        TFT_RST_PORT |= TFT_RST_PIN;
        Thread::Sleep(500);
    }
#endif

//-- Device-specific commands...    
    CommandList(Rcmd1);
#if (TAB_COLOR == INITR_GREENTAB)
    CommandList(Rcmd2green);
    m_ucColStart = 2;
    m_ucRowStart = 1;
#else
    CommandList(Rcmd2red);
#endif
    CommandList(Rcmd3);

#if (TAB_COLOR == INITR_BLACKTAB)
    // if black, change MADCTL color filter
    WriteCommand(ST7735_MADCTL);
    WriteData(0xC0);
#endif

}


//---------------------------------------------------------------------------
void GraphicsST7735::FastVLine(DrawLine_t *pstLine_)
{
    // Tell the driver which pixels we're going to write
    // (in a bounding-box 1-pixel high, n-pixels wide)
    DrawRectangle_t stRect;

    stRect.usLeft = pstLine_->usX1;
    stRect.usRight = stRect.usLeft;
    stRect.usTop = pstLine_->usY1;
    stRect.usBottom = pstLine_->usY2;

    SetOpWindow(&stRect);

    // Count the pixels
    K_USHORT usPixels = pstLine_->usY2 - pstLine_->usY1 + 1;

    // Set the high/low bytes of the color that we're going to write
    K_UCHAR ucHigh    = (K_UCHAR)((pstLine_->uColor) >> 8);
    K_UCHAR ucLow     = (K_UCHAR)(pstLine_->uColor & 0xFF);

    // Clock the pixel data out
    TFT_CD_OUT |= TFT_CD_PIN;
    TFT_CS_OUT &= ~TFT_CS_PIN;
    while (usPixels--)
    {
        TFT_SPI_WRITE(ucHigh);
        TFT_SPI_WRITE(ucLow);
    }
    TFT_CS_OUT |= TFT_CS_PIN;
}

//---------------------------------------------------------------------------
void GraphicsST7735::FastHLine(DrawLine_t *pstLine_)
{
    // Tell the driver which pixels we're going to write
    // (in a bounding-box 1-pixel high, n-pixels wide)
    {
        DrawRectangle_t stRect;

        stRect.usLeft = pstLine_->usX1;
        stRect.usRight = pstLine_->usX2;
        stRect.usTop = pstLine_->usY1;
        stRect.usBottom = stRect.usTop;

        SetOpWindow(&stRect);
    }

    // Count the pixels
    K_USHORT usPixels = pstLine_->usX2 - pstLine_->usX1 + 1;

    // Set the high/low bytes of the color that we're going to write
    K_UCHAR ucHigh    = (K_UCHAR)((pstLine_->uColor) >> 8);
    K_UCHAR ucLow     = (K_UCHAR)(pstLine_->uColor & 0xFF);

    // Clock the pixel data out
    TFT_CD_OUT |= TFT_CD_PIN;
    TFT_CS_OUT &= ~TFT_CS_PIN;
    while (usPixels--)
    {
        TFT_SPI_WRITE(ucHigh);
        TFT_SPI_WRITE(ucLow);
    }
    TFT_CS_OUT |= TFT_CS_PIN;
}

//---------------------------------------------------------------------------
void GraphicsST7735::SetOpWindow(DrawRectangle_t *pstRectangle_) 
{
    // Set Column limits
    WriteCommand(ST7735_CASET);
	WriteData(0x00);
    WriteData(m_ucColStart + pstRectangle_->usLeft);
	WriteData(0x00);
    WriteData(m_ucColStart + pstRectangle_->usRight);

    // Set row limits
    WriteCommand(ST7735_RASET);
	WriteData(0x00);
    WriteData(m_ucRowStart + pstRectangle_->usTop);
	WriteData(0x00);
    WriteData(m_ucRowStart + pstRectangle_->usBottom);

    WriteCommand(ST7735_RAMWR);
}

//---------------------------------------------------------------------------
void GraphicsST7735::ClearScreen()
{
	DrawRectangle_t stRect;

    // Optimized operation - clear the screen by performing a giant filled
    // rectangle write.
	stRect.usLeft = 0;
    stRect.usRight = m_usResX - 1;
	stRect.usTop = 0;
    stRect.usBottom = m_usResY - 1;
    stRect.bFill = true;
    stRect.uLineColor = COLOR_BLACK;
    stRect.uFillColor = COLOR_BLACK;

    Rectangle(&stRect);
}
//---------------------------------------------------------------------------
void GraphicsST7735::Rectangle(DrawRectangle_t *pstRectangle_)
{    
    // Write one-big-area if this is a filled rectangle
    if (pstRectangle_->bFill)
    {
        // Set the window that we're going to set
        if (pstRectangle_->uLineColor != pstRectangle_->uFillColor)
        {
            pstRectangle_->usTop++;
            pstRectangle_->usBottom--;
            pstRectangle_->usLeft++;
            pstRectangle_->usRight--;
        }

        SetOpWindow(pstRectangle_);

        K_ULONG ulPixels =  (K_ULONG)(pstRectangle_->usBottom - pstRectangle_->usTop + 1) *
                            (K_ULONG)(pstRectangle_->usRight - pstRectangle_->usLeft + 1);

        // Set the high/low bytes of the color that we're going to write
        K_UCHAR ucHigh   = (K_UCHAR)((pstRectangle_->uFillColor) >> 8);
        K_UCHAR ucLow    = (K_UCHAR)(pstRectangle_->uFillColor & 0xFF);

        // Clock the pixel data out
        TFT_CD_OUT |= TFT_CD_PIN;
        TFT_CS_OUT &= ~TFT_CS_PIN;
        while (ulPixels--)
        {
            TFT_SPI_WRITE(ucHigh);
            TFT_SPI_WRITE(ucLow);
        }
        TFT_CS_OUT |= TFT_CS_PIN;

        // If the line/fill colors are the same, then bail here.
        if (pstRectangle_->uLineColor == pstRectangle_->uFillColor)
        {
            return;
        }

        pstRectangle_->usTop--;
        pstRectangle_->usBottom++;
        pstRectangle_->usLeft--;
        pstRectangle_->usRight++;
    }

    // Draw the Horizontal/Vertical components.
    DrawLine_t stLine;

    // Top/Bottom lines
    stLine.usX1 = pstRectangle_->usLeft;
    stLine.usX2 = pstRectangle_->usRight;
    stLine.usY1 = pstRectangle_->usTop;
    stLine.usY2 = stLine.usY1;
    stLine.uColor = pstRectangle_->uLineColor;
    FastHLine(&stLine);

    stLine.usY1 = pstRectangle_->usBottom;
    stLine.usY2 = stLine.usY1;
    FastHLine(&stLine);

    // Left/Right lines
    stLine.usX2 = pstRectangle_->usLeft;
    stLine.usY1 = pstRectangle_->usTop;
    FastVLine(&stLine);

    stLine.usX1 = pstRectangle_->usRight;
    stLine.usX2 = pstRectangle_->usRight;
    FastVLine(&stLine);
}

//---------------------------------------------------------------------------
void GraphicsST7735::Line(DrawLine_t *pstLine_)
{
    // If the X values are the same, Use the "fast" vertical line code
    if (pstLine_->usX1 == pstLine_->usX2)
    {
        FastVLine(pstLine_);
    }
    // If the Y values are the same, use the "fast" horizontal line code
    else if (pstLine_->usY1 == pstLine_->usY2)
    {
        FastHLine(pstLine_);
    }

    // Just use the normal line-drawing algorithm.
    GraphicsDriver::Line(pstLine_);
}

//---------------------------------------------------------------------------
void GraphicsST7735::Point(DrawPoint_t *pstPoint_)
{
    // Check for pixel data within limits...
    if ((pstPoint_->usX >= m_usResX) || (pstPoint_->usY >= m_usResY))
    {
        return;
    }

    // Write row/column address
    K_UCHAR ucX = m_ucColStart + pstPoint_->usX;

    WriteCommand(ST7735_CASET);
    WriteData(0x00);
    WriteData(ucX);
    WriteData(0x00);
    WriteData(ucX);

    K_UCHAR ucY = m_ucRowStart + pstPoint_->usY;

    WriteCommand(ST7735_RASET);
    WriteData(0x00);
    WriteData(ucY);
    WriteData(0x00);
    WriteData(ucY);

    WriteCommand(ST7735_RAMWR);

    // Get pixel color data in high/low bytes
    K_UCHAR ucHigh   = (K_UCHAR)((pstPoint_->uColor) >> 8);
    K_UCHAR ucLow    = (K_UCHAR)(pstPoint_->uColor & 0xFF);

    // Write the pixel data out
    TFT_CD_OUT |= TFT_CD_PIN;
    TFT_CS_OUT &= ~TFT_CS_PIN;

    TFT_SPI_WRITE(ucHigh);
    TFT_SPI_WRITE(ucLow);

    TFT_CS_OUT |= TFT_CS_PIN;
}

//---------------------------------------------------------------------------
void GraphicsST7735::Bitmap(DrawBitmap_t *pstBitmap_)
{
    // Only draw 16bpp data - add algorithms to convert later...
    if (pstBitmap_->ucBPP != 16)
    {
        return;
    }

    DrawRectangle_t stRect;
    // Set the window used for drawing the image.
    stRect.usLeft   = pstBitmap_->usX;
    stRect.usRight  = stRect.usLeft + pstBitmap_->usWidth - 1;
    stRect.usTop    = pstBitmap_->usY;
    stRect.usBottom = stRect.usTop + pstBitmap_->usHeight - 1;

    SetOpWindow(&stRect);

    K_ULONG ulPixels = (K_ULONG)pstBitmap_->usWidth *
                        (K_ULONG)pstBitmap_->usHeight;

    K_UCHAR *pucData = pstBitmap_->pucData;

    // Write the pixel data out, assuming native 16-bit format.
    TFT_CD_OUT |= TFT_CD_PIN;
    TFT_CS_OUT &= ~TFT_CS_PIN;

    while (ulPixels--)
    {
        // Get pixel color data in high/low bytes
        K_UCHAR ucLow    = *pucData++;
		K_UCHAR ucHigh   = *pucData++;        

        TFT_SPI_WRITE(ucHigh);
        TFT_SPI_WRITE(ucLow);
    }

    TFT_CS_OUT |= TFT_CS_PIN;
}
