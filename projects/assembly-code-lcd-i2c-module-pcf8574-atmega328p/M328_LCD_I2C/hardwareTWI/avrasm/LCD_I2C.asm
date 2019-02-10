;Thursday, February 07, 2019, 10:10:16 AM
;LCD to I2C board with PCF8574
;boards available on ebay for under $1 (see photo in zip file)
;this code works with the default address (no mods to board)
;i.e. A0, A1, A2 all high or untouched
;
;using TWI interface
;there is no error checking in this version
;
;works on any atmega328p with 16MHz crystal
;tested in arduino UNO R3 and Arduino NANO
;
;program fuses:
;FuseL: 0xFF
;FuseH: 0xDE
;FuseExt: 0xFD
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
.def temp = r16 	;
;.def temp1 = r17 	;
.def	LCD_Reg = r18	;
.def	I2WData = r19	; Write data for I2C routines
.def	count = r20		;
;
; Equate statements
.equ start = $08 		; Start Condition
.equ SLA = $4E 		; the slave address of the PCF8574
;
.equ     FirstLine_Add =0x80 	;Base address of first line for 2x16 lcd
.equ     SecondLine_Add=0xC0 	;Base address of second line
;
;these are the bits in LCD_Reg (not the I/O)
;bit 0 is only changed when sending addr byte
;Bit 0 = R or W bit
.equ W = 0 			; Write Bit
.equ R = 1 			; Read Bit (not used)
;
.equ	LCD_RS = 0		;low to send command, high to send data	
.equ	LCD_RW = 1		;low = write, high = read
.equ	LCD_E = 2		;high (idle) high to low to send byte
.equ	BackLight = 3	;high turns on the backlight, low turns it off
;
;I don't have an LCD with a backlight so I didn't include this.
;if you want to add it in, you will have to set this bit in LCD_Reg every time
;"E" is pulsed or set it at the beginning and be careful 
;when you clear the lower 4 bits before "E" is pulsed
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

	rcall	I2CInit	   		;Initialize I2C bus
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
.include "TWI.inc"
;
;uncomment the next two lines if you want the bootloader included
;.org FIRSTBOOTSTART
;.include "bootloader.inc"
;-------------------------------------
.exit