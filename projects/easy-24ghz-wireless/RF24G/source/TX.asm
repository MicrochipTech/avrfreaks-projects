;-----------------------------------------------
; Nordic nRF2401A 2.4GHz Transceiver Software
;-----------------------------------------------
;Tuesday, October 09, 2007, 7:05:02 PM
;
;Starts out in RX mode, and monitors DR line
;if data comes in, print to terminal.
;To activate Transmit mode, press a key in terminal. This
;activates the usart interrupt and sets a flag to switch to
;TX mode and transmit data to the other RF24G.
;When done transmitting, RF24G returns to RX mode and
;monitors DR line for next data.
;
.nolist
.include "m8515def.inc"
.list
;-----------------------------------------------
;definitions, register alias and constants
;-----------------------------------------------
.equ		CE	= 2	;PD2
.equ		CS	= 3	;PD3
.equ		DR1	= 4	;PD4
.equ		CLK 	= 5	;PD5
.equ		Data	= 6	;PD6

.equ		RamBuffer =	0x60

.def		Temp	=	r16
.def		Address =	r17
.def		Data1	=	r18
.def		temp2	=	r19
.def		Counter =	r20
.def		BitCnt =	r21	
.def		ByteCnt =   r22
.def		flags   =  r23
;
.org	0
Reset:
	jmp	Start			; 

.org	URXCaddr
	jmp	UART_RX_interrupt	;
;****************************************
Start:
	ldi	temp2,Low(RAMEND)
	out	SPL,temp2		; Set stack pointer
	ldi	temp2,High(RAMEND)
	out	SPH,temp2
	
	ldi	temp,0b10101110 	;PD0,PD4,PD6 inputs
	out	DDRD,temp		;
	ldi	temp,0b00000000	;
	out	PORTD,temp

	rcall	USART_Init

;config nRF2401A
	rcall	BuildSPIWord	;configuration data
	rcall	SPInRF2401		;clock out config string
	sei				;enable interrupts
