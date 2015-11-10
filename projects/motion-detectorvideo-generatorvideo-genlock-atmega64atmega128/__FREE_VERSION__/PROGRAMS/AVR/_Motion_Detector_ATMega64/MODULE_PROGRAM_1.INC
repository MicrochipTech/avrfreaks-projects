
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
;=======> 		M O D U L E  -  P R O G R A M # 1		 		 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module is free to implement a program #1						|
;	 -----------------------------------------------------------------------------------------------

	#include "BMPS\BMP_EARTHINV_UFO_1.INC"
	#include "BMPS\BMP_EARTHINV_UFO_2.INC"
	#include "BMPS\BMP_EARTHINV_UFO_3.INC"
	#include "BMPS\BMP_EARTHINV_UFO_S_1.INC"
	#include "BMPS\BMP_EARTHINV_UFO_S_2.INC"
	#include "BMPS\BMP_EARTHINV_UFO_S_3.INC"
	#include "BMPS\BMP_EARTHINV_UFO_MOTHER.INC"
	#include "BMPS\BMP_EARTHINV_UFO_CANNON.INC"
	#include "BMPS\BMP_EARTHINV_UFO_BARRIER.INC"
	#include "BMPS\BMP_EXPLOSION.INC"
	#include "BMPS\BMP_EXPLOSION_2.INC"
	#include "BMPS\BMP_BLACK_BOX.INC"
	#include "BMPS\BMP_PROJECTILE.INC"
	#include "BMPS\BMP_UFO_PROJECTILE.INC"
	
	;_UFO_ARRAY_STRUCT
	;	.BYTE	X
	;	.BYTE	Y			;BIT7 Y=1 show UFO, BITY=0 not show, bits6..0 Y=coord

	.EQU	_UFO_ARRAY_STRUCT_SIZE  =2
	.EQU	_UFO_ARRAY_ROWS		=5
	.EQU	_UFO_ARRAY_COLS		=8
	.EQU	_UFO_ARRAY_X_POS	=0
	.EQU	_UFO_ARRAY_Y_POS	=1
	.EQU	_UFO_ARRAY_OFFSET_X	=0
	.EQU	_UFO_ARRAY_OFFSET_Y	=12
	.EQU	_UFO_WIDTH		=14
	.EQU	_UFO_HEIGHT		=8
	.EQU	_UFO_ARRAY_SPEED	=_UFO_ARRAY_ROWS*_UFO_ARRAY_COLS*2	;speed int ticks (125 ticks per second) hi values means low speed
	.EQU	_UFO_MOTHER_SPEED	=2
	.EQU	_UFO_MOTHER_APPEAR_TIME	=1250/2
	.EQU	_UFO_ARRAY_INC_Y	=2
	.EQU	_UFO_ARRAY_MAX_X	=120
	.EQU	_UFO_ARRAY_MIN_X	=1
	.EQU	_UFO_ARRAY_MAX_Y	=78
	.EQU	_UFO_PROJ_SHOW_TIME	=100
	.EQU	_UFO_PROJ_SPEED		=3
	.EQU	_MOTHER_WIDTH		=10
	.EQU	_MOTHER_HEIGHT		=8
	.EQU	_CANNON_SPEED		=8
	.EQU	_CANNON_HEIGHT		=8
	.EQU	_CANNON_WIDTH		=10
	.EQU	_CANNON_POS_X_START	=59
	.EQU	_CANNON_POS_Y_START	=80
	.EQU	_PROJECTILE_SPEED	=2
	.EQU	_FLAG_MOTHER_SHOW	=0
	.EQU	_FLAG_PROJECTILE_FIRE	=1
	.EQU	_FLAG_VISIBLE		=7
	.EQU	_FLAG_UFO_PROJ_FIRE	=2
	.EQU	_LINE_BARRIER_Y_POS	=87
	.EQU	_SCORE_X_POS		=97
	.EQU	_SCORE_Y_POS		=89

	.DSEG
		_LIFES:			.BYTE	_BYTE				;number of lifes
		_SCORE:			.BYTE	_WORD				;score
		_CANNON_POS_X:		.BYTE	_BYTE				;hold cannon position
		_CANNON_POS_TICKS:	.BYTE	_WORD				;time to process cannon moves
		_CANNON_SHOOT_X:	.BYTE	_BYTE				;Shoot x coord
		_CANNON_SHOOT_Y:	.BYTE	_BYTE				;Shoot y coord
		_MOTHER_POS:		.BYTE	_BYTE				;hold mother position
		_MOTHER_POS_TICKS:	.BYTE	_WORD				;time to process mother moves
		_MOTHER_SHOW_TICKS:	.BYTE	_WORD				;time to show mother ~10seconds
		_FLAGS:			.BYTE	_BYTE				;flags
		_PROJECTILE_X_POS:	.BYTE	_BYTE				;Projectile X position
		_PROJECTILE_Y_POS:	.BYTE	_BYTE				;Projectile Y position
		_PROJECTILE_POS_TICKS:	.BYTE	_WORD				;Projectile speed
		_UFOS_MOVE_TICKS:	.BYTE	_WORD
		_UFOS_SPEED:		.BYTE	_BYTE
		_UFOS_DIR_X:		.BYTE	_BYTE				;1=Direction Right 0xff=Direction Left 0=stop
		_UFOS_PROJ_X_POS:	.BYTE	_BYTE				;UFO projectile X position
		_UFOS_PROJ_Y_POS:	.BYTE	_BYTE				;UFO projectile Y position
		_UFOS_PROJ_SHOW_TICKS:	.BYTE	_WORD				;UFO time to fire UFO projectile
		_UFOS_PROJ_POS_TICKS:	.BYTE	_WORD				;UFO projectile speed
		_UFO_SEED:		.BYTE	_BYTE				;for random generator
		_UFOS_ARRAY:		.BYTE	_BYTE*_UFO_ARRAY_ROWS*_UFO_ARRAY_COLS*_UFO_ARRAY_STRUCT_SIZE
	.CSEG
	_MODULE_PROGRAM_1:
		call	_RASTER_GET_CHAR_MODE				;get actual char mode
		push	Acc
		ldiaw	0
		stsaw	_SCORE						;clear score
		ldi	Acc,3						;set number of 3 lives
		sts	_LIFES,Acc
		ldi	Acc,0x27
		sts	_UFO_SEED,Acc
		call	_EARTHINV_INITIALIZATION			;Initialize
		;##############################################################################################################
		;#
		;#  EARTH INVADER MAIN LOOP
		;#
		;##############################################################################################################
_EARTHINV_LOOP:
		call	_EARTHINV_PROCESS_CANNON_MOVE				;process cannon move
		call	_EARTHINV_PROCESS_MOTHER_MOVE				;process mother move
		call	_EARTHINV_PROCESS_UFOS_MOVE				;process ufo move
		call	_EARTHINV_PROCESS_PROJECTILE				;process cannon projectile
		call	_EARTHINV_PROCESS_UFO_PROJECTILES			;process UFO projectiles
		call	_KEYBOARD_READ_FLASH					;read keyboard
		cpi	Acc,_KEY_DOWN						;down?
		lbreq	_EARTHINV_EXIT						;yes, exit
		rjmp	_EARTHINV_LOOP						;loop
		;#######################################################################################################
		;#################################
		; GET UFO RANDOM NUMBER
		; Input : 
		; Output: Acc RANDOM NUMBER 1..255
		;
		; +-+-+-+-+-+-+-+-+
		; |8|7|6|5|4|3|2|1|
		; +-+-+-+-+-+-+-+-+
		; 
		; Computed using the polymon x^8+x^6+x^5+x^4
		;#################################
_GET_RND:
		push	AccH							;save useds
		lds	Acc,_UFO_SEED						;get seed
		push	Acc							;save it
		ldi	AccH,0							;clear poly sum
		eor	AccH,Acc						;x^8
		lsl	Acc
		lsl	Acc							
		eor	AccH,Acc						;x^6
		lsl	Acc
		eor	AccH,Acc						;x^5
		lsl	Acc
		eor	AccH,Acc						;x^4
		lsl	AccH							;cy=result x^8+x^6+x^5+x^4
		pop	Acc							;restore original
		rol	Acc							;bound with orinal
		sts	_UFO_SEED,Acc
		pop	AccH							;restore useds
		ret
		;#################################
		; GET UFO RANDOM NUMBER COLUMN
		; Input : Acc COLUMN 1..8
		; Output: Acc COLUMN 0..7
		;#################################

		;random steps
