;
;             BUTTLOAD - Butterfly ISP Programmer
;
;              Copyright (C) Dean Camera, 2007.
;
;            dean [at] fourwalledcubicle [dot] com
;                  www.fourwalledcubicle.com
;
;           Released under the GPL Licence, Version 2.

; Routine provides a 32.768KHz clock to the target via the MOSI line, while listening for clocks on the
; MISO line. This is used to calibrate the target's OSCCAL value - see AVR053 for more details. Routine
; times out after a maximum of 1024 clocks.

#include "USI.h"

.global  AICI_SendCalibrationClocks                     ; \ Link the AICI_SendCalibrationClocks label to the function
.func    AICI_SendCalibrationClocks                     ; / prototype declared in AVRISPCommandInterpreter.h

; DEFINES:
#define  TOKEN_COMB(x, y)        x ## y

#define  IO_ADDR(sfr)            _SFR_IO_ADDR(sfr)

#define  HIGH(val)               (val >> 8)
#define  LOW(val)                (val & 0xFF)

#define  PH(sfrpair)             TOKEN_COMB(sfrpair, H)
#define  PL(sfrpair)             TOKEN_COMB(sfrpair, L)

#define  ZL                      r30
#define  ZH                      r31
#define  XL                      r26
#define  XH                      r27

; REGISTER ALLOCATIONS (all call-saved):
#define  CAL_PREVMISO_ASM_REG    r20
#define  CAL_DELAY_ASM_REG       r21
#define  CAL_RETCLOCKREM_ASM_REG r22
#define  CAL_PARAM_ASM_REG       r24
#define  CAL_CLOCKSREM_ASM_REG   X
#define  CAL_TEMPW_ASM_REG       Z


; NAME:      | AICI_SendCalibrationClocks
; PURPOSE:   | Sends 32.768KHz clocks via the MOSI line, and checks for pulses on the MISO line
; ARGUMENTS: | None
; RETURNS:   | 1 if sucessful, 0 otherwise 
AICI_SendCalibrationClocks:
	ldi  PL(CAL_CLOCKSREM_ASM_REG), LOW(1024 * 2)
	ldi  PH(CAL_CLOCKSREM_ASM_REG), HIGH(1024 * 2)

	ldi  CAL_RETCLOCKREM_ASM_REG, (4 * 2)

	cli

  SendLoop:
	sbi  IO_ADDR(PINE), USI_DATAOUT_PIN                   ; Toggle MOSI
	
	in   PL(CAL_TEMPW_ASM_REG), IO_ADDR(PINE)
	eor  PL(CAL_TEMPW_ASM_REG), CAL_PREVMISO_ASM_REG
	andi PL(CAL_TEMPW_ASM_REG), USI_DATAIN_PIN
	breq Continue

	dec  CAL_RETCLOCKREM_ASM_REG
	breq ExitOk

  Continue:
	nop
	
	in   CAL_PREVMISO_ASM_REG, IO_ADDR(PINE)
	
	ldi  CAL_DELAY_ASM_REG, ((((F_CPU / 32768) - 17) / 2) / 4)

  Delay:
	dec  CAL_DELAY_ASM_REG
	nop
	brne Delay
	nop

	subi PL(CAL_CLOCKSREM_ASM_REG), 1
	sbci PH(CAL_CLOCKSREM_ASM_REG), 0

	movw PL(CAL_CLOCKSREM_ASM_REG), PL(CAL_TEMPW_ASM_REG) ; Copy remaing clocks pair to temp pair
	eor  PH(CAL_TEMPW_ASM_REG), PL(CAL_TEMPW_ASM_REG)     ; XOR the two halves together
	brne SendLoop                                         ; Keep looping if timeout not reached

	ldi  CAL_PARAM_ASM_REG, 0
	rjmp Epilogue

  ExitOk:
	ldi  CAL_PARAM_ASM_REG, 1

  Epilogue:
	sei
	ret

