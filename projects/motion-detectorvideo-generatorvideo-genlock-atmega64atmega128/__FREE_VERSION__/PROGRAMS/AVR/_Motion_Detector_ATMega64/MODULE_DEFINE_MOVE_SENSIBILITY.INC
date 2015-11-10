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
;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======> 	      M O D U L E  -  D E F I N E  M O V E M E N T  S E N S I B I L I T Y 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description: ----										|											|
;	 -----------------------------------------------------------------------------------------------

	.INCLUDE	"BMPS\BMP_SENSE_BAR_1.INC"				;sensor bar image
	.INCLUDE	"BMPS\BMP_SENSE_BAR_ARROW_1.INC"			;sensor bar arrow image

	_MODULE_SET_SENSIBILITY:
		call	_VIDEO_DIGITIZING_CLEAR_DIFFERENCE_BUFFER	;clear dif buffer
	_MODULE_SET_SENSIBILITY_LOOP:
		;//////////////////////////////
		;/// DRAW SENSIBILITY BAR
		;//////////////////////////////
		call	_VIDEO_OUT_SYNC_WITH_VERTICAL			;synchronize with vertical retrace
		call	_VIDEO_OUT_SYNC_WITH_VERTICAL			;synchronize with vertical retrace
		call	_VIDEO_OUT_SYNC_WITH_VERTICAL			;synchronize with vertical retrace
		call	_VIDEO_OUT_SYNC_WITH_VERTICAL			;synchronize with vertical retrace
		ldiw	Z,BMP_SENSE_BAR_1*2
		ldiw	X,99
		ldiw	Y,4
		call	_RASTER_SET_BMP_FAST
		;//////////////////////////////
		;/// DRAW SENSIBILITY ARROW 
		;//////////////////////////////
		pushw	X
		pushw	Y
		addiw	X,2						;to put int correctly position
		addiw	Y,1
		lds	Acc,_MENU_CONFIGURE_CAPTURE_MOVEMENT_SENSIBILITY;get sensibility value
		mov	r0,Acc
		ldi	Acc,60
		mov	r1,Acc
		ldi	Acc,100
		mov	r2,Acc
		call	_MULDIV_U8U8U8U8
		addiw	Y,60
		sub	YL,r0
		sbc	YH,r1
		ldiw	Z,BMP_SENSE_BAR_ARROW_1*2			;draw arrow
		call	_RASTER_SET_BMP_FAST
		ldi	Temp,_OFF
		addiw	Y,2
		addi	XL,9
		ldi	XH,11
		call	_RASTER_SET_HLINE_FAST
		popw	Y
		popw	X
		;//////////////////////////////
		;/// DRAW SENSIBILITY BAR
		;//////////////////////////////
		pushw	X
		pushw	Y
		addiw	X,18						;positione at number value field
		addiw	Y,3+61
		call	_VIDEO_DIGITIZING_GET_FRAME_DIFFERENCE_VALUE	;get actual dif max dot
		mov	r0,Acc
		ldi	Acc,61
		mov	r1,Acc
		ldi	Acc,255
		mov	r2,Acc
		call	_MULDIV_U8U8U8U8
		ldi	XH,3						;bar width
		sub	YL,r0						;bar start position
		mov	YH,r0						;bar height
		ldi	Temp,_OFF
		call	_RASTER_SET_BOX_FULL_FAST

		popw	Y
		popw	X

		;//////////////////////////////
		;/// DRAW SENSIBILITY VALUE
		;//////////////////////////////
		pushw	X
		pushw	Y
		addiw	X,4						;positione at number value field
		addiw	Y,70
		;convert to percentage value
		lds	Acc,_MENU_CONFIGURE_CAPTURE_MOVEMENT_SENSIBILITY;get sensibility value
		call	_UBTOSUS
		adiw	ZL,1						;only digits
		call	_RASTER_SET_TEXT_OUT_SRAM
		popw	Y
		popw	X
		
		;//////////////////////////////
		;/// READ KEYBOARD WITHOUT WAIT AND PROCESS
		;//////////////////////////////
		call	_KEYBOARD_READ_FLASH				;read keyboard flash
		cpi	Acc,_KEY_UP					;UP KEY ?
		breq	_MODULE_SET_SENSIBILITY_KEY_UP			;yes, branch
		cpi	Acc,_KEY_DOWN					;DOWN KEY ?
		breq	_MODULE_SET_SENSIBILITY_KEY_DOWN		;yes, branch
		cpi	Acc,_KEY_LEFT					;LEFT KEY ?
		breq	_MODULE_SET_SENSIBILITY_KEY_NONE		;yes, same as key none
		cpi	Acc,_KEY_RIGHT					;RIGHT KEY ?
		breq	_MODULE_SET_SENSIBILITY_KEY_NONE		;yes, same as key none
		cpi	Acc,_KEY_ENTER					;ENTER KEY ?
		breq	_MODULE_SET_SENSIBILITY_KEY_ENTER		;yes, same as key none
		;//////////////////////////////
		;// OTHER PROCESS WHEN NO KEY PRESSED
		;//////////////////////////////
	_MODULE_SET_SENSIBILITY_KEY_NONE:

		rjmp	_MODULE_SET_SENSIBILITY_LOOP			;loop AGAIN
		;//////////////////////////////
		;// PROCESS KEY UP
		;//////////////////////////////
	_MODULE_SET_SENSIBILITY_KEY_UP:
		lds	Acc,_MENU_CONFIGURE_CAPTURE_MOVEMENT_SENSIBILITY;get sensibility value
		cpi	Acc,100						;max value reach?
		breq	_MODULE_SET_SENSIBILITY_KEY_UP_10		;yes, branch
		inc	Acc						;else sensi++
	_MODULE_SET_SENSIBILITY_KEY_UP_10:
		sts	_MENU_CONFIGURE_CAPTURE_MOVEMENT_SENSIBILITY,Acc
		rjmp	_MODULE_SET_SENSIBILITY_LOOP			;loop AGAIN
		;//////////////////////////////
		;// PROCESS KEY DOWN
		;//////////////////////////////
	_MODULE_SET_SENSIBILITY_KEY_DOWN:
		lds	Acc,_MENU_CONFIGURE_CAPTURE_MOVEMENT_SENSIBILITY;get sensibility value
		cpi	Acc,0						;min value reach?
		breq	_MODULE_SET_SENSIBILITY_KEY_DOWN_10		;yes, branch
		dec	Acc						;else sensi++
	_MODULE_SET_SENSIBILITY_KEY_DOWN_10:
		sts	_MENU_CONFIGURE_CAPTURE_MOVEMENT_SENSIBILITY,Acc
		rjmp	_MODULE_SET_SENSIBILITY_LOOP			;loop AGAIN
		;//////////////////////////////
		;// PROCESS KEY LEFT
		;//////////////////////////////
	_MODULE_SET_SENSIBILITY_KEY_LEFT:
		rjmp	_MODULE_SET_SENSIBILITY_LOOP			;loop AGAIN
		;//////////////////////////////
		;// PROCESS KEY RIGHT
		;//////////////////////////////
	_MODULE_SET_SENSIBILITY_KEY_RIGHT:
		rjmp	_MODULE_SET_SENSIBILITY_LOOP			;loop AGAIN
		;//////////////////////////////
		;// PROCESS KEY ENTER
		;//////////////////////////////
	_MODULE_SET_SENSIBILITY_KEY_ENTER:
		lds	Acc,_MENU_CONFIGURE_CAPTURE_MOVEMENT_SENSIBILITY;get sensibility value
		mov	r0,Acc
		ldi	Acc,255
		mov	r1,Acc
		ldi	Acc,100
		mov	r2,Acc
		call	_MULDIV_U8U8U8U8
		mov	Acc,r0
		call	_VIDEO_DIGITIZING_SET_SENSIBILITY		;set sensibility
		call	_KEYBOARD_READ_STATIC				;read static
	_MODULE_SET_SENSIBILITY_EXIT:
		call	fn_SAVE_MENU					;save new value if changed
		call	fn_MENU_DRAW_BACKGROUND				;redraw background
		call	_MENU_DRAW_STACK				;redraw menus in stack
		ret