_GET_RND_COL:
		pushw	Z
		pushw	X
		pushw	Y
		dec	Acc							;ajust 0..7
		ldiw	Z,_GET_RND_STEP_TABLE*2					;Z-->Step col table
		add	ZL,Acc
		ldi	AccH,0							;Z-->Step(N)
		adc	ZH,AccH
		lpm	XL,Z							;XL=Step
		mov	XH,XL							;XH=first step
		inc	Acc							;restore column
		mov	YL,Acc							;count
		ldi	YH,0							;index
		rcall	_GET_RND						;get random number
;		ldi	Acc,255							;TESTE#############
_GET_RND_COL_10:
		dec	YL							;count=0
		breq	_GET_RND_COL_20						;yes, branch
		cp	Acc,XH							;random<Step
		brlo	_GET_RND_COL_20						;yes, branch
		add	XH,XL							;next step
		inc	YH							;index++
		rjmp	_GET_RND_COL_10						;next
_GET_RND_COL_20:
		mov	Acc,YH							;get random column
		popw	Y
		popw	X
		popw	Z
		ret
_GET_RND_STEP_TABLE:
		.DB	255/1,255/2,255/3,255/4,255/5,255/6,255/7,255/8
		
		;#################################
		; GET UFO ARRAY POINTER
		; Input  : XL,YL coords
		; Output : Z->array(x,y)
		; Destroy: Flags
		;#################################
_UFO_GET_PTR:
		push	Acc
		push	R0
		push	R1
		ldiw	Z,_UFOS_ARRAY						;Z->UFOS ARRAY
		ldi	Acc,_UFO_ARRAY_COLS*_UFO_ARRAY_STRUCT_SIZE
		mul	YL,Acc							;compute Y pos
		add	ZL,r0
		adc	ZH,r1
		ldi	Acc,_UFO_ARRAY_STRUCT_SIZE				;compute X pos
		mul	XL,Acc
		add	ZL,r0
		adc	ZH,r1
		pop	r1
		pop	r0
		pop	Acc
		ret
		;#################################
		; UFO SET VISIBILITY
		; Input : XL,YL ufo array coord(x,y)
		;	  Acc = ON , visible Acc=OFF invisible
		; Output: None
		;#################################
_UFO_SET_VISIBILITY:
		pushw	Z
		pushaw
		rcall	_UFO_GET_PTR						;get ufo array(x,y)
		ldd	AccH,Z+_UFO_ARRAY_Y_POS					;get y pos
		andi	AccH,~(1<<_FLAG_VISIBLE)				;clear show flag
		andi	Acc,(1<<_FLAG_VISIBLE)
		or	AccH,Acc
		std	Z+_UFO_ARRAY_Y_POS,AccH					;set new visible state
		popaw
		popw	Z
		ret
		;#################################
		; INITIALIZE UFO ARRAY WITH INITIAL VALUES
		; Input : None
		; Output: None
		;#################################
_UFOS_INIT_ARRAY:
		push	XL
		push	YL
		pushw	Z
		pushaw
		ldi	XL,0
		ldi	YL,0
_UFOS_INIT_00:
		rcall	_UFO_GET_PTR
		ldi	Acc,_UFO_WIDTH						;x width
		mul	XL,Acc
		ldi	Acc,_UFO_ARRAY_OFFSET_X					;X offset
		add	Acc,r0
		st	Z+,Acc							;set ufo x pos
		ldi	Acc,_UFO_HEIGHT						;y height
		mul	YL,Acc
		ldi	Acc,_UFO_ARRAY_OFFSET_Y					;y offset
		add	Acc,r0
		ori	Acc,0x80						;set ufo  show flag on
		st	Z+,Acc							;set ufo y pos
		inc	XL							;x++
		cpi	XL,_UFO_ARRAY_COLS
		brlo	_UFOS_INIT_00
		ldi	XL,0
		inc	YL
		cpi	YL,_UFO_ARRAY_ROWS
		brlo	_UFOS_INIT_00
		popaw
		popw	Z
		pop	YL
		pop	XL
		ret
		;#################################
		; SHOW UFO ARRAY 
		; Input : None
		; Output: None
		;#################################
_UFOS_SHOW_ARRAY:
		pushw	X
		pushw	Y
		pushw	Z
		pushaw
		ldi	XL,0
		ldi	YL,0
_UFOS_SHOW_00:
		rcall	_UFO_GET_PTR					;GET UFO(X,Y)
		mov	Acc,YL						;acc=Y
		push	XL
		push	YL
		ld	XL,Z+						;get X
		ld	YL,Z+						;get Y
_UFOS_SHOW_10:
		mov	AccH,XL						;show ufos types acooding x bit 1
		andi	AccH,1
		brne	_UFOS_SHOW_10A					;yes, branch
		ldiw	Z,BMP_EARTHINV_UFO_1*2
		cpi	Acc,0						;ufo 1?
		breq	_UFOS_SHOW_20					;yes, branch
		ldiw	Z,BMP_EARTHINV_UFO_2*2
		dec	XL
		cpi	Acc,1						;ufo 2
		breq	_UFOS_SHOW_20					;yes, branch
		cpi	Acc,2
		breq	_UFOS_SHOW_20					;yes, branch
		inc	XL
		ldiw	Z,BMP_EARTHINV_UFO_3*2				;set ufo 3
		rjmp	_UFOS_SHOW_20					;show
_UFOS_SHOW_10A:
		ldiw	Z,BMP_EARTHINV_UFO_S_1*2
		cpi	Acc,0						;ufo 1?
		breq	_UFOS_SHOW_20					;yes, branch
		ldiw	Z,BMP_EARTHINV_UFO_S_2*2
		dec	XL
		cpi	Acc,1						;ufo 2
		breq	_UFOS_SHOW_20					;yes, branch
		cpi	Acc,2
		breq	_UFOS_SHOW_20					;yes, branch
		inc	XL
		ldiw	Z,BMP_EARTHINV_UFO_S_3*2			;set ufo 3
_UFOS_SHOW_20:
		sbrs	YL,7						;skip if show flag on
		rjmp	_UFOS_SHOW_30					;no, branch
		andi	YL,~(1<<_FLAG_VISIBLE)				;clear show flag

		ldi	XH,0
		ldi	YH,0
		call	_RASTER_SET_BMP_FAST				;set ufo
_UFOS_SHOW_30:
		pop	YL
		pop	XL
		inc	XL
		cpi	XL,_UFO_ARRAY_COLS
		brlo	_UFOS_SHOW_00
		ldi	XL,0
		inc	YL
		cpi	YL,_UFO_ARRAY_ROWS
		brlo	_UFOS_SHOW_00
		popaw
		popw	Z
		popw	Y
		popw	X
		ret
		
		;#################################
		; UPDATE UFO ARRAY X COORDS
		; Input : Acc valute to update
		; Output: None
		;#################################
_UFOS_UPDATE_ARRAY_X:
		pushw	X
		pushw	Z
		pushaw
		ldiw	Z,_UFOS_ARRAY					;UFO->ARRAY
		ldiw	X,_UFO_ARRAY_ROWS*_UFO_ARRAY_COLS		;size of array
_UFOS_UPDATE_ARRAY_X_10A:
		ldd	AccH,Z+_UFO_ARRAY_X_POS				;get X
		add	AccH,Acc					;add value
		std	Z+_UFO_ARRAY_X_POS,AccH	
		adiw	Z,_UFO_ARRAY_STRUCT_SIZE			;next UFO
		sbiw	X,1
		brne	_UFOS_UPDATE_ARRAY_X_10A
		popaw
		popw	Z
		popw	X
		ret
		;#################################
		; UPDATE UFO ARRAY Y COORDS
		; Input : Acc valute to update
		; Output: None
		;#################################
