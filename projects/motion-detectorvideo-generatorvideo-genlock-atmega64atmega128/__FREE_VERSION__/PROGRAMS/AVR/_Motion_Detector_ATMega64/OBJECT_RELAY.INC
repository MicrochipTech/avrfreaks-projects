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
;-----------------------------------------------||  	       M O T I O N  R E L A Y 		||-----------------------------------------------------
;						\\						//
;						 \\\\\\\\\\\\\\\\\\\\\\\/////////////////////////

;>--------------------------------------------------------------<
;| OBJECT     : MOTION_RELAY					|
;>--------------------------------------------------------------<
;| DESCRIPTION: This object set state of MOTION RELAY, that if	|
;|		programmed to used, will activated when motion  |
;|		in camera area occur.				|
;>--------------------------------------------------------------<

;>----------------------<
;| OBJECT DEFINITIONS	|
;>----------------------<

;-------------------------< PORT DEFINITONS >----------------------------

.EQU	_MOTION_RELAY_PORT_OUTPUT	= PORTG
.EQU	_MOTION_RELAY_PORT_DIR		= DDRG
.EQU	_MOTION_RELAY_PORT_INPUT	= PING


;-------------------------< CONSTANT DEFINITONS >------------------------

.EQU	_MOTION_RELAY_BIT		= 1
.EQU	_MOTION_RELAY_EVENT_START	= 1
.EQU	_MOTION_RELAY_EVENT_SET_ON	= 2
.EQU	_MOTION_RELAY_EVENT_SET_OFF	= 3
.EQU	_MOTION_RELAY_EVENT_LOOP	= 4
.EQU	_MOTION_RELAY_EVENT_END		= 5

;-------------------------< SRAM VARIABLES DEFINITONS >------------------------

.DSEG
	_MOTION_RELAY_EVENT:		.BYTE	_BYTE			;event
	_MOTION_RELAY_TIME_COUNT:	.BYTE	_WORD			;time counter
	_MOTION_RELAY_TIME_STEP:	.BYTE	_WORD			;time step increment in ms
	_MOTION_RELAY_TIME_ON:		.BYTE	_WORD			;time ON in ms
	_MOTION_RELAY_TIME_OFF:		.BYTE	_WORD			;time OFF in ms
	_MOTION_RELAY_REPEATS:		.BYTE	_BYTE			;repeats
