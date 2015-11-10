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
;-----------------------------------------------||  	       	 K E Y B O A R D 		||-----------------------------------------------------
;						\\						//
;						 \\\\\\\\\\\\\\\\\\\\\\\/////////////////////////

;>--------------------------------------------------------------<
;| OBJECT     : KEYBOARD					|
;>--------------------------------------------------------------<
;| DESCRIPTION: This object allow user read states of keyboard	|
;|		keys 						|
;>--------------------------------------------------------------<

;>----------------------<
;| OBJECT DEFINITIONS	|
;>----------------------<


;-------------------------< PORT DEFINITONS >----------------------------

.EQU	_KEYBOARD_PORT_OUTPUT		= PORTD
.EQU	_KEYBOARD_PORT_DIR		= DDRD
.EQU	_KEYBOARD_PORT_INPUT		= PIND


;-------------------------< CONSTANT DEFINITONS >------------------------

.EQU	_KEYBOARD_KEY_ENTER_BIT		= 1
.EQU	_KEYBOARD_KEY_UP_BIT		= 4
.EQU	_KEYBOARD_KEY_DOWN_BIT		= 5
.EQU	_KEYBOARD_KEY_LEFT_BIT		= 6
.EQU	_KEYBOARD_KEY_RIGHT_BIT		= 7

.EQU	_KEY_NONE	= 0X00
.EQU	_KEY_ENTER	= 0X01
.EQU	_KEY_UP		= 0X02
.EQU	_KEY_DOWN	= 0X03
.EQU	_KEY_LEFT	= 0X04
.EQU	_KEY_RIGHT	= 0X05

.EQU	_KEYBOARD_DEBOUNCE_TIME	= 120		;in mileseconds
;Trigger and Repeat time are in units of _KEYBOARD_DEBOUNCE_TIME
.EQU	_KEYBOARD_TRIGGER_TIME	= 50		;trigger time for repeat mode
.EQU	_KEYBOARD_REPEAT_TIME	= 35		;repeat time for repeat mode

;-------------------------< SRAM VARIABLES >-----------------------------
.DSEG
	_KEYBOARD_CHAR_LAST:	.BYTE 	_BYTE	;hold  last char pressed by KEYBOARD_READ
	_KEYBOARD_TIME_COUNTER:	.BYTE	_BYTE	;hold a time of keyboard trigger time for repeat and repeat time