_UFOS_UPDATE_ARRAY_Y:
		pushw	X
		pushw	Z
		pushaw
		push	Temp
		ldiw	Z,_UFOS_ARRAY					;UFO->ARRAY
		ldiw	X,_UFO_ARRAY_ROWS*_UFO_ARRAY_COLS		;size of array
_UFOS_UPDATE_ARRAY_Y_10A:
		ldd	AccH,Z+_UFO_ARRAY_Y_POS				;get Y
		mov	Temp,AccH					;get show flag
		andi	Temp,(1<<_FLAG_VISIBLE)				;only show flag
		add	AccH,Acc					;add value
		andi	AccH,~(1<<_FLAG_VISIBLE)			;only 0..6 bits
		or	AccH,Temp					;restore show flag
		std	Z+_UFO_ARRAY_Y_POS,AccH	
		adiw	Z,_UFO_ARRAY_STRUCT_SIZE			;next UFO
		sbiw	X,1
		brne	_UFOS_UPDATE_ARRAY_Y_10A
		pop	Temp
		popaw
		popw	Z
		popw	X
		ret
		;#################################
		; UFOS ARRAY GET MAX X COORD
		; Input : None
		; Output: Acc Max X
		;	  AccH Ufos count
		;#################################
_UFOS_ARRAY_GET_MAX_X:
		pushw	Z
		pushw	X
		push	Temp
		ldi	Temp,0						;clear ufos count
		ldiw	Z,_UFOS_ARRAY					;Z->UFOS_ARRAY
		ldiw	X,_UFO_ARRAY_ROWS*_UFO_ARRAY_COLS		;size of array
		ldi	Acc,0						;Acc=hold max number init with min
_UFOS_ARRAY_GET_MAX_X_10:
		ldd	AccH,Z+_UFO_ARRAY_Y_POS				;Get show flag
		andi	AccH,(1<<_FLAG_VISIBLE)				;on?
		breq	_UFOS_ARRAY_GET_MAX_X_20			;no, branch
		inc	Temp						;ufos count++
		ldd	AccH,Z+_UFO_ARRAY_X_POS				;get x pos
		cp	AccH,Acc					;X Pos >max?
		brlo	_UFOS_ARRAY_GET_MAX_X_20			;no, branch
		mov	Acc,AccH					;set new high value
_UFOS_ARRAY_GET_MAX_X_20:
		adiw	Z,_UFO_ARRAY_STRUCT_SIZE			;next position
		sbiw	X,1
		brne	_UFOS_ARRAY_GET_MAX_X_10
		mov	AccH,Temp					;AccH=Temp
		pop	Temp
		popw	X
		popw	Z
		ret
		;#################################
		; UFOS ARRAY GET MIN X COORD
		; Input : None
		; Output: Acc X min
		;	  AccH number of ufos
		;#################################
_UFOS_ARRAY_GET_MIN_X:
		pushw	Z
		pushw	X
		push	Temp
		ldi	Temp,0						;ufos count
		ldiw	Z,_UFOS_ARRAY					;Z->UFOS_ARRAY
		ldiw	X,_UFO_ARRAY_ROWS*_UFO_ARRAY_COLS		;size of array
		ldi	Acc,255						;Acc=hold min number
_UFOS_ARRAY_GET_MIN_X_10:
		ldd	AccH,Z+_UFO_ARRAY_Y_POS				;Get show flag
		andi	AccH,(1<<_FLAG_VISIBLE)				;on?
		breq	_UFOS_ARRAY_GET_MIN_X_20			;no, branch
		inc	Temp						;ufos count++
		ldd	AccH,Z+_UFO_ARRAY_X_POS				;get x pos
		cp	AccH,Acc					;X Pos >max?
		brsh	_UFOS_ARRAY_GET_MIN_X_20			;no, branch
		mov	Acc,AccH					;set new high value
_UFOS_ARRAY_GET_MIN_X_20:
		adiw	Z,_UFO_ARRAY_STRUCT_SIZE			;next position
		sbiw	X,1
		brne	_UFOS_ARRAY_GET_MIN_X_10
		mov	AccH,Temp					;AccH= ufos count
		pop	Temp
		popw	X
		popw	Z
		ret
		;#################################
		; UFOS ARRAY GET MAX Y COORD
		; Input : None
		; Output: Acc y max
		;	  AccH ufos count
		;#################################
_UFOS_ARRAY_GET_MAX_Y:
		pushw	Z
		pushw	X
		push	Temp
		ldi	Temp,0						;clear ufos count
		ldiw	Z,_UFOS_ARRAY					;Z->UFOS_ARRAY
		ldiw	X,_UFO_ARRAY_ROWS*_UFO_ARRAY_COLS		;size of array
		ldi	Acc,0						;Acc=hold max number init with min
_UFOS_ARRAY_GET_MAX_Y_10:
		ldd	AccH,Z+_UFO_ARRAY_Y_POS				;Get show flag
		andi	AccH,(1<<_FLAG_VISIBLE)				;on?
		breq	_UFOS_ARRAY_GET_MAX_Y_20			;no, branch
		inc	Temp						;ufos count++
		ldd	AccH,Z+_UFO_ARRAY_Y_POS				;get x pos
		andi	AccH,~(1<<_FLAG_VISIBLE)			;clear show flags
		cp	AccH,Acc					;X Pos >max?
		brlo	_UFOS_ARRAY_GET_MAX_Y_20			;no, branch
		mov	Acc,AccH					;set new high value
_UFOS_ARRAY_GET_MAX_Y_20:
		adiw	Z,_UFO_ARRAY_STRUCT_SIZE			;next position
		sbiw	X,1
		brne	_UFOS_ARRAY_GET_MAX_Y_10
		mov	AccH,Temp					;AccH=ufos count
		pop	Temp
		popw	X
		popw	Z
		ret
		;#################################
		; UFOS ARRAY GET EXPLOSION UFO(X,Y) CELL AND X,Y COORDS
		; Input : Acc	X Contact coords
		;	  AccH  Y Contact coords
		; Output: XL,YL	  Coords of cell
		;	  XH,YH   Cell(x,y)
		;         cy=1 if cell found
		;         cy=0 if not
		;#################################
_UFOS_ARRAY_GET_EXP_COORDS:
		pushw	Z
		ldiw	Z,_UFOS_ARRAY					;Z->UFOS ARRAY
		ldi	XL,0						;col index
		ldi	YL,0						;row index
_UFOS_ARRAY_GET_EXP_COORDS_LOOP:
		push	XL
		push	YL
		ldd	XL,Z+_UFO_ARRAY_X_POS				;get x pos
		ldd	YL,Z+_UFO_ARRAY_Y_POS				;get y pos
		andi	YL,~(1<<_FLAG_VISIBLE)				;mask show bit
		mov	XH,XL						;compute XH
		subi	XH,-_UFO_WIDTH
		;inc	XH						;this made for compare
		mov	YH,YL						;compute YH
		subi	YH,-_UFO_HEIGHT					
		;inc	YH						;this made for compare
		;#################################----------------------------------
		;COMPARE COORDS check if Acc>=XL and Acc<=XH and AccH>=YL and AccH<=YH
		;#################################----------------------------------
		cp	Acc,XL						;Acc>=XL?
		brlo	_UFOS_ARRAY_GET_EXP_COORDS_10			;no, exit
		cp	Acc,XH						;Acc<=XH
		brsh	_UFOS_ARRAY_GET_EXP_COORDS_10			;no, exit
		cp	AccH,YL						;AccH>=YL
		brlo	_UFOS_ARRAY_GET_EXP_COORDS_10			;no, exit
		cp	AccH,YH						;AccH<=YH
		brsh	_UFOS_ARRAY_GET_EXP_COORDS_10			;no, exit
		;#################################----------------------------------
		; cell found ? return XL,YL Coords cell XH,YH cell(x,y)
		;#################################----------------------------------
		pop	YH						;restore cell Y
		pop	XH						;restore cell X
		sec							;set carry
		rjmp	_UFOS_ARRAY_GET_EXP_COORDS_EXIT			;exit
