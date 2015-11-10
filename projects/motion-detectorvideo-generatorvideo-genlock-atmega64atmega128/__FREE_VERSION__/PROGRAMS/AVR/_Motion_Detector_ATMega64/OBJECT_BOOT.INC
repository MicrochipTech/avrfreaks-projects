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
;-----------------------------------------------||  	       	     B O O T   			||-----------------------------------------------------
;						\\						//
;						 \\\\\\\\\\\\\\\\\\\\\\\/////////////////////////

;>--------------------------------------------------------------<
;| OBJECT     : BOOT						|
;>--------------------------------------------------------------<
;| DESCRIPTION: This object check boot state if HOT or COLD	|
;>--------------------------------------------------------------<

;>----------------------<
;| OBJECT DEFINITIONS	|
;>----------------------<


;-------------------------< PORT DEFINITONS >----------------------------


;-------------------------< CONSTANT DEFINITONS >------------------------

.EQU	_BOOT_SIGNATURE_1	= 27			;signature for hot boot byte 1
.EQU	_BOOT_SIGNATURE_2	= 18			;signature for hot boot byte 2
.EQU	_BOOT_SIGNATURE_3	= 28			;signature for hot boot byte 3
.EQU	_BOOT_RTC_SIGNATURE_OFF	= 0X5A			;signature for hot boot byte 4

;-------------------------< SRAM  DEFINITONS >------------------------

.DSEG
	_BOOT_SIGNATURE_BYTE_1:	.BYTE	_BYTE		;if hot boot contents = 27
	_BOOT_SIGNATURE_BYTE_2:	.BYTE	_BYTE		;if hot boot contents = 18
	_BOOT_SIGNATURE_BYTE_3:	.BYTE	_BYTE		;if hot boot contents = 28
	_BOOT_RTC_STATUS:	.BYTE	_BYTE		;if not set contents = 0x5a
.CSEG

	;>--------------------------------------------------------------<
	;| OBJECT     : BOOT						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : BOOT_STATE					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: CHECK BOOT STATE COLD OR HOT			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Acc  _ON=HOT _OFF=COLD				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_BOOT_GET_STATE:
		lds	Acc,_BOOT_SIGNATURE_BYTE_1			;get signature byte 1
		cpi	Acc,_BOOT_SIGNATURE_1				;math ?	
		brne	_BOOT_GET_STATE_10				;no, branch
		lds	Acc,_BOOT_SIGNATURE_BYTE_2			;get signature byte 2
		cpi	Acc,_BOOT_SIGNATURE_2				;math ?	
		brne	_BOOT_GET_STATE_10				;no, branch
		lds	Acc,_BOOT_SIGNATURE_BYTE_3			;get signature byte 3
		cpi	Acc,_BOOT_SIGNATURE_3				;math ?	
		brne	_BOOT_GET_STATE_10				;no, branch
		;/////////////////
		;// HOT BOOT
		;/////////////////
		ldi	Acc,_ON
		ret
		;/////////////////
		;// COLD BOOT
		;/////////////////
	_BOOT_GET_STATE_10:
		ldi	Acc,_BOOT_SIGNATURE_1				;set signature bytes
		sts	_BOOT_SIGNATURE_BYTE_1,Acc
		ldi	Acc,_BOOT_SIGNATURE_2
		sts	_BOOT_SIGNATURE_BYTE_2,Acc
		ldi	Acc,_BOOT_SIGNATURE_3
		sts	_BOOT_SIGNATURE_BYTE_3,Acc
		ldi	Acc,_BOOT_RTC_SIGNATURE_OFF			;set clock not set
		sts	_BOOT_RTC_STATUS,Acc
		ldi	Acc,_OFF
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : BOOT						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : BOOT_GET_RTC_STATE				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: CHECK RTC STATE SET OR NOT			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Acc  _ON=if set _OFF=not set			|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_BOOT_GET_RTC_STATE:
		lds	Acc,_BOOT_RTC_STATUS				;get RTC STATUS
		cpi	Acc,_BOOT_RTC_SIGNATURE_OFF			;not set?
		ldi	Acc,_OFF					;Assume yes
		breq	PC+2						;branch if yes
		ldi	Acc,_ON						;else ON
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : BOOT						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : BOOT_SET_RTC_OK					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: SET RTC STATE TO OK				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None			                        |
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_BOOT_SET_RTC_OK:
		push	Acc
		ldi	Acc,0
		sts	_BOOT_RTC_STATUS,Acc
		pop	Acc
		ret
