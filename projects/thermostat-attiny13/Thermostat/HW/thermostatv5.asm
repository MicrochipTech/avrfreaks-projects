; ////////////////////////////////////////////////////////////////////////////////////////////////////////////
; This code is dedicated for simple programmable control unit for the heating pump.
; The on/off temperatures can be set via two buttons. 
; Mode button - in case of pressing, selection of: CT->on->XX->oF->YY->CT, where XX is temperature of the output 
; switching on/off and YY is temperature of switching off/on, CT is current temperature.
; Value button sets the value for XX and YY temperature.
; 
; Author Peter Falat, www.falcom.sk
; ////////////////////////////////////////////////////////////////////////////////////////////////////////////
; MCU: ATtiny13
; Fuses: 
; 

.DEVICE ATtiny13
.INCLUDE "tn13def.inc"
.ORG 0
	rjmp	RESET			; Reset Handler
.ORG 1
;	rjmp	EXT_INT0		; IRQ0 Handler
.ORG 2
;	rjmp	PCINTO			; PCIINT0 Handler
.ORG 3
	rjmp	TIM0_OVF		; Timer0 Overflow Handler
.ORG 4
;	rjmp	EE_RDY			; EEPROM Ready Handler
.ORG 5
;	rjmp	ANA_COMP		; Ananlog Comparator Handler
.ORG 6	
;	rjmp	TIM0_COMPA		; Timer0 CopmpareA Handler
.ORG 7
;	rjmp	TIM0_COMPB		; Timer0 CompareB Handler
.ORG 8
;	rjmp	WATCHDOG		; Watchdog Interrupt Hnadler
.ORG 9
;	rjmp	ADCCOMPL		; ADC Conversion Complete


.DSEG						; Data segment

.ESEG						; Eeprom segment
	TOn:	.DB	70			; Temperature values for switching on and off
	TOff:	.DB	60
.CSEG						; Code segment
	numbers:.DB	3, 159, 37, 13, 153, 73, 65, 31, 1, 9, 253, 197, 213, 113	; Data for 7-segments display


.DEF 	rm1 = R0 	; Binary number to be multiplicated (8 Bit)
.DEF 	rmh = R1 	; Interim storage
.DEF 	rm2 = R2 	; Binary number to be multiplicated with (8 Bit)
.DEF 	rel = R4 	; Result, LSB (16 Bit)
.DEF 	reh = R5 	; Result, MSB

.DEF 	rd1l = R0 	; LSB 16-bit-number to be divided
.DEF 	rd1h = R1 	; MSB 16-bit-number to be divided
.DEF 	rd1u = R2 	; interim register
.DEF 	rd2 = R3 	; 8-bit-number to divide with
.DEF 	rmp = R17	; multipurpose register for loading

.DEF	TempC = R6
.DEF	TempOn = R7
.DEF	TempOff = R8
.DEF	Flag = R10	; bit array, b0=0=>CA1, b0=1=>CA2, b1=SW1, b2=SW2, b3=RELAY
.DEF	var8 = R11
.DEF	var7 = R12
.DEF	var4 = R13
.DEF	var6 = R15

.DEF	temp = R16	; Multi purpose register for loading
.DEF	i = R17
.DEF	j = R18
.DEF	k = R19
.DEF	status = R20
.DEF	var5 = R21
.DEF	digit10 = R22
.DEF	digit1 = R23
.DEF	timer = R24
.DEF	timer_adc = R25
.DEF	adr = R26
.DEF	tmp1 = R9



.EQU	CA1=PB0				; out DDB0=1, high
.EQU	CA2=PB1				; out DDB1=1, high
.EQU	CLK=PB2				; out DDB2=1, low
.EQU	AIN=PB3				; out DDB3=1, low
.EQU	KTY=PB4				; in DDB4=0, no pull-up
.EQU	RELAY=PB5			; out DDB5=1, high
.EQU	SW1=1
.EQU	SW2=2
.EQU	SWREL=3

.MACRO delay				; Time delay macro
	clr timer
