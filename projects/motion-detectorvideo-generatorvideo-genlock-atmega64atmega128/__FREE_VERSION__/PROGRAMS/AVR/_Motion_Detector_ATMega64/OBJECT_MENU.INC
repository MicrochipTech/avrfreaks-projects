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
;						 ///////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\
;						//						\\
;-----------------------------------------------||  	             M E N U  			||-----------------------------------------------------
;						\\						//
;						 \\\\\\\\\\\\\\\\\\\\\\\/////////////////////////

;>--------------------------------------------------------------<
;| OBJECT     : MENU						|
;>--------------------------------------------------------------<
;| DESCRIPTION: This object allow user access and modify menu 	|
;|		itens						|
;>--------------------------------------------------------------<

;>----------------------<
;| OBJECT DEFINITIONS	|
;>----------------------<


;-------------------------< PORT DEFINITONS >----------------------------


;-------------------------< CONSTANT DEFINITONS >------------------------

.EQU	_MENU_STACK_SIZE	= 8					;max item in stack
.EQU	_MENU_STACK_PARM_SIZE	= (1+2)					;2 for address and 1 for item

.EQU	_MENU_INPUT_BOX_TYPE_OK			= 1
.EQU	_MENU_INPUT_BOX_TYPE_YES_NO		= 2
.EQU	_MENU_INPUT_BOX_TYPE_YES_NO_CANCEL	= 3

.EQU	_MENU_INPUT_BOX_ANSWER_OK	= 1
.EQU	_MENU_INPUT_BOX_ANSWER_YES	= 2
.EQU	_MENU_INPUT_BOX_ANSWER_NO	= 3
.EQU	_MENU_INPUT_BOX_ANSWER_CANCEL	= 4


;-------------------------< SRAM  DEFINITONS >------------------------

.DSEG
	_MENU_ITEM_FIRST:		.BYTE	_BYTE			;hold first item set on menu
	_MENU_STACK_PTR:		.BYTE	_BYTE			;MENU STACK POINTER
	_MENU_STACK:			.BYTE	_MENU_STACK_PARM_SIZE*_MENU_STACK_SIZE	;RESERVE SPACE FOR MENU STACK
									;first 2 bytes address
									;next byte item
