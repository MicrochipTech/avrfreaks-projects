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
;| D O U B L E  F L O A T  P O I N T  R O U T I N E S		|
;| F L O A T  L O A D  A N D  S T O R E 			|
;+--------------------------------------------------------------+
#ifndef __MATH_FLOAT_LOAD_STORE__
	#define __MATH_FLOAT_LOAD_STORE__
	.message "[ (MATH) FLOAT LOAD & STORE ]"
	;+----------------------+
	;| NECESSARY INCLUDES	|
	;+----------------------+
	#ifndef __MATH_FLOAT_INFINIT__
		.INCLUDE	"MATH\FLOAT_DOUBLE\INFINIT_RESULT\DFINF.INC"
	#endif
	;+--------------------------------------+
	;| LOAD ACCUMULADOR WITH ZERO	   	|
	;|				   	|
	;| Input  : None			|
	;| Output : Facc Zero,Acc FPOK		|
	;| Destroy: 				|
	;+--------------------------------------+
	_DFLD0:
		rcall	_DFUNDER00			;clear facc
	_DFLDFPOK:
		ldi	Acc,_FPOK			;set operation ok
		ret
	;+--------------------------------------+
	;| LOAD ACCUMULADOR WITH ONE	   	|
	;|				   	|
	;| Input  : None			|
	;| Output : Facc One,Acc FPOK		|
	;| Destroy: 				|
	;+--------------------------------------+
	_DFLD1:
		rcall	_DFUNDER00			;clear Facc
		ldi	_acc_e,0x81			;exponent of 1
		rjmp	_DFLDFPOK
	;+--------------------------------------+
	;| LOAD ACCUMULADOR WITH TWO	   	|
	;|					|
	;| Input  : None			|
	;| Output : Facc TWO,Acc FPOK		|
	;| Destroy: 				|
	;+--------------------------------------+
	_DFLD2:
		rcall	_DFUNDER00			;clear Facc
		ldi	_acc_e,0x82			;exponent of 2
		rjmp	_DFLDFPOK
	;+--------------------------------------+
	;| LOAD ACCUMULADOR WITH TEN	   	|
	;|				   	|
	;| Input  : None			|
	;| Output : Facc TEN,Acc FPOK		|
	;| Destroy: 				|
	;+--------------------------------------+
	_DFLD10:
		rcall	_DFUNDER00			;clear Facc
		ldi	_acc_s,0x20			;mantisa of 10
		ldi	_acc_e,0x84			;exponent of 10
		rjmp	_DFLDFPOK
	;+----------------------------------------------+
	;| LOAD ACCUMULADOR WITH e=2,71828182845904524 	|
	;|				   		|
	;| Input  : None				|
	;| Output : Facc e,Acc FPOK			|
	;| Destroy: 					|
	;+----------------------------------------------+
	_DFLDE:
		ldi	_acc_0,0x4a
		ldi	_acc_1,0xbb
		ldi	_acc_2,0xa2
		ldi	_acc_3,0x58
		ldi	_acc_4,0x54
		ldi	_acc_5,0xf8
		ldi	_acc_s,0x2d
		ldi	_acc_e,0x82
		rjmp	_DFLDFPOK
	;+----------------------------------------------+
	;| LOAD ACCUMULADOR WITH sqrt(2)=1,414213562373 |
	;|				   		|
	;| Input  : None				|
	;| Output : Facc sqrt(2),Acc FPOK		|
	;| Destroy: 					|
	;+----------------------------------------------+
	_DFLDSQRT2:
		ldi	_acc_0,0x64
		ldi	_acc_1,0xDE
		ldi	_acc_2,0xF9
		ldi	_acc_3,0x33
		ldi	_acc_4,0xF3
		ldi	_acc_5,0x04
		ldi	_acc_s,0x35
		ldi	_acc_e,0x81
		rjmp	_DFLDFPOK
	;+------------------------------------------------+
	;| LOAD ACCUMULADOR WITH sqrt(2)-1=0,414213562373 |
	;|				   		  |
	;| Input  : None				  |
	;| Output : Facc sqrt(2)-1,Acc FPOK		  |
	;| Destroy: 					  |
	;+------------------------------------------------+
	_DFLDSQRT2M1:
		ldi	_acc_0,0x92
		ldi	_acc_1,0x79
		ldi	_acc_2,0xe7
		ldi	_acc_3,0xcf
		ldi	_acc_4,0xcc
		ldi	_acc_5,0x13
		ldi	_acc_s,0x54
		ldi	_acc_e,0x7F
		rjmp	_DFLDFPOK
	;+------------------------------------------------+
	;| LOAD ACCUMULADOR WITH sqrt(2)+1=2,414213562373 |
	;|				   		  |
	;| Input  : None				  |
	;| Output : Facc sqrt(2)+1,Acc FPOK		  |
	;| Destroy: 					  |
	;+------------------------------------------------+
	_DFLDSQRT2P1:
		ldi	_acc_0,0x32
		ldi	_acc_1,0xef
		ldi	_acc_2,0xfc
		ldi	_acc_3,0x99
		ldi	_acc_4,0x79
		ldi	_acc_5,0x82
		ldi	_acc_s,0x1a
		ldi	_acc_e,0x82
		rjmp	_DFLDFPOK
	
	;+------------------------------+
	;| LOAD MANTISA OF NUMBER PI	|
	;+------------------------------+
	_DFLDPI_MAN:
		ldi	_acc_0,0xc2
		ldi	_acc_1,0x68
		ldi	_acc_2,0x21
		ldi	_acc_3,0xa2
		ldi	_acc_4,0xda
		ldi	_acc_5,0x0f
		ldi	_acc_s,0x49
		ret
	;+----------------------------------------------+
	;| LOAD ACCUMULADOR WITH pi/2=1,5707963267948966|
	;|				   		|
	;| Input  : None				|
	;| Output : Facc pi/2,Acc FPOK			|
	;| Destroy: 					|
	;+----------------------------------------------+
	_DFLDPI2:
		rcall	_DFLDPI_MAN
		ldi	_acc_e,0x81			;exponent of pi/2
		rjmp	_DFLDFPOK
	;+----------------------------------------------+
	;| LOAD ACCUMULADOR WITH pi/4=0,7853981633974483|
	;|					   	|
	;| Input  : None				|
	;| Output : Facc pi/4,Acc FPOK			|
	;| Destroy: 					|
	;+----------------------------------------------+
	_DFLDPI4:
		rcall	_DFLDPI_MAN
		ldi	_acc_e,0x80			;exponent of pi/4
		rjmp	_DFLDFPOK
	;+----------------------------------------------+
	;| LOAD ACCUMULADOR WITH pi=3,14159265358979324	|
	;|				   		|
	;| Input  : None				|
	;| Output : Facc pi,Acc FPOK			|
	;| Destroy: 					|
	;+----------------------------------------------+
	_DFLDPI:
		rcall	_DFLDPI_MAN
		ldi	_acc_e,0x82			;exponent of pi
		rjmp	_DFLDFPOK
	;+------------------------------------------------------+
	;| LOAD ACCUMULADOR WITH 3*pi/2=4,71238898038468985	|
	;|				   			|
	;| Input  : None					|
	;| Output : Facc 3*pi/2,Acc FPOK			|
	;| Destroy: 						|
	;+------------------------------------------------------+
	_DFLD3PI2:
		ldi	_acc_0,0x92
		ldi	_acc_1,0x0e
		ldi	_acc_2,0x99
		ldi	_acc_3,0xf9
		ldi	_acc_4,0xe3
		ldi	_acc_5,0xcb
		ldi	_acc_s,0x16
		ldi	_acc_e,0x83
		rjmp	_DFLDFPOK
	;+----------------------------------------------+
	;| LOAD ACCUMULADOR WITH 2pi=6,28318530717958648|
	;|				   		|
	;| Input  : None				|
	;| Output : Facc 2*pi,Acc FPOK			|
	;| Destroy: 					|
	;+----------------------------------------------+
	_DFLD2PI:
		rcall	_DFLDPI_MAN
		ldi	_acc_e,0x83			;exponent of 2*pi
		rjmp	_DFLDFPOK
	;+--------------------------------------+
	;| FLOAT POINT ACCUMULATOR LOAD	   	|
	;| FROM Z REGISTER USED AS POINTER 	|
	;|				   	|
	;| Input  : Z --> Float number into SRAM|
	;| Output : Float accumulator number	|
	;| Destroy: Z				|
	;+--------------------------------------+
	.MACRO	_DFLDACC			;Macro to facilit use of routine
		ldi	ZL,LOW(@0)
		ldi	ZH,HIGH(@0)
		rcall	_DFLDZ
	.ENDMACRO
	_DFLDZ:
		ld	_acc_0,Z+
		ld	_acc_1,Z+
		ld	_acc_2,Z+
		ld	_acc_3,Z+
		ld	_acc_4,Z+
		ld	_acc_5,Z+
		ld	_acc_s,Z+
		ld	_acc_e,Z+
		ret
	;+--------------------------------------+
	;| FLOAT POINT ACCUMULATOR LOAD	   	|
	;| FROM Z DOUBLE CONTANT	 	|
	;|				   	|
	;| Input  : Z --> Float number into FLASH|
	;| Output : Float accumulator number	|
	;| Destroy: Z				|
	;+--------------------------------------+
	_DFLDDC:
		push	r0
		lpm
		mov	_acc_0,r0	
		adiw	ZL,1
		lpm
		mov	_acc_1,r0
		adiw	ZL,1
		lpm
		mov	_acc_2,r0	
		adiw	ZL,1
		lpm
		mov	_acc_3,r0	
		adiw	ZL,1
		lpm
		mov	_acc_4,r0	
		adiw	ZL,1
		lpm
		mov	_acc_5,r0	
		adiw	ZL,1
		lpm
		mov	_acc_s,r0	
		adiw	ZL,1
		lpm
		mov	_acc_e,r0	
		pop	r0
		ret
	;+--------------------------------------+
	;| FLOAT POINT ACCUMULATOR STORE   	|
	;| TO Z REGISTER USED AS POINTER 	|
	;|				   	|
	;| Input  : Float accumulator number	|
	;| Output : Z --> Float number into SRAM|
	;| Destroy: Z				|
	;+--------------------------------------+
	.MACRO	_DFSTACC			;Macro to facilit use of routine
		ldi	ZL,LOW(@0)
		ldi	ZH,HIGH(@0)
		rcall	_DFSTZ
	.ENDMACRO
	_DFSTZ:
		st	Z+,_acc_0
		st	Z+,_acc_1
		st	Z+,_acc_2
		st	Z+,_acc_3
		st	Z+,_acc_4
		st	Z+,_acc_5
		st	Z+,_acc_s
		st	Z+,_acc_e
		ret
	;+--------------------------------------+
	;| FLOAT POINT OP1 LOAD	FROM Z REGISTER |
	;| USED AS POINTER		 	|
	;|				   	|
	;| Input  : Z --> Float number into SRAM|
	;| Output : op1	|
	;| Destroy: Z				|
	;+--------------------------------------+
	.MACRO	_DFLDOP1			;Macro to facilit use of routine
		ldi	ZL,LOW(@0)
		ldi	ZH,HIGH(@0)
		rcall	_DFLDOP1Z
	.ENDMACRO
	_DFLDOP1Z:
		ld	_op1_0,Z+
		ld	_op1_1,Z+
		ld	_op1_2,Z+
		ld	_op1_3,Z+
		ld	_op1_4,Z+
		ld	_op1_5,Z+
		ld	_op1_s,Z+
		ld	_op1_e,Z+
		ret
	;+--------------------------------------+
	;| FLOAT POINT OP2 LOAD	FROM Z REGISTER |
	;| USED AS POINTER		 	|
	;|				   	|
	;| Input  : Z --> Float number into SRAM|
	;| Output : Float accumulator number	|
	;| Destroy: Z				|
	;+--------------------------------------+
	.MACRO	_DFLDOP2			;Macro to facilit use of routine
		ldi	ZL,LOW(@0)
		ldi	ZH,HIGH(@0)
		rcall	_DFLDOP2Z
	.ENDMACRO
	_DFLDOP2Z:
		ld	_op2_0,Z+
		ld	_op2_1,Z+
		ld	_op2_2,Z+
		ld	_op2_3,Z+
		ld	_op2_4,Z+
		ld	_op2_5,Z+
		ld	_op2_s,Z+
		ld	_op2_e,Z+
		ret
	;+--------------------------------------+
	;| MOVE FLOAT POINT ACCUMULATOR TO OP1 	|
	;| FLOAT POINT REGISTER		 	|
	;|				   	|
	;| Input  : none			|
	;| Output : none			|
	;| Destroy: none			|
	;+--------------------------------------+
	.MACRO	_DFMOVACCOP1
		rcall	_DFACCOP1
	.ENDMACRO
	_DFACCOP1:	
		mov	_op1_0,_acc_0
		mov	_op1_1,_acc_1
		mov	_op1_2,_acc_2
		mov	_op1_3,_acc_3
		mov	_op1_4,_acc_4
		mov	_op1_5,_acc_5
		mov	_op1_s,_acc_s
		mov	_op1_e,_acc_e
		ret
	;+--------------------------------------+
	;| MOVE FLOAT POINT OP1 TO ACCUMULATOR 	|
	;| FLOAT POINT REGISTER		 	|
	;|				   	|
	;| Input  : none			|
	;| Output : none			|
	;| Destroy: none			|
	;+--------------------------------------+
	.MACRO	_DFMOVOP1ACC
		rcall	_DFOP1ACC
	.ENDMACRO
	_DFOP1ACC:	
		mov	_acc_0,_op1_0
		mov	_acc_1,_op1_1
		mov	_acc_2,_op1_2
		mov	_acc_3,_op1_3
		mov	_acc_4,_op1_4
		mov	_acc_5,_op1_5
		mov	_acc_s,_op1_s
		mov	_acc_e,_op1_e
		ret
	;+--------------------------------------+
	;| MOVE FLOAT POINT ACCUMULATOR TO OP2 	|
	;| FLOAT POINT REGISTER		 	|
	;|				   	|
	;| Input  : none			|
	;| Output : none			|
	;| Destroy: none			|
	;+--------------------------------------+
	.MACRO	_DFMOVACCOP2
		rcall	_DFACCOP2
	.ENDMACRO
	_DFACCOP2:	
		mov	_op2_0,_acc_0
		mov	_op2_1,_acc_1
		mov	_op2_2,_acc_2
		mov	_op2_3,_acc_3
		mov	_op2_4,_acc_4
		mov	_op2_5,_acc_5
		mov	_op2_s,_acc_s
		mov	_op2_e,_acc_e
		ret
	;+--------------------------------------+
	;| MOVE FLOAT POINT OP2 TO ACCUMULATOR 	|
	;| FLOAT POINT REGISTER		 	|
	;|				   	|
	;| Input  : none			|
	;| Output : none			|
	;| Destroy: none			|
	;+--------------------------------------+
	.MACRO	_DFMOVOP2ACC
		rcall	_DFOP2ACC
	.ENDMACRO
	_DFOP2ACC:	
		mov	_acc_0,_op2_0
		mov	_acc_1,_op2_1
		mov	_acc_2,_op2_2
		mov	_acc_3,_op2_3
		mov	_acc_4,_op2_4
		mov	_acc_5,_op2_5
		mov	_acc_s,_op2_s
		mov	_acc_e,_op2_e
		ret
	;+--------------------------------------+
	;| FLOAT POINT PUSH AND POPS MACROS  	|
	;+--------------------------------------+
	.MACRO	_DFPUSHACC
		push	_acc_0
		push	_acc_1
		push	_acc_2
		push	_acc_3
		push	_acc_4
		push	_acc_5
		push	_acc_s
		push	_acc_e
	.ENDMACRO
	.MACRO	_DFPOPACC
		push	_acc_e
		push	_acc_s
		push	_acc_5
		push	_acc_4
		push	_acc_3
		push	_acc_2
		push	_acc_1
		push	_acc_0
	.ENDMACRO
	.MACRO	_DFPUSHOP1
		push	_op1_0
		push	_op1_1
		push	_op1_2
		push	_op1_3
		push	_op1_4
		push	_op1_5
		push	_op1_s
		push	_op1_e
	.ENDMACRO
	.MACRO	_DFPOPOP1
		push	_op1_e
		push	_op1_s
		push	_op1_5
		push	_op1_4
		push	_op1_3
		push	_op1_2
		push	_op1_1
		push	_op1_0
	.ENDMACRO
	.MACRO	_DFPUSHOP2
		push	_op2_0
		push	_op2_1
		push	_op2_2
		push	_op2_3
		push	_op2_4
		push	_op2_5
		push	_op2_s
		push	_op2_e
	.ENDMACRO
	.MACRO	_DFPOPOP2
		push	_op2_e
		push	_op2_s
		push	_op2_5
		push	_op2_4
		push	_op2_3
		push	_op2_2
		push	_op2_1
		push	_op2_0
	.ENDMACRO
#endif
.exit