Jmp10:
	cpi timer, 100
	brlt Jmp10
.ENDM
	
RESET:
    ldi r16, RAMEND			; Stackpointer initialisation
    out SPL, r16
	ldi i, (1<<WDE) | (1<<WDP2) | (1<<WDP1);
	out WDTCR, i                     
	ldi i, 0b00000000		; TCCR0A setting
	out TCCR0A, i
	ldi i, (1<<CS01) | (1<<CS00)	; TCCR0B setting, CLK/64
	out TCCR0B, i
	ldi i, 0b00000010		; TIMSK0 setting, overflow timer interrupt enabled
	out TIMSK0, i
	ldi i, 0b00000010	
	out ADMUX, i			; Set PortB4 as analog input, Uref=Us
	ldi i, 0b00000000
	out ADCSRB, i			; ADC enabled and started in free running mode
	ldi i, (0<<PB5)|(0<<PB4)|(0<<PB3)|(0<<PB2)|(0<<PB1)|(0<<PB0)
	out PORTB, i
	ldi i, (1<<DDB5)|(0<<DDB4)|(1<<DDB3)|(1<<DDB2)|(1<<DDB1)|(1<<DDB0)
	out DDRB, i

	ldi adr, 0				; Reading of temperature values, TempOn@0x00, TempOff@0x01
	rcall EEPROM_read
	mov TempOn, i
	rcall Display
	delay
	inc adr
	rcall EEPROM_read
	mov TempOff, i
	rcall Display
	delay


DefaultTemp:
	ldi temp, 255
	cp TempOn, temp
	breq TempOnDef
	cp TempOff, temp
	breq TempOffDef
	clr status
	clt
	bld Flag, SWREL
	sbi PORTB, RELAY		; Relay is switched on by default
	sei						; Global Interupt Enable
	rjmp Start
	
TempOnDef:
	ldi temp, 70
	mov TempOn, temp			; if not set, max default TempOn
	mov i, TempOn
	ldi adr,0
	rcall EEPROM_write
	rjmp DefaultTemp
	
TempOffDef:
	ldi temp, 60
	mov TempOff, temp			; if not set, min default TempOff
	mov i, TempOff
	ldi adr,1
	rcall EEPROM_write
	rjmp DefaultTemp
	
Start:
	sbrc Flag, SW1
	rjmp Switch1
	rcall Calculation
	cp TempC, TempOn
	brsh RelOn
	cp TempC, TempOff
	brlo RelOff
Cont:
	clr timer
Jmp0:
	cpi timer, 10
	brlt Jmp0
	rjmp Start

RelOn:
	set
	bld Flag, SWREL
	cbi PORTB, RELAY
	rjmp Cont
RelOff:
	clt
	bld Flag, SWREL
	sbi PORTB, RELAY
	rjmp Cont

; =======================================================================
Switch1:
	inc status
 	cpi status, 1
	breq Case1
	cpi status, 2
	breq Case2
	cpi status, 3
	breq Case3
	cpi status, 4
	breq Case4
	rjmp Default

Case1:
	ldi digit10, 11
	ldi digit1, 12
	delay
	rjmp Break1

Case2:
	ldi adr, 0
	rcall EEPROM_read		; i contains data
	mov TempOn, i
	rcall Display
	delay
	rjmp Break1

Case3:
	ldi digit10, 11
	ldi digit1, 13
	delay
	rjmp Break1

Case4:
	ldi adr, 1
	rcall EEPROM_read		; i contains data
	mov TempOff, i
	rcall Display
	delay
	rjmp Break1

Default:
	clr status
	
Break1:
	sbrc Flag, SW1
	rjmp Break1
	tst status
	breq Start
	cpi status, 2
	breq Status2
	cpi status, 3
	breq Wait4Button
	cpi status, 4
	breq Status4

Wait4Button:
	sbrs Flag, SW1
	rjmp Wait4Button
	rjmp Switch1

; =======================================================================
Status2:
Jmp20:					
	sbrs Flag, SW2
	rjmp Jmp_SW1
	rjmp Jmp25
