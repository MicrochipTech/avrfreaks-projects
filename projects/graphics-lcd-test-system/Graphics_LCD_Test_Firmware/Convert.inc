; ********************************************************
; * Number conversion routines, Version 0.1 January 2002 *
; * (C)2002 by http://www.avr-asm-tutorial.net           *
; * additional routines added 8-15-2010 by Chris Schroeder
; ********************************************************
;
; The following rules apply for all conversion routines:
; - Errors during conversion set the T-bit in the status
;   register.
; - Z points to either SRAM (Z&gt;=$0060) or to registers
;   ($0001 .. $001D, exclude R0, R16 and R30/31).
; - ASCII- and BCD-coded numbers with multiple digits are
;   placed with higher significant digits at lower adres-
;   ses. Z should always point to the most significant
;   digit.
; - 16-bit-binary values are generally located in the
;   registers rBin1H:rBin1L. These must be defined within
;   the calling program.
; - Register rmp (range: R16..R29) is used within the
;   routines, its content is changed.
; - Register pair Z is used within the routines, its
;   content is set depending on the result.
; - Some routines use register R0 temporarily, its content
;   is restored.
; - Due to the use of the Z-Register pair the header file
;   for the target processor (xxxxdef.inc) must be inclu-
;   ded in the calling program or ZH (R31) and ZL (R30)
;   must be defined manually. Otherwise an error message
;   results, or you will get crazy things going on, when
;   you run the program.
; - Because subroutines and push/pop-operations are used
;   within these routines you must set the stack pointer
;   (SPH:SPL, resp. SPL for devices with equal or less
;   than 256 Byte internal SRAM).
;
; ***************** Routines Overview ********************
; Routines   Input        Conditions           Out, Errors
; --------------------------------------------------------
; AscToBin2  Z points on  stops at first non-  16-bit-bin
;            first ASCII  non-decimal digit,   rBin1H:L,
;            char         ignores leading      Overflow
;                         blanks and zeros
; Asc5ToBin2 Z points on  requires exact 5     16-bit-bin
;            first ASCII  valid digits,        rBin1H:L,
;            char         ignores leading      Overflow
;                         blanks and zeros     and non-dec
; Bcd5ToBin2 5-digit-BCD  requires exact 5     16-bit-bin
;            Z points to  valid digits         rBin1H:L
;            first digit                       Overflow
;                                              and non-BCD
; Bin2ToBcd5 16-bit-bin   Z points to first    5-digit-BCD
;            in rBin1H:L  BCD digit            Z on first,
;                                              no errors
; Bin2ToHex4 16-bit-bin   Z points to first    4-digit-hex
;            in rBin1H:L  Hex ASCII digit,     Z on first
;                         hex digits A..F      no errors
; Hex4ToBin2 4-digit-hex  Z points to first    16-bit-bin
;            Z points to  hex ASCII digit,     rBin1H:L,
;            first char   requires 4 digits,   invalid hex
;                         A..F or a..f ok      digit
; ******************* Conversion code ********************
;


; Package I: From ASCII resp. BCD to binary
;
; AscToBin2
; =========
; converts an ASCII coded number to a 2-Byte bi-
; nary
; In: Z points to first digit, conversion stops at first
;   digit detected or if overflow of the result occurs,
;   end of number must be terminated by a non-decimal
;   ASCII-digit!
; Out: Z points to first non-valid digit or to the digit
;   where the overflow occurred, if number is valid the
;   T-Flag is clear and the number is in registers
;   rBin1H:rBin1L
; Used registers: rBin1H:L (result), rBin2H:L (restored
;   after use), rmp
; Called subroutines: Bin1Mul10
;
AscToBin2:
	clr rBin1H ; Clear the result
	clr rBin1L
	clt ; Clear error flag bit
AscToBin2a:
	ld rmp,Z+ ; ignore leading blanks and zeros
	cpi rmp,' ' ; blank?
	breq AscToBin2a
	cpi rmp,'0' ; zero?
	breq AscToBin2a
