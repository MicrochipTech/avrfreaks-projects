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
;+--------------------------------------------------------------+---+-----------------------------------+
;|													|
;| DATE:24/05/2001 VERSION 1.0										|
;>------------------------------------------------------+-----------------------------------------------<
;| D O U B L E   F L O A T  P O I N T  R O U T I N E S	|
;| M A C R O S  T O  F A C I L I T Y  U S E  O F  A L L |
;| M A T H  D O U B L E  F U N C T I O N S		|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;+------------------------------------------------------+
#ifndef __MATH_FLOAT_MACROS__
	#define __MATH_FLOAT_MACROS__
#endif
#ifndef __MATH_FLOAT_HEADER__
	.message "<<<<< MATH FLOAT HEADER NOT INCLUDED >>>>>"	
#endif
.message "+-------------------+"
.message "|                   |"
.message "| MATH FLOAT MACROS |"
.message "|                   |"
.message "+-------------------+"
;+---------------------------------+
;| FLOAT POINT INITIALIZE ENGENE   |
;+---------------------------------+
.MACRO	_FINIT				;initialize float point engine
	rcall	_FDBL_INIT
.ENDMACRO
.MACRO	_FDECST				;decrement stack point
	rcall	_FDBL_DECST
.ENDMACRO
.MACRO	_FINCST				;increment stack point
	rcall	_FDBL_INCST
.ENDMACRO
;+---------------------------------+
;| FLOAT POINT LOAD CONSTANTS	   |
;+---------------------------------+
.MACRO	_FLD0				;load zero
	rcall	_FDBL_LD_0		;Syntax _FLD0
.ENDMACRO
.MACRO	_FLD1				;load one
	rcall	_FDBL_LD_1		;Syntax _FLD1
.ENDMACRO
.MACRO	_FLD2				;load two
	rcall	_FDBL_LD_2		;Syntax _FLD2
.ENDMACRO
.MACRO	_FLD10				;load ten
	rcall	_FDBL_LD_10		;Syntax _FLD10
.ENDMACRO
.MACRO	_FLDPI2				;load PI/2
	rcall	_FDBL_LD_PI2		;Syntax _FLDPI
.ENDMACRO
.MACRO	_FLDPI4				;load PI/4
	rcall	_FDBL_LD_PI4		;Syntax _FLDPI
.ENDMACRO
.MACRO	_FLDPI				;load PI
	rcall	_FDBL_LD_PI		;Syntax _FLDPI
.ENDMACRO
.MACRO	_FLD3PI2			;load 3*PI/2
	rcall	_FDBL_LD_3PI2		;Syntax _FDL3PI2
.ENDMACRO
.MACRO	_FLD2PI				;load 2PI
	rcall	_FDBL_LD_2PI		;Syntax _FDL2PI
.ENDMACRO
.MACRO	_FLDE				;load E
	rcall	_FDBL_LD_E		;Syntax _FDLDE
.ENDMACRO
.MACRO	_FLDSQRT2			;load SQRT(2)
	rcall	_FDBL_LD_SQRT2		;Syntax _FDLDSQRT2
.ENDMACRO
.MACRO	_FLDSQRT2M1			;load SQRT(2)-1
	rcall	_FDBL_LD_SQRT2M1	;Syntax _FDLDSQRT2M1
.ENDMACRO
.MACRO	_FLDSQRT2P1			;load SQRT(2)+1
	rcall	_FDBL_LD_SQRT2P1	;Syntax _FDLDSQRT2P1
.ENDMACRO
;+---------------------------------+
;| FLOAT POINT LOADERS		   |
;+---------------------------------+
.MACRO	_FLDII				;Float point load integer imediate
	ldi	Acc,LOW(@0)		;sintax _FLDII number
	ldi	AccH,HIGH(@0)
	rcall	_FDBL_LDII
.ENDMACRO
.MACRO	_FLDLI				;Float point load long integer imediate
	ldi	Acc,LOW(LWRD(@0))	;sintax _FLDLI number
	ldi	AccH,HIGH(LWRD(@0))
	ldi	AccT,LOW(HWRD(@0))
	ldi	AccTH,HIGH(HWRD(@0))
	rcall	_FDBL_LDLI
.ENDMACRO
.MACRO	_FLDDC				;Float point double contant to float stack
	ldi	Acc,LOW(@0*2)		;sintax _FLDLI number
	ldi	AccH,HIGH(@0*2)
	rcall	_FDBL_LDFC
.ENDMACRO
.MACRO	_FLDS				;Float point load string with zero ended from flash
	ldi	ZL,LOW(@0*2)		;sintax _FLDS String
	ldi	ZH,HIGH(@0*2)
	rcall	_FDBL_LDS