Jmp_SW1:
	sbrs Flag, SW1
	rjmp Jmp20
	rjmp Jmp_Save
Jmp25:
	inc	TempOn
	ldi i, 100				; if (TempOn>99) TempOn=0;
	cp TempOn, i
	brlo Jmp24
	clr TempOn
Jmp24:
	mov i, TempOn
	rcall Display
Jmp22:
	sbrc Flag, SW2			; while (SW2==1); - wait for SW2 release
	rjmp Jmp22
	rjmp Jmp20
Jmp_Save:
	mov i, TempOn
	ldi adr,0
	rcall EEPROM_write
;	delay
	rjmp Switch1

; =======================================================================
Status4:
Jmp40:					
	sbrs Flag, SW2
	rjmp Jmp_SW41
	rjmp Jmp45
Jmp_SW41:
	sbrs Flag, SW1
	rjmp Jmp40
	rjmp Jmp_Save4
Jmp45:
	inc	TempOff
	ldi i, 100				; if (TempOff>99) TempOff=0;
	cp TempOff, i
	brlo Jmp44
	clr TempOff
Jmp44:
	mov i, TempOff
	rcall Display
Jmp42:
	sbrc Flag, SW2			; while (SW2==1); - wait for SW2 release
	rjmp Jmp42
	rjmp Jmp40
Jmp_Save4:
	mov i, TempOff
	ldi adr,1
	rcall EEPROM_write
;	delay
	rjmp Switch1

; =======================================================================
Calculation:		; Aproximation formula for KTY81-210: T=(86/100*ADC)-122
	mov rm1,var7	; ADC measurement => T

//  Multiplication by 86

	ldi rmp,86 		
	mov rm2,rmp

Mult8:
					; Clear start values
	clr rmh 		; clear interim storage
	clr rel 		; clear result registers
	clr reh
					; Here we start with the multiplication loop
Mult8a:
					; 	Step 1: Rotate lowest bit of binary number 2 to the carry flag 
					; (divide by 2, rotate a zero into bit 7)
	clc 			; clear carry bit
	ror rm2 		; bit 0 to carry, bit 1 to 7 one position to the right, carry bit to bit 7
					; 	Step 2: Branch depending if a 0 or 1 has been rotated to the carry bit
	brcc Mult8b 	; jump over adding, if carry has a 0
					; 	Step 3: Add 16 bits in rmh:rml to the result, with overflow from LSB to MSB
	add rel,rm1 	; add LSB of rm1 to the result
	adc reh,rmh 	; add carry and MSB of rm1

Mult8b:
					; 	Step 4: Multiply rmh:rm1 by 2 (16 bits, shift left)
	clc 			; clear carry bit
	rol rm1 		; rotate LSB left (multiply by 2)
	rol rmh 		; rotate carry into MSB and MSB one left
					; 	Step 5: Check if there are still one's in binary 2, if yes, go on multiplicating
	tst rm2 		; all bits zero?
	brne MULT8a 	; if not, go on in the loop
					; End of the multiplication, result in reh:rel

//  Division ADC/100
					
	mov rd1h,reh
	mov rd1l,rel
	ldi rmp, 100 	; 100 to be divided with
	mov rd2, rmp
					; Divide rd1h:rd1l by rd2
div8:
	clr rd1u 		; clear interim register
	clr reh 		; clear result (the result registers
	clr rel 		; are also used to count to 16 for the
	inc rel 		; division steps, is set to 1 at start)
					; Here the division loop starts
div8a:
	clc 			; clear carry-bit
	rol rd1l 		; rotate the next-upper bit of the number
	rol rd1h 		; to the interim register (multiply by 2)
	rol rd1u
	brcs div8b 		; a one has rolled left, so subtract
	cp rd1u,rd2 	; Division result 1 or 0?
	brcs div8c 		; jump over subtraction, if smaller
div8b:
	sub rd1u,rd2	; subtract number to divide with
	sec 			; set carry-bit, result is a 1
	rjmp div8d 		; jump to shift of the result bit
div8c:
	clc 			; clear carry-bit, resulting bit is a 0