AscToBin2b:
	subi rmp,'0' ; subtract ASCII zero
	brcs AscToBin2d ; End of the number
	cpi rmp,10 ; check invalid digit
	brcc AscToBin2d ; No-decimal char
	rcall Bin1Mul10 ; Multiply binary number by 10
	brts AscToBin2c ; overflow, return with T-Flag set
	add rBin1L,rmp ; add the digit to the binary
	ld rmp,Z+ ; read next char
	brcc AscToBin2b ; no overflow to binary MSB
	inc rBin1H ; Overflow to binary MSB
	brne AscToBin2b ; no overflow of binary MSB
	set ; Set overflow flag
AscToBin2c:
	sbiw ZL,1 ; Back one char, last char end/invalid
AscToBin2d:
	ret
;
; Asc5ToBin2
; ==========
; converts a 5-digit ASCII to a 16-bit-binary
; In: Z points to first decimal ASCII-digit, leading
;   blanks and zeros are ok. Requires exact 5 digits.
; Result: T-Flag reports result:
;   T=0: result in rBin1H:rBin1L, valid, Z points to
;     first digit of the hex-ASCII-number
;   T=1: error, Z points to the first illegal character
;     or to the digit, where the overflow occurred
; Used registers: rBin1H:L (result), R0 (restored after
;   use), rBin2H:L (restored after use), rmp
; Called subroutines: Bin1Mul10
;
Asc5ToBin2:
	push R0 ; R0 is used as counter, save it first
	ldi rmp,6 ; five chars, one too much
	mov R0,rmp
	clr rBin1H ; Clear result
	clr rBin1L
	clt ; Clear T-Bit
Asc5ToBin2a:
	dec R0 ; all chars read?
	breq Asc5ToBin2d ; last char
	ld rmp,Z+ ; read a char
	cpi rmp,' ' ; ignore blanks
	breq Asc5ToBin2a ; next char
	cpi rmp,'0' ; ignore leading zeros
	breq Asc5ToBin2a ; next char
Asc5ToBin2b:
	subi rmp,'0' ; treat digit
	brcs Asc5ToBin2e ; Last char was invalid
	cpi rmp,10 ; digit &gt; 9
	brcc Asc5ToBin2e ; Last char invalid
	rcall Bin1Mul10 ; Multiply result by 10
	brts Asc5ToBin2e ; Overflow occurred
	add rBin1L,rmp ; add the digit
	ld rmp,z+
	brcc Asc5ToBin2c ; no overflow to MSB
	inc rBin1H ; Overflow to MSB
	breq Asc5ToBin2e ; Overflow of MSB
Asc5ToBin2c:
	dec R0 ; downcount number of digits
	brne Asc5ToBin2b ; convert more chars
Asc5ToBin2d: ; End of ASCII number reached ok
	sbiw ZL,5 ; Restore start position of ASCII number
	pop R0 ; Restore register R0
	ret
Asc5ToBin2e: ; Last char was invalid
	sbiw ZL,1 ; Point to invalid char
	pop R0 ; Restore register R0
	set ; Set T-Flag for error
	ret ; and return with error condition set
;
; Bcd5ToBin2
; ==========
; converts a 5-bit-BCD to a 16-bit-binary
; In: Z points to the most signifant digit of the BCD
; Out: T-flag shows general result:
;   T=0: Binary in rBin1H:L is valid, Z points to the
;     first digit of the BCD converted
;   T=1: Error during conversion. Either the BCD was too
;     big (must be 0..65535, Z points to BCD where the
;     overflow occurred) or an illegal BCD was detected
;     (Z points to the first non-BCD digit).
; Used registers: rBin1H:L (result), R0 (restored after
;   use), rBin2H:L (restored after use), rmp
; Called subroutines: Bin1Mul10
;
Bcd5ToBin2:
	push R0 ; Save register
	clr rBin1H ; Empty result
	clr rBin1L
	ldi rmp,5 ; Set counter to 5
	mov R0,rmp
	clt ; Clear error flag
