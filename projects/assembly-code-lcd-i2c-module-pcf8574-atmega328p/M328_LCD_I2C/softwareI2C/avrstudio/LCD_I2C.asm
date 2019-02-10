;Saturday, February 09, 2019, 8:49:28 PM
;LCD to I2C board with PCF8574
;boards available on ebay for under $1 (see photo in zip file)
;this code works with the default address (no mods to board)
;i.e. A0, A1, A2 all high or untouched
;
;using bit-bang I2C routines from avr300.asm appnote
;there is no error checking in this version
;there is no reading I2C or reading from LCD display
;keep it simple!
;the "C" version is at least five times the size of this code
;
;works on any atmega328p with 16MHz crystal
;tested in arduino UNO R3 and Arduino NANO
;
;program fuses:
;FuseL: 0xFF
;FuseH: 0xDE
;FuseExt: 0xFC
;
;Arduino UNO R3 pins or
;Arduino NANO pins
;connect SDA to A4
;connect SCL to A5
;connect 5V and GND
;
;on atmega328p
;SCL = Pin 28 (PC5)
;SDA = Pin 27 (PC4)
;
.nolist
.include "m328def.inc"
.list
;------------------------------------------------
;for long delays
.def	dcount  =r6
.def	dcount2 =r7
.def	dcount3 =r8
.def	dcount4 =r9
;
.def	temp		= r16		; 
.def	i2cdata	= r17		; I2C data transfer register
.def	i2cstat	= r19		; I2C bus status register
.def	i2cdelay	= r20		; Delay loop variable
.def	LCD_Reg 	= r21		;
.def	count 	= r22		;

.equ i2cadr = $4E		; the slave address of the PCF8574
;
.equ     FirstLine_Add =0x80 	;Base address of first line for 2x16 lcd
.equ     SecondLine_Add=0xC0 	;Base address of second line
;
;these are the bits in LCD_Reg (not the I/O)
;bit 0 is only changed when sending addr byte
;Bit 0 = R or W bit
.equ i2cwr = 0 			; Write Bit
.equ i2crd = 1 			; Read Bit (not used)
;
;Port C
.equ	I2CPORT 	= PORTC	;*Set to correspond to I/O Port used for I2C
.equ	I2CDDR  	= DDRC	;*Set to correspond to I/O Port used for I2C
.equ	I2CPIN  	= PINC	;*Set to correspond to I/O Port used for I2C
;
.equ	SDAP = 4
.equ	SCLP = 5
;
.equ	LCD_RS = 0		;low to send command, high to send data	
.equ	LCD_RW = 1		;low = write, high = read
.equ	LCD_E = 2		;high (idle) high to low to send byte
;
;LCD_Data = bits 4 - 7	;
;------------------------------------------------
.cseg
.org 0
;
Reset:
	rjmp Begin

Begin:
	ldi temp,low(RAMEND) 	; Set stackptr to ram end
	out SPL,temp
	ldi temp, high(RAMEND)
	out SPH, temp

	rcall	i2c_init	   		;Initialize I2C bus
	rcall	LCD_init
	
Print_Msg:
;	ldi	LCD_Reg,FirstLine_Add	;cursor to 1st line, first position
;	rcall	LCD_command			;should already be there from initialize

	ldi	ZH,HIGH(message1*2) 	;Point to message1
	ldi	ZL,LOW(message1*2)
	ldi	count,0x10			;max 16 chars per line
	rcall	WrLCD				;Transmit message

	ldi	LCD_Reg,SecondLine_Add	;cursor to 2nd line, first position
	rcall	LCD_command			;

	ldi	ZH,HIGH(message2*2) 	;Point to message2
	ldi	ZL,LOW(message2*2)	;
	ldi	count,0x10			;max 16 chars per line
	rcall	WrLCD				;Transmit message
;------------------------------------
loop:	
	rjmp loop
;------------------------------------
.include "LCD.inc"
.include "delays.inc"
.include "I2C.inc"
;
;don't need the bootloader unless you want to program with arduino IDE
;if so, uncomment the next two lines
;.org FIRSTBOOTSTART
;.include "bootloader.inc"
;-------------------------------------
.exit