;A program to read LIS302DL accelerometer and display readings via HyperTerminal
;Connection to Accelerometer is through SPI bus
;AtMega 48 is running at 8 MHz internal clock speed
;PB2 is used as chip select line in addition to normal SPI lines on ATMega48 and LIS 302 DL
;ST3232 is used to convert between TTL RS232 Signal and standard RS232 in serial port


 .include "m48def.inc"



 .def	Unit	=r7
 .def	Zero=	r9
  .def 	Hundred =r13
 .def	Ten		=r14
  .def	Temp	=r16		;temporary register

.def	Temp2	=r21
.def	Temp3	=r22   //should be used locally only

 .def	Gx		=r23
 .def	Gy		=r24
 .def	Gz		=r25
 .def	Sign	=r26

;***** Initialization;
.DSEG



	.CSEG
;.org $0000
;**** Interrupt Vectors ****	
rjmp Start	
rjmp	AINT0addr     ;01	External Interrupt0
rjmp	AINT1addr     ;02	external Interrupt1
rjmp	APCI0addr   ;03	Pin Change Interrupt0
rjmp	APCI1addr   ;04	Pin Change Interrupt1
rjmp	APCI2addr   ;05	Pin Change Interrupt2
rjmp	AWDTaddr	     ;06	Watchdog Timeout
rjmp	AOC2Aaddr     ;07	Timer/Counter2 Compare Match Interrupt
rjmp	AOC2Baddr     ;08	Timer/Counter2 Compare Match Interrupt	
rjmp	AOVF2addr     ;09	Overflow2 Interrupt
rjmp	AICP1addr     ;0a	Input Capture1 Interrupt 	
rjmp	AOC1Aaddr     ;0b	Output Compare1A Interrupt 
rjmp	AOC1Baddr     ;0c	Output Compare1B Interrupt 
rjmp	AOVF1addr     ;0d	Overflow1 Interrupt 
rjmp	AOC0Aaddr     ;0e	Timer/Counter0 Compare Match Interrupt
rjmp	AOC0Baddr     ;0f	Timer/Counter0 Compare Match Interrupt	
rjmp	AOVF0addr     ;10	Overflow0 Interrupt
rjmp	ASPIaddr      ;11	SPI Interrupt 	
rjmp	AURXCaddr     ;12	USART Receive Complete Interrupt 
rjmp	AUDREaddr     ;13	SART Data Register Empty Interrupt 
rjmp	AUTXCaddr     ;14	USART Transmit Complete Interrupt 
rjmp    AADCCaddr     ;15	ADC Conversion Complete Handle
rjmp	AERDYaddr     ;16	EEPROM write complete
rjmp	AACIaddr	     ;17	Analog Comparator Interrupt 
rjmp    ATWIaddr      ;18	TWI Interrupt Vector Address
rjmp	ASPMRaddr     ;19	Store Program Memory Ready Interrupt 


	


;
; ************** Interrupt service routines ********
;
; External Interrupt 0
;
AInt0addr:
	reti
;
; External Interrupt 1
;
AInt1addr:
	reti
;
; Timer/Counter 1, Capture event
;
APCI0addr:
reti
;
APCI1addr:

reti
;
APCI2addr:

;

AWDTaddr:
	reti
;
; Watch dog timer interrupt
;
AOC2Aaddr:
	reti
;
; Timer/Counter 1, Compare match interrupt B
;
AOC2Baddr:
	reti
;
; Timer/Counter 1, Overflow interrupt
;
AOVF2addr:
	reti
;
; Timer/Counter 0, Overflow interrupt
;
AICP1addr:
	reti
;
; SPI Serial Transfer Complete interrupt
;
AOC1Aaddr:

reti
	
;
; Uart Rx Complete Interrupt
;
AOC1Baddr:
	reti
;
; Uart Data register empty interrupt
;
AOVF1addr:
	reti
;
; Uart Tx complete interrupt
;
AOC0Aaddr:
	
	reti
;
; Analog comparator interrupt
;
AOC0Baddr:
	reti
;
AOVF0addr:
	reti
	;
ASPIaddr:
reti
;
AURXCaddr:
reti

AUDREaddr:
reti

AUTXCaddr:
reti

AADCCaddr:
reti

AERDYaddr:
reti

AACIaddr:
reti
ATWIaddr:
reti
ASPMRaddr:
reti

;

; **************** End of interrupt service routines ***********
Start:
	ldi Temp,LOW(RAMEND)
	out SPL,Temp
	ldi Temp,HIGH(RAMEND)
	out	SPH,Temp	;setting stack pointer
	sei