_UFOS_ARRAY_GET_EXP_COORDS_10:
		pop	YL						;restore cell index
		pop	XL
		adiw	Z,_UFO_ARRAY_STRUCT_SIZE			;next ufo cell
		inc	XL
		cpi	XL,_UFO_ARRAY_COLS				;end col?
		brlo	_UFOS_ARRAY_GET_EXP_COORDS_LOOP			;no, loop
		ldi	XL,0						;reset x index
		inc	YL						;end row?
		cpi	YL,_UFO_ARRAY_ROWS				;no, loop
		brlo	_UFOS_ARRAY_GET_EXP_COORDS_LOOP
		clc							;clear carry
_UFOS_ARRAY_GET_EXP_COORDS_EXIT:
		popw	Z
		ret
		;#################################
		; UFOS ARRAY GET TOP MOST POSITION AND MOST CLOSED TO CANNON
		; Input : None
		;
		; Output: R0,..,R7 top most Y index of each row where r0=Y.col(0), R1=Y.col(1) and so on
		;         R8	counts of not empty cols
		;	  XL	nearest X pos
		;	  YL	nearest Y pos
		;#################################
		;
		;             0 1 2 3 4 5 6 7
		;             # # # # # # . #		#,X - Visible UFOs
		;             # # # # # # . #		. - Destroyed UFOs (Position)
		;             # X # # # # . #		@ - Cannon
		;             # . # # # # . #		X - UFOs at top most row
		;             X . X . X . . X
		;
		;                   @
		;
		;	   +-+-+-+-+-+-+-+-+
		;	R0 |e|x|x|x|x|y|y|y|		y -  top most y index of UFO of row 0
		;	   +-+-+-+-+-+-+-+-+		x -  top most x index
		;	R1 |e|x|x|x|x|y|y|y|		y -  top most y index of UFO of row 1
		;	   +-+-+-+-+-+-+-+-+		x -  top most x index
		;	R2 |e|x|x|x|x|y|y|y|		y -  top most y index of UFO of row 2
		;	   +-+-+-+-+-+-+-+-+		x -  top most x index
		;	R3 |e|x|x|x|x|y|y|y|		y -  top most y index of UFO of row 3
		;	   +-+-+-+-+-+-+-+-+		x -  top most x index
		;	R4 |e|x|x|x|x|y|y|y|		y -  top most y index of UFO of row 4
		;	   +-+-+-+-+-+-+-+-+		x -  top most x index
		;	R5 |e|x|x|x|x|y|y|y|		y -  top most y index of UFO of row 5
		;	   +-+-+-+-+-+-+-+-+		x -  top most x index
		;	R6 |e|x|x|x|x|y|y|y|		y -  top most y index of UFO of row 6
		;	   +-+-+-+-+-+-+-+-+		x -  top most x index
		;	R7 |e|x|x|x|x|y|y|y|		y -  top most y index of UFO of row 7
		;	   +-+-+-+-+-+-+-+-+
		;
		;	e=1 means row is empty (no UFOs)
		;
		;	all empty rows appear on top of R7 dowto R0	

.EQU	_UFO_E=7
.EQU	_UFO_P=6

_UFOS_GET_TOPMOST:
		pushaw
		push	Temp
		push	XH
		push	YH
		pushw	Z
		;############################################
		; Get top most index
		;############################################
		ldiw	Z,0						;Z-->R0 and col=0
_UFOS_GET_TOPMOST_10:
		mov	XL,ZL						;get col
		rcall	_UFOS_GET_TOP					;get top
		mov	Temp,ZL						;bound col
		swap	Temp
		andi	Temp,0b01110000
		or	Acc,Temp
		st	Z+,Acc						;store topmost y index and col++
		cpi	ZL,_UFO_ARRAY_COLS
		brlo	_UFOS_GET_TOPMOST_10				;until zero
		;############################################
		; Get the nearest distance from de Cannon
		;############################################
		ldiw	Z,0						;Z-->R0
		lds	AccH,_CANNON_POS_X				;get cannon position
		ldi	Temp,127					;hold nearest distance
_UFOS_GET_TOPMOST_20:
		ld	YL,Z						;get Y index
		andi	YL,0b10001111					;extract x index
		sbrc	YL,_UFO_E					;skip if not empty
		rjmp	_UFOS_GET_TOPMOST_40				;else branch
		mov	XL,ZL						;get X index
		pushw	Z						;save Z
		call	_UFO_GET_PTR					;get pointer
		ldd	Acc,Z+_UFO_ARRAY_X_POS				;get X pos
		sub	Acc,AccH					;compute distance UFO to CANNON
		brpl	_UFOS_GET_TOPMOST_25				;branch if positive
		neg	Acc						;get abs(UFO-CANNON)
_UFOS_GET_TOPMOST_25:
		cp	Acc,Temp					;(UFO-CANNON)<MIN
		brsh	_UFOS_GET_TOPMOST_30				;no, branch
		mov	Temp,Acc					;else MIN=X
		ldd	XH,Z+_UFO_ARRAY_X_POS				;get y pos
		ldd	YH,Z+_UFO_ARRAY_Y_POS				;get y pos
		andi	YH,~(1<<_FLAG_VISIBLE)				;extract visible flag
_UFOS_GET_TOPMOST_30:
		popw	Z						;restore Z	
_UFOS_GET_TOPMOST_40:
		adiw	Z,1
		cpi	ZL,_UFO_ARRAY_COLS				;max col?
		brlo	_UFOS_GET_TOPMOST_20				;no, branch
		;############################################
		; Place ampty rows on top (r7-->r0)
		;############################################
		push	XH						;save x,y coords of the nearest 
		push	YH
		ldiw	X,0						;X->r0
		ldiw	Y,0						;Y->r0 too
		ldi	AccH,0						;total not empty rows
_UFOS_GET_TOPMOST_50:
		ld	Acc,X+						;get indexes
		mov	Temp,Acc
		andi	Acc,(1<<_UFO_E)					;empty?
		brne	_UFOS_GET_TOPMOST_60				;yes, branch
		inc	AccH						;total not empty rows --
		st	Y+,Temp						;store back
_UFOS_GET_TOPMOST_60:
		cpi	XL,_UFO_ARRAY_COLS				;last col?
		brlo	_UFOS_GET_TOPMOST_50				;no repeat
_UFOS_GET_TOPMOST_70:
		cpi	YL,_UFO_ARRAY_COLS				;destiny finish?
		brsh	_UFOS_GET_TOPMOST_80				;yes, branch
		ldi	Temp,0x80					;place empty at the end
		st	Y+,Temp
		rjmp	_UFOS_GET_TOPMOST_70
_UFOS_GET_TOPMOST_80:
		mov	R8,AccH						;r8=Total not empty rows
		pop	YH
		pop	XH
		mov	XL,XH						;get nearest X
		mov	YL,YH						;get nearest Y
		popw	Z
		pop	YH
		pop	XH
		pop	Temp
		popaw
		ret
		;#################################
		; Get Top row
		; input  : XL col
		; output : Acc topmost Y index
		; destroy: YH,YL
		;#################################
_UFOS_GET_TOP:
		pushw	Z
		ldi	YL,_UFO_ARRAY_ROWS-1				;get UFO rows
		ldi	YH,_UFO_ARRAY_ROWS				;UFO row counter
_UFOS_GET_TOP_10:
		call	_UFO_GET_PTR					;get UFO pointer
		ldd	Acc,Z+_UFO_ARRAY_Y_POS				;check if visible?
		andi	Acc,(1<<_FLAG_VISIBLE)
		brne	_UFOS_GET_TOP_20				;yes, exit
		dec	YL						;next row
		dec	YH						;count--
		brne	_UFOS_GET_TOP_10				;until zero
		ori	Acc,(1<<_UFO_E)					;set row is empty
_UFOS_GET_TOP_EXIT:
		popw	Z
		ret
_UFOS_GET_TOP_20:
		mov	Acc,YL						;YL top most row
		rjmp	_UFOS_GET_TOP_EXIT

		;#################################
		; EARTH INVADER EXIT
		;#################################
_EARTHINV_EXIT:
		call	_KEYBOARD_READ
		pop	Acc						;restore old mode
		call	_RASTER_SET_CHAR_MODE				;set mode
		call	fn_MENU_DRAW_BACKGROUND				;redraw background
		call	_MENU_DRAW_STACK				;redraw menus in stack
		ret
		;#################################
		; EARTH INVADER INITIALIZATION
		;#######################################################################################################-
