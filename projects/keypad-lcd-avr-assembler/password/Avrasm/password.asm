;Thursday, January 07, 2021, 3:47:16 PM
;LCD to I2C board with PCF8574
;
;Grayhill 3x4 Keypad 96AB2-102-F 
;plugs right into arduino UNO PORTD
;pin 1 on keypad = PD0
;
;If user enters correct password (12345) on keypad
;LCD shows message Good Password.
;for someone trying to learn keypad and LCD routines
;
;works on any atmega328p with stock 1MHz clock
;tested in arduino UNO R3 and Arduino NANO
;
;program stock fuses:
;FuseL: 0x62
;FuseH: 0xD9
;FuseExt: 0xFF
;
;PCF8574 Board connections:
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
;LED's on PORTC
.equ	GLED = 0	;Green LED on PC0
.equ	RLED = 1	;Red LED on PC1
;
.nolist
.include "m328pdef.inc"
.list
;------------------------------------------------
;for long delays
.def	dcount  =r6
.def	dcount2 =r7
.def	dcount3 =r8
.def	dcount4 =r9
;
.def	KeyCnt	= R4 
;
.def	temp		= r16		; 
.def	i2cdata	= r17		; I2C data transfer register
.def	i2cstat	= r19		; I2C bus status register
.def	i2cdelay	= r20		; Delay loop variable
.def	LCD_Reg 	= r21		;
.def	count 	= r22		;
.def	key		= r23		; key pressed

.equ i2cadr = $4E		; the slave address of the PCF8574
;
.equ FirstLine_Add =0x80 	;Base address of first line for 2x16 lcd
.equ SecondLine_Add=0xC0 	;Base address of second line
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
	ldi temp,low(RAMEND) 		; Set stack pointer to ram end
	out SPL,temp
	ldi temp, high(RAMEND)
	out SPH, temp

	rcall	i2c_init	   		;Initialize I2C bus
	rcall	LCD_init
	rcall	Init_Keypad	

	in	temp,PORTC
	sbr	temp,(1<<RLED) | (1<<GLED)	;both LED's off
	out	PORTC,temp

	in	temp,DDRC
	sbr	temp,(1<<RLED) | (1<<GLED)	;both Led's are outputs
	out	DDRC,temp

	rcall	Delay250mS			;give LCD time to initialize

Get_Pass:
	ldi	temp,5			;5 chars in password
	mov	KeyCnt,temp

	ldi	ZH,HIGH(EnterPass_msg*2) 	;Point to "Enter Password" message
	ldi	ZL,LOW(EnterPass_msg*2)
	ldi	count,16			;max 16 chars per line
	rcall	WrLCD				;Transmit message

	ldi	LCD_Reg,SecondLine_Add	;move cursor to 2nd line, first position
	rcall	LCD_command	
		
	rcall InitY			;beginning of ram buffer

Get_PassLp:
	rcall	Get_Key		;wait for keypress
	st	Y+,key		;store key @ Y
	call	disp_no_of_key	;show the key pressed on the LCD
	dec	KeyCnt
	brne	Get_PassLp		;5 char password
					;user must enter all 5 chars

	call	InitY			;Buffer at $0100

	ld	Temp,Y+		;if password = 12345, success!
	cpi	temp,0x31		;
	brne	PassNG		

	ld	Temp,Y+		
	cpi	temp,0x32
	brne	PassNG

	ld	temp,Y+		
	cpi	temp,0x33
	brne	PassNG

	ld	temp,Y+		
	cpi	temp,0x34
	brne	PassNG

	ld	temp,Y+	
	cpi	temp,0x35
	brne	PassNG

	rjmp	GoodPass

PassNG:
	rjmp	BadPass	

EndProg:
	rjmp EndProg
;---------------------------------------------------
GoodPass:
	sbi PORTC, RLED	;turn off red led
	cbi PORTC, GLED	;turn on green led

	rcall	ClearLCD	;cursor to first line

	ldi	ZH,HIGH(GoodPass_msg*2) 	;Point to UNLocked message
	ldi	ZL,LOW(GoodPass_msg*2)
	ldi	count,16			;max 16 chars per line
	rcall	WrLCD				;Transmit message
	rjmp EndProg
;---------------------------------------------------
BadPass:
	cbi PORTC, RLED	;turn on red led
	sbi PORTC, GLED	;turn off green led

	rcall	ClearLCD	;cursor to first line

	ldi	ZH,HIGH(BadPass_msg*2) 	;Point to Locked message
	ldi	ZL,LOW(BadPass_msg*2)	;
	ldi	count,16			;max 16 chars per line
	rcall	WrLCD				;Transmit message
	rjmp EndProg
;---------------------------------------------------
InitY:
	ldi	YH,0x01
	ldi	YL,0x00		; start of rx buffer
	ret
;----------------------------------------------------
.include "LCD.inc"
.include "delays.inc"
.include "I2C.inc"
.include "keypad.inc"
;----------------------------------------------------
.exit