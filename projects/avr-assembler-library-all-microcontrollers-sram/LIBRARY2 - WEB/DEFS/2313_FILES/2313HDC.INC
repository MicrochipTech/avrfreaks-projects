;+------------------------------------------------------+
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA			|
;| DATA: 10/04/2000					|
;>------------------------------------------------------<
;| H A N D D L E S  I N T E R R U P T S  F O R 		|
;| A T 9 0 S 2 3 1 3			 		|
;+------------------------------------------------------+

#ifndef __2313_HANDDLES__

	#define __2313_HANDDLES__

	#ifndef __GLOBAL_DEFINITIONS__
		.message "/--------------------------------\"
		.message "|GLOBAL DEFINITIONS NOT INCLUDED |"
		.message "\--------------------------------/"
	#endif


	.message "+---------------+"
	.message "|               |"
	.message "| 2313 HANDDLES |"
	.message "|               |"
	.message "+---------------+"

	;+----------------------------------+
	;| HANDDLE SRAM VECTORS DEFINITIONS |
	;+----------------------------------+

	.DSEG
		_HDC_START:
		_HDC_INT0_VECT:		.BYTE 2		;use thats label names to set handdle address routines
		_HDC_INT1_VECT:		.BYTE 2
		_HDC_ICP1_VECT:		.BYTE 2
		_HDC_OC1_VECT:		.BYTE 2
		_HDC_OVF1_VECT:		.BYTE 2
		_HDC_OVF0_VECT:		.BYTE 2
		_HDC_URXC_VECT:		.BYTE 2
		_HDC_UDRE_VECT:		.BYTE 2
		_HDC_UTXC_VECT:		.BYTE 2
		_HDC_ACI_VECT:		.BYTE 2
		_HDC_END:
	.CSEG

	;+----------------------------------+
	;| HANDDLE JUMP VECTORS DEFINITIONS |
	;+----------------------------------+

	rjmp	_HDC_INIT		;jump to initialization routine
	rjmp	_HDC_INT0_PROC		;jump to haddle routines
	rjmp	_HDC_INT1_PROC
	rjmp	_HDC_ICP1_PROC
	rjmp	_HDC_OC1_PROC
	rjmp	_HDC_OVF1_PROC
	rjmp	_HDC_OVF0_PROC
	rjmp	_HDC_URXC_PROC
	rjmp	_HDC_UDRE_PROC
	rjmp	_HDC_UTXC_PROC
	rjmp	_HDC_ACI_PROC

	;+----------------------
	;| INITIALIZE STACK & SRAM 
	;+----------------------
	.EQU	RAMSTART	= 0X60

	_HDC_INIT:
		ldi	Acc,low(RAMEND)
		out	SPL,Acc
	
	.INCLUDE "DEFS\SRAM_INITIALIZATION\SRAM_INIT.INC"		;SRAM & Stack initialization include file

	rjmp	_MAIN_INIT		;jump to main initializations

	;+----------------------------------+
	;| HANDDLES 			    |
	;+----------------------------------+
	;------------------<<  INT 0   >>---------------------------------------------------------------------
	_HDC_INT0_PROC:
		push	ZL			;save Z
		push	ZH
		lds	ZL,_HDC_INT0_VECT+0	;Z=HANDDLE VECTOR
		lds	ZH,_HDC_INT0_VECT+1
		rjmp	_HDC_EXIT
	;------------------<<  INT 1   >>---------------------------------------------------------------------
	_HDC_INT1_PROC:
		push	ZL			;save Z
		push	ZH
		lds	ZL,_HDC_INT1_VECT+0	;Z=HANDDLE VECTOR
		lds	ZH,_HDC_INT1_VECT+1
		rjmp	_HDC_EXIT
	;------------------<<  ICP 1  >>---------------------------------------------------------------------
	_HDC_ICP1_PROC:
		push	ZL			;save Z
		push	ZH
		lds	ZL,_HDC_ICP1_VECT+0	;Z=HANDDLE VECTOR
		lds	ZH,_HDC_ICP1_VECT+1
		rjmp	_HDC_EXIT
	;------------------<<  OC  1  >>---------------------------------------------------------------------
	_HDC_OC1_PROC:
		push	ZL			;save Z
		push	ZH
		lds	ZL,_HDC_OC1_VECT+0	;Z=HANDDLE VECTOR
		lds	ZH,_HDC_OC1_VECT+1
		rjmp	_HDC_EXIT
	;------------------<<  OVF 1   >>---------------------------------------------------------------------
	_HDC_OVF1_PROC:
		push	ZL			;save Z
		push	ZH
		lds	ZL,_HDC_OVF1_VECT+0	;Z=HANDDLE VECTOR
		lds	ZH,_HDC_OVF1_VECT+1
		rjmp	_HDC_EXIT
	;------------------<<  OVF 0   >>---------------------------------------------------------------------
	_HDC_OVF0_PROC:
		push	ZL			;save Z
		push	ZH
		lds	ZL,_HDC_OVF0_VECT+0	;Z=HANDDLE VECTOR
		lds	ZH,_HDC_OVF0_VECT+1
		rjmp	_HDC_EXIT
	;------------------<<  URXC   >>---------------------------------------------------------------------
	_HDC_URXC_PROC:
		push	ZL			;save Z
		push	ZH
		lds	ZL,_HDC_URXC_VECT+0	;Z=HANDDLE VECTOR
		lds	ZH,_HDC_URXC_VECT+1
		rjmp	_HDC_EXIT
	;------------------<<  UDRE   >>---------------------------------------------------------------------
	_HDC_UDRE_PROC:
		push	ZL			;save Z
		push	ZH
		lds	ZL,_HDC_UDRE_VECT+0	;Z=HANDDLE VECTOR
		lds	ZH,_HDC_UDRE_VECT+1
		rjmp	_HDC_EXIT
	;------------------<<  UTXC   >>---------------------------------------------------------------------
	_HDC_UTXC_PROC:
		push	ZL			;save Z
		push	ZH
		lds	ZL,_HDC_UTXC_VECT+0	;Z=HANDDLE VECTOR
		lds	ZH,_HDC_UTXC_VECT+1
		rjmp	_HDC_EXIT
	;------------------<<  ACI    >>---------------------------------------------------------------------
	_HDC_ACI_PROC:
		push	ZL			;save Z
		push	ZH
		lds	ZL,_HDC_ACI_VECT+0	;Z=HANDDLE VECTOR
		lds	ZH,_HDC_ACI_VECT+1
		rjmp	_HDC_EXIT
	;------------------<<  COMMON RETURN ADDRESS   >>----------------------------------------------------
	_HDC_EXIT:
		icall				;call routine
		pop	ZH
		pop	ZL
		reti				;return from interrupt
	_HDC_RETURN:
		ret				;return from interrupt routine
	
#endif

.exit
