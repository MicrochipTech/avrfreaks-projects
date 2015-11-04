;<$DS_HEADER>
;+--------------------------------------------------------------------------------+              
;|João D´Artagnan A. Oliveira Programmer and Author; 				  |
;|										  |
;| All rights reserved.								  |
;|										  |
;|   Redistribution and use in source and binary forms, with or without		  |
;|  modification, are permitted provided that the following conditions are met:	  |
;|										  |
;|   * Redistributions of source code must retain the above copyright		  |
;|     notice, this list of conditions and the following disclaimer.		  |
;|										  |
;|   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"  |
;|   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE    |
;|   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   |
;|   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     |
;|   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR	  |
;|   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF	  |
;|   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS	  |
;|   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN	  |
;|   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)	  |
;|   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE   |
;|   POSSIBILITY OF SUCH DAMAGE.						  |
;|										  |
;|   Brasília, Brazil November 30,2015 	06:28 am				  |
;+--------------------------------------------------------------------------------+
;</$DS_HEADER>
;+------------------------------------------------------+
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA			|
;| DATA:4/12/2005					|
;>------------------------------------------------------<
;| A V R  R A S T E R  O P E R A T I O N S  		|
;| V E R S I O N  1.0					|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH MEGA AVRS		|
;+------------------------------------------------------+
;| IMPLEMENT BASIC RASTER OPERATIONS IN MEMORY		
;|
;| GENERIC MEMORY FORMAT FOR 1,2,4 AND 8 BITS
;|
;|			|    byte 0     |    byte 1     |    byte 2     |		 |    byte N     |
;|			+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                +-+-+-+-+-+-+-+-+ \
;| START ADDR -->LINE 0 |7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|................|7|6|5|4|3|2|1|0| |
;|			+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                +---------------+ |
;|		 LINE 1 |7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|................|7|6|5|4|3|2|1|0| |
;|			+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+		 +-+-+-+-+-+-+-+-+  > Height Pixel
;|				.		.		.		.          	 .	   |
;|				.		.		.		.                .         |
;|			+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                +-+-+-+-+-+-+-+-+ |
;|		 LINE N |7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|................|7|6|5|4|3|2|1|0| |
;|			+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                +-+-+-+-+-+-+-+-+ /
;|  pixels number -->    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3		                
;|					     1 1 1 1 1 1 1 1 1 1 2 2 2 2  
;|			\------------------------------------- ------------------------------------------/
;|							      v
;|							  Width bytes
;|
;| Correlation between bytes and pixels pixel bit size is formatted as follow:
;|
;| 1 - BIT
;|      
;|	|    byte 0     |    byte 1     |
;|	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0| .............
;|	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	|0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|  <---- pixel numbering
;|	| | | | | | | | | | |1|1|1|1|1|1|
;|
;| 2 - BIT
;|      
;|	|    byte 0     |    byte 1     |
;|	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0| .............
;|	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |  <---- pixel numbering
;|	|   |   |   |   |   |   |   |   |
;|
;| 4 - BIT
;|      
;|	|    byte 0     |    byte 1     |
;|	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0| .............
;|	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	|   0   |   1   |   2   |   3   | <---- pixel numbering
;|	|       |       |       |       |
;|
;| 8 - BIT
;|      
;|	|    byte 0     |    byte 1     |
;|	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0| .............
;|	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	|       0       |       1       | <---- pixel numbering
;|	|               |               | 
;|
;+------------------------------------------------------------------------------+
;|										|
;| THIS RASTER OPERATOR IS LIMITED TO 255x255 PIXEL RESOLUTION			|
;|										|
;+------------------------------------------------------------------------------+
#ifndef __RASTER_BASE__
	#define __RASTER_BASE__
	.message "+-------------+"
	.message "|             |"
	.message "| RASTER BASE |"
	.message "|             |"
	.message "+-------------+"
	;				  ///////////////////////\\\\\\\\\\\\\\\\\\\\\\\\
	;				 //						\\
	;-------------------------------||    R A S T  B A S E  O P E R A T I O N S     ||-----------------------------------
	;				 \\						//
	;				  \\\\\\\\\\\\\\\\\\\\\\\///////////////////////
	;-------------------------< CONSTANT DEFINITONS >------------------------
	;/////////////////////////
	;// FLAG CONSTANTS
	;/////////////////////////
	.EQU	_RASTER_FLAG_BIT_RANGE	= 0X01			;and mask to field _RASTER_FLAGS
	;/////////////////////////
	;// BITBLT CONSTANTS
	;/////////////////////////
	.EQU	_RASTER_BITBLT_FUNCTION_BLACK		=1	;DRAW A BLACK BOX
	.EQU	_RASTER_BITBLT_FUNCTION_SCROLL_UP	=2	;SCROLL BOX UP LINES(X) WITH DESIDERED TIME(Y)
	.EQU	_RASTER_BITBLT_FUNCTION_SCROLL_DOWN	=3	;SCROLL BOX DOWN LINES(X) WITH DESIDERED TIME(Y)
	.EQU	_RASTER_BITBLT_FUNCTION_SCROLL_LEFT	=4	;SCROLL BOX LEFT COLUMNS(X) WITH DESIDERED TIME(Y)
	.EQU	_RASTER_BITBLT_FUNCTION_SCROLL_RIGHT	=5	;SCROLL BOX RIGHT COLUMNS(X) WITH DESIDERED TIME(Y)
	.EQU	_RASTER_BITBLT_FUNCTION_COPY_BMP_FLASH	=6	;COPY BMP FROM FLASH TO RASTER MEMORY
	.EQU	_RASTER_BITBLT_FUNCTION_WHITE		=7	;DRAW A WHITE BOX
	.EQU	_RASTER_BITBLT_FUNCTION_COLOR		=8	;DRAW A BOX WITH SPECIFIC COLOR
	;/////////////////////////
	;// CHARACTERS CONSTANTS
	;/////////////////////////
	.EQU	_RASTER_CHAR_MODE_NORMAL	= 0		;normal character
	.EQU	_RASTER_CHAR_MODE_INVERTED	= 1		;inverted character
	.EQU	_RASTER_CHAR_SOURCE_FLASH	= 1		;source FLASH
	.EQU	_RASTER_CHAR_SOURCE_SRAM	= 2		;source SRAM
	.EQU	_RASTER_CHAR_TAB_SIZE		= 4
	.EQU	_RASTER_CHAR_WIDTH		= 6		;fixed character width
	.EQU	_RASTER_CHAR_HEIGHT		= 8		;fixed character height
	;-------------------------< SRAM VARIABLES >-----------------------------
	.DSEG
		_RASTER_WIDTH:		.BYTE	_BYTE			;width of raster in pixels
		_RASTER_WIDTH_BYTES:	.BYTE	_BYTE			;width of raster in bytes
		_RASTER_HEIGHT:		.BYTE	_BYTE			;height of raster in pixels
		_RASTER_BIT_SIZE:	.BYTE	_BYTE			;size of bmp
		_RASTER_START_ADDR:	.BYTE	_WORD			;start address of raster
		_RASTER_X:		.BYTE	_WORD			;x coordinate or bitblt source coordinate
		_RASTER_Y:		.BYTE	_WORD			;y coordinate or bitblt source coordinate
		_RASTER_BITBLT_XD:	.BYTE	_WORD			;x destination coordinate
		_RASTER_BITBLT_YD:	.BYTE	_WORD			;y destination coordinate
		_RASTER_BITBLT_WIDTH:	.BYTE	_WORD			;width destination
		_RASTER_BITBLT_HEIGHT:	.BYTE	_WORD			;height destination
		_RASTER_BITBLT_XI_MASK:	.BYTE	_BYTE			;Initial X mask
		_RASTER_BITBLT_XF_MASK:	.BYTE	_BYTE			;Final X mask
		_RASTER_CHAR_FORECOLOR:	.BYTE	_BYTE			;hold a default character forecolor
		_RASTER_CHAR_BACKCOLOR:	.BYTE	_BYTE			;hold a default character backcolor
		_RASTER_CHAR_MODE:	.BYTE	_BYTE			;hold a default character mode
		_RASTER_CHAR_SOURCE:	.BYTE	_BYTE			;hold a default character source
	.CSEG
	;
	;-------------------------< M A C R O S  >-------------------------------
	;
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_INIT					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Initialize RASTER engine			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	After this initialization RASTER has default 	|
	;|		initialized values				|
	;|		_RASTER_CHAR_MODE=NORMAL			|
	;|		_RASTER_CHAR_FORECOLOR=WHITE			|
	;|		_RASTER_CHAR_BACKCOLOR=BLACK			|
	;|		_RASTER_CHAR_SOURCE=FLASH			|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_INIT:
		push	Acc
		in	Acc,SREG				;save SREG
		cli						;disable interrupts
		push	Acc
		ldi	Acc,_RASTER_CHAR_MODE_NORMAL
		sts	_RASTER_CHAR_MODE,Acc
		ldi	Acc,255				;fore color white
		sts	_RASTER_CHAR_FORECOLOR,Acc
		ldi	AccH,0				;back color = black
		sts	_RASTER_CHAR_BACKCOLOR,Acc
		ldi	Acc,_RASTER_CHAR_SOURCE_FLASH	;source is flash
		sts	_RASTER_CHAR_SOURCE,Acc
		pop	Acc
		out	SREG,Acc				;retore SREG state
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_CREATE					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Create a new RASTER with Width and Height and	|
	;|		number of bits					|
	;>--------------------------------------------------------------<
	;| INPUT      :	X	Width in pixels				|
	;|		Y	Height in pixels			|
	;|		Z-->	Memory area of raster			|
	;|		Acc	Raster bit size				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	valids bits sizes are 1,2,4 and 8 if other value|
	;|		is supplied a umpredictable result my occur,	|
	;|		area pointed by Z must be reserved space before	|
	;|		call this routine				|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_CREATE:
		push	AccH				;save useds
		pushw	X
		sts	_RASTER_BIT_SIZE,Acc		;save bit size
		stsw	_RASTER_START_ADDR,Z		;save start address
		sts	_RASTER_WIDTH,XL		;save width
		sts	_RASTER_HEIGHT,YL		;save height
		;//////////////////////////////////////////////
		;// compute size in bytes according bit size
		;//
		;// 1 bit - widthbyte=width/8
		;// 2 bit - widthbyte=width/4
		;// 4 bit - widthbyte=width/2
		;// 8 bit - widthbyte=width
		;//////////////////////////////////////////////
		ldi	AccH,3				;X>>3
		cpi	Acc,1				;1 bit size ?
		breq	_RASTER_CREATE_10		;yes, branch
		ldi	AccH,2				;X>>2
		cpi	Acc,2				;2 bit size ?
		breq	_RASTER_CREATE_10		;yes, branch
		ldi	AccH,1				;X>>1
		cpi	Acc,4				;4 bit size ?
		breq	_RASTER_CREATE_10		;yes, branch
		ldi	AccH,0				;else assume 8 bit size	
	_RASTER_CREATE_10:
		tst	AccH				;divide done ?
		breq	_RASTER_CREATE_20		;yes, branch
		lsr	XL				;X=X/2
		ror	XH
		dec	AccH
		rjmp	_RASTER_CREATE_10
	_RASTER_CREATE_20:
		sts	_RASTER_WIDTH_BYTES,XL		;save width in bytes
		popw	X				;restore useds
		pop	AccH
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_GET_WIDTH				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER get width of Raster in pixels		|
	;>--------------------------------------------------------------<
	;| INPUT      : None						|
	;| OUTPUT     :	AccH:Acc  width in pixels of Raster		|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_GET_WIDTH:
		lds	Acc,_RASTER_WIDTH
		ldi	AccH,0
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_GET_HEIGHT				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER get height of Raster in pixels		|
	;>--------------------------------------------------------------<
	;| INPUT      : None						|
	;| OUTPUT     :	AccH:Acc height in pixels of Raster		|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_GET_HEIGHT:
		lds	Acc,_RASTER_HEIGHT
		ldi	AccH,0
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_ADJUST_COLOR_MASK			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Adjust color MASK for each bit size		|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc	Color					|
	;| OUTPUT     :	Acc	Color mask				|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_ADJUST_COLOR_MASK:
		push	AccH
		;/////////////////////////////////
		;// ADJUST COLOR INPUT
		;/////////////////////////////////
		lds	AccH,_RASTER_BIT_SIZE			;get bit size
		cpi	AccH,1					;1 bit ?
		breq	_RASTER_ADJUST_COLOR_MASK_10		;yes, branch
		cpi	AccH,2					;2 bit ?
		breq	_RASTER_ADJUST_COLOR_MASK_20		;yes, branch
		cpi	AccH,4					;4 bit ?
		breq	_RASTER_ADJUST_COLOR_MASK_30		;yes, branch
		rjmp	_RASTER_ADJUST_COLOR_MASK_40		;else assume 8 bits
		;////////////////////////////////
		;// ADJUST COLOR FOR 1 BIT
		;////////////////////////////////
	_RASTER_ADJUST_COLOR_MASK_10:	
		andi	Acc,0x1					;clear bits 1..7
		sbrc	Acc,0					;skip if bit 0=0
		ori	Acc,0xfe				;else set bit 1..7
		rjmp	_RASTER_ADJUST_COLOR_MASK_40		;process the rest
		;////////////////////////////////
		;// ADJUST COLOR FOR 2 BIT
		;////////////////////////////////
	_RASTER_ADJUST_COLOR_MASK_20:
		andi	Acc,0x3					;clear bits 2..7
		sbrc	Acc,0					;skip if bit 0=0
		ori	Acc,0b01010100				;else set bits 2,4,6
		sbrc	Acc,1					;skip if bit 1=0
		ori	Acc,0b10101000				;else set bits 3,5,7
		rjmp	_RASTER_ADJUST_COLOR_MASK_40		;process the rest
		;////////////////////////////////
		;// ADJUST COLOR FOR 4 BIT
		;////////////////////////////////
	_RASTER_ADJUST_COLOR_MASK_30:
		mov	AccH,Acc				;copy bits 0.3-->4..7
		swap	AccH
		andi	AccH,0xf0
		andi	Acc,0x0f
		or	Acc,AccH
	_RASTER_ADJUST_COLOR_MASK_40:
		pop	AccH
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_CREATE					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Clear Frame with Color/Level Raster 		|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc Color/Level					|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+------------------------
	_RASTER_FILL:
		pushw	Z
		pushaw
		pushawt
		ldsw	Z,_RASTER_START_ADDR		;get start Raster start address
		lds	AccT,_RASTER_WIDTH_BYTES	;get width in bytes
		lds	AccTH,_RASTER_HEIGHT		;get height
		rcall	_RASTER_ADJUST_COLOR_MASK	;get color mask
		rjmp	_RASTER_CLEAR_10		;jump fill routine
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_CLEAR					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Clear Frame raster bits to Black or zero	|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+------------------------
	_RASTER_CLEAR:
		pushw	Z
		pushaw
		pushawt
		ldsw	Z,_RASTER_START_ADDR		;get start Raster start address
		lds	AccT,_RASTER_WIDTH_BYTES	;get width in bytes
		lds	AccTH,_RASTER_HEIGHT		;get height
		clr	Acc
	_RASTER_CLEAR_10:
		st	Z+,Acc				;clear Raster
		dec	AccT				;width--==0?
		brne	_RASTER_CLEAR_10		;no, continue
		lds	AccT,_RASTER_WIDTH_BYTES	;get width in bytes again
		dec	AccTH				;height--==0?
		brne	_RASTER_CLEAR_10		;no, continue		
	_RASTER_CLEAR_20:
		popawt
		popaw
		popw	Z
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_FILL_ROWS				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Fill Frame rows raster bits pattern 		|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc     Pattern					|
	;|		YL	start row				|
	;|		YH	number of rows				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	Flags,r0,r1					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+------------------------
	_RASTER_FILL_ROWS:
		pushw	Z
		pushaw
		pushawt
		mov	AccT,YL				;get start row
		add	AccT,YH				;add with number of rows
		brcc	_RASTER_FILL_ROWS_10		;branch if no overflow
		lds	AccTH,_RASTER_HEIGHT		;get height
		sub	AccTH,YL			;YH=height-start row
		mov	YH,AccTH			
		rjmp	_RASTER_FILL_ROWS_20
	_RASTER_FILL_ROWS_10:
		lds	AccTH,_RASTER_HEIGHT		;get height
		cp	AccT,AccTH			;number of rows>total rows?
		brlo	_RASTER_FILL_ROWS_20		;no, branch
		sub	AccTH,YL			;YH=height-start row
		mov	YH,AccTH
	_RASTER_FILL_ROWS_20:
		ldsw	Z,_RASTER_START_ADDR		;get start Raster start address
		lds	AccT,_RASTER_WIDTH_BYTES	;get width in bytes
		mul	AccT,YL				;compute start address
		add	ZL,r0
		adc	ZH,r1
		mov	AccH,YH				;AccH=number of rows
	_RASTER_FILL_ROWS_30:
		tst	AccH				;last row ?
		breq	_RASTER_FILL_ROWS_50		;yes, branch
		pushw	Z				;save Z
		lds	AccT,_RASTER_WIDTH_BYTES	;number of byte into one line
	_RASTER_FILL_ROWS_40:
		st	Z+,Acc				;set Raster pattern
		dec	AccT				;width--==0?
		brne	_RASTER_FILL_ROWS_40		;no, continue
		popw	Z				;restore Z
		lds	AccT,_RASTER_WIDTH_BYTES	;get width in bytes again
		add	ZL,AccT				;point to next row
		ldi	AccT,0
		adc	ZH,AccT
		dec	AccH				;height--==0?
		brne	_RASTER_FILL_ROWS_30		;no, continue		
	_RASTER_FILL_ROWS_50:
		popawt
		popaw
		popw	Z
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_PIXEL_ADDR				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Compute initial address of pixel for further set|
	;|		or reset					|
	;>--------------------------------------------------------------<
	;| INPUT      :	X,Y	pixel coordinate			|
	;| OUTPUT     :	Z-->	pixel address				|
	;|		AccH	pixel mask				|
	;|		CY=1	if address out of screen area		|
	;| DESTROYED  :	r0,r1,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_PIXEL_ADDR:
		;//////////////////////////////////////////////
		;//
		;// if X<0 or X>=width then CY<-1 exit
		;// if Y<0 or Y>=height then CY<-1 exit
		;//
		;//////////////////////////////////////////////		
		pushawt						;save useds
		tst	XH					;X<0 ?
		lbrmi	_RASTER_SET_PIXEL_ADDR_EXIT_INVALID	;yes, exit
		tst	YH					;Y<0 ?
		lbrmi	_RASTER_SET_PIXEL_ADDR_EXIT_INVALID	;yes, exit
		lds	AccH,_RASTER_WIDTH			;X>=Width ?
		cp	XL,AccH
		ldi	AccH,0
		cpc	XH,AccH
		lbrge	_RASTER_SET_PIXEL_ADDR_EXIT_INVALID	;yes, exit
		lds	AccH,_RASTER_HEIGHT			;Y>=Height ?
		cp	YL,AccH
		ldi	AccH,0
		cpc	YH,AccH
		lbrge	_RASTER_SET_PIXEL_ADDR_EXIT_INVALID	;yes, exit
		;//////////////////////////////////////////////
		;// COMPUTE PIXEL_START_ADDRESS by Y value
		;//
		;// ADDR(r2:r1:r0)=(start addr)+(Y*(unsigned)Width Bytes)+X/2^(3-log2(bitsize))
		;//
		;//////////////////////////////////////////////		
		ldsw	Z,_RASTER_START_ADDR			;Z-->raster pixels
		lds	AccH,_RASTER_WIDTH_BYTES			;get raster width bytes
		mul	YL,AccH					;(Y*Width_bytes)
		add	ZL,r0
		adc	ZH,r1
		;//////////////////////////////////////////////
		;// COMPUTE 2^(3-log2(bitsize))
		;//////////////////////////////////////////////
		rcall	_RASTER_PIXELS_TO_BYTES		;AccT:AccH=X/X/2^(3-log2(bitsize))
		add	ZL,AccT				;ADDR+=X/2^(3-log2(bitsize))
		adc	ZH,AccTH
		;//////////////////////////////////////////////
		;// COMPUTE MASK X
		;//
		;// 1 BIT
		;//
		;//    VALUE 	 MASK
		;// X =-2 	00000010
		;// X =-1 	00000001
		;// X = 0  	10000000
		;// X = 1  	01000000
		;// X = 2  	00100000
		;//
		;// 2 BIT
		;//
		;//    VALUE 	 MASK
		;// X =-2 	00001100
		;// X =-1 	00000011
		;// X = 0  	11000000
		;// X = 1  	00110000
		;// X = 2  	00001100
		;//
		;// 4 BIT
		;//
		;//    VALUE 	 MASK
		;// X =-2 	11110000
		;// X =-1 	00001111
		;// X = 0  	11110000
		;// X = 1  	00001111
		;//
		;// 8 BIT
		;//
		;//    VALUE 	 MASK
		;// X = 0  	11111111
		;//
		;//////////////////////////////////////////////
		lds	AccH,_RASTER_BIT_SIZE			;get bit size
		cpi	AccH,1					;1 bit ?
		breq	_RASTER_SET_PIXEL_ADDR_1_BIT		;yes, branch
		cpi	AccH,2					;2 bit ?
		breq	_RASTER_SET_PIXEL_ADDR_2_BIT		;yes, branch
		cpi	AccH,4					;4 bit ?
		breq	_RASTER_SET_PIXEL_ADDR_4_BIT		;yes, branch
		;////////////////////
		;// 8 BIT MASK
		;////////////////////
		ldi	AccH,0xff				;if none of above then assume 8 bit mask
		rjmp	_RASTER_SET_PIXEL_ADDR_EXIT_VALID	;exit
		;////////////////////
		;// 1 BIT MASK
		;////////////////////
	_RASTER_SET_PIXEL_ADDR_1_BIT:
		pushw	Z
		ldiw	Z,_RASTER_TABLE_MASK_1_BIT*2		;z--> MASK BITS
		mov	AccH,XL					;get number of bit
		andi	AccH,7
		add	ZL,AccH
		ldi	AccH,0
		adc	ZH,AccH
		lpm	AccH,Z					;get mask
		popw	Z
		rjmp	_RASTER_SET_PIXEL_ADDR_EXIT_VALID	;exit
		;////////////////////
		;// 2 BIT MASK
		;////////////////////
	_RASTER_SET_PIXEL_ADDR_2_BIT:
		pushw	Z
		ldiw	Z,_RASTER_TABLE_MASK_2_BIT*2		;z--> MASK BITS
		mov	AccH,XL					;get number of bit
		andi	AccH,3
		add	ZL,AccH
		ldi	AccH,0
		adc	ZH,AccH
		lpm	AccH,Z					;get mask
		popw	Z
		rjmp	_RASTER_SET_PIXEL_ADDR_EXIT_VALID	;exit
		;////////////////////
		;// 4 BIT MASK
		;////////////////////
	_RASTER_SET_PIXEL_ADDR_4_BIT:
		ldi	AccH,0xf0				;assume bit 0=0
		sbrc	XL,0					;skip if really yes
		ldi	AccH,0x0f				;else bit 0=1
	_RASTER_SET_PIXEL_ADDR_EXIT_VALID:
		clc						;set valid address range
	_RASTER_SET_PIXEL_ADDR_EXIT:
		popawt
		ret
	_RASTER_SET_PIXEL_ADDR_EXIT_INVALID:			
		sec						;set invalid address range
		rjmp	_RASTER_SET_PIXEL_ADDR_EXIT
	;////////////////////////////////
	;// TABLE OF MASK FOR ALL BIT MODES
	;////////////////////////////////
	_RASTER_TABLE_MASK_1_BIT:
		.DB	0X80,0X40,0X20,0X10,0X08,0X04,0X02,0X01		;<--- MASK TABLE FOR 1 BIT
	_RASTER_TABLE_MASK_2_BIT:
		.DB	0XC0,0X030,0X0C,0X03				;<--- MASK TABLE FOR 2 BIT
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_PIXELS_TO_BYTES				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Get X in bytes from X in pixels			|
	;|		Computed as follow X/2^(3-log2(bitsize))	|
	;>--------------------------------------------------------------<
	;| INPUT      :	X	   X in Pixels				|
	;| OUTPUT     :	AccTH:AccT X in bytes				|
	;| DESTROYED  :	r0,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_PIXELS_TO_BYTES:
		movw	AccT,X				;AccTH:AccT=X
		lds	r0,_RASTER_BIT_SIZE		;get bit size
	_RASTER_PIXELS_TO_BYTES_10:
		sbrc	r0,3				;log computed ?
		rjmp	_RASTER_PIXELS_TO_BYTES_20	;yes, branch
		asr	AccTH				;x=x/2 (signed)
		ror	AccT
		lsl	r0
		rjmp	_RASTER_PIXELS_TO_BYTES_10
	_RASTER_PIXELS_TO_BYTES_20:
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_PIXEL				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Set/Reset pixel at coordinates X,Y		|
	;>----------------------------------------------------------------------<
	;| INPUT      :	X,Y			Pixel coordinate		|
	;|		Acc			Pixel Color/Level		|
	;| OUTPUT     :	None							|
	;| DESTROYED  :	r0,r1,Flags						|
	;| TIME	      :	----							|
	;>----------------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_PIXEL:
		push	Temp				;save useds
		pushaw
		pushw	Z
		stsw	_RASTER_X,X			;store coordinates
		stsw	_RASTER_Y,Y
		rcall	_RASTER_SET_PIXEL_ADDR		;compute area to pixel
		mov	Temp,AccH			;Temp=bit mask
		brcs	_RASTER_SET_PIXEL_EXIT		;exit if at invalid area
		rcall	_RASTER_ADJUST_COLOR_MASK
		and	Acc,Temp			;Acc=valid bits
		ld	AccH,Z				;get screen pixels
		com	Temp
		and	AccH,Temp			;mask bit to be set
		or	Acc,AccH			;bound pixels
		st	Z,Acc				;store back
	_RASTER_SET_PIXEL_EXIT:
		popw	Z
		popaw
		pop	Temp
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_BITBLT_SET_DEST				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER Bit Block Transfer Set Destination 	|
	;>--------------------------------------------------------------<
	;| INPUT      :	X,Y		Destination Pixel coordinate	|
	;|		AccH:Acc	Destination Width		|
	;|		AccTH:AccT	Destination Height		|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_BITBLT_SET_DEST:
		stsw	_RASTER_BITBLT_XD,X			;set coordinates
		stsw	_RASTER_BITBLT_YD,Y
		stsaw	_RASTER_BITBLT_WIDTH			;set width and height
		stsawt	_RASTER_BITBLT_HEIGHT
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_BITBLT_SET_SOURCE			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER Bit Block Transfer Set Destination 	|
	;>--------------------------------------------------------------<
	;| INPUT      :	X,Y		Source Pixel coordinate		|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_BITBLT_SET_SOURCE:
		stsw	_RASTER_X,X				;set coordinates
		stsw	_RASTER_Y,Y
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_BITBLT_COMPUTE_XI_MASK			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Compute Initil mask of X coordinate		|
	;>--------------------------------------------------------------<
	;| INPUT      : X	coordinate 				|
	;| OUTPUT     : _RASTER_BITBLT_XI_MASK Xi Mask			|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_BITBLT_COMPUTE_XI_MASK:
		pushaw
		lds	Acc,_RASTER_BIT_SIZE			;get bit size
		cpi	Acc,1					;1 bit ?
		breq	_RASTER_BITBLT_COMPUTE_XI_MASK_1_BIT	;yes, branch
		cpi	Acc,2					;2 bit ?
		breq	_RASTER_BITBLT_COMPUTE_XI_MASK_2_BIT	;yes, branch
		cpi	Acc,4					;4 bit ?
		breq	_RASTER_BITBLT_COMPUTE_XI_MASK_4_BIT	;yes, branch
		clr	Acc
		sts	_RASTER_BITBLT_XI_MASK,Acc		;else assume 8 bits
		rjmp	_RASTER_BITBLT_COMPUTE_XI_MASK_EXIT
		;///////////////////
		;// MASK FOR 1 BIT
		;///////////////////
	_RASTER_BITBLT_COMPUTE_XI_MASK_1_BIT:
		ldi	AccH,0					;clear mask
		mov	Acc,XL					;get X
	_RASTER_BITBLT_COMPUTE_XI_MASK_1_BIT_10:
		andi	Acc,7					;zero ?
		breq	_RASTER_BITBLT_COMPUTE_XI_MASK_1_BIT_20	;yes, branch
		sec						;else compute mask
		ror	AccH
		dec	Acc
		rjmp	_RASTER_BITBLT_COMPUTE_XI_MASK_1_BIT_10	;until done
	_RASTER_BITBLT_COMPUTE_XI_MASK_1_BIT_20:
		sts	_RASTER_BITBLT_XI_MASK,AccH
		rjmp	_RASTER_BITBLT_COMPUTE_XI_MASK_EXIT	;exit
		;///////////////////
		;// MASK FOR 2 BIT
		;///////////////////
	_RASTER_BITBLT_COMPUTE_XI_MASK_2_BIT:
		ldi	AccH,0					;clear mask
		mov	Acc,XL					;get X
	_RASTER_BITBLT_COMPUTE_XI_MASK_2_BIT_10:
		andi	Acc,3					;zero ?
		breq	_RASTER_BITBLT_COMPUTE_XI_MASK_2_BIT_20	;yes, branch
		sec						;else compute mask
		ror	AccH
		sec
		ror	AccH
		dec	Acc
		rjmp	_RASTER_BITBLT_COMPUTE_XI_MASK_2_BIT_10	;until done
	_RASTER_BITBLT_COMPUTE_XI_MASK_2_BIT_20:
		sts	_RASTER_BITBLT_XI_MASK,AccH
		rjmp	_RASTER_BITBLT_COMPUTE_XI_MASK_EXIT	;exit
		;///////////////////
		;// MASK FOR 4 BIT
		;///////////////////
	_RASTER_BITBLT_COMPUTE_XI_MASK_4_BIT:
		ldi	AccH,0x00
		sbrc	XL,0					;skip if xl bit 0=0
		ldi	AccH,0xF0
		sts	_RASTER_BITBLT_XI_MASK,AccH
	_RASTER_BITBLT_COMPUTE_XI_MASK_EXIT:
		popaw	ret
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_BITBLT_COMPUTE_XF_MASK			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Compute final mask of X coordinate		|
	;>--------------------------------------------------------------<
	;| INPUT      : X	coordinate 				|
	;| OUTPUT     : _RASTER_BITBLT_XF_MASK Xf Mask			|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_BITBLT_COMPUTE_XF_MASK:
		pushaw
		lds	Acc,_RASTER_BIT_SIZE			;get bit size
		cpi	Acc,1					;1 bit ?
		breq	_RASTER_BITBLT_COMPUTE_XF_MASK_1_BIT	;yes, branch
		cpi	Acc,2					;2 bit ?
		breq	_RASTER_BITBLT_COMPUTE_XF_MASK_2_BIT	;yes, branch
		cpi	Acc,4					;4 bit ?
		breq	_RASTER_BITBLT_COMPUTE_XF_MASK_4_BIT	;yes, branch
		clr	Acc
		sts	_RASTER_BITBLT_XF_MASK,Acc		;else assume 8 bits
		rjmp	_RASTER_BITBLT_COMPUTE_XF_MASK_EXIT
		;///////////////////
		;// MASK FOR 1 BIT
		;///////////////////
	_RASTER_BITBLT_COMPUTE_XF_MASK_1_BIT:
		ldi	AccH,0					;clear mask
		mov	Acc,XL					;get X
		com	Acc					;7-X
	_RASTER_BITBLT_COMPUTE_XF_MASK_1_BIT_10:
		andi	Acc,7					;zero ?
		breq	_RASTER_BITBLT_COMPUTE_XF_MASK_1_BIT_20	;yes, branch
		sec						;else compute mask
		rol	AccH
		dec	Acc
		rjmp	_RASTER_BITBLT_COMPUTE_XF_MASK_1_BIT_10	;until done
	_RASTER_BITBLT_COMPUTE_XF_MASK_1_BIT_20:
		sts	_RASTER_BITBLT_XF_MASK,AccH
		rjmp	_RASTER_BITBLT_COMPUTE_XF_MASK_EXIT	;exit
		;///////////////////
		;// MASK FOR 2 BIT
		;///////////////////
	_RASTER_BITBLT_COMPUTE_XF_MASK_2_BIT:
		ldi	AccH,0					;clear mask
		mov	Acc,XL					;get X
		com	Acc					;3-x
	_RASTER_BITBLT_COMPUTE_XF_MASK_2_BIT_10:
		andi	Acc,3					;zero ?
		breq	_RASTER_BITBLT_COMPUTE_XF_MASK_2_BIT_20	;yes, branch
		sec						;else compute mask
		rol	AccH
		sec
		rol	AccH
		dec	Acc
		rjmp	_RASTER_BITBLT_COMPUTE_XF_MASK_2_BIT_10	;until done
	_RASTER_BITBLT_COMPUTE_XF_MASK_2_BIT_20:
		sts	_RASTER_BITBLT_XF_MASK,AccH
		rjmp	_RASTER_BITBLT_COMPUTE_XF_MASK_EXIT	;exit
		;///////////////////
		;// MASK FOR 4 BIT
		;///////////////////
	_RASTER_BITBLT_COMPUTE_XF_MASK_4_BIT:
		ldi	AccH,0x0F
		sbrc	XL,0					;skip if xl bit 0=0
		ldi	AccH,0x00
		sts	_RASTER_BITBLT_XF_MASK,AccH
	_RASTER_BITBLT_COMPUTE_XF_MASK_EXIT:
		popaw	ret
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_BITBLT_COMPUTE				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER Bit Block Transfer compute parameters	|
	;|		XdI initial destination coordinate in bytes	|
	;|		XdF final destination coordinate in byte	|
	;|		H  (number of rows)height of the box		|
	;|		XIM Initial mask				|
	;|		XFM Final mask					|
	;>--------------------------------------------------------------<
	;| INPUT      : None		 				|
	;| OUTPUT     :	XL	XdI coordinate				|
	;|		XH	XdF coordinate				|
	;|		AccH	Number of rows				|
	;|		_RASTER_BITBLT_XI_MASK	XI mask			|
	;|		_RASTER_BITBLT_XF_MASK	XF mask			|
	;|		Y	Address of RASTER pixels (FIRST ROW)	|
	;| 	        Z	Address of RASTER pixels (LAST ROW)	|
	;|		CY=1	if out of screen			|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_BITBLT_COMPUTE:
		push	Acc
		pushawt
		ldsawt	_RASTER_X					;save X,Y
		pushawt
		ldsaw	_RASTER_Y
		pushawt							;save used
		ldsw	X,_RASTER_BITBLT_XD				;get XdI
		lds	Acc,_RASTER_WIDTH				;XdI>=WIDTH
		cp	XL,Acc
		ldi	Acc,0
		cpc	XH,Acc
		lbrge	_RASTER_BITBLT_COMPUTE_EXIT_INVALID		;address invalid exit
		tst	XH						;XdI<0 ?
		brpl	_RASTER_BITBLT_COMPUTE_10			;no, branch
		ldiw	X,0						;else XD=0
	_RASTER_BITBLT_COMPUTE_10:
		stsw	_RASTER_X,X
		rcall	_RASTER_BITBLT_COMPUTE_XI_MASK			;compute XI mask
		rcall	_RASTER_PIXELS_TO_BYTES				;get in bytes
		push	AccT						;save XdI in bytes
		ldsw	X,_RASTER_BITBLT_XD				;XdF=XdI+Width
		subi	XL,1
		sbci	XH,0
		ldsaw	_RASTER_BITBLT_WIDTH
		add	XL,Acc
		adc	XH,AccH						;XdI>0 ?
		brpl	_RASTER_BITBLT_COMPUTE_20			;yes, branch
		pop	AccT						;restore XdI
		rjmp	_RASTER_BITBLT_COMPUTE_EXIT_INVALID		;address invalid exit
	_RASTER_BITBLT_COMPUTE_20:
		lds	Acc,_RASTER_WIDTH				;get Width
		cp	XL,Acc						;XdF >= Width ?
		ldi	Acc,0
		cpc	XH,Acc
		brlt	_RASTER_BITBLT_COMPUTE_30			;no, branch
		lds	XL,_RASTER_WIDTH				;get width
		ldi	XH,0						;else XdF=Width-1
		subiw	X,1						
	_RASTER_BITBLT_COMPUTE_30:
		rcall	_RASTER_BITBLT_COMPUTE_XF_MASK			;compute XF mask
		rcall	_RASTER_PIXELS_TO_BYTES				;get in bytes
		pop	XL						;XL=XdI in bytes
		mov	XH,AccT						;XH=XdF in bytes
		ldsw	Y,_RASTER_BITBLT_YD				;get YdI
		lds	Acc,_RASTER_HEIGHT				;YdI>=HEIGHT
		cp	YL,Acc
		ldi	Acc,0
		cpc	YH,Acc
		lbrge	_RASTER_BITBLT_COMPUTE_EXIT_INVALID		;address invalid exit
		tst	YH						;YdI<0 ?
		brpl	_RASTER_BITBLT_COMPUTE_50			;no, branch
		ldiw	Y,0						;else Ydi=0
	_RASTER_BITBLT_COMPUTE_50:
		stsw	_RASTER_Y,Y
		push	YL
		ldsw	Y,_RASTER_BITBLT_YD				;YdF=YdI+Height
		subi	YL,1
		sbci	YH,0
		ldsaw	_RASTER_BITBLT_HEIGHT
		add	YL,Acc
		adc	YH,AccH
		brpl	_RASTER_BITBLT_COMPUTE_60			;no, branch
		pop	YL						;restore XdI
		rjmp	_RASTER_BITBLT_COMPUTE_EXIT_INVALID		;address invalid exit
	_RASTER_BITBLT_COMPUTE_60:
		lds	Acc,_RASTER_HEIGHT				;get Height
		cp	YL,Acc						;YdF >= Height?
		ldi	Acc,0
		cpc	YH,Acc
		brlt	_RASTER_BITBLT_COMPUTE_70			;no, branch
		lds	YL,_RASTER_HEIGHT				;get height
		ldi	YH,0						;else YdF=Height-1
		subiw	Y,1						
	_RASTER_BITBLT_COMPUTE_70:
		mov	YH,YL
		pop	YL						;YL=YdI in bytes
		pushw	X						;save xi,xf
		pushw	Y						;save yi,yf
		ldsw	X,_RASTER_X					;compute RASTER PIXEL address
		ldsw	Y,_RASTER_Y
		rcall	_RASTER_SET_PIXEL_ADDR
		movw	AccT,ZL						;AccTH:AccT=start address
		ldsw	X,_RASTER_X
		popw	Y						;restore yi,yf
		pushw	Y
		mov	YL,YH						;get yf
		ldi	YH,0
		rcall	_RASTER_SET_PIXEL_ADDR				;z=end address
		popw	Y
		sub	YH,YL						;compute number of rows
		inc	YH
		mov	AccH,YH
		movw	YL,AccT						;Y=screen(start row)
		popw	X
		clc
		rjmp	_RASTER_BITBLT_COMPUTE_EXIT			;exit
	_RASTER_BITBLT_COMPUTE_EXIT_INVALID:
		sec							;set invalid exit
	_RASTER_BITBLT_COMPUTE_EXIT:
		popawt							;restore useds
		stsawt	_RASTER_Y
		popawt
		stsawt	_RASTER_X
		popawt
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_BITBLT 					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER Execute Bit Block Trasnfer functions	|
	;>--------------------------------------------------------------<
	;| INPUT      : Acc  RASTER FUNCTION 				|
	;|		AccH parameter					|
	;|		X,Y  parameter 					|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,r2,r3,Flags				|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|
	;|	FUNCTION		DESCRIPTION			PARAMETER
	;|
	;|
	;|	BLACK			DRAW A BLACK BOX		NONE
	;|	SCROLL_UP		SCROLL BOX UP LINES		X=QUANTITY,Y=DELAY
	;|	SCROLL_DOWN		SCROLL BOX DOWN LINES		X=QUANTITY,Y=DELAY
	;|	SCROLL_LEFT		SCROLL BOX LEFT COLUMNS		X=QUANTITY,Y=DELAY
	;|	SCROLL_RIGHT		SCROLL BOX RIGHT COLUMNS	X=QUANTITY,Y=DELAY	
	;|	COPY_BMP_FLASH		COPY BMP FROM FLASH		NONE
	;|	WHITE			DRAW A WHITE BOX		NONE
	;|	COLOR			DRAW A BOX WITH COLOR		AccH=COLOR
	;|
	_RASTER_BITBLT:
		pushaw
		pushw	X
		pushw	Y
	_RASTER_BITBLT_00:
		rcall	_RASTER_BITBLT_EXECUTE
		cpi	Acc,_RASTER_BITBLT_FUNCTION_COLOR		;exit if no parameters
		breq	_RASTER_BITBLT_MAIN_EXIT
		cpi	Acc,_RASTER_BITBLT_FUNCTION_BLACK		;exit if no parameters
		breq	_RASTER_BITBLT_MAIN_EXIT
		cpi	Acc,_RASTER_BITBLT_FUNCTION_WHITE		;exit if no parameters
		breq	_RASTER_BITBLT_MAIN_EXIT
		cpi	Acc,_RASTER_BITBLT_FUNCTION_COPY_BMP_FLASH	;exit if no parameters
		breq	_RASTER_BITBLT_MAIN_EXIT
		push	Acc
		movw	Acc,YL
		call	_DELAY_MILLISECONDS				;wait a desidered time
		pop	Acc
		subiw	X,1
		brne	_RASTER_BITBLT_00
	_RASTER_BITBLT_MAIN_EXIT:
		popw	Y
		popw	X
		popaw	
		ret
		;/////////////////////////////////////////////////
		;//						//
		;// MAIN MODULE THAT EXECUTE ALL FUNCTIONS	//
		;//						//
		;/////////////////////////////////////////////////
	_RASTER_BITBLT_EXECUTE:
		pushTempW
		pushaw
		pushawt
		pushw	X
		pushw	Y
		pushw	Z
		mov	r4,AccH					;save color
		rcall	_RASTER_BITBLT_COMPUTE			;compute area and ger parameters
		lbrcs	_RASTER_BITBLT_EXIT			;exit if any invalid parameter
		;////////////////////////////////////
		;// SOME ADJUST FOR SCROLL UP
		;////////////////////////////////////
		cpi	Acc,_RASTER_BITBLT_FUNCTION_SCROLL_UP	;adjust end address if scroll_up function
		brne	_RASTER_BITBLT_10A			;no, branch
		movw	ZL,YL					;Z=Y
		lds	Temp,_RASTER_WIDTH_BYTES		;Z=SCREEN(YI+1)
		add	ZL,Temp
		ldi	Temp,0
		adc	ZH,Temp
		dec	AccH					;row counter -1
	_RASTER_BITBLT_10A:
		;////////////////////////////////////
		;// SOME ADJUST FOR SCROLL DOWN
		;////////////////////////////////////	
		cpi	Acc,_RASTER_BITBLT_FUNCTION_SCROLL_DOWN	;adjust end address if scroll_down function
		brne	_RASTER_BITBLT_10B			;no, branch
		movw	YL,ZL					;Y=Z
		lds	Temp,_RASTER_WIDTH_BYTES		;Z=SCREEN(YI-1)
		sub	ZL,Temp
		ldi	Temp,0
		sbc	ZH,Temp
		dec	AccH					;row counter -1
	_RASTER_BITBLT_10B:
		;////////////////////////////////////
		;// SOME ADJUST FOR SCROLL LEFT
		;////////////////////////////////////	
		cpi	Acc,_RASTER_BITBLT_FUNCTION_SCROLL_LEFT	;adjust end address if scroll_left function
		brne	_RASTER_BITBLT_10C			;no, branch
		movw	ZL,YL					;Z=Y+1
		adiw	ZL,1
	_RASTER_BITBLT_10C:
		;////////////////////////////////////
		;// SOME ADJUST FOR SCROLL RIGHT
		;////////////////////////////////////	
		cpi	Acc,_RASTER_BITBLT_FUNCTION_SCROLL_RIGHT;adjust end address if scroll_right function
		brne	_RASTER_BITBLT_10D			;no, branch
		mov	Temp,XH					;Y=Y+WIDTH
		sub	Temp,XL
		inc	Temp
		add	YL,Temp
		ldi	Temp,0
		adc	YH,Temp
		movw	ZL,YL					;Z=Y-1
		sbiw	ZL,1
	_RASTER_BITBLT_10D:
		;////////////////////////////////////
		;// SOME ADJUST FOR COLOR BOX
		;////////////////////////////////////	
		cpi	Acc,_RASTER_BITBLT_FUNCTION_COLOR	;adjust color pixel if color function
		brne	_RASTER_BITBLT_10E			;no, branch
		push	Acc
		mov	Acc,r4					;get color
		rcall	_RASTER_ADJUST_COLOR_MASK		;adjust color mask
		mov	r4,Acc
		pop	Acc
	_RASTER_BITBLT_10E:
		mov	AccTH,Acc				;AccTH=function
		lds	Temp,_RASTER_BITBLT_XI_MASK		;get initial mask
		lds	TempH,_RASTER_BITBLT_XF_MASK		;get final mask
		lds	AccT,_RASTER_BIT_SIZE			;AccT=bit size
		mov	Acc,AccH				;Acc=row counter
		tst	Acc					;exit if no row
		lbreq	_RASTER_BITBLT_EXIT
	_RASTER_BITBLT_10:
		push	Acc					;save row counter
		mov	Acc,XL					;Acc=XI
		pushw	Y					;save Y,Z
		pushw	Z
	_RASTER_BITBLT_20:
		push	Acc					;save xi
		;///////////////////////////////////////////////////////////////////////////
		;//
		;//  F U N C T I O N   C O L O R 
		;//
		;///////////////////////////////////////////////////////////////////////////
		cpi	AccTH,_RASTER_BITBLT_FUNCTION_COLOR
		brne	_RASTER_BITBLT_BLACK
		;//////////////////////////
		;// WHEN XI=XF
		;//////////////////////////
		cp	XL,XH					;xi=xf?
		brne	_RASTER_BITBLT_COLOR_10			;no, branch
		ld	r0,Y					;get old pixel
		mov	r1,Temp					;bound masks
		or	r1,TempH
		com	r1
		mov	r2,r4					;r2=color
		and	r2,r1
		com	r1
		and	r0,r1					;bound mask xi and xf
		or	r0,r2					;bound color
		st	Y+,r0					;store pixel back
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XI=X
		;//////////////////////////
	_RASTER_BITBLT_COLOR_10:
		cp	XL,Acc					;xi=x?
		brne	_RASTER_BITBLT_COLOR_20			;no, branch
		ld	r0,Y					;get old pixel
		com	Temp
		mov	r2,r4					;mask color
		and	r2,Temp
		com	Temp
		and	r0,Temp					;bound mask xi and xf
		or	r0,r2
		st	Y+,r0					;store pixel back
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XF=X
		;//////////////////////////
	_RASTER_BITBLT_COLOR_20:
		cp	XH,Acc					;xf=x?
		brne	_RASTER_BITBLT_COLOR_30			;no, branch
		ld	r0,Y					;get old pixel
		com	TempH
		mov	r2,r4					;mask color
		and	r2,TempH
		com	TempH
		and	r0,TempH				;bound mask xi and xf
		or	r0,r2
		st	Y+,r0					;store pixel back
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XI<>X AND XF<>X
		;//////////////////////////
	_RASTER_BITBLT_COLOR_30:
		st	Y+,r4					;store pixel color
		rjmp	_RASTER_BITBLT_NEXT
		;///////////////////////////////////////////////////////////////////////////
		;//
		;//  F U N C T I O N   B L A C K  
		;//
		;///////////////////////////////////////////////////////////////////////////
	_RASTER_BITBLT_BLACK:
		cpi	AccTH,_RASTER_BITBLT_FUNCTION_BLACK
		brne	_RASTER_BITBLT_SCROLL_UP
		;//////////////////////////
		;// WHEN XI=XF
		;//////////////////////////
		cp	XL,XH					;xi=xf?
		brne	_RASTER_BITBLT_BLACK_10			;no, branch
		ld	r0,Y					;get old pixel
		mov	r1,Temp
		or	r1,TempH
		and	r0,r1					;bound mask xi and xf
		st	Y+,r0					;store pixel back
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XI=X
		;//////////////////////////
	_RASTER_BITBLT_BLACK_10:
		cp	XL,Acc					;xi=x?
		brne	_RASTER_BITBLT_BLACK_20			;no, branch
		ld	r0,Y					;get old pixel
		mov	r1,Temp
		and	r0,r1					;bound mask xi and xf
		st	Y+,r0					;store pixel back
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XF=X
		;//////////////////////////
	_RASTER_BITBLT_BLACK_20:
		cp	XH,Acc					;xf=x?
		brne	_RASTER_BITBLT_BLACK_30			;no, branch
		ld	r0,Y					;get old pixel
		mov	r1,TempH
		and	r0,r1					;bound mask xi and xf
		st	Y+,r0					;store pixel back
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XI<>X AND XF<>X
		;//////////////////////////
	_RASTER_BITBLT_BLACK_30:
		clr	r0					;set all bits
		st	Y+,r0					;store pixel back
		rjmp	_RASTER_BITBLT_NEXT
		;///////////////////////////////////////////////////////////////////////////
		;//
		;//  F U N C T I O N   S C R O L L  U P  O R  D O W N 
		;//
		;///////////////////////////////////////////////////////////////////////////
	_RASTER_BITBLT_SCROLL_UP:
		cpi	AccTH,_RASTER_BITBLT_FUNCTION_SCROLL_UP
		breq	_RASTER_BITBLT_SCROLL_UP_DOWN
		cpi	AccTH,_RASTER_BITBLT_FUNCTION_SCROLL_DOWN
		breq	_RASTER_BITBLT_SCROLL_UP_DOWN
		rjmp	_RASTER_BITBLT_SCROLL_LEFT
	_RASTER_BITBLT_SCROLL_UP_DOWN:
		;//////////////////////////
		;// WHEN XI=XF
		;//////////////////////////
		cp	XL,XH					;xi=xf?
		brne	_RASTER_BITBLT_SCROLL_UP_10		;no, branch
		mov	r0,Temp
		or	r0,TempH
		ld	r1,Z+					;get source
		com	r0
		and	r1,r0					;mask source pixels
		com 	r0
		ld	r2,Y					;get destine
		and	r2,r0					;mask destine pixels
		or	r1,r2					;bound
		st	Y+,r1
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XI=X
		;//////////////////////////
	_RASTER_BITBLT_SCROLL_UP_10:
		cp	XL,Acc					;xi=x?
		brne	_RASTER_BITBLT_SCROLL_UP_20		;no, branch
		ld	r0,Z+					;get source
		com	Temp
		and	r0,Temp
		com	Temp
		ld	r1,Y					;get destine
		and	r1,Temp
		or	r0,r1
		st	Y+,r0
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XF=X
		;//////////////////////////
	_RASTER_BITBLT_SCROLL_UP_20:
		cp	XH,Acc					;xf=x?
		brne	_RASTER_BITBLT_SCROLL_UP_30		;no, branch
		ld	r0,Z+					;get source
		com	TempH
		and	r0,TempH
		com	TempH
		ld	r1,Y					;get destine
		and	r1,TempH
		or	r0,r1
		st	Y+,r0
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XI<>X AND XF<>X
		;//////////////////////////
	_RASTER_BITBLT_SCROLL_UP_30:
		ld	r0,Z+					;destine=source
		st	Y+,r0
		rjmp	_RASTER_BITBLT_NEXT
		;///////////////////////////////////////////////////////////////////////////
		;//
		;//  F U N C T I O N   S C R O L L  L E F T
		;//
		;///////////////////////////////////////////////////////////////////////////
	_RASTER_BITBLT_SCROLL_LEFT:
		cpi	AccTH,_RASTER_BITBLT_FUNCTION_SCROLL_LEFT
		brne	_RASTER_BITBLT_SCROLL_RIGHT
		;//////////////////////////
		;// WHEN XI=XF
		;//////////////////////////
		cp	XL,XH					;xi=xf?
		brne	_RASTER_BITBLT_SCROLL_LEFT_10		;no, branch
		mov	r3,Temp
		or	r3,TempH
		ld	r0,Y					;get source
		mov	r1,r0
		and	r1,r3					;mask pixels
		mov	AccH,AccT
		lsl	r0					;rotate 
		dec	AccH
		brne	PC-2
		com	r3
		and	r0,r3
		com	r3
		or	r0,r1
		st	Y+,r0
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XI=X
		;//////////////////////////
	_RASTER_BITBLT_SCROLL_LEFT_10:
		cp	XL,Acc					;xi=x?
		brne	_RASTER_BITBLT_SCROLL_LEFT_20		;no, branch
		ld	r0,Z+					;get source
		ld	r1,Y					;get destine
		mov	r2,r1					;r2=r1
		and	r2,Temp					;mask rotate area
		mov	AccH,AccT
		lsl	r0					;rotate
		rol	r1
		dec	AccH
		brne	PC-3
		com	Temp
		and	r1,temp
		com	Temp
		or	r1,r2
		st	Y+,r1					;store back
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XF=X
		;//////////////////////////
	_RASTER_BITBLT_SCROLL_LEFT_20:
		cp	XH,Acc					;xf=x?
		brne	_RASTER_BITBLT_SCROLL_LEFT_30		;no, branch
		ld	r0,Y					;get source
		mov	r1,r0
		and	r1,TempH				;mask pixels 
		mov	AccH,AccT
		lsl	r0					;rotate 
		dec	AccH
		brne	PC-2
		com	TempH
		and	r0,TempH
		com	TempH
		or	r0,r1
		st	Y+,r0
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XI<>X AND XF<>X
		;//////////////////////////
	_RASTER_BITBLT_SCROLL_LEFT_30:
		ld	r0,Z+					;get source
		ld	r1,Y					;get destine\
		mov	AccH,AccT				;rotate
		lsl	r0
		rol	r1
		dec	AccH
		brne	PC-3
		st	Y+,r1
		rjmp	_RASTER_BITBLT_NEXT
		;///////////////////////////////////////////////////////////////////////////
		;//
		;//  F U N C T I O N   S C R O L L  R I G H T 
		;//
		;///////////////////////////////////////////////////////////////////////////
	_RASTER_BITBLT_SCROLL_RIGHT:
		cpi	AccTH,_RASTER_BITBLT_FUNCTION_SCROLL_RIGHT
		brne	_RASTER_BITBLT_COPY_BMP_FLASH
		;//////////////////////////
		;// WHEN XI=XF
		;//////////////////////////
		cp	XL,XH					;xi=xf?
		brne	_RASTER_BITBLT_SCROLL_RIGHT_10		;no, branch
		mov	r3,Temp
		or	r3,TempH
		ld	r0,Y					;get source
		mov	r1,r0
		and	r1,r3					;mask pixels
		mov	AccH,AccT
		lsr	r0					;rotate 
		dec	AccH
		brne	PC-2
		com	r3
		and	r0,r3
		com	r3
		or	r0,r1
		st	Y+,r0
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XI=X
		;//////////////////////////
	_RASTER_BITBLT_SCROLL_RIGHT_10:
		cp	XL,Acc					;xi=x?
		brne	_RASTER_BITBLT_SCROLL_RIGHT_20		;no, branch
		ld	r0,-Z					;get source
		ld	r1,-Y					;get destine
		mov	r2,r1					;r2=r1
		and	r2,TempH				;mask rotate area
		mov	AccH,AccT
		lsr	r0					;rotate
		ror	r1
		dec	AccH
		brne	PC-3
		com	TempH
		and	r1,TempH
		com	TempH
		or	r1,r2
		st	Y,r1					;store back
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XF=X
		;//////////////////////////
	_RASTER_BITBLT_SCROLL_RIGHT_20:
		cp	XH,Acc					;xf=x?
		brne	_RASTER_BITBLT_SCROLL_RIGHT_30		;no, branch
		ld	r0,-Y					;get source
		mov	r1,r0
		and	r1,Temp					;mask pixels
		mov	AccH,AccT
		lsr	r0					;rotate 
		dec	AccH
		brne	PC-2
		com	Temp
		and	r0,Temp
		com	Temp
		or	r0,r1
		st	Y,r0
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XI<>X AND XF<>X
		;//////////////////////////
	_RASTER_BITBLT_SCROLL_RIGHT_30:
		ld	r0,-Z					;get source
		ld	r1,-Y					;get destine\
		mov	AccH,AccT				;rotate
		lsr	r0
		ror	r1
		dec	AccH
		brne	PC-3
		st	Y,r1
		rjmp	_RASTER_BITBLT_NEXT
		;///////////////////////////////////////////////////////////////////////////
		;//
		;//  F U N C T I O N   C O P Y  B M P  F L A S H 
		;//
		;///////////////////////////////////////////////////////////////////////////
	_RASTER_BITBLT_COPY_BMP_FLASH:
		cpi	AccTH,_RASTER_BITBLT_FUNCTION_COPY_BMP_FLASH
		brne	_RASTER_BITBLT_WHITE
		;///////////////////////////////////////////////////////////////////////////
		;//
		;//  F U N C T I O N   W H I T E
		;//
		;///////////////////////////////////////////////////////////////////////////
	_RASTER_BITBLT_WHITE:
		cpi	AccTH,_RASTER_BITBLT_FUNCTION_WHITE
		brne	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XI=XF
		;//////////////////////////
		cp	XL,XH					;xi=xf?
		brne	_RASTER_BITBLT_WHITE_10			;no, branch
		ld	r0,Y					;get old pixel
		mov	r1,Temp
		or	r1,TempH
		com	r1
		or	r0,r1					;bound mask xi and xf
		st	Y+,r0					;store pixel back
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XI=X
		;//////////////////////////
	_RASTER_BITBLT_WHITE_10:
		cp	XL,Acc					;xi=x?
		brne	_RASTER_BITBLT_WHITE_20			;no, branch
		ld	r0,Y					;get old pixel
		mov	r1,Temp
		com	r1
		or	r0,r1					;bound mask xi and xf
		st	Y+,r0					;store pixel back
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XF=X
		;//////////////////////////
	_RASTER_BITBLT_WHITE_20:
		cp	XH,Acc					;xf=x?
		brne	_RASTER_BITBLT_WHITE_30			;no, branch
		ld	r0,Y					;get old pixel
		mov	r1,TempH
		com	r1
		or	r0,r1					;bound mask xi and xf
		st	Y+,r0					;store pixel back
		rjmp	_RASTER_BITBLT_NEXT
		;//////////////////////////
		;// WHEN XI<>X AND XF<>X
		;//////////////////////////
	_RASTER_BITBLT_WHITE_30:
		clr	r0
		dec	r0
		st	Y+,r0					;store pixel back
	_RASTER_BITBLT_NEXT:
		pop	Acc					;restore xi
		inc	Acc					;xi++
		mov	AccH,XH
		inc	AccH
		cp	Acc,AccH				;xi>xf ?
		lbrlo	_RASTER_BITBLT_20			;no, return
		popw	Z					;restore Y,Z
		popw	Y
		cpi	AccTH,_RASTER_BITBLT_FUNCTION_SCROLL_DOWN	;is scroll down
		breq	_RASTER_BITBLT_30			;no, branch
		lds	AccH,_RASTER_WIDTH_BYTES		;new row++
		add	YL,AccH
		ldi	AccH,0
		adc	YH,AccH
		lds	AccH,_RASTER_WIDTH_BYTES		;new row++
		add	ZL,AccH
		ldi	AccH,0
		adc	ZH,AccH
		rjmp	_RASTER_BITBLT_40
	_RASTER_BITBLT_30:
		lds	AccH,_RASTER_WIDTH_BYTES		;new row--
		sub	YL,AccH
		ldi	AccH,0
		sbc	YH,AccH
		lds	AccH,_RASTER_WIDTH_BYTES		;new row--
		sub	ZL,AccH
		ldi	AccH,0
		sbc	ZH,AccH
	_RASTER_BITBLT_40:
		pop	Acc					;restore row counter
		dec	Acc
		lbrne	_RASTER_BITBLT_10
	_RASTER_BITBLT_EXIT:
		popw	Z
		popw	Y
		popw	X
		popawt
		popaw
		popTempW
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_CHAR_FORECOLOR			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER Define character ForeColor		|
	;>--------------------------------------------------------------<
	;| INPUT      : Acc  Character ForeColor			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_CHAR_FORECOLOR:
		sts	_RASTER_CHAR_FORECOLOR,Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_GET_CHAR_FORECOLOR			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER get character ForeColor			|	
	;>--------------------------------------------------------------<
	;| INPUT      : None						|
	;| OUTPUT     :	Acc Character ForeColor				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_GET_CHAR_FORECOLOR:
		lds	Acc,_RASTER_CHAR_FORECOLOR
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_CHAR_BACKCOLOR			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER Define character BackColor		|
	;>--------------------------------------------------------------<
	;| INPUT      : Acc  Character BackColor			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_CHAR_BACKCOLOR:
		sts	_RASTER_CHAR_BACKCOLOR,Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_GET_CHAR_BACKCOLOR			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER get character BackColor			|
	;>--------------------------------------------------------------<
	;| INPUT      : None						|
	;| OUTPUT     :	Acc  Character BackColor			|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_GET_CHAR_BACKCOLOR:
		lds	Acc,_RASTER_CHAR_BACKCOLOR
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_CHAR_MODE				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER Define character mode			|
	;>--------------------------------------------------------------<
	;| INPUT      : Acc  Character Mode				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Modes may be Normal,Inverted,Transparent	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_CHAR_MODE:
		sts	_RASTER_CHAR_MODE,Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_GET_CHAR_MODE				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER get character mode			|
	;>--------------------------------------------------------------<
	;| INPUT      : None						|
	;| OUTPUT     :	Acc Character mode				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Modes may be Normal,Inverted,Transparent	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_GET_CHAR_MODE:
		lds	Acc,_RASTER_CHAR_MODE
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_CHAR					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER Put a ASCII character in RASTER bits	|
	;>--------------------------------------------------------------<
	;| INPUT      : X,Y  coordinate to put character		|
	;|		Acc  Character ASCII code			|
	;| OUTPUT     :	X,Y  next coordinate to put			|
	;| DESTROYED  :	r0,r1,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;| NOT IMPLEMENTED YET FOR 2 AND 8 BITS PER PIXEL
	;| 
	_RASTER_SET_CHAR:
		pushTempW
		pushaw		
		pushawt
		pushw	X
		pushw	Y						;save useds
		pushw	Z
		rcall	_RASTER_SET_PIXEL_ADDR				;compute raster address and see if 
									;coordinates x,y is valid
		lbrcs	_RASTER_SET_CHAR_EXIT				;branch if valid
		movw	r0,YL						;r1:r0=Y
		movw	YL,ZL						;Y-->RASTER pixels (start)
		;//////////////////////////////////////
		;// compute character ASCII table entry
		;//////////////////////////////////////
		ldiw	Z,_RASTER_CHARACTERS_TABLE*2			;Z-->(ascii code) entry point
		ldi	AccH,0
		lsl	Acc						;(ascii code)*8
		rol	AccH
		lsl	Acc
		rol	AccH
		lsl	Acc
		rol	AccH
		add	ZL,Acc
		adc	ZH,AccH
		;//////////////////////////////////////
		;//  put according bit size 
		;//////////////////////////////////////
		lds	Acc,_RASTER_BIT_SIZE				;get bit size
		cpi	Acc,1						;1 bit ?
		breq	_RASTER_SET_CHAR_1_BIT				;yes, branch
		cpi	Acc,2						;2 bit?
		lbreq	_RASTER_SET_CHAR_2_BIT				;yes, branch
		cpi	Acc,4						;4 bit ?
		lbreq	_RASTER_SET_CHAR_4_BIT				;yes, branch
		rjmp	_RASTER_SET_CHAR_8_BIT				;else assume 8 bits
		;//////////////////////////////////////
		;//  1 BIT
		;//////////////////////////////////////
	_RASTER_SET_CHAR_1_BIT:
		lds	Acc,_RASTER_WIDTH				;get RASTER Width
		subi	Acc,8						;Width-=8
		cp	XL,Acc						;X<(width-8)
		ldi	Acc,0
		cpc	XH,Acc
		brlt	_RASTER_SET_CHAR_1_BIT_40			;yes, branch
		;//////////////////////////////////////
		;// put only 1 char byte
		;//////////////////////////////////////
		lds	Acc,_RASTER_HEIGHT				;get height
		ldi	AccH,0
		sub	Acc,r0						;h-=Y
		sbc	AccH,r1
		cpiaw	_RASTER_CHAR_HEIGHT
		brlo	_RASTER_SET_CHAR_1_BIT_10B			;no, branch
		ldi	Acc,_RASTER_CHAR_HEIGHT				;else sature = height
	_RASTER_SET_CHAR_1_BIT_10B:
		mov	TempH,Acc					;height count
	_RASTER_SET_CHAR_1_BIT_10:
		lpm	Acc,Z+						;get ASCII code
		lds	Temp,_RASTER_CHAR_MODE				;get mode ?
		cpi	Temp,_RASTER_CHAR_MODE_NORMAL			;normal mode ?
		breq	_RASTER_SET_CHAR_1_BIT_10A			;yes, branch
		com	Acc						;else assume inverted
		andi	Acc,0b11111100
	_RASTER_SET_CHAR_1_BIT_10A:
		mov	AccH,XL						;compute MASK and ASCII bits position
		ldi	Temp,0b00000011					;initial mask
	_RASTER_SET_CHAR_1_BIT_20:
		andi	AccH,7						;ASCII code adjusted?
		breq	_RASTER_SET_CHAR_1_BIT_30			;yes, branch
		lsr	Acc						;else adjust
		sec
		ror	Temp
		dec	AccH
		rjmp	_RASTER_SET_CHAR_1_BIT_20
	_RASTER_SET_CHAR_1_BIT_30:
		ld	AccH,Y						;get RASTER bit
		and	AccH,Temp					;mask pixels bits
		or	AccH,Acc					;bound with ASCII bits
		st	Y,AccH						;store back
		lds	Acc,_RASTER_WIDTH_BYTES				;next row on RASTER
		add	YL,Acc
		ldi	Acc,0
		adc	YH,Acc
		dec	TempH						;next row ASCII
		brne	_RASTER_SET_CHAR_1_BIT_10
		rjmp	_RASTER_SET_CHAR_EXIT				;exit
		;//////////////////////////////////////
		;// put 2 char byte
		;//////////////////////////////////////
	_RASTER_SET_CHAR_1_BIT_40:
		lds	Acc,_RASTER_HEIGHT				;get height
		ldi	AccH,0
		sub	Acc,r0						;h-=Y
		sbc	AccH,r1
		cpiaw	_RASTER_CHAR_HEIGHT
		brlo	_RASTER_SET_CHAR_1_BIT_40A			;no, branch
		ldi	Acc,_RASTER_CHAR_HEIGHT				;else sature = height
	_RASTER_SET_CHAR_1_BIT_40A:
		mov	TempH,Acc					;height count
	_RASTER_SET_CHAR_1_BIT_50:
		lpm	Acc,Z+						;get ASCII code
		lds	Temp,_RASTER_CHAR_MODE				;get mode ?
		cpi	Temp,_RASTER_CHAR_MODE_NORMAL			;normal mode ?
		breq	_RASTER_SET_CHAR_1_BIT_50A			;yes, branch
		com	Acc						;else assume inverted
		andi	Acc,0b11111100
	_RASTER_SET_CHAR_1_BIT_50A:
		ldi	AccH,0						;ascii code 2n byte
		ldi	AccT,0b00000011					;inicial mask
		ldi	AccTH,0b11111111
		mov	Temp,XL						;compute MASK and ASCII bits position
	_RASTER_SET_CHAR_1_BIT_60:
		andi	Temp,7						;ASCII code adjusted?
		breq	_RASTER_SET_CHAR_1_BIT_70			;yes, branch
		lsr	Acc						;else adjust
		ror	AccH
		sec
		ror	AccT
		ror	AccTH
		dec	Temp
		rjmp	_RASTER_SET_CHAR_1_BIT_60
	_RASTER_SET_CHAR_1_BIT_70:
		ld	Temp,Y						;get RASTER bit
		and	Temp,AccT					;mask pixels bits 1 byte
		or	Temp,Acc					;bound with ASCII bits
		st	Y+,Temp						;store back
		ld	Temp,Y						;get RASTER bit 2nd byte
		and	Temp,AccTH
		or	Temp,AccH
		st	Y,Temp
		sbiw	YL,1
		lds	Acc,_RASTER_WIDTH_BYTES				;next row on RASTER
		add	YL,Acc
		ldi	Acc,0
		adc	YH,Acc
		dec	TempH						;next row ASCII
		brne	_RASTER_SET_CHAR_1_BIT_50
		rjmp	_RASTER_SET_CHAR_EXIT				;exit
		;//////////////////////////////////////
		;//  2 BIT
		;//////////////////////////////////////
	_RASTER_SET_CHAR_2_BIT:
		rjmp	_RASTER_SET_CHAR_EXIT				;<---NOT IMPLEMENTED YET
		;//////////////////////////////////////
		;//  4 BIT
		;//////////////////////////////////////
	_RASTER_SET_CHAR_4_BIT:
		lds	Acc,_RASTER_HEIGHT				;get height
		ldi	AccH,0
		sub	Acc,r0						;h-=Y
		sbc	AccH,r1
		cpiaw	_RASTER_CHAR_HEIGHT
		brlo	_RASTER_SET_CHAR_4_BIT_10			;no, branch
		ldi	Acc,_RASTER_CHAR_HEIGHT				;else sature = height
	_RASTER_SET_CHAR_4_BIT_10:
		mov	TempH,Acc					;height count
		lds	AccT,_RASTER_CHAR_FORECOLOR			;get forecolor
		andi	AccT,0x0f					;mask low nibble
		lds	AccTH,_RASTER_CHAR_BACKCOLOR			;get backcolor
		andi	AccTH,0x0f					;mask low nibble
		lds	Temp,_RASTER_CHAR_MODE				;get mode ?
		cpi	Temp,_RASTER_CHAR_MODE_NORMAL			;normal mode ?
		breq	_RASTER_SET_CHAR_4_BIT_10A			;yes, branch
		mov	Temp,AccT					;Forecolor<->Backcolor
		mov	AccT,AccTH
		mov	AccTH,Temp
	_RASTER_SET_CHAR_4_BIT_10A:
		lds	r0,_RASTER_WIDTH				;r1:r0=get width 
		clr	r1
		sbrc	XL,0						;skip if case x even
		rjmp	_RASTER_SET_CHAR_4_BIT_30
	_RASTER_SET_CHAR_4_BIT_20:
		pushw	X
		lpm	Acc,Z+						;get char code
		ldi	Temp,0
		mov	AccH,AccT					;assume forecolor of pixel 0
		sbrs	Acc,7						;skip if yes
		mov	AccH,AccTH					;else assume backcolor
		or	Temp,AccH					;bound this bit
		swap 	Temp						;prepare for next bit
		mov	AccH,AccT					;assume forecolor of pixel 1
		sbrs	Acc,6						;skip if yes
		mov	AccH,AccTH					;else assume backcolor
		or	Temp,AccH					;bound this bit
		st	Y+,Temp						;set bits 0,1
		adiw	XL,1						;x++
		cp	XL,r0						;X>width ?
		cpc	XH,r1
		brlo	PC+3
		sbiw	YL,1
		rjmp	_RASTER_SET_CHAR_4_BIT_20A			;yes, branch
		ldi	Temp,0
		mov	AccH,AccT					;assume forecolor of pixel 2
		sbrs	Acc,5						;skip if yes
		mov	AccH,AccTH					;else assume backcolor
		or	Temp,AccH					;bound this bit
		swap 	Temp						;prepare for next bit
		mov	AccH,AccT					;assume forecolor of pixel 3
		sbrs	Acc,4						;skip if yes
		mov	AccH,AccTH					;else assume backcolor
		or	Temp,AccH					;bound this bit
		st	Y+,Temp						;set bits 2,3
		adiw	XL,1						;x++
		cp	XL,r0
		cpc	XH,r1
		brlo	PC+3
		sbiw	YL,2
		rjmp	_RASTER_SET_CHAR_4_BIT_20A			;yes, branch
		ldi	Temp,0
		mov	AccH,AccT					;assume forecolor of pixel 4
		sbrs	Acc,3						;skip if yes
		mov	AccH,AccTH					;else assume backcolor
		or	Temp,AccH					;bound this bit
		swap 	Temp						;prepare for next bit
		mov	AccH,AccT					;assume forecolor of pixel 5
		sbrs	Acc,2						;skip if yes
		mov	AccH,AccTH					;else assume backcolor
		or	Temp,AccH					;bound this bit
		st	Y+,Temp						;set bits 4,5
		sbiw	YL,3
	_RASTER_SET_CHAR_4_BIT_20A:
		popw	X
		lds	Acc,_RASTER_WIDTH_BYTES				;next row on RASTER
		add	YL,Acc
		ldi	Acc,0
		adc	YH,Acc
		dec	TempH						;next row ASCII
		brne	_RASTER_SET_CHAR_4_BIT_20
		rjmp	_RASTER_SET_CHAR_EXIT				;exit 
	_RASTER_SET_CHAR_4_BIT_30:
		pushw	X
		lpm	Acc,Z+						;get char code
		ldi	Temp,0
		mov	AccH,AccT					;assume forecolor of pixel 0
		sbrs	Acc,7						;skip if yes
		mov	AccH,AccTH					;else assume backcolor
		ld	Temp,Y						;get old bits
		andi	Temp,0xf0					;mask high bits
		or	Temp,AccH					;bound with pixel 0
		st	Y+,Temp						;put back
		adiw	XL,1						;x++
		cp	XL,r0
		cpc	XH,r1
		brlo	PC+3
		sbiw	YL,1
		rjmp	_RASTER_SET_CHAR_4_BIT_30A			;yes, branch
		ldi	Temp,0
		mov	AccH,AccT					;assume forecolor of pixel 1
		sbrs	Acc,6						;skip if yes
		mov	AccH,AccTH					;else assume backcolor
		or	Temp,AccH					;bound this bit
		swap 	Temp						;prepare for next bit
		mov	AccH,AccT					;assume forecolor of pixel 2
		sbrs	Acc,5						;skip if yes
		mov	AccH,AccTH					;else assume backcolor
		or	Temp,AccH					;bound this bit
		st	Y+,Temp						;set bits 2,3
		adiw	XL,1						;x++
		cp	XL,r0
		cpc	XH,r1
		brlo	PC+3
		sbiw	YL,2
		rjmp	_RASTER_SET_CHAR_4_BIT_30A			;yes, branch
		ldi	Temp,0
		mov	AccH,AccT					;assume forecolor of pixel 3
		sbrs	Acc,4						;skip if yes
		mov	AccH,AccTH					;else assume backcolor
		or	Temp,AccH					;bound this bit
		swap 	Temp						;prepare for next bit
		mov	AccH,AccT					;assume forecolor of pixel 4
		sbrs	Acc,3						;skip if yes
		mov	AccH,AccTH					;else assume backcolor
		or	Temp,AccH					;bound this bit
		st	Y+,Temp						;set bits 4,5
		adiw	XL,1						;x++
		cp	XL,r0
		cpc	XH,r1
		brlo	PC+3
		sbiw	YL,3
		rjmp	_RASTER_SET_CHAR_4_BIT_30A			;yes, branch
		mov	AccH,AccT					;assume forecolor of pixel 3
		sbrs	Acc,2						;skip if yes
		mov	AccH,AccTH					;else assume backcolor
		swap 	AccH						;prepare for next bit
		ld	Temp,Y						;get old pixels
		andi	Temp,0x0f					;mask low bits
		or	Temp,AccH
		st	Y,Temp
		
		sbiw	YL,3
	_RASTER_SET_CHAR_4_BIT_30A:
		popw	X
		lds	Acc,_RASTER_WIDTH_BYTES				;next row on RASTER
		add	YL,Acc
		ldi	Acc,0
		adc	YH,Acc
		dec	TempH						;next row ASCII
		lbrne	_RASTER_SET_CHAR_4_BIT_30
		rjmp	_RASTER_SET_CHAR_EXIT
		;//////////////////////////////////////
		;//  8 BIT
		;//////////////////////////////////////
	_RASTER_SET_CHAR_8_BIT:
	_RASTER_SET_CHAR_EXIT:
		popw	Z
		popw	Y
		popw	X
		addiw	X,_RASTER_CHAR_WIDTH				;next char position
		popawt
		popaw
		popTempW
		ret
		
	;/////////////////////////////////////////////////
	;//
	;// INCLUDE ASCII TABLE CHARACTERS DEFINITIONS
	;//
	;/////////////////////////////////////////////////
	.INCLUDE	"GRAPH\RASTER\BASE\RASTER_ASCII_TABLE.INC"
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_CHAR_SOURCE				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER Define character source			|
	;>--------------------------------------------------------------<
	;| INPUT      : Acc  Character source				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	source may be FLASH OR SRAM			|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_CHAR_SOURCE:
		sts	_RASTER_CHAR_SOURCE,Acc
		ret
		
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_TEXT_OUT				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER put character string into RASTER memory	|
	;>--------------------------------------------------------------<
	;| INPUT      : X,Y   coordinates				|
	;|		Z---> characters string				|
	;| OUTPUT     :	X,Y   last coordinates ploted			|
	;| DESTROYED  :	r0,r1,flags					|
	;| TIME	      :	---						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	source may be FLASH OR SRAM			|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_TEXT_OUT:
		pushaw					;save useds
		pushawt
		pushTempw
		pushw	Z
		movw	AccT,XL				;AccTH:AccT=X
	_RASTER_SET_TEXT_OUT_10:
		;////////////////////////////////////////////////
		;// Get character form string
		;////////////////////////////////////////////////
		lds	Acc,_RASTER_CHAR_SOURCE		;get source
		cpi	Acc,_RASTER_CHAR_SOURCE_FLASH	;source is Flash ?
		brne	_RASTER_SET_TEXT_OUT_15		;no, branch
		lpm	Temp,Z+				;get character from Flash
		rjmp	_RASTER_SET_TEXT_OUT_16
	_RASTER_SET_TEXT_OUT_15:
		ld	Temp,Z+				;get character from SRAM
	_RASTER_SET_TEXT_OUT_16:
		tst	Temp				;0 terminator ?
		lbreq	_RASTER_SET_TEXT_OUT_EXIT	;yes, exit to done
		;////////////////////////////////////////////////
		;// Process for NULL CHAR
		;////////////////////////////////////////////////
		cpi	Temp,_NC			;NULL CHAR ?
		breq	_RASTER_SET_TEXT_OUT_10		;yes, get next char ignore it
		;////////////////////////////////////////////////
		;// Process for TAB
		;////////////////////////////////////////////////
		cpi	Temp,_TAB			;TAB control ?
		brne	_RASTER_SET_TEXT_OUT_20		;no, brach
		addiw	X,(_RASTER_CHAR_WIDTH*_RASTER_CHAR_TAB_SIZE)
		rjmp	_RASTER_SET_TEXT_OUT_10		;get next char
		;////////////////////////////////////////////////
		;// Process for CR
		;////////////////////////////////////////////////
	_RASTER_SET_TEXT_OUT_20:
		cpi	Temp,_CR			;CR control ?
		brne	_RASTER_SET_TEXT_OUT_30		;no, brach
		movw	XL,AccT				;else restore do x start coordinates
		rjmp	_RASTER_SET_TEXT_OUT_10		;get next char
		;////////////////////////////////////////////////
		;// Process for LF
		;////////////////////////////////////////////////
	_RASTER_SET_TEXT_OUT_30:
		cpi	Temp,_LF			;LF control ?
		brne	_RASTER_SET_TEXT_OUT_40		;no, brach
		addiw	Y,_RASTER_CHAR_HEIGHT		;else advance y coordinate
		rjmp	_RASTER_SET_TEXT_OUT_10		;get next char
		;////////////////////////////////////////////////
		;// DRAW character on screen
		;////////////////////////////////////////////////
	_RASTER_SET_TEXT_OUT_40:
		mov	Acc,Temp
		call	_RASTER_SET_CHAR
		rjmp	_RASTER_SET_TEXT_OUT_10		;next char
	_RASTER_SET_TEXT_OUT_EXIT:
		popw	Z
		popTempw
		popawt
		popaw
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_GET_TEXT_WIDTH				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER get character string width in pixels	|
	;>--------------------------------------------------------------<
	;| INPUT      : Z---> 	 characters string			|
	;| OUTPUT     :	AccH:Acc width in pixels			|
	;| DESTROYED  :	r0,r1,flags					|
	;| TIME	      :	---						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	source must be FLASH OR SRAM			|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_GET_TEXT_WIDTH:
		pushw	Z
		pushawt
		ldi	Acc,0					;characters count
		ldi	AccT,0					;max size
	_RASTER_GET_TEXT_WIDTH_10:
		lpm	AccH,Z+					;end ?
		tst	AccH
		breq	_RASTER_GET_TEXT_WIDTH_EXIT
		cpi	AccH,_NC				;null character
		breq	_RASTER_GET_TEXT_WIDTH_10		;yes, branch
		cpi	AccH,_CR				;carriage return
		breq	_RASTER_GET_TEXT_WIDTH_10		;yes, branch
		cpi	AccH,_LF				;line feed?
		brne	_RASTER_GET_TEXT_WIDTH_30		;no, branch
		cp	Acc,AccT				;new count> max ?
		brlo	_RASTER_GET_TEXT_WIDTH_20		;no, branch
		mov	AccT,Acc				;else get max
	_RASTER_GET_TEXT_WIDTH_20:
		clr	Acc					;clear char count
		rjmp	_RASTER_GET_TEXT_WIDTH_10
	_RASTER_GET_TEXT_WIDTH_30:
		inc	Acc					;char.count++
		rjmp	_RASTER_GET_TEXT_WIDTH_10
	_RASTER_GET_TEXT_WIDTH_EXIT:
		cp	Acc,AccT				;new count> max ?
		brlo	_RASTER_GET_TEXT_WIDTH_EXIT_A		;no, branch
		mov	AccT,Acc				;else max char.count
	_RASTER_GET_TEXT_WIDTH_EXIT_A:
		ldi	AccTH,_RASTER_CHAR_WIDTH		;get character width
		mul	AccT,AccTH
		movw	Acc,r0					;AccH:Acc string width pixels
		popawt
		popw	Z
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_GET_TEXT_HEIGHT				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER get character string height in pixels	|
	;>--------------------------------------------------------------<
	;| INPUT      : Z---> 	 characters string			|
	;| OUTPUT     :	AccH:Acc height in pixels			|
	;| DESTROYED  :	r0,r1,flags					|
	;| TIME	      :	---						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	source must be FLASH				|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_GET_TEXT_HEIGHT:
		pushw	Z
		ldi	Acc,1					;line count
	_RASTER_GET_TEXT_HEIGHT_10:
		lpm	AccH,Z+					;end ?
		tst	AccH
		breq	_RASTER_GET_TEXT_HEIGHT_EXIT
		cpi	AccH,_LF				;line feed?
		brne	_RASTER_GET_TEXT_HEIGHT_10		;no, branch
		inc	Acc					;line++
		rjmp	_RASTER_GET_TEXT_HEIGHT_10
	_RASTER_GET_TEXT_HEIGHT_EXIT:
		ldi	ZL,_RASTER_CHAR_HEIGHT			;get char height
		mul	Acc,ZL
		movw	Acc,r0					;get height
		popw	Z
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_TEXT_OUT_FLASH			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER put character string into RASTER memory	|
	;|		From FLASH string 0 terminated			|
	;>--------------------------------------------------------------<
	;| INPUT      : X,Y   coordinates				|
	;|		Z---> characters string				|
	;| OUTPUT     :	X,Y   last coordinates ploted			|
	;| DESTROYED  :	r0,r1,flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_TEXT_OUT_FLASH:
		push	Acc
		lds	Acc,_RASTER_CHAR_SOURCE
		push	Acc
		ldi	Acc,_RASTER_CHAR_SOURCE_FLASH
	_RASTER_SET_TEXT_OUT_FLASH_10:
		rcall	_RASTER_SET_CHAR_SOURCE
		rcall	_RASTER_SET_TEXT_OUT
		pop	Acc
		rcall	_RASTER_SET_CHAR_SOURCE
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_TEXT_OUT_SRAM			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER put character string into RASTER memory	|
	;|		From SRAM string 0 terminated			|
	;>--------------------------------------------------------------<
	;| INPUT      : X,Y   coordinates				|
	;|		Z---> characters string				|
	;| OUTPUT     :	X,Y   last coordinates ploted			|
	;| DESTROYED  :	r0,r1,flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_TEXT_OUT_SRAM:
		push	Acc
		lds	Acc,_RASTER_CHAR_SOURCE
		push	Acc
		ldi	Acc,_RASTER_CHAR_SOURCE_SRAM
		rjmp	_RASTER_SET_TEXT_OUT_FLASH_10
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_HLINE				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER draw horizontal line			|
	;>--------------------------------------------------------------<
	;| INPUT      : X,Y  	   Start coordinate			|
	;|		AccTH:AccT Width				|
	;|		Acc        Color				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	---						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_HLINE:
		pushaw						;save useds
		pushawt
		push	Acc
		movw	Acc,AccT				;set width
		ldiawt	1					;set height=1
		call	_RASTER_BITBLT_SET_DEST
		pop	Acc
		mov	AccH,Acc
		ldi	Acc,_RASTER_BITBLT_FUNCTION_COLOR	;set BITBLT=COLOR
		rcall	_RASTER_BITBLT				;draw hline
		popawt						;restore useds
		popaw
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_VLINE				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER draw horizontal line			|
	;>--------------------------------------------------------------<
	;| INPUT      : X,Y  	   Start coordinate			|
	;|		AccTH:AccT Height				|
	;|		Acc        Color				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	---						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_VLINE:
		pushaw						;save useds
		pushawt
		push	Acc
		ldiaw	1					;set width=1
		call	_RASTER_BITBLT_SET_DEST
		pop	Acc
		mov	AccH,Acc
		ldi	Acc,_RASTER_BITBLT_FUNCTION_COLOR	;set BITBLT=COLOR
		rcall	_RASTER_BITBLT				;draw hline
		popawt						;restore useds
		popaw
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_BOX					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER draw box 				|
	;>--------------------------------------------------------------<
	;| INPUT      : X,Y  	   Start coordinate			|
	;|		AccH:Acc   Width				|
	;|		AccTH:AccT Height				|
	;|		Temp        Color				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	---						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_BOX:
		pushaw
		pushawt
		pushawt						;save height
		push	Acc
		movw	AccT,Acc				;set width
		mov	Acc,Temp				;set color
		rcall	_RASTER_SET_HLINE			;draw up side
		pop	Acc
		popawt						;restore height
		pushw	Y					;save Y
		add	YL,AccT					;compute YF
		adc	YH,AccTH
		subiw	Y,1
		push	Acc
		pushawt						;save height
		movw	AccT,Acc
		mov	Acc,Temp				;set color
		rcall	_RASTER_SET_HLINE			;draw down side
		popawt						;restore height
		pop	Acc
		popw	Y
		push	Acc
		mov	Acc,Temp
		rcall	_RASTER_SET_VLINE			;draw left side
		pop	Acc
		pushw	X
		push	Acc
		add	XL,Acc					;compute XF
		adc	XH,AccH
		subiw	X,1
		mov	Acc,Temp				;set color
		rcall	_RASTER_SET_VLINE
		pop	Acc
		popw	X
		popawt						;restore height
		popaw
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_BOX_FULL				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER draw box FULL 				|
	;>--------------------------------------------------------------<
	;| INPUT      : X,Y  	   Start coordinate			|
	;|		AccH:Acc   Width				|
	;|		AccTH:AccT Height				|
	;|		Temp       Border Color				|
	;|		TempH	   Fill Color				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	---						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_BOX_FULL:
		pushw	X						;save useds
		pushw	Y
		pushaw
		pushawt
		call	_RASTER_SET_BOX					;draw border
		addiw	X,1						;adjust coordinates for background
		addiw	Y,1
		subiaw	2
		subiawt	2
		call	_RASTER_BITBLT_SET_DEST				;set destination area
		ldi	Acc,_RASTER_BITBLT_FUNCTION_COLOR		;set color
		mov	AccH,TempH
		call	_RASTER_BITBLT					;draw back with specific color
		popawt							;restore useds
		popaw
		popw	Y
		popw	X
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_HLINE_FAST				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER draw horizonal line Fast method		|
	;>--------------------------------------------------------------<
	;| INPUT      : XL,YL 	   Start coordinate			|
	;|		XH	   Width				|
	;|		Temp       Color				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,r2,r4,r5,flags				|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	This routine not allow negative coordinates	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_HLINE_FAST:
		pushaw							;save useds
		pushw	X
		pushw	Z
		;//////////////////////////////
		;//
		;// CHECK IF IN VALID RANGE
		;//
		;// done : XL=xi XH=xf YL=yi
		;//////////////////////////////
		lds	Acc,_RASTER_HEIGHT				;get height
		cp	YL,Acc						;yi>=height?
		lbrsh	_RASTER_SET_HLINE_FAST_EXIT			;yes, exit
		lds	Acc,_RASTER_WIDTH				;get width 
		cp	XL,Acc						;xi>=width ?
		lbrsh	_RASTER_SET_HLINE_FAST_EXIT			;yes, exit
		add	XH,XL						;xf=xi+width-1
		brcc	_RASTER_SET_HLINE_FAST_10			;branch if no overflow
		ldi	XH,255						;else xf=255
	_RASTER_SET_HLINE_FAST_10:
		subi	XH,1
		cp	XH,Acc						;xf>=width
		brlo	_RASTER_SET_HLINE_FAST_20			;no, branch
		mov	XH,Acc						;else xf=width-1
		dec	XH
	_RASTER_SET_HLINE_FAST_20:
		;//////////////////////////////
		;//
		;// COMPUTE START AND END MASK
		;//
		;// r4=start mask r5=end mask
		;//////////////////////////////
		ldiw	Z,_RASTER_SET_HLINE_FAST_TABLE_START*2		;Z-->START table
		mov	Acc,XL						;get bit
		andi	Acc,7						;mask bits
		add	ZL,Acc						;compute START mask
		ldi	Acc,0
		adc	ZH,Acc
		lpm	r4,Z						;get START mask
		ldiw	Z,_RASTER_SET_HLINE_FAST_TABLE_END*2		;Z-->END table
		mov	Acc,XH						;get bit
		andi	Acc,7						;mask bits
		add	ZL,Acc						;compute END mask
		ldi	Acc,0
		adc	ZH,Acc
		lpm	r5,Z						;get END mask
		;//////////////////////////////
		;//
		;// COMPUTE START POSITION(1 bit)
		;//
		;// m<-(yi*width_bytes)+xl/8
		;//////////////////////////////
		ldsw	Z,_RASTER_START_ADDR				;Z-->raster pixels
		lds	Acc,_RASTER_WIDTH_BYTES				;get raster width bytes
		mul	Acc,YL						;(yi*width_bytes)
		add	ZL,r0
		adc	ZH,r1
		mov	Acc,XL						;xi/8
		lsr	Acc
		lsr	Acc
		lsr	Acc
		add	ZL,Acc						;Z-->(yi*width_bytes)+xi/8
		ldi	Acc,0
		adc	ZH,Acc
		;//////////////////////////////
		;//
		;// COMPUTE XI,XF IN BYTES
		;//
		;// Xi_bytes=Xi/8 Xf_bytes=Xf/8
		;//////////////////////////////
		lsr	XL						;xi/8
		lsr	XL
		lsr	XL
		lsr	XH						;xf/8
		lsr	XH
		lsr	XH
		;//////////////////////////////
		;//
		;// CREATE COLOR MASK
		;//
		;//////////////////////////////
		andi	Temp,1						;prepare color 
		sbrc	Temp,0						;skip if zero
		ori	Temp,0xfe					;else set all bits
		;//////////////////////////////
		;//
		;// CHECK CASE IF XI=XF
		;//
		;//////////////////////////////
		cp	XL,XH						;Xi=Xf
		brne	_RASTER_SET_HLINE_FAST_20A			;no, branch
		ld	r0,Z						;get actual color bits
		mov	r1,Temp						;get color
		mov	r2,r4						;bound mask xi and xf
		or	r2,r5
		and	r0,r2						;extract field to be set
		com	r2						;not mask
		and	r1,r2						;extract color field
		or	r0,r1
		st	Z,r0						;store back and increment address
		rjmp	_RASTER_SET_HLINE_FAST_EXIT			;exit
		;//////////////////////////////
		;//
		;// MAIN LOOP
		;//
		;// for Acc=xi to xf
		;//////////////////////////////
	_RASTER_SET_HLINE_FAST_20A:
		mov	Acc,XL						;Acc=x=xi
	_RASTER_SET_HLINE_FAST_30:
		cp	Acc,XL						;x=xi?
		brne	_RASTER_SET_HLINE_FAST_40			;no, branch
		ld	r0,Z						;get actual color bits
		mov	r1,Temp						;get color
		and	r0,r4						;extract field to be set
		com	r4
		and	r1,r4						;extract color field
		com	r4
		or	r0,r1
		st	Z+,r0						;store back and increment address
		rjmp	_RASTER_SET_HLINE_FAST_60
	_RASTER_SET_HLINE_FAST_40:
		cp	Acc,XH						;x=xf ?
		brne	_RASTER_SET_HLINE_FAST_50				;no, branch
		ld	r0,Z						;get actual color bits
		mov	r1,Temp						;get color
		and	r0,r5						;extract field to be set
		com	r5
		and	r1,r5						;extract color field
		com	r5
		or	r0,r1
		st	Z+,r0						;store back and increment address
		rjmp	_RASTER_SET_HLINE_FAST_60
	_RASTER_SET_HLINE_FAST_50:
		st	Z+,Temp						;if x<>xi and x<>xf then store color
	_RASTER_SET_HLINE_FAST_60:
		inc	Acc						;x++
		mov	AccH,XH
		inc	AccH
		cp	Acc,AccH					;x>xf
		brlo	_RASTER_SET_HLINE_FAST_30			;until transfer done
	_RASTER_SET_HLINE_FAST_EXIT:					;program exit
		popw	Z						;restore useds
		popw	X
		popaw
		ret
		; ///////////////////////
		;//			//
		;//   START MASK TABLE 	//
		;//			//
		; ///////////////////////
	_RASTER_SET_HLINE_FAST_TABLE_START:
		.DB	0x00,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe
		; ///////////////////////
		;//			//
		;//    END MASK TABLE 	//
		;//			//
		; ///////////////////////
	_RASTER_SET_HLINE_FAST_TABLE_END:
		.DB	0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x01,0x00
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_BOX_FULL_FAST			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER draw box Fast method			|
	;>--------------------------------------------------------------<
	;| INPUT      : XL,YL 	   Start coordinate			|
	;|		XH	   Width				|
	;|		YH	   Height				|
	;|		Temp       Color				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,r2,r3,r4,r5,flags				|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	This routine not allow negative coordinates	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_BOX_FULL_FAST:
		pushw	Y					;save useds
	_RASTER_SET_BOX_FULL_FAST_10:
		tst	YH					;done ?
		breq	_RASTER_SET_BOX_FULL_FAST_EXIT		;yes, exit
		rcall	_RASTER_SET_HLINE_FAST			;draw hline fast
		inc	YL					;yi++
		dec	YH
		rjmp	_RASTER_SET_BOX_FULL_FAST_10		;until all
	_RASTER_SET_BOX_FULL_FAST_EXIT:
		popw	Y
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_VLINE_FAST				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER draw vertical line Fast method		|
	;>--------------------------------------------------------------<
	;| INPUT      : XL,YL 	   Start coordinate			|
	;|		YH	   Height				|
	;|		Temp       Color				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,r2,r3,r4,flags				|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	This routine not allow negative coordinates	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_VLINE_FAST:
		pushaw							;save useds
		pushw	Y
		pushw	Z
		;//////////////////////////////
		;//
		;// CHECK IF IN VALID RANGE
		;//
		;// done : YL=yi YH=yf XL=xi
		;//////////////////////////////
		lds	Acc,_RASTER_WIDTH				;get width 
		cp	XL,Acc						;xi>=width ?
		lbrsh	_RASTER_SET_VLINE_FAST_EXIT			;yes, exit
		lds	Acc,_RASTER_HEIGHT				;get height
		cp	YL,Acc						;yi>=height?
		lbrsh	_RASTER_SET_VLINE_FAST_EXIT			;yes, exit
		add	YH,YL						;yf=yi+height-1
		brcc	_RASTER_SET_VLINE_FAST_10			;branch if no overflow
		ldi	YH,255						;else xf=255
	_RASTER_SET_VLINE_FAST_10:
		subi	YH,1
		cp	YH,Acc						;yf>=height
		brlo	_RASTER_SET_VLINE_FAST_20			;no, branch
		mov	YH,Acc						;else yf=height-1
		dec	YH
	_RASTER_SET_VLINE_FAST_20:
		;//////////////////////////////
		;//
		;// COMPUTE BIT MASK
		;//
		;// r4=bit mask 
		;//////////////////////////////
		ldiw	Z,_RASTER_TABLE_MASK_1_BIT*2			;Z-->START table
		mov	Acc,XL						;get bit
		andi	Acc,7						;mask bits
		add	ZL,Acc						;compute START mask
		ldi	Acc,0
		adc	ZH,Acc
		lpm	r4,Z						;get START mask
		;//////////////////////////////
		;//
		;// COMPUTE START POSITION(1 bit)
		;//
		;// m<-(yi*width_bytes)+xl/8
		;//////////////////////////////
		ldsw	Z,_RASTER_START_ADDR				;Z-->raster pixels
		lds	Acc,_RASTER_WIDTH_BYTES				;get raster width bytes
		mul	Acc,YL						;(yi*width_bytes)
		add	ZL,r0
		adc	ZH,r1
		mov	Acc,XL						;xi/8
		lsr	Acc
		lsr	Acc
		lsr	Acc
		add	ZL,Acc						;Z-->(yi*width_bytes)+xi/8
		ldi	Acc,0
		adc	ZH,Acc
		;//////////////////////////////
		;//
		;// CREATE COLOR MASK
		;//
		;//////////////////////////////
		andi	Temp,1						;prepare color 
		sbrc	Temp,0						;skip if zero
		ori	Temp,0xfe					;else set all bits
		;//////////////////////////////
		;//
		;// MAIN LOOP
		;//
		;// for Acc=yi to yf
		;//////////////////////////////
		lds	r1,_RASTER_WIDTH_BYTES				;to compute next line
		clr	r2
		mov	r3,Temp						;get color
		and	r3,r4						;mask color
		com	r4						;mask and not
		mov	Acc,YL						;acc=YI
		inc	YH						;to ease compare
	_RASTER_SET_VLINE_FAST_30:
		ld	r0,Z						;get actual color
		and	r0,r4						;bound color
		or	r0,r3
		st	Z,r0						;store new color
		add	ZL,r1						;Z-->next row
		adc	ZH,r2
		inc	Acc
		cp	Acc,YH						;last row ?
		brlo	_RASTER_SET_VLINE_FAST_30			;no, loop until done all
	_RASTER_SET_VLINE_FAST_EXIT:					;program exit
		popw	Z						;restore useds
		popw	Y
		popaw
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_BOX_FAST				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER draw box with Fast method		|
	;>--------------------------------------------------------------<
	;| INPUT      : XL,YL 	   Start coordinate			|
	;|		XH	   Width				|
	;|		YH	   Height				|
	;|		Temp       Color				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,r2,r4,flags				|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	This routine not allow negative coordinates	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_BOX_FAST:
		rcall	_RASTER_SET_HLINE_FAST				;draw up side
		push	YL
		add	YL,YH						;compute yf
		subi	YL,1
		rcall	_RASTER_SET_HLINE_FAST				;draw down side
		pop	YL
		rcall	_RASTER_SET_VLINE_FAST				;draw left side
		push	XL
		add	XL,XH						;compute xf
		subi	XL,1
		rcall	_RASTER_SET_VLINE_FAST				;draw right side
		pop	XL
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_PIXEL_FAST				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER draw a pixel with Fast method		|
	;>--------------------------------------------------------------<
	;| INPUT      : XL,YL 	   Start coordinate			|
	;|		Temp       Color				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,r2,flag	s				|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	This routine not allow negative coordinates	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_PIXEL_FAST:
		push	Acc
		pushw	Z
		lds	Acc,_RASTER_WIDTH				;x>=width ?
		cp	XL,Acc
		brsh	_RASTER_SET_PIXEL_FAST_EXIT			;yex, exit
		lds	Acc,_RASTER_HEIGHT				;y>=height ?
		cp	YL,Acc
		brsh	_RASTER_SET_PIXEL_FAST_EXIT			;yex, exit
		;//////////////////////////////
		;//
		;// COMPUTE BIT MASK
		;//
		;// r2=bit mask 
		;//////////////////////////////
		ldiw	Z,_RASTER_TABLE_MASK_1_BIT*2			;Z-->START table
		mov	Acc,XL						;get bit
		andi	Acc,7						;mask bits
		add	ZL,Acc						;compute START mask
		ldi	Acc,0
		adc	ZH,Acc
		lpm	r2,Z						;get START mask
		;//////////////////////////////
		;//
		;// COMPUTE START POSITION(1 bit)
		;//
		;// m<-(yi*width_bytes)+xl/8
		;//////////////////////////////
		ldsw	Z,_RASTER_START_ADDR				;Z-->raster pixels
		lds	Acc,_RASTER_WIDTH_BYTES				;get raster width bytes
		mul	Acc,YL						;(yi*width_bytes)
		add	ZL,r0
		adc	ZH,r1
		mov	Acc,XL						;xi/8
		lsr	Acc
		lsr	Acc
		lsr	Acc
		add	ZL,Acc						;Z-->(yi*width_bytes)+xi/8
		ldi	Acc,0
		adc	ZH,Acc
		;//////////////////////////////
		;//
		;// CREATE COLOR MASK
		;//
		;//////////////////////////////
		andi	Temp,1						;prepare color 
		sbrc	Temp,0						;skip if zero
		ori	Temp,0xfe					;else set all bits
		;//////////////////////////////
		;//
		;// SET A PIXEL
		;//
		;//////////////////////////////
		ld	r0,Z						;get actual color
		mov	r1,Temp						;get color to be set
		and	r1,r2						;mask color
		com	r2
		and	r0,r2						;mask source color
		or	r0,r1						;bound colors
		st	Z,r0
	_RASTER_SET_PIXEL_FAST_EXIT:
		popw	Z
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_BMP_FAST				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER draw a BMP into RASTER pixels		|
	;>--------------------------------------------------------------<
	;| INPUT      : X,Y 	   Start coordinate			|
	;|		Z-->	   BMP into Flash			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0..r11,flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	This routine not allow negative coordinates	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|
	;| BMP formatting into FLASH memory
	;|
	;| BMP_IMAGE:
	;|		.dw	BIT_SIZE	;BMP number of bits per pixel 
	;|		.dw	WIDTH		;BMP width in pixels
	;|		.dw	HEIGHT		;BMP height in pixels
	;|		.dw	WIDTH_BYTES	;BMP width in bytes
	;|		.dw	SIZE		;BMP size in bytes
	;|		.db	0xnn,0xnn,...	;BMP pixels data
	;|
	;|
	.EQU	_RASTER_BMP_OFFSET_BIT_SIZE	=0
	.EQU	_RASTER_BMP_OFFSET_WIDTH	=2
	.EQU	_RASTER_BMP_OFFSET_HEIGHT	=4
	.EQU	_RASTER_BMP_OFFSET_WIDTH_BYTES	=6
	.EQU	_RASTER_BMP_OFFSET_SIZE		=8
	.EQU	_RASTER_BMP_OFFSET_DATA		=10
	_RASTER_SET_BMP_FAST:
		pushaw
		pushawt
		pushw	X
		pushw	Y
		pushw	Z
		;//////////////////////////////////
		;//
		;// check if in range
		;// x>=0 x<width
		;// y>=0 y<height
		;//
		;//////////////////////////////////
		tst	XH					;X<0?
		lbrmi	_RASTER_SET_BMP_FAST_EXIT		;yes, exit
		tst	YH					;Y<0?
		lbrmi	_RASTER_SET_BMP_FAST_EXIT		;yex, exit
		lds	Acc,_RASTER_WIDTH			;X>=width ?
		cp	XL,Acc
		ldi	Acc,0
		cpc	XH,Acc
		lbrge	_RASTER_SET_BMP_FAST_EXIT		;yes, exit
		lds	Acc,_RASTER_HEIGHT			;Y>=height ?
		cp	YL,Acc
		ldi	Acc,0
		cpc	YH,Acc
		lbrge	_RASTER_SET_BMP_FAST_EXIT		;yes, exit
		;//////////////////////////////////
		;//
		;// Output
		;// XL=xi YL=yi XH=xf YH=yf
		;//
		;//////////////////////////////////
		pushw	Z					;save bmp pointer
		addiw	Z,_RASTER_BMP_OFFSET_WIDTH		;Z-->BMP width
		lpm	Acc,Z+					;get width
		lpm	AccH,Z+
		add	Acc,XL					;xf=width-1
		adc	AccH,XH
		subiaw	1
		lds	AccT,_RASTER_WIDTH			;xf>=width ?
		cp	Acc,AccT
		ldi	AccT,0
		cpc	AccH,AccT
		brlo	_RASTER_SET_BMP_FAST_10			;no, branch
		lds	Acc,_RASTER_WIDTH			;else xf=width-1
		dec	Acc
	_RASTER_SET_BMP_FAST_10:
		mov	XH,Acc					;XH=xf
		popw	Z					;restore bmp pointer
		pushw	Z					;save again
		addiw	Z,_RASTER_BMP_OFFSET_HEIGHT		;Z-->BMP height
		lpm	Acc,Z+
		lpm	AccH,Z+
		add	Acc,YL					;yf=height-1
		adc	AccH,YH
		subiaw	1
		lds	AccT,_RASTER_HEIGHT			;yf>=height ?
		cp	Acc,AccT
		ldi	AccT,0
		cpc	AccH,AccT
		brlo	_RASTER_SET_BMP_FAST_20			;no, branch
		lds	Acc,_RASTER_HEIGHT			;else yf=height-1
		dec	Acc
	_RASTER_SET_BMP_FAST_20:
		mov	YH,Acc					;YH=yf
		popw	Z
		lpm	Acc,Z					;get bit size
		cpi	Acc,1					;1 bit size ?
		lbrne	_RASTER_SET_BMP_FAST_CHECK_2_BIT	;no, exit
		;/////////////////////////////////
		;//				//
		;// 	  1(ONE) bit BMP	//
		;//				//
		;///////////////////////////////////////////////////////////////////////////////
		;//////////////////////////////////
		;//
		;// Output
		;// r2	 =mask start
		;// r3	 =mask end
		;// r5:r4=BMP width bytes
		;//
		;//////////////////////////////////
		pushw	Z
		addiw	Z,_RASTER_BMP_OFFSET_WIDTH_BYTES
		lpm	r4,Z+						;r5:r5<-BMP.width_bytes
		lpm	r5,Z
		ldiw	Z,_RASTER_SET_HLINE_FAST_TABLE_START*2		;Z-->START table
		mov	Acc,XL						;get bit
		andi	Acc,7						;mask bits
		add	ZL,Acc						;compute START mask
		ldi	Acc,0
		adc	ZH,Acc
		lpm	r2,Z						;get START mask
		ldiw	Z,_RASTER_SET_HLINE_FAST_TABLE_END*2		;Z-->END table
		mov	Acc,XH						;get bit
		andi	Acc,7						;mask bits
		add	ZL,Acc						;compute END mask
		ldi	Acc,0
		adc	ZH,Acc
		lpm	r3,Z						;get END mask
		;//////////////////////////////
		;//
		;// COMPUTE START POSITION(1 bit)
		;//
		;// m<-(yi*width_bytes)+xl/8
		;//
		;//////////////////////////////
		ldsw	Z,_RASTER_START_ADDR				;Z-->raster pixels
		lds	Acc,_RASTER_WIDTH_BYTES				;get raster width bytes
		mul	Acc,YL						;(yi*width_bytes)
		add	ZL,r0
		adc	ZH,r1
		mov	Acc,XL						;xi/8
		lsr	Acc
		lsr	Acc
		lsr	Acc
		add	ZL,Acc						;Z-->(yi*width_bytes)+xi/8
		ldi	Acc,0
		adc	ZH,Acc
		;//////////////////////////////
		;//
		;// COMPUTE XI,XF IN BYTES
		;//
		;// Xi_bytes=Xi/8 Xf_bytes=Xf/8
		;// r6=xi and 7
		;// r7=xf and 7
		;//////////////////////////////
		mov	Acc,XL						;r6=xi and 7
		andi	Acc,7
		mov	r6,Acc
		mov	Acc,XH						;r7=xf and 7
		andi	Acc,7
		mov	r7,Acc
		lsr	XL						;xi/8
		lsr	XL
		lsr	XL
		lsr	XH						;xf/8
		lsr	XH
		lsr	XH
		;//////////////////////////////
		;//
		;// for Acc=xi to xf
		;//
		;// AccT =rows
		;// Y---> pixel(x,y)
		;// Z---> BMP.Data
		;// r1:r0=SCREEN.width_bytes
		;//////////////////////////////
		lds	r0,_RASTER_WIDTH_BYTES				;r0=width_bytes
		clr	r1
		mov	AccT,YH						;rows=yf-yi+1
		sub	AccT,YL
		inc	AccT
		movw	YL,ZL						;Y-->pixel(x,y)
		popw	Z						;Z-->BMP
		addiw	Z,_RASTER_BMP_OFFSET_DATA			;Z-->BMP.Data
	_RASTER_SET_BMP_FAST_30:
		mov	Acc,XL						;Acc=xi
		pushw	Y						;save Y
		pushw	Z
	_RASTER_SET_BMP_FAST_40:
		cp	XL,XH						;xi=xf?
		brne	_RASTER_SET_BMP_FAST_50				;no, branch
		;////////////////////////////////
		;// xi=xf
		;// r8 =data
		;// r10=(xi mask) or (xf mask)
		;//
		;////////////////////////////////
		lpm	r8,Z+						;get BMP data
		mov	r10,r2						;get mask start
		or	r10,r3						;or mask end
		mov	AccH,r6						;AccH=xi and 7
	_RASTER_SET_BMP_FAST_40A:
		tst	AccH						;shifted ?
		breq	_RASTER_SET_BMP_FAST_40B			;yes, branch
		lsr	r8						;shift data
		dec	AccH
		rjmp	_RASTER_SET_BMP_FAST_40A			;until all data shifted
	_RASTER_SET_BMP_FAST_40B:
		ld	AccH,Y						;get old data
		and	AccH,r10					;mask lower data
		or	AccH,r8						;add BMP data
		st	Y+,AccH						;store back
		rjmp	_RASTER_SET_BMP_FAST_NEXT			;next data
	_RASTER_SET_BMP_FAST_50:
		cp	Acc,XL						;Acc=xi?
		brne	_RASTER_SET_BMP_FAST_60				;no, branch
		;////////////////////////////////
		;// x=xi
		;// r9:r8 =data
		;//
		;////////////////////////////////
		lpm	r8,Z+						;get BMP data
		lpm	r9,Z
		mov	AccH,r6						;AccH=xi and 7
	_RASTER_SET_BMP_FAST_50A:
		tst	AccH						;shifted ?
		breq	_RASTER_SET_BMP_FAST_50B			;yes, branch
		lsr	r8						;shift data
		ror	r9
		dec	AccH
		rjmp	_RASTER_SET_BMP_FAST_50A			;until all data shifted
	_RASTER_SET_BMP_FAST_50B:
		ld	AccH,Y						;get old data
		and	AccH,r2						;mask lower data
		or	AccH,r8						;add BMP data
		st	Y+,AccH						;store back
		rjmp	_RASTER_SET_BMP_FAST_NEXT			;next data
	_RASTER_SET_BMP_FAST_60:
		cp	Acc,XH						;Acc=xf?
		brne	_RASTER_SET_BMP_FAST_70
		;////////////////////////////////
		;// x=xf
		;// r9=last shifted data
		;//
		;////////////////////////////////
		ld	AccH,Y						;get old data
		and	AccH,r3						;mask high data
		com	r3
		and	r9,r3
		com	r3
		or	AccH,r9						;add BMP data
		st	Y,AccH						;store back
		rjmp	_RASTER_SET_BMP_FAST_NEXT			;next data
	_RASTER_SET_BMP_FAST_70:
		;////////////////////////////////
		;// x<>xi and x<>xf
		;// r9:r8 =data
		;//
		;////////////////////////////////
		ld	AccH,Y						;get old data
		com	r2
		and	AccH,r2						;mask high data
		com	r2
		and	r9,r2
		or	AccH,r9						;add BMP data
		st	Y,AccH						;store back
		lpm	r8,Z+						;get BMP data
		lpm	r9,Z
		mov	AccH,r6						;AccH=xi and 7
	_RASTER_SET_BMP_FAST_70A:
		tst	AccH						;shifted ?
		breq	_RASTER_SET_BMP_FAST_70B			;yes, branch
		lsr	r8						;shift data
		ror	r9
		dec	AccH
		rjmp	_RASTER_SET_BMP_FAST_70A			;until all data shifted
	_RASTER_SET_BMP_FAST_70B:
		ld	AccH,Y						;get old data
		and	AccH,r2						;mask lower data
		com	r2
		and	r8,r2
		com	r2
		or	AccH,r8						;add BMP data
		st	Y+,AccH						;store back
	_RASTER_SET_BMP_FAST_NEXT:
		inc	Acc						;x++
		mov	AccH,XH
		inc	AccH
		cp	Acc,AccH					;x>xf?
		lbrlo	_RASTER_SET_BMP_FAST_40				;no, wait reach
		popw	Z
		popw	Y						;restore Y
		add	ZL,r4						;Z+=BMP.width_bytes
		adc	ZH,r5
		add	YL,r0						;Y+=SCREEN.width_bytes
		adc	YH,r1
		dec	AccT						;row--
		lbrne	_RASTER_SET_BMP_FAST_30				;until all compute rows done
		rjmp	_RASTER_SET_BMP_FAST_EXIT
	_RASTER_SET_BMP_FAST_CHECK_2_BIT:
		cpi	Acc,2						;2 bit size ?
		lbrne	_RASTER_SET_BMP_FAST_CHECK_4_BIT		;no, exit
		;/////////////////////////////////
		;//				//
		;// 	  2(TWO) bit BMP	//
		;//				//
		;///////////////////////////////////////////////////////////////////////////////
		rjmp	_RASTER_SET_BMP_FAST_EXIT
	_RASTER_SET_BMP_FAST_CHECK_4_BIT:
		cpi	Acc,4						;4 bit size ?
		lbrne	_RASTER_SET_BMP_FAST_CHECK_8_BIT		;no, exit
		;/////////////////////////////////
		;//				//
		;// 	  4(FOUR) bit BMP	//
		;//				//
		;///////////////////////////////////////////////////////////////////////////////
		;//////////////////////////////////
		;//
		;// Output
		;// r2	 =mask start
		;// r3	 =mask end
		;// r5:r4=BMP width bytes
		;//
		;//////////////////////////////////
		pushw	Z
		addiw	Z,_RASTER_BMP_OFFSET_WIDTH_BYTES
		lpm	r4,Z+						;r5:r5<-BMP.width_bytes
		lpm	r5,Z
		ldi	Acc,0x00					;assume upper nibble
		sbrc	XL,0						;skip if lower
		ldi	Acc,0xf0					;else lower nibble
		mov	r2,Acc						;r2=mask start
		ldi	Acc,0x0f					;assume upper nibble
		sbrc	XH,0						;skip if lower
		ldi	Acc,0x00					;else lower nibble
		mov	r3,Acc						;r3=mask end			
		;//////////////////////////////
		;//
		;// COMPUTE START POSITION(4 bit)
		;//
		;// m<-(yi*width_bytes)+xl/2
		;//
		;//////////////////////////////
		ldsw	Z,_RASTER_START_ADDR				;Z-->raster pixels
		lds	Acc,_RASTER_WIDTH_BYTES				;get raster width bytes
		mul	Acc,YL						;(yi*width_bytes)
		add	ZL,r0
		adc	ZH,r1
		mov	Acc,XL						;xi/2
		lsr	Acc
		add	ZL,Acc						;Z-->(yi*width_bytes)+xi/2
		ldi	Acc,0
		adc	ZH,Acc
		;//////////////////////////////
		;//
		;// COMPUTE XI,XF IN BYTES
		;//
		;// Xi_bytes=Xi/2 Xf_bytes=Xf/2
		;// r6=xi and 1
		;// r7=xf and 1
		;//////////////////////////////
		mov	Acc,XL						;r6=xi and 1
		andi	Acc,1
		mov	r6,Acc
		mov	Acc,XH						;r7=xf and 1
		andi	Acc,1
		mov	r7,Acc
		lsr	XL						;xi/2
		lsr	XH						;xf/2
		;//////////////////////////////
		;//
		;// for Acc=xi to xf
		;//
		;// AccT =rows
		;// Y---> pixel(x,y)
		;// Z---> BMP.Data
		;// r1:r0=SCREEN.width_bytes
		;//////////////////////////////
		lds	r0,_RASTER_WIDTH_BYTES				;r0=width_bytes
		clr	r1
		mov	AccT,YH						;rows=yf-yi+1
		sub	AccT,YL
		inc	AccT
		movw	YL,ZL						;Y-->pixel(x,y)
		popw	Z						;Z-->BMP
		addiw	Z,_RASTER_BMP_OFFSET_DATA			;Z-->BMP.Data
	_RASTER_SET_BMP_FAST_4_30:
		mov	Acc,XL						;Acc=xi
		pushw	Y						;save Y
		pushw	Z
	_RASTER_SET_BMP_FAST_4_40:
		cp	XL,XH						;xi=xf?
		brne	_RASTER_SET_BMP_FAST_4_50			;no, branch
		;////////////////////////////////
		;// xi=xf
		;// r8 =data
		;// r10=(xi mask) or (xf mask)
		;//
		;////////////////////////////////
		lpm	r8,Z+						;get BMP data
		mov	r10,r2						;get mask start
		or	r10,r3						;or mask end
		mov	AccH,r6						;AccH=xi and 1
	_RASTER_SET_BMP_FAST_4_40A:
		tst	AccH						;shifted ?
		breq	_RASTER_SET_BMP_FAST_4_40B			;yes, branch
		lsr	r8
		lsr	r8
		lsr	r8
		lsr	r8						;shift data
		dec	AccH
		rjmp	_RASTER_SET_BMP_FAST_4_40A			;until all data shifted
	_RASTER_SET_BMP_FAST_4_40B:
		ld	AccH,Y						;get old data
		and	AccH,r10					;mask lower data
		or	AccH,r8						;add BMP data
		st	Y+,AccH						;store back
		rjmp	_RASTER_SET_BMP_FAST_4_NEXT			;next data
	_RASTER_SET_BMP_FAST_4_50:
		cp	Acc,XL						;Acc=xi?
		brne	_RASTER_SET_BMP_FAST_4_60			;no, branch
		;////////////////////////////////
		;// x=xi
		;// r9:r8 =data
		;//
		;////////////////////////////////
		lpm	r8,Z+						;get BMP data
		lpm	r9,Z
		mov	AccH,r6						;AccH=xi and 1
	_RASTER_SET_BMP_FAST_4_50A:
		tst	AccH						;shifted ?
		breq	_RASTER_SET_BMP_FAST_4_50B			;yes, branch
		lsr	r8						;shift data
		ror	r9
		lsr	r8						;shift data
		ror	r9
		lsr	r8						;shift data
		ror	r9
		lsr	r8						;shift data
		ror	r9
		dec	AccH
		rjmp	_RASTER_SET_BMP_FAST_4_50A			;until all data shifted
	_RASTER_SET_BMP_FAST_4_50B:
		ld	AccH,Y						;get old data
		and	AccH,r2						;mask lower data
		or	AccH,r8						;add BMP data
		st	Y+,AccH						;store back
		rjmp	_RASTER_SET_BMP_FAST_4_NEXT			;next data
	_RASTER_SET_BMP_FAST_4_60:
		cp	Acc,XH						;Acc=xf?
		brne	_RASTER_SET_BMP_FAST_4_70
		;////////////////////////////////
		;// x=xf
		;// r9=last shifted data
		;//
		;////////////////////////////////
		ld	AccH,Y						;get old data
		and	AccH,r3						;mask high data
		com	r3
		and	r9,r3
		com	r3
		or	AccH,r9						;add BMP data
		st	Y,AccH						;store back
		rjmp	_RASTER_SET_BMP_FAST_4_NEXT			;next data
	_RASTER_SET_BMP_FAST_4_70:
		;////////////////////////////////
		;// x<>xi and x<>xf
		;//
		;////////////////////////////////
		ld	AccH,Y						;get old data
		com	r2
		and	AccH,r2						;mask high data
		com	r2
		and	r9,r2
		or	AccH,r9						;add BMP data
		st	Y,AccH						;store back
		lpm	r8,Z+						;get BMP data
		lpm	r9,Z
		mov	AccH,r6						;AccH=xi and 1
	_RASTER_SET_BMP_FAST_4_70A:
		tst	AccH						;shifted ?
		breq	_RASTER_SET_BMP_FAST_4_70B			;yes, branch
		lsr	r8						;shift data
		ror	r9
		lsr	r8						;shift data
		ror	r9
		lsr	r8						;shift data
		ror	r9
		lsr	r8						;shift data
		ror	r9
		dec	AccH
		rjmp	_RASTER_SET_BMP_FAST_4_70A			;until all data shifted
	_RASTER_SET_BMP_FAST_4_70B:
		ld	AccH,Y						;get old data
		and	AccH,r2						;mask lower data
		com	r2
		and	r8,r2
		com	r2
		or	AccH,r8						;add BMP data
		st	Y+,AccH						;store back
	_RASTER_SET_BMP_FAST_4_NEXT:
		inc	Acc						;x++
		mov	AccH,XH
		inc	AccH
		cp	Acc,AccH					;x>xf?
		lbrlo	_RASTER_SET_BMP_FAST_4_40			;no, wait reach
		popw	Z
		popw	Y						;restore Y
		add	ZL,r4						;Z+=BMP.width_bytes
		adc	ZH,r5
		add	YL,r0						;Y+=SCREEN.width_bytes
		adc	YH,r1
		dec	AccT						;row--
		lbrne	_RASTER_SET_BMP_FAST_4_30			;until all compute rows done
		rjmp	_RASTER_SET_BMP_FAST_EXIT
	_RASTER_SET_BMP_FAST_CHECK_8_BIT:
		cpi	Acc,8						;8 bit size ?
		lbrne	_RASTER_SET_BMP_FAST_EXIT			;no, exit		
		;/////////////////////////////////
		;//				//
		;// 	  8(EIGHT) bit BMP	//
		;//				//
		;///////////////////////////////////////////////////////////////////////////////
		rjmp	_RASTER_SET_BMP_FAST_EXIT
	_RASTER_SET_BMP_FAST_EXIT:
		popw	Z
		popw	Y
		popw	X
		popawt
		popaw
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_GET_BMP_WIDTH				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER get width of BMP in pixels		|
	;>--------------------------------------------------------------<
	;| INPUT      : Z-->	   BMP into Flash			|
	;| OUTPUT     :	AccH:Acc   BMP width in pixels			|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_BMP_GET_WIDTH:
		pushw	Z
		addiw	Z,_RASTER_BMP_OFFSET_WIDTH		;Z-->BMP.width
		lpm	Acc,Z+
		lpm	AccH,Z+
		popw	Z
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_GET_BMP_HEIGHT				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER get height of BMP in pixels		|
	;>--------------------------------------------------------------<
	;| INPUT      : Z-->	   BMP into Flash			|
	;| OUTPUT     :	AccH:Acc   BMP height in pixels			|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_BMP_GET_HEIGHT:
		pushw	Z
		addiw	Z,_RASTER_BMP_OFFSET_HEIGHT		;Z-->BMP.width
		lpm	Acc,Z+
		lpm	AccH,Z+
		popw	Z
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_GET_BIT_XY_POS_AND_MASK 			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER get SRAM memory position to set bit and	|
	;|		and mask using coordinates X,Y			|
	;>--------------------------------------------------------------<
	;| INPUT      : Z-->	   Bit Area				|
	;|		XL,YL	   Bit Coordinate			|
	;|		XH,YH	   Width and Height of bit buffer	|	
	;| OUTPUT     :	Y-->	   Mem.Pos				|
	;|		Temp	   Bit.Mask				|
	;| DESTROYED  :	r0,r1,Z,Flags					|
	;| TIME	      :	28 clocks					|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	range value for x=0..255 and y=0..255		|
	;|		range value for Width=1..255 and Height=1..255	|
	;|		this routine is used internaly only		|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_GET_BIT_XY_POS_AND_MASK:
		;////////////////////////////////////
		;//         	         Y*Width+X
		;// Mem.Pos=bit.Buffer + ---------
		;//                          8
		;////////////////////////////////////
		ldi	Temp,0
		mul	YL,XH					;Y*Width
		add	r0,XL					;Y*Width+X
		adc	r1,Temp
		mov	Temp,r0					;Temp=bit mask number
		andi	Temp,7
		lsr	r1
		ror	r0
		lsr	r1
		ror	r0
		lsr	r1
		ror	r0
		movw	YL,ZL					;Y<--Z
		add	YL,r0					;Y-->Mem.Pos
		adc	YH,r1
		ldiw	Z,_RASTER_TABLE_MASK_XY_1_BIT*2		;Z-->bit mask
		add	ZL,Temp
		ldi	Temp,0
		adc	ZH,Temp
		lpm	Temp,Z
		ret
	;////////////////////////////////
	;// MASK TABLE FOR BIT SET/RESET
	;////////////////////////////////
	_RASTER_TABLE_MASK_XY_1_BIT:
		.DB	0X01,0X02,0X04,0X08,0X10,0X20,0X40,0X80
				
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_SET_BIT_XY	 			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER set bit directly into SRAM memory	|
	;|		using coordinates x,y 				|
	;>--------------------------------------------------------------<
	;| INPUT      : Z-->	   Bit Area				|
	;|		XL,YL	   Bit Coordinate			|
	;|		XH,YH	   Width and Height of bit buffer	|	
	;|		Acc	   Bit to be set 0 or 1			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	Min=42 clocks					|
	;|		Max=79 clocks					|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	range value for x=0..255 and y=0..255		|
	;|		range value for Width=1..255 and Height=1..255	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_BIT_XY:
		push	r0
		push	r1
		push	Acc
		push	Temp
		pushw	Y
		pushw	Z
		cp	XL,XH					;valid value for X?
		brsh	_RASTER_SET_BIT_XY_EXIT			;no, branch
		cp	YL,YH					;valid value for Y?
		brsh	_RASTER_SET_BIT_XY_EXIT			;no, branch
		rcall	_RASTER_GET_BIT_XY_POS_AND_MASK		;compute position and mask
		andi	Acc,1					;clear bits 1..7
		sbrc	Acc,0					;skip if bit=0 clear
		ori	Acc,0xFE				;else set bits 1..7
		and	Acc,Temp				;mask bit to be set
		com	Temp
		ld	r0,Y					;get old bits
		and	r0,Temp					;mask bit that not be set
		or	r0,Acc					;bound new bits
		st	Y,r0
	_RASTER_SET_BIT_XY_EXIT:
		popw	Z
		popw	Y
		pop	Temp
		pop	Acc
		pop	r1
		pop	r0
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_GET_BIT_XY	 			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RASTER get bit directly from SRAM memory	|
	;|		using coordinates x,y 				|
	;>--------------------------------------------------------------<
	;| INPUT      : Z-->	   Bit Area				|
	;|		XL,YL	   Bit Coordinate			|
	;|		XH,YH	   Width and Height of bit buffer	|	
	;| OUTPUT     :	Acc	   _ON=1,_OFF=0				|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	Min=35 clocks					|
	;|		Max=72 clocks					|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	range value for x=0..255 and y=0..255		|
	;|		range value for Width=1..255 and Height=1..255	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_GET_BIT_XY:
		push	r0
		push	r1
		push	Temp
		pushw	Y
		pushw	Z
		ldi	Acc,_OFF				;assume bit clear
		cp	XL,XH					;valid value for X?
		brsh	_RASTER_GET_BIT_XY_EXIT			;no, branch
		cp	YL,YH					;valid value for Y?
		brsh	_RASTER_GET_BIT_XY_EXIT			;no, branch
		rcall	_RASTER_GET_BIT_XY_POS_AND_MASK		;compute position and mask
		ldi	Acc,_OFF				;assume off
		ld	r0,Y					;get bit
		and	r0,Temp					;mask bit
		breq	_RASTER_GET_BIT_XY_EXIT			;branch if zero
		ldi	Acc,_ON					;else assume ON
	_RASTER_GET_BIT_XY_EXIT:
		popw	Z
		popw	Y
		pop	Temp
		pop	r1
		pop	r0
		ret
#endif