_EARTHINV_INITIALIZATION:
		call	_VIDEO_OUT_CLS					;Clear screen
		ldi	Acc,_RASTER_CHAR_MODE_NORMAL			;Set character mode Normal
		call	_RASTER_SET_CHAR_MODE		
		;#################################
		; DRAW BARRIES
		;#################################
		ldiw	X,16						;Draw 1st barrier at 16,70
		ldiw	Y,70
		ldiw	Z,BMP_EARTHINV_UFO_BARRIER*2			;->barrier
		call 	_RASTER_SET_BMP_FAST				;Show Barrier
		ldiw	X,56						;Draw 1st barrier at 56,70
		ldiw	Y,70
		ldiw	Z,BMP_EARTHINV_UFO_BARRIER*2			;->barrier
		call 	_RASTER_SET_BMP_FAST				;Show Barrier
		ldiw	X,97						;Draw 1st barrier at 97,70
		ldiw	Y,70
		ldiw	Z,BMP_EARTHINV_UFO_BARRIER*2			;->barrier
		call 	_RASTER_SET_BMP_FAST				;Show Barrier
		;#################################
		; DRAW LINE SCORE SPARATOR
		;#################################
		ldiw	X,0						;draw line separator at 0,87->127,87
		ldiw	Y,_LINE_BARRIER_Y_POS
		ldiawt	SCREEN_WIDTH
		ldi	Acc,_ON
		call	_RASTER_SET_HLINE
		;#################################
		; INIT VARS
		;#################################
		ldi	Acc,_CANNON_POS_X_START				;59
		sts	_CANNON_POS_X,Acc				;cannon position
		ldi	Acc,0
		sts	_MOTHER_POS,Acc					;clear mother pos
		sts	_FLAGS,Acc					;clear flags
		ldi	Acc,1
		sts	_UFOS_DIR_X,Acc					;Set ufo direction
		ldi	Acc,76						;projectile y pos
		sts	_PROJECTILE_Y_POS,Acc
		ldi	Acc,0
		sts	_PROJECTILE_X_POS,Acc
		ldi	Acc,_UFO_ARRAY_SPEED				;set default speed
		sts	_UFOS_SPEED,Acc
		call	_RTC_GET_TICKS					;set new seed
		tst	Acc						;seed not be zero
		brne	_EARTHINV_INITIALIZATION_10			;ok, branch
		inc	Acc						;turn Acc=1
_EARTHINV_INITIALIZATION_10:
		sts	_UFO_SEED,Acc
		;#################################
		; INIT TIMERS
		;#################################
		ldiw	X,_CANNON_POS_TICKS				;speed of cannon movement
		call	_RTC_SET_ELAPSED
		ldiw	X,_MOTHER_POS_TICKS				;speed of mother movement
		call	_RTC_SET_ELAPSED
		ldiw	X,_MOTHER_SHOW_TICKS				;time to show mother
		call	_RTC_SET_ELAPSED
		ldiw	X,_UFOS_MOVE_TICKS				;time to move ufos
		call	_RTC_SET_ELAPSED
		ldiw	X,_PROJECTILE_POS_TICKS				;time to move projectile
		call	_RTC_SET_ELAPSED
		ldiw	X,_UFOS_PROJ_POS_TICKS				;time to move UFO projectile
		call	_RTC_SET_ELAPSED
		ldiw	X,_UFOS_PROJ_SHOW_TICKS				;time to fire UFO projectile
		call	_RTC_SET_ELAPSED
		;#################################
		; proces another thinks
		;#################################
		call	_EARTHINV_SHOW_LIFES				;show lives
		call	_EARTHINV_SHOW_SCORE				;show score
		call	_UFOS_INIT_ARRAY				;init ufos Array
		;call	_UFOS_SHOW_ARRAY				;show ufos
		;---------------------------------
;		ldi	XL,1
;		ldi	YL,4
;		ldi	Acc,0
;		call	_UFO_SET_VISIBILITY
;		ldi	XL,2
;		ldi	YL,4
;		ldi	Acc,0
;		call	_UFO_SET_VISIBILITY
;		ldi	XL,2
;		ldi	YL,3
;		ldi	Acc,0
;		call	_UFO_SET_VISIBILITY
;		ldi	XL,5
;		ldi	YL,0
;VOLTA00:
;		ldi	Acc,0
;		call	_UFO_SET_VISIBILITY
;		inc	XL
;		cpi	XL,_UFO_ARRAY_COLS-1
;		brlo	VOLTA00
;		ldi	XL,5
;		inc	YL
;		cpi	YL,_UFO_ARRAY_ROWS
;		brlo	VOLTA00

		call	_UFOS_SHOW_ARRAY				;show ufos
		ret
		;#################################
		; EARTH INVADER SHOW LIVES
		;#######################################################################################################-
_EARTHINV_SHOW_LIFES:
		pushaw
		pushawt
		push	Temp
		push	TempH
		pushw	X
		pushw	Y
		pushw	Z
		ldiw	X,0						;Clear LIVES background
		ldiw	Y,89
		ldiaw	32
		ldiawt	8
		ldi	Temp,_OFF
		ldi	TempH,_OFF
		call	_RASTER_SET_BOX_FULL
		lds	Acc,_LIFES					;get lives
		tst	Acc						;no, lives exit
		breq	_EARTHINV_SHOW_LIFES_EXIT
		lds	Acc,_LIFES					;get how many lifes
_EARTHINV_SHOW_LIFES_10:
		push	Acc						;savel live
		dec	Acc						;adjust 1..3 -> 0..2
		ldi	AccH,10						;multiply by 10
		mul	Acc,AccH
		movw	XH:XL,R1:R0					;get result x coords
		ldiw	Y,89						;Life Y position
		ldiw	Z,BMP_EARTHINV_UFO_CANNON*2
		call	_RASTER_SET_BMP_FAST
		pop	Acc
		dec	Acc						;Live--
		brne	_EARTHINV_SHOW_LIFES_10				;until Live=0
_EARTHINV_SHOW_LIFES_EXIT:
		popw	Z
		popw	Y
		popw	X
		pop	TempH
		pop	Temp
		popawt
		popaw
		ret
		;#################################
		; EARTH INVADER SHOW SCORE
		;#######################################################################################################-
_EARTHINV_SHOW_SCORE:
		pushw	X
		pushw	Y
		pushw	Z
		pushaw
		ldiw	X,_SCORE_X_POS					;x score
		ldiw	Y,_SCORE_Y_POS					;y score
		ldsaw	_SCORE						;get score
		call	_UITOSU
		adiw	Z,1
		call	_RASTER_SET_TEXT_OUT_SRAM			;show score
		popaw
		popw	Z
		popw	Y
		popw	X
		ret
		ret
		;#################################
		; EARTH INVADER PROCESS CANNON MOVE
		;#######################################################################################################-
_EARTHINV_PROCESS_CANNON_MOVE:
		ldiw	X,_CANNON_POS_TICKS				;time to process move?
		call	_RTC_GET_ELAPSED				;
		cpiaw	_CANNON_SPEED
		brge	_EARTHINV_PROCESS_CANNON_MOVE_10A		;yes, branch
		ret	
									;else returne
_EARTHINV_PROCESS_CANNON_MOVE_10A:
		call	_RTC_SET_ELAPSED				;set ticks
		call	_KEYBOARD_READ_FLASH				;read keyboard
		cpi	Acc,_KEY_LEFT					;move to left?
		brne	_EARTHINV_PROCESS_CANNON_MOVE_10		;no, branch
		;MOVE LEFT
		lds	XL,_CANNON_POS_X				;min position ?
		ldi	XH,0
		cpiw	X,0	
		sts	_CANNON_POS_X,XL	
		breq	_EARTHINV_PROCESS_CANNON_MOVE_10		;yes, branch
		subiw	X,1						;move to left
		sts	_CANNON_POS_X,XL
		rjmp	_EARTHINV_PROCESS_CANNON_MOVE_20		;move