Bcd5ToBin2a:
	ld rmp,Z+ ; Read BCD digit
	cpi rmp,10 ; is it valid?
	brcc Bcd5ToBin2c ; invalid BCD
	rcall Bin1Mul10 ; Multiply result by 10
	brts Bcd5ToBin2c ; Overflow occurred
	add rBin1L,rmp ; add digit
	brcc Bcd5ToBin2b ; No overflow to MSB
	inc rBin1H ; Overflow to MSB
	breq Bcd5ToBin2c ; Overflow of MSB
Bcd5ToBin2b:
	dec R0 ; another digit?
	brne Bcd5ToBin2a ; Yes
	pop R0 ; Restore register
	sbiw ZL,5 ; Set to first BCD digit
	ret ; Return
Bcd5ToBin2c:
	sbiw ZL,1 ; back one digit
	pop R0 ; Restore register
	set ; Set T-flag, error
	ret ; and return
;
; Bin1Mul10
; =========
; multiplies a 16-bit-binary by 10
; Sub used by: AscToBin2, Asc5ToBin2, Bcd5ToBin2
; In: 16-bit-binary in rBin1H:L
; Out: T-flag shows general result:
;   T=0: Valid result, 16-bit-binary in rBin1H:L ok
;   T=1: Overflow occurred, number too big
;
Bin1Mul10:
	push rBin2H ; Save the register of 16-bit-binary 2
	push rBin2L
	mov rBin2H,rBin1H ; Copy the number
	mov rBin2L,rBin1L
	add rBin1L,rBin1L ; Multiply by 2
	adc rBin1H,rBin1H
	brcs Bin1Mul10b ; overflow, get out of here
Bin1Mul10a:
	add rBin1L,rbin1L ; again multiply by 2 (4*number reached)
	adc rBin1H,rBin1H
	brcs Bin1Mul10b ; overflow, get out of here
	add rBin1L,rBin2L ; add the copied number (5*number reached)
	adc rBin1H,rBin2H
	brcs Bin1Mul10b ;overflow, get out of here
	add rBin1L,rBin1L ; again multiply by 2 (10*number reached)
	adc rBin1H,rBin1H
	brcc Bin1Mul10c ; no overflow occurred, don't set T-flag
Bin1Mul10b:
	set ; an overflow occurred during multplication
Bin1Mul10c:
	pop rBin2L ; Restore the registers of 16-bit-binary 2
	pop rBin2H
	ret
;
; *************************************************
;
; Package II: From binary to ASCII resp. BCD
;
; Bin2ToAsc5
; ==========
; converts a 16-bit-binary to a 5 digit ASCII-coded decimal
; In: 16-bit-binary in rBin1H:L, Z points to the highest
;   of 5 ASCII digits, where the result goes to
; Out: Z points to the beginning of the ASCII string, lea-
;   ding zeros are filled with blanks
; Used registers: rBin1H:L (content is not changed),
;   rBin2H:L (content is changed), rmp
; Called subroutines: Bin2ToBcd5
;
Bin2ToAsc5:
	rcall Bin2ToBcd5 ; convert binary to BCD
	ldi rmp,4 ; Counter is 4 leading digits
	mov rBin2L,rmp
Bin2ToAsc5a:
	ld rmp,z ; read a BCD digit
	tst rmp ; check if leading zero
	brne Bin2ToAsc5b ; No, found digit &gt;0
	ldi rmp,' ' ; overwrite with blank
	st z+,rmp ; store and set to next position
	dec rBin2L ; decrement counter
	brne Bin2ToAsc5a ; further leading blanks
	ld rmp,z ; Read the last BCD
Bin2ToAsc5b:
	inc rBin2L ; one more char
