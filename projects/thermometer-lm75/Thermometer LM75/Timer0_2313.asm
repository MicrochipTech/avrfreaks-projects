;**** Timer/Counter 0 Control Subroutines *********************************
;
; Filename		:Timer0_2313.asm
;
; Project		;Brick For Programs
; First Target	:Roulette With Fiskal Counter (Roulette_F.asm)
; Device		:AT90S2313
; Created		:2002.10.11
; Last Updated	:
; Version		:1.0
;
; DESCRIPTION	:Timer/Counter 0 Control Subroutines
;				
;**************************************************************************

;--------------------------------------------------------------------------
;Timer/Counter Interrupt Mask Register – TIMSK
;Bit 			7		6		5		4		3		2		1	 	0
;$39 ($59) 		TOIE1	OCIE1A 	– 		– 		TICIE1 	– 		TOIE0 	– 
;Read/Write 	R/W 	R/W 	R 		R 		R/W 	R 		R/W 	R
;Initial value 	0 		0 		0 		0 		0	 	0 		0 		0
;--------------------------------------------------------------------------
;• Bit 7 – TOIE1: Timer/Counter1 Overflow Interrupt Enable
;• Bit 6 – OCIE1A: Timer/Counter1 Output Compare Match Interrupt Enable
;• Bit 5,4 – Res: Reserved Bits.
;• Bit 3 – TICIE1: Timer/Counter1 Input Capture Interrupt Enable
;• Bit 2 – Res: Reserved Bit
;• Bit 1 – TOIE0: Timer/Counter0 Overflow Interrupt Enable
;• Bit 0 – Res: Reserved Bit.
;--------------------------------------------------------------------------
;Timer/Counter Interrupt FLAG Register – TIFR
;Bit 			7 		6 		5 		4 		3 		2 		1 		0
;$38 ($58) 		TOV1 	OCF1A 	– 		– 		ICF1 	– 		TOV0 	– 
;Read/Write 	R/W 	R/W 	R 		R 		R/W 	R 		R/W 	R
;Initial value 	0 		0 		0 		0 		0 		0 		0 		0
;--------------------------------------------------------------------------
;• Bit 7 – TOV1: Timer/Counter1 Overflow Flag
;• Bit 6 – OCF1A: Output Compare Flag 1A
;• Bits 5, 4 – Res: Reserved Bits.
;• Bit 3 – ICF1: Input Capture Flag 1
;• Bit 2 – Res: Reserved Bit
;• Bit 0 – Res: Reserved Bit.

;**************************************************************************
;**************************************************************************

;***** Timer/Counter0
;--------------------------------------------------------------------------
;Timer/Counter0 Control Register – TCCR0
;Bit 			7	 	6 		5 		4 		3 		2 		1 		0
;$33 ($53) 		– 		– 		– 		– 		– 		CS02 	CS01 	CS00
;Read/Write 	R 		R 		R 		R 		R 		R/W 	R/W 	R/W
;Initial value 	0 		0	 	0	 	0	 	0	 	0	 	0	 	0
;--------------------------------------------------------------------------
;• Bits 7..3 – Res: Reserved Bits.
;• Bits 2,1,0 – CS02, CS01, CS00: Clock Select0, Bit 2,1 and 0

;****  Clock 0 Prescale Select ********************************************
;CS02 	CS01 	CS00 	Description
;0 		0 		0 		Stop, the Timer/Counter0 is stopped.
;0 		0 		1 		CK
;0 		1 		0 		CK/8
;0 		1 		1 		CK/64
;1 		0 		0 		CK/256
;1 		0		1 		CK/1024
;1 		1 		0 		External Pin T0, falling edge
;1 		1 		1 		External Pin T0, rising edge
;**************************************************************************
;--------------------------------------------------------------------------
;Timer/Counter0 – TCNT0
;Bit	 		7 		6 		5 		4 		3 		2 		1 		0
;$32 ($52) 		MSB 													LSB
;Read/Write 	R/W 	R/W 	R/W 	R/W 	R/W 	R/W 	R/W 	R/W
;Initial value 	0 		0 		0 		0 		0 		0 		0 		0
;--------------------------------------------------------------------------
;**************************************************************************
;--------------------------------------------------
; T0max ms  (TCNT0 = 0)
;--------------------------------------------------
;  cFrequency (Hz)	8.000.000	
;    CK/1		0.125 us	x 256 = 32us
;    CK/8		1 us		x 256 = 256us
;    CK/64		8 us		x 256 = 2.048ms
;    CK/256		32 us		x 256 = 8.192ms
;    CK/1024	0.128 ms	x 256 = 32.768ms
;--------------------------------------------------
; Tt0 = (256-Ct0)*t0
; Ct0 = 256 - Tt0/t0
;**************************************************************************
;**************************************************************************
;--------------------------------------------------
.equ T0_Stop	= 0
.equ T0_0125us 	= 1
.equ T0_1us		= 2
.equ T0_8us		= 3
.equ T0_32us	= 4
.equ T0_128us	= 5
;--------------------------------------------------

;--------------------------------------------------
.macro m_enable_timerT0
	in	temp, TIFR
	cbr	temp, 1<<TOV0
	out	TIFR, temp
	in	temp, TIMSK
	sbr	temp, 1<<TOIE0
	out	TIMSK, temp
	ldi	temp, T0_8us	;Settings T0_time * 256
	out	TCCR0, temp
.endmacro
;--------------------------------------------------

