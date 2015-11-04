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
;| DATE:24/05/2001 VERSION 1.0							|
;>--------------------------------------------------------------+---------------<
;| D O U B L E   F L O A T  P O I N T  R O U T I N E S		|
;| S Q U A R E  R O O T  					|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+
;+----------------------------------------------------------------------+	
;| DOUBLE FLOAT POINT SQUARE ROOT 					|
;|									|
;| Input  : Float Acc X							|
;| Output : Float Acc Square Root of X,Acc exception code		|
;| Destroy: Flags,AccH							|
;| Method : Newton/Rapton Approximation					|
;| Description of Method						|
;|									|
;|	Newtow/Rapton approximation algorithm follow below		|
;|	arithmetics.							|
;|									|
;|	X0=X	assume that XO is my first aproximation			|
;|									|
;|	then the next approximation is performed according below	|
;|	operation 							|
;|									|
;|	Xn+1=(X/Xn+Xn)/2						|
;|									|
;|	repeat above operation until Xn+1=Xn, in that instant		|
;|	Xn+1 contents is a square root of X				|
;|									|
;|	others considerations 						|
;|									|
;|	the number X is store into memory using the base 2 		|
;|	formatting that follow.						|
;|									|
;|	      Ex							|
;|	X=Mx.2	  Mx=Mantisa of X 					|
;|		  Ex=Exponent of X base 2				|
;|									|
;|	this well-known, the below simplification occurs		|
;|									|
;|	 ----     --------     ----   -----				|
;|	 |	  |    Ex      |      | Ex				|
;|	\| X  => \|Mx.2    => \|Mx  .\|2				|
;|									|
;|	when Ex is even then 						|
;|									|
;|	 ----     ---       						|
;|	 |	  |     Ex/2						|
;|	\| X  => \|Mx .2  						|
;|									|
;|	when Ex is odd then 						|
;|									|
;|	 ----     ---             ---					|
;|	 |	  |     (Ex-1)/2  |					|
;|	\| X  => \|Mx .2  	 \|2					|
;|									|
;|	How the base is 2 de maximum value for de Mx is 2, the		|
;|	number of interration of Newton/Rapton aproximation is		|
;|	6 for a 16 decimal digits number found by experimental		|
;|	form.								|
;+----------------------------------------------------------------------+
#ifndef __MATH_FLOAT_SQUARE_ROOT__
	#define __MATH_FLOAT_SQUARE_ROOT__
	.message "[ (MATH) FLOAT SQUARE ROOT ]"
	;+----------------------+
	;| NECESSARY INCLUDES	|
	;+----------------------+
	#ifndef __MATH_FLOAT_INFINIT__
		.INCLUDE	"MATH\FLOAT_DOUBLE\INFINIT_RESULT\DFINF.INC"
	#endif
	#ifndef __MATH_FLOAT_NORMALIZE__
		.INCLUDE	"MATH\FLOAT_DOUBLE\NORMALIZE\DFNORM.INC"
	#endif
	#ifndef __MATH_FLOAT_ADDSUB__
		.INCLUDE	"MATH\FLOAT_DOUBLE\ADDSUB\DFADDSUB.INC"
	#endif
	#ifndef __MATH_FLOAT_MULDIV__
		.INCLUDE	"MATH\FLOAT_DOUBLE\MULDIV\DFMULDIV.INC"
	#endif
	#ifndef __MATH_FLOAT_LOAD_STORE__
		.INCLUDE	"MATH\FLOAT_DOUBLE\LOAD_STORE\DFLDXSTX.INC"
	#endif
	
	_DFSQRT:
		tst	_acc_s			;number is negative ?
		brpl	_DFSQRT00		;no, continue
		ldi	Acc,_FEILLEG		;else set illegal function call
		ret
	_DFSQRT00:
		push	ZL
		push	ZH
		ldi	ZL,low(_DF_BCD)		;DF_BCD=x
		ldi	ZH,high(_DF_BCD)
		rcall	_DFSTZ
		subi	_acc_e,_FBIAS		;extract float bias
		push	_acc_e			;save exponent parity
		ldi	_acc_e,_FBIAS		;only mantisa to extract square root
		_DFSTACC	_DF_P10		;p10=X
		_DFSTACC	_DF_MAC		;MAC=X
		ldi	Acc,6			;number of loops
	_DFSQRT30:
		push	Acc
		_DFLDOP1	_DF_P10		;op1=p10
		_DFLDOP2	_DF_MAC		;op2=mac
		_DFMDIV
		_DFMOVACCOP1
		_DFLDOP2	_DF_MAC
		_DFMADD				;acc=p10/mac+mac
		_DFMDIV2			;acc=(p10/mac+mac)/2
		_DFSTACC	_DF_MAC		;mac=(p10/mac+mac)/2
		pop	Acc
		dec	Acc
		brne	_DFSQRT30
		pop	Acc			;se if exponent is odd
		bst	Acc,0
		brtc	_DFSQRT40		;no, branch
		dec	Acc			;Ex=Ex-1
		asr	Acc			;Ex=Ex/2
		subi	Acc,-_FBIAS
		sts	_DF_MAC+7,Acc
		rcall	_DFLDSQRT2		;acc=sqr(2)
		_DFMOVACCOP1			;op1=acc
		_DFLDOP2	_DF_MAC
		_DFMMUL				;acc=x*sqr(2)
		rjmp	_DFSQRT50
	_DFSQRT40:
		asr	Acc
		subi	Acc,-_FBIAS
		sts	_DF_MAC+7,Acc
		_DFLDACC	_DF_MAC
	_DFSQRT50:
		ldi	Acc,_FPOK
		pop	ZH
		pop	ZL
		ret
#endif
.exit