Bin2ToAsc5c:
	subi rmp,-'0' ; Add ASCII-0
	st z+,rmp ; store and inc pointer
	ld rmp,z ; read next char
	dec rBin2L ; more chars?
	brne Bin2ToAsc5c ; yes, go on
	sbiw ZL,5 ; Pointer to beginning of the BCD
	ret ; done
;
; Bin2ToAsc
; =========
; converts a 16-bit-binary to a 5-digit ASCII coded decimal,
;   the pointer points to the first significant digit of the
;   decimal, returns the number of digits
; In: 16-bit-binary in rBin1H:L, Z points to first digit of
;   the ASCII decimal (requires 5 digits buffer space, even
;   if the number is smaller!)
; Out: Z points to the first significant digit of the ASCII
;   decimal, rBin2L has the number of characters (1..5)
; Used registers: rBin1H:L (unchanged), rBin2H (changed),
;   rBin2L (result, length of number), rmp
; Called subroutines: Bin2ToBcd5, Bin2ToAsc5
;
Bin2ToAsc:
	rcall Bin2ToAsc5 ; Convert binary to ASCII
	ldi rmp,6 ; Counter is 6
	mov rBin2L,rmp
Bin2ToAsca:
	dec rBin2L ; decrement counter
	ld rmp,z+ ; read char and inc pointer
	cpi rmp,' ' ; was a blank?
	breq Bin2ToAsca ; Yes, was a blank
	sbiw ZL,1 ; one char backwards
	ret ; done
;
; Bin2ToBcd5
; ==========
; converts a 16-bit-binary to a 5-digit-BCD
; In: 16-bit-binary in rBin1H:L, Z points to first (most significant) digit
;   where the result goes to
; Out: 5-digit-BCD, Z points to first BCD-digit
; Used registers: rBin1H:L (unchanged), rBin2H:L (changed),
;   rmp
; Called subroutines: Bin2ToDigit
;
Bin2ToBcd5:
	push rBin1H ; Save number
	push rBin1L
	ldi rmp,HIGH(10000) ; Start with tenthousands
	mov rBin2H,rmp
	ldi rmp,LOW(10000)
	mov rBin2L,rmp
	rcall Bin2ToDigit ; Calculate digit
	ldi rmp,HIGH(1000) ; Next with thousands
	mov rBin2H,rmp
	ldi rmp,LOW(1000)
	mov rBin2L,rmp
	rcall Bin2ToDigit ; Calculate digit
	ldi rmp,HIGH(100) ; Next with hundreds
	mov rBin2H,rmp
	ldi rmp,LOW(100)
	mov rBin2L,rmp
	rcall Bin2ToDigit ; Calculate digit
	ldi rmp,HIGH(10) ; Next with tens
	mov rBin2H,rmp
	ldi rmp,LOW(10)
	mov rBin2L,rmp
	rcall Bin2ToDigit ; Calculate digit
	st z,rBin1L ; Remainder are ones
	sbiw ZL,4 ; Put pointer to first BCD
	pop rBin1L ; Restore original binary
	pop rBin1H
	ret ; and return
;
; Bin2ToDigit
; ===========
; converts one decimal digit by continued subraction of a
;   binary coded decimal
; Used by: Bin2ToBcd5, Bin2ToAsc5, Bin2ToAsc
; In: 16-bit-binary in rBin1H:L, binary coded decimal in
;   rBin2H:L, Z points to current BCD digit
; Out: Result in Z, Z incremented
; Used registers: rBin1H:L (holds remainder of the binary),
;   rBin2H:L (unchanged), rmp
; Called subroutines: -
;
Bin2ToDigit:
	clr rmp ; digit count is zero
Bin2ToDigita:
	cp rBin1H,rBin2H ; Number bigger than decimal?
	brcs Bin2ToDigitc ; MSB smaller than decimal
	brne Bin2ToDigitb ; MSB bigger than decimal
	cp rBin1L,rBin2L ; LSB bigger or equal decimal
	brcs Bin2ToDigitc ; LSB smaller than decimal
