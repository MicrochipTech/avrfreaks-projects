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

;; ASCII NUL
#define NUL 0x00
	
	.section .text

;;; ts_puts
;;; in:     r25:r24 16-bit address of NUL-terminated C string in FLASH to
;;;                 write to USART.
;;; out:    r31:30 trashed - should be OK according to GNU libc ABI.
;;;         r27:r26, r18, r24 also trashed by calling ts_putc().
;;; return: nothing
;;;
;;; Use this function to write NUL-terminated C strings from Flash RAM
;;; to the USART.

	.global ts_putsf
ts_putsf:
	movw r30, r24	 	; move string addr into Z register
loop:	lpm r24, Z+		; load char *Z into r24 and Z++
	cpi r24, NUL		; If *Z is NUL we have reach end of string...
	breq done		; ... branch to done.
	call ts_putc		; send *Z to USART
	rjmp loop		; loop back for next char in string
done:	ret

	.end