.CSEG
	;>--------------------------------------------------------------<
	;| OBJECT     : MOTION_RELAY					|
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
	_MOTION_RELAY_INIT:
		push	Acc						;save useds
		in	Acc,SREG					;save SREG
		cli							;disable interrupts during initialization
		pushaw
		;@----------------------@
		;| Relay port bit	|
		;@----------------------@
		lds	Acc,_MOTION_RELAY_PORT_OUTPUT			;get old state
		andi	Acc,~(1<<_MOTION_RELAY_BIT)			;set RELAY OFF
		sts	_MOTION_RELAY_PORT_OUTPUT,Acc
		lds	Acc,_MOTION_RELAY_PORT_DIR			;get old state
		ori	Acc,(1<<_MOTION_RELAY_BIT)			;set as output
		sts	_MOTION_RELAY_PORT_DIR,Acc
		;@----------------------@
		;| Relay event		|
		;@----------------------@
		ldi	Acc,_MOTION_RELAY_EVENT_END			;set event end
		sts	_MOTION_RELAY_EVENT,Acc
		ldiaw	8						;set 8 ms (125Hz) RELAY_TIME_STEP
		stsaw	_MOTION_RELAY_TIME_STEP
		popaw							;restore SREG state
		out	SREG,Acc
		pop	Acc						;restore useds
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : MOTION_RELAY					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : set MOTION RELAY state 				|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc relay state _ON or _OFF			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_MOTION_RELAY_SET_STATE:
		push	AccH						;save useds
		in	AccH,SREG					;get SREG
		cli							;disable interrupts
		push	AccH						;save SREG
		cpi	Acc,_ON						;set ON?
		brne	_MOTION_RELAY_10				;no, branch
		lds	AccH,_MOTION_RELAY_PORT_OUTPUT			;get rele state
		ori	AccH,(1<<_MOTION_RELAY_BIT)			;set ON
	_MOTION_RELAY_00:
		sts	_MOTION_RELAY_PORT_OUTPUT,AccH			;set state
		pop	AccH						;restore SREG
		out	SREG,AccH
		pop	AccH						;restore useds	
		ret							;return
	_MOTION_RELAY_10:
		lds	AccH,_MOTION_RELAY_PORT_OUTPUT			;get rele state
		andi	AccH,~(1<<_MOTION_RELAY_BIT)			;set OFF
		rjmp	_MOTION_RELAY_00

	;>--------------------------------------------------------------<
	;| OBJECT     : MOTION_RELAY					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : set MOTION RELAY time ON			|
	;>--------------------------------------------------------------<
	;| INPUT      :	AccH:Acc time ON in ms				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_MOTION_RELAY_SET_TIME_ON:
		push	Temp						;save useds
		in	Temp,SREG					;save SREG
		cli							;disable interrupts
		stsaw	_MOTION_RELAY_TIME_ON				;set on time
	_MOTION_RELAY_SET_TIME_ON_DONE:
		out	SREG,Temp					;restore SREG
		pop	Temp
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : MOTION_RELAY					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : get MOTION RELAY time ON			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	AccH:Acc time ON in ms				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_MOTION_RELAY_GET_TIME_ON:
		push	Temp						;save useds
		in	Temp,SREG					;save SREG
		cli							;disable interrupts
		ldsaw	_MOTION_RELAY_TIME_ON				;set on time
		rjmp	_MOTION_RELAY_SET_TIME_ON_DONE

	;>--------------------------------------------------------------<
	;| OBJECT     : MOTION_RELAY					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : set MOTION RELAY time OFF			|
	;>--------------------------------------------------------------<
	;| INPUT      :	AccH:Acc time ON in ms				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_MOTION_RELAY_SET_TIME_OFF:
		push	Temp						;save useds
		in	Temp,SREG					;save SREG
		cli							;disable interrupts
		stsaw	_MOTION_RELAY_TIME_OFF				;set off time
		rjmp	_MOTION_RELAY_SET_TIME_ON_DONE
	;>--------------------------------------------------------------<
	;| OBJECT     : MOTION_RELAY					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : get MOTION RELAY time Off			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	AccH:Acc time OFF in ms				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_MOTION_RELAY_GET_TIME_OFF:
		push	Temp						;save useds
		in	Temp,SREG					;save SREG
		cli							;disable interrupts
		ldsaw	_MOTION_RELAY_TIME_OFF				;set off time
		rjmp	_MOTION_RELAY_SET_TIME_ON_DONE

	;>--------------------------------------------------------------<
	;| OBJECT     : MOTION_RELAY					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : set MOTION RELAY time step			|
	;>--------------------------------------------------------------<
	;| INPUT      :	AccH:Acc time step in ms			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_MOTION_RELAY_SET_TIME_STEP:
		push	Temp						;save useds
		in	Temp,SREG					;save SREG
		cli							;disable interrupts
		stsaw	_MOTION_RELAY_TIME_STEP				;set time step
		rjmp	_MOTION_RELAY_SET_TIME_ON_DONE
	;>--------------------------------------------------------------<
	;| OBJECT     : MOTION_RELAY					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : get MOTION RELAY time step			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	AccH:Acc time step in ms			|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_MOTION_RELAY_GET_TIME_STEP:
		push	Temp						;save useds
		in	Temp,SREG					;save SREG
		cli							;disable interrupts
		ldsaw	_MOTION_RELAY_TIME_STEP				;set time step
		rjmp	_MOTION_RELAY_SET_TIME_ON_DONE

	;>--------------------------------------------------------------<
	;| OBJECT     : MOTION_RELAY					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : set MOTION RELAY start				|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc	repeats					|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	if Acc=0 or EVENT!=EVENT END start event will   |
	;|		not issued					|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_MOTION_RELAY_SET_EVENT_START:
		cpi	Acc,0						;zero?
		brne	_MOTION_RELAY_SET_EVENT_START_10		;no, branch
		ret							;else do noting
	_MOTION_RELAY_SET_EVENT_START_10:
		push	Acc						;save
		lds	Acc,_MOTION_RELAY_EVENT				;get event
		cpi	Acc,_MOTION_RELAY_EVENT_END			;stoped ?
		breq	_MOTION_RELAY_SET_EVENT_START_20		;yes, branch
		pop	Acc						;else return
		ret
	_MOTION_RELAY_SET_EVENT_START_20:
		pop	Acc
		push	Temp						;save temp
		in	Temp,SREG					;save SREG
		cli							;disable interrupts
		sts	_MOTION_RELAY_REPEATS,Acc			;set new repeats
		push	Acc
		ldi	Acc,_MOTION_RELAY_EVENT_START			;issue start event
		sts	_MOTION_RELAY_EVENT,Acc
		pop	Acc
		rjmp	_MOTION_RELAY_SET_TIME_ON_DONE			
		
	;>--------------------------------------------------------------<
	;| OBJECT     : MOTION_RELAY					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : process MOTION RELAY events			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	This event must be called in fixed time period	|
	;|		to process occurs normally			|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_MOTION_RELAY_PROCESS_EVENT:
		push	Temp
		in	Temp,SREG					;save SREG state
		pushaw
		pushawt							;save useds
		lds	Acc,_MOTION_RELAY_EVENT				;get EVENT
		cpi	Acc,_MOTION_RELAY_EVENT_START			;EVENT START?
		brne	_MOTION_RELAY_PROCESS_EVENT_10			;no, branch
		;@----------------------@
		;| Process EVENT START	|
		;@----------------------@
		ldiaw	0						;TIME COUNT<-0
		stsaw	_MOTION_RELAY_TIME_COUNT
		ldi	Acc,_MOTION_RELAY_EVENT_SET_ON			;EVENT<-EVENT SET ON
		sts	_MOTION_RELAY_EVENT,Acc
		rjmp	_MOTION_RELAY_PROCESS_EVENT_EXIT		;exit
	_MOTION_RELAY_PROCESS_EVENT_10:
		cpi	Acc,_MOTION_RELAY_EVENT_SET_ON			;EVENT SET ON?
		brne	_MOTION_RELAY_PROCESS_EVENT_20			;no, branch
		;@----------------------@
		;| Process EVENT SET ON	|
		;@----------------------@
		ldi	Acc,_ON						;set RELAY ON
		rcall	_MOTION_RELAY_SET_STATE
		ldsaw	_MOTION_RELAY_TIME_COUNT			;TIME COUNT+=TIME STEP
		ldsawt	_MOTION_RELAY_TIME_STEP
		add	Acc,AccT
		adc	AccH,AccTH
		stsaw	_MOTION_RELAY_TIME_COUNT
		ldsawt	_MOTION_RELAY_TIME_ON				;TIME COUNT>TIME ON?
		cp	Acc,AccT
		cpc	AccH,AccTH
		brlo	_MOTION_RELAY_PROCESS_EVENT_10A			;no, branch
		ldiaw	0						;else TIME COUNT<-0
		stsaw	_MOTION_RELAY_TIME_COUNT
		ldi	Acc,_MOTION_RELAY_EVENT_SET_OFF			;EVENT<-EVENT SET OFF
		sts	_MOTION_RELAY_EVENT,Acc
	_MOTION_RELAY_PROCESS_EVENT_10A:
		rjmp	_MOTION_RELAY_PROCESS_EVENT_EXIT		;exit
	_MOTION_RELAY_PROCESS_EVENT_20:
		cpi	Acc,_MOTION_RELAY_EVENT_SET_OFF			;EVENT SET OFF?
		brne	_MOTION_RELAY_PROCESS_EVENT_30			;no, branch
		;@----------------------@
		;| Process EVENT SET OFF|
		;@----------------------@
		ldi	Acc,_OFF					;set RELAY OFF
		rcall	_MOTION_RELAY_SET_STATE
		ldsaw	_MOTION_RELAY_TIME_COUNT			;TIME COUNT+=TIME STEP
		ldsawt	_MOTION_RELAY_TIME_STEP
		add	Acc,AccT
		adc	AccH,AccTH
		stsaw	_MOTION_RELAY_TIME_COUNT
		ldsawt	_MOTION_RELAY_TIME_OFF				;TIME COUNT>TIME OFF?
		cp	Acc,AccT
		cpc	AccH,AccTH
		brlo	_MOTION_RELAY_PROCESS_EVENT_20A			;no, branch
		ldiaw	0						;else TIME COUNT<-0
		stsaw	_MOTION_RELAY_TIME_COUNT
		ldi	Acc,_MOTION_RELAY_EVENT_LOOP			;EVENT<-EVENT LOOP
		sts	_MOTION_RELAY_EVENT,Acc
	_MOTION_RELAY_PROCESS_EVENT_20A:
		rjmp	_MOTION_RELAY_PROCESS_EVENT_EXIT		;exit
	_MOTION_RELAY_PROCESS_EVENT_30:
		cpi	Acc,_MOTION_RELAY_EVENT_LOOP			;EVENT LOOP?
		brne	_MOTION_RELAY_PROCESS_EVENT_EXIT		;no, exit
		;@----------------------@
		;| Process EVENT LOOP	|
		;@----------------------@
		lds	Acc,_MOTION_RELAY_REPEATS			;repeats --
		dec	Acc
		sts	_MOTION_RELAY_REPEATS,Acc			;repeats=0?
		breq	_MOTION_RELAY_PROCESS_EVENT_30A			;yes, branch
		ldi	Acc,_MOTION_RELAY_EVENT_SET_ON			;EVENT<-EVENT SET ON 
		sts	_MOTION_RELAY_EVENT,Acc
		rjmp	_MOTION_RELAY_PROCESS_EVENT_EXIT		;exit
	_MOTION_RELAY_PROCESS_EVENT_30A:
		ldi	Acc,_MOTION_RELAY_EVENT_END			;EVENT<-EVENT END
		sts	_MOTION_RELAY_EVENT,Acc
	_MOTION_RELAY_PROCESS_EVENT_EXIT:
		popawt
		popaw
		out	SREG,Temp					;restore SREG
		pop	Temp
		ret		
		
