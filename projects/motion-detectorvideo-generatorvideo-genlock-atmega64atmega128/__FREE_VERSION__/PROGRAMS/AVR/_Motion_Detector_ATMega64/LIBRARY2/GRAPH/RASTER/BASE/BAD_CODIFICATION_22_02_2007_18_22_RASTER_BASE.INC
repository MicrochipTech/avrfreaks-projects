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
;+--------------------------------------------------------------+---+
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA			|
;| DATA:4/12/2005					|
;>------------------------------------------------------<
;| A V R  I P  P R O T O C O L  I P V 4  		|
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
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _SLIP_SEND		SEND DATA AT TX_BUFFER							|
;|												|
;|			RCALL	_SLIP_SEND		;send data into tx_buffer		|
;+----------------------------------------------------------------------------------------------+
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
.EQU	_RASTER_FLAG_BIT_RANGE	= 0X01				;and mask to field _RASTER_FLAGS
;-------------------------< SRAM VARIABLES >-----------------------------
.DSEG
	_RASTER_WIDTH:		.BYTE	_BYTE			;width of raster in pixels
	_RASTER_WIDTH_BYTES:	.BYTE	_BYTE			;width of raster in bytes
	_RASTER_HEIGHT:		.BYTE	_BYTE			;height of raster in pixels
	_RASTER_START_ADDR:	.BYTE	_WORD			;start address of raster
	_RASTER_PIXEL_ADDR:	.BYTE	_WORD			;address to set/reset pixels
	_RASTER_FLAGS:		.BYTE	_BYTE			;bit 0=1 pixel(X,Y) out of range
	_RASTER_X:		.BYTE	_WORD			;x coordinate
	_RASTER_Y:		.BYTE	_WORD			;y coordinate
	_RASTER_BIT_SIZE:	.BYTE	_BYTE			;size of bmp
	_RASTER_BIT_MASK:	.BYTE	_BYTE			;holding a and mask to set and reset a bit
