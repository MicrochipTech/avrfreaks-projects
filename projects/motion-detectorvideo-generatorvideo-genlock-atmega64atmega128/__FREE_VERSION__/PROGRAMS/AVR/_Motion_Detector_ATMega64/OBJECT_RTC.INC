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
;-----------------------------------------------||  	   R E A L  T I M E   C L O C K 	||-----------------------------------------------------
;						\\						//
;						 \\\\\\\\\\\\\\\\\\\\\\\/////////////////////////

;>--------------------------------------------------------------<
;| OBJECT     : RTC						|
;>--------------------------------------------------------------<
;| DESCRIPTION: This object emulate a Real Time Clock		|
;>--------------------------------------------------------------<

;>----------------------<
;| OBJECT DEFINITIONS	|
;>----------------------<


;-------------------------< PORT DEFINITONS >----------------------------


;-------------------------< CONSTANT DEFINITONS >------------------------

.EQU	_RTC_TIMER0_DIV_FACTOR	= 125-1				;factor to generate 125Hz timer0 interrupt
.EQU	_RTC_SECOND_DIV_FACTOR	= 125				;factor to generate 1 second timer

;-------------------------< SRAM  DEFINITONS >------------------------

.DSEG
	_RTC_DIV:		.BYTE	_BYTE			;RTC DIVISION FACTOR TO GENERATE 1 SECOND CLOCK
	_RTC_SECOND:		.BYTE	_BYTE			;SECONDS
	_RTC_MINUTE:		.BYTE	_BYTE			;MINUTES
	_RTC_HOUR:		.BYTE	_BYTE			;HOURS
	_RTC_TICKS:		.BYTE	_WORD			;RTC TICK (125 TICKS PER SECOND)