div8d:
	rol rel 		; rotate carry-bit into result registers
	rol reh
	brcc div8a 		; as long as zero rotate out of the result registers: go on with the division loop
					; End of the division reached

	mov i, rel
	subi i, 122				; -122
	mov TempC, i 		 

Display:
	clr k
	ldi	j, 10

Loop3:
	inc k
	sub i, j				; i contains 1s
	brsh Loop3
	dec k					; k contains 10s
	add i, j
	mov digit10, k
	mov digit1, i
	ret

; =======================================================================
TIM0_OVF:
	push temp
	in temp, SREG
	wdr
	ldi var5, 163	
	out TCNT0, var5			; Reinitialize Timer0 value
	sbis PINB, CA2			; if (PINB & CA2)=1
	rjmp Jmp3
	sbis PINB, KTY			; if (PINB & KTY)=1
	rjmp Jmp4			
	set						; SW1=1
	bld Flag, SW1
	rjmp Send

Jmp4:
	clt						; SW1=0
	bld Flag, SW1
	rjmp Send

Jmp3:						; if (PINB & CA2)=0
	sbis PINB, KTY			; if (PINB & KTY)=1
	rjmp Jmp5
	set						; SW2=1
	bld Flag, SW2
	rjmp Send

Jmp5:
	clt						; SW2=0
	bld Flag, SW2

Send:
	cbi PORTB, CA1			; Switch off segment 10
	cbi PORTB, CA2			; Switch off segment 1
	tst status				; 0 = nothing pressed
	brne No_ADC
	cpi timer_adc, 50
	brne No_ADC
	rcall Read_ADC
No_ADC:
	inc timer
	inc timer_adc
	mov var5, digit10	 
	sbrc Flag, CA1
	mov var5, digit1
	ldi ZH, high (numbers<<1)
	ldi ZL, low (numbers<<1)
	add ZL, var5
	lpm var6, Z
	sbrc Flag, CA1
	sbrs Flag, SWREL
	rjmp Jmp7
	dec var6
Jmp7:
	ldi var5, 8

Loop4:
	cbi	PORTB, CLK
	cbi PORTB, AIN
	sbrc var6, 0			
	sbi PORTB, AIN	
	lsr var6
	sbi PORTB, CLK
	dec var5
	brne Loop4
	sbrs Flag, CA1			
	rjmp Jmp6
	sbi PORTB, CA2
	ldi var5, 0b00000001
	eor Flag, var5
	out SREG, temp
	pop temp
	reti

Jmp6:
	sbi PORTB, CA1
	set 
	bld Flag, CA1
	out SREG, temp
	pop temp
	reti
	
; =======================================================================
Read_ADC:
	ldi var5, 0b11000011
	out ADCSRA, var5		; ADC enabled and started
ADC_Ready:	
	sbis ADCSRA, ADIF
	rjmp ADC_Ready
	sbi ADCSRA, ADIF
	in var7, ADCL			; read AD converter MSB
	in var8, ADCH	 
	clr timer_adc
	ret						; return from subroutine
	
; =======================================================================
EEPROM_write:
	cli
	sbic EECR,EEPE			; Wait for completion of previous write
	rjmp EEPROM_write
	clr tmp1				; (0<<EEPM1)|(0<<EEPM0)	; Set Programming mode
	out EECR, tmp1
	out EEARL, adr			; Set up address (r17) in address register
	out EEDR,i				; Write data (r16) to data register
	sbi EECR,EEMPE			; Write logical one to EEMPE
	sbi EECR,EEPE			; Start eeprom write by setting EEPE
	sbic EECR,EEPE			; Wait for completion of previous write
	sei
	ret
	
; =======================================================================
EEPROM_read:
	cli
	sbic EECR,EEPE			; Wait for completion of previous write
	rjmp EEPROM_read
	out EEARL, adr			; Set up address in address register
	sbi EECR,EERE			; Start eeprom read by writing EERE
	in i,EEDR				; Read data from data register
	sei
	ret

; ============ E N D =====================================================
