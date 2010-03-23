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

;;; ts_putc
;;; in:     r24 character to write to the USART
;;; out:    r27:r26, r18 trashed - should be OK according to GNU libc ABI
;;; return: nothing
;;;
;;; Use this function to write single characters to the USART.

	.global ts_putc
ts_putc:
	;; Busy-wait for the transmit buffer to be empty. Wait for the
	;; Data Register Empty Flag to be set in the USART Control and
	;; Status Register 0 A.
	ldi r26, UCSR0AL	; load UCSR0A's address into X register
	ldi r27, UCSR0AH
wait:	ld r18, X		; load UCSR0A's value into register
	sbrs r18, UDRE0		; skip rjmp if UDRE0 bit is set
	rjmp wait		; UDRE0 not set, loop back and try again
	
	;; Put the character into the transmit buffer.  This sends the char.
	sts UDR0, r24
	ret

	.end
