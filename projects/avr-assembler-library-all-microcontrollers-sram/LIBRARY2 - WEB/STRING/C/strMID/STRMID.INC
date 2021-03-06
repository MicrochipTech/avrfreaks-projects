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
;+--------------------------------------------------------------+---+
;| BY VICTOR HUGO DE OLIVEIRA CAMPOS			|
;| SUPERVISION BY JO�O DARTAGNAN ANTUNES OLIVEIRA	|
;| DATA:4/12/2005					|
;>------------------------------------------------------<
;| A V R  S T R L E N  C  F U N C T I O N		|
;| V E R S I O N  1.0					|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH MEGA AVRS 		|
;+------------------------------------------------------+
#ifndef	__STRMID__
	
	#define	__STRMID__
	;>----------------------<
	;| NEED INCLUDES	|
	;>----------------------<
	;#ifndef	__NEED_INCLUDE__
	;#endif
	
	.message "+---------------------+"
	.message "|                     |"
	.message "| STRMID C Function	|"
	.message "|                     |"
	.message "+---------------------+"
	;>--------------------------------------------------------------<
	;| OBJECT     : _STRMID 					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: This object implements some functions(methods)	|
	;|		do handdle STRMID Function			|
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
	;| OBJECT     : _STRMID						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : STRMID extracts a substring from a string    	|
	;|		expression.					|
	;>--------------------------------------------------------------<
	;| INPUT      : X as destination string 1			|
	;|		Y as input string 2				|
	;|		Z as number of first char			|
	;|		AccH:Acc as number os chars to get		|
	;| OUTPUT     :	X as new string					|
	;| DESTROYED  :	----						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_STRMID:
		
		pushtempw						;Save Useds
		pushw	X						;
		pushw	Y						;
		
		addw	Y,Z						;Y = Y + Z
	_STRMID_10:
		
		ld	Temp,Y+						;Load Y to AccTH
		tst	Temp						;End of string 2
		breq	_STRMID_EXIT					;Yes jump
		
		st	X+,Temp						;No, store new value
		
		subiaw	1						;End of number of chars?
		brne	_STRMID_10					;No, Jump
		
	_STRMID_EXIT:
		ldi	Temp,0						;\0 zero end
		st	X+,Temp						;		
		popw	Y						;Restore Useds
		popw	X						;
		poptempw							;
		ret							;End
	
#endif
.EXIT