Bin2ToDigitb:
	sub rBin1L,rBin2L ; Subtract LSB decimal
	sbc rBin1H,rBin2H ; Subtract MSB decimal
	inc rmp ; Increment digit count
	rjmp Bin2ToDigita ; Next loop
Bin2ToDigitc:
	st z+,rmp ; Save digit and increment
	ret ; done
;
; **************************************************
;
; Package III: From binary to Hex-ASCII
;
; Bin2ToHex4
; ==========
; converts a 16-bit-binary to uppercase Hex-ASCII
; In: 16-bit-binary in rBin1H:L, Z points to first
;   position of the four-character Hex-ASCII
; Out: Z points to the first digit of the four-character
;   Hex-ASCII, ASCII digits A..F in capital letters
; Used registers: rBin1H:L (unchanged), rmp
; Called subroutines: Bin1ToHex2, Bin1ToHex1
;
Bin2ToHex4:
	mov rmp,rBin1H ; load MSB
	rcall Bin1ToHex2 ; convert byte
	mov rmp,rBin1L
	rcall Bin1ToHex2
	sbiw ZL,4 ; Set Z to start
	ret
;
; Bin1ToHex2 converts an 8-bit-binary to uppercase hex
; Called by: Bin2ToHex4
;
Bin1ToHex2:
	push rmp ; Save byte
	swap rmp ; upper to lower nibble
	rcall Bin1ToHex1
	pop rmp ; Restore byte
Bin1ToHex1:
	andi rmp,$0F ; mask upper nibble
	subi rmp,-'0' ; add 0 to convert to ASCII
	cpi rmp,'9'+1 ; A..F?
	brcs Bin1ToHex1a
	subi rmp,-7 ; add 7 for A..F
Bin1ToHex1a:
	st z+,rmp ; store in target
	ret ; and return
;
; *******************************************
;
; Package IV: From Hex-ASCII to binary
;
; Hex4ToBin2
; converts a 4-digit-hex-ascii to a 16-bit-binary
; In: Z points to first digit of a Hex-ASCII-coded number
; Out: T-flag has general result:
;   T=0: rBin1H:L has the 16-bit-binary result, Z points
;     to the first digit of the Hex-ASCII number
;   T=1: illegal character encountered, Z points to the
;     first non-hex-ASCII character
; Used registers: rBin1H:L (result), R0 (restored after
;   use), rmp
; Called subroutines: Hex2ToBin1, Hex1ToBin1
;
Hex4ToBin2:
	clt ; Clear error flag
	rcall Hex2ToBin1 ; convert two digits hex to Byte
	brts Hex4ToBin2a ; Error, go back
	mov rBin1H,rmp ; Byte to result MSB
	rcall Hex2ToBin1 ; next two chars
	brts Hex4ToBin2a ; Error, go back
	mov rBin1L,rmp ; Byte to result LSB
	sbiw ZL,4 ; result ok, go back to start
Hex4ToBin2a:
	ret
;
; Hex2ToBin1 converts 2-digit-hex-ASCII to 8-bit-binary
; Called By: Hex4ToBin2
;
Hex2ToBin1:
	push R0 ; Save register
	rcall Hex1ToBin1 ; Read next char
	brts Hex2ToBin1a ; Error
	swap rmp; To upper nibble
	mov R0,rmp ; interim storage
	rcall Hex1ToBin1 ; Read another char
	brts Hex2ToBin1a ; Error
	or rmp,R0 ; pack the two nibbles together
Hex2ToBin1a:
	pop R0 ; Restore R0
	ret ; and return
;
; Hex1ToBin1 reads one char and converts to binary
;
Hex1ToBin1:
	ld rmp,z+ ; read the char
	subi rmp,'0' ; ASCII to binary
	brcs Hex1ToBin1b ; Error in char
	cpi rmp,10 ; A..F
	brcs Hex1ToBin1c ; not A..F
	cpi rmp,$30 ; small letters?
	brcs Hex1ToBin1a ; No
	subi rmp,$20 ; small to capital letters
