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

;;; ASCII codes for line-ending characters.
#define NUL 0x00
#define LF  0x0A
#define CR  0x0D

	
	.section .text

;;; ts_gets
;;; in:     r25:r24 16-bit address of buffer to hold string
;;;         r23:r22 16-bit size of buffer in bytes
;;;         r20     TS_ECHO_ON or TS_ECHO_OFF
;;; out:    r31:30, trashed - should be OK according to GNU libc ABI.
;;;         r27:r26 also trashed by calling ts_putc().
;;; return: r25:r24 16-bit count of characters placed in buffer.
;;;
;;; Use this function to read strings from the USART.
;;; 
;;; Rules for detecting end of input:
;;; 1. CR indicates end of input.
;;; 2. LF indicates end of input.
;;; 3. NUL indicates end of input.

	
	.global ts_gets
ts_gets:
	push r29		; I need to use the Y reg (r29:r28)...
	push r28		; ... save whatever caller left there.
	ldi r29, 0x00		; Y (r19:r18) will be our count of chars...
	ldi r28, 0x00		; ... stored in buffer.  Init to 0x0000.
	movw r30, r24	 	; Move buffer addr into Z register.
loop:	cp r28, r22		; If we've stored as many chars as we can...
	cpc r29, r23		; ... then we're done.
	brsh done
	mov r24, r20		; Pass echo on/off parm to ts_getc.
	call ts_getc		; Get char from USART into r24
	cpi r24, CR		; '\r' means end of input.
	breq done
	cpi r24, LF		; '\n' means end of input.
	breq done
	cpi r24, NUL		; '\0' means end of input.
	breq done
	st Z+, r24		; Store char from USART into buffer[Z], Z++.
	adiw r28, 0x01		; Increment our count of chars stored.
	rjmp loop		; Loop back for next char in string.
done:	movw r24, r28		; Return 16-bit count of chars stored.
	pop r28			; Restore the Y reg (r29:r28).
	pop r29
	ret

	.end
