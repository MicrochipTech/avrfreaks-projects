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
#ifndef	__STRCOLL__
	
	#define	__STRCOLL__
	;>----------------------<
	;| NEED INCLUDES	|
	;>----------------------<
	;#ifndef	__NEED_INCLUDE__
	;#endif
	
	.message "+---------------------+"
	.message "|                     |"
	.message "| STRCMP C Function	|"
	.message "|                     |"
	.message "+---------------------+"
	;>--------------------------------------------------------------<
	;| OBJECT     : _STRCOLL 					|
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
	;| OBJECT     : _STRCOLL					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : STRCOLL Compare two strings considering the case|
	;>--------------------------------------------------------------<
	;| INPUT      : X as input string 1				|
	;|		Y as input string 2				|
	;| OUTPUT     :	Acc as comare output				|
	;| DESTROYED  :	----						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	                           			|
	;|		                  				|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_STRCOLL:
		
		pushawt							;Save Useds
		pushw	X						;
		pushw	Y						;
	_STRCOLL_10:
		
		ld	AccT,X+						;Load X to AccT
		ld	AccTH,Y+					;Load Y to AccTH
		mov	Acc,AccT						;
		sub	Acc,AccTH					;Char match?
		brne	_STRCOLL_20					;Yes, Jump
		
		tst	AccT						;End of string 1
		brne	_STRCOLL_10					;Yes jump
		
		ldi	Acc,0						;Consider str1 = str2
		
		rjmp	_STRCOLL_EXIT					;Jump
	_STRCOLL_20:
		cp	AccT,AccTH					;Acc < AccH?
		brlo	_STRCOLL_30					;Yes, Jump
		ldi	Acc,1						;No, Acc > AccH
		rjmp	_STRCOLL_EXIT					;Exit
	_STRCOLL_30:
		ldi	Acc,-1						;Acc < AccH
	_STRCOLL_EXIT:
		popw	Y						;Restore Useds
		popw	X						;
		popawt							;
		ret							;End
	
#endif
.EXIT

