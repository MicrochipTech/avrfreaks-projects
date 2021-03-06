;<$DS_HEADER>
;+--------------------------------------------------------------------------------+              
;|Jo�o D�Artagnan A. Oliveira Programmer and Author; 				  |
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
;|   Bras�lia, Brazil November 30,2015 	06:28 am				  |
;+--------------------------------------------------------------------------------+
;</$DS_HEADER>
;+--------------------------------------------------------------+---+---+
;|									|
;| DATE:24/05/2001 VERSION 1.0						|
;>----------------------------------------------------------------------<
;| F O R M A T T I N G  R O U T I N E S	     				|
;| I N T E G E R, F L O A T  T O  S T R I N G  F O R M A T T I N G	|
;+----------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM			|
;+----------------------------------------------------------------------+
;+----------------------------------------------------------------------+
;| SET SIZE OF DECIMAL PLACES						|
;| Input  : Acc number of decimal places				|
;| Output : None							|
;| Destroy: None							|
;| Obs: Max size is defined by _IS_BUFFER_SIZE-1 and above value	|
;| 	assume de max size						|
;+----------------------------------------------------------------------+
_FMT_SET_DECI:
	push	AccH					;save useds
	lds	AccH,_IS_SIZE				;get old size
	andi	AccH,0xf0				;clear decimal old places
	andi	Acc,0x0f				;mask input value to range 0..15
	or	Acc,AccH
	sts	_IS_SIZE,Acc				;store new value
	pop	AccH					;restore useds
	ret
;+----------------------------------------------------------------------+
;| SET SIZE OF integer PLACES						|
;| Input  : Acc number of integer places				|
;| Output : None							|
;| Destroy: None							|
;| Obs: Max size is defined by _IS_BUFFER_SIZE-1 and above value	|
;| 	assume de max size						|
;+----------------------------------------------------------------------+
_FMT_SET_INT:
	push	AccH					;save useds
	lds	AccH,_IS_SIZE				;get old size
	andi	AccH,0x0f				;clear decimal old places
	andi	Acc,0x0f				;mask input value to range 0..15
	swap	Acc					;swap nibble to put into correctly position
	or	Acc,AccH
	sts	_IS_SIZE,Acc				;store new value
	pop	AccH					;restore useds
	ret
;+--------------------------------------------------------------+
;| SET USE PLUS SIGN SEPARATOR FOR POSITIVE NUMBERS		|
;| Input  : None						|
;| Output : None						|
;| Destroy: None						|
;+--------------------------------------------------------------+
_FMT_SET_PLUS:
	push	Acc
	lds	Acc,_DF_FMASK				;get old state
	ori	Acc,(1<<0)				;set + for positive numbers
_FMT_SET_P00:
	sts	_DF_FMASK,Acc
	pop	Acc
	ret
;+--------------------------------------------------------------+
;| SET NO USE PLUS SIGN SEPARATOR FOR POSITIVE NUMBERS		|
;| Input  : None						|
;| Output : None						|
;| Destroy: None						|
;+--------------------------------------------------------------+
_FMT_SET_NO_PLUS:
	push	Acc
	lds	Acc,_DF_FMASK				;get old state
	andi	Acc,~(1<<0)				;clear for space instead plus sign
	rjmp	_FMT_SET_P00
;+--------------------------------------------------------------+
;| SET POINT FOR DECIMAL SEPARATOR				|
;| Input  : None						|
;| Output : None						|
;| Destroy: None						|
;+--------------------------------------------------------------+
_FMT_SET_POINT:
	push	Acc
	lds	Acc,_DF_FMASK				;get old state
	ori	Acc,(1<<2)				;set for point
	rjmp	_FMT_SET_P00
;+--------------------------------------------------------------+
;| SET comma FOR DECIMAL SEPARATOR				|
;| Input  : None						|
;| Output : None						|
;| Destroy: None						|
;+--------------------------------------------------------------+
_FMT_SET_COMMA:
	push	Acc
	lds	Acc,_DF_FMASK				;get old state
	andi	Acc,~(1<<2)				;clear for comma
	rjmp	_FMT_SET_P00
;+--------------------------------------------------------------+
;| SET round result 						|
;| Input  : None						|
;| Output : None						|
;| Destroy: None						|
;+--------------------------------------------------------------+
_FMT_SET_ROUND:
	push	Acc
	lds	Acc,_DF_FMASK				;get old state
	ori	Acc,(1<<3)				;set for round
	rjmp	_FMT_SET_P00
;+--------------------------------------------------------------+
;| SET not round result 					|
;| Input  : None						|
;| Output : None						|
;| Destroy: None						|
;+--------------------------------------------------------------+
_FMT_SET_NO_ROUND:
	push	Acc
	lds	Acc,_DF_FMASK				;get old state
	andi	Acc,~(1<<3)				;set for round
	rjmp	_FMT_SET_P00

