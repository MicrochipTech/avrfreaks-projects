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
;+--------------------------------------------------------------+---+-----------+
;|										|
;| DATE:17/12/2001 VERSION 1.0							|
;>------------------------------------------------------------------------------<
;| I N T E G E R  R O U T I N E S	     					|
;| U N S I G N E D  L O N G I N T E G E R  T O  H E X A  S T R I N G 		|
;+------------------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM				|
;+------------------------------------------------------------------------------+
;+----------------------------------------------------------------------+
;| _IPTOS								|
;| Convert IP number into Y:X register to string ASCII ip formated	|
;| with 0 terminated							|
;|									|
;| Input  : Y:X IP number 						|
;| Output : Z--> SRAM string output with 0			  	|
;+----------------------------------------------------------------------+
#ifndef	__INTEGER_IPTOS__
	#define __INTEGER_IPTOS__
	.message "[ (MATH) INTEGER IPTOS ]"
	;+----------------------+
	;| NECESSARY INCLUDES	|
	;+----------------------+
	#ifndef	__INTEGER_UBTOS__
		.INCLUDE	"MATH\INTEGER\UBTOS\UBTOS.INC"
	#endif
	_IPTOS:
		push	Acc
		ldi	ZL,low(_DF_STR)				;output area
		ldi	ZH,high(_DF_STR)
		mov	Acc,YL					;most significant value fist
		rcall	_UBTOS					;convert
		pop	Acc
		ret
#endif
.exit
	