_EARTHINV_PROCESS_CANNON_MOVE_10:
		cpi	Acc,_KEY_RIGHT					;move to rigth?
		brne	_EARTHINV_PROCESS_CANNON_MOVE_20		;no, branch
		;MOVE RIGHT
		lds	XL,_CANNON_POS_X				;max pos?
		ldi	XH,0
		cpiw	X,120
		sts	_CANNON_POS_X,XL
		breq	_EARTHINV_PROCESS_CANNON_MOVE_20		;no, branch
		addiw	X,1						;move to right
		sts	_CANNON_POS_X,XL					
_EARTHINV_PROCESS_CANNON_MOVE_20:
		lds	XL,_CANNON_POS_X
		ldi	XH,0
		ldiw	Y,_CANNON_POS_Y_START
		ldiw	Z,BMP_EARTHINV_UFO_CANNON*2			;->cannon
		call 	_RASTER_SET_BMP_FAST				;Show Cannon
		ret
		;#################################
		; EARTH INVADER PROCESS MOTHER MOVE
		;#######################################################################################################-
_EARTHINV_PROCESS_MOTHER_MOVE:
		ldiw	X,_MOTHER_SHOW_TICKS				;time to show mother?
		call	_RTC_GET_ELAPSED
		cpiaw	_UFO_MOTHER_APPEAR_TIME				;10 seconds time reach?
		brge	_EARTHINV_PROCESS_MOTHER_MOVE_10B		;yes, branch
		ret
_EARTHINV_PROCESS_MOTHER_MOVE_10B:
		lds	Acc,_FLAGS					;show flag set?
		andi	Acc,(1<<_FLAG_MOTHER_SHOW)
		brne	_EARTHINV_PROCESS_MOTHER_MOVE_10C		;yes, process move
		lds	Acc,_FLAGS					;set time to show flag
		ori	Acc,(1<<_FLAG_MOTHER_SHOW)
		sts	_FLAGS,Acc
		ldiw	X,_MOTHER_POS_TICKS				;clear mother ticks move
		call	_RTC_SET_ELAPSED
		ret
_EARTHINV_PROCESS_MOTHER_MOVE_10C:
		ldiw	X,_MOTHER_POS_TICKS				;time to process move?
		call	_RTC_GET_ELAPSED				;
		cpiaw	_UFO_MOTHER_SPEED
		brge	_EARTHINV_PROCESS_MOTHER_MOVE_10A		;yes, branch
		ret							;else returne
_EARTHINV_PROCESS_MOTHER_MOVE_10A:
		call	_RTC_SET_ELAPSED				;set ticks
		lds	XL,_MOTHER_POS
		ldi	XH,0
		addiw	X,1
		cpiw	X,128
		brne	_EARTHINV_PROCESS_MOTHER_MOVE_10
		lds	Acc,_FLAGS					;set move mother occured
		andi	Acc,~(1<<_FLAG_MOTHER_SHOW)
		sts	_FLAGS,Acc
		ldiw	X,_MOTHER_SHOW_TICKS				;clear time to show mother
		call	_RTC_SET_ELAPSED
		ldiw	X,0
		sts	_MOTHER_POS,XL
		ret
_EARTHINV_PROCESS_MOTHER_MOVE_10:
		sts	_MOTHER_POS,XL
		ldiw	Y,0
		ldiw	Z,BMP_EARTHINV_UFO_MOTHER*2			;->mother
		call 	_RASTER_SET_BMP_FAST				;Show mother
		ret
		;#################################
		; EARTH INVADER UFOS MOVE
		;#######################################################################################################-
_EARTHINV_PROCESS_UFOS_MOVE:
		ldiw	X,_UFOS_MOVE_TICKS				;time to show ufos?
		call	_RTC_GET_ELAPSED
		lds	Temp,_UFOS_SPEED				;get actual speed
		ldi	TempH,0
		cp	Acc,Temp					;time to process ufos
		cpc	AccH,TempH
		brge	_EARTHINV_PROCESS_UFO_MOVE_10			;yes, branch
		ret							;no, return
_EARTHINV_PROCESS_UFO_MOVE_10:
		call	_RTC_SET_ELAPSED
		lds	Acc,_UFOS_DIR_X					;get ufo direction
		call	_UFOS_UPDATE_ARRAY_X
		cpi	Acc,1						;Right direction
		breq	_EARTHINV_PROCESS_UFO_MOVE_10A			;yes, branch
		call	_UFOS_ARRAY_GET_MIN_X				;get min X
		cpi	Acc,_UFO_ARRAY_MIN_X				;low value?
		brsh	_EARTHINV_PROCESS_UFO_MOVE_20			;no, branch
		lds	Acc,_UFOS_DIR_X					;change direction
		neg	Acc
		sts	_UFOS_DIR_X,Acc
		call	_UFOS_ARRAY_GET_MAX_Y				;get max y
		cpi	Acc,_UFO_ARRAY_MAX_Y					;max reach?
		brlo	_EARTHINV_PROCESS_UFO_MOVE_10C			;no, branch
		ldi	Acc,0
		sts	_UFOS_DIR_X,Acc					;stop movement
;		rjmp	_EARTHINV_PROCESS_UFO_MOVE_20
		rjmp	_EARTHINV_GAME_OVER				;game over
_EARTHINV_PROCESS_UFO_MOVE_10C:
		ldi	Acc,_UFO_ARRAY_INC_Y
		call	_UFOS_UPDATE_ARRAY_Y				;donw y 1 pixel
		rjmp	_EARTHINV_PROCESS_UFO_MOVE_20			;else change direction		
_EARTHINV_PROCESS_UFO_MOVE_10A:
		call	_UFOS_ARRAY_GET_MAX_X				;get max x
		cpi	Acc,_UFO_ARRAY_MAX_X				;max right?
		brlo	_EARTHINV_PROCESS_UFO_MOVE_20			;no, branch
		lds	Acc,_UFOS_DIR_X					;change direction
		neg	Acc
		sts	_UFOS_DIR_X,Acc
		call	_UFOS_ARRAY_GET_MAX_Y				;Get max Y
		cpi	Acc,_UFO_ARRAY_MAX_Y				;max reach?
		brlo	_EARTHINV_PROCESS_UFO_MOVE_10C
		ldi	Acc,0						;stop move
		sts	_UFOS_DIR_X,Acc
		jmp	_EARTHINV_GAME_OVER				;game over
;#################################----------------			alterado
_EARTHINV_PROCESS_UFO_MOVE_20:
		call	_UFOS_SHOW_ARRAY
		ret
		;#################################
		; EARTH INVADER PROCESS CANNON PROJECTILE
		;#######################################################################################################-
_EARTHINV_PROCESS_PROJECTILE:
		ldiw	X,_PROJECTILE_POS_TICKS				;time do check fire?
		call	_RTC_GET_ELAPSED
		cpiaw	_PROJECTILE_SPEED
		brge	_EARTHINV_PROCESS_PROJECTILE_10			;yes, branch
		ret							;no, return
_EARTHINV_PROCESS_PROJECTILE_10:
		call	_RTC_SET_ELAPSED				;update fire time
		lds	Acc,_FLAGS					;get projectle flags
		andi	Acc,(1<<_FLAG_PROJECTILE_FIRE)
;		cpi	Acc,_ON						;firing?
		brne	_EARTHINV_PROCESS_PROJECTILE_FIRING		;yes, branch
		call	_KEYBOARD_READ_FLASH				;fire pressed?
		cpi	Acc,_KEY_ENTER
		breq	_EARTHINV_PROCESS_PROJECTILE_20			;yes, start fire
		ret
_EARTHINV_PROCESS_PROJECTILE_20:
		lds	Acc,_FLAGS
;		ldi	Acc,_ON						;set project firing
		ori	Acc,(1<<_FLAG_PROJECTILE_FIRE)
		sts	_FLAGS,Acc
		lds	Acc,_CANNON_POS_X				;get cannon position
		subi	Acc,-4
		sts	_PROJECTILE_X_POS,Acc
		ldi	Acc,76
		sts	_PROJECTILE_Y_POS,Acc
		ret
