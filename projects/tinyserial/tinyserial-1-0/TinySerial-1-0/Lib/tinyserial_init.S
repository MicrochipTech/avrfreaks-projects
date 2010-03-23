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

	.section .text

;;; ts_init
;;; in:     r25:r24 baud rate config value.  Use TS_CONFIG_* from header.
;;; 	    r22     mode bitmask.  Use TS_MODE_* from header.
;;; out:    nothing
;;; return: nothing
;;;
;;; Call this before you attempt to read or write from the serial device.
;;; This function sets up the USART in what the data sheet calls its
;;; "asynchronous normal" operating mode.

	.global ts_init
ts_init:
	;; Set USART 0 Baud Rate Register to the value appropriate
	;; for our desired baud rate using the formula in table
	;; 19-1 of the data sheet.
	;; (clockfrequency / (16 * baudrate)) - 1
	sts UBRR0H, r25
	sts UBRR0L, r24

	;; Set USART MSPIM Control and Status Register 0 B to put
	;; the USART in the readonly/writeonly/redwrite mode we want.
	sts UCSR0B, r22
	ret

	.end
	