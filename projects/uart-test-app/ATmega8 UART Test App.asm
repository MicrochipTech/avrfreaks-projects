;*****************************************************************************
;FILENAME: 	ATmega8 UART TEst App
;AUTHOR: 	CAS
;COMPANY:	
;TARGET DEVICE:	ATmega8
;TARGET CLOCK FREQ: 4MHz
;
;DESCRIPTION:
;	Tests UART hardware and firmware functionality by cycling through the 
;	ASCII character set and transmitting each character over RS232 to a PC 
;	terminal program (most commonly HyperTerminal).
;
;INTERNAL RESOURCES:
;	- 2 registers for delay generation
;	- 1 register for loop counting
;	- 1 register for ASCII value
;
;PERIPHERAL RESOURCES:
;	- UART Tx and Rx pins
;
;PIN UTILIZATION:
;	- Pin2 - PD0 - Rx
;	- Pin3 - PD1 - Tx
;	
;CHANGE HISTORY:
;	CAS 04Jul2005:
;		Started from scratch
;	
;	CAS 05Jul2005:
;		Added Carriage Return and Line Feed loops to insert a blank line 
;		between each block of ASCII characters.  Released source files and 
;		flash files into directory:
;
;*****************************************************************************




.include "m8def.inc"



;Constants & Expressions
;.ORG	$xx
;==============================================================================
.equ	ASCII_CHAR_OFFSET	=33
.equ	BAUD_9600			=25
;.equ
;.equ
;.equ
;.equ
;.equ
;.equ
;.equ
;.equ
;.equ
;.equ
;==============================================================================


;Definitions
;==============================================================================
;.def	=r0			;result byte for 'lpm' and low byte for 'mul' instructions
;.def	=r1			;result high byte for 'mul' instruction
;.def	=r2
;.def	=r3
;.def	=r4
;.def	=r5
;.def	=r6
;.def	=r7
;.def	=r8
;.def	=r9
;.def	=r10
;.def	=r11
;.def	=r12
;.def	=r13
;.def	=r14
;.def	=r15
.def	count0	=r16
.def	count1	=r17
.def	count2	=r18
.def	temp0	=r19
.def	temp1	=r20
;.def	=r21
;.def	=r22
;.def	=r23
;.def	=r24
;.def	=r25
;.def	=r26		;XL by default
;.def	=r27		;XH by default
;.def	=r28		;YL by default
;.def	=r29		;YH by default
;.def	=r30		;ZL by default
;.def	=r31		;ZH by default
;==============================================================================


;*****     Variables in SRAM     *****
;.dseg
;.ORG	$xx			;If necessary
;===================================================================================
;Put SRAM Variable Declarations here
;
;var_name:  .BYTE	#_of_bytes
;===================================================================================


;*****     Program Momory Data     *****
;.dseg
;.ORG	$xx			;If necessary
;===================================================================================
;Put Constant Declarations here
;
;const_bytes_name:
;.DB	128, 0xFF, 0b01010101	;comma-delimited byte values (odd # of bytes results 
;				;in compile-time warning)
;
;const_words_name:
;.DW	32768, 0xFFFF, 0b0101010101010101	;comma-delimited words
;===================================================================================


;*****     Initializations     *****
;.cseg
;.ORG	$xx			;If necessary
;===================================================================================
;Can put in .inc file for larger applications.
;===================================================================================


;Interrupts
.cseg
;==============================================================================
.org 0x000
	rjmp RESET
;.org INT0addr						;External Interrupt 0
;	rjmp _INT0
;.org INT1addr						;External Interrupt 1
;	rjmp _INT1
;.org OC2addr						;Timer/Counter 2 Compare Interrupt
;	rjmp _OC2INT	
;.org OVF2addr						;Timer/Counter 2 Overflow Interrupt
;	rjmp _OVF2INT
;.org ICP1addr						;Timer/Counter 1 Capture Event Interrupt
;	rjmp _ICP1INT
;.org OC1Aaddr						;Timer/Counter 1 Compare Match A Interrupt
;	rjmp _OC1AINT
;.org OC1Baddr						;Timer/Counter 1 Compare Match B Interrupt
;	rjmp _OC1BINT
;.org OVF1addr						;Timer/Counter 1 Overflow Interrupt
;	rjmp _OVF1INT
;.org OVF0addr						;Timer/Counter 0 Overflow Interrupt
;	rjmp _OVF0INT
;.org SPIaddr						;Serial Transfer Complete Interrupt
;	rjmp _SPIINT
;.org URXCaddr						;USART Rx Complete Interrupt
;	rjmp _URXINT
;.org UDREaddr						;USART Data Register Empty Interrupt
;	rjmp _EDREINT
;.org UTXCaddr						;USART Tx Complete Interrupt
;	rjmp _UTXCINT
;.org ADCCaddr						;A/D Conversion Complete Interrupt
;	rjmp _ADCCINT
;.org ERDYaddr						;EEPROM Ready Interrupt
;	rjmp _ERDYINT
;.org ACIaddr						;Analog Comparator Interrupt
;	rjmp _ACIINT
;.org TWIaddr						;Two-Wire Serial Interface Interrupt
;	rjmp _TWIINT
;.org SPMaddr						;Store Program Memory Ready Interrupt
;	rjmp _SPMINT
;==============================================================================



.cseg
RESET:
	clr	count0
	clr	count1
	clr count2

; U2X = 0
; UBRR = [fosc/16*BAUD]-1
	rjmp USART_init



TX_ASCII_CHARS:
	ldi temp0,ASCII_CHAR_OFFSET
next_ASCII_char:
	out UDR,temp0
	inc temp0
	breq RESTART_TX  
	rjmp DELAY0



RESTART_TX:
	clr count2
	ldi temp1,2

;	Send Carriage Return
	ldi temp0,13
x0:
	dec count0
	brne x0
	dec count1
	brne x0
	out UDR,temp0

;	Send Line Feed
	ldi temp0,10
x1:
	dec count0
	brne x1
	dec count1
	brne x1
	out UDR,temp0

	ldi temp0,13
	inc count2
	cpi count2,2
	brne x0
	
	ldi temp0,ASCII_CHAR_OFFSET
	rjmp DELAY0



DELAY0:
	clr count1
	clr count0
dly0:
	dec count0
	brne dly0
	dec count1
	brne dly0
	rjmp next_ASCII_char



USART_init:
;	Set baud rate
	ldi	temp1,(0<<URSEL)|high(BAUD_9600)
	ldi	temp0,low(BAUD_9600)
	out UBRRH,temp1
	out UBRRL,temp0
;	Enable Receiver and Transmitter
	ldi temp0,(0<<RXEN)|(1<<TXEN)
	out UCSRB,temp0
;	Set frame format: 8data, 2stop bit
	ldi temp0, (1<<URSEL)|(0<<USBS)|(3<<UCSZ0)
	out UCSRC,temp0
	rjmp TX_ASCII_CHARS