Hex1ToBin1a:
	subi rmp,7 ; A..F
	cpi rmp,10 ; A..F?
	brcs Hex1ToBin1b ; Error, is smaller than A
	cpi rmp,16 ; bigger than F?
	brcs Hex1ToBin1c ; No, digit ok
Hex1ToBin1b: ; Error
	sbiw ZL,1 ; one back
	set ; Set flag
Hex1ToBin1c:
	ret ; Return


; *******************************************
;
; Package V: 16 bit binary to right justified ASCII coded decimal
; added by Chris Schroeder 8-15-2010
; registers and buffers are pre-defined for convenience (this may need to change)

.def	rBin1L	=r16
.def	rBin1H	=r17
.def	decimal_places	=r18
.def	rBin2L	=r19
.def	rBin2H	=r20
.def	rmp		=r21

.def	num_flag	=r22
				
.equ	neg_num		=7	;b7 set for negative number


.def leading_zeros	=r23
.def	decimal_pos	=r24
.def	Ypointer		=r25

.equ	temp_buffer1	=$200
.equ	temp_buffer2	=$210
					;keep memory space free to $21F 


;
; Bin2SToAsc
; =========
; converts a 16-bit-signed binary to a 6 character right justified ASCII coded decimal
; In: 16-bit-binary in rBin1H:L, 
; Out: Z points to the first (left) character 
; Used registers: rBin1H:L (unchanged), rBin2H (changed),
;   rBin2L (result, length of number), rmp, num_flag, Z
;   all registers must be r16 or higher, also requires Z buffer space
; Called subroutines: Bin2ToBcd5, Bin2ToAsc5
; Approx. 30 usec at 16 Mhz 

Bin2SToAsc:
					;set Z buffer address
		ldi	r30,low(temp_buffer1)
		ldi	r31,high(temp_buffer1)

					;clear num_flag and test sign bit in r17
					;if positive jump ahead
	
		clr	num_flag
		sbrs	r17,7
		rjmp	Bin2SToAsc_1

					
					;negative number
					;set neg_num flag
		sbr	num_flag,1<<neg_num

					;two's complement r17:r16
		com	r16
		com	r17
		subi	r16,$FF
		sbci	r17,$FF

				
Bin2SToAsc_1:
					;increment Z 
					;(so position Z+0 is free)
		adiw	Z,1

					;convert to ASCII
		call	Bin2ToAsc
					;returns with Z at first (left) character
					;and number of characters in rBin2L		

					;test neg_num flag
		sbrs	num_flag,neg_num
		rjmp	Bin2SToAsc_2
		
					;negative number
					;increment number of characters to
					;account for "-" sign 
		inc rBin2L
					;realign Z to sign character position
					;and store "-" sign character
		sbiw	Z,1
		ldi	rmp,'-'
		st	Z,rmp
		
Bin2SToAsc_2:
					;insert space character in unused left positions
					;so result is 6 characters long
		ldi	rmp,' '
		
Bin2SToAsc_3:	cpi	rBin2L,6
		breq	Bin2SToAsc_4

		st	-Z,rmp
		inc	rBin2L

		rjmp	Bin2SToAsc_3		

Bin2SToAsc_4:		ret





;
; Bin2SToAscD
; =========
; converts a 16-bit-signed binary to a 7 character right justified ASCII coded decimal
; with decimal point and leading zeros
; In: 16-bit-binary in rBin1H:L, decimal_places (digits to right of decimal point)
;   Note maximum 4 decimal places 
; Out: Z points to the first (left) character 
; Used registers: rBin1H:L (unchanged), decimal_places, rBin2H:L, rmp, num_flag,
;   leading_zeros,decimal_pos,Ypointer, Y, Z 
;   all registers must be r16 or higher, also requires Y, Z buffer space	
; Called subroutines: Bin2ToBcd5, Bin2ToAsc5
; Approx. 35 usec at 16 Mhz


