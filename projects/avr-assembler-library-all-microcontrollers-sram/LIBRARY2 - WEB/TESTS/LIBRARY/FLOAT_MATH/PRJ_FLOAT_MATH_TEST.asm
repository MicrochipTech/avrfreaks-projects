;+------------------------------------------------------+
;| SAMPLE PROGRAM TO DEMOSTRATE HOW USE DOUBLE PRESICION|
;| FLOAT POINT FUNCTIONS				|
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA			|
;| DATA:2/5/2005					|
;>------------------------------------------------------<
;| P R O G R A M  F I L E 				|
;+------------------------------------------------------+

;+------------------------------------------------------+
;| A V R  C L O C K 					|
;+------------------------------------------------------+------------------------------------------------------------------

.EQU	_AVR_CLOCK	= 16000000				;Define AVR Crystal Frequency

;+------------------------------------------------------+
;| G L O B A L  D E F I N I T I O N S  I N C L U D E S	|
;+------------------------------------------------------+------------------------------------------------------------------

.INCLUDE	"DEFS\GLOBAL_DEFINITIONS\GLOBDEFS.INC"		;Global Library Definitions must be a 1st line
.INCLUDE 	"DEFS\8515_FILES\8515DEF.INC"			;Microcontroler Definitions must be a 2nd line
.INCLUDE	"DEFS\8515_FILES\8515HDC.INC"			;must be a first include file with code
.INCLUDE	"DEFS\MATH_DEFINITIONS\MATHCONS.INC"		;math definitions

;+------------------------------------------------------+
;| S R A M  I N I T I A L I Z A T I O N 		|
;+------------------------------------------------------+------------------------------------------------------------------

.EQU	_SRAM_BOOT_TYPE	= _SRAM_CLEAR				;Clear SRAM duging boot
								;must be defined after inclusion of GLOBDEFS.INC

;+------------------------------------------------------+
;| H A N D D L E  I N T E R R U P T S  I N C L U D E S	|
;+------------------------------------------------------+------------------------------------------------------------------



;+------------------------------------------------------+
;| I N C L U D E  F L O A T  P O I N T D O U B L E  	|
;| P R E C I S I O N  M A T H  R O U T I N E S 		|
;+------------------------------------------------------+------------------------------------------------------------------

.INCLUDE	"MATH\FLOAT_DOUBLE\HEADER_MACROS\DOUBLE_FLOAT_HEADER.INC"	;header for float double math routines
.INCLUDE	"MATH\FLOAT_DOUBLE\HEADER_MACROS\DOUBLE_FLOAT_MACROS.INC"	;header for float double math routines

.INCLUDE "MATH\FLOAT_DOUBLE\ADDSUB\DFADDSUB.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\MULDIV\DFMULDIV.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\SINE\SINE.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\COSINE\COSINE.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\ASINE\ASIN.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\ACOS\ACOS.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\ATAN\ATAN.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\TAN\TAN.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\SQUARE_ROOT\DFSQRT.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\INT\INT.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\LOAD_STORE\DFLDXSTX.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\LONG_TO_FLOAT\DFLTOF.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\SERIES\ATN\SERIE_ATN.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\SERIES\LN\SERIE_LN.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\SERIES\SINE\SERIE_SINE.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\FLOAT_TO_BCD\DFFTOBCD.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\FLOAT_TO_LONG\DFFTOL.INC"
.INCLUDE "MATH\FLOAT_DOUBLE\BCD_TO_STRING\DFBCDTOS.INC"
.INCLUDE "MATH\DFT\DFT_8BITS_64POINTS\DFT864V1.INC"
.INCLUDE "MATH\INTFRAC\IF_MUBSFB\MUBSFB.INC"
.INCLUDE "MATH\INTFRAC\IF_MUBSFB\MUBSFB.INC"
.INCLUDE "MATH\INTEGER\ATOS\ATOS.INC"
.INCLUDE "MATH\INTEGER\UBTOS\UBTOS.INC"
.INCLUDE "MATH\INTEGER\DIV_S32S32S32\DIV_S32S32S32.INC"
.INCLUDE "MATH\INTEGER\DIV_U8U8U8\DIV_U8U8U8.INC"
.INCLUDE "MATH\INTEGER\ITOS\ITOS.INC"
.INCLUDE "MATH\INTEGER\UITOS\UITOS.INC"
.INCLUDE "MATH\INTEGER\MUL_S8S8S8\MUL_S8S8S8.INC"
.INCLUDE "MATH\INTEGER\MUL_U8U8U16\MUL_U8U8U16.INC"
.INCLUDE "MATH\INTEGER\MUL_U16U16U16\MUL_U16U16U16.INC"
.INCLUDE "MATH\INTEGER\MUL_U16U16U32\MUL_U16U16U32.INC"
.INCLUDE "MATH\INTEGER\MUL_U32U32U32\MUL_U32U32U32.INC"
.INCLUDE "MATH\INTEGER\MUL_U32U32U64\MUL_U32U32U64.INC"
.INCLUDE "MATH\INTEGER\MULDIV_U8U8U8\MULDIV_U8U8U8U8.INC"

;+------------------------------------------------------+
;|							|
;| M A I N   I N I T I A L I Z A T I O N  		|
;|							|
;+------------------------------------------------------+------------------------------------------------------------------
_MAIN_INIT:

	_SET_HANDDLE _HDC_INT0_VECT,_TESTE

	ldi	Acc,(1<<INT0)
	out	GIMSK,Acc
	
;	sei

;+------------------------------------------------------+
;|							|
;| M A I N  P R O G R A M				|	
;|							|
;+------------------------------------------------------+-------------------------------------------------------------------


_MAIN:

	_FINIT
	_FLDII	1
	_FCOS
	_FSTBCD	_A
	rjmp	_MAIN
		
NUMBER:
	.DB	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x81
_TESTE:
	push	Acc
	in	Acc,SREG
	push	Acc
	_M_PUSH_ALL_REGS
	_M_PUSH_MATH_FLOAT_STACK
	_M_PUSH_MATH_VARIABLES


	_FINIT
	_FLDII	8
	_FLDII	17
	_FADD
	_FLDII	9
	_FDIV	
	_FSTBCD	_B


	_M_POP_MATH_VARIABLES
	_M_POP_MATH_FLOAT_STACK
	_M_POP_ALL_REGS
	pop	Acc
	out	SREG,Acc
	pop	Acc
	ret

.DSEG
_A:		.BYTE	_FBCD
_B:		.BYTE	_FBCD
.CSEG
