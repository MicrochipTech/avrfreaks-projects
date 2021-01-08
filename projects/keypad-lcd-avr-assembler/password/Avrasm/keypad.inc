;this one is for grayhill 96AB2-102-F 3x4 keypad
;
;***********set up port for keypad initialization
;
Init_Keypad:
	ldi	temp,0b01110000 	;outputs (columns),inputs (rows)
	out	KeyDDR,temp
	ldi	temp,0b11111111	;enable internal pull-ups
	out	KeyPort,temp
	ret
;-----------------------------------------------
;for grayhill 96AB2-102-F 3x4 keypad
.equ	row1 = 0
.equ	row2 = 1
.equ	row3 = 2
.equ	row4 = 3
;
.equ	col1 = 4
.equ	col2 = 5
.equ	col3 = 6
;
.equ	KeyPort = PORTD
.equ	KeyDDR  = DDRD
.equ	KeyPIN  = PIND
;------------------------------------------------
;Get key from Keypad, exit with key in "key" register
;
Get_Key:	
	cbi	KeyPort,col1	; Enable column1
	sbi	KeyPort,col2	; Disable the second column
	sbi	KeyPort,col3	; Disable the third column
key1:
	sbic	KeyPIN,Row4		; Check row 4
	rjmp 	key2			; if Not, check next key
	ldi	key,'*'		; if yes...
	rcall	debounce
key1a:
	sbis 	KeyPIN,Row4		; wait for pin to be released
	rjmp	key1a			;
	ret				;

key2:	
	sbic 	KeyPIN,Row3		; Check row 3
	rjmp 	key3			; if Not, check next key
	ldi	key, 0x37		; if yes...
	rcall	debounce	
key2a:
	sbis 	KeyPIN,Row3		; wait for pin to be released 
	rjmp	key2a			;
	ret				; 

key3:	
	sbic 	KeyPIN,Row2		; Check row 2
	rjmp 	key4			; if Not, check next key
	ldi	key, 0x34		; if yes...	
	rcall	debounce
key3a:
	sbis 	KeyPIN,Row2		; wait for pin to be released 
	rjmp	key3a			;
	ret				;

key4:	
	sbic 	KeyPIN,Row1		; Check row 1
	rjmp 	key5			; if Not, check next key
	ldi	key, 0x31		; if yes...	
	rcall	debounce
key4a:
	sbis 	KeyPIN,Row1		; wait for pin to be released
	rjmp	key4a			;
	ret				; 

key5:
	sbi	KeyPort,col1	; Disable the first column
	rcall	Delay40us		;
	cbi	KeyPort,col2	; and enable the second

	sbic 	KeyPIN,Row4		; Check row 4
	rjmp 	key6			; if Not... 
	ldi	key, 0x30		;
	rcall	debounce
key5a:
	sbis 	KeyPIN,Row4		; wait for pin to be released
	rjmp	key5a			;
	ret	

key6:	
	sbic 	KeyPIN,Row3		; Check row 3
	rjmp 	key7			; if Not... 
	ldi	key, 0x38		;
	rcall	debounce
key6a:
	sbis 	KeyPIN,Row3		; wait for pin to be released
	rjmp	key6a			;
	ret				;

key7:	
	sbic 	KeyPIN,Row2		; Check row 2
	rjmp 	key8			; if Not... 
	ldi	key, 0x35		;
	rcall	debounce
key7a:
	sbis 	KeyPIN,Row2		; wait for pin to be released
	rjmp	key7a			;
	ret				;

key8:	
	sbic 	KeyPIN,Row1		; Check row 1
	rjmp 	key9			; if Not... 
	ldi	key, 0x32		;	
	rcall	debounce
key8a:
	sbis 	KeyPIN,Row1		; wait for pin to be released
	rjmp	key8a			;
	ret	

key9:	
	sbi	KeyPort,col2	; Disable the second column
	cbi	KeyPort,col3	; and enable the third

	sbic 	KeyPIN,Row4		; Check row 4
	rjmp 	key10			; if Not... 
	ldi	key,'#'		; 
	rcall	debounce
key9a:
	sbis 	KeyPIN,Row4		; wait for pin to be released 
	rjmp	key9a			;
	ret

key10:	
	sbic 	KeyPIN,Row3		; Check row 3
	rjmp 	key11			; if Not... 
	ldi	key, 0x39		; 
	rcall	debounce
key10a:
	sbis 	KeyPIN,Row3		; wait for pin to be released 
	rjmp	key10a		;
	ret

key11:	
	sbic 	KeyPIN,Row2		; Check row 2
	rjmp 	key12			; if Not... 
	ldi	key, 0x36		; 
	rcall	debounce
key11a:
	sbis 	KeyPIN,Row2		; wait for pin to be released 
	rjmp	key11a		;
	ret

key12:	
	sbic 	KeyPIN,Row1		; Check row 2
	rjmp 	Get_Key		; loop until a key is pressed
	ldi	key, 0x33		; 
	rcall	debounce
key12a:
	sbis 	KeyPIN,Row1		; wait for pin to be released 
	rjmp	key12a		;
	ret
;----------------------------------------------------