_EARTHINV_PROCESS_PROJECTILE_FIRING:
		lds	XL,_PROJECTILE_X_POS
		ldi	XH,0
		lds	YL,_PROJECTILE_Y_POS
		ldi	YH,0
		call	_RASTER_GET_PIXEL
		cpi	Acc,_ON
		breq	_EARTHINV_PROCESS_PROJECTILE_EXPLODE
		lds	XL,_PROJECTILE_X_POS
		ldi	XH,0
		lds	YL,_PROJECTILE_Y_POS
		ldi	YH,0
		ldiw	Z,BMP_PROJECTILE*2				;->Projectile
		call 	_RASTER_SET_BMP_FAST	
		lds	Acc,_PROJECTILE_Y_POS				;end of fire?
		cpi	Acc,0
		brne	_EARTHINV_PROCESS_PROJECTILE_FIRING_10		;no, continue
		lds	XL,_PROJECTILE_X_POS
		ldi	YL,0
		ldi	YH,5
		ldi	Temp,_OFF
		call	_RASTER_SET_VLINE_FAST
		lds	Acc,_FLAGS
;		ldi	Acc,_OFF						;set fire off
		andi	Acc,~(1<<_FLAG_PROJECTILE_FIRE)
		sts	_FLAGS,Acc
		ret
_EARTHINV_PROCESS_PROJECTILE_FIRING_10:
		dec	Acc
		sts	_PROJECTILE_Y_POS,Acc					;decrement y fire position
		ret
_EARTHINV_PROCESS_PROJECTILE_EXPLODE:
		lds	Acc,_PROJECTILE_X_POS					;check if object in this coord
		lds	AccH,_PROJECTILE_Y_POS
		call	_UFOS_ARRAY_GET_EXP_COORDS
		lbrcs	_EARTHINV_PROCESS_PROJECTILE_EXPLODE_10			;have, yes branch
		;#################################-------------------
		; PROCESS MOTHER UFO SHOOT
		;#################################-------------------
		lds	Acc,_PROJECTILE_X_POS					;get projectile X pos
		lds	AccH,_PROJECTILE_Y_POS					;get projectile Y pos
		lds	XL,_MOTHER_POS						;XL=mother X
		mov	XH,XL							;compute XH mother
		subi	XH,-_MOTHER_WIDTH
		ldi	YL,0							;YL=mother y0
		ldi	YH,_MOTHER_HEIGHT					;YH=mother yf
		cp	Acc,XL							;x proj < mother xL
		brlo	_EARTHINV_PROCESS_PROJECTILE_EXPLODE_10A		;yes, branch
		cp	Acc,XH							;x proj > mother xh
		brsh	_EARTHINV_PROCESS_PROJECTILE_EXPLODE_10A		;yes, branch
		cp	AccH,YH							;y proj > mother yh
		brsh	_EARTHINV_PROCESS_PROJECTILE_EXPLODE_10A		;yes, branch
		lds	XL,_MOTHER_POS
		ldi	XH,0
		ldiw	Y,0
		ldiw	Z,BMP_EXPLOSION*2					;->explosion
		call 	_RASTER_SET_BMP_FAST
		_m_delay_milliseconds	100					;little time to view explosion
		ldiw	Z,BMP_BLACK_BOX*2					;->black box
		call 	_RASTER_SET_BMP_FAST
		ldiw	X,_MOTHER_SHOW_TICKS					;time to show mother?
		call	_RTC_SET_ELAPSED
		lds	Acc,_FLAGS						;mother show flag off
;		ldi	Acc,_OFF
		andi	Acc,~(1<<_FLAG_MOTHER_SHOW)
		sts	_FLAGS,Acc
		sts	_MOTHER_POS,Acc						;clear motehr position
		lds	Acc,_FLAGS
;		ldi	Acc,_OFF						;set fire off
		andi	Acc,~(1<<_FLAG_PROJECTILE_FIRE)
		sts	_FLAGS,Acc
		ldsaw	_SCORE							;add 100 to score
		addiaw	100
		stsaw	_SCORE
		call	_EARTHINV_SHOW_SCORE					;show score
		ret
		;#################################-------------------
		; PROCESS BARRIER SHOOT
		;#################################-------------------
_EARTHINV_PROCESS_PROJECTILE_EXPLODE_10A:
		lds	XL,_PROJECTILE_X_POS					;get projectile coords
		lds	YL,_PROJECTILE_Y_POS
		ldi	YH,6
		ldi	Temp,_OFF
		call	_RASTER_SET_VLINE_FAST					;destroy barrier drawing a black line
		lds	Acc,_FLAGS
;		ldi	Acc,_OFF						;set fire off
		andi	Acc,~(1<<_FLAG_PROJECTILE_FIRE)
		sts	_FLAGS,Acc
		ret
_EARTHINV_PROCESS_PROJECTILE_EXPLODE_10:
		push	XL
		push	YL
		mov	XL,XH
		mov	YL,YH
		ldi	Acc,0
		call	_UFO_SET_VISIBILITY
		pop	YL
		pop	XL
		push	YH							;save ufo row
		ldi	XH,0
		ldi	YH,0
		ldiw	Z,BMP_EXPLOSION*2					;->explosion
		call 	_RASTER_SET_BMP_FAST
		_m_delay_milliseconds	100					;little time to view explosion
		ldiw	Z,BMP_BLACK_BOX*2					;->black box
		call 	_RASTER_SET_BMP_FAST
		lds	XL,_PROJECTILE_X_POS					;retire projectile of screen
		lds	YL,_PROJECTILE_Y_POS
		ldi	YH,6
		ldi	Temp,_OFF
		call	_RASTER_SET_VLINE_FAST
		POP	YH
		cpi	YH,0							;ufo column 1 destroyed
		brne	_EARTHINV_PROCESS_PROJECTILE_EXPLODE_20			;no, branch
		pushaw
		ldsaw	_SCORE							;score+=30
		addiaw	30
		stsaw	_SCORE
		popaw
		rjmp	_EARTHINV_PROCESS_PROJECTILE_EXPLODE_40
_EARTHINV_PROCESS_PROJECTILE_EXPLODE_20:
		cpi	YH,1							;ufo column 2 destroyed
		breq	_EARTHINV_PROCESS_PROJECTILE_EXPLODE_30			;no, branch
		cpi	YH,2							;ufo column 3 destroyed
		breq	_EARTHINV_PROCESS_PROJECTILE_EXPLODE_30			;no, branch
		pushaw
		ldsaw	_SCORE							;score+=10
		addiaw	10
		stsaw	_SCORE
		popaw
		rjmp	_EARTHINV_PROCESS_PROJECTILE_EXPLODE_40
_EARTHINV_PROCESS_PROJECTILE_EXPLODE_30:
		pushaw
		ldsaw	_SCORE							;score+=20
		addiaw	20
		stsaw	_SCORE
		popaw
_EARTHINV_PROCESS_PROJECTILE_EXPLODE_40:
		call	_UFOS_ARRAY_GET_MAX_X					;get number of ufo
		cpi	AccH,0							;all ufos destroyed?
		breq	_EARTHINV_PROCESS_PROJECTILE_EXPLODE_50			;yes, branch
		call	_EARTHINV_SHOW_SCORE					;show score
		sts	_UFOS_SPEED,AccH					;set new speed
		lds	Acc,_FLAGS
;		ldi	Acc,_OFF						;set fire off
		andi	Acc,~(1<<_FLAG_PROJECTILE_FIRE)				;set fire off
		sts	_FLAGS,Acc
		ret
_EARTHINV_PROCESS_PROJECTILE_EXPLODE_50:
		call	_EARTHINV_INITIALIZATION				;Initialize
		ret
		;#################################
		; EARTH INVADER GAME OVER
		;#######################################################################################################-
_EARTHINV_GAME_OVER:
		_M_DELAY_MILLISECONDS	500					;wait a half a second
		call	_VIDEO_OUT_CLS						;clear screen
		ldiw	X,6*_RASTER_CHAR_WIDTH					;show video state
		ldiw	Y,5*_RASTER_CHAR_HEIGHT
		ldiw	Z,_MSG_GAME_OVER*2					;Z->Game over
		call	_RASTER_SET_TEXT_OUT_FLASH
		call	_KEYBOARD_READ
		pop	Acc							;restore old mode
		jmp	_MODULE_PROGRAM_1					;restart game
