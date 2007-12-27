;----------------------------------------
;
; Driver is written for smallest AVR´s (Tiny11,12,15) with
; Hardware-Stack only (3 Levels deep) and limited Instruction-Set
;
; Requires additional Hardware (see attached Circuit-Diagram)
;
; Change the Constant "xtal" in the Main-Program according to your clock-speed. 
;
; Function : Display content of Register "Data" as Decimal Number (0-255)
;            on 3x7-Segment Display.
;
; Stack used : 1
;
; Words used : 74 (incl.Lookup-Table)
;-----------------------------------------
;
;input : Register "Data" = 8 bit (value 0..255)
;output: Byte_High,Byte_Med,Byte_Low = Decimal Numbers
;        in low nibble of registers
;used  : Temp,TempA,TempB,ZH,ZL,r0,Byte_Out
;
; First convert to decimal
hex2dec:
	mov	TempB,Data
	ldi	TempA, -1 + '0'
_bcd1:
	inc	TempA
	subi	TempB, 100
	brcc	_bcd1
	ldi	Temp, 10 + '0'
_bcd2:
	dec	Temp
	subi	TempB, -10
	brcs	_bcd2
	subi	TempB, -'0'
;now save only low nibbles of converted values (0 to 9)
	andi	TempA,0x0F		
	mov		Byte_High,TempA
	andi	Temp,0x0F
	mov		Byte_Med,Temp
	andi	TempB,0x0F
	mov		Byte_Low,TempB
Out_7Segment:
; Do a table lookup on the low nibble of Byte_Low
	ldi		ZH, high(SegTable*2)
	ldi		ZL, low(SegTable*2)
	add		ZL,Byte_Low
	tst		ZL
	brne	No_Inc1
	inc		ZH
No_Inc1:
	lpm						;load Segment-Info
	mov Byte_Out,r0
	rcall 	OutByte			;and shift out
; Do a table lookup on the low nibble of Byte_Med
	ldi		ZH, high(SegTable*2)
	ldi		ZL, low(SegTable*2)
	add		ZL,Byte_Med
	tst		ZL
	brne	No_Inc2
	inc		ZH
No_Inc2:
	lpm						;load Segment Info
	ldi		TempA,DP		;should we display Dezimal-Point? 
	tst		TempA
	breq	No_DP1			;not at all
	cpi		TempA,2			;display DP on middle 7-Segment?
	brne	No_DP1			;no
	ldi		TempA,DP_Bit	;yes,clear DP-Bit to show DP
	and		r0,TempA
No_DP1:
	mov		Byte_Out,r0
	rcall 	OutByte			;and shift out
; Do a table lookup on the low nibble of Byte_High
	ldi		ZH, high(SegTable*2)
	ldi		ZL, low(SegTable*2)
	add		ZL,Byte_High
	tst		ZL
	brne	No_Inc3
	inc		ZH
No_Inc3:
	lpm						;load Segment-Info
	ldi		TempA,DP		;should we display Dezimal-Point? 
	tst		TempA
	breq	No_DP2			;not at all
	cpi		TempA,1			;display DP on left 7-Segment?
	brne	No_DP2			;no
	ldi		TempA,DP_Bit	;yes,clear DP-Bit to show DP
	and		r0,TempA
No_DP2:
	mov Byte_Out,r0

;------- Shifts Register "Out_Byte" into Hardware --------
; input : Byte to be sent in Byte_Out
; used  : TempA,TempB,Byte_Out
; Stack used : 0
;---------------------------------------------------------
OutByte:
	ldi	TempA,8				; load Bit Counter
	sbi	SegPORT,SegPIN		; set Output High
Next_Bit:
	lsl	Byte_Out			; Shift next bit into carry
	brcs	Write_One
Write_Zero:
	cbi	SegPORT,SegPIN		; if a zero, pin is low for delay-time
Write_One:					; if a one, pin stays high for delay-time
	ldi TempB,Del_Val		; delay-time
Delay_E1:
  	dec	TempB
    brne 	Delay_E1
	cbi	SegPORT,SegPIN		; edge falls here
	sbi	SegPORT,SegPIN		; rising edge clocks data into 74HCT164
	dec	TempA				; Decrement BitCount
	brne Next_Bit			; all Bits done ?
	ret						; yes					

;------------------------------------------------------------------
; A "0" bit equals TTL low, grounding the corresponding segment
; and turning it on.
;
; Register "Byte_Out" -> 7-Segment Matrix:
;
; Bit 0 Segment E,Bit 1 Segment F,Bit 2 Segment G,Bit 3 Segment DP,
; Bit 4 Segment A,Bit 5 Segment B,Bit 6 Segment C,Bit 7 Segment D
;------------------------------------------------------------------
SegTable:
;		0    1    2    3    4    5    6    7    8    9      
	.db	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x83,0xF8,0x80,0x90
;       
;Special_Characters:
;		  A    B    C    D    E    F    DP   H    L    G    Y    Minus
;	.db 0x88,0x83,0xC6,0x91,0x96,0x8E,0x80,0x89,0xC7,0x82,0x91,0xBF
