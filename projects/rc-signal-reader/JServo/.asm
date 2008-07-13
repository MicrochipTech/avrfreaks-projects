 //Program to read servo signal from PortC PIN5 and display a value based on pulse width
 //aTMega48 running at  1MHZ  every clock cycle is 1us
 //Servo needs a pulse of 1000us to 2000us .1500us is the middle position
 
 
 .include "m48def.inc"
 .def 	Hundred =r13
 .def	Ten		=r14
 .def	Unit	=r15
 .def Temp		=r16
 .def Temp2		=r17
 .def CounterA	=r18
 
;***** Initialization;
	.CSEG
.org $0000
; Reset- and Interrupt-vectors
;
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

rcall P13CINTHandler

reti
;
APCI2addr:
reti
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
	rcall Comp0AIntHandler
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
	ldi Temp,0xFE
	out SPL,Temp
	ldi Temp,0x2
	out	SPH,Temp	;setting stack pointer
	

clr CounterA
sbi DDRC,DDC5
cbi PORTC,PC5
rcall InitializeUART
		//	rcall SetupCompareTimer
		//	ldi Temp,0xFF
		//	out OCR0A,Temp

ldi Temp,(0<<DDC5)			//configure portC Pin 5 as input for pin change interrupt
out DDRC,Temp
rcall SetupPCInt
rcall SetupCounterTimer
sei
ForeeverLoop:
rjmp ForeeverLoop   //wait for interrupts

InitializeUART:		
	
	ldi	Temp,12	; Baud Rate = 4800	
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
ret


SetupCompareTimer:
;** Enable timer inerrupts
	ldi		Temp, (1<<OCIE0A)  
	sts		TIMSK0, Temp

	;** Setup timer1
	clr		Temp
	sbr 	Temp,2
	out 	TCCR0A,Temp

	ldi		Temp, (1<<CS02)+(1<<CS00)
	out		TCCR0B, Temp

	
	;** Enable interrupts
	//sei

ret


Comp0AIntHandler:

inc CounterA
cpi CounterA,0x2
brne Endloop
clr CounterA
in Temp,PINC
sbrc Temp,PINC5
rjmp ClearLED
sbrs Temp,PINC5
rjmp SetLED
ClearLED:

cbi PORTC,PC5
rjmp EndLoop

SetLED:
sbi PORTC,PC5
Endloop:

reti

SetupPCInt:
clr Temp
sbr Temp,2		//set PCIE1 bit PCINT 8-14 will be enabled
sts PCICR,Temp
sbr Temp,0x20		//set bit 5 to enable PCINT13
sts PCMSK1,Temp

ret




P13CINTHandler:
in Temp,PINC
sbrc Temp,PINC5
rjmp ZeroCounter
rjmp ReadAndSendCounter

ZeroCounter:
clr Temp
out TCNT0,Temp
rjmp PCINT13End


ReadAndSendCounter:
clr Temp
in   Temp,TCNT0
rcall Binary2Ascii
rcall WriteXYZ
rcall WriteNL
///////rcall UARTTransmit

PCINT13End:
ret



SetupCounterTimer:
;** Enable timer inerrupts
	clr		Temp
	sts		TIMSK0, Temp

	;** Setup timer1

	
	out 	TCCR0A,Temp
	ldi		Temp, (1<<CS01)
	out		TCCR0B, Temp

	
	;** Enable interrupts
	//sei

ret


UARTTransmit:
	sts	UDR0, Temp	; output char to UART Data Reg
UARTTransmitLoop:
	lds Temp,UCSR0A
	sbrs Temp,UDRE0
	rjmp UARTTransmitLoop
	ret


R1Interpreter:
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
//rcall WriteNL
mov Temp,Temp2

ret

WriteOne:
ldi Temp,0x31
rcall UARTTransmit
ret
WriteZero:
ldi Temp,0x30
rcall UARTTransmit
ret
WriteNL:
ldi Temp,0xA
rcall UARTTransmit
ldi Temp,0xD
rcall UARTTransmit
ret


/////////////////////////////////////////////////////////////
Binary2Ascii:
ldi Temp2,0x30
mov	Hundred,Temp2			//routine to convert 8 bit binary number into an ascii string 
mov Ten,Temp2				//Hundred stored in Hundred variable and so on
mov Unit,Temp2

B2ALoop1:
cpi Temp,0x64

brcc	IncHundred
clc
B2ALoop2:
cpi	Temp,0xa

brcc	IncTen
clc
add Unit,Temp

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
////This Routine writes the ascii value of number along with sign
/////////////////////// 

WriteXYZ:

mov Temp,Hundred
rcall UARTTransmit
mov	Temp,Ten
rcall UARTTransmit
mov Temp,Unit
rcall UARTTransmit
ldi Temp,0x20
rcall UARTTransmit

ret
///////////////////////////////////////////////////////////////////////////////////////
////////////////
