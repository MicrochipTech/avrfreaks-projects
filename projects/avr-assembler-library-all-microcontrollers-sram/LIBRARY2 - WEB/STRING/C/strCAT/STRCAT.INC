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
#ifndef	__STRCAT__
	
	#define	__STRCAT__
	;>----------------------<
	;| NEED INCLUDES	|
	;>----------------------<
	;#ifndef	__NEED_INCLUDE__
	;#endif
	
	.message "+---------------------+"
	.message "|                     |"
	.message "| STRCAT C Function	|"
	.message "|                     |"
	.message "+---------------------+"
	;>--------------------------------------------------------------<
	;| OBJECT     : _STRCAT 					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: This object implements some functions(methods)	|
	;|		do handdle STRCAT C Function			|
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
	;| OBJECT     : _STRCAT						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : StrCat Concatenate two strings in one.		|
	;>--------------------------------------------------------------<
	;| INPUT      : X as input string 1				|
	;|		Y as input string 2				|
	;| OUTPUT     :	X as new string					|
	;| DESTROYED  :	----						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	String1=String1+String2+'\0'			|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_STRCAT:
		
		push 	Temp						;Save Temp
		pushw	X
	
	_STRCAT_10:
		ld	Temp,X+						;Load X to Temp
		cpi	Temp,0						;End of string1?
		brne	_STRCAT_10					;No, jump
		sbiw	XH:XL,1						;X--
	_STRCAT_20:
		ld	Temp,Y+						;Load Y to Temp
		st	X+,Temp						;Add new char to X
		cpi	Temp,0						;End of string2?
		brne	_STRCAT_20					;No, jump
		popw	X
		pop	Temp						;Restore Temp
		ret							;End
	
#endif
.EXIT

