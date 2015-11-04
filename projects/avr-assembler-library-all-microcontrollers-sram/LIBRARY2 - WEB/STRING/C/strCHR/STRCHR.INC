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
;| BY VICTOR HUGO DE OLIVEIRA CAMPOS			|
;| SUPERVISION BY JOÃO DARTAGNAN ANTUNES OLIVEIRA	|
;| DATA:4/12/2005					|
;>------------------------------------------------------<
;| A V R  S T R L E N  C  F U N C T I O N		|
;| V E R S I O N  1.0					|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH MEGA AVRS 		|
;+------------------------------------------------------+
#ifndef	__STRCHR__
	
	#define	__STRCHR__
	;>----------------------<
	;| NEED INCLUDES	|
	;>----------------------<
	;#ifndef	__NEED_INCLUDE__
	;#endif
	
	.message "+---------------------+"
	.message "|                     |"
	.message "| STRCHR C Function	|"
	.message "|                     |"
	.message "+---------------------+"
	;>--------------------------------------------------------------<
	;| OBJECT     : _STRCHR 					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: This object implements some functions(methods)	|
	;|		do handdle STRCHR C Function			|
	;>--------------------------------------------------------------<
	;>----------------------<
	;| OBJECT DEFINITIONS	|
	;>----------------------<
	;-------------------------< CONSTANT DEFINITONS >------------------------
	;-------------------------< SRAM VARIABLES >-----------------------------
	.DSEG
	.CSEG
	;
	;-------------------------< M A C R O S  >-------------------------------
	;
	;>--------------------------------------------------------------<
	;| OBJECT     : _STRCHR						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : STRCHR Looks for first occurance of a character	|
	;>--------------------------------------------------------------<
	;| INPUT      : X-->Input string				|
	;|		Acc as char to find				|
	;| OUTPUT     :	X-->char if found				|
	;|		cy=1 if not found				|
	;| DESTROYED  :	----						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_STRCHR:
		push	Temp
	_STRCHR_10:
		ld	Temp,X+					;get char
		tst	Temp					;end char '\0'
		breq	_STRCHR_30				;yes, exit
		cp	Acc,Temp				;found ?
		brne	_STRCHR_10
		sbiw	XH:XL,1					;point to correct position
		clr	Acc					;set found
	_STRCHR_20:
		pop	Temp
		ret
	_STRCHR_30:
		sec						;set not found
		rjmp	_STRCHR_20				;*<NO CHANGE>*
	
#endif
.EXIT

