.include "m644def.inc"

.equ 	f=2457600		;frequency 2.4576MHz
.equ 	d1s=200			;(2457600/12288=200)Booting delay time
.equ 	d5ms=f/1966		;(2457600/1966=1250) 5 ms Wait after each control word
.equ 	d50us=f/49152	;(2457600/49152=50) 50 us Wait after each char
.equ	Reset =0		;PB0
.equ	drdy  =3		;PB3
.equ 	DD_SS =4    	;PB4
.equ 	DD_MOSI=5		;PB5
.equ 	DD_MISO=6		;PB6
.equ 	DD_SCK =7		;PB7

.def 	temp 	=r16	
.def 	temp2 	=r17
.def	temp3	=r18
.def	temp4	=r19
.def	dr3	=r20
.def 	data 	=r23
.def 	data2	=r24


.org 0x0000      ; The following code starts from this address

rjmp Intial

Intial:

;**************************************
;Stack Setup
;**************************************

ldi		temp, low(RAMEND)
out		SPL, temp

ldi		temp, high(RAMEND)
out		SPH, temp

ser		temp			
out 	DDRC,	temp	;PortC as outputs (connected to LEDS)

Main:

	ldi		temp,	0x00
	out		PortC,	temp

	cbi		PinB, Reset				;AD7705 reset routine
   	rcall 	lcd_delay50us

	sbi 	PinB, Reset
	rcall 	lcd_delay50us


	rcall 	SPI_MasterInit			;Initialise SPI
	rcall 	lcd_delay50us

	ldi		data, 0xFF				;Serially reset the chip
	rcall	SPI_Transfer
	rcall	lcd_delay50us

	ldi		data, 0xFF				;Serially reset the chip
	rcall	SPI_Transfer
	rcall	lcd_delay50us

	ldi		data, 0xFF				;Serially reset the chip
	rcall	SPI_Transfer
	rcall	lcd_delay50us

	ldi		data, 0xFF				;Serially reset the chip
	rcall	SPI_Transfer
	rcall	lcd_delay50us

	ldi		data, 0xFF				;Serially reset the chip
	rcall	SPI_Transfer
	rcall	lcd_delay50us

	ldi 	data,0x20				;Next op to be write to clock reg of AD7705
	rcall 	SPI_Transfer
	rcall 	lcd_delay50us

	ldi 	data,0x04
	rcall 	SPI_Transfer			;Writes byte to the clock reg of AD7705
	rcall 	lcd_delay50us
		
	ldi 	data,0x10
	rcall 	SPI_Transfer			;Next op will be written to setup reg of the AD7705
	rcall 	lcd_delay50us

	ldi 	data,0x40
	rcall 	SPI_Transfer			;Writes byte to the setup reg of the AD7705
	rcall 	lcd_delay50us

PollDRDY:							;Waiting for calibration routine to finish
	in		temp, PortB
	sbrc	temp, 3		
	rjmp	PollDRDY

	ldi 	data,0x39				;Tells com register of AD7705 it is going to be read from 
	rcall 	SPI_Transfer
	rcall 	lcd_delay50us

	ldi		data, 0x00
	rcall	SPI_Transfer
	rcall	lcd_delay50us
	push	data

	ldi		data,	0x00
	rcall	SPI_Transfer
	rcall	lcd_delay50us
	mov		data2,	data
	pop		data

	Out   	PortC, data2			;Shows low byte results on LEDs
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s

	

 	Out   	PortC, data		;Shows high byte results on LEDs
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s
	rcall 	lcd_delay1s


	


ldi temp3,			0b00000000	;Loads reference voltage into temp 3
ldi temp4,			0b11000000	;Loads reference voltage into temp 4

cp	data2,			temp3		;Compares Low byte of ADCL to reference byte
cpc	data,			temp4		;Compares High byte of ADCL to reference byte

brsh	Flash

rjmp	PollDRDY
	

Flash:
Ldi		temp,	0b00000000
out		PortC,	temp

Call	lcd_delay1s
Call	lcd_delay1s

Ldi		temp,	0b11111111
out		PortC,	temp

Call	lcd_delay1s
Call	lcd_delay1s

rjmp	Flash
	

SPI_MasterInit:


	; Set MOSI and SCK output, all others input
	ldi 	temp,(1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_SS)	;ldi temp,(1<<DDB4)|(1<<DDB5)|(1<<DDB7)		
	out 	DDRB,temp									;out DDR_SPI,temp
	sbi 	PORTB,DD_SS
	; Enable SPI, Master, set clock rate fck/128
	ldi 	temp,(1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0)|(1<<CPOL)|(1<<CPHA)
	out 	SPCR,temp
	clr 	temp
	out 	SPSR,temp
	ret


SPI_Transfer: 

   cbi    PORTB,DD_SS ;Lower /SS
	; Start transmission of data (r22) 
   out SPDR,data                  ;Whatever is loaded in data is sent to SPDR 
Wait_Transmit: 
   ; Wait for transmission complete   
   in temp, spsr 
   sbrs   temp, 7 
   rjmp Wait_Transmit 
   ; Read received data and return 
   in data,SPDR               ;Whatever is loaded in SPDR is sent to data 
   sbi    PORTB,DD_SS ;Raise /SS 

   ret   

;*********DELAY ROUTINES************
lcd_delay1s:
		ldi 	temp,d1s
lcd_delay1sa:
		rcall 	lcd_delay5ms	
		dec 	temp
		brne 	lcd_delay1sa	
		ret
		
lcd_delay5ms:
		push 	zh
		push 	zl
		ldi 	zh,high(d5ms)
		ldi 	zl,low(d5ms)
lcd_delay5msa:
		sbiw 	zl,1
		brne 	lcd_delay5msa
		pop 	zl
		pop 	zh
		ret

lcd_delay50us:
		ldi 	temp,d50us
lcd_delay50usa:
		nop
		dec 	temp
		brne 	lcd_delay50usa
		ret


