;
;             BUTTLOAD - Butterfly ISP Programmer
;
;              Copyright (C) Dean Camera, 2007.
;
;            dean [at] fourwalledcubicle [dot] com
;                  www.fourwalledcubicle.com
;
;           Released under the GPL Licence, Version 2.

; Routine sends a byte in the SPI format via the USI subsystem. Routine sets and returns the
; USI data according to the AVR-GCC ABI. The clock speed is configurable via a value preloaded
; into the GPIOR0 register. Interrupts are disabled during the transfer.
;
; No push/pops required; only the "Call-Saved" registers are used, which improves speed. For
; information on how GCC handles register management, see the "FAQ" section of the AVRLIBC manual.

#include "USI.h"

.global  USI_SPITransmit                                ; \ Link the USI_SPITransmit label to the function
.func    USI_SPITransmit                                ; / prototype declared in USI.h

; DEFINES:
#define  TOKEN_COMB(x, y)      x ## y

#define  MEM_ADDR(sfr)         _SFR_MEM_ADDR(sfr)
#define  IO_ADDR(sfr)          _SFR_IO_ADDR(sfr)

#define  HIGH(val)             (val >> 8)
#define  LOW(val)              (val & 0xFF)

#define  PH(sfrpair)           TOKEN_COMB(sfrpair, H)
#define  PL(sfrpair)           TOKEN_COMB(sfrpair, L)

#define  DELAY_3_CYCLES()      lpm r18, Z               ; Garbage command - wastes 3 cycles

#define  XL                    r26
#define  XH                    r27

; REGISTER ALLOCATIONS (all call-saved):
#define  USI_SCRATCH_ASM_REG   r20
#define  USI_CONTROL_ASM_REG   r21
#define  USI_CONTROLCK_ASM_REG r22
#define  USI_BITSREM_ASM_REG   r23
#define  USI_PARAM_ASM_REG     r24
#define  USI_DELAYCNT_ASM_REG  r25
#define  USI_USICR_PTR_ASM_REG X

; NAME:      | USI_SPITransmit
; PURPOSE:   | Transmits and recieves a single byte via the USI subsystem (in SPI mode)
; ARGUMENTS: | None
; RETURNS:   | None
USI_SPITransmit:
	sts   MEM_ADDR(USIDR), USI_PARAM_ASM_REG                       ; Input uint8_t stored in R24, place in USI data register
	
	ldi	  USI_CONTROL_ASM_REG,   (USICONTROLREGS)                  ; Preload values to be set into a temp reg for speed
	ldi	  USI_CONTROLCK_ASM_REG, (USICONTROLREGS | (1 << USICLK))  ; Preload values to be set into a temp reg for speed	

	ldi   PH(USI_USICR_PTR_ASM_REG), HIGH(MEM_ADDR(USICR))         ; XH = R27
	ldi   PL(USI_USICR_PTR_ASM_REG), LOW(MEM_ADDR(USICR))          ; XL = R26

	ldi   USI_BITSREM_ASM_REG, 8

	in    USI_SCRATCH_ASM_REG, IO_ADDR(USIDelay)                   ; Grab the delay value from the GPIOR0 register

	cli                                                            ; Kill interrupts while transferring

; DELAY CHECKS - Convert delay index into the actual cycle delay
	cpi   USI_SCRATCH_ASM_REG, 0                                   ; Test for 1843200 Hz frequency
	breq  SuperFastTransfer

	cpi   USI_SCRATCH_ASM_REG, 1                                   ; Test for 921600 Hz frequency
	breq  FastTransfer

	ldi   USI_DELAYCNT_ASM_REG, (((F_CPU / 230400) - 6) / 3)
	cpi   USI_SCRATCH_ASM_REG, 2                                   ; Test for 230400 Hz frequency
	breq  SlowTransfer
	
	ldi   USI_DELAYCNT_ASM_REG, (((F_CPU / 115200) - 6) / 3)
	cpi   USI_SCRATCH_ASM_REG, 3                                   ; Test for 115200 Hz frequency
	breq  SlowTransfer

	ldi   USI_DELAYCNT_ASM_REG, (((F_CPU /  57600) - 6) / 3)
	cpi   USI_SCRATCH_ASM_REG, 4                                   ; Test for 57600 Hz frequency
	breq  SlowTransfer

	ldi   USI_DELAYCNT_ASM_REG, (((F_CPU /  28800) - 6) / 3)
	cpi   USI_SCRATCH_ASM_REG, 5                                   ; Test for 28800 Hz frequency
	breq  SlowTransfer

	rjmp  ClockingTransfer                                         ; Otherwise continue to the clocking loop
