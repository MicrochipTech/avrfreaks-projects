/* Copyright (C) 1999 Denis Chertykov <denisc@overta.ru>

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

In addition to the permissions in the GNU General Public License, the
author gives you unlimited permission to link the
compiled version of this file into combinations with other programs,
and to distribute those combinations without any restriction coming
from the use of this file.  (The General Public License restrictions
do apply in other respects; for example, they cover modification of
the file, and distribution when not linked into a combine
executable.)

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.  */

/* Hacked a lot by Marek Michalkiewicz <marekm@linux.org.pl> to add
   support for many MCU types, etc.  These changes are public domain.  */

;; -*- mode: asm -*-

#include "macros.inc"

#if !defined (__AVR_ASM_ONLY__)
; not required by gas, but still a nice way to show referenced symbols
	.extern _U(main)
	.extern _etext
	.extern __data_start
	.extern _edata
	.extern __bss_start
	.extern __bss_end

; Stack init in main() (can override with avr-ld --defsym __stack=...)
	.weak __stack
__stack = RAMEND

; These too can be set with the avr-ld --defsym name=value option
; (MCUCR must be set up correctly to enable external SRAM).
	.weak	__init_mcucr__
	.weak	__init_emcucr__
	.weak	__init_wdtcr__

; You can plug in your own code to handle unexpected
; interrupts (which usually indicate a bug - if the program
; enables an interrupt for which there is no handler).
	.weak	_U(_unexpected_)
#endif /* if !defined (__AVR_ASM_ONLY__) */

; You can plug in your own asm code that runs immediately
; after reset, without any initialization at all.  When done,
; it should jump to _real_init_.
	.weak	_U(_init_)

	.macro	VEC name def
	.if . - .__start_of_init__ - \def
	.err
	.endif
	.weak	_U(\name)
#if !defined (__AVR_ASM_ONLY__)
	.set	_U(\name), _unexpected_1_
#endif
	XJMP	_U(\name)
	.endm

; reset and interrupt vectors, starting at absolute address 0

	.section .init, "ax", @progbits
	.func	.__start_of_init__
.__start_of_init__:
	XJMP	_U(_init_)
/* some devices have interrupt vectors in different order */
#if defined (__AVR_AT94K__)
/* many more interrupt vectors (some in different order) than other devices */
	VEC	_fpga_int0_, FPGA_INT0_vect
	VEC	_interrupt0_, INT0_vect
	VEC	_fpga_int1_, FPGA_INT1_vect
	VEC	_interrupt1_, INT1_vect
	VEC	_fpga_int2_, FPGA_INT2_vect
	VEC	_interrupt2_, INT2_vect
	VEC	_fpga_int3_, FPGA_INT3_vect
	VEC	_interrupt3_, INT3_vect
	VEC	_output_compare2_, TIMER2_COMP_vect
	VEC	_overflow2_, TIMER2_OVF_vect
	VEC	_input_capture1_, TIMER1_CAPT_vect
	VEC	_output_compare1a_, TIMER1_COMPA_vect
	VEC	_output_compare1b_, TIMER1_COMPB_vect
	VEC	_overflow1_, TIMER1_OVF_vect
	VEC	_output_compare0_, TIMER0_COMP_vect
	VEC	_overflow0_, TIMER0_OVF_vect
	VEC	_fpga_int4_, FPGA_INT4_vect
	VEC	_fpga_int5_, FPGA_INT5_vect
	VEC	_fpga_int6_, FPGA_INT6_vect
	VEC	_fpga_int7_, FPGA_INT7_vect
	VEC	_uart_recv_, UART_RX_vect
	VEC	_uart_data_, UART_UDRE_vect
	VEC	_uart_trans_, UART_TX_vect
	VEC	_fpga_int8_, FPGA_INT8_vect
	VEC	_fpga_int9_, FPGA_INT9_vect
	VEC	_fpga_int10_, FPGA_INT10_vect
	VEC	_fpga_int11_, FPGA_INT11_vect
	VEC	_uart1_recv_, UART1_RX_vect
	VEC	_uart1_data_, UART1_UDRE_vect
	VEC	_uart1_trans_, UART1_TX_vect
	VEC	_fpga_int12_, FPGA_INT12_vect
	VEC	_fpga_int13_, FPGA_INT13_vect
	VEC	_fpga_int14_, FPGA_INT14_vect
	VEC	_fpga_int15_, FPGA_INT15_vect
	VEC	_twi_, TWI_INT_vect
#elif defined (__AVR_ATtiny15__)
	VEC	_interrupt0_, INT0_vect
	VEC	_pin_change_, PIN_vect
	VEC	_output_compare1a_, TIMER1_COMPA_vect
	VEC	_overflow1_, TIMER1_OVF_vect
	VEC	_overflow0_, TIMER0_OVF_vect
	VEC	_eeprom_ready_, EE_RDY_vect
	VEC	_comparator_, ANA_COMP_vect
	VEC	_adc_, ADC_vect  /* below _eeprom_ready_ in other devices */
#else  /* most other devices (not AT94K, not ATtiny15) */
#ifdef INT0_vect
	VEC	_interrupt0_, INT0_vect
#endif
#ifdef INT1_vect
	VEC	_interrupt1_, INT1_vect
#endif
#ifdef INT2_vect
	VEC	_interrupt2_, INT2_vect
#endif
#ifdef INT3_vect
	VEC	_interrupt3_, INT3_vect
#endif
#ifdef INT4_vect
	VEC	_interrupt4_, INT4_vect
