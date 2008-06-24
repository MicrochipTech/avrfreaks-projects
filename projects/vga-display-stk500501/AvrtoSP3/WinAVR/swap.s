#include <avr/io.h>

	.section .text

	.global Disable
	.global Enable

Disable:

	in	r24,_SFR_IO_ADDR(SREG)		;get status register
	cli								;disable interrupt
	andi r24,0x80					;strip off interrupt bit
	ret

Enable:
	in r25,_SFR_IO_ADDR(SREG)
	or r25,r24
	out _SFR_IO_ADDR(SREG),r25
	ret

	.end