InitializeUART:		
	
	ldi	Temp,103;103 for 8 mhz,12 for 1 mhz	; Baud Rate = 4800	
	sts UBRR0L,Temp
	ldi Temp,0
	sts	UBRR0H,Temp

	; Set frame format: 8data, 1stop bit
	ldi r16,0
ldi r16, (0<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01)
sts UCSR0C,r16	; Enable receiver and transmitter
ldi r16,0
ldi r16, (0<<RXEN0)|(1<<TXEN0)
sts UCSR0B,r16

rcall InitializeSPI
				
	

InitializeSPI:

clr Temp
; Enable SPI, Master, set clock rate fck/16
ldi Temp,(1<<SPE)|(1<<MSTR)|(1<<SPR0)|(0<<SPR1)
out SPCR,Temp

;setting data direction register to output and PB2 to zero forslave select


ldi Temp,(1<<DDB0)|(1<<DDB2)|(1<<DDB3)|(1<<DDB5)|(0<<DDB4)
out	DDRB,Temp

ldi Temp,(0<<DDC0)|(0<<DDC1)|(0<<DDC2)|(1<<DDC4)|(1<<DDC5)	;	DDRC Setup for reading and writing 
out	DDRC,Temp


sbi PORTB,PORTB2



sei

AccelLoop1:
rcall StartAccel   

Forever:	;InfiniteLoop
rjmp AccelLoop1
					



SPI_write_byte:
	
	out		SPDR,Temp		; Daten ins Transferregister

