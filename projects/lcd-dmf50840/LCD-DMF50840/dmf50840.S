/*
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation, 
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
//
// LCD controller for Optrex DMF-50840 display.
// See LCD_DMF50840.c for description.
//
// Review:
// SetX now takes 2 byte arguments. Change this to SetY too.
// SetColor now a 1 byte command. Undo. Place argument in data byte
// SetPicture takes different number of data bytes
// Need to Add SetCluster command
//
#include "./avr.inc"
#include <avr/io.h>
#include <avr/portpins.h>

#include "Commoncfg.h"

	.extern TextPage
	.extern Font8x8

//-- map the IO register back into the IO address space
#define PA_DDR		(_SFR_IO_ADDR(PORTA) - 1)
#define PB_DDR		(_SFR_IO_ADDR(PORTB) - 1)
#define PC_DDR		(_SFR_IO_ADDR(PORTC) - 1)
#define PD_DDR		(_SFR_IO_ADDR(PORTD) - 1)
#define PA_OUT		_SFR_IO_ADDR(PORTA)
#define PB_OUT		_SFR_IO_ADDR(PORTB)
#define PC_OUT		_SFR_IO_ADDR(PORTC)
#define PD_OUT		_SFR_IO_ADDR(PORTD)
#define PA_IN		(_SFR_IO_ADDR(PORTA) - 2)
#define PB_IN		(_SFR_IO_ADDR(PORTB) - 2)
#define PC_IN		(_SFR_IO_ADDR(PORTC) - 2)
#define PD_IN		(_SFR_IO_ADDR(PORTD) - 2)
//
// GENERAL DEFINITIONS
//
// Vx is a memory nibble address, not a pixel address
// Vy is line number (pixel and memory values are identical)
//
// PB0..PB6 connected to SRam A0..A6 (Vx)
//
#define	SRAMVX		PB_OUT
#define SRAMVX_DDR	PB_DDR
//
// PA0..PA7 connected to A7..A14 (Vy)
//
#define	SRAMVY		PA_OUT
#define	SRAMVY_DDR	PA_DDR
//
// PD4..PD7 connected to D0..D3 (nibble data)
//
#define	SRAMDATAI	PD_IN
#define	SRAMDATAO	PD_OUT
#define	SRAMDATA_DDR PD_DDR
//
// PC0 connected to LCD Enable
//
#define	LCD_ENABLE	PC_OUT,0
#define	ENABLE_DDR	PC_DDR,0
//
// PC6 connected to software DOTCLOCK
//
#define	DOTCLK		PC_OUT,6
#define	DOTCLK_DDR	PC_DDR,6
#define	DOTCLK_BIT	6
//
// PD2 connected to LCD VSYNC
//
#define	LCD_VSYNC	PD_OUT,2
#define	VSYNC_DDR	PD_DDR,2
//
// PD3 connected to LCD HSYNC
//
#define	LCD_HSYNC	PD_OUT,3
#define	HSYNC_DDR	PD_DDR,3
//
// PC1 connected to SRam WE
//
#define	SRAM_WE		PC_OUT,1
#define	WR_DDR		PC_DDR,1
#define	WR_BIT		1
//
// PC7 connected to SRAM CE or LED
//
#define	SRAM_CE		PC_OUT,7
#define	CE_DDR		PC_DRC,7

#define	LEDI		PC_IN,7
#define	LEDO		PC_OUT,7
#define	LED_DDR		PC_DDR,7

#define COLORWHITE	0b00000001
#define COLORBLACK	0b00000000

#define BITSPERPIXEL	1
#define DATABITS		4

#define nLines	240
#define lastLine (nLines - 1)
#define nHorPixels 320
#define	nDataPerLine ((nHorPixels * BITSPERPIXEL) / DATABITS)
#define lastPixelColumn (nHorPixels - 1)

.section	.text

#define	PORTCCLKOFF		r21
#define	PORTCCLKON		r20

        .global TIMER0_COMPA_vect
//
// This clocking method uses the "change address" sram read method (read cycle #1)
//
	.macro	OneNibble
	out		SRAMVX,r19
#if USE_CE
	cbi		SRAM_CE					//Enable RAM
#endif
	inc		r19					//bits 0..6
	out		PC_OUT,PORTCCLKON	//CLK ON
	out		PC_OUT,PORTCCLKOFF	//CLK OFF
#if USE_CE
	sbi		SRAM_CE
#endif
	.endm

//
// TC0MatchHandler
//
// Executed every 76.2uSec (14,880 Hz).
//
// Refreshed one display line.
// 240 lines -> 62 Screen refreshes / second
//
TIMER0_COMPA_vect:
	push	r16
	in		r16, _SFR_IO_ADDR(SREG)
	push	r16
	clr		r16
	out		_SFR_IO_ADDR(TCNT0),r16
	push	r17
	push	r18
	push	r19
	push	PORTCCLKOFF
	push	PORTCCLKON

#if DEFDATAPORTOUT
	ldi		r16,0b00001100			//set data port to input (High Z)
	out		SRAMDATA_DDR,r16
#endif

//
// get the line we're refreshing, set line address
//
	lds		r18,RefreshLine			// Y
	out		SRAMVY,r18
//
// preload the clock toggle registers
//
	in		PORTCCLKOFF,PC_OUT
	mov		PORTCCLKON,PORTCCLKOFF
	sbr		PORTCCLKON,(1<<DOTCLK_BIT)	//DotClk On
	clr		r19						//Horizontal Nibble Address (X)

	OneNibble						//1

	tst		r18
	brne	1f
	sbi		LCD_VSYNC			//start VSYNC 
1:
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble

	OneNibble						//11
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble

	OneNibble						//21
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble

	OneNibble						//31
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble

	OneNibble						//41
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble

	OneNibble						//51
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble

	OneNibble						//61
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble

	OneNibble						//71
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
	OneNibble
//
// Horizontal Sync.
//
	sbi		LCD_HSYNC
	cbi		LCD_HSYNC

	tst		r18					//first line?
	brne	1f
	cbi		LCD_VSYNC			//end VSYNC
1:
//
// Update Line Address
//
	inc		r18
	cpi		r18,nLines
	brsh	DoLCD_LastLine
	rjmp	RefreshExit			//done

DoLCD_LastLine:
//
// start new frame
//
	clr		r18
//
// Enable display after 3 frames
//
	lds		r16,pwrup_state
	cpi		r16,3
	brsh	Turn_On_LCDX
	inc 	r16
	sts		pwrup_state,r16
	rjmp	1f
Turn_On_LCDX:
	sbi		LCD_ENABLE
1:
#if 0
//
// update M (AC) line
//
  	sbi		LCD_MTOGGLE				//toggle M (AC) line
#endif

RefreshExit:
	sts		RefreshLine,r18
#if DEFDATAPORTOUT
	ldi		r16,0b11111100			//set data port back to output
	out		SRAMDATA_DDR,r16
#endif

TExit:
	pop		PORTCCLKON
	pop		PORTCCLKOFF
	pop		r19
	pop		r18
	pop		r17
Tdone:
	pop		r16
	out		_SFR_IO_ADDR(SREG),r16
	pop		r16
	reti
#undef	PORTCCLKOFF
#undef	PORTCCLKON

	.global	InitMem

#define	RAMSTART 0x0100

InitMem:
	push	r18
	push	r20
	push	r21
	push	ZL
	push	ZH
	in		r20,_SFR_IO_ADDR(SPL)					;top
	in		r21,_SFR_IO_ADDR(SPH)
	
	ldi		r18,0xcc
	ldiw	ZL,ZH,RAMSTART			;Preset RAM
1:
	st		Z+,r18
	cp		ZL,r20
	cpc		ZH,r21
	brne	1b
	pop		ZH
	pop		ZL
	pop		r21
	pop		r20
	pop		r18
	ret


//##########################################################
//##                     ###################################
//## FONTS & BITMAPS     ###################################
//##                     ###################################

//##########################################################
	.global	Font8x8
	.global	Font8x8End

#include ".\Font8x8.inc"

Font8x8End:
	.align

	.global	DialData
	.global DialDigits
	.global SecPtrData
	.global MinPtrData
	.global HrPtrData

#include ".\qvgatable.inc"

	.align

#if SPLASHSCREEN
#include ".\RoseBM.inc"

	.global	bitmap
#endif

	.align
