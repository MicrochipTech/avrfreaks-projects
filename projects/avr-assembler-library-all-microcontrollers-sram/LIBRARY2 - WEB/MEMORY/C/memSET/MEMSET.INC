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
#ifndef	__MEMSET__
	
	#define	__MEMSET__
	
	;>----------------------<
	;| NEED INCLUDES	|
	;>----------------------<
	;#ifndef	__NEED_INCLUDE__
	;#endif
	
	.message "+---------------------+"
	.message "|                     |"
	.message "|        MEMSET	|"
	.message "|                     |"
	.message "+---------------------+"
	;>--------------------------------------------------------------<
	;| OBJECT     : _LIBPADRAO 					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: This object implements some functions(methods)	|
	;|		do handdle LIBPADRAO Function			|
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
	;| OBJECT     : _MEMSET						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : _MEMSET Sets num of bytes in memory pointed 	|
	;|		with specified value.				|
	;>--------------------------------------------------------------<
	;| INPUT      : X --> memory block				|
	;|		Acc  bYTE pattern to fill			|
	;|		Y    number of chars				|
	;| OUTPUT     :	----						|
	;| DESTROYED  :	----						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_MEMSET:	
		pushw	X					;
		pushw	Y					;
		adiw	YH:YL,1					;Y++
	_MEMSET_10:
		sbiw	YH:YL,1					;Y-- last byte filled?
		breq	_MEMSET_20				;yes, branch
		st	X+,Acc					;else fill new position.
		rjmp	_MEMSET_10				;again
	_MEMSET_20:
		popw	Y					;Restore Useds
		popw	X					;
		ret						;End
	
#endif
.EXIT

