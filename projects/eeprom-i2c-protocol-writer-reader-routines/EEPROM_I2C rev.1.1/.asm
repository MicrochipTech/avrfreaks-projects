;-----------------------------------------------------------------
;
; Name:		EEPROM_I2C.asm
; Title:	AVR EEPROM I2C Writer Reader
; Version:	1.1
; Last updated:	11-04-2006
; Target:	ATmega48
; Clock:	8MHz
; PC4=SDA
; PC5=SCL
; PD1=TXD
; board STK500
;
;-----------------------------------------------------------------

.include "m48def.inc"


.def tmp = R16
.def DATA = R17
.def AddressH = R18
.def AddressL = R19
.def DataRead = R20
;*********************************************************************

.equ ADDR24LC256	=0xA0	; LC256 address byte (1010 000R)

;*********************************************************************
.macro WAIT
wait1:
LDS tmp,TWCR
sbrs tmp,TWINT
rjmp wait1
.endmacro

;*********************************************************************
.macro START
ldi tmp,(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);send START
STS TWCR,tmp
.endmacro

;*********************************************************************
.macro STOP
ldi tmp,(1<<TWINT)|(1<<TWSTO)|(1<<TWEN)	;send STOP
STS TWCR,tmp
.endmacro

;*********************************************************************
.macro Delay10ms
;set timer2 normal port operation
LDI tmp,0x00
STS TCCR2A,tmp
;reset interrupts of timer2
LDI tmp,0x00
STS TIMSK2,tmp
;prescaler to 1024 0B00000111 so the timer starts
LDI tmp,0B00000111
STS TCCR2B,tmp
Attendi:	;when timer reachs 78 we have a delay of 10ms at 8MHz
lds tmp,TCNT2
cpi tmp,78
brne attendi
;timer off
LDI tmp,0B00000000
STS TCCR2B,tmp
STS TCNT2,tmp
.endmacro

;*************************************************************************
;
;  Reset and interrupt vectors
;
;*************************************************************************
.cseg
.org $00
   rjmp Reset       ; Go to Reset handler


Reset:

	LDI DATA,0B10101010
	LDI AddressH, 0B00000000
	LDI AddressL, 0B00000000

	Rcall TWI_Setup

;Write data into EEPROM 
Send:
	Rcall TWI_Send
Read:
;Read data from EEPROM
	Rcall TWI_Read
;Send dataread to pc by RS232	
	Rcall SendToPc
END:
	Rjmp END


;*********************************************************************
;
;	Send EEPROM data by usart
;
SendToPc:
	ldi		tmp,51		; Set Baudrate register to 51
	STS		UBRR0L,tmp		; 9600 baud x 8MHz
		
	LDS 	tmp,UCSR0B		;enable transmission
	SBR 	tmp,8
	STS 	UCSR0B, tmp

	;transmit value
	STS		UDR0,DataRead

wait1:	;wait for Transmit complete bit
	LDS 	tmp,UCSR0A
	SBRS	tmp, TXC0
	rjmp	wait1
	
	ret

;*********************************************************************
TWI_Setup:
	LDI tmp,32	;velocity 100KHz
	STS TWBR,tmp
	LDI tmp,(0<<TWPS1)|(0<<TWPS0)
	STS TWSR,tmp
	REt

;*********************************************************************
TWI_Send:
	
	START

	WAIT

	ldi tmp,ADDR24LC256		;send EEPROM Address 
	STS TWDR,tmp
	ldi tmp,(1<<TWINT)|(1<<TWEN)
	STS TWCR,tmp

	WAIT

	STS TWDR,AddressH	;send Address High byte
	ldi tmp,(1<<TWINT)|(1<<TWEN)
	STS TWCR,tmp

	WAIT

	STS TWDR,AddressL	;send Address Low Byte
	ldi tmp,(1<<TWINT)|(1<<TWEN)
	STS TWCR,tmp

	WAIT

	STS TWDR,DATA	;send EEPROM DATA
	ldi tmp,(1<<TWINT)|(1<<TWEN)
	STS TWCR,tmp

	WAIT

	STOP
	
	;EEPROM need 10ms to store data
	Delay10ms
	
ret

;*********************************************************************
TWI_Read:

	START

	WAIT

	ldi tmp,ADDR24LC256	;send EEPROM Address 
	STS TWDR,tmp
	ldi tmp,(1<<TWINT)|(1<<TWEN)
	STS TWCR,tmp

	WAIT

	STS TWDR,AddressH	;send Slave Address
	ldi tmp,(1<<TWINT)|(1<<TWEN)
	STS TWCR,tmp

	WAIT

	STS TWDR,AddressL	;send Slave Address
	ldi tmp,(1<<TWINT)|(1<<TWEN)
	STS TWCR,tmp

	WAIT

	START

	WAIT

	ldi tmp,ADDR24LC256+1	;send EEPROM Address 
	STS TWDR,tmp
	ldi tmp,(1<<TWINT)|(1<<TWEN)
	STS TWCR,tmp

	WAIT

	ldi tmp,(1<<TWINT)|(1<<TWEN)
	STS TWCR,tmp

	WAIT

	STOP

	;Store data in DataRead
	LDS DataRead,TWDR
	NOP
ret