; END DELAY CHECKS
	
; SLOW LOOP
SlowTransfer:
	; Cycle-counted to ensure 50% duty cycle per clock; equates to (((F_CPU / ProgFreq) + 6) * 3) cycles between toggles

	st	  USI_USICR_PTR_ASM_REG, USI_CONTROL_ASM_REG
	mov   USI_SCRATCH_ASM_REG, USI_DELAYCNT_ASM_REG

	DELAY_3_CYCLES()

  TimeLoop:
	dec   USI_SCRATCH_ASM_REG
	brne  TimeLoop
	nop

	st	  USI_USICR_PTR_ASM_REG, USI_CONTROLCK_ASM_REG
	mov   USI_SCRATCH_ASM_REG, USI_DELAYCNT_ASM_REG

  TimeLoop2:
	dec   USI_SCRATCH_ASM_REG
	brne  TimeLoop2
	nop

	dec   USI_BITSREM_ASM_REG
	brne  SlowTransfer
	
	rjmp  Epilogue
; END SLOW LOOP

; FAST LOOP
FastTransfer:
	; Cycle-counted to ensure 50% duty cycle per clock; equates to 4 cycles between toggles

	st    USI_USICR_PTR_ASM_REG, USI_CONTROL_ASM_REG
	nop                                                            ; NOP to maintain 50% duty cycle
	dec   USI_BITSREM_ASM_REG                                      ; DEC can be executed here since ST doesn't modify the Z flag
	st    USI_USICR_PTR_ASM_REG, USI_CONTROLCK_ASM_REG
	brne  FastTransfer

	rjmp  Epilogue
; END FAST LOOP

; SUPER FAST LOOP
SuperFastTransfer:
	; Cycle-counted to ensure 50% duty cycle per clock; equates to 2 cycles between toggles

	st    USI_USICR_PTR_ASM_REG, USI_CONTROL_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROLCK_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROL_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROLCK_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROL_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROLCK_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROL_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROLCK_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROL_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROLCK_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROL_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROLCK_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROL_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROLCK_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROL_ASM_REG
	st    USI_USICR_PTR_ASM_REG, USI_CONTROLCK_ASM_REG
		
	rjmp  Epilogue
; END SUPER FAST LOOP

; CLOCKING LOOP
ClockingTransfer:
	; Cycle-counted to ensure 50% duty cycle per clock; equates to 10 cycles between CLKOUT toggles
	; and approx 80 cycles between USI clock toggles, 8:1 ratio of CLKOUT clocks to USI clocks

	ldi   USI_DELAYCNT_ASM_REG, 24 ; CLKOUT gives 16 extra clocks at start to ensure target is ready

  ClockingLoop:
	sbi   IO_ADDR(USI_CLOCK_OUT_PORT), USI_CLOCK_BIT
	
	DELAY_3_CYCLES()
	DELAY_3_CYCLES()

	dec   USI_DELAYCNT_ASM_REG
	brne  ClockingLoop
	
	nop
	sbi   IO_ADDR(USI_CLOCK_OUT_PORT), USI_CLOCK_BIT
	st    USI_USICR_PTR_ASM_REG, USI_CONTROL_ASM_REG

	DELAY_3_CYCLES()
	DELAY_3_CYCLES()

	ldi   USI_DELAYCNT_ASM_REG, 8

  ClockingLoop2:
	sbi   IO_ADDR(USI_CLOCK_OUT_PORT), USI_CLOCK_BIT

	DELAY_3_CYCLES()
	DELAY_3_CYCLES()

	dec   USI_DELAYCNT_ASM_REG
	brne  ClockingLoop2

	nop
	sbi   IO_ADDR(USI_CLOCK_OUT_PORT), USI_CLOCK_BIT
	st    USI_USICR_PTR_ASM_REG, USI_CONTROLCK_ASM_REG

	DELAY_3_CYCLES()
	
	dec   USI_BITSREM_ASM_REG
	brne  ClockingTransfer
; END CLOCKING LOOP

Epilogue:
	sei
	lds   USI_PARAM_ASM_REG, MEM_ADDR(USIDR)                       ; Output uint8_t stored in R24, get from USI data register
	ret
