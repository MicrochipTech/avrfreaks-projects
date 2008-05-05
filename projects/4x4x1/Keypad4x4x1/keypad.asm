; One-wire 4x4 keypad routines

.equ NO_PUSH = 0xE0		; What the keypad indicates as 'nothing pressed'
						; Mine only said FF or FE, but we want lots of 
						; extra space here.  I used $E0 for that.

KEYP:		; Fetches value from ADC
			; Returns 8-bit value in register r22
			; Trashes Z ptr, temp

	rcall KEYP_GET			; Fetch ADC value
	cpi temp, (NO_PUSH)		; Test if greater than NO_PUSH
	brlo KEYPa				; If so, return (no key pressed)
ret

KEYPa:					; Else fetch value
	rcall KEYP_AVG		; And throw it away (debouncing)
	rcall KEYP_AVG		; Fetch it again

	cpi r22, (NO_PUSH)		; Test if greater than NO_PUSH
	brsh KEYPc					; If so, return (no key pressed, noise spike)
								; This has to be done here again to prevent weird
								; mistriggering on a key release

cli								; Note no interrupts allowed
	ldi ZL, low(2*KEY_LOOKUP)	; Translate from ADC value into
	ldi ZH, high(2*KEY_LOOKUP)	; Keypad value.
	add ZL, r22				; We just add the result from KEYP_AVG to the address
	adc ZH, temp2				; and fetch whatever is at that address.
	lpm r22, Z
sei

	rcall KEYP_SER		; Put the translated character out RS-232


KEYPb:
	rcall KEYP_GET		; Fetch ADC value
	cpi temp, (NO_PUSH)		; Test if greater than NO_PUSH
	brlo KEYPb			; If not, loop (some key is still pressed)

			; This would be a place for a clever programmer
			; to insert a 'key repeat delay', but at the moment
			; we want to make sure that one push only gets us
			; one character put.

KEYPc:					; Key's released, we're done.
ret

; Subroutines

KEYP_GET:				; Fires the ADC, returns 8-bits in 'temp'
	clr ZH
	ldi ZL, low(ADCSRA)
	ld temp, Z
	sbr temp, $40		; Fire conversion
	st Z, temp

KEYP_GETa:				; Loop until conversion done
	ld temp, Z
	andi temp, $40		; Mask off all except 'conversion in progress'
	brne KEYP_GETa

	ldi ZL, low(ADCH)
	ld temp, Z
ret


KEYP_AVG:			; Average 256x
					; Gets ADC value 256x
					; Adds temp to r22:L 256x
					; Returns avg. in r22, drop r21 (aka Divide by 256!)
					; Uses, trashes LoopC0, r22:L, temp, temp2

	clr LoopC0
	clr r22
	clr r21
	clr temp2

KEYP_AVGa:
	rcall KEYP_GET

;	mov r22, temp		; Uncomment for no averaging
;	rjmp KEYP_AVGb			; Not really recommended.

cli
	add r21, temp		; Note no interrupts during 16-bit math
	adc r22, temp2
sei

	dec LoopC0
	brne KEYP_AVGa

KEYP_AVGb:
ret

KEYP_SER:
	mov Txbyte, r22
	call PUT_CHAR

	ldi Txbyte, $20
	call PUT_CHAR
ret


KEYP_RAW:
	mov Txbyte, r22
	call PUT_HEX

	ldi Txbyte, $20
	call PUT_CHAR
ret

KEY_LOOKUP:	; This is my table.  Have fun building your own!
				; They'll be different with different resistor values
				; And no analog components are ever perfectly matched.

;    0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F	; Byte address line
.db ' ',' ','B','B','B','B','B','B',' ',' ',' ',' ',' ',' ',' ',' ' ;  00-15 , 00-0F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;  16-31 , 10-1F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;  32-47 , 20-2F
.db ' ',' ',' ',' ',' ',' ','5','5','5','5','5','5',' ',' ',' ',' ' ;  48-63 , 30-3F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;  64-79 , 40-4F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ','4','4','4','4','4','4',' ' ;  80-95 , 50-5F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;  96-111, 60-6F
.db ' ',' ','6','6','6','6','6',' ',' ',' ',' ',' ',' ',' ',' ',' ' ; 112-127, 70-7F
.db ' ',' ','C','C','C','C','C',' ',' ',' ',' ',' ',' ',' ',' ',' ' ; 128-143, 80-8F
.db ' ','8','8','8','8','8','8',' ',' ',' ',' ',' ','7','7','7','7' ; 144-159, 90-9F
.db '7',' ',' ',' ',' ','9','9','9','9','9',' ',' ','D','D','D','D' ; 160-175, A0-AF
.db 'D',' ',' ','0','0','0','0','0','*','*','*','*','*','#','#','#' ; 176-191, B0-BF
.db '#','#','A','A','A','2','2','2','1','1','1','3','3','3','3',' ' ; 192-207, C0-CF
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;        , D0-DF
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;        , NO_PUSH-EF
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;    -255, F0-FF


KEY_LOOKUP_BLANK:	;  This is a blank lookup table.


;    0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F	; Byte address line
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;  00-15 , 00-0F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;  16-31 , 10-1F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;  32-47 , 20-2F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;  48-63 , 30-3F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;  64-79 , 40-4F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;  80-95 , 50-5F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;  96-111, 60-6F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ; 112-127, 70-7F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ; 128-143, 80-8F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ; 144-159, 90-9F
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ; 160-175, A0-AF
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ; 176-191, B0-BF
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ; 192-207, C0-CF
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;        , D0-DF
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;        , NO_PUSH-EF
.db ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' ;    -255, F0-FF
