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

    \file   kernel++.cpp 

    \brief  Test harness

*/

//---------------------------------------------------------------------------

#include "kerneltypes.h"
#include "kernel.h"
#include "thread.h"
#include "drvUART.h"
#include "slip.h"
#include "slip_mux.h"

#include "font.h"
#include "arial_10.h"

#include <avr/io.h>
#include <avr/sleep.h>

//---------------------------------------------------------------------------
// Need to have this to handle pure-virtual functions
extern "C" void __cxa_pure_virtual() {}

//---------------------------------------------------------------------------
// Global objects
static Thread AppThread;			//!< Main "application" thread
static Thread IdleThread;			//!< Idle thread - runs when app can't

static ATMegaUART clUART;			//!< UART device driver object

//---------------------------------------------------------------------------
#define STACK_SIZE_APP		(256)	//!< Size of the main app's stack
#define STACK_SIZE_IDLE		(256)	//!< Size of the idle thread stack

//---------------------------------------------------------------------------
#define UART_SIZE_RX		(32)	//!< UART RX Buffer size
#define UART_SIZE_TX		(64)	//!< UART TX Buffer size

//---------------------------------------------------------------------------
static K_UCHAR aucAppStack[STACK_SIZE_APP];
static K_UCHAR aucIdleStack[STACK_SIZE_IDLE];

//---------------------------------------------------------------------------
static K_UCHAR aucTxBuffer[UART_SIZE_TX];
static K_UCHAR aucRxBuffer[UART_SIZE_RX];

//---------------------------------------------------------------------------
static void AppEntry(void);
static void IdleEntry(void);

void MemSet( void *pvData_, K_UCHAR ucValue_, K_USHORT usCount_ )
{
	K_UCHAR *pucData = (K_UCHAR*)pvData_;
	K_USHORT i;
	for (i = 0; i < usCount_; i++)
	{
		pucData[i]= ucValue_;
	}
	
}

//---------------------------------------------------------------------------
int main(void)
{		
	Kernel::Init();						//!< MUST be before other kernel ops
	
	AppThread.Init(	aucAppStack,		//!< Pointer to the stack
					STACK_SIZE_APP,		//!< Size of the stack
					1,					//!< Thread priority
					(ThreadEntry_t)AppEntry,	//!< Entry function
					(void*)&AppThread );//!< Entry function argument

	IdleThread.Init( aucIdleStack,		//!< Pointer to the stack
					 STACK_SIZE_IDLE,	//!< Size of the stack
					 0,					//!< Thread priority		
					 (ThreadEntry_t)IdleEntry,	//!< Entry function
					 NULL );			//!< Entry function argument
	
	AppThread.Start();					//!< Schedule the threads
	IdleThread.Start();
	
	clUART.SetName("/dev/tty");			//!< Add the serial driver
	clUART.Init();	
	
	DriverList::Add( &clUART );
	
	Kernel::Start();					//!< Start the kernel!
}

void Font_PrintGlyph( Font_t *pstFont_, K_UCHAR ucGlyph_ )
{
	K_UCHAR i, j;
	K_UCHAR ucMask;
	
	K_UCHAR ucWidth;
	K_UCHAR ucHeight;
	
	K_UCHAR ucTempByte;
	
	K_USHORT usOffset = 0;
		
	// If this font uses a subset of printable characters, 
	// remove the offset before doing any calculations/pointer math
	ucGlyph_ -= pstFont_->ucStartChar;
	
	// Read the glyphs from memory until we arrive at the one we wish to print
	for (i = 0; i < ucGlyph_; i++)
	{
		// Glyphs are variable-sized for efficiency - to look up a particular 
		// glyph, we must traverse all preceding glyphs in the list
		ucWidth  = Font_ReadByte(usOffset, pstFont_->pucFontData);
		ucHeight = Font_ReadByte(usOffset + 1, pstFont_->pucFontData);

		// Adjust the offset to point to the next glyph
		usOffset += ((((K_USHORT)ucWidth + 7) >> 3) * (K_USHORT)ucHeight) 
					+ (sizeof(Glyph_t) - 1);		
	}

	// Echo the character out to terminal, using an X for a lit pixel
	// and a space for a vacant pixel
	
	ucWidth  = Font_ReadByte(usOffset, pstFont_->pucFontData);
	ucHeight = Font_ReadByte(usOffset+1, pstFont_->pucFontData);
	
	usOffset += sizeof(Glyph_t) - 1;
	
	ucTempByte = Font_ReadByte(usOffset, pstFont_->pucFontData);
	for (i = 0; i < ucHeight; i++)
	{
		ucMask = 0x80;
		for (j = 0; j < ucWidth; j++)
		{
			if (!ucMask)
			{
				ucMask = 0x80;
				usOffset++;
				ucTempByte = Font_ReadByte(usOffset, pstFont_->pucFontData);
			}
			
			if (ucTempByte & ucMask)
			{
				while( 0 == clUART.Write(1, (K_UCHAR*)"X") ) {}
			}
			else
			{
				while( 0 == clUART.Write(1, (K_UCHAR*)" ") ) {}
			}
			
			ucMask >>= 1;
		}
		while( 0 == clUART.Write(1, (K_UCHAR*)"\n") ){}
		usOffset++;
		ucTempByte = Font_ReadByte(usOffset, pstFont_->pucFontData);
	}
	while( 0 == clUART.Write(1, (K_UCHAR*)"\n") ){}
}

//---------------------------------------------------------------------------
void AppEntry(void)
{
	// Initialize the serial mux
	SlipMux::Init("/dev/tty", UART_SIZE_RX, aucRxBuffer, UART_SIZE_TX, aucTxBuffer);
	clUART.Open();
	
	// Dummy code - just write hello world forever...
	while(1)
	{
		Font_PrintGlyph( &fntArial_10_False_False_False_, 'M' );
		Font_PrintGlyph( &fntArial_10_False_False_False_, 'a' );
		Font_PrintGlyph( &fntArial_10_False_False_False_, 'r' );
		Font_PrintGlyph( &fntArial_10_False_False_False_, 'k' );
		Font_PrintGlyph( &fntArial_10_False_False_False_, '3' );
		Thread::Sleep(1000);
	}
}

//---------------------------------------------------------------------------
void IdleEntry(void)
{
	while(1)
	{
		// LPM code;
		set_sleep_mode(SLEEP_MODE_IDLE);
		cli();
		sleep_enable();
		sei();
		sleep_cpu();
		sleep_disable();
		sei();
	}
}