;****************************************
Main:
	sbic	PIND,DR1		;skip if DR is low (no data rec'd)
	rcall	ReceiveShock

	sbrc	Flags,0		;check if char rec'd from term
	rcall	Transmit 		;	
	rjmp	main
;****************************************
Transmit:
	rcall	TX_EN			;set TX mode
	rcall	BuildShockWord  
	rcall	ShockBurst		
	clr	Flags
	rcall	RX_EN
	ret
;****************************************	
TX_EN:
	cbi	PortD,CE	; config mode		
	sbi	PortD,CS	; config mode
	rcall	Del_5uS

	sbi	DDRD,DATA	; Set Data as output	
	cbi	PortD,DATA	; disable bit 0 (RXEN)

	rcall	DoSPIClock	; clock it

	nop
	nop
	cbi	PortD,CS	; Sleep Mode
	cbi	PortD,CE	;

	cbi	DDRD,DATA	; Set Data as Input
	ret
;****************************************
RX_EN:
	cbi	PortD,CE	; config mode	
	sbi	PortD,CS	; 
	rcall	Del_200uS		; 200uS delay

	sbi	DDRD,DATA	; Set Data as output	
	sbi	PortD,DATA	; enable RX (set bit 0)

	rcall	DoSPIClock	; clock it

	sbi	PortD,CE	; active mode
	cbi	PortD,CS	; 

	cbi	DDRD,DATA	; Set Data as input
	cbi	PortD,DATA	; Data Line Floating
	ret
;****************************************
;RF24G is configured for:
;RXEN, CHANNEL 2, RF Power: 0 DBm, 16MHz clock,Data Rate: 250 kbps,
;ShockBurst Mode, One Channel Receive, 16 bit CRC enabled, 40 bit address,
;10 bytes payload length
;
;transfer config string to Ram @RamBuffer			
BuildSPIWord:	
	ldi	XL,RamBuffer	
	clr	XH
	ldi	temp,15	; Number of bytes
	st	X+,temp
	
	ldi	ZH,high(configword*2)
	ldi	ZL,low(configword*2)

BuildLp:
	lpm
	st	X+,R0
	adiw	ZL,1
	dec	temp
	brne	BuildLp
	ret
;****************************************
configword:
.db 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA
.db 0xBB, 0xCC, 0xDD, 0xEE, 0xA3, 0x4F, 0x05, 0x00 ;<padded extra byte
;****************************************
BuildShockWord:	
	ldi	XL,RamBuffer	;0x0060 in ram
	clr	XH
	ldi	temp,15	; Number of bytes
	st	X+,temp

	ldi	temp,0xAA	; MSB    ;transmitter 1 address High byte
	st	X+,temp

	ldi	temp,0xBB	; MSB-1  ;transmitter 1 address High  byte
	st	X+,temp

	ldi	temp,0xCC	; MSB-2  ;transmitter 1 address Low byte
	st	X+,temp

	ldi	temp,0xDD	; MSB-3  ;transmitter 1 address Low  byte
	st	X+,temp

	ldi	temp,0xEE	; MSB-4  ;transmitter 1 address Low  byte
	st	X+,temp
		
	ldi	temp,0x30	; MSB-5  ;data byte 9
	st	X+,temp
	ldi	temp,0x31	; MSB-6  ; data byte 8
	st	X+,temp
	ldi	temp,0x32	; MSB-7  ; data byte 7
	st	X+,temp
	ldi	temp,0x33	; MSB-8  ; data byte 6
	st	X+,temp
	ldi	temp,0x34	; MSB-9  ; data byte 5
	st	X+,temp
	ldi	temp,0x35	; MSB-10  ; data byte 4
	st	X+,temp
	ldi	temp,0x36	; MSB-11 ; data byte 3
	st	X+,temp
	ldi	temp,0x37	; MSB-12 ; data byte 2
	st	X+,temp
	ldi	temp,0x38	; MSB-13 ; data byte 1
	st	X+,temp
	ldi	temp,0x39	; MSB-14 ; data byte 0   
	st	X+,temp
	ret
;****************************************
SPInRF2401:	
	ldi	XL,RamBuffer	; Load Address for where SPI data starts
	clr	XH			; 
	sbi	DDRD,DATA		; Set data as output

	cbi	PORTD,CE		; config mode
	sbi	PORTD,CS		; 

	rcall	Del_200uS		; 200uS delay

	ld	ByteCnt,X+		; Number of bytes
		
Highloop:
	ld	temp,X+		; Data in "temp"
	ldi	BitCnt,8		; "BitCnt" tracks number of programmed bits

LowLoop:
	sbrs	temp,7		; Check if first bit is HIGH
	cbi	PORTD,DATA		; NO: Set Data low
	sbrc	temp,7		; Check if first bit is LOW
	sbi	PORTD,DATA		; NO: Set Data high
	rcall	DoSPIClock		; Call "DoClock" to generate a clock pulse
	dec	BitCnt		; Decrement "BitCnt"
	lsl	temp			; Left Shift to get next bit
	cpi	BitCnt,0		; Check if all bits are written in this byte
	brne	Lowloop		; NO: Write next one

	dec	ByteCnt		; YES: Decrement "ByteCnt"
	cpi	ByteCnt,0		; Check if all bytes are written in this SPI round
	brne	Highloop		; NO: Get next byte and write it out

SPIReturn:
	sbi	PortD,CE		; ENABLE SHOCKBURST

	rcall	Delay10mS		; wait 10mS for TX

	cbi	PORTD,CE		; sleep mode
	cbi	PORTD,CS		;

	cbi	PORTD,DATA		; Set Data low
	cbi	DDRD,DATA		; Set data as input (low)
	ret

DoSPIClock:
	sbi	PORTD,CLK
	nop
	nop
	cbi	PORTD,CLK
	ret
;****************************************
ShockBurst:					
	sbi	DDRD,Data		; Set data as output

	sbi	PORTD,CE		; enable TX
	rcall	Del_200uS		; 200uS delay

	ldi	XL,RamBuffer	; 0x0060 in ram
	clr	XH			; 
	ld	ByteCnt,X+		; 1st byte is length of send
BurstLoop1:
	ld	temp,X+		; Load databyte in temp
	rcall	SendData		; Call SendData
	dec	ByteCnt		; Check if all bytes are sent
	cpi	ByteCnt,0
	brne	BurstLoop1		; NO: Loop around and do it again

BurstReturn:
	cbi	PORTD,CE		; ENABLE SHOCKBURST
	rcall	Delay10mS		;
	cbi	PORTD,Data		; Set Data low
	cbi	DDRD,Data		; Set data as input (low)
	ret
;**************************************************
;send a byte from X
SendData:
	ldi	BitCnt,8		; Load number of bits in 'BitCnt'
LowLoopburst:
	sbrs	temp,7			
	cbi	PORTD,Data		; Set Data low
	sbrc	temp,7
	sbi	PORTD,Data		; Set Data high
	rcall	DoSPIClock
	dec	BitCnt
	lsl	temp
	cpi	BitCnt,0
	brne	Lowloopburst	
	ret
;**************************************************
ReceiveShock:	
	cbi	DDRD,Data		; Set data as input
	cbi	PortD,Data		; Set data tri state
	ldi	XL,RamBuffer	; 0x0060 in ram
	clr	XH
	clr	ByteCnt

RxByteLp:
	clr	temp
	clr	BitCnt

in1loop:
	lsl	temp			; shift left to receive next bit
	sbi	PORTD,CLK		; set clock high
	nop
	nop				; wait one clk cycle
	sbic	PIND,Data		; check if Data=low		
	ori	temp,1		; no - set bit high
	cbi	PORTD,CLK		; set clock low
	nop				; wait one clk cycle
	inc	BitCnt		; increment bit counter
	cpi	BitCnt,8		; 
	brne	in1loop

	nop				;done 8 bits
	inc	ByteCnt		;next byte
	st	X+,temp		;store byte @X		
	sbis	PinD,DR1		;check if DR1 is still high
	rjmp	Receiveret		;no, exit subroutine
	rjmp	RxByteLp		;YES, get another byte
		
Receiveret:
	nop			
	ldi	XL,RamBuffer	; 0x0060 in ram
	clr	XH			; 

SendLoop:
	ld	temp,X+		;Load databyte in temp
	rcall	USART_TX		;Print to terminal
	dec	ByteCnt
	brne	SendLoop
	ret
;****************************************
.include "usart.inc"
.include "delays.inc"
.exit
			