_MSG_GAME_OVER:
		.db	"GAME OVER",0
		;#################################
		; EARTH INVADER PROCESS UFO PROJECTILES
		;#######################################################################################################-
		
_EARTHINV_PROCESS_UFO_PROJECTILES:
		ldiw	X,_UFOS_PROJ_POS_TICKS				;UFO proj speed?
		call	_RTC_GET_ELAPSED
		cpiaw	_UFO_PROJ_SPEED
		brge	_EARTHINV_PROCESS_UFO_PROJECTILES_10		;yes, branch
		ret							;no, return
_EARTHINV_PROCESS_UFO_PROJECTILES_10:
		call	_RTC_SET_ELAPSED				;set new UFO speed
		ldiw	X,_UFOS_PROJ_SHOW_TICKS				;time to UFO fire?
		call	_RTC_GET_ELAPSED
		cpiaw	_UFO_PROJ_SHOW_TIME
		brge	_EARTHINV_PROCESS_UFO_PROJECTILES_20		;yes, branch
		ret							;no, return
_EARTHINV_PROCESS_UFO_PROJECTILES_20:	
		lds	Acc,_FLAGS					;UFO firing?
		andi	Acc,(1<<_FLAG_UFO_PROJ_FIRE)
		brne	_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING	;yes, branch firing
		;###############################
		; UFO START FIRE
		;###############################
		lds	Acc,_FLAGS					;set Firing
		ori	Acc,(1<<_FLAG_UFO_PROJ_FIRE)
		sts	_FLAGS,Acc
		call	_UFOS_GET_TOPMOST				;get topmost and nearest
		call	_GET_RND					;shot nearest or other
		cpi	Acc,128
		brlo	_EARTHINV_PROCESS_UFO_PROJECTILES_NEAREST	;branch if nearest
		;###############################
		; PROCESS RANDOM FIRE HERE
		;###############################
		mov	Acc,R8						;get total not empty cols
		call	_GET_RND_COL					;get random col
		ldiw	Z,0						;Z-->R0
		add	ZL,Acc						;point to col(random)
		ld	Acc,Z						;get X and Y index
		mov	XL,Acc						;extract X index
		swap	XL
		andi	XL,7
		mov	YL,Acc						;extract Y index
		andi	YL,7
		call	_UFO_GET_PTR					;get data pointer
		ldd	XL,Z+_UFO_ARRAY_X_POS				;get x coord
		ldd	YL,Z+_UFO_ARRAY_Y_POS				;get y coord
		andi	YL,~(1<<_FLAG_VISIBLE)				;extract visible flag
		;###############################
		; PROCESS NEAREST FIRE HERE
		;###############################
_EARTHINV_PROCESS_UFO_PROJECTILES_NEAREST:	
		subi	XL,-4						;ufo fire x offset
		sts	_UFOS_PROJ_X_POS,XL
		subi	YL,-_UFO_HEIGHT					;add UFO height offset
		subi	YL,2						;retire black part of fire
		sts	_UFOS_PROJ_Y_POS,YL
		ret
		;###############################
		; UFO FIRING
		;###############################
_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING:	
		lds	XL,_UFOS_PROJ_X_POS				;check if ufo fire reach anything
		ldi	XH,0
		lds	YL,_UFOS_PROJ_Y_POS
		ldi	YH,0
		adiw	YH:YL,5						;compesate ufo y start position
		call	_RASTER_GET_PIXEL
		cpi	Acc,_ON
		breq	_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING_EXPLOSION	;exit for now, here insert barrier of cannon explosion TESTE ####

		lds	YL,_UFOS_PROJ_Y_POS				;get ufo proj y pos
		inc	YL
		sts	_UFOS_PROJ_Y_POS,YL				;update ufo proj y

		lds	XL,_UFOS_PROJ_X_POS
		ldi	XH,0
		lds	YL,_UFOS_PROJ_Y_POS
		ldi	YH,0
		ldiw	Z,BMP_UFO_PROJECTILE*2				;->UFO projectile
		call 	_RASTER_SET_BMP_FAST
		ret
		;###############################
		;UFO PROJECTILE EXIT
		;###############################
_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING_EXIT:
		ldiw	X,_UFOS_PROJ_SHOW_TICKS				;UFO seet time to fire?
		call	_RTC_SET_ELAPSED				
		lds	XL,_UFOS_PROJ_X_POS				;retire projectile of screen
		lds	YL,_UFOS_PROJ_Y_POS
		ldi	YH,6
		ldi	Temp,_OFF
		call	_RASTER_SET_VLINE_FAST
		lds	Acc,_FLAGS					;end fire
		andi	Acc,~(1<<_FLAG_UFO_PROJ_FIRE)
		sts	_FLAGS,Acc
		ret
		;###############################
		;UFO PROJECTILE EXPLOSION (REACH ANYTHING)
		;###############################
_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING_EXPLOSION:
		cpi	YL,_LINE_BARRIER_Y_POS					;line barrier?
		brne	_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING_EXPLOSION_10	;no, branch
		;###############################
		;GROUND EXPLOSION
		;###############################
		lds	Acc,_UFOS_PROJ_Y_POS					;compensate blank
		subi	Acc,2
		sts	_UFOS_PROJ_Y_POS,Acc
		call	_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING_EXIT
		ret
		;##############################
		;CANNON EXPLOSION
		;##############################
_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING_EXPLOSION_10:
		mov	Acc,XL							;get UFO fire X coord
		mov	AccH,YL							;get UFO fire Y coord
		lds	XL,_CANNON_POS_X					;get cannon position
		mov	XH,XL
		subi	XH,-_CANNON_WIDTH				
		ldi	YL,_CANNON_POS_Y_START
		mov	YH,YL
		subi	YH,-_CANNON_HEIGHT
		cp	Acc,XL							;ufo x>cannon XL?
		lbrlo	_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING_EXPLOSION_20	;yes, branch
		cp	Acc,XH							;ufo x<cannon XH
		lbrsh	_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING_EXPLOSION_20	;yes, branch
		cp	AccH,YL							;ufo y>cannon YL?
		lbrlo	_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING_EXPLOSION_20	;yes, branch
		cp	AccH,YH							;ufo y<cannon YH?
		lbrsh	_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING_EXPLOSION_20	;yes, branch
		lds	XL,_CANNON_POS_X					;get cannon coords
		ldi	XH,0
		ldiw	Y,_CANNON_POS_Y_START-3					
		ldiw	Z,BMP_EXPLOSION*2					;->explosion
		call 	_RASTER_SET_BMP_FAST
		_m_delay_milliseconds	100					;little time to view explosion
		ldiw	Z,BMP_BLACK_BOX*2					;->black box
		call 	_RASTER_SET_BMP_FAST
		lds	AccH,_LIFES
		ldi	Acc,3
_LIFES_10:
		push	Acc						;blink between actual number and new number of cannos remainder
		mov	Acc,AccH
		dec	Acc
		sts	_LIFES,Acc
		call	_EARTHINV_SHOW_LIFES
		_m_delay_milliseconds	250
		mov	Acc,AccH
		sts	_LIFES,Acc
		call	_EARTHINV_SHOW_LIFES
		_m_delay_milliseconds	250
		pop	Acc
		dec	Acc
		brne	_LIFES_10
		mov	Acc,AccH
		dec	Acc
		sts	_LIFES,Acc
		call	_EARTHINV_SHOW_LIFES
		dec	AccH						;update lifes
		lbreq	_EARTHINV_GAME_OVER				;if zero , game over
		sts	_LIFES,Acc
		call	_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING_EXIT
		ret

_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING_EXPLOSION_20:
		call	_EARTHINV_PROCESS_UFO_PROJECTILES_FIRING_EXIT
		ret



		;JA ATIRANDO NO CANHAO O UFO MAIS PROXIMO
		;PARA FAZER:
		;  RANDOMIZAR QUAL UFO IRA ATIRAR (completado 23/11/2015)
		;  CHECAR SE TIRO ATINJIL O CANHAO