;***** Thermometer with I2C Bus ******************************
;*
;* Project		:Miscellaneous
;* Filename		:TH03_V1.asm
;* Device		:AT90S2313	 8.0 MHz (0.125 us)
;* Created		:
;* Last Updated	:
;* Version		:1.0
;*
;* DESCRIPTION	Measurement of temperature in a range -55'C to 125'C (+-3'C max)
;*													-25'C to 100'C (+-2'C max)
;*
;***************************************************************************
.include "2313def.inc"
.include "Ports_2313.asm"
.include "MemoryMap_2313.asm"
.LISTMAC
.def temp	=r16	;temp register
.def flags	=r21
	.equ	RD_I2C	=0
	.equ	lf	=1
;**************************************************************************

;***** START COVER ********************************************************
.cseg
.org $0000
	rjmp RESET 		; Reset Handler
	rjmp Z ;EXT_INT0 	; IRQ0 Handler
	rjmp Z ;EXT_INT1 	; IRQ1 Handler
	rjmp Z ;TIM_CAPT1 	; Timer1 Capture Handler
	rjmp Z ;TIM_COMP1 	; Timer1 Compare Handler
	rjmp TIM_OVF1 	; Timer1 Overflow Handler
	rjmp TIM_OVF0 	; Timer0 Overflow Handler
	rjmp Z ;UART_RXC 	; UART RX Complete Handler
	rjmp Z ;UART_DRE 	; UDR Empty Handler
	rjmp Z ;UART_TXC 	; UART TX Complete Handler
	rjmp Z ;ANA_COMP 	; Analog Comparator Handler
Z:

RESET:
	MAIN: ldi r16,low(RAMEND); Main program start
	out SPL,r16


clr zl
clr zh
clr r0
clr r1
clr r2
clr r16
clr r17
clr r18
clr r19
clr r21
clr r22

;***** Set Pin Port D (ANODES & INPUT)***
	ldi temp,set_D_port_DDR
	out DDRD,temp
	ldi temp,set_D_port
	out PORTD,temp

;***** Set Pin Port B (SEGMENTS)***
	ldi temp,set_B_port_DDR
	out DDRB,temp
	ldi temp,set_B_port
	out PORTB,temp

;***** 
	ldi temp,8
	sts S_SYM0,temp
	sts S_SYM1,temp
	sts S_SYM2,temp
	sts S_SYM3,temp

	sei
;**************************************************************************
.include "Timer0_2313.asm"
.include "Timer1_2313.asm"
m_enable_timerT0
m_enable_timerT1
;**************************************************************************

M:
		sbrs flags, RD_I2C
		rjmp M

		rcall read_data
		rcall CONVERT_FF_255
		cbr flags, (1<< RD_I2C)

		rjmp M

;**************************************************************************
.include "SymDef_0F_Inv.asm"
.include "Display_4LED.asm"
.include "I2CM_T.asm"
.include "FF_255.asm"
;**************************************************************************
TIM_OVF1:
sbr flags, (1<<rd_i2c)
reti