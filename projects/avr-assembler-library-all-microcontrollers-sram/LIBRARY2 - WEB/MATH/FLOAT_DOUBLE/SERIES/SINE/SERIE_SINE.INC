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
;| P A R T I A L  S I N E   					|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+
;+----------------------------------------------------------------------+	
;| DOUBLE FLOAT POINT PARTIAL SINE 					|
;|									|
;| Input  : Float Acc X	value must in range -pi/2 to pi/2		|
;| Output : Float Acc sine of X,Acc Exception code			|
;| Destroy: Flags							|
;| Method : use Maclaurin serie						|
;| 									|
;|									|
;| Below a Maclaurin series to calculate sine function, where x expres-	|
;| sed in radians varying of -pi/2 to +pi/2. values out of this range	|
;| lost precision.							|
;|									|
;| original maclaurin serie for sine function				|
;|									|
;|		         3     5     7     9     11			|
;|	       	        x     x     x     x     x			|
;|	(1) sin x = x - --- + --- - --- + --- - --- ...			|
;|		        3!    5!    7!     9!   11!			|
;|									|
;| but because speed reason the equation (1) is rewriten as following	|
;|			  						|
;|			   2  1     2  1     2  1     2  1     2	|
;|	(2) sin x = x(1 - x (--- - x (--- - x (--- - x (--- - x (....	|
;|			      3!       5!       7!       9!		|
;|									|
;| using equation (2) reduced the number of computations because all    |
;| exponents of x is power of 2 except first x and for each serie term	|
;| only a multiply and a sun is need.					|
;|									|
;| the number of terms to obtain a precision of 19 digits are 12 and 	|
;| inverse factorial constants are store in memory.			|
;|									|
;+----------------------------------------------------------------------+
#ifndef __MATH_FLOAT_SINE_SERIE__
	#define __MATH_FLOAT_SINE_SERIE__
	.message "[ (MATH) FLOAT SINE SERIE ]"
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
	#ifndef __MATH_FLOAT_UNARY__
		.INCLUDE	"MATH\FLOAT_DOUBLE\UNARY\UNARY.INC"
	#endif
	#ifndef __MATH_FLOAT_LOAD_STORE__
		.INCLUDE	"MATH\FLOAT_DOUBLE\LOAD_STORE\DFLDXSTX.INC"
	#endif
;	.EQU	_DFMACS_ADDR_OP2_0	= 8	;address of op2_0 ; must be altered if op2_0 definitions change
	_DFSERSIN:
		push	ZL					;save Z
		push	ZH
		push	YL					;save Y
		push	YH
		rcall	_DFACCOP1				;mov op1=op2=X
		rcall	_DFACCOP2
		push	_acc_0					;save X
		push	_acc_1
		push	_acc_2
		push	_acc_3
		push	_acc_4
		push	_acc_5
		push	_acc_s
		push	_acc_e
		rcall	_DFMUL					;calcute X^2
		ldi	Acc,0x80				;change X^2 signal
		eor	_acc_s,Acc
		ldi	ZL,low(_DF_MAC)
		ldi	ZH,high(_DF_MAC)
		rcall	_DFSTZ					;store into DF_MAC=-X^2
		ldi	ZL,low(_DFSERS_FACTORIAL_TABLE*2)	;get first parm
		ldi	ZH,high(_DFSERS_FACTORIAL_TABLE*2)
		ldi	YL,low(_DF_P10)				;DF_P10=S=First constant
		ldi	YH,high(_DF_P10)
		rcall	_DFSERS_GETCONSTANT			;get constant
		ldi	Acc,11					;number of constants
	_DFSERS_LOOP:
		push	Acc					;save constant counter
		push	ZL					;save constant pointer
		push	ZH
		ldi	ZL,low(_DF_P10)				;Z-->DF_P10=S
		ldi	ZH,high(_DF_P10)
		rcall	_DFLDOP1Z				;op1=S
		ldi	ZL,low(_DF_MAC)				;Z-->DF_MAC=X^2
		ldi	ZH,high(_DF_MAC)
		rcall	_DFLDOP2Z				;op2=X^2
		rcall	_DFMUL					;S*X^2
		pop	ZH					;restore constant pointer
		pop	ZL
;======================================================================================
; below remark code is removed at date 16/6/2008 5:33 for speed and security of the code
;
;		ldi	YL,low(_DFMACS_ADDR_OP2_0)		;get FAT(acc)
;		ldi	YH,high(_DFMACS_ADDR_OP2_0)
;		rcall	_DFSERS_GETCONSTANT			;op2=constant
;======================================================================================
; then code below is a replaced code 
		lpm
		adiw	ZL,1
		mov	_op2_0,r0
		lpm
		adiw	ZL,1
		mov	_op2_1,r0
		lpm
		adiw	ZL,1
		mov	_op2_2,r0
		lpm
		adiw	ZL,1
		mov	_op2_3,r0
		lpm
		adiw	ZL,1
		mov	_op2_4,r0
		lpm
		adiw	ZL,1
		mov	_op2_5,r0
		lpm
		adiw	ZL,1
		mov	_op2_s,r0
		lpm
		adiw	ZL,1
		mov	_op2_e,r0
;=======================================================================================
		rcall	_DFACCOP1				;op1=S*X^2
		rcall	_DFADD					;S*X^2+FAT(Acc)
		push	ZL					;save constant pointer again
		push	ZH
		ldi	ZL,low(_DF_P10)				;Z-->DF_P10=S
		ldi	ZH,high(_DF_P10)
		rcall	_DFSTZ					;S=S*X^2+FAT(Acc)
		pop	ZH					;restore constant pointer
		pop	ZL
		pop	Acc					;restore constant counter
		dec	Acc
		brne	_DFSERS_LOOP				;until all done
		ldi	ZL,low(_DF_P10)				;Z-->DF_P10=S
		ldi	ZH,high(_DF_P10)
		rcall	_DFLDOP1Z				;op1=S
		pop	_op2_e					;op2=X
		pop	_op2_s
		pop	_op2_5
		pop	_op2_4
		pop	_op2_3
		pop	_op2_2
		pop	_op2_1
		pop	_op2_0
		rcall	_DFMUL					;Facc=S*X
		ldi	Acc,_FPOK
		pop	YH					;restore Y
		pop	YL
		pop	ZH					;restore Z
		pop	ZL
		ret
	
	;+----------------------------------------------+
	;| GET CONSTANT POINTED BY REGISTER Z AND STORE |
	;| INTO SRAM POINTED BY REGISTER Y		|
	;+----------------------------------------------+
	_DFSERS_GETCONSTANT:
		ldi	Acc,8					;byte counter
	_DFSERS_G00:
		lpm						;get data from constant data into flash
		st	Y+,r0					;store into sram
		adiw	ZL,1
		dec	Acc					;next data
		brne	_DFSERS_G00				;until done
		ret
	
	;+----------------------------------------------+
	;| DOUBLE PRECISION INVERSE FACTORIAL TABLE	|
	;+----------------------------------------------+
	_DFSERS_FACTORIAL_TABLE:
	;
	;	     lsb        mantisa           msb   exp
	;.........../---------------------------------\/--\
		.DB 0xcc,0xc0,0xb1,0x98,0xa0,0x0d,0x3b,0x36 	;1/23! = 3,8681701706306840377169119315228e-23
		.DB 0x8c,0xab,0xe7,0xb6,0x77,0xdc,0x38,0x3f 	;1/21! = 1,9572941063391261230847574373505e-20
		.DB 0xb9,0x0a,0x0a,0x34,0xda,0xa4,0x17,0x48 	;1/19! = 8,2206352466243297169559812368723e-18
		.DB 0x53,0x6a,0x85,0x81,0x3b,0x96,0x4a,0x50 	;1/17! = 2,8114572543455207631989455830103e-15
		.DB 0xf8,0xc0,0x9d,0x39,0x9f,0x3f,0x57,0x58 	;1/15! = 7,6471637318198164759011319857881e-13
		.DB 0x4b,0x68,0x43,0x9d,0x30,0x92,0x30,0x60 	;1/13! = 1,6059043836821614599392377170155e-10
		.DB 0x1c,0x27,0xaa,0x3f,0x2b,0x32,0x57,0x67 	;1/11! = 2,5052108385441718775052108385442e-8
		.DB 0x9c,0x39,0xb6,0x2a,0x1d,0xef,0x38,0x6e 	;1/9!  = 2,7557319223985890652557319223986e-6
		.DB 0xd0,0x00,0x0d,0xd0,0x00,0x0d,0x50,0x74 	;1/7!  = 0,0001984126984126984126984126984127
		.DB 0x88,0x88,0x88,0x88,0x88,0x88,0x08,0x7a 	;1/5!  = 0,0083333333333333333333333333333333
		.DB 0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x2a,0x7e 	;1/3!  = 0,16666666666666666666666666666667
		.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x81	;1/1!  = 1
	
#endif
.exit
	
	
	
	
	
	
	
	
	
	

