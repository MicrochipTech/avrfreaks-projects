;GENERIC FINITE STATE MACHINE
.include"m8def.inc"
.def ctr=r16
.def ctr1=r17
.def ctr2=r18
.def ctr3=r19
.def temp=r20
.def adl=r21
.def adh=r22
.def data=r23
.def stor=r24
.def ps=r25

.cseg
.org $00
rjmp reset

.org ovf0addr
rjmp tim0_ovf

.org $0B
rjmp rxpoll

reset:	
	ldi temp,low(RAMEND)
	out spl,temp
	ldi temp,high(RAMEND)
	out sph,temp
	ldi temp,$0f
	out ddrb,temp
	ldi temp,$ff
	out portb,temp
	ldi ctr1,0

intialize_usart :
	ldi temp,51
	out UBRRL,temp
	ldi temp,0
	out UBRRH,temp
	ldi temp,0b10001110
	out UCSRC,temp
		
initialize_timer :
	ldi temp,5
	out TCCR0,temp
	ldi temp,1
	out TIMSK,temp
	ldi temp,255
	out TCNT0,temp



ldi temp,5
out portb,temp
rjmp del

main:
	rcall usart;to store ps
	cpi stor,'q'
	breq display
	andi stor,$0f
	mov data,stor
	ldi adl,$ff
	ldi adh,1
    rcall eeprom_write
	rcall eeprom_read
	com data
	out portb,data
	com data

	clr adh
	clr adl

start :
	rcall usart
	cpi stor,'q'
	breq display

do: andi stor ,$0f
    mov data,stor
    rcall eeprom_write
	rcall eeprom_read

	com data
	out portb,data
	com data
	inc adl
	cpi adl,0
	breq display
	rjmp start
	
display :
	ldi adl,$ff
	ldi adh,1
	rcall delay
	rcall eeprom_read
	mov ps,data
	clr adh
	clr adl
	sei
loop :
    rjmp loop

tim0_ovf :
	cli
	ldi temp,255
	out TCNT0,temp
	com ps
	out portb,ps
	rcall input ;value of input is in ctr
	ldi temp,16
	com ps	 
	mul temp,ps
	mov ps,r0
	add ps,ctr
	mov adl,ps
	rcall eeprom_read
	mov ps,data
	reti

eeprom_write :
	sbic eecr,eewe
	rjmp eeprom_write
	out eearl,adl
	out eearh,adh
	out eedr,data
	sbi eecr,eemwe
	sbi eecr,eewe
	ret

eeprom_read :
	sbic eecr,eewe
	rjmp eeprom_read
	out eearl,adl
	out eearh,adh
	sbi eecr,eere
	in data,eedr
	ret

usart :	
	ldi temp,0b00010000
	out UCSRB,temp
rxpoll :	
	sbis UCSRA,RXC
	rjmp rxpoll
	in stor,UDR
	ret

delay:	
		ldi r26,50
one: 	ldi r27,40
two:	ldi r28,250
three:	dec r28
		cpi r28,00
		brne three
		dec r27
		cpi r27,00
		brne two
		dec r26
		cpi r26,00
		brne one
		ret

delay_20:
		ldi r27,40
four:	ldi r28,250
five:	dec r28
		cpi r28,00
		brne five
		dec r27
		cpi r27,00
		brne four
		ret

input:
	ldi ctr,$ff
	ldi r26,100
x:	ldi r27,80
y:	ldi r28,250
z:	dec r28

	in temp,pinb
	ori temp,$0f
	and ctr,temp	

	cpi r28,00
	brne z

	dec r27
	cpi r27,00
	brne y
	dec r26
	cpi r26,00
	brne x

	swap ctr
	com ctr
	ret

del:	
		ldi r26,100
o:	 	ldi r27,40
p:		ldi r28,250
q:		dec r28
	
	ldi temp,0b00010000
	out UCSRB,temp	
	sbis UCSRA,RXC
	rjmp r
	rjmp main
		
r:		cpi r28,00
		brne q
		dec r27
		cpi r27,00
		brne p
		dec r26
		cpi r26,00
		brne o
		rjmp display

