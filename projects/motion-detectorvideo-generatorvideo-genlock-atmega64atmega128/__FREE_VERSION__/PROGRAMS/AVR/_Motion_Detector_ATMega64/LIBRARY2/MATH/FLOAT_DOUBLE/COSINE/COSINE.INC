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
;| DATE:23/01/2003 VERSION 1.0							|
;>--------------------------------------------------------------+---------------<
;| D O U B L E   F L O A T  P O I N T  R O U T I N E S		|
;| G E N E R A L  C O - S I N E 				|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+
;+----------------------------------------------------------------------+	
;| DOUBLE FLOAT POINT CO-SINE 						|
;|									|
;| Input  : Float Acc X	precision loss if x>+/-1e16			|
;| Output : Float Acc co-sine of X,Acc Exception code			|
;| Destroy: Flags							|
;| Method : COS(X)=SIN(X+PI/2)						|
;| 									|
;+----------------------------------------------------------------------+
#ifndef __MATH_FLOAT_COSINE__
	#define __MATH_FLOAT_COSINE__
	.message "[ (MATH) FLOAT COSINE ]"
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
	#ifndef __MATH_FLOAT_COMPARE__
		.INCLUDE	"MATH\FLOAT_DOUBLE\COMPARE\DFCPM.INC"
	#endif
	#ifndef __MATH_FLOAT_SINE_SERIE__
		.INCLUDE	"MATH\FLOAT_DOUBLE\SERIES\SINE\SERIE_SINE.INC"
	#endif
	#ifndef __MATH_FLOAT_SINE__
		.INCLUDE	"MATH\FLOAT_DOUBLE\SINE\SINE.INC"
	#endif
	_DFCOS:
		rcall	_DFACCOP1			;op1=acc
		rcall	_DFLDPI2			;acc=pi/2
		rcall	_DFACCOP2			;op2=pi/2
		rcall	_DFADD				;acc=arg+pi/2
		rcall	_DFSIN				;compute sine function
		ret					;end
#endif
.exit