.CSEG

	;>--------------------------------------------------------------<
	;| OBJECT     : RTC						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RTC_INIT					|
	;>--------------------------------------------------------------<
	;| Description: Initialize RTC engine				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Disable interrupts during initialization	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_RTC_INIT:
		push	Acc						;save useds
		in	Acc,SREG					;save SREG
		cli							;disable interrupts during initialization
		push	Acc
		;//////////////////////////////
		;// program timer0 to generate 125Hz
		;// using 16Mhz crystal and 1024 prescaler
		;//////////////////////////////
		ldi	Acc,(1<<WGM01)|(0<<WGM00)|_TIMERM0_DIV_1024	;timer0 in CTC mode prescaler 1024
		out	TCCR0,Acc
		ldi	Acc,_RTC_TIMER0_DIV_FACTOR			;load CTC with division factor to generate
									;125 Hz
		out	OCR0,Acc
		_SET_HANDDLE	_HDC_OC0_VECT,_RTC_PROCESS		;set handdle interrupt process
		in	Acc,TIMSK					;enable TIMER0 CTC interrupt
		ori	Acc,(1<<OCIE0)
		out	TIMSK,Acc
		ldi	Acc,0
		sts	_RTC_DIV,Acc
		sts	_RTC_TICKS+0,Acc
		sts	_RTC_TICKS+1,Acc
		pop	Acc						;restore SREG state
		out	SREG,Acc
		pop	Acc						;restore useds
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : RTC						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RTC_PROCESS					|
	;>--------------------------------------------------------------<
	;| Description: Process RTC engine				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Interrupts are enabled when enter here		|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_RTC_PROCESS:
		push	Acc						;Save Acc
		in	Acc,SREG					;Save SREG
		push	Acc
		sei							;enable interrupts
		;/////////////////////////
		;// PUMP RTC TICK
		;/////////////////////////
		lds	Acc,_RTC_TICKS+0
		subi	Acc,-1
		sts	_RTC_TICKS+0,Acc
		lds	Acc,_RTC_TICKS+1
		sbci	Acc,-1
		sts	_RTC_TICKS+1,Acc
		;/////////////////////////
		;// PUMP RTC DIV
		;/////////////////////////
		lds	Acc,_RTC_DIV					;GET_DIV
		inc	Acc						;DIV++
		sts	_RTC_DIV,Acc		
		cpi	Acc,_RTC_SECOND_DIV_FACTOR			;DIV>=FACTOR?
		brlo	_RTC_PROCESS_EXIT				;no, branch
		ldi	Acc,0						;else clear DIV
		sts	_RTC_DIV,Acc		
		;/////////////////////////
		;// PUMP RTC SECOND
		;/////////////////////////		
		lds	Acc,_RTC_SECOND					;get second
		inc	Acc						;second++
		sts	_RTC_SECOND,Acc
		cpi	Acc,60						;second>60?
		brlo	_RTC_PROCESS_EXIT				;no, branch
		clr	Acc						;else second=0
		sts	_RTC_SECOND,Acc
		;/////////////////////////
		;// PUMP RTC MINUTE
		;/////////////////////////		
		lds	Acc,_RTC_MINUTE					;get minute
		inc	Acc						;minute++
		sts	_RTC_MINUTE,Acc
		cpi	Acc,60						;minute>60?
		brlo	_RTC_PROCESS_EXIT				;no, branch
		clr	Acc						;else second=0
		sts	_RTC_MINUTE,Acc
		;/////////////////////////
		;// PUMP RTC HOUR
		;/////////////////////////		
		lds	Acc,_RTC_HOUR					;get hour
		inc	Acc						;hour++
		sts	_RTC_HOUR,Acc
		cpi	Acc,24						;hour>24?
		brlo	_RTC_PROCESS_EXIT				;no, branch
		clr	Acc						;else second=0
		sts	_RTC_HOUR,Acc
	_RTC_PROCESS_EXIT:
		;/////////////////////////
		;// PROCESS RELAY EVENTS
		;/////////////////////////		
		call	_MOTION_RELAY_PROCESS_EVENT			;process relay events
		pop	Acc
		out	SREG,Acc					;restore Acc
		pop	Acc
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : RTC						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RTC_SET_SECOND					|
	;>--------------------------------------------------------------<
	;| Description: Set Second					|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc Second					|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	if Second>59 then Second<-0			|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_RTC_SET_SECOND:
		cpi	Acc,60						;second > 60 ?
		brlo	_RTC_SET_SECOND_10				;no, branch
		ldi	Acc,0						;else second=0
	_RTC_SET_SECOND_10:
		sts	_RTC_SECOND,Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RTC						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RTC_GET_SECOND					|
	;>--------------------------------------------------------------<
	;| Description: Set Second					|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Acc Second					|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_RTC_GET_SECOND:
		lds	Acc,_RTC_SECOND
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RTC						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RTC_SET_MINUTE					|
	;>--------------------------------------------------------------<
	;| Description: Set Minute					|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc Minute					|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	if Minute>59 then Minute<-0			|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_RTC_SET_MINUTE:
		cpi	Acc,60						;minute > 60 ?
		brlo	_RTC_SET_MINUTE_10				;no, branch
		ldi	Acc,0						;else minute=0
	_RTC_SET_MINUTE_10:
		sts	_RTC_MINUTE,Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RTC						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RTC_GET_MINITE					|
	;>--------------------------------------------------------------<
	;| Description: Set Minute					|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Acc Minute					|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_RTC_GET_MINUTE:
		lds	Acc,_RTC_MINUTE
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RTC						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RTC_SET_HOUR					|
	;>--------------------------------------------------------------<
	;| Description: Set Hour					|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc Hour					|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	if Hour>23 then Hour<-0				|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_RTC_SET_HOUR:
		cpi	Acc,24						;hour >= 24 ?
		brlo	_RTC_SET_HOUR_10				;no, branch
		ldi	Acc,0						;else Hour=0
	_RTC_SET_HOUR_10:
		sts	_RTC_HOUR,Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RTC						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RTC_GET_HOUR					|
	;>--------------------------------------------------------------<
	;| Description: Set Hour					|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Acc Hour					|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_RTC_GET_HOUR:
		lds	Acc,_RTC_HOUR
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RTC						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RTC_GET_TICKS					|
	;>--------------------------------------------------------------<
	;| Description: Get RTC TIME TICKS				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	AccH:Acc Ticks (125Hz)				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_RTC_GET_TICKS:
		pushawt						;save useds
	_RTC_GET_TICKS_10:
		ldsawt	_RTC_TICKS				;Get ticks
		ldsaw	_RTC_TICKS				;Get Ticks Again
		cp	Acc,AccT
		cpc	AccH,AccTH
		brne	_RTC_GET_TICKS_10			;branch until get correct tiks
		popawt						;restore useds
		ret	
	;>--------------------------------------------------------------<
	;| OBJECT     : RTC						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RTC_SET_ELAPSED_TICK				|
	;>--------------------------------------------------------------<
	;| Description: Get Actual ticks and save x->			|
	;>--------------------------------------------------------------<
	;| INPUT      :	X--> (_WORD) Ticks variable			|
	;| OUTPUT     :							|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|				
	_RTC_SET_ELAPSED:
		pushaw						;save useds
		call	_RTC_GET_TICKS				;get ticks
		st	X+,Acc					;save ticks
		st	X+,AccH	
		sbiw	X,2				
		popaw
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : RTC						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : RTC_gET_ELAPSED_TICK				|
	;>--------------------------------------------------------------<
	;| Description: Get Elapsed ticks				|
	;>--------------------------------------------------------------<
	;| INPUT      :	X--> (_WORD) Ticks variable			|
	;| OUTPUT     :	AccH:Acc Elapsed Ticks				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|				
	_RTC_GET_ELAPSED:
		pushawT						;save used
		ld	AccT,X+					;get old ticks
		ld	AccTH,X+
		sbiw	X,2
		call	_RTC_GET_TICKS				;Get actual ticks
		sub	Acc,AccT				;subtract new-old 
		sbc	AccH,AccTH
		popawT
		ret