.CSEG
;
;-------------------------< M A C R O S  >-------------------------------
;
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
	;| FUNCTION   : RASTER_SET_PIXEL_ADDR				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Compute initial address of pixel to future set	|
	;|		or reset					|
	;>----------------------------------------------------------------------<
	;| INPUT      :	X,Y			pixel coordinate		|
	;| OUTPUT     :	_RASTER_PIXEL_ADDR 	Address of pixel to be set/reset|
	;|		_RASTER_X,_RASTER_Y	Coordinates in pixels		|
	;|		_RASTER_BIT_MASK	and mask of actual bit position	|
	;|		_RASTER_FLAGS		pixel validating status		|
	;| DESTROYED  :	r0,r1,r2,r4,Flags					|
	;| TIME	      :	----							|
	;>----------------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_PIXEL_ADDR:
		push	Acc				;save useds
		;//////////////////////////////////////////////
		;// set _RASTER_FLAGS_FLAG_BIT_RANGE<1 if not int range
		;//
		;// if X<0 or X>=width then flag=1
		;// if Y<0 or Y>=height then flag=1
		;//
		;//////////////////////////////////////////////		
		pushaw
		pushawt
		stsw	_RASTER_X,X			;store X
		stsw	_RASTER_Y,Y			;store Y
		lds	AccT,_RASTER_FLAGS		;assume not in range
		ori	AccT,_RASTER_FLAG_BIT_RANGE	;set flag
		tst	XH				;X<0 ?
		brlt	_RASTER_SET_PIXEL_ADDR_15	;yes, branch
		tst	YH				;Y<0 ?
		brlt	_RASTER_SET_PIXEL_ADDR_15	;yes, branch
		lds	Acc,_RASTER_WIDTH		;get width
		ldi	AccH,0
		cp	XL,Acc				;X>=width ?
		cpc	XH,AccH
		brge	_RASTER_SET_PIXEL_ADDR_15	;yes, ranch
		lds	Acc,_RASTER_HEIGHT		;get height
		cp	YL,Acc				;Y>=height ?
		cpc	YH,AccH
		brge	_RASTER_SET_PIXEL_ADDR_15 
		andi	AccT,~(_RASTER_FLAG_BIT_RANGE)	;set in range
	_RASTER_SET_PIXEL_ADDR_10:
		sts	_RASTER_FLAGS,AccT		;set flag range
		rjmp	_RASTER_SET_PIXEL_ADDR_16
	_RASTER_SET_PIXEL_ADDR_15:
		sts	_RASTER_FLAGS,AccT		;set flag range
		rjmp	_RASTER_SET_PIXEL_ADDR_EXIT	;then exit
	_RASTER_SET_PIXEL_ADDR_16:
		;//////////////////////////////////////////////
		;// COMPUTE PIXEL_START_ADDRESS by Y value
		;//
		;// ADDR(r2:r1:r0)=(start addr)+((signed)Y*(unsigned)Width Bytes)+X/2^(3-log2(bitsize))
		;//
		;//////////////////////////////////////////////		
		lds	AccT,_RASTER_WIDTH_BYTES	;get width in bytes
		mov	AccTH,YH
		mulsu	AccTH,AccT			;multiply high part signed
		movw	r2,r0				;r3:r2=(YH*Acc)
		mul	YL,AccT				;multiply low part unsigned
		add	r1,r2				;add acoording
		ldi	AccT,0
		adc	AccT,r3
		mov	r2,AccT				;put in correcty position
		ldsawt	_RASTER_START_ADDR		;add ADDR
		add	r0,AccT
		adc	r1,AccTH
		;//////////////////////////////////////////////
		;// COMPUTE 2^(3-log2(bitsize))
		;//////////////////////////////////////////////
		movw	AccT,X				;AccTH:AccT=X
		lds	Acc,_RASTER_BIT_SIZE		;get bit size
	_RASTER_SET_PIXEL_ADDR_20:
		sbrc	Acc,3				;log computed ?
		rjmp	_RASTER_SET_PIXEL_ADDR_30	;yes, branch
		asr	AccTH				;x=x/2 (signed)
		ror	AccT
		lsl	Acc
		rjmp	_RASTER_SET_PIXEL_ADDR_20
	_RASTER_SET_PIXEL_ADDR_30:
		add	r0,AccT				;ADDR+=X/2^(3-log2(bitsize))
		adc	r1,AccTH
		sts	_RASTER_PIXEL_ADDR+0,r0		;set PIXEL_ADDR
		sts	_RASTER_PIXEL_ADDR+1,r1
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
		;// X = 0  	11110000
		;//
		;//////////////////////////////////////////////
		lds	Acc,_RASTER_BIT_SIZE		;get bit size
		cpi	Acc,1				;1 bit ?
		breq	_RASTER_SET_PIXEL_ADDR_1_BIT	;yes, branch
		cpi	Acc,2				;2 bit ?
		breq	_RASTER_SET_PIXEL_ADDR_2_BIT	;yes, branch
		cpi	Acc,4				;4 bit ?
		breq	_RASTER_SET_PIXEL_ADDR_4_BIT	;yes, branch
		;////////////////////
		;// 8 BIT MASK
		;////////////////////
		ldi	Acc,0xff			;if none of above then assume 8 bit mask
		sts	_RASTER_BIT_MASK,Acc
		rjmp	_RASTER_SET_PIXEL_ADDR_EXIT	;exit
		;////////////////////
		;// 1 BIT MASK
		;////////////////////
	_RASTER_SET_PIXEL_ADDR_1_BIT:
		pushw	Z
		ldiw	Z,_RASTER_TABLE_MASK_1_BIT*2	;z--> MASK BITS
		mov	Acc,XL				;get number of bit
		andi	Acc,7
		add	ZL,Acc
		ldi	Acc,0
		adc	ZH,Acc
		lpm	Acc,Z				;get mask
		sts	_RASTER_BIT_MASK,Acc
		popw	Z
		rjmp	_RASTER_SET_PIXEL_ADDR_EXIT	;exit
		;////////////////////
		;// 2 BIT MASK
		;////////////////////
	_RASTER_SET_PIXEL_ADDR_2_BIT:
		pushw	Z
		ldiw	Z,_RASTER_TABLE_MASK_2_BIT*2	;z--> MASK BITS
		mov	Acc,XL				;get number of bit
		andi	Acc,3
		add	ZL,Acc
		ldi	Acc,0
		adc	ZH,Acc
		lpm	Acc,Z				;get mask
		sts	_RASTER_BIT_MASK,Acc
		popw	Z
		rjmp	_RASTER_SET_PIXEL_ADDR_EXIT	;exit
		;////////////////////
		;// 4 BIT MASK
		;////////////////////
	_RASTER_SET_PIXEL_ADDR_4_BIT:
		ldi	Acc,0xf0			;assume bit 0=0
		sbrc	XL,0				;skip if really yes
		ldi	Acc,0x0f			;else bit 0=1
		sts	_RASTER_BIT_MASK,Acc	
	_RASTER_SET_PIXEL_ADDR_EXIT:
		popawt
		popaw
		ret
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
	;| FUNCTION   : RASTER_SET_PIXEL				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Set/Reset pixel at coordinates X,Y		|
	;>----------------------------------------------------------------------<
	;| INPUT      :	X,Y			Pixel coordinate		|
	;|		Acc			Pixel Color/Level		|
	;| OUTPUT     :	None							|
	;| DESTROYED  :	r0,r1,r2,r3,Flags					|
	;| TIME	      :	----							|
	;>----------------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_SET_PIXEL:
		call	_RASTER_SET_PIXEL_ADDR		;compute area to pixel
		call	_RASTER_UPDATE_PIXEL
		ret
		
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_UPDATE_PIXEL				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Set/Reset pixel at last set coordinates		|
	;>----------------------------------------------------------------------<
	;| INPUT      :	Acc			Pixel Color/Level		|
	;| OUTPUT     :	None							|
	;| DESTROYED  :	r0,r1,r2,r3,Flags					|
	;| TIME	      :	----							|
	;>----------------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_UPDATE_PIXEL:
		pushaw					;save useds
		pushawt
		pushw	Z
		lds	AccT,_RASTER_FLAGS		;get flags to see if in valid region
		andi	AccT,_RASTER_FLAG_BIT_RANGE	;valid ?
		brne	_RASTER_UPDATE_PIXEL_EXIT	;no, exit
		;/////////////////////////////////
		;// ADJUST COLOR INPUT
		;/////////////////////////////////
		lds	AccT,_RASTER_BIT_SIZE		;get bit size
		cpi	AccT,1				;1 bit ?
		breq	_RASTER_UPDATE_PIXEL_10		;yes, branch
		cpi	AccT,2				;2 bit ?
		breq	_RASTER_UPDATE_PIXEL_20		;yes, branch
		cpi	AccT,4				;4 bit ?
		breq	_RASTER_UPDATE_PIXEL_30		;yes, branch
		rjmp	_RASTER_UPDATE_PIXEL_40		;else assume 8 bits
		;////////////////////////////////
		;// ADJUST COLOR FOR 1 BIT
		;////////////////////////////////
	_RASTER_UPDATE_PIXEL_10:	
		andi	Acc,0x1				;clear bits 1..7
		sbrc	Acc,0				;skip if bit 0=0
		ori	Acc,0xfe			;else set bit 1..7
		rjmp	_RASTER_UPDATE_PIXEL_40		;process the rest
		;////////////////////////////////
		;// ADJUST COLOR FOR 2 BIT
		;////////////////////////////////
	_RASTER_UPDATE_PIXEL_20:
		andi	Acc,0x3				;clear bits 2..7
		sbrc	Acc,0				;skip if bit 0=0
		ori	Acc,0b01010100			;else set bits 2,4,6
		sbrc	Acc,1				;skip if bit 1=0
		ori	Acc,0b10101000			;else set bits 3,5,7
		rjmp	_RASTER_UPDATE_PIXEL_40		;process the rest
		;////////////////////////////////
		;// ADJUST COLOR FOR 4 BIT
		;////////////////////////////////
	_RASTER_UPDATE_PIXEL_30:
		mov	AccH,Acc			;copy bits 0.3-->4..7
		swap	AccH
		andi	AccH,0xf0
		andi	Acc,0x0f
		or	Acc,AccH
	_RASTER_UPDATE_PIXEL_40:
		lds	AccT,_RASTER_BIT_MASK		;AccT=BIT MASK
		and	Acc,AccT			;Acc=valid bits
		ldsw	Z,_RASTER_PIXEL_ADDR		;Z-->pixel address
		ld	AccH,Z				;get screen pixels
		com	AccT
		and	AccH,AccT			;mask bit to be set
		or	Acc,AccH			;bound pixels
		st	Z,Acc				;store back
	_RASTER_UPDATE_PIXEL_EXIT:
		popw	Z
		popawt
		popaw
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RASTER						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RASTER_PIXEL_TO_LEFT				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Advance one pixel to left(X++)			|
	;>----------------------------------------------------------------------<
	;| INPUT      : None							|
	;| OUTPUT     :	None							|
	;| DESTROYED  :	r0,r1,r2,r3,Flags					|
	;| TIME	      :	----							|
	;>----------------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_RASTER_PIXEL_TO_RIGHT:
		pushaw					;save useds
		pushw	X
		ldsw	X,_RASTER_X			;get X coord
		addiw	X,1				;X++
		stsw	_RASTER_X,X
		tst	XH				;X<0 ?
		brge	_RASTER_PIXEL_TO_RIGHT_00	;no, branch
		ldi	Acc,0x01			;set LEFT BORDER MASK
		rjmp	_RASTER_PIXEL_TO_RIGHT_05
	_RASTER_PIXEL_TO_RIGHT_00:
		lds	Acc,_RASTER_WIDTH		;X>=width
		ldi	AccH,0
		cp	XL,Acc
		cpc	XH,AccH
		brlt	_RASTER_PIXEL_TO_RIGHT_10	;no, branch
		ldi	Acc,0x80			;<---- GET BORDER MASK
	_RASTER_PIXEL_TO_RIGHT_05:
		sts	_RASTER_BIT_MASK,Acc
		lds	Acc,_RASTER_FLAGS		;get flags
		ori	Acc,_RASTER_FLAG_BIT_RANGE	;range=1
		sts	_RASTER_FLAGS,Acc
		rjmp	_RASTER_PIXEL_TO_RIGHT_EXIT	;exit
		;////////////////////////////////
		;// LEFT ACCORDING BIT SIZE
		;////////////////////////////////
	_RASTER_PIXEL_TO_RIGHT_10:
		lds	Acc,_RASTER_FLAGS		;range=0
		andi	Acc,~(_RASTER_FLAG_BIT_RANGE)
		sts	_RASTER_FLAGS,Acc
		lds	Acc,_RASTER_BIT_SIZE		;get bit size
		cpi	Acc,1				;1 bit ?
		breq	_RASTER_PIXEL_TO_RIGHT_1_BIT	;yes, branch
		cpi	Acc,2				;2 bit ?
		breq	_RASTER_PIXEL_TO_RIGHT_2_BIT	;yes, branch
		cpi	Acc,4				;4 bit ?
		breq	_RASTER_PIXEL_TO_RIGHT_4_BIT	;yes, branch
		rjmp	_RASTER_PIXEL_TO_RIGHT_8_BIT	;else assume 8 bits
		;////////////////////////////////
		;// PIXEL LEFT FOR 1 BIT
		;////////////////////////////////
	_RASTER_PIXEL_TO_RIGHT_1_BIT:
		lds	Acc,_RASTER_BIT_MASK		;get mask
		cpi	Acc,0x01			;reach right border ?
		brne	_RASTER_PIXEL_TO_RIGHT_1_BIT_10	;no, branch
		ldi	Acc,0X80			;else set left border
		sts	_RASTER_BIT_MASK,Acc
		ldsaw	_RASTER_PIXEL_ADDR		;PIXEL_ADDR++
		addiaw	1
		stsaw	_RASTER_PIXEL_ADDR
		rjmp	_RASTER_PIXEL_TO_RIGHT_EXIT	;exit
	_RASTER_PIXEL_TO_RIGHT_1_BIT_10:
		lsr	Acc				;MASK>>1
		sts	_RASTER_BIT_MASK,Acc
		rjmp	_RASTER_PIXEL_TO_RIGHT_EXIT	;exit
		;////////////////////////////////
		;// PIXEL LEFT FOR 2 BIT
		;////////////////////////////////
	_RASTER_PIXEL_TO_RIGHT_2_BIT:
		lds	Acc,_RASTER_BIT_MASK		;get mask
		cpi	Acc,0x03			;reach right border ?
		brne	_RASTER_PIXEL_TO_RIGHT_2_BIT_10	;no, branch
		ldi	Acc,0Xc0			;else set left border
		sts	_RASTER_BIT_MASK,Acc
		ldsaw	_RASTER_PIXEL_ADDR		;PIXEL_ADDR++
		addiaw	1
		stsaw	_RASTER_PIXEL_ADDR
		rjmp	_RASTER_PIXEL_TO_RIGHT_EXIT	;exit
	_RASTER_PIXEL_TO_RIGHT_2_BIT_10:
		lsr	Acc				;MASK>>2
		lsr	Acc
		sts	_RASTER_BIT_MASK,Acc
		rjmp	_RASTER_PIXEL_TO_RIGHT_EXIT	;exit		
		;////////////////////////////////
		;// PIXEL LEFT FOR 4 BIT
		;////////////////////////////////
	_RASTER_PIXEL_TO_RIGHT_4_BIT:
		lds	Acc,_RASTER_BIT_MASK		;get mask
		cpi	Acc,0x0f			;reach right border ?
		brne	_RASTER_PIXEL_TO_RIGHT_4_BIT_10	;no, branch
		ldi	Acc,0Xf0			;else set left border
		sts	_RASTER_BIT_MASK,Acc
		ldsaw	_RASTER_PIXEL_ADDR		;PIXEL_ADDR++
		addiaw	1
		stsaw	_RASTER_PIXEL_ADDR
		rjmp	_RASTER_PIXEL_TO_RIGHT_EXIT	;exit
	_RASTER_PIXEL_TO_RIGHT_4_BIT_10:
		ldi	Acc,0x0f			;mask=0x0f
		sts	_RASTER_BIT_MASK,Acc
		rjmp	_RASTER_PIXEL_TO_RIGHT_EXIT	;exit		
		;////////////////////////////////
		;// PIXEL LEFT FOR 8 BIT
		;////////////////////////////////
	_RASTER_PIXEL_TO_RIGHT_8_BIT:
		ldsaw	_RASTER_PIXEL_ADDR		;PIXEL_ADDR++
		addiaw	1
		stsaw	_RASTER_PIXEL_ADDR
	_RASTER_PIXEL_TO_RIGHT_EXIT:
		popw	X
		popaw
		ret
#endif

