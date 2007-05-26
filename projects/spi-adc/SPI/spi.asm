.include "m162def.inc"

.dseg
.org $100		;MC-s like 128,64,16,161,169 etc... which are all havind external IO Memories.
.def temp= r16	;Normaly the Data Segment Starts at $60 for all other devices except the above mentioned ICs
.def data= r22
.equ DD_SS = 4
.equ DD_MOSI=5
.equ DD_MISO=6
.equ DD_SCK = 7

.cseg
.org $000
rjmp main

.include "usart162.asm"
.include "lcds.asm"

main:
	ldi temp,high(ramend)
	out sph,temp
	ldi temp,low(ramend)
	out spl,temp

	clr temp
	clr data

	rcall usart1_init_9600
	rcall lcd_init
	rcall lcd_clr

	ldi zl,low(rtd*2)
	ldi zh,high(rtd*2)
	rcall lpm_loader
	
repeat:
	rcall SPI_MasterInit
	rcall lcd_delay50us
	cbi PORTB,DD_SS

	ldi data,0b00000001
	rcall SPI_MasterTransmit
	rcall lcd_delay50us
	rcall SPI_MasterReceive

	ldi data,0b11000000
	rcall SPI_MasterTransmit
	rcall lcd_delay50us
	rcall SPI_MasterReceive
		
;	ldi data,0b00000000
;	rcall SPI_MasterTransmit
;	rcall lcd_delay50us
;	rcall SPI_MasterReceive

	sbi PORTB,DD_SS
	cbi PORTB,DD_SCK

	rcall usart1_transmit

	rcall lcd_delay1s
	rcall repeat

end: rjmp end



SPI_MasterInit:
	; Set MOSI and SCK output, all others input
	ldi temp,(1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_SS)	;ldi temp,(1<<DDB4)|(1<<DDB5)|(1<<DDB7)		
	out DDRB,temp									;out DDR_SPI,temp
	sbi PORTB,DD_SS
	; Enable SPI, Master, set clock rate fck/128
	ldi temp,(1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0)
	out SPCR,temp
	clr temp
	out SPSR,temp
	ret

;	ldi temp,$53
;	out spcr,temp
;	ldi temp,$00
;	out spsr,temp

;	sbi ddrb,mosi
;	sbi ddrb,ss
;	sbi ddrb,sck
;	cbi ddrb,miso
	
;	sbi portb,ss

SPI_MasterTransmit:
	; Start transmission of data (r22)
	out SPDR,data
	Wait_Transmit:
	; Wait for transmission complete
	sbis SPSR,SPIF
	rjmp Wait_Transmit
	ret

SPI_MasterReceive:
	; Wait for reception complete
	sbis SPSR,SPIF
	rjmp SPI_MasterReceive
	; Read received data and return
	in data,SPDR
	ret

SPI_SlaveInit:
	; Set MISO output, all others input
	ldi temp,(1<<DDB6)		;ldi r17,(1<<DD_MISO)
	out DDRB,temp			;out DDR_SPI,temp
	; Enable SPI
	ldi temp,(1<<SPE)
	out SPCR,temp
	ret

SPI_SlaveReceive:
	; Wait for reception complete
	sbis SPSR,SPIF
	rjmp SPI_SlaveReceive
	; Read received data and return
	in data,SPDR
	ret

rtd: .db "RTD CONNECTED",$00