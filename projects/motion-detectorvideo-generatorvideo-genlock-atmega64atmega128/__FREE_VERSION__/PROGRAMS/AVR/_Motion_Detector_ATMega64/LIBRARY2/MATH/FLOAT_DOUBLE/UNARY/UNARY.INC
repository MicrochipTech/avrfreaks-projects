;+------------------------------------------------------------------------------+
;| DOUBLE PRECISION FLOAT POINT 	I					|
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA						|
;| NEED FILES 									|
;|										|
;|	FILE		DIRECTORY						|
;|										|
;|	GLOBDEFS.INC	AVR\LIBRARY\DEFS\GLOBAL_DEFINITIONS\GlobDefs.inc	|
;|	MATHCONS.INC	AVR\LIBRARY\MATH\FLOAT_DOUBLE\HEADER_MACROS\MathCons.Inc|
;|	DFINF.INC	AVR\LIBRARY\MATH\FLOAT_DOUBLE\INFINIT_RESULT\DFInf.Inc	|
;|	DFNORM.INC	AVR\LIBRARY\MATH\FLOAT_DOUBLE\NORMALIZE\DFNorm.Inc	|
;|	DFXTOF.INC 	AVR\LIBRARY\MATH\FLOAT_DOUBLE\LONG_TO_FLOAT\DFLTOF.Inc	|
;|	DFMULDIV.INC 	AVR\LIBRARY\MATH\FLOAT_DOUBLE\MULDIV\DFMulDiv.Inc	|
;|	DFADDSUB.INC 	AVR\LIBRARY\MATH\FLOAT_DOUBLE\ADDSUB\DFAddSub.Inc	|
;|	DFLDXSTX.INC 	AVR\LIBRARY\MATH\FLOAT_DOUBLE\LOAD_STORE\DFLdXStX.Inc	|
;|										|										|
;| DATE:24/05/2001 VERSION 1.0							|
;>--------------------------------------------------------------+---------------<
;| D O U B L E  F L O A T  P O I N T  R O U T I N E S		|
;| U N A R Y  F U N C T I O N S		 			|
;+--------------------------------------------------------------+

#ifndef __MATH_FLOAT_UNARY__

	#define __MATH_FLOAT_UNARY__

	.message "[ (MATH) FLOAT UNARY FUNCTIONS ]"


	;+----------------------------------------------+
	;| ABSOLUTE VALUE				|
	;|				   		|
	;| Input  : Facc				|
	;| Output : absolute(Facc),Acc FPOK		|
	;| Destroy: 					|
	;+----------------------------------------------+
	_DFABS:
		andi	_acc_s,~(1<<7)			;clear sign bit
		rjmp	_DFLDFPOK

	;+----------------------------------------------+
	;| CHANGE SIGNAL OF  VALUE			|
	;|				   		|
	;| Input  : Facc				|
	;| Output : -Facc,Acc FPOK			|
	;| Destroy: 					|
	;+----------------------------------------------+
	_DFCHS:
		ldi	Acc,(1<<7)			;mask of bit sign
		eor	_acc_s,Acc			;change sign bit
		rjmp	_DFLDFPOK	

	;+----------------------------------------------+
	;| INVERSE = 1/VALUE				|
	;|				   		|
	;| Input  : Facc				|
	;| Output : 1/Facc,Acc FPOK			|
	;| Destroy: 					|
	;+----------------------------------------------+
	_DFINV:
		clr	_op1_0					;op1=1
		clr	_op1_1
		clr	_op1_2
		clr	_op1_3
		clr	_op1_4
		clr	_op1_5
		clr	_op1_s
		ldi	Acc,0x81
		mov	_op1_e,Acc
		rcall	_DFACCOP2				;op2=Facc
		rcall	_DFDIV					;divide op1/op1=1/Facc
		rjmp	_DFLDFPOK

	;+----------------------------------------------+
	;| SQUARE OF VALUE				|
	;|				   		|
	;| Input  : Facc				|
	;| Output : Facc*Facc,Acc FPOK			|
	;| Destroy: 					|
	;+----------------------------------------------+
	_DFSQR:
		rcall	_DFACCOP1			;op1=Facc
		rcall	_DFACCOP2			;op2=Facc
		rcall	_DFMUL				;multiply
		rjmp	_DFLDFPOK

#endif

.exit