#endif
#ifdef INT5_vect
	VEC	_interrupt5_, INT5_vect
#endif
#ifdef INT6_vect
	VEC	_interrupt6_, INT6_vect
#endif
#ifdef INT7_vect
	VEC	_interrupt7_, INT7_vect
#endif
#ifdef PIN_vect
	VEC	_pin_change_, PIN_vect
#endif
#ifdef TIMER2_COMP_vect
	VEC	_output_compare2_, TIMER2_COMP_vect
#endif
#ifdef TIMER2_OVF_vect
	VEC	_overflow2_, TIMER2_OVF_vect
#endif
#ifdef TIMER1_CAPT_vect
	VEC	_input_capture1_, TIMER1_CAPT_vect
#endif
#ifdef TIMER1_COMPA_vect
	VEC	_output_compare1a_, TIMER1_COMPA_vect
#endif
#ifdef TIMER1_COMPB_vect
	VEC	_output_compare1b_, TIMER1_COMPB_vect
#endif
#ifdef TIMER1_OVF_vect
	VEC	_overflow1_, TIMER1_OVF_vect
#endif
#ifdef TIMER0_COMP_vect
	VEC	_output_compare0_, TIMER0_COMP_vect
#endif
#ifdef TIMER0_OVF_vect
	VEC	_overflow0_, TIMER0_OVF_vect
#endif
#ifdef SPI_STC_vect
	VEC	_spi_, SPI_STC_vect
#endif
#ifdef UART_RX_vect
	VEC	_uart_recv_, UART_RX_vect
#endif
#ifdef UART1_RX_vect
	VEC	_uart1_recv_, UART1_RX_vect
#endif
#ifdef UART_UDRE_vect
	VEC	_uart_data_, UART_UDRE_vect
#endif
#ifdef UART1_UDRE_vect
	VEC	_uart1_data_, UART1_UDRE_vect
#endif
#ifdef UART_TX_vect
	VEC	_uart_trans_, UART_TX_vect
#endif
#ifdef UART1_TX_vect
	VEC	_uart1_trans_, UART1_TX_vect
#endif
#ifdef ADC_vect
	VEC	_adc_, ADC_vect
#endif
#ifdef EE_RDY_vect
	VEC	_eeprom_ready_, EE_RDY_vect
#endif
#ifdef ANA_COMP_vect
	VEC	_comparator_, ANA_COMP_vect
#endif
#ifdef TWI_INT_vect
	VEC	_twi_, TWI_INT_vect
#endif
#endif /* most other devices (not AT94K, not ATtiny15) */

	/* extra check */
	.if . - .__start_of_init__ - INT_VECT_SIZE
	.err
	.endif

	.endfunc

#if !defined (__AVR_ASM_ONLY__)
	.section .text, "ax", @progbits
	.func	.__c_startup__
.__c_startup__:
; two remporary registers (usable with ldi, not pointer registers)
#define r_tmp1 r18
#define r_tmp2 r19

	.global _U(_real_init_)
_U(_real_init_):
_U(_init_):
	clr	__zero_reg__
	out	SREG, __zero_reg__
#ifdef WDTCR
	ldi	r_tmp1, lo8(__init_wdtcr__)
	wdr
	out	WDTCR, r_tmp1
#endif
#ifdef MCUCR
	ldi	r_tmp1, lo8(__init_mcucr__)
	out	MCUCR, r_tmp1
#endif
#ifdef EMCUCR
	ldi	r_tmp1, lo8(__init_emcucr__)
	out	EMCUCR, r_tmp1
#endif
; XXX should we also read, save and clear MCUSR (if present) here?
; (datasheets suggest to do it as soon as possible after reset)
	ldi	ZL, lo8(_etext); load the end of .text segment
	ldi	ZH, hi8(_etext); this is a start of .data image
#if BIG_CODE
	ldi	r_tmp2, hh8(_etext)	; this will be in RAMPZ for "elpm"
#endif
	ldi	XL, lo8(__data_start)	; load start of ram
	ldi	XH, hi8(__data_start)
#if XRAMEND > 0x100  /* don't bother with high byte if we know it must be 0 */
	ldi	r_tmp1, hi8(_edata)
#endif
	LPM_R0_ZPLUS_INIT r_tmp2
	rjmp	.copy_data_start
.copy_data_loop:
	LPM_R0_ZPLUS_NEXT r_tmp2
	st	X+, r0
.copy_data_start:
	cpi	XL, lo8(_edata)
#if XRAMEND > 0x100
	cpc	XH, r_tmp1	; hi8(_edata)
#endif
	brne	.copy_data_loop
	;; avr-ld always put .bss after .data
	; If the above is true, then no need to load X again below.
	; But there are two separate symbols _edata and __bss_start
	; which seem to have the same value but are initialized
	; in two different places in the linker script.
#if 1
	ldi	XL, lo8(__bss_start)
	ldi	XH, hi8(__bss_start)
#endif
#if XRAMEND > 0x100
	ldi	r_tmp1, hi8(__bss_end)
#endif
	rjmp	.zero_bss_start
.zero_bss_loop:
	st	X+, __zero_reg__
.zero_bss_start:
	cpi	XL, lo8(__bss_end)
#if XRAMEND > 0x100
	cpc	XH, r_tmp1	; hi8(__bss_end)
#endif
	brne	.zero_bss_loop
	XJMP	_U(main)

_unexpected_1_:
	XJMP	_U(_unexpected_)

_U(_unexpected_):
	reti

	.endfunc
#endif /* if !defined (__AVR_ASM_ONLY__) */