wait_transmit_write:
	in	Temp,SPSR
	sbrs Temp,SPIF				; SPIF=1 wenn Transfer fertig,skip if SPIF=1
	rjmp	wait_transmit_write

	in		Temp,SPDR		; Rückgabe einlesen (SPIF lösc

	ret
UARTTransmit:
push Temp
	sts	UDR0, Temp	; output char to UART Data Reg
UARTTransmitLoop:
	lds Temp,UCSR0A
	sbrs Temp,UDRE0
	rjmp UARTTransmitLoop
	pop Temp
	ret


R1Interpreter:				;Prints the binary representation of a byte via UART
push Temp
push Temp2
rcall WriteNL
mov Temp2,Temp

Loop6:
sbrc Temp2,7
rcall WriteOne
sbrs Temp2,7
rcall WriteZero
sbrc Temp2,6
rcall WriteOne
sbrs Temp2,6
rcall WriteZero
sbrc Temp2,5
rcall WriteOne
sbrs Temp2,5
rcall WriteZero
sbrc Temp2,4
rcall WriteOne
sbrs Temp2,4
rcall WriteZero
sbrc Temp2,3
rcall WriteOne
sbrs Temp2,3
rcall WriteZero
sbrc Temp2,2
rcall WriteOne
sbrs Temp2,2
rcall WriteZero
sbrc Temp2,1
rcall WriteOne
sbrs Temp2,1
rcall WriteZero
sbrc Temp2,0
rcall WriteOne
sbrs Temp2,0
rcall WriteZero
rcall WriteNL
mov Temp,Temp2
pop Temp2
pop Temp
ret

WriteOne:
ldi Temp,0x31
rcall UARTTransmit
ret
WriteZero:
ldi Temp,0x30
rcall UARTTransmit
ret
WriteNL:				;Writes NewLine character via UART
push Temp
ldi Temp,0xA
rcall UARTTransmit
ldi Temp,0xD
rcall UARTTransmit
pop Temp
ret






StartAccel:
sbi PORTB,PORTB2
//////////////sets up the lis302dl accelerometer for reading at 100Hz or 400 Hz,2G range
//	Write 47h in controlReg1(0x20)
//Command 00100000 (Write ControlReg1)      1000111

/////////20h,47h,0xff,0xff,0xff
cbi PORTB,PORTB2
ldi Temp,0x20
rcall SPI_write_byte
ldi Temp,0xc7///C3 is with Z axis Disabled,C7 is for 400 Hz data rate...47h is for 100hz data
rcall SPI_write_byte
sbi PORTB,PORTB2

//	Write ControlReg3 (0x22h)

//////00100010,00100100

/////22h,0x84,0xff	//Enabe Pin8  Interrupt on data ready

/*
cbi PORTB,PORTB2
ldi Temp,0x22
rcall SPI_write_byte
ldi Temp,0x24		///Pin 8 Interrupt Interrrupt level is high
rcall SPI_write_byte
sbi PORTB,PORTB2
*/
clr Gx

clr Gy

clr Gz

clr Zero





/////////////Main routine reads status register and if any value is updated is read

//	ReadStatus Register(0x27)

//	Command 10100111 (27h),Read value will come out of DO

/////A7,0xff,0xff,0xff

Readlis302StatusReg:

sbi PORTB,PORTB2
cbi PORTB,PORTB2
ldi Temp,0xa7
rcall SPI_write_byte
ldi Temp,0xff
rcall SPI_write_byte
sbi PORTB,PORTB2
cpi Temp,0
breq Readlis302StatusReg




;push Temp
;sbrc Temp,6
rcall ReadZ

;pop Temp
;push Temp
;sbrc Temp,5
rcall ReadY

;pop Temp
;sbrc Temp,4

rcall ReadX









		rjmp Readlis302StatusReg

ret
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
Binary2Ascii:
push Temp
push Temp2
ldi Temp2,0x30
mov	Hundred,Temp2			//routine to convert 8 bit binary number into an ascii string 
mov Ten,Temp2				//Hundred stored in Hundred variable and so on
mov Unit,Temp2
clr Sign
ldi Sign,0x2b

B2ALoop1:
cpi Temp,0x64

brcc	IncHundred
clc
B2ALoop2:
cpi	Temp,0xa

brcc	IncTen
clc
add Unit,Temp
pop Temp2
pop Temp
ret

IncHundred:

inc Hundred
subi Temp,0x64

rjmp B2Aloop1

IncTen:

inc Ten
subi Temp,0xa
rjmp B2ALoop2



////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////This Routine writes the ascii value of number
/////////////////////// 

WriteXYZ:
push Temp
mov Temp,Hundred
rcall UARTTransmit
mov	Temp,Ten
rcall UARTTransmit
mov Temp,Unit
rcall UARTTransmit
ldi Temp,0x20
rcall UARTTransmit
pop Temp
ret


//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
/////////////ReadX,ReadY and ReadZ subroutines
//////////Reads the values and saves it to Register Gx,Gy,Gz.The value is converted from two's complement to
///////////represent all positive numbers 
////////////////////////////

ReadX:
cbi PORTB,PORTB2
ldi Temp,0xa9
rcall SPI_write_byte
ldi Temp,0xff
rcall SPI_write_byte
sbi PORTB,PORTB2

//////if SatusReg (7) ==1   Read data,else goto ReadStatusRegister

////A9,0xff,0xff
clr Gx
push Temp

clc

ldi sign,0x80
pop Temp
sbrc Temp,7
rjmp Xloop2


Xloop1:
add Gx,Temp
add Gx,sign

rjmp ReadXEnd
Xloop2:
neg Temp
sub sign,Temp
mov Gx,sign
ReadXEnd:
ldi Temp,88
rcall UARTTransmit
ldi Temp,32
rcall UARTTransmit
mov Temp,Gx
rcall Binary2Ascii
rcall WriteXYZ
ldi Temp,32
rcall UARTTransmit
ret

ReadY:

//	Read OUTY (0x2B)

/////////AB,0xff,0xff,0xff
clr Gy
cbi PORTB,PORTB2
ldi Temp,0xab
rcall SPI_write_byte
ldi Temp,0xff
rcall SPI_write_byte
sbi PORTB,PORTB2
push Temp
clc


ldi sign,0x80
pop Temp
sbrc Temp,7
rjmp Yloop2
Yloop1:
mov Gy,Temp
add Gy,sign

rjmp ReadYEnd

Yloop2:
;push Temp
neg Temp
sub sign,Temp
mov Gy,sign
;pop Temp
ReadYEnd:
ldi Temp,89
rcall UARTTransmit
ldi Temp,32
rcall UARTTransmit
mov Temp,Gy
rcall Binary2Ascii
rcall WriteXYZ
ldi Temp,32
rcall UARTTransmit
ret


ReadZ:


clr Gz
cbi PORTB,PORTB2
ldi Temp,0xad
rcall SPI_write_byte
ldi Temp,0xff
rcall SPI_write_byte
sbi PORTB,PORTB2
 
clc

ldi sign,0x80
sbrc Temp,7
rjmp Zloop2
Zloop1:
add Gz,Temp
add Gz,sign

rjmp ReadZEnd
Zloop2:

neg Temp
sub sign,Temp
mov Gz,sign

ReadZEnd:
ldi Temp,90
rcall UARTTransmit
ldi Temp,32
rcall UARTTransmit
mov Temp,Gz
rcall Binary2Ascii
rcall WriteXYZ
ldi Temp,32
rcall UARTTransmit
rcall WriteNL

ret
