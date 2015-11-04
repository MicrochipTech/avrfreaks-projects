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
;| U N S I G N E D  B Y T E  T O  H E X A  S T R I N G 				|
;+------------------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM				|
;+------------------------------------------------------------------------------+
;+----------------------------------------------------------------------+
;| _UBTOH								|
;| Convert unsigned byte to hex string ASCII with fixed size 2 chars and|
;|								  	|
;| Input  : Acc unsigned byte input				  	|
;|	    Z--> SRAM string reserved area when in multitask mode	|
;| Output : Z--> SRAM string output with 0			  	|
;+----------------------------------------------------------------------+
#ifndef	__INTEGER_UBTOH__
	#define __INTEGER_UBTOH__
	.message "[ (MATH) INTEGER UBTOH ]"
	;+----------------------+
	;| NECESSARY INCLUDES	|
	;+----------------------+
	#ifndef	__INTEGER_DUMPHEX__
		.INCLUDE	"MATH\INTEGER\DUMPHEX\DUMPHEX.INC"
	#endif
	_UBTOH:
		push	Acc
		#ifdef	__MULTITASK_MODE__
			pushw	Z
		#else
			ldiw	Z,_DF_STR			;output area
		#endif
		rcall	_DUMPHEX				;convert to hex
		ldi	Acc,0					;zero terminate
		st	Z+,Acc
		#ifdef	__MULTITASK_MODE__
			popw	Z
		#else
			ldiw	Z,_DF_STR			;output area
		#endif
		pop	Acc
		ret
#endif
.exit
	