.ENDMACRO
.MACRO	_FLDSS				;Float point load string with zero ended from sram
	ldi	ZL,LOW(@0)		;sintax _FLDSS String
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_LDSS
.ENDMACRO
.MACRO	_FLDB				;Float point load byte 
	ldi	ZL,LOW(@0)		;sintax _FLDB byte
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_LDB
.ENDMACRO
.MACRO	_FLDI				;Float point load integer
	ldi	ZL,LOW(@0)		;sintax _FLDI integer
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_LDI
.ENDMACRO
.MACRO	_FLDL				;Float point load long
	ldi	ZL,LOW(@0)		;syntax _FLDL long
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_LDL
.ENDMACRO
.MACRO	_FLDF				;Float point load single precision float point
	ldi	ZL,LOW(@0)		;syntax _FLDF FloatSingle
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_LDF
.ENDMACRO
.MACRO	_FLDD				;Float point load double precision float point
	ldi	ZL,LOW(@0)		;syntax _FLDD FloatDouble
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_LDD
.ENDMACRO
.MACRO	_FLD				;same as _FLDD
	ldi	ZL,LOW(@0)
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_LDD
.ENDMACRO
;+---------------------------------+
;| FLOAT POINT STORES		   |
;+---------------------------------+
.MACRO	_FSTB				;Float point store byte
	ldi	ZL,LOW(@0)		;syntax _FSTB byte
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_STB
.ENDMACRO
.MACRO	_FSTI				;Float point store integer
	ldi	ZL,LOW(@0)		;syntax _FSTI integer
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_STI
.ENDMACRO
.MACRO	_FSTL				;Float point store long
	ldi	ZL,LOW(@0)		;syntax _FSTL long
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_STL		
.ENDMACRO
.MACRO	_FSTF				;Float point store single precision float point
	ldi	ZL,LOW(@0)		;syntax _FSTF FloatSingle
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_STF
.ENDMACRO
.MACRO	_FSTD				;Float point store double precision float point
	ldi	ZL,LOW(@0)		;syntax _FSTD FloatDouble
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_STD
.ENDMACRO
.MACRO	_FST				;same as _FSTD
	ldi	ZL,LOW(@0)
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_STD
.ENDMACRO
.MACRO	_FSTBCD				;Float point store BCD 
	ldi	ZL,LOW(@0)		;Syntax _FSTBCD	FloatBCD
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_BCD
.ENDMACRO
;+------------------------------------------+
;| FLOAT POINT STORES WIT STACK POINT POP   |
;+------------------------------------------+
.MACRO	_FSTBP				;Float point store byte
	ldi	ZL,LOW(@0)		;syntax _FSTB byte
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_STBP
.ENDMACRO
.MACRO	_FSTIP				;Float point store integer
	ldi	ZL,LOW(@0)		;syntax _FSTI integer
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_STIP
.ENDMACRO
.MACRO	_FSTLP				;Float point store long
	ldi	ZL,LOW(@0)		;syntax _FSTL long
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_STLP		
.ENDMACRO
.MACRO	_FSTFP				;Float point store single precision float point
	ldi	ZL,LOW(@0)		;syntax _FSTF FloatSingle
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_STFP
.ENDMACRO
.MACRO	_FSTDP				;Float point store double precision float point
	ldi	ZL,LOW(@0)		;syntax _FSTD FloatDouble
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_STDP
.ENDMACRO
.MACRO	_FSTP				;same as _FSTD
	ldi	ZL,LOW(@0)
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_STDP
.ENDMACRO
.MACRO	_FSTBCDP			;Float point store BCD and pop
	ldi	ZL,LOW(@0)		;Syntax _FSTBCD	FloatBCD and pop
	ldi	ZH,HIGH(@0)
	rcall	_FDBL_BCDP
.ENDMACRO
;+------------------------------------------+
;| FLOAT POINT ALGEBRIC FUNCTIONS	    |
;+------------------------------------------+
.MACRO	_FADD				;Float point Addition
	rcall	_FDBL_ADD		;Syntax _FADD
.ENDMACRO
.MACRO	_FSUB				;Float point subtraction
	rcall	_FDBL_SUB		;Syntax _FSUB
.ENDMACRO
.MACRO	_FMUL				;Float point multiplication
	rcall	_FDBL_MUL		;Syntax _FMUL
.ENDMACRO
.MACRO	_FDIV				;Float point division
	rcall	_FDBL_DIV		;Syntax _FDIV
.ENDMACRO
.MACRO	_FDIVFRAC			;Float point division and return fraction only
	rcall	_FDBL_DIVFRAC		;Syntax _FDIVFRAC
