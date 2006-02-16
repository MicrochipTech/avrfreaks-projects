;**** Timer/Counter 1 Control Subroutines *********************************
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
; DESCRIPTION	:Timer/Counter 1 Control Subroutines
;				
;**************************************************************************
;--------------------------------------------------------------------------
;Timer/Counter Interrupt Mask Register – TIMSK
;Bit 			7		6		5		4		3		2		1		0
;$39 ($59) 		TOIE1	OCIE1A 	– 		– 		TICIE1 	– 		TOIE0 	– 
;Read/Write 	R/W 	R/W 	R 		R 		R/W 	R 		R/W 	R
;Initial value 	0 		0 		0 		0 		0 		0 		0 		0
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

;***************************************************************************
;***************************************************************************

;***** Timer/Counter1
;---------------------------------------------------------------------------
;Timer/Counter1 Control Register A – TCCR1A
;Bit 			7 		6 		5 		4 		3 		2 		1 		0
;$2F ($4F) 		COM1A1 	COM1A0 	– 		– 		– 		– 		PWM11 	PWM10
;Read/Write 	R/W 	R/W 	R 		R 		R 		R 		R/W 	R/W
;Initial value 	0 		0 		0 		0 		0 		0 		0 		0
;--------------------------------------------------------------------------
;• Bits 7,6 – COM1A1, COM1A0: Compare Output Mode1, Bits 1 and 0
;• Bits 5..2 – Res: Reserved Bits.
;• Bits 1,0 – PWM11, PWM10: Pulse Width Modulator Select Bits

;Compare 1 Mode Select
;--------------------------------------------------------------------------
;COM1A1 	COM1A0 		Description
;0 			0 			Timer/Counter1 disconnected from output pin OC1
;0 			1 			Toggle the OC1 output line.
;1 			0 			Clear the OC1 output line (to zero).
;1 			1 			Set the OC1 output line (to one).
;--------------------------------------------------------------------------

;PWM Mode Select
;--------------------------------------------------------------------------
;PWM11 		PWM10 		Description
;0 			0 			PWM operation of Timer/Counter1 is disabled
;0 			1 			Timer/Counter1 is an 8-bit PWM
;1 			0 			Timer/Counter1 is a 9-bit PWM
;1 			1 			Timer/Counter1 is a 10-bit PWM
;--------------------------------------------------------------------------

;--------------------------------------------------------------------------
;Timer/Counter1 Control Register B – TCCR1B
;Bit 			7 		6 		5 		4 		3 		2 		1 		0
;$2E ($4E) 		ICNC1 	ICES1 	– 		– 		CTC1 	CS12 	CS11 	CS10
;Read/Write 	R/W 	R/W 	R 		R 		R/W 	R/W 	R/W 	R/W
;Initial value 	0 		0 		0 		0 		0 		0 		0 		0
;--------------------------------------------------------------------------
;• Bit 7 – ICNC1: Input Capture1 Noise Canceler (4 CKs)
;• Bit 6 – ICES1: Input Capture1 Edge Select
;• Bits 5, 4 – Res: Reserved Bits.
;• Bit 3 – CTC1: Clear Timer/Counter1 on Compare Match. In PWM mode, this bit has no effect.
;• Bits 2,1,0 – CS12, CS11, CS10: Clock Select1, Bits 2, 1 and 0

;Clock 1 Prescale Select
;--------------------------------------------------------------------------
;CS12 	CS11 	CS10 	Description
;0 		0 		0 		Stop, the Timer/Counter1 is stopped.
;0 		0 		1 		CK
;0 		1 		0 		CK/8
;0 		1 		1 		CK/64
;1 		0 		0 		CK/256
;1 		0 		1	 	CK/1024
;1 		1 		0 		External Pin T1, falling edge
;1 		1 		1	 	External Pin T1, rising edge
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
;Timer/Counter1 – TCNT1H and TCNT1L
;Bit 			15 		14 		13 		12 		11 		10 		9 		8	 ;TCNT1H
;$2D ($4D) 		MSB
;$2C ($4C)																LSB
;Bit 			7	 	6	 	5 		4 		3 		2 		1 		0	;TCNT1L
;Read/Write 	R/W 	R/W 	R/W 	R/W 	R/W 	R/W 	R/W 	R/W
;				R/W 	R/W 	R/W 	R/W 	R/W 	R/W 	R/W 	R/W
;Initial value 	0 		0 		0 		0 		0 		0 		0 		0
;				0 		0 		0 		0 		0 		0 		0 		0
;--------------------------------------------------------------------------

;**************************************************************************
;--------------------------------------------------
; T1max ms  (TCNT1 = 0)
;--------------------------------------------------
;  cFrequency (Hz)	8.000.000	
;    CK/1		0.125 us	x 65535 = 8.191875ms
;    CK/8		1 us		x 65535 = 65.535ms
;    CK/64		8 us		x 65535 = 0.52428s
;    CK/256		32 us		x 65535 = 2.09712s
;    CK/1024	0.128 ms	x 65535 = 8.38848s
;--------------------------------------------------
; Tt1 = (65535-Ct1)*t0
; Ct1 = 65535 - Tt1/t1
;**************************************************************************
;**************************************************************************
;--------------------------------------------------
.equ T1_Stop	= 0
.equ T1_0125us 	= 1
.equ T1_1us		= 2
.equ T1_8us		= 3
.equ T1_32us	= 4
.equ T1_128us	= 5
;--------------------------------------------------

;--------------------------------------------------
.macro m_enable_timerT1
	in	temp, TIFR
	cbr	temp, 1<<TOV1
	out	TIFR, temp
	in	temp, TIMSK
	sbr	temp, 1<<TOIE1
	out	TIMSK, temp
	ldi	temp, T0_8us	;Settings T1_time xx
	out	TCCR1B, temp
.endmacro
;--------------------------------------------------
