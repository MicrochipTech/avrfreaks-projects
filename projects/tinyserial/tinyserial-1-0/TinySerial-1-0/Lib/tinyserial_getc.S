;;; The Tinyserial library.
;;; Copyright (C) 2010 Timothy Jon Fraser <tfraser@alum.wpi.edu>
;;; 
;;; This file is part of the Tinyserial library.
;;; 
;;; The Tinyserial library is free software: you can redistribute it
;;; and/or modify it under the terms of the GNU Lesser General Public
;;; License as published by the Free Software Foundation, either version 3
;;; of the License, or (at your option) any later version.
;;; 
;;; The Tinyserial is distributed in the hope that it will be useful, but
;;; WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;;; Lesser General Public License for more details.
;;; 
;;; You should have received a copy of the GNU Lesser General Public
;;; License along with the Tinyserial library.  If not, see
;;; <http://www.gnu.org/licenses/>.
;;; 
;;; 
;;; This code respects the AVR Libc 1.6.5 ABI described here:
;;; http://savannah.nongnu.org/projects/avr-libc/
;;; so these routines should be callable from C.
;;; 
;;; This code uses some of the AVR Libc header files.
;;; I'm using avr-gcc to preprocess and assemble.
	
#include <avr/io.h>

;;; I'm breaking up the 16-bit address of UCSR0A into 8-bit high and low
;;; halves because the ldi instruction works only on 8-bit quantities.
#define UCSR0AL (UCSR0A & 0xFF)
#define	UCSR0AH ((UCSR0A >> 8) & 0XFF)
	
	.section .text

;;; ts_getc
;;; in:     r24          - TS_ECHO_OFF for echo off, TS_ECHO_ON for echo on.
;;; out:    r18, r27:r26 - trashed - should be OK according to GNU Libc ABI.
;;; return: r24          - characte read from USART.
;;;
;;; This function will block (busy-wait) until a character arrives at the
;;; USART and then return that character.
;;;
;;; If echo is on, this function will also ts_putchar each character back
;;; to the USART after it is read.

	.global ts_getc
ts_getc:
	;; Busy-wait for a character to arrive at the USART
	ldi r26, UCSR0AL	; load UCSR0A's address into X register
	ldi r27, UCSR0AH
wait:	ld r18, X		; load UCSR0A's value into register
	sbrs r18, RXC0		; skip rjmp if RXC0 bit is set
	rjmp wait		; RXC0 not set, loop back and try again

	;; If echo is on, echo character back to USART.
	tst r24			; If r24 is clear (echo off) set Z flag.
	lds r24, UDR0		; Move char read from USART to r24.
	breq end		; If Z flag set, skip echo.
	call ts_putc		; Call ts_putc to echo char to USART.
	
	;; Return received character
end:	ret			; Char is still in r24, return it.

	.end