.CSEG

	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_INIT					|
	;>--------------------------------------------------------------<
	;| Description: Initialize menu engine				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	Z,AccH,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Disable interrupts during initialization	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_MENU_INIT:
		push	Acc						;save useds
		in	Acc,SREG					;save SREG
		cli							;disable interrupts during initialization
		push	Acc
		ldi	Acc,0						;menu stack pointer=0
		sts	_MENU_STACK_PTR,Acc
		ldi	Acc,_MENU_STACK_PARM_SIZE*_MENU_STACK_SIZE	;clear menu stack
		ldi	AccH,0
		ldiw	Z,_MENU_STACK
	_MENU_INIT_10:
		st	Z+,AccH
		dec	Acc
		brne	_MENU_INIT_10
		pop	Acc						;restore SREG state
		out	SREG,Acc
		pop	Acc						;restore useds
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_DRAW_BOX					|
	;>--------------------------------------------------------------<
	;| Description: Draw a box with border White and background 	|
	;|		black						|
	;>--------------------------------------------------------------<
	;| INPUT      :	X,Y	   Coordinates				|
	;|		AccH:Acc   Width				|
	;|		AccTH:AccT Height				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,r2,r3,_RASTER COORDINATES DESTINE,Flags	|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_MENU_DRAW_BOX:
		pushTempW
		ldi	Temp,0
		ldi	TempH,_ON
		call	_RASTER_SET_BOX_FULL
		popTempW
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_COMPUTE_HW					|
	;>--------------------------------------------------------------<
	;| Description: Compute Text Width and Height in characters	|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z->	Menu on Flash				|
	;| OUTPUT     :	Acc	Width in characters			|
	;| 		AccH	Height in characters			|
	;| DESTROYED  :	r0,r1,r2,r3,_RASTER COORDINATES DESTINE,Flags	|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_COMPUTE_HW:
		pushw	Z					;save char pointer
		pushTempW
		ldi	Acc,0					;width =0
		ldi	AccH,0					;height =0
		ldi	TempH,0					;max Width =0
	_MENU_COMPUTE_HW_10:
		lpm	Temp,Z+					;get char
		tst	Temp					;zero ?(finish)
		breq	_MENU_COMPUTE_HW_EXIT			;yes, exit
		cpi	Temp,_NC				;none character ?
		breq	_MENU_COMPUTE_HW_10
		cpi	Temp,_CR				;carriage return ?
		breq	_MENU_COMPUTE_HW_10			;yes, get next char
		cpi	Temp,_LF				;line feed ?
		brne	_MENU_COMPUTE_HW_20			;no, branch
		inc	AccH					;else increment height
		cp	Acc,TempH				;width>=Max Width ?
		brlo	_MENU_COMPUTE_HW_10A			;no, branch
		mov	TempH,Acc				;else max width=width
	_MENU_COMPUTE_HW_10A:
		ldi	Acc,0
		rjmp	_MENU_COMPUTE_HW_10			;next char
	_MENU_COMPUTE_HW_20:
		inc	Acc					;increment width
		rjmp	_MENU_COMPUTE_HW_10			;next char
	_MENU_COMPUTE_HW_EXIT:
		cp	Acc,TempH				;see last width
		brsh	_MENU_COMPUTE_HW_EXITA			;width > max width ? no, branch
		mov	Acc,TempH				;get max width
	_MENU_COMPUTE_HW_EXITA:
		inc	AccH					;one line more
		popTempW
		popw	Z
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_GET_COORDS					|
	;>--------------------------------------------------------------<
	;| Description: Get MENU coordinates to draw			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z->	Menu on Flash				|
	;| OUTPUT     :	X,Y	Coordinates				|
	;| DESTROYED  :	r0,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_GET_COORDS:
		pushw	Z
	_MENU_GET_COORDS_10:
		lpm	r0,Z+				;get menu string
		tst	r0				;end of menu string ?
		brne	_MENU_GET_COORDS_10		;no, continue until reach
		lpm	XL,Z+				;get X coordinate
		lpm	XH,Z+
		lpm	YL,Z+				;get Y coordinate
		lpm	YH,Z+
		popw	Z
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_GET_ITEM_MENU_LINK				|
	;>--------------------------------------------------------------<
	;| Description: Get MENU item link to another MENU		|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z->	MENU on Flash				|
	;|		AccH	Item number to link			|
	;| OUTPUT     :	Y->	MENU LINK ADDRESS			|
	;|		ZF=1	if no link				|
	;| DESTROYED  :	r0,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_GET_ITEM_MENU_LINK:
		pushaw
		pushw	Z
	_MENU_GET_ITEM_LINK_10:
		lpm	r0,Z+				;get menu string
		tst	r0				;end of menu string ?
		brne	_MENU_GET_ITEM_LINK_10		;no, continue until reach
		addiw	Z,(2+2+2)			;bypass X,Y coordinates and check address
	_MENU_GET_ITEM_LINK_20:
		dec	AccH				;because itens (1..n)
		lsl	AccH				;because link is a word
		add	ZL,AccH
		ldi	AccH,0
		adc	ZH,AccH
		lpm	YL,Z+				;get ITEM MENU LINK coordinate
		lpm	YH,Z+
		lsl	YL				;because address of menu into FLASH
		rol	YH
		ldi	Acc,0				;see if zero (NO LINK)
		cp	YL,Acc
		cpc	YH,Acc
		popw	Z
		popaw
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_GET_ITEM_PROGRAM_LINK			|
	;>--------------------------------------------------------------<
	;| Description: Get MENU item program link 			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z->	MENU on Flash				|
	;|		AccH	Item number to link			|
	;| OUTPUT     :	Y->	PROGRAM LINK ADDRESS			|
	;|		cy=1	if no link				|
	;| DESTROYED  :	r0,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_GET_ITEM_PROGRAM_LINK:
		pushaw
		pushw	Z
		rcall	_MENU_GET_ITEM_LENGHT		;get ITEM LENGHT
	_MENU_GET_ITEM_PROGRAM_LINK_10:
		lpm	r0,Z+				;get menu string
		tst	r0				;end of menu string ?
		brne	_MENU_GET_ITEM_PROGRAM_LINK_10	;continue until reach
		addiw	Z,(2+2+2)			;bypass X,Y coordinates and check address
		lsl	Acc				;compute number of bytes in MENU LINK to bypass
		add	ZL,Acc
		ldi	Acc,0
		adc	ZH,Acc
		dec	AccH				;because itens (1..n)
		lsl	AccH				;because link is a word
		add	ZL,AccH
		ldi	AccH,0
		adc	ZH,AccH
		lpm	YL,Z+				;get ITEM MENU LINK coordinate
		lpm	YH,Z+
		ldi	Acc,0				;see if zero (NO LINK)
		cp	YL,Acc
		cpc	YH,Acc
		popw	Z
		popaw
		ret


	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_GET_ITEM_LENGHT 				|
	;>--------------------------------------------------------------<
	;| Description: Get MENU number of itens (number of options)	|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z->	Menu on Flash				|
	;| OUTPUT     :	Acc	Number of itens				|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_GET_ITEM_LENGHT:
		pushw	Z
		push	Temp
		ldi	Acc,0				;clear number of itens count
	_MENU_GET_ITEM_LENGHT_10:
		lpm	Temp,Z+				;get character string
		tst	Temp				;end of menu string ?
		breq	_MENU_GET_ITEM_LENGHT_EXIT	;yes, exit
		cpi	Temp,_CR			;carriage return ?
		breq	_MENU_GET_ITEM_LENGHT_10	;yes, wait end
		cpi	Temp,_LF			;line feed ?
		brne	_MENU_GET_ITEM_LENGHT_10	;no, wait
		inc	Acc				;iten++
		rjmp	_MENU_GET_ITEM_LENGHT_10
	_MENU_GET_ITEM_LENGHT_EXIT:
		inc	Acc
		pop	Temp
		popw	Z
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_SET_ITEM_CHECK 				|
	;>--------------------------------------------------------------<
	;| Description: Set MENU check field state			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z->	Menu on Flash				|
	;|		Acc	_ON(Checked)/_OFF			|
	;|		AccH	Item number				|
	;| OUTPUT     : None						|
	;| DESTROYED  :	r0,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_SET_ITEM_CHECK:
		pushw	Z					;save useds
		pushw	Y
		pushaw
		push	Temp
		push	Acc
		rcall	_MENU_GET_ITEM_LENGHT			;get length of this MENU
		mov	Temp,Acc
		pop	Acc
		inc	Temp					;plus 1 only for compare purpose
		cpi	AccH,1					;see if item <1 ?
		brlo	_MENU_SET_ITEM_CHECK_EXIT		;yes, branch
		cp	AccH,Temp				;see if item > lenght ?
		brsh	_MENU_SET_ITEM_CHECK_EXIT		;yes, branch
	_MENU_SET_ITEM_CHECK_10:
		lpm	Temp,Z+					;get MENU text string 
		tst	Temp					;end ?
		brne	_MENU_SET_ITEM_CHECK_10			;no, search until found
		ldi	Temp,1					;bit mask
		adiw	ZL,4					;skip x,y coordinates
		lpm	YL,Z+					;get address of set
		lpm	YH,Z+
	_MENU_SET_ITEM_CHECK_20:
		cpi	AccH,9					;positionated byte ?
		brlo	_MENU_SET_ITEM_CHECK_20A		;no, branch
		subi	AccH,8					;else subtract 8
		adiw	YL,1					;next position
		rjmp	_MENU_SET_ITEM_CHECK_20
	_MENU_SET_ITEM_CHECK_20A:
		cpi	AccH,1					;check bit found ?
		breq	_MENU_SET_ITEM_CHECK_30			;yes, branch
		lsl	Temp					;next bit mask
		dec	AccH
		rjmp	_MENU_SET_ITEM_CHECK_20	
	_MENU_SET_ITEM_CHECK_30:
		and	Acc,Temp				;mask bit to set reset
		ld	AccH,Y					;get old value
		com	Temp
		and	AccH,Temp				;mask no current bit
		or	Acc,AccH
		st	Y,Acc					;store back new setting
	_MENU_SET_ITEM_CHECK_EXIT:
		pop	Temp					;restore useds
		popaw
		popw	Y
		popw	Z
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_GET_ITEM_CHECK 				|
	;>--------------------------------------------------------------<
	;| Description: get MENU check field state			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z->	Menu on Flash				|
	;|		AccH	Item number				|
	;| OUTPUT     : Acc  _ON(checked)/_OFF				|
	;| DESTROYED  :	r0,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_GET_ITEM_CHECK:
		pushw	Z					;save useds
		pushw	Y
		push	AccH
		push	Temp
		push	Acc
		rcall	_MENU_GET_ITEM_LENGHT			;get length of this MENU
		mov	Temp,Acc
		pop	Acc
		inc	Temp					;plus 1 only for compare purpose
		cpi	AccH,1					;see if item <1 ?
		brlo	_MENU_GET_ITEM_CHECK_EXIT		;yes, branch
		cp	AccH,Temp				;see if item > lenght ?
		brsh	_MENU_GET_ITEM_CHECK_EXIT		;yes, branch
	_MENU_GET_ITEM_CHECK_10:
		lpm	Temp,Z+					;get MENU text string 
		tst	Temp					;end ?
		brne	_MENU_GET_ITEM_CHECK_10			;no, search until found
		ldi	Temp,1					;bit mask
		adiw	ZL,4					;skip x,y coordinates
		lpm	YL,Z+					;get address of set
		lpm	YH,Z+
	_MENU_GET_ITEM_CHECK_20:
		cpi	AccH,9					;positionated byte ?
		brlo	_MENU_GET_ITEM_CHECK_20A		;no, branch
		subi	AccH,8					;else subtract 8
		adiw	YL,1					;next position
		rjmp	_MENU_GET_ITEM_CHECK_20
	_MENU_GET_ITEM_CHECK_20A:
		cpi	AccH,1					;check time ?
		breq	_MENU_GET_ITEM_CHECK_30			;yes, branch
		lsl	Temp					;next bit mask
		dec	AccH
		rjmp	_MENU_GET_ITEM_CHECK_20	
	_MENU_GET_ITEM_CHECK_30:
		ldi	AccH,0
		ldi	Acc,_OFF				;assume exit=_OFF
		cp	YL,AccH
		cpc	YH,AccH
		breq	_MENU_GET_ITEM_CHECK_EXIT		;exit if y=0
		ld	Acc,Y					;get old state
		and	Acc,Temp				;mask bit stat
		ldi	Acc,_OFF				;assume OFF
		breq	_MENU_GET_ITEM_CHECK_EXIT		;exit if yes
		ldi	Acc,_ON					;else ON
	_MENU_GET_ITEM_CHECK_EXIT:
		pop	Temp					;restore useds
		pop	AccH
		popw	Y
		popw	Z
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_GET_ITEM_CHECK_FIRST			|
	;>--------------------------------------------------------------<
	;| Description: Get a MENU item first ITEM set			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z->    Menu on Flash				|
	;| OUTPUT     : AccH   Number of the first ITEM set 0 if none	|
	;| DESTROYED  :	r0,r1,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_GET_ITEM_CHECK_FIRST:
		push	Acc
		push	AccT						;save useds
		rcall	_MENU_GET_ITEM_LENGHT				;get number of ITENS
		mov	AccT,Acc					;AccT=number of item+1
		inc	AccT
		ldi	AccH,1						;set First item
	_MENU_GET_ITEM_CHECK_FIRST_10:
		rcall	_MENU_GET_ITEM_CHECK				;see if checked ?
		cpi	Acc,_ON
		breq	_MENU_GET_ITEM_CHECK_FIRST_20			;yes, branch
		inc	AccH						;next item
		cp	AccH,AccT					;last ?
		brlo	_MENU_GET_ITEM_CHECK_FIRST_10			;no, continue
		clr	AccH						;set not ITENS checked found
	_MENU_GET_ITEM_CHECK_FIRST_20:
		sts	_MENU_ITEM_FIRST,AccH				;store number of first ITEM found
		pop	AccT						;restore useds
		pop	Acc
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_GET_ITEM_STRING 				|
	;>--------------------------------------------------------------<
	;| Description: Get a MENU item string				|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z->    Menu on Flash				|
	;|		AccH   item to draw selected			|
	;| OUTPUT     : Z->    string(terminated by _cr,_lf or 0)	|
	;| DESTROYED  :	r0,r1,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_GET_ITEM_STRING:
		push	Temp
		pushaw
		rcall	_MENU_GET_ITEM_LENGHT			;get lenght of this menu
		cpi	AccH,1					;item < 1 ?
		brlo	_MENU_GET_ITEM_STRING_EXIT		;yes, exit
		inc	Acc
		cp	AccH,Acc				;item > max width ?
		brsh	_MENU_GET_ITEM_STRING_EXIT		;yes, exit
		ldi	Acc,1					;item count
	_MENU_GET_ITEM_STRING_10:
		cp	Acc,AccH				;item positionated ?
		breq	_MENU_GET_ITEM_STRING_EXIT		;yes exit
	_MENU_GET_ITEM_STRING_20:
		lpm	Temp,Z+					;get char menu 
		tst	Temp					;zero ?
		breq	_MENU_GET_ITEM_STRING_EXIT		;yes, done
		cpi	Temp,_CR				;carriage return ?
		breq	_MENU_GET_ITEM_STRING_20		;yes, get more char
		cpi	Temp,_LF				;line feed
		brne	_MENU_GET_ITEM_STRING_20		;no, get more char
		inc	Acc					;inc count
		rjmp	_MENU_GET_ITEM_STRING_10		;pointed
	_MENU_GET_ITEM_STRING_EXIT:
		popaw
		pop	Temp
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_DRAW_ITEM	 				|
	;>--------------------------------------------------------------<
	;| Description: Draw a MENU item with inverted normal color	|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z->	Menu on Flash				|
	;|		AccH	item to draw selected			|
	;| OUTPUT     : None						|
	;| DESTROYED  :	r0,r1,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_DRAW_ITEM:
		pushw	X
		pushw	Y
		pushw	Z
		pushaw
		pushawt
		ldi	Acc,_RASTER_CHAR_MODE_INVERTED		;set inverted
		call	_RASTER_SET_CHAR_MODE
		rcall	_MENU_GET_ITEM_LENGHT			;get menu lenght
		inc	Acc
		cpi	AccH,1					;item < 1 ?
		brsh	_MENU_DRAW_ITEM_10			;no, branch
		ldi	AccH,1
	_MENU_DRAW_ITEM_10:
		cp	AccH,Acc				;item > max itens ?
		brlo	_MENU_DRAW_ITEM_20			;no, branch
		mov	AccH,Acc				;else item=max item
		dec	AccH
	_MENU_DRAW_ITEM_20:
		mov	AccT,AccH				;AccT=item
		rcall	_MENU_DRAW				;draw menu
		rcall	_MENU_GET_COORDS			;get menu coordinates
		addiw	X,4
		addiw	Y,4
		ldi	Acc,_RASTER_CHAR_HEIGHT			;positione Y
		dec	AccH					;because 1st line is 0
		mul	Acc,AccH
		add	YL,r0					;Y+=(char_height*item)
		adc	YH,r1
		call	_RASTER_GET_CHAR_MODE			;get actual raster mode
		push	Acc					;save actual mode
		ldi	Acc,_RASTER_CHAR_MODE_NORMAL		;set inverted mode
		call	_RASTER_SET_CHAR_MODE
		rcall	_MENU_COMPUTE_HW			;get width
		mov	AccTH,Acc				;AccTH=width
		mov	AccH,AccT				;get item
		rcall	_MENU_GET_ITEM_STRING			;get item string
		ldi	AccH,0					;end string control
	_MENU_DRAW_ITEM_30:
		lpm	Acc,Z+					;get char and draw with inverted mode
		cpi	Acc,0
		breq	_MENU_DRAW_ITEM_40
		cpi	Acc,_CR					;set rest of the line space if _cr,_lf or 0 found
		breq	_MENU_DRAW_ITEM_40
		cpi	Acc,_LF
		breq	_MENU_DRAW_ITEM_40
		rjmp	_MENU_DRAW_ITEM_50
	_MENU_DRAW_ITEM_40:
		ldi	AccH,' '
	_MENU_DRAW_ITEM_50:
		cpi	AccH,' ' 
		brne	_MENU_DRAW_ITEM_60
		mov	Acc,AccH
	_MENU_DRAW_ITEM_60:
		call	_RASTER_SET_CHAR
		dec	AccTH
		brne	_MENU_DRAW_ITEM_30
		pop	Acc
		call	_RASTER_SET_CHAR_MODE			;restore mode
		popawt
		popaw
		popw	Z
		popw	Y
		popw	X
		ret


	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_INPUT	 				|
	;>--------------------------------------------------------------<
	;| Description: Get MENU item checked and key pressed value	|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z->	Menu on Flash				|
	;|		AccH	start item number to select		|
	;| OUTPUT     : Acc	pressed key value			|
	;|		AccH	item number				|
	;| DESTROYED  :	r0,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_INPUT:
		pushw	X					;save useds
		pushw	Y
		pushw	Z
		push	Temp
		rcall	_MENU_GET_ITEM_LENGHT			;get lenght
		mov	Temp,Acc				;temp = max height
		cpi	AccH,1					;start item < 1 ?
		brsh	_MENU_INPUT_10				;no, branch
		ldi	AccH,1					;else item=1
	_MENU_INPUT_10:
		inc	Acc					;only to make easy comparation
		cp	AccH,Acc				;start item > max itens ?
		brlo	_MENU_INPUT_20				;no, branch
		mov	AccH,Acc				;else set to max
		dec	AccH
	_MENU_INPUT_20:
		rcall	_MENU_DRAW_ITEM				;draw menu item 
		call	_KEYBOARD_READ				;read keyboard
		cpi	Acc,_KEY_UP				;up ?
		brne	_MENU_INPUT_30				;no, branch
		dec	AccH					;item--
		cpi	AccH,1					;item < 1?
		brsh	_MENU_INPUT_20				;no, branch
		mov	AccH,Temp				;else item=max
		rjmp	_MENU_INPUT_20
	_MENU_INPUT_30:
		cpi	Acc,_KEY_DOWN				;down ?
		brne	_MENU_INPUT_EXIT			;exit if other key
		mov	Acc,Temp				;to facility comparation
		inc	Acc
		inc	AccH					;item ++
		cp	AccH,Acc				;item > max ?
		brlo	_MENU_INPUT_20				;no, branch
		ldi	AccH,1					;else item=1
		rjmp	_MENU_INPUT_20
	_MENU_INPUT_EXIT:
		pop	Temp					;restore useds
		popw	Z
		popw	Y
		popw	X
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_TRANSLATE_MESSAGE				|
	;>--------------------------------------------------------------<
	;| Description: Get message from matriz and return pointer	|
	;|		for translated message				|
	;>--------------------------------------------------------------<
	;| INPUT      : Z message number				|
	;| OUTPUT     : Z --> translated message			|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	;| Single dranslate message
	;|
	;| .DW	MSG_INDEX_ID_1
	;|	.DB "MESSAGE",_cr,0	<-- end for odd characters message
	;| .DW	MSG_INDEX_ID_2
	;|	.DB "MESSAGE2",0	<-- end for even characgters message
	;|
	;| Multi dranslate message
	;|
	;|  .DW MSG_INDEX_ID_1
	;|	.DB "MESSAGE LINE 1",_cr,_lf ;<-- add one more _cr if odd
	;|				     ;character in message
	;|      .DB "MESSAGE LINE 2",_cr,_lf
	;|      .DB "MESSAGE LINE 3",_cr,0   ;message end
	;|


	;
	;TRANSLATED MESSAGES
	;

	.INCLUDE 	"MESSAGES.INC"				;translated messages

	_MENU_TRANSLATE_MESSAGE:
		pushw	X
		pushw	Y					;save useds
		pushaw
		movw	YL,ZL					;Y<--message index number
		ldiw	Z,_MENU_PORTUGUESE_IDIOM*2		;Z-->MENU idioma
		;/////////////////
		;// PORTUGUESE
		;/////////////////
		ldi	AccH,_MENU_IDIOMA_ITEM_PORTUGUES	;set item to portugues
		call	_MENU_GET_ITEM_CHECK			;get this setting
		ldiw	X,_MSG_LANGUAGE_PORTUGUESE*2		;set to PORTUGUESE
		cpi	Acc,_ON
		breq	_MENU_TRANSLATE_MESSAGE_10		;branch if yes
		;/////////////////
		;// ENGLISH
		;/////////////////
		ldi	AccH,_MENU_IDIOMA_ITEM_INGLES		;set item to ingles
		call	_MENU_GET_ITEM_CHECK			;get this setting
		ldiw	X,_MSG_LANGUAGE_ENGLISH*2		;set to ENGLISH
		cpi	Acc,_ON
		breq	_MENU_TRANSLATE_MESSAGE_10		;branch if yes
		;/////////////////
		;// SPANISH
		;/////////////////
		ldi	AccH,_MENU_IDIOMA_ITEM_ESPANHOL		;set item to spanish
		call	_MENU_GET_ITEM_CHECK			;get this setting
		ldiw	X,_MSG_LANGUAGE_SPANISH*2		;set to SPANISH
		cpi	Acc,_ON
		breq	_MENU_TRANSLATE_MESSAGE_10		;branch if yes
	_MENU_TRANSLATE_MESSAGE_10:
		movw	ZL,XL					;Z<--X(IDIOM.MATRIZ)
	_MENU_TRANSLATE_MESSAGE_10A:
		lpm	Acc,Z+					;get MESSAGE index
		lpm	AccH,Z+
		cp	Acc,YL					;message found ?
		cpc	AccH,YH
		breq	_MENU_TRANSLATE_MESSAGE_30			;yes, branch
	_MENU_TRANSLATE_MESSAGE_20:	
		lpm	Acc,Z+					;search for 0 end string
		tst	Acc					;end string ?
		brne	_MENU_TRANSLATE_MESSAGE_20			;no, continue until reach
		rjmp	_MENU_TRANSLATE_MESSAGE_10A			;see if string found
	_MENU_TRANSLATE_MESSAGE_30:
		popaw
		popw	Y
		popw	X
		ret
	
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_DRAW					|
	;>--------------------------------------------------------------<
	;| Description: Draw menu on screen				|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z-->	Menu in Flash				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,r2,r3,_RASTER COORDINATES DESTINE,Flags	|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|
	;| MENU STRUCTURE 
	;|
	;| MENU:	.DB "option 1",_cr,_lf	  <-- many menu itens separated by _cr,_lf
	;|		.DB "----  2",_nc,_cr,_lf <-- add _nc if number of bytes in line is odd
	;|		.DB "option 3",_cr,_lf
	;|		.DB "option 4",0	  <-- place 0 to finish menu 
	;| COORDINATES:
	;|		.DW	4,38		  <-- X and Y of top left corner of menu in pixels
	;| MEMORY:
	;|		.DW	0 OR SRAM	  <-- zero if no menu itens check is used
	;|					      or SRAM address hold them
	;| MENU_LINKS:
	;|		.DW 0			  <-- zero if this menu item 1 no has other menu link
	;|		.DW _MENU2		  <-- or name of 2nd menu 
	;|		.DW 0			  <-- number of menu links must be igual a menu itens
	;|		.DW 0
	;| PROG_LINKS:
	;|		.DW 0			  <- zero means that menu item 1 no has program link i.e. Program to be called
	;|		.DW 0
	;|		.DW _PROG		  <- address of program entry point in flash menu iten 3
	;|		.DW 0			  <-- number of program links must be igual a menu itens
	;|
	;| This output a following screen
	;|
	;|		+-----------+
	;|		| option 1  |
	;|		| option 2 >|		  <- ">" simbol appear if item has a other menu link
	;|		| option 3  |
	;|		| option 4  |
	;|		+-----------+
	;|
	;| Selected item appear in Black with White characters
	;| and not selected itens appear in White with Black characters
	;|
	_MENU_DRAW:
		pushTempW					;save useds
		pushaw
		pushawt
		pushw	X
		pushw	Y
		rcall	_MENU_COMPUTE_HW
		rcall	_MENU_GET_COORDS			;get coordinates
		ldi	Temp,_RASTER_CHAR_HEIGHT		;get char width
		mul	Temp,AccH				;compute height
		movw	AccT,r0
		ldi	Temp,_RASTER_CHAR_WIDTH			;compute width
		inc	Acc					;one col for check 
		mul	Temp,Acc
		movw	Acc,r0
		addiaw	8
		addiawt	8
		call	_MENU_DRAW_BOX				;draw menu box
		addiw	X,4					;positione text inside box
		addiw	Y,4
		pushw	X					;for future use
		pushw	Y
		call	_RASTER_SET_TEXT_OUT_FLASH
		popw	Y					;future is now
		popw	X
		add	XL,Acc					;add Width
		adc	XH,AccH
		subiw	X,_RASTER_CHAR_WIDTH+8
		rcall	_MENU_GET_ITEM_LENGHT			;get number of item for this menu
		mov	AccH,Acc				;AccH=number of itens
		ldi	Acc,1					;item=1
	_MENU_DRAW_10:
		pushaw						;save number of itens and item
		mov	AccH,Acc				;set item to get menu link
		pushw	Y
		rcall	_MENU_GET_ITEM_MENU_LINK		;get link
		popw	Y
		breq	_MENU_DRAW_10A				;branch if have a link
		ldi	Acc,207					;link char >
		rjmp	_MENU_DRAW_20
	_MENU_DRAW_10A:
		mov	AccH,Acc				;set item to get check state
		rcall	_MENU_GET_ITEM_CHECK			;get check state
		cpi	Acc,_ON					;item ON ?
		ldi	Acc,251					;assume check symbol
		breq	_MENU_DRAW_20				;yes, branch
		ldi	Acc,' '					;else space
	_MENU_DRAW_20:
		pushw	X					;save X
		addiw	X,1
		call	_RASTER_SET_CHAR			;draw ckeck char
		popw	X					;restore X
		addiw	Y,_RASTER_CHAR_HEIGHT			;next row
		popaw						;restore item counter and item
		inc	Acc					;item++
		dec	AccH					;until all itens done
		brne	_MENU_DRAW_10
		popw	Y					;restore useds
		popw	X
		popawt
		popaw
		popTempW
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_DRAW_MESSAGE_BOX				|
	;>--------------------------------------------------------------<
	;| Description: Draw MENU BOX MESSAGE on screen			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z-->	Menu in Flash				|
	;|		X,Y	Coordinates				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_MESSAGE_BOX:
		pushTempW					;save useds
		pushaw
		pushawt
		pushw	X
		pushw	Y
		rcall	_MENU_COMPUTE_HW
		ldi	Temp,_RASTER_CHAR_HEIGHT		;get char width
		mul	Temp,AccH				;compute height
		movw	AccT,r0
		ldi	Temp,_RASTER_CHAR_WIDTH			;compute width
		mul	Temp,Acc
		movw	Acc,r0
		addiaw	8
		addiawt	8
		call	_MENU_DRAW_BOX				;draw menu box
		addiw	X,4					;positione text inside box
		addiw	Y,4
		call	_RASTER_SET_TEXT_OUT_FLASH
		popw	Y					;restore useds
		popw	X
		popawt
		popaw
		popTempW
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_DRAW_INPUT_BOX_BUTTON			|
	;>--------------------------------------------------------------<
	;| Description: Draw MENU INPUT BOX MESSAGE BUTTON		|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z-->  Button.Caption				|
	;|		XL,YL Button.Coordinates			|
	;|		Temp  _ON/_OFF					|
	;| OUTPUT     : XL    Next coordinate after Button.Caption	|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_DRAW_INPUT_BOX_BUTTON:
		push	Temp
		pushaw
		pushw	X
		pushw	Y
		call	_RASTER_GET_TEXT_WIDTH			;get width
		mov	XH,Acc
		addi	XH,4					;width+=4
		push	Acc					;save width
		ldi	YH,_RASTER_CHAR_HEIGHT+4		;Box.Height=Char.Height+4
		push	XL					;save Xi
		lds	Acc,_RASTER_CHAR_MODE			;get mode
		cpi	Acc,_RASTER_CHAR_MODE_NORMAL		;normal mode?
		breq	_MENU_DRAW_INPUT_BOX_BUTTON_10		;yes, branch
		com	Temp					;else complement
	_MENU_DRAW_INPUT_BOX_BUTTON_10:
		call	_RASTER_SET_BOX_FAST			;draw selection
		pop	XL
		clr	XH
		clr	YH
		addi	XL,2					;x+=2
		addi	YL,2					;y+=2
		call	_RASTER_SET_TEXT_OUT_FLASH		;draw text
		pop	Acc					;restore width
		popw	Y
		popw	X
		add	XL,Acc					;x+=Caption.Width+4
		addi	XL,4
		popaw
		pop	Temp
		ret
	
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_GET_INPUT_BOX_ANSWER			|
	;>--------------------------------------------------------------<
	;| Description: MENU GET ANSWER OF INPUT BOX BUTTONS		|
	;>--------------------------------------------------------------<
	;| INPUT      :	XL,YL	Coordinates of outsize input box	|
	;|		XH   	Width of input box			|
	;|		YH 	Height of input box			|
	;|		Temp	Input box type				|
	;| OUTPUT     :	Acc	Answer type				|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	
	;|
	;|  (X,Y)----->	+---------------+\
	;|		|   input box	| |
	;|		|		| |
	;|		|  *->+--+	| > Height
	;|		|     |ok|	| |
	;|		|     +--+	| |
	;|		+---------------+/
	;|		\---- Width  ---/
	;|
	;| * - buttons coordiantes are computed by this function
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\	;| Anser type my be as follow:
	;|
	;|	_MENU_INPUT_BOX_ANSWER_OK
	;|	_MENU_INPUT_BOX_ANSWER_YES
	;|	_MENU_INPUT_BOX_ANSWER_NO
	;|	_MENU_INPUT_BOX_ANSWER_CANCEL
	;|

	.EQU	_MENU_INPUT_BOX_BUTTON_YES_BIT		= 7
	.EQU	_MENU_INPUT_BOX_BUTTON_NO_BIT		= 6
	.EQU	_MENU_INPUT_BOX_BUTTON_CANCEL_BIT	= 5

	_MENU_GET_INPUT_BOX_ANSWER:
		add	YL,YH					;yi=yi+height-2*char
		subi	YL,2*_RASTER_CHAR_HEIGHT
		cpi	Temp,_MENU_INPUT_BOX_TYPE_OK		;type = ok
		breq	_MENU_GET_INPUT_BOX_ANSWER_OK		;yes, branch
		cpi	Temp,_MENU_INPUT_BOX_TYPE_YES_NO	;type = yes, no
		breq	_MENU_GET_INPUT_BOX_ANSWER_YES_NO	;yes, branch
		cpi	Temp,_MENU_INPUT_BOX_TYPE_YES_NO_CANCEL	;type = yes, no, cancel
		lbreq	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL;yes, branch
		ret
	;///////////////////////////////////
	;// MOUNT BUTTON FOR OK
	;///////////////////////////////////
	_MENU_GET_INPUT_BOX_ANSWER_OK:
		;////
		;//  COMPUTE Xi=X+(Width-Wyes-4)/2
		;////
		ldiw	Z,_MSG_INPUT_BOX_YES			;translate YES message
		call	_MENU_TRANSLATE_MESSAGE
		call	_RASTER_GET_TEXT_WIDTH			;get width
		addi	Acc,4
		sub	XH,Acc					;XH=Width-Wyes
		lsr	XH					;XH/=2
		add	XL,XH					;XL=xi
		ldiw	Z,_MSG_INPUT_BOX_OK			;translate OK message
		call	_MENU_TRANSLATE_MESSAGE
		ldi	Temp,_ON
		call	_MENU_DRAW_INPUT_BOX_BUTTON
		;////
		;// WAIT KEYBOARD CONTROLS
		;////
		call	_KEYBOARD_READ_STATIC			;wait control
		ldi	Acc,_MENU_INPUT_BOX_ANSWER_OK		;set OK code
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_EXIT

	;///////////////////////////////////
	;// MOUNT BUTTON FOR YES,NO
	;///////////////////////////////////
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO:
		;////
		;//  COMPUTE Xi=X+(Width-(Wyes+Wno+8))/2
		;////
		ldiw	Z,_MSG_INPUT_BOX_YES			;translate YES message
		call	_MENU_TRANSLATE_MESSAGE
		call	_RASTER_GET_TEXT_WIDTH			;get width
		mov	AccT,Acc
		ldiw	Z,_MSG_INPUT_BOX_NO			;translate NO message
		call	_MENU_TRANSLATE_MESSAGE
		call	_RASTER_GET_TEXT_WIDTH			;get width
		add	AccT,Acc				;AccT=Wyes+Wno
		addi	AccT,8					;AccT=Wyes+Wno+8
		sub	XH,AccT					;XH=Width-(Wyes+Wno+8)
		lsr	XH					;XH/=2
		add	XL,XH					;XL=xi
		ldi	Temp,_ON
		;////
		;// DRAW BUTTONS YES,NO
		;// TempH=bit 7=Yes state
		;//	  bit 6=No state
		;//
		;// LOOP KEYBOARD INPUT CONTROLS
		;////
		clr	XH
		clr	YH
		ldi	TempH,(1<<_MENU_INPUT_BOX_BUTTON_YES_BIT)	;set YES
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_LOOP:
		pushw	X
		pushw	Y
		ldiw	Z,_MSG_INPUT_BOX_YES			;translate YES message
		call	_MENU_TRANSLATE_MESSAGE
		ldi	Temp,_OFF				;set button YES state
		sbrc	TempH,_MENU_INPUT_BOX_BUTTON_YES_BIT
		ldi	Temp,_ON
		call	_MENU_DRAW_INPUT_BOX_BUTTON
		ldiw	Z,_MSG_INPUT_BOX_NO			;translate NO message
		call	_MENU_TRANSLATE_MESSAGE
		ldi	Temp,_OFF				;set button NO state
		sbrc	TempH,_MENU_INPUT_BOX_BUTTON_NO_BIT
		ldi	Temp,_ON
		call	_MENU_DRAW_INPUT_BOX_BUTTON
		popw	Y
		popw	X
		;////
		;// WAIT KEYBOARD CONTROLS
		;////
		call	_KEYBOARD_READ_STATIC				;wait control
		cpi	Acc,_KEY_ENTER					;ENTER key ?
		breq	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_ENTER		;yes, branch
		cpi	Acc,_KEY_LEFT					;LEFT key ?
		breq	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_LEFT		;yes, branch
		cpi	Acc,_KEY_RIGHT					;RIGHT key ?
		breq	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_RIGHT		;yes, branch
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_LOOP		;else wait a valid key
		;////
		;// PROCESS ENTER KEY
		;////
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_ENTER:
		ldi	Acc,_MENU_INPUT_BOX_ANSWER_YES				;set return YES
		sbrc	TempH,_MENU_INPUT_BOX_BUTTON_YES_BIT
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_EXIT				;exit if yes
		ldi	Acc,_MENU_INPUT_BOX_ANSWER_NO				;set return NO
		sbrc	TempH,_MENU_INPUT_BOX_BUTTON_NO_BIT
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_EXIT				;exit if yes
		ldi	Acc,_MENU_INPUT_BOX_ANSWER_CANCEL			;set return CANCEL
		sbrc	TempH,_MENU_INPUT_BOX_BUTTON_CANCEL_BIT
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_EXIT				;exit if yes
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_LOOP
		;////
		;// PROCESS LEFT KEY
		;////
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_LEFT:
		rol	TempH							;set next key
		brcc	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_LEFT_10		;branch if no last key
		ldi	TempH,(1<<_MENU_INPUT_BOX_BUTTON_NO_BIT)		;else set NO
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_LEFT_10:
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_LOOP
		;////
		;// PROCESS RIGHT KEY
		;////
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_RIGHT:
		lsr	TempH							;set next key
		cpi	TempH,(1<<(_MENU_INPUT_BOX_BUTTON_NO_BIT-1))		;last key ?	
		brne	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_RIGHT_10		;branch if no
		ldi	TempH,(1<<_MENU_INPUT_BOX_BUTTON_YES_BIT)		;else set YES
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_RIGHT_10:
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_LOOP
	;///////////////////////////////////
	;// MOUNT BUTTON FOR YES,NO,CANCEL
	;///////////////////////////////////
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL:
		;////
		;//  COMPUTE Xi=X+(Width-(Wyes+Wno+Wcancel+12))/2
		;////
		ldiw	Z,_MSG_INPUT_BOX_YES			;translate YES message
		call	_MENU_TRANSLATE_MESSAGE
		call	_RASTER_GET_TEXT_WIDTH			;get width
		mov	AccT,Acc
		ldiw	Z,_MSG_INPUT_BOX_NO			;translate NO message
		call	_MENU_TRANSLATE_MESSAGE
		call	_RASTER_GET_TEXT_WIDTH			;get width
		add	AccT,Acc				;AccT=Wyes+Wno
		ldiw	Z,_MSG_INPUT_BOX_CANCEL			;translate CANCEL message
		call	_MENU_TRANSLATE_MESSAGE
		call	_RASTER_GET_TEXT_WIDTH			;get width
		add	AccT,Acc				;AccT=Wyes+Wno+Wcancel
		addi	AccT,12					;AccT=Wyes+Wno+Wcancel+12
		sub	XH,AccT					;XH=Width-(Wyes+Wno+Wcancel+12)
		lsr	XH					;XH/=2
		add	XL,XH					;XL=xi
		ldi	Temp,_ON
		;////
		;// DRAW BUTTONS YES,NO,CANCEL
		;// TempH=bit 7=Yes state
		;//	  bit 6=No state
		;//	  bit 5=Cancel state
		;//
		;// LOOP KEYBOARD INPUT CONTROLS
		;////
		clr	XH
		clr	YH
		ldi	TempH,(1<<_MENU_INPUT_BOX_BUTTON_YES_BIT)	;set YES
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_LOOP:
		pushw	X
		pushw	Y
		ldiw	Z,_MSG_INPUT_BOX_YES			;translate YES message
		call	_MENU_TRANSLATE_MESSAGE
		ldi	Temp,_OFF				;set button YES state
		sbrc	TempH,_MENU_INPUT_BOX_BUTTON_YES_BIT
		ldi	Temp,_ON
		call	_MENU_DRAW_INPUT_BOX_BUTTON
		ldiw	Z,_MSG_INPUT_BOX_NO			;translate NO message
		call	_MENU_TRANSLATE_MESSAGE
		ldi	Temp,_OFF				;set button NO state
		sbrc	TempH,_MENU_INPUT_BOX_BUTTON_NO_BIT
		ldi	Temp,_ON
		call	_MENU_DRAW_INPUT_BOX_BUTTON
		ldiw	Z,_MSG_INPUT_BOX_CANCEL			;translate CANCEL message
		call	_MENU_TRANSLATE_MESSAGE
		ldi	Temp,_OFF				;set button CANCEL state
		sbrc	TempH,_MENU_INPUT_BOX_BUTTON_CANCEL_BIT
		ldi	Temp,_ON
		call	_MENU_DRAW_INPUT_BOX_BUTTON
		popw	Y
		popw	X
		;////
		;// WAIT KEYBOARD CONTROLS
		;////
		call	_KEYBOARD_READ_STATIC					;wait control
		cpi	Acc,_KEY_ENTER						;ENTER key ?
		breq	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_ENTER		;yes, branch
		cpi	Acc,_KEY_LEFT						;LEFT key ?
		breq	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_LEFT		;yes, branch
		cpi	Acc,_KEY_RIGHT						;RIGHT key ?
		breq	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_RIGHT		;yes, branch
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_LOOP		;else wait a valid key
		;////
		;// PROCESS ENTER KEY
		;////
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_ENTER:
		ldi	Acc,_MENU_INPUT_BOX_ANSWER_YES				;set return YES
		sbrc	TempH,_MENU_INPUT_BOX_BUTTON_YES_BIT
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_EXIT				;exit if yes
		ldi	Acc,_MENU_INPUT_BOX_ANSWER_NO				;set return NO
		sbrc	TempH,_MENU_INPUT_BOX_BUTTON_NO_BIT
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_EXIT				;exit if yes
		ldi	Acc,_MENU_INPUT_BOX_ANSWER_CANCEL			;set return CANCEL
		sbrc	TempH,_MENU_INPUT_BOX_BUTTON_CANCEL_BIT
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_EXIT				;exit if yes
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_LOOP
		;////
		;// PROCESS LEFT KEY
		;////
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_LEFT:
		rol	TempH							;set next key
		brcc	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_LEFT_10	;branch if no last key
		ldi	TempH,(1<<_MENU_INPUT_BOX_BUTTON_CANCEL_BIT)		;else set CANCEL
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_LEFT_10:
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_LOOP
		;////
		;// PROCESS RIGHT KEY
		;////
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_RIGHT:
		lsr	TempH							;set next key
		cpi	TempH,(1<<(_MENU_INPUT_BOX_BUTTON_CANCEL_BIT-1))	;last key ?	
		brne	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_RIGHT_10	;branch if no
		ldi	TempH,(1<<_MENU_INPUT_BOX_BUTTON_YES_BIT)		;else set YES
	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_RIGHT_10:
		rjmp	_MENU_GET_INPUT_BOX_ANSWER_YES_NO_CANCEL_LOOP
		;/////
		;// RETURN
		;/////
	_MENU_GET_INPUT_BOX_ANSWER_EXIT:
			ret

			
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_DRAW_INPUT_BOX				|
	;>--------------------------------------------------------------<
	;| Description: Draw MENU INPUT BOX MESSAGE on screen and	|
	;|		wait a key according pass parameter		|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z-->	Menu in Flash				|
	;|		X,Y	Coordinates				|
	;|		Acc	Input Box Type				|
	;| OUTPUT     :	Acc	Answer type				|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Input Box Type my be as follow:			|
	;|			_MENU_INPUT_BOX_TYPE_OK			|
	;|			_MENU_INPUT_BOX_TYPE_YES_NO		|
	;|			_MENU_INPUT_BOX_TYPE_YES_NO_CANCEL	|
	;|		Input Box Answer my be as follow:		|
	;|			_MENU_INPUT_BOX_ANSWER_OK	= 1	|
	;|			_MENU_INPUT_BOX_ANSWER_YES	= 2	|
	;|			_MENU_INPUT_BOX_ANSWER_NO	= 3	|
	;|			_MENU_INPUT_BOX_ANSWER_CANCEL	= 4	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_INPUT_BOX:
		pushTempW					;save useds
		push	AccH
		pushawt
		pushw	X
		pushw	Y
		pushw	Z
		mov	AccT,Acc				;save input box type
		rcall	_MENU_COMPUTE_HW
		addi	AccH,2					;reserve space for buttons dialog
		ldi	AccTH,3
		cpi	AccT,_MENU_INPUT_BOX_TYPE_OK		;type = ok
		breq	_MENU_INPUT_BOX_10			;yes, branch
		ldi	AccTH,8
		cpi	AccT,_MENU_INPUT_BOX_TYPE_YES_NO	;type = yes, no
		breq	_MENU_INPUT_BOX_10			;yes, branch
		ldi	AccTH,14
	_MENU_INPUT_BOX_10:
		cp	Acc,AccTH				;string.width>buttons.width ?
		brsh	_MENU_INPUT_BOX_20			;yes, branch
		mov	Acc,AccTH				;else assume max value
	_MENU_INPUT_BOX_20:
		push	AccT					;save input box type
		ldi	Temp,_RASTER_CHAR_HEIGHT		;get char width
		mul	Temp,AccH				;compute height
		movw	AccT,r0
		ldi	Temp,_RASTER_CHAR_WIDTH			;compute width
		mul	Temp,Acc
		movw	Acc,r0
		addiaw	8
		addiawt	8
		lds	Temp,_RASTER_CHAR_MODE			;get mode
		cpi	Temp,_RASTER_CHAR_MODE_NORMAL		;normal ?
		ldi	Temp,_OFF
		ldi	TempH,_ON
		brne	_MENU_INPUT_BOX_30			;yes, branch
		ldi	Temp,_ON				;else assume inverted
		ldi	TempH,_OFF
	_MENU_INPUT_BOX_30:
		pushw	X
		pushw	Y
		call	_RASTER_SET_BOX_FULL
		addiw	X,4					;positione text inside box
		addiw	Y,4
		call	_RASTER_SET_TEXT_OUT_FLASH
		popw	Y
		popw	X
		pop	Temp
		mov	XH,Acc					;put width and height in appropriate registers
		mov	YH,AccT
		call	_MENU_GET_INPUT_BOX_ANSWER		;draw buttons
		popw	Z
		popw	Y					;restore useds
		popw	X
		popawt
		pop	AccH
		popTempW
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_PUSH					|
	;>--------------------------------------------------------------<
	;| Description: PUSH MENU INTO STACK				|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z-->	Menu in Flash				|
	;|		AccH	Item					|
	;| OUTPUT     :	Acc	stack position				|
	;| DESTROYED  :	r0,r1,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_PUSH:
		pushw	Y
		pushw	Z					;save useds
		push	AccH
		push	AccT
		lds	Acc,_MENU_STACK_PTR			;get stack pointer
		ldi	AccT,_MENU_STACK_PARM_SIZE		;compute stack position
		mul	Acc,AccT
		ldiw	Y,_MENU_STACK				;Y-->menu stack
		add	YL,r0
		adc	YH,r1
		st	Y+,ZL					;stack address
		st	Y+,ZH
		st	Y+,AccH					;stack item
		lds	Acc,_MENU_STACK_PTR			;get stack ptr
		inc	Acc					;stack++
		cpi	Acc,_MENU_STACK_SIZE			;stack> max ?
		brlo	_MENU_PUSH_10				;no, branch
		ldi	Acc,_MENU_STACK_SIZE-1			;else sature with max
	_MENU_PUSH_10:
		sts	_MENU_STACK_PTR,Acc
		pop	AccT
		pop	AccH					;restore useds
		popw	Z
		popw	Y
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_POP					|
	;>--------------------------------------------------------------<
	;| Description: POP MENU FROM STACK				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Z--> MENU poped					|
	;|		Acc  MENU stack position			|
	;|		AccH ITEM poped					|
	;| DESTROYED  :	r0,r1,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_POP:
		pushw	Y
		push	AccT
		lds	Acc,_MENU_STACK_PTR			;get stack pointer
		cpi	Acc,0					;empty ?
		breq	_MENU_POP_10				;yes, branch
		dec	Acc					;else decrement stack
	_MENU_POP_10:
		sts	_MENU_STACK_PTR,Acc
		ldi	AccT,_MENU_STACK_PARM_SIZE		;compute stack position
		mul	Acc,AccT
		ldiw	Y,_MENU_STACK				;Y-->menu stack
		add	YL,r0
		adc	YH,r1
		ld	ZL,Y+					;get stack address
		ld	ZH,Y+
		ld	AccH,Y+					;get item
		pop	AccT					;restore useds
		popw	Y
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_GET_STACK					|
	;>--------------------------------------------------------------<
	;| Description: GET MENU FROM STACK				|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc MENU stack item				|
	;| OUTPUT     :	Z--> MENU					|
	;|		AccH ITEM 					|
	;|		cy=1 if MENU ITEM>STACK				|
	;| DESTROYED  :	r0,r1,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_GET_STACK:
		pushw	Y
		push	Acc
		push	AccT
		lds	AccT,_MENU_STACK_PTR			;get stack pointer
		cp	Acc,AccT				;MENU item > stack ?
		brlo	_MENU_GET_STACK_10			;no, branch
		sec						;set end
		rjmp	_MENU_GET_STACK_EXIT
	_MENU_GET_STACK_10:
		ldi	AccT,_MENU_STACK_PARM_SIZE		;compute stack position
		mul	Acc,AccT
		ldiw	Y,_MENU_STACK				;Y-->menu stack
		add	YL,r0
		adc	YH,r1
		ld	ZL,Y+					;get stack address
		ld	ZH,Y+
		ld	AccH,Y+					;get item
		clc						;set ok
	_MENU_GET_STACK_EXIT:
		pop	AccT					;restore useds
		pop	Acc
		popw	Y
		ret
		
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_GET_STACK_CURRENT				|
	;>--------------------------------------------------------------<
	;| Description: GET MENU FROM STACK CURRENT			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Z--> MENU					|
	;|		Acc  MENU stack position			|
	;|		AccH ITEM 					|
	;|		cy=1 if stack is empty				|
	;| DESTROYED  :	r0,r1,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_GET_STACK_CURRENT:
		pushw	Y
		push	AccT
		lds	Acc,_MENU_STACK_PTR			;get stack pointer
		tst	Acc					;stack empty?
		brne	_MENU_GET_STACK_CURR_10			;no, branch
		sec						;set end
		rjmp	_MENU_GET_STACK_CURR_EXIT
	_MENU_GET_STACK_CURR_10:
		dec	Acc					;put in correct position
		ldi	AccT,_MENU_STACK_PARM_SIZE		;compute stack position
		mul	Acc,AccT
		ldiw	Y,_MENU_STACK				;Y-->menu stack
		add	YL,r0
		adc	YH,r1
		ld	ZL,Y+					;get stack address
		ld	ZH,Y+
		ld	AccH,Y+					;get item
		clc						;set ok
	_MENU_GET_STACK_CURR_EXIT:
		pop	AccT					;restore useds
		popw	Y
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_GET_STACK_CURRENT				|
	;>--------------------------------------------------------------<
	;| Description: GET MENU FROM STACK CURRENT			|
	;>--------------------------------------------------------------<
	;| INPUT      :	AccH Item					|
	;| OUTPUT     :	CY=1 if stack empty				|
	;| DESTROYED  :	r0,r1,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_SET_STACK_CURRENT_ITEM:
		pushw	Y
		push	AccT
		lds	Acc,_MENU_STACK_PTR			;get stack pointer
		tst	Acc					;stack empty?
		brne	_MENU_SET_STACK_CURR_10			;no, branch
		sec						;set end
		rjmp	_MENU_SET_STACK_CURR_EXIT
	_MENU_SET_STACK_CURR_10:
		dec	Acc					;put in correct position
		ldi	AccT,_MENU_STACK_PARM_SIZE		;compute stack position
		mul	Acc,AccT
		ldiw	Y,_MENU_STACK				;Y-->menu stack
		add	YL,r0
		adc	YH,r1
		std	Y+2,AccH				;update item
		clc						;set ok
	_MENU_SET_STACK_CURR_EXIT:
		pop	AccT					;restore useds
		popw	Y
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : MENU						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : MENU_DRAW_STACK					|
	;>--------------------------------------------------------------<
	;| Description: DRAW all MENUS IN STACK				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	r0,r1,Flags					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_MENU_DRAW_STACK:
		pushw	Z
		pushaw
		ldi	Acc,0					;set first menu item
	_MENU_DRAW_STACK_10:
		rcall	_MENU_GET_STACK				;get MENU parms from stack
		brcs	_MENU_DRAW_STACK_EXIT			;exit if end
		rcall	_MENU_DRAW_ITEM
		inc	Acc					;next item in stack
		rjmp	_MENU_DRAW_STACK_10			;until done
	_MENU_DRAW_STACK_EXIT:
		popaw
		popw	Z
		ret
