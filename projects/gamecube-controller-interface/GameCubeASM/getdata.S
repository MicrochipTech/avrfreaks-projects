; Getdata.S
#include <avr/io.h>

.global GetData

GetData:
		movw 	r30,r24				; load Z with Bytes location from passed pointer
		adiw 	r30,8				; add 8 to Z first bit to send is 9'th bit
		ldi		r20, 0x1A			;load r20 with 26 one more than command string 
10:									;10: ****** Start Of Command String Transmit ********
		dec		r20				
		breq	16f					; jump to end of transmit
		sbi		0x14, 0x05			; pull line low to start a bit send
		ld		r18, Z+	 			; load Bytes Value from memory to Z
		cpi		r18, 0				; check if it is a 1 or 0
		brne	13f					; value is highbit jump to highbit
		ldi		r19, 0x0D			; load r19 with 13 for timing loop loop to keep line low for 3uS total
11:									;11:
		dec		r19					; decrement r19	
		brne	11b					; check if r19 = 0 back to 11 if not
		nop							; for timing
		nop							; ditto
		cbi		0x14, 0x05			; Release line to high
		ldi		r19,0x03			; Second timing loop while line is high short to keep line high for 1uS
12:									;12:
		dec		r19					; decrement r19
		brne	12b					; r19 = 0 ?
		nop							; Another timing NOP
		rjmp	10b					; back to top for next bit
13:									;13: Start of High Bit Transmit
		ldi		r19, 0x02			; Short loop since line is already low
14:									;14:
		dec		r19					; decrement r19
		brne	14b					; r19 = 0 ?
		nop							; Timing
		nop							; ditto
		cbi		0x14, 0x05			; line back to high
		ldi		r19, 0x0E			; load r19 with 14 for long loop for highbit
15:									;15:
		dec		r19					; decrement r19	
		brne	15b					; back to 15
		rjmp	10b					; back to top of loop
16:									; ******************* End Of Transmit ****************
; ****** receive loops 64 times to receive all bits
; during receive - compact 64 received bits to
; first 8 bits of Bytes.
; Bytes[(r20>>3)] |= r19 to set
; Bytes[(r20>>3)] &= ~r19 to clear
		ldi		r20, 0				; load r20 with 0 for 0-63 loop
		ldi		r19, 0x80			; mask load r19 with 0x80 high bit set
		rjmp	21f					; jump to receive start
20:
		inc		r20					; increment loop counter
		cpi		r20, 64				; does loop counter = 64 ?
		breq	99f					; jump to end if = 64
		lsr		r19					; shift mask bit once to right
		brcc	21f					; if mask <> 0 jmp to 21
		ldi		r19, 0x80			; reload mask value
21:
		sbic	0x13, 0x05			; wait for controller to pull line low
		rjmp	21b					; 
		movw 	r30, r24			; copy r24,25 to r30,31
		clr		r22					; clear r22 in prepeation for add
		mov		r21, r20			; copy r20 to 21 for a temp register
		lsr		r21					;
		lsr		r21					;
		lsr		r21					; r21/8
		add		r30, r21    		; Add low byte
		adc		r31, r22    		; Add with carry high byte
		ld		r18, Z				; load r18 with Byte Value
		ldi		r21, 0x04			; load r21 with timing loop value
22:									; waiting for center of bit send to test if value is high or low
		dec		r21					; decrement r21
		brne	22b					; r21 = 0?
		sbic	0x13, 0x05			; check if bit is low
		rjmp	24f					; jump to high
		mov		r21, r19			; start of low bit receive
		com		r21					; not r21
		and		r18, r21			; r18=r18 and r21
		st		Z, r18				; 
23:									; wait for line to switch high
		sbis	0x13, 0x05			; so we don't catch the same bit low
		rjmp	23b
		rjmp	20b
24:									; start of highbit
		or		r18, r19			; r18=r18 or r19
		st		Z, r18
		rjmp	20b
99:				
		ret