.CSEG
	;>--------------------------------------------------------------<
	;| OBJECT     : KEYBOARD					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Initialize Object				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Disable interrupts during initialization	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_KEYBOARD_INIT:
		push	Acc						;save useds
		in	Acc,SREG					;save SREG
		cli							;disable interrupts during initialization
		push	Acc
		sbi	_KEYBOARD_PORT_OUTPUT,_KEYBOARD_KEY_ENTER_BIT	;all keys with pull up
		sbi	_KEYBOARD_PORT_OUTPUT,_KEYBOARD_KEY_UP_BIT
		sbi	_KEYBOARD_PORT_OUTPUT,_KEYBOARD_KEY_DOWN_BIT
		sbi	_KEYBOARD_PORT_OUTPUT,_KEYBOARD_KEY_LEFT_BIT
		sbi	_KEYBOARD_PORT_OUTPUT,_KEYBOARD_KEY_RIGHT_BIT
		cbi	_KEYBOARD_PORT_DIR,_KEYBOARD_KEY_ENTER_BIT	;all input
		cbi	_KEYBOARD_PORT_DIR,_KEYBOARD_KEY_UP_BIT
		cbi	_KEYBOARD_PORT_DIR,_KEYBOARD_KEY_DOWN_BIT
		cbi	_KEYBOARD_PORT_DIR,_KEYBOARD_KEY_LEFT_BIT
		cbi	_KEYBOARD_PORT_DIR,_KEYBOARD_KEY_RIGHT_BIT
		ldi	Acc,-1
		sts	_KEYBOARD_CHAR_LAST,Acc
		pop	Acc						;restore SREG state
		out	SREG,Acc
		pop	Acc						;restore useds
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : KEYBOARD					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Read keyboard without waiting			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|								
	_KEYBOARD_READ_FLASH:
		ldi	Acc,_KEY_ENTER					;assume KEY ENTER
		sbis	_KEYBOARD_PORT_INPUT,_KEYBOARD_KEY_ENTER_BIT	;skip if no
		rjmp	_KEYBOARD_READ_FLASH_EXIT
		ldi	Acc,_KEY_UP					;assume KEY UP
		sbis	_KEYBOARD_PORT_INPUT,_KEYBOARD_KEY_UP_BIT	;skip if no
		rjmp	_KEYBOARD_READ_FLASH_EXIT
		ldi	Acc,_KEY_DOWN					;assume KEY DOWN
		sbis	_KEYBOARD_PORT_INPUT,_KEYBOARD_KEY_DOWN_BIT	;skip if no
		rjmp	_KEYBOARD_READ_FLASH_EXIT
		ldi	Acc,_KEY_LEFT					;assume KEY LEFT
		sbis	_KEYBOARD_PORT_INPUT,_KEYBOARD_KEY_LEFT_BIT	;skip if no
		rjmp	_KEYBOARD_READ_FLASH_EXIT
		ldi	Acc,_KEY_RIGHT					;assume KEY RIGHT
		sbis	_KEYBOARD_PORT_INPUT,_KEYBOARD_KEY_RIGHT_BIT	;skip if no
		rjmp	_KEYBOARD_READ_FLASH_EXIT
		ldi	Acc,_KEY_NONE					;otherwise NONE KEY pressed
	_KEYBOARD_READ_FLASH_EXIT:
		ret


	;>--------------------------------------------------------------<
	;| OBJECT     : KEYBOARD					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Read keyboard and if already pressed key is 	|
	;|		igual a parameter key then wait			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc Key code (parameter passed)			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|						
	_KEYBOARD_READ_AND_WAIT:
		push	Acc
		mov	AccH,Acc				;code to wait in AccH
	_KEYBOARD_READ_AND_WAIT_10:
		rcall	_KEYBOARD_READ_FLASH			;get key
		cp	Acc,AccH				;igual to a parameter
		breq	_KEYBOARD_READ_AND_WAIT_10		;yes, wait release
		pop	Acc					;return
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : KEYBOARD					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Read keyboard and wait key will be pressed	|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Acc Key Code					|
	;|		cy=1 if exit in repeat time			|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|					
	;| if in DEBUG_MODE define sequency of pressed key in _KEY_TB
	;|	

	#ifdef __DEBUG_MODE__
		.DSEG
			_KEYC:	.BYTE	_BYTE
		.CSEG
		_KEY_TB:
			.DB	_KEY_ENTER,_KEY_ENTER,_KEY_DOWN,_KEY_DOWN
			.DB	_KEY_ENTER,_KEY_DOWN,_KEY_DOWN,_KEY_ENTER
			.DB	_KEY_ENTER,_KEY_ENTER,_KEY_LEFT,_KEY_LEFT
			.DB	_KEY_DOWN,_KEY_DOWN,_KEY_ENTER,_KEY_ENTER
	#endif

	_KEYBOARD_READ:
		#ifdef	__DEBUG_MODE__
			pushw	Z
			lds	Acc,_KEYC
			ldiw	Z,_KEY_TB*2
			add	ZL,Acc
			ldi	Acc,0
			adc	ZH,Acc
			lds	Acc,_KEYC
			inc	Acc
			sts	_KEYC,Acc
			lpm	Acc,Z
			clc
			popw	Z
			ret
		#endif
		push	AccH					;save useds
		push	AccT
		rcall	_KEYBOARD_READ_FLASH			;check if any key pressed
		cpi	Acc,_KEY_NONE				;any key pressed ?
		brne	_KEYBOARD_READ_40			;yes, branch
		;/////////////////////////////////
		;// process when no key pressed 
		;/////////////////////////////////
	_KEYBOARD_READ_10:
		rcall	_KEYBOARD_READ_FLASH			;read keys
		cpi	Acc,_KEY_NONE				;none key pressed ?
		breq	_KEYBOARD_READ_10			;yes, wait
	_KEYBOARD_READ_20:
		clc						;set no repeat time
		ldi	AccT,_KEYBOARD_TRIGGER_TIME		;Timer.Count<-Trigger.Time
		sts	_KEYBOARD_TIME_COUNTER,AccT
	_KEYBOARD_READ_30:
		rcall	_KEYBOARD_DEBOUNCE			;debounce time
		rjmp	_KEYBOARD_READ_EXIT			;exit
		;/////////////////////////////////
		;// process when key pressed 
		;/////////////////////////////////
	_KEYBOARD_READ_40:
		_M_DELAY_MILLISECONDS	1			;wait 1 milesecond
		rcall	_KEYBOARD_READ_FLASH			;read keyboard
		cpi	Acc,_KEY_NONE				;none key pressed ?
		brne	_KEYBOARD_READ_50
		ldi	AccT,_KEYBOARD_TRIGGER_TIME		;Timer.Count<-Trigger.Time
		sts	_KEYBOARD_TIME_COUNTER,AccT
		ldi	AccT,0
		sts	_KEYBOARD_CHAR_LAST,AccT		;Char.Last<-0
		rjmp	_KEYBOARD_READ_40
	_KEYBOARD_READ_50:
		lds	AccH,_KEYBOARD_CHAR_LAST		;get Char.Last
		cp	Acc,AccH				;Char=Char.Last?
		brne	_KEYBOARD_READ_60			;no, branch
		lds	AccT,_KEYBOARD_TIME_COUNTER		;Timer.Count--
		dec	AccT
		sts	_KEYBOARD_TIME_COUNTER,AccT		;Timer.Count=0?
		brne	_KEYBOARD_READ_40			;no, wait
		ldi	AccT,_KEYBOARD_REPEAT_TIME		;load repeat time
		sts	_KEYBOARD_TIME_COUNTER,AccT		;Timer.Count<-Repeat.Time
		sec						;set in repeat time
		rjmp	_KEYBOARD_READ_EXIT
	_KEYBOARD_READ_60:
		sts	_KEYBOARD_CHAR_LAST,Acc			;Char.Last<-Char
		rjmp	_KEYBOARD_READ_20		
	_KEYBOARD_READ_EXIT:
       ;         rcall	_KEYBOARD_DEBOUNCE			;debounce time
		pop	AccT					;restore useds
		pop	AccH	
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : KEYBOARD					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Read keyboard without repeat time   		|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Acc Key Code					|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|					
	;| if in DEBUG_MODE define sequency of pressed key in _KEY_TB
	;|	

	#ifdef __DEBUG_MODE__
		.DSEG
			_KEYC_ST: .BYTE	_BYTE
		.CSEG
		_KEY_TB_ST:
			.DB	_KEY_ENTER,_KEY_ENTER,_KEY_DOWN,_KEY_DOWN
			.DB	_KEY_ENTER,_KEY_DOWN,_KEY_DOWN,_KEY_DOWN
			.DB	_KEY_ENTER,_KEY_ENTER
	#endif

	_KEYBOARD_READ_STATIC:
		#ifdef	__DEBUG_MODE__
			pushw	Z
			lds	Acc,_KEYC_ST
			ldiw	Z,_KEY_TB_ST*2
			add	ZL,Acc
			ldi	Acc,0
			adc	ZH,Acc
			lds	Acc,_KEYC_ST
			inc	Acc
			sts	_KEYC_ST,Acc
			lpm	Acc,Z
			clc
			popw	Z
			ret
		#endif
		push	AccH					;save useds
		rcall	_KEYBOARD_READ_FLASH			;check if any key pressed
		cpi	Acc,_KEY_NONE				;any key pressed ?
		brne	_KEYBOARD_READ_STATIC_30		;no, branch
		;/////////////////////////////////
		;// process when key not pressed 
		;/////////////////////////////////
	_KEYBOARD_READ_STATIC_10:
		rcall	_KEYBOARD_READ_FLASH			;read keyboard
		cpi	Acc,_KEY_NONE				;none key pressed?
		breq	_KEYBOARD_READ_STATIC_10		;yes, wait key pressed
	_KEYBOARD_READ_STATIC_20:
		rjmp	_KEYBOARD_READ_STATIC_EXIT		;exit with code in Acc
		;/////////////////////////////////
		;// process when key pressed 
		;/////////////////////////////////
	_KEYBOARD_READ_STATIC_30:
		_M_DELAY_MILLISECONDS	1			;wait a milesecond
		lds	AccH,_KEYBOARD_CHAR_LAST		;AccH=Char.Last
		rcall	_KEYBOARD_READ_FLASH			;read key
		cpi	Acc,_KEY_NONE				;key pressed?
		brne	_KEYBOARD_READ_STATIC_40		;yes, branch
		sts	_KEYBOARD_CHAR_LAST,Acc			;Char.Last<-Char<-0
		rjmp	_KEYBOARD_READ_STATIC_30
	_KEYBOARD_READ_STATIC_40:
		cp	Acc,AccH				;Char.Actual=Char.Last?
		breq	_KEYBOARD_READ_STATIC_30		;no, wait
		sts	_KEYBOARD_CHAR_LAST,Acc			;Char.Last<-Char
	_KEYBOARD_READ_STATIC_EXIT:
		rcall	_KEYBOARD_DEBOUNCE			;debounce time
		pop	AccH	
		ret


	;>--------------------------------------------------------------<
	;| OBJECT     : KEYBOARD					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Wait a debound time				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|						
	_KEYBOARD_DEBOUNCE:
		_M_DELAY_MILLISECONDS	_KEYBOARD_DEBOUNCE_TIME
		ret