Bin2SToAscD:
				;set Y, Z buffer address
		ldi	r28,low(temp_buffer1)
		ldi	r29,high(temp_buffer1)
		ldi	r30,low(temp_buffer2)
		ldi	r31,high(temp_buffer2)

					;clear num_flag and test sign bit in r17
					;if positive jump ahead
	
		clr	num_flag
		sbrs	r17,7
		rjmp	Bin2SToAscD_1
					
					;negative number
					;set neg_num flag
		sbr	num_flag,1<<neg_num

					;two's complement r17:r16
		com	r16
		com	r17
		subi	r16,$FF
		sbci	r17,$FF

				
Bin2SToAscD_1:

					;convert to ASCII
		call	Bin2ToAsc
					;returns with Z at first (left) character
					;and number of characters in rBin2L
					
					;leading_zeros = decimal_places + 1 - rBin2L
					;zero or negative value means no leading zeros required	
		mov	leading_zeros,decimal_places
		subi	leading_zeros,-1
		sub	leading_zeros,rBin2L

					;decimal_pos = 6 - decimal_places
		ldi	decimal_pos,6
		sub	decimal_pos,decimal_places
		
					;realign Z to last (right) character + 1
					;Z = Z + (rBin2L -1) + 1
		clr	rBin2H
		add	ZL,rBin2L
		adc	ZH,rBin2H
	
					;realign Y to last (right) character + 1
		adiw	Y,7

					;initialize Ypointer = 6
		ldi	Ypointer,6
		

					;digit character copy loop
Bin2SToAscD_2:

					;copy character @ -Y = -Z
		ld	rmp,-Z
		st	-Y,rmp

					;decrement Ypointer and rBin2L
		dec	Ypointer
		dec	rBin2L

					;decimal point insertion
Bin2SToAscD_3:

					;if Ypointer = decimal_pos
					;store '.' decimal point character at -Y
					;and decrement Ypointer
					;else jump ahead
		cp	Ypointer,decimal_pos
		brne	Bin2SToAscD_4
		
		ldi	rmp,'.'
		st	-Y,rmp
		dec	Ypointer
		

					;digit character test
Bin2SToAscD_4:
					;if rBin2L > 0
					;digit characters remain, loop again 
		tst	rBin2L
		brne	Bin2SToAscD_2					


					;leading zero insertion
Bin2SToAscD_5:

					;if leading_zeros > 0
					;store '0' zero character at -Y
					;and decrement Ypointer	
		tst	leading_zeros
		breq	Bin2SToAscD_6
		brmi	Bin2SToAscD_6
		
		ldi	rmp,'0'
		st	-Y,rmp
		dec	Ypointer
		dec	leading_zeros

					;jump back to decimal point insertion
		rjmp	Bin2SToAscD_3


					;negative sign insertion					
Bin2SToAscD_6:

					;if neg_num flag set
					;store '-' negative sign character at -Y
					;and decrement Ypointer
		sbrs	num_flag,neg_num
		rjmp	Bin2SToAscD_7
		
		ldi	rmp,'-'
		st	-Y,rmp
		dec	Ypointer


					;space character insertion					
Bin2SToAscD_7:
					;if Ypointer >= 0
					;store ' ' blank character at -Y
					;and decrement Ypointer
					;loop until done with last blank
		tst	Ypointer
		brmi	Bin2SToAscD_8

		ldi	rmp,' '
		st	-Y,rmp
		dec	Ypointer

		rjmp	Bin2SToAscD_7

					;7 characters are now in Y buffer
Bin2SToAscD_8:
					;Y is already at start of buffer
					;set Z=Y so results are returned  
					;at Z buffer address 
		movw	Z,Y			

		ret


