.ENDMACRO
;+------------------------------------------+
;| FLOAT POINT UNARY ALGEBRIC FUNCTIONS	    |
;+------------------------------------------+
.MACRO	_FINV				;Float point inverse function
	rcall	_FDBL_INV		;Syntax _FINV
.ENDMACRO
.MACRO	_FABS				;Float point absolute
	rcall	_FDBL_ABS		;Syntax _FABS
.ENDMACRO
.MACRO	_FCHS				;Float point change signal
	rcall	_FDBL_CHS		;Syntax _FCHS
.ENDMACRO
.MACRO	_FSQR				;Float point Square
	rcall	_FDBL_SQR		;Systax _FSQR
.ENDMACRO
.MACRO	_FSQRT				;Float point Square Root
	rcall	_FDBL_SQRT		;Syntax _FSQRT
.ENDMACRO
.MACRO	_FINT				;Float point Integer part
	rcall	_FDBL_INT		;Syntax _FINT
.ENDMACRO
.MACRO	_FFIX				;Float point Integer part using FIX
	rcall	_FDBL_FIX		;Syntax _FFIX
.ENDMACRO
;+------------------------------------------+
;| FLOAT POINT TRIGONOMETRIC FUNCTIONS	    |
;+------------------------------------------+
.MACRO	_FSERSIN			;Float point maclaurin partial sine 
	rcall	_FDBL_SERSIN
.ENDMACRO
.MACRO	_FSERATN			;Float point taylor partial arc tangent
	rcall	_FDBL_SERATN
.ENDMACRO
.MACRO	_FSIN				;Float point sine function
	rcall	_FDBL_SIN
.ENDMACRO
.MACRO	_FCOS				;Float point co-sine function
	rcall	_FDBL_COS
.ENDMACRO
.MACRO	_FTAN				;Float point tangent function
	rcall	_FDBL_TAN
.ENDMACRO
.MACRO	_FATAN				;Float point arc tangent function
	rcall	_FDBL_ATAN
.ENDMACRO
.MACRO	_FASIN				;Float point arc sine function
	rcall	_FDBL_ASIN
.ENDMACRO
.MACRO	_FACOS				;Float point arc co-sine function
	rcall	_FDBL_ACOS
.ENDMACRO
;+------------------------------------------+
;| FLOAT POINT COMPARE ROUTINES		    |	
;+------------------------------------------+
.MACRO	_FCOMP				;Float point compare float top stack with float top -1
	rcall	_FDBL_COMP
.ENDMACRO
.MACRO	_FBRANCH_EQ			;branch if Float point equal st(n)=st(n-1)
	lds	r0,_DF_FSREG		;get float point SREG
	sbrc	r0,_FF_ZERO		;skip if result is not zero
	rjmp	@0			;else branch *<NOCHANGE>* 
.ENDMACRO
.MACRO	_FBRANCH_NEQ			;branch if Float point not equal st(n)<>st(n-1)
	lds	r0,_DF_FSREG		;get float point SREG
	sbrs	r0,_FF_ZERO		;skip if result is zero
	rjmp	@0			;else branch *<NOCHANGE>* 
.ENDMACRO
.MACRO	_FBRANCH_GT			;branch if Float point great than st(n)>st(n-1)
	lds	r0,_DF_FSREG		;get float point SREG
	sbrc	r0,_FF_ZERO		;skip if result is not zero
	rjmp	PC+3			;exit if zero  *<NOCHANGE>* 
	sbrs	r0,_FF_CARRY		;skip if carry set
	rjmp	@0			;else branch *<NOCHANGE>* 
.ENDMACRO
.MACRO	_FBRANCH_LT			;branch if Float point great than st(n)<t(n-1)
	lds	r0,_DF_FSREG		;get float point SREG
	sbrc	r0,_FF_ZERO		;skip if result is not zero
	rjmp	PC+3			;exit if zero  *<NOCHANGE>* 
	sbrc	r0,_FF_CARRY		;skip if carry clear
	rjmp	@0			;else branch *<NOCHANGE>* 
.ENDMACRO
.MACRO	_FBRANCH_GTEQ			;branch if Float point great than or igual st(n)>st(n-1)
	lds	r0,_DF_FSREG		;get float point SREG
	sbrs	r0,_FF_CARRY		;skip if carry set
	rjmp	@0			;else branch *<NOCHANGE>* 
.ENDMACRO
.MACRO	_FBRANCH_LTEQ			;branch if Float point less than or igual st(n)>st(n-1)
	lds	r0,_DF_FSREG		;get float point SREG
	sbrc	r0,_FF_ZERO		;skip if no zero
	rjmp	@0			;else branch *<NOCHANGE>* 
	sbrc	r0,_FF_CARRY		;skip if carry no set
	rjmp	@0
.ENDMACRO
	

