; Mrg1634.asm   MIDI merger _two inputs, one output.  No stuck notes. 
; Example use:  Connect a PC MIDI sequencer to one input channel and 
;  a MIDI keyboard to the other.  Plug the output into a multi channel
;  MIDI tone module in order to play along with the sequencer's output.
;
; use non-kerned font like Terminal or Lucida Console to display correctly.

.nolist
.include "tn1634def.inc"
.list

; 10/25/2014 10:42AM
; build: a  

;======================================================================================================
;
;   Atmel AVR Tiny1634  [16K flash / 1K SRAM [address range:0x100 to 0x4ff]/ 256 EEPROM]
;                  ________
;       TXD0  PB0  |1   20| PB1  RXD1   MOSI
;       RXD0  PA7  |2   19| PB2         MISO
;             PA6  |3   18| PB3
;             PA5  |4   17| PC0
;             PA4  |5   16| PC1         SCK
;             PA3  |6   15| PC2  LED_B
;             PA2  |7   14| PC3         reset\
;             PA1  |8   13| PC4        
;             PA0  |9   12| PC5        
;  merger     GND  |10  11| VCC         ISP signals
;                  --------
;  main differences from Mega168:  no MUL instruction: TWI is slave only: two USARTs: USI: no DIP package

;======================================
; TIMER equates  (for AVR Tiny1634)
;--------------------------------------
.equ	clock			=  8000000

.equ	HB_PRESCALE		=  256
.equ	HB_LOGIC		=  (1<<CS12) | (0<<CS11) | (0<<CS10)
.equ	HB_PER_SECOND		=   32

.equ	LongPeriod 		=  (clock / HB_PRESCALE) / HB_PER_SECOND
.equ	HEARTBEAT_IRQ_MASK	= TIMSK	  ;0x3a timer 1 interrupt mask register
.equ	HEARTBEAT_IRQ_FLAGS	= TIFR    ;0x39
.equ	HEARTBEAT_COUNT_HIGH	= TCNT1H  ;0x6f
.equ	HEARTBEAT_COUNT_LOW	= TCNT1L  ;0x6e
.equ	HEARTBEAT_CNTRL_SCALER	= TCCR1B  ;0x71

;======================================
; USART equates
;--------------------------------------
.equ	MIDIbaud		= ((clock/31250)/ 16) - 1   ; 15 for 8MHz clock
.equ	INQSIZE   		=   64
.equ	OUTQSIZE		=   64
.equ	INQMASK			= INQSIZE  - 1
.equ	OUTQMASK		= OUTQSIZE - 1

;======================================
; PORT and LED equates
;--------------------------------------
.equ	LED_DDR			= DDRC
.equ	LED_PORT		= PORTC
.equ	LED_B			= 2	; port C


;=====================================
;
;  	REGISTER Definitions
;
;=====================================
;
;; low register use
.def	SREGtemp 	= r14
.def	ZeroReg		= r15

;  upper registers   r16 - r31
.def	temp		= r16
.def	temp2		= r17
.def	temp3		= r18
.def	irqtemp		= r19
.def	TaskFlags	= r20
.equ	  T1_HeartBeat 	= 7
.def	ToggleReg	= r21

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



;===============================================================================================================
;
;  SRAM Definitions  ; 1024 bytes of static RAM in Tiny1634   SRAM_START= 0x0100   RAMEND = 0x04ff
;  stack grows down from RAMEND, max size is about 30 deep (six rcall returns + 18 pushed registers)
;==============================================================================================================
.dseg
InQ_Zero:		.byte	INQSIZE 	; 0x100 64 bytes
InQ_One:		.byte	INQSIZE 	; 0x140 64 bytes
OutQueue:		.byte	OUTQSIZE 	; 0x180 64 bytes
InQ_0_InPtr:		.byte	 1		; 0x1c0
InQ_0_OutPtr:		.byte	 1		; 0x1c1
InQ_1_InPtr:		.byte	 1		; 0x1c2
InQ_1_OutPtr:		.byte	 1		; 0x1c3
OutQ_InPtr:		.byte	 1		; 0x1c4
OutQ_OutPtr:		.byte	 1		; 0x1c5
HeartBeatCntr:		.byte	 1		; 0x1c6
SecondCount:		.byte	 1		; 0x1c7
PrevSecondCount:	.byte	 1		; 0x1c8
NewMIDI0byte:		.byte	 1		; 0x1c9
NewMIDI1byte:		.byte	 1		; 0x1ca

;___________________________________________________________________________
;SRAM variables for MIDI command parse routine
RunCmd0:		.byte	1 		; 0x1cb KEEP RunCmd just before MsgBuff!
MsgBuff0:		.byte	32		; 0x1cc teathered to RunCmd
MsgLen0:		.byte	1		; 0x1ec
BuffPtr0L:		.byte	1		; 0x1ed
BuffPtr0H:		.byte	1		; 0x1ee
MsgFlags0:		.byte	1		; 0x1ef
  .equ	isMsgRdy	= 7 	; always uses bit 7 for TST instruction
  .equ	isRunStatus	= 4
  .equ	isError		= 3
  .equ	isSysEx		= 2
  .equ	isThreeByte	= 1
  .equ	isTwoByte	= 0
;___________________________________________________________________________

;_____________________________________________________________________________
;SRAM variables for MIDI command parse routine
RunCmd1:		.byte	1 		; 0x1f0  KEEP RunCmd just before MsgBuff!
MsgBuff1:		.byte	32		; 0x1f1  teathered to RunCmd
MsgLen1:		.byte	1		; 0x211
BuffPtr1L:		.byte	1		; 0x212
BuffPtr1H:		.byte	1		; 0x213
MsgFlags1:		.byte	1		; 0x214
;  .equ	isMsgRdy	= 7 	; always uses bit 7 for TST instruction
;  .equ	isRunStatus	= 4
;  .equ	isError		= 3
;  .equ	isSysEx		= 2
;  .equ	isThreeByte	= 1
;  .equ	isTwoByte	= 0
;___________________________________________________________________________



.cseg
;***********************************************************************************************************
; interrupt vectors for Tiny1634
	jmp	reset
; ***** INTERRUPT VECTORS ************************************************
	reti nop		;= 0x0002	; External Interrupt Request 0
	reti nop		;= 0x0004	; Pin Change Interrupt Request 0
	reti nop		;= 0x0006	; Pin Change Interrupt Request 1
	reti nop		;= 0x0008	; Pin Change Interrupt Request 2
	reti nop		;= 0x000a	; Watchdog Time-out Interrupt
	reti nop		;= 0x000c	; Timer/Counter1 Capture Event
	reti nop		;= 0x000e	; Timer/Counter1 Compare Match A
	reti nop		;= 0x0010	; Timer/Counter1 Compare Match B
	jmp	TIM1_OVF	;= 0x0012	; Timer/Counter1 Overflow
	reti nop		;= 0x0014	; TimerCounter0 Compare Match A
	reti nop		;= 0x0016	; TimerCounter0 Compare Match B
	reti nop		;= 0x0018	; Timer/Couner0 Overflow
	reti nop		;= 0x001a	; Analog Comparator
	reti nop		;= 0x001c	; ADC Conversion Complete
	reti nop		;= 0x001e	; USART0, Start
	jmp	USART0_Rx	;= 0x0020	; USART0, Rx Complete
	jmp	UDRE_IRQ	;= 0x0022	; USART0 Data Register Empty
	reti nop		;= 0x0024	; USART0, Tx Complete
	reti nop		;= 0x0026	; USART1, Start
	jmp	USART1_Rx	;= 0x0028	; USART1, Rx Complete
	reti nop		;= 0x002a	; USART1 Data Register Empty
	reti nop		;= 0x002c	; USART1, Tx Complete
	reti nop		;= 0x002e	; USI Start Condition
	reti nop		;= 0x0030	; USI Overflow
	reti nop		;= 0x0032	; Two-wire Serial Interface
	reti nop		;= 0x0034	; EEPROM Ready
	reti nop		;= 0x0036	; Touch Sensing


reset:

; initialize Input/Output pins    	; for winking LED 
	ldi	temp, (1<<LED_B)
	out	LED_DDR,  temp		; DDRC
	out	LED_Port, temp		; PORTC lights on logic high


; initialize Timer 1  16bit
	ldi	temp, (1 << TOV1)
	out	HEARTBEAT_IRQ_FLAGS, temp	;TIFR   0x39 clear T1 IRQ flag
	ldi	temp, (1 << TOIE1)
	out	HEARTBEAT_IRQ_MASK, temp	;TIMSK  0x3a enable interrupt
	ldi	temp, high (~LongPeriod )
	sts	HEARTBEAT_COUNT_HIGH, temp	;TCNT1H 0x6f
	ldi	temp, low  (~LongPeriod )
	sts	HEARTBEAT_COUNT_LOW, temp 	;TCNT1L 0x6e
	ldi	temp, HB_LOGIC			; ( (1<<CS12) | (0<<CS11) | (0<<CS10) )
	sts	HEARTBEAT_CNTRL_SCALER, temp	;TCCR1B 0x71 start incrementing
	clr	temp				;  timer1 at every 256 system clocks
	sts	HeartBeatCntr, temp


; *** initialize USARTs   ; USCR0C default values OK for 8/N/1
	clr	temp
	out	UBRR0H, temp		; 0x22
	ldi	temp, MIDIbaud		; Baud rate
	out	UBRR0L, temp		; 0x21
	ldi	temp,  (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0)
	out	UCSR0B, temp 		; 0x25

	clr	temp
	sts	UBRR1H, temp            ; 0x75
	ldi	temp, MIDIbaud		; Baud rate
	sts	UBRR1L, temp           	; 0x74
	ldi	temp,  (1<<RXEN1) | (1<<RXCIE1)
	sts	UCSR1B, temp		; 0x78


; initialize global interrupt enable
	sei	; switch on the AVR general interrupt enabler

;=============================================================================
 ; initialize application
  	clr	taskflags
	clr	ZeroReg
 	sts	HeartBeatCntr,  ZeroReg
	sts	InQ_0_OutPtr,   ZeroReg
	sts	InQ_0_InPtr,    ZeroReg
	sts	InQ_1_OutPtr,   ZeroReg
	sts	InQ_1_InPtr,    ZeroReg
	sts	OutQ_OutPtr,    ZeroReg
	sts	OutQ_InPtr,     ZeroReg
	sts	SecondCount,    ZeroReg
	sts	PrevSecondCount,ZeroReg

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  
  

;##############################################################################################
;						;##############################################
;  Main Code loop				;##############################################
;    						;##############################################
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;##############################################
main_loop:					;####					   ####
	sbrc	TaskFlags, T1_HeartBeat		;####	 Heartbeat time?  32 a second	   ####
	rcall	HeartBeat 			;####					   ####
						;##############################################
						;##############################################
	lds	temp,  InQ_0_OutPtr		;##############################################
	lds	temp2, InQ_0_InPtr		;########			     ##########
	cp	temp, temp2			;########  any new MIDI Zero input?  ##########
	breq	mn_noNewMIDI0			;########			     ##########
	rcall	DoNewMIDI_Zero			;##############################################
mn_noNewMIDI0:					;##############################################
						;##############################################
	lds	temp,  InQ_1_OutPtr		;########			     ##########
	lds	temp2, InQ_1_InPtr		;########  any new MIDI One input?   ##########
	cp	temp, temp2			;########			     ##########
	breq	mn_noNewMIDI1			;##############################################
	rcall	DoNewMIDI_One			;##############################################
mn_noNewMIDI1:					;##############################################
						;##############################################
	rjmp	main_loop 			;##############################################
						;##############################################
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




;***************************************************************;
; Do_NewMIDI_Zero   Handle new MIDI bytes on USART0		;
;  Merge new MIDI data into a message and retransmit.		;
;      called when InQ_0_OutPtr is not equal to InQ_0_InPtr	;
;								;
;  	send:	 {SRAM}		Row_Current, Hex_Position	;
;	returns: {SRAM updated}	Row_Current, Hex_Position	;
;								;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;
DoNewMIDI_Zero:							; DoNewMIDI_0
	push	temp						; DoNewMIDI_0
	push	ZH						; DoNewMIDI_0
	push	ZL						; DoNewMIDI_0
	push	YH						; DoNewMIDI_0
	push	YL						; DoNewMIDI_0
	push	XH						; DoNewMIDI_0
	push	XL						; DoNewMIDI_0
								; DoNewMIDI_0
; load NewMIDI_byte and adjust pointer.				; DoNewMIDI_0
;  Byte placed in InQueue by USART_Rx0 IRQ.			; DoNewMIDI_0
	ldi	ZL, low  (InQ_Zero)				; DoNewMIDI_0
 	ldi	ZH, high (InQ_Zero)				; DoNewMIDI_0
	lds	temp, InQ_0_OutPtr ; tail of Rx1 queue		; DoNewMIDI_0
	add	ZL, temp					; DoNewMIDI_0
	adc	ZH, ZeroReg					; DoNewMIDI_0
	ld	temp2, Z					; DoNewMIDI_0
	sts	NewMIDI0byte, temp2				; DoNewMIDI_0
	inc	temp						; DoNewMIDI_0
	andi	temp, INQMASK					; DoNewMIDI_0
	sts	InQ_0_OutPtr, temp				; DoNewMIDI_0
;								; DoNewMIDI_0
;;-----------------------   used for testing			; DoNewMIDI_0    
;;  transmit single byte to MIDIOUT [USART #0]			; DoNewMIDI_0
;	ldi	YH, high (OutQueue)				; DoNewMIDI_0
;	ldi	YL, low  (OutQueue)				; DoNewMIDI_0
;	lds	temp3, OutQ_InPtr				; DoNewMIDI_0
;	add	YL, temp3					; DoNewMIDI_0
;	adc	YH, ZeroReg					; DoNewMIDI_0
;	inc	temp3						; DoNewMIDI_0
;	andi	temp3, OUTQMASK					; DoNewMIDI_0
;	sts	OutQ_InPtr, temp3				; DoNewMIDI_0
;	st	y, temp2					; DoNewMIDI_0
;;								; DoNewMIDI_0
;; transmit byte in OutQ by enabling UDRE interrupt		; DoNewMIDI_0
;	in	temp, UCSR0B	   	; 0x25			; DoNewMIDI_0
;	ori	temp, (1 << UDRIE0)	; bit 5			; DoNewMIDI_0
;	out	UCSR0B, temp					; DoNewMIDI_0
;;								; DoNewMIDI_0	
;-----------------------					; DoNewMIDI_0  
;===============================================================; Do_parse0
; New MIDI stored. Now parse new MIDI-In byte 			; Do_parse0
DNM0_parse:		 					; Do_parse0
	rcall	ParseMIDI_Zero					; Do_parse0
;								; Do_parse0
	lds	temp, MsgFlags0					; Do_parse0
	sbrc	temp, isError					; Do_parse0
	rjmp	DNM0_MPerror					; Do_parse0
;								; Do_parse0
	sbrs	temp, isMsgRdy					; Do_parse0
	rjmp	DNM0_exitparse					; Do_parse0
;								; Do_parse0
	lds	ZH, BuffPtr0H					; Do_parse0
	lds	ZL, BuffPtr0L					; Do_parse0
	lds	temp2, MsgLen0					; Do_parse0
;								; Do_parse0
DNM0_c:								; Do_parse0
  	ld 	temp, z+					; Do_parse0
;								; Do_parse0
DNM0_d:								; Do_parse0
	ldi	YH, high (OutQueue)				; Do_parse0
	ldi	YL, low  (OutQueue)				; Do_parse0
	lds	temp3, OutQ_InPtr				; Do_parse0
	add	YL, temp3					; Do_parse0
	adc	YH, ZeroReg					; Do_parse0
	inc	temp3						; Do_parse0
	andi	temp3, OUTQMASK					; Do_parse0
	sts	OutQ_InPtr, temp3				; Do_parse0
;								; Do_parse0
	st	y, temp						; Do_parse0
;								; Do_parse0
	dec	temp2						; Do_parse0
	brne	DNM0_c						; Do_parse0
;								; Do_parse0
; transmit new MIDI message by enabling UDRE interrupt		; Do_parse0
	in	temp, UCSR0B	   	; 0x25			; Do_parse0
	ori	temp, (1 << UDRIE0)	; bit 5			; Do_parse0
	out	UCSR0B, temp					; Do_parse0
;								; Do_parse0
	clr	temp						; Do_parse0
	sts	MsgLen0, temp					; Do_parse0
	sts	MsgFlags0, temp					; Do_parse0
								; Do_parse0
DNM0_exitparse:							; Do_parse0
;	rjmp	DMN0_exit 					; Do_parse0 
;								; Do_parse0            	
DMN0_exit:							; DoNewMIDI_0    	
	pop	XL						; DoNewMIDI_0    	
	pop	XH						; DoNewMIDI_0    	
	pop	YL						; DoNewMIDI_0    	
	pop	YH						; DoNewMIDI_0    	
	pop	ZL						; DoNewMIDI_0    	
	pop	ZH						; DoNewMIDI_0    	
	pop	temp						; DoNewMIDI_0    	
	ret							; DoNewMIDI_0    	
								; DoNewMIDI_0    	
;								; Do_parse0
DNM0_MPerror:							; Do_parse0
	rjmp	DMN0_exit 					; Do_parse0
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;%%%%%%%%%%




;***************************************************************;
; Do_NewMIDI_One   Handle new MIDI bytes on USART0		;
;  Merge new MIDI data into a message and retransmit.		;
;      called when InQ_0_OutPtr is not equal to InQ_0_InPtr	;
;								;
;  	send:	 {SRAM}		Row_Current, Hex_Position	;
;	returns: {SRAM updated}	Row_Current, Hex_Position	;
;								;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;
DoNewMIDI_One:							; DoNewMIDI_1
	push	temp						; DoNewMIDI_1
	push	ZH						; DoNewMIDI_1
	push	ZL						; DoNewMIDI_1
	push	YH						; DoNewMIDI_1
	push	YL						; DoNewMIDI_1
	push	XH						; DoNewMIDI_1
	push	XL						; DoNewMIDI_1
;		   L						; DoNewMIDI_1
; load NewMIDI_byte and adjust pointer.				; DoNewMIDI_1
;  Byte placed in InQueue by USART_Rx1 IRQ.			; DoNewMIDI_1
	ldi	ZL, low  (InQ_One)				; DoNewMIDI_1
 	ldi	ZH, high (InQ_One)				; DoNewMIDI_1
	lds	temp, InQ_1_OutPtr ; tail of Rx1 queue		; DoNewMIDI_1
	add	ZL, temp					; DoNewMIDI_1
	adc	ZH, ZeroReg					; DoNewMIDI_1
	ld	temp2, Z					; DoNewMIDI_1
	sts	NewMIDI1byte, temp2				; DoNewMIDI_1
	inc	temp						; DoNewMIDI_1
	andi	temp, INQMASK					; DoNewMIDI_1
	sts	InQ_1_OutPtr, temp				; DoNewMIDI_1
;								; DoNewMIDI_1
;;-----------------------					; DoNewMIDI_1    
;;  transmit single byte to MIDIOUT [USART #0]			; DoNewMIDI_1    
;	ldi	YH, high (OutQueue)				; DoNewMIDI_1    
;	ldi	YL, low  (OutQueue)				; DoNewMIDI_1    
;	lds	temp3, OutQ_InPtr				; DoNewMIDI_1    
;	add	YL, temp3					; DoNewMIDI_1    
;	adc	YH, ZeroReg					; DoNewMIDI_1    
;	inc	temp3						; DoNewMIDI_1    
;	andi	temp3, OUTQMASK					; DoNewMIDI_1    
;	sts	OutQ_InPtr, temp3				; DoNewMIDI_1    
;	st	y, temp2					; DoNewMIDI_1    
;;								; DoNewMIDI_1    
;; transmit butes in OutQ by enabling UDRE0 interrupt		; DoNewMIDI_1    
;	in	temp, UCSR0B	   	; 0x25			; DoNewMIDI_1    
;	ori	temp, (1 << UDRIE0)	; bit 5			; DoNewMIDI_1    
;	out	UCSR0B, temp					; DoNewMIDI_1    
;;								; DoNewMIDI_1  
;-----------------------					; DoNewMIDI_1    
;	inc	temp3						; DoNewMIDI_1
;	andi	temp3, OUTQMASK					; DoNewMIDI_1
;	sts	OutQ_InPtr, temp3				; DoNewMIDI_1
;;								; DoNewMIDI_1
;; transmit new MIDI message by enabling UDRE interrupt		; DoNewMIDI_1
;	in	temp, UCSR0B	   	; 0x25			; DoNewMIDI_1
;	ori	temp, (1 << UDRIE0)	; bit 5			; DoNewMIDI_1
;	out	UCSR0B, temp					; DoNewMIDI_1
;								; DoNewMIDI_1
;;==============================================================; Do_parse1
;			 					; Do_parse1
; New MIDI stored. Now parse new MIDI-In byte 			; Do_parse1
	rcall	ParseMIDI_One					; Do_parse1
;								; Do_parse1
	lds	temp, MsgFlags1					; Do_parse1
	sbrc	temp, isError					; Do_parse1
	rjmp	DNM1_MPerror					; Do_parse1
;								; Do_parse1
	sbrs	temp, isMsgRdy					; Do_parse1
	rjmp	DNM1_exitparse					; Do_parse1
;								; Do_parse1
	lds	ZH, BuffPtr1H					; Do_parse1
	lds	ZL, BuffPtr1L					; Do_parse1
	lds	temp2, MsgLen1					; Do_parse1
;								; Do_parse1
DNM1_c:								; Do_parse1
  	ld 	temp, z+					; Do_parse1
;								; Do_parse1
DNM1_d:								; Do_parse1
	ldi	YH, high (OutQueue)				; Do_parse1
	ldi	YL, low  (OutQueue)				; Do_parse1
	lds	temp3, OutQ_InPtr				; Do_parse1
	add	YL, temp3					; Do_parse1
	adc	YH, ZeroReg					; Do_parse1
	inc	temp3						; Do_parse1
	andi	temp3, OUTQMASK					; Do_parse1
	sts	OutQ_InPtr, temp3				; Do_parse1
	st	y, temp						; Do_parse1
;								; Do_parse1
DNM1_4: 							; Do_parse1
	dec	temp2						; Do_parse1
	brne	DNM1_c						; Do_parse1
;								; Do_parse1
; transmit new MIDI message by enabling UDRE interrupt		; Do_parse1
	in	temp, UCSR0B	   	; 0x25	 		; Do_parse1
	ori	temp, (1 << UDRIE1)	; bit 5			; Do_parse1
	out	UCSR0B, temp					; Do_parse1
;								; Do_parse1
	clr	temp						; Do_parse1
	sts	MsgLen1, temp					; Do_parse1
	sts	MsgFlags1, temp					; Do_parse1
								; Do_parse1
DNM1_exitparse:							; Do_parse1
;	rjmp	DMN1_exit 					; Do_parse1
;								; Do_parse1
;===========================					; DoNewMIDI_1
DMN1_exit:							; DoNewMIDI_1 
	pop	XL						; DoNewMIDI_1 
	pop	XH						; DoNewMIDI_1 
	pop	YL						; DoNewMIDI_1 
	pop	YH						; DoNewMIDI_1 
	pop	ZL						; DoNewMIDI_1 
	pop	ZH						; DoNewMIDI_1 
	pop	temp						; DoNewMIDI_1 
	ret							; DoNewMIDI_1 
;								; DoNewMIDI_1 
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; Do_parse1
DNM1_MPerror:							; Do_parse1
	rjmp	DMN1_exit 					; Do_parse1
;								; Do_parse1
;;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;%%%%%%%%%%




;***************************************************************;
;  Heart_Beat      Branch to here 32 times a second. 		;
;    All this does presently is toggle LED on PCB.		;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;
HeartBeat:							; Heart_Beat
	push	temp						; Heart_Beat
	push	temp2						; Heart_Beat
	push	ZH        					; Heart_Beat
	push	ZL          					; Heart_Beat
	             						; Heart_Beat
	cbr	TaskFlags, ( 1 << T1_HeartBeat )		; Heart_Beat
								; Heart_Beat
	lds	temp, SecondCount	; incremented by 	; Heart_Beat
	lds	temp2, PrevSecondCount	; the T1_overflow IRQ	; Heart_Beat
	cp	temp, temp2					; Heart_Beat
	breq	HBP_exit					; Heart_Beat
	sts	PrevSecondCount, temp				; Heart_Beat
								; Heart_Beat
	ldi	temp, ( 1 << LED_B)	; toggle port C1 	; Heart_Beat
	mov	ToggleReg, temp		; left red LED		; Heart_Beat
	in 	temp, LED_PORT					; Heart_Beat
	eor	temp, ToggleReg					; Heart_Beat
	out	LED_PORT, temp					; Heart_Beat
								; Heart_Beat
								; Heart_Beat
HBP_exit:							; Heart_Beat
	pop	ZL						; Heart_Beat
	pop	ZH						; Heart_Beat
	pop	temp2						; Heart_Beat
	pop	temp						; Heart_Beat
	ret							; Heart_Beat
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%; Heart_Beat




;###############################################################;<> Interrupt_Routines
;								;<> Interrupt_Routines
;  Interrupts							;<> Interrupt_Routines
;								;<> Interrupt_Routines
;###############################################################;<> Interrupt_Routines
								;<> Interrupt_Routines
;***************************************************************;<> Interrupt_Routines
; USART0_Rx  -USART Zero_RECEIVE  Ignores ActiveSense bytes	;<> Interrupt_Routines
;                   						;<> Interrupt_Routines
; send:	   {SRAM} InQ__Zero, InQ_0__InPtr,  INQMASK		;<> Interrupt_Routines
; returns: {SRAM adjusted}  InQ__Zero, InQ_0__InPtr		;<> Interrupt_Routines
;  					@36 instructions	;<> Interrupt_Routines
;***************************************************************;<> Interrupt_Routines
USART0_Rx:							;<> Interrupt_Routines
	in	SREGtemp, SREG	; 0x3f				;<> Interrupt_Routines
	push	XH						;<> Interrupt_Routines
	push	XL						;<> Interrupt_Routines
								;<> Interrupt_Routines
	in	irqtemp, UDR0	; 0x20				;<> Interrupt_Routines
	cpi	irqtemp, 0xfe  ; ignore ActiveSense 0xfe	;<> Interrupt_Routines
	breq	USART0_Rx_Exit					;<> Interrupt_Routines
								;<> Interrupt_Routines
	push	YL						;<> Interrupt_Routines
	ldi	XH, high (InQ_Zero)				;<> Interrupt_Routines
	ldi	XL, low  (InQ_Zero)				;<> Interrupt_Routines
	lds	YL, InQ_0_InPtr					;<> Interrupt_Routines
	add	XL, YL						;<> Interrupt_Routines
	adc	XH, ZeroReg					;<> Interrupt_Routines
	inc	YL						;<> Interrupt_Routines
	andi	YL, INQMASK					;<> Interrupt_Routines
	sts	InQ_0_InPtr, YL					;<> Interrupt_Routines
	st	X, irqtemp  					;<> Interrupt_Routines
	pop	YL						;<> Interrupt_Routines
               							;<> Interrupt_Routines
USART0_Rx_Exit:							;<> Interrupt_Routines
	pop	XL						;<> Interrupt_Routines
	pop	XH						;<> Interrupt_Routines
	out	SREG, SREGtemp	; 0x3f				;<> Interrupt_Routines
	reti							;<> Interrupt_Routines
                                          			;<> Interrupt_Routines
								;<> Interrupt_Routines
;***************************************************************;<> Interrupt_Routines
; USART1_Rx  -USART_RECEIVE	Ignores ActiveSense bytes	;<> Interrupt_Routines
;                   						;<> Interrupt_Routines
; send:	   {SRAM} InQ_One, InQ_1_InPtr,  INQMASK		;<> Interrupt_Routines
; returns: {SRAM adjusted} InQ_One, InQ_1_InPtr			;<> Interrupt_Routines
;  					@36 instructions	;<> Interrupt_Routines
;***************************************************************;<> Interrupt_Routines
USART1_Rx:							;<> Interrupt_Routines
	in	SREGtemp, SREG		; 0x3f			;<> Interrupt_Routines
	push	XH						;<> Interrupt_Routines
	push	XL						;<> Interrupt_Routines
								;<> Interrupt_Routines
	lds	irqtemp, UDR1		; 0x73			;<> Interrupt_Routines
	cpi	irqtemp, 0xfe  ; ignore ActiveSense 		;<> Interrupt_Routines
	breq	USART1_Rx_Exit			 		;<> Interrupt_Routines
								;<> Interrupt_Routines
	push	YL						;<> Interrupt_Routines
	ldi	XH, high (InQ_One)				;<> Interrupt_Routines
	ldi	XL, low  (InQ_One)				;<> Interrupt_Routines
	lds	YL, InQ_1_InPtr					;<> Interrupt_Routines
	add	XL, YL						;<> Interrupt_Routines
	adc	XH, ZeroReg					;<> Interrupt_Routines
	inc	YL						;<> Interrupt_Routines
	andi	YL, INQMASK					;<> Interrupt_Routines
	sts	InQ_1_InPtr, YL					;<> Interrupt_Routines
	st	X, irqtemp  					;<> Interrupt_Routines
	pop	YL						;<> Interrupt_Routines
               							;<> Interrupt_Routines
USART1_Rx_Exit:							;<> Interrupt_Routines
	pop	XL						;<> Interrupt_Routines
	pop	XH						;<> Interrupt_Routines
	out	SREG, SREGtemp	 ; 0x3f				;<> Interrupt_Routines
	reti							;<> Interrupt_Routines
                                          			;<> Interrupt_Routines
								;<> Interrupt_Routines
;***************************************************************;<> Interrupt_Routines
;  Timer 1 overflow   set to activate every 50 milliseconds	;<> Interrupt_Routines
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;<> Interrupt_Routines
								;<> Interrupt_Routines
TIM1_OVF:							;<> Interrupt_Routines
	in	SREGtemp, SREG	; 0x3f				;<> Interrupt_Routines
								;<> Interrupt_Routines
	ldi	irqtemp, high (~LongPeriod)			;<> Interrupt_Routines
	sts	HEARTBEAT_COUNT_HIGH, irqtemp  ;TCNT1H	0x6f	;<> Interrupt_Routines
	ldi	irqtemp, low  (~LongPeriod)			;<> Interrupt_Routines
	sts	HEARTBEAT_COUNT_LOW,  irqtemp  ;TCNT1L	0x6e	;<> Interrupt_Routines
								;<> Interrupt_Routines
	sbr	TaskFlags, (1 << T1_HeartBeat)			;<> Interrupt_Routines
								;<> Interrupt_Routines
	lds	irqtemp, HeartBeatCntr				;<> Interrupt_Routines
	inc	irqtemp						;<> Interrupt_Routines
	sts	HeartBeatCntr, irqtemp				;<> Interrupt_Routines
								;<> Interrupt_Routines
 	andi	irqtemp, HB_PER_SECOND - 1			;<> Interrupt_Routines
	brne	T1ovf_exit					;<> Interrupt_Routines
								;<> Interrupt_Routines
	lds	irqtemp, SecondCount				;<> Interrupt_Routines
	inc	irqtemp						;<> Interrupt_Routines
	sts	SecondCount, irqtemp				;<> Interrupt_Routines
								;<> Interrupt_Routines
T1ovf_exit:							;<> Interrupt_Routines
	out	SREG, SREGtemp		;0x3f			;<> Interrupt_Routines
 	reti 							;<> Interrupt_Routines
								;<> Interrupt_Routines
								;<> Interrupt_Routines
;***************************************************************;<> Interrupt_Routines
; UDRE_IRQ:  Takes a data byte out of the Transmit queue	;<> Interrupt_Routines
;	and puts it into the UDR.				;<> Interrupt_Routines
; 								;<> Interrupt_Routines
;  This interrupt routine is the second half of the code needed	;<> Interrupt_Routines
;  to have a non-polling output UART.	Interrupt-based UART	;<> Interrupt_Routines
;  allows the main code to avoid polling the Data_Ready bit	;<> Interrupt_Routines
;  (UDRE) in order to know when the UART is ready to send	;<> Interrupt_Routines
;  another byte.						;<> Interrupt_Routines
;								;<> Interrupt_Routines
;  Set-up:  Put the byte that is to be sent into the		;<> Interrupt_Routines
;           output queue, then enable the UDRE interrupt.	;<> Interrupt_Routines
;  The UDRE interrupt will immediately happen, unless the	;<> Interrupt_Routines
;  UART is currently transmitting a character sequence		;<> Interrupt_Routines
;  and is not empty.						;<> Interrupt_Routines
;    Out pointer ==  bytes taken out of the queue. "tail"	;<> Interrupt_Routines
;    In pointer  ==  bytes put into the queue. "head"		;<> Interrupt_Routines
;  This code assumes that when the UDRE interrupt happens,	;<> Interrupt_Routines
;  the OutQ_InPtr will be greater than the OutQ_OutPtr.		;<> Interrupt_Routines
;								;<> Interrupt_Routines
;        Out=in	 {queue state after reset initialization}	;<> Interrupt_Routines
; UDRE off  |							;<> Interrupt_Routines
;  OUT_Q:  00  01  02  03  04  05  06  07  08  09  0a  0b	;<> Interrupt_Routines
;  ^^^ state of OutQueue when main code				;<> Interrupt_Routines
;      wants to use the USART to transmit a byte.		;<> Interrupt_Routines
;								;<> Interrupt_Routines
;  Put b1 into SRAM OutQueue at In ptr, then increment In ptr	;<> Interrupt_Routines
;	   Out  In 		 (to 01) and enable UDREirq.	;<> Interrupt_Routines
; UDREon    |   |						;<> Interrupt_Routines
;  OUT_Q:  b1  01  02  03  04  05  06  07  08  09  0a  0b	;<> Interrupt_Routines
;  ^^^ state of OutQueue when UDRE_IRQ is entered.		;<> Interrupt_Routines
;								;<> Interrupt_Routines
;								;<> Interrupt_Routines
; Usually interrupt routines are enabled by the			;<> Interrupt_Routines
; initialization code, but then remain inactive			;<> Interrupt_Routines
; until triggered. But the Data_Register_Empty			;<> Interrupt_Routines
; IRQ starts as soon as it is enabled and runs over		;<> Interrupt_Routines
; and over again until it is specifically disabled.		;<> Interrupt_Routines
;								;<> Interrupt_Routines
;   When enabled, UDRE_IRQ gets the byte pointed		;<> Interrupt_Routines
; to by the queue Out ptr and transmits it out of		;<> Interrupt_Routines
; the USART by putting it into the UDR.  Next it		;<> Interrupt_Routines
; increments the Out pointer to the next location		;<> Interrupt_Routines
; in the queue.  Then it checks if the adjusted Out		;<> Interrupt_Routines
; pointer equals the In pointer.  If yes, then there		;<> Interrupt_Routines
; are no more new bytes from the main program to send		;<> Interrupt_Routines
; to USART OUT, so the USRE interrupt disables itself		;<> Interrupt_Routines
; by turning off the interrupt enable (clearing			;<> Interrupt_Routines
; UDRIE bit in USCRB control register).				;<> Interrupt_Routines
;								;<> Interrupt_Routines
;    If the Out pointer is still behind the In ptr,		;<> Interrupt_Routines
; then the USRE interrupt is not disabled and will		;<> Interrupt_Routines
; trigger again as soon as the current byte in the		;<> Interrupt_Routines
; USART shift register has finished being transmitted.		;<> Interrupt_Routines
;								;<> Interrupt_Routines
;  At that time the USRE flag is set, and (after		;<> Interrupt_Routines
; one main code instruction) this UDRE interrupt		;<> Interrupt_Routines
; starts transmitting the next data byte that is		;<> Interrupt_Routines
; pointed to by the OUT pointer. The UDRE interrupt		;<> Interrupt_Routines
; will keep sending data like this until the Out ptr		;<> Interrupt_Routines
; equals the In ptr.						;<> Interrupt_Routines
;								;<> Interrupt_Routines
; External equates:	OUTQMASK	{OUTQUEUE_SIZE - 1}	;<> Interrupt_Routines
; Peripheral registers:	UDR, UCSRB				;<> Interrupt_Routines
; low registers:	SREGtemp, ZeroReg (value always zero)	;<> Interrupt_Routines
; high registers:	irqtemp, ZH:ZL (perserved)		;<> Interrupt_Routines
; SRAM:			OutQueue, OutQ_OutPtr,OutQ_InPtr	;<> Interrupt_Routines
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;<> Interrupt_Routines
UDRE_IRQ:							;<> Interrupt_Routines
	in	SREGtemp, SREG	; 0x3f save the CPU flags  	;<> Interrupt_Routines
	push	ZH			  			;<> Interrupt_Routines
	push	ZL			  			;<> Interrupt_Routines
								;<> Interrupt_Routines
	ldi	ZH, high (OutQueue)				;<> Interrupt_Routines
	ldi	ZL, low  (OutQueue)				;<> Interrupt_Routines
	lds	irqtemp, OutQ_OutPtr				;<> Interrupt_Routines
	add	ZL, irqtemp		 			;<> Interrupt_Routines
	adc	ZH, ZeroReg					;<> Interrupt_Routines
	ld	irqtemp, Z					;<> Interrupt_Routines
	out	UDR0, irqtemp	; 0x20				;<> Interrupt_Routines
								;<> Interrupt_Routines
	lds	irqtemp, OutQ_OutPtr	 			;<> Interrupt_Routines
	inc	irqtemp						;<> Interrupt_Routines
	andi	irqtemp, OUTQMASK	  			;<> Interrupt_Routines
	sts	OutQ_OutPtr, irqtemp				;<> Interrupt_Routines
								;<> Interrupt_Routines
	lds	ZH, OutQ_InPtr					;<> Interrupt_Routines
	lds	ZL, OutQ_OutPtr					;<> Interrupt_Routines
	cpse	ZH, ZL			 			;<> Interrupt_Routines
	rjmp	U2M_exit		 			;<> Interrupt_Routines
					 			;<> Interrupt_Routines
	in	irqtemp, UCSR0B	   ; 0x25	 		;<> Interrupt_Routines
	andi	irqtemp, ~ (1 << UDRIE0) ; bit 5		;<> Interrupt_Routines
	out	UCSR0B, irqtemp	   ; 0x25	 		;<> Interrupt_Routines
								;<> Interrupt_Routines
U2M_exit:							;<> Interrupt_Routines
	pop	ZL						;<> Interrupt_Routines
	pop	ZH						;<> Interrupt_Routines
	out	SREG, SREGtemp     ; 0x3f			;<> Interrupt_Routines
	reti							;<> Interrupt_Routines
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_Zero
; Parse_MIDI0     Called by DoNewMIDIin whenever the main loop detects that the	;Parse_MIDI_Zero
;    InQ_0's IN pointer is not the same as the OUT pointer.  It is sent the	;Parse_MIDI_Zero
;    value pointed to by the InQ_0's OUT pointer.  This is the new MIDI byte	;Parse_MIDI_Zero
;    that has just arrived.							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
;    Booleans:	isMsgRdy	checked by caller on return, always bit 7	;Parse_MIDI_Zero
; in MsgFlags	isError		checked by caller on return			;Parse_MIDI_Zero
;		isRunStatus	volatile, keeps value between calls		;Parse_MIDI_Zero
;		isSysEx		volatile, keeps value between calls		;Parse_MIDI_Zero
;		isTwoByte	volatile, keeps value between calls		;Parse_MIDI_Zero
;		isThreeByte	volatile, keeps value between calls		;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
;	SRAM:	MsgBuff0[]	32 bytes					;Parse_MIDI_Zero
;		RunCmd0		 1 byte 					;Parse_MIDI_Zero
;		MsgLen0		 1 byte 					;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_Zero
ParseMIDI_Zero:									;Parse_MIDI_Zero
	push	temp								;Parse_MIDI_Zero
	push	temp2								;Parse_MIDI_Zero
	push	ZL								;Parse_MIDI_Zero
	push	ZH		; pops at MP_exit line 3597			;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	lds	temp, MsgFlags0	; clear these two flags before each parse	;Parse_MIDI_Zero
	andi	temp, ~ ( (1<<isMsgRdy) | (1<<isError) ) 			;Parse_MIDI_Zero
	sts	MsgFlags0, temp							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_Zero
; Command or Data new MIDI byte?						;Parse_MIDI_Zero
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte						;Parse_MIDI_Zero
	tst	temp								;Parse_MIDI_Zero
	brpl	zMP0								;Parse_MIDI_Zero
	rjmp	zMP_doCmd							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_Zero
; New Data MIDI byte 								;Parse_MIDI_Zero
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_Zero
;   Labels start with MP0_.  This data byte could be part of a			;Parse_MIDI_Zero
; running-status message. If a command byte had been sent before		;Parse_MIDI_Zero
; this data byte, it's a standard MIDI message.					;Parse_MIDI_Zero
; If MsgLen is zero, then check if a previous command has			;Parse_MIDI_Zero
; established a	Running-Status command byte. If there is no			;Parse_MIDI_Zero
; previous command byte in the Running-Status-command variable			;Parse_MIDI_Zero
; (in .dseg static RAM as RunCmd), then possibly this device			;Parse_MIDI_Zero
; came out of RESET into a MIDI data steam.  This byte just			;Parse_MIDI_Zero
; received could be a data byte from a standard message.			;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP0:									;*e	;Parse_MIDI_Zero
; first test if there has been a valid message command byte			;Parse_MIDI_Zero
; received after the last completed MIDI message. MsgLen=1 if yes.		;Parse_MIDI_Zero
	lds	temp, MsgLen0  							;Parse_MIDI_Zero
	tst	temp								;Parse_MIDI_Zero
	breq	zMP0_f								;Parse_MIDI_Zero
	rjmp	zMP0_j								;Parse_MIDI_Zero
										;Parse_MIDI_Zero
;  test whether NMB data byte has arrived before ANY command byte		;Parse_MIDI_Zero
zMP0_f:	;MsgLen=0, NMB is data byte					;*f	;Parse_MIDI_Zero
	lds	temp, RunCmd0							;Parse_MIDI_Zero
	tst	temp								;Parse_MIDI_Zero
	breq	zMP0_f1								;Parse_MIDI_Zero
	rjmp	zMP0_g								;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP0_f1:	; **Error  NMB data byte has arrived before ANY command byte	;Parse_MIDI_Zero
	lds	temp, MsgFlags0							;Parse_MIDI_Zero
	sbr	temp, (1 << isError)						;Parse_MIDI_Zero
	sts	MsgFlags0, temp							;Parse_MIDI_Zero
	rjmp	MP_exit								;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_Zero
;  Running-Status message handler						;Parse_MIDI_Zero
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_Zero
; NMB is a data byte; MsgLen=0; RunCmd != 0x00.  A previous Cmd byte received.	;Parse_MIDI_Zero
; This is a Running-Status msg.							;Parse_MIDI_Zero
; 1> store NMB into MsgBuff[MsgLen].						;Parse_MIDI_Zero
; 2> Increment MsgLen. 								;Parse_MIDI_Zero
; 3> Determine if 1st or 2nd data byte of a Running-Status msg.			;Parse_MIDI_Zero
; 4a> If this is a rare 1-byte RS message (RunCmd==0xCn or 0xDn)		;Parse_MIDI_Zero
;	-load the RunCmd position into pointer BuffPtrH:L			;Parse_MIDI_Zero
;	-increment MsgLen to value 2.						;Parse_MIDI_Zero
;	-set isMsgRdy flag in MsgFlags.						;Parse_MIDI_Zero
;	-return to main code.							;Parse_MIDI_Zero
;  Main code will send two consecutive bytes starting at RunCmd.		;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
; 4b> If this is normal 2-byte RS message (RunCmd !=0xCn or 0xDn)		;Parse_MIDI_Zero
;	-load the address of RunCmd into pointer BuffPtrH:L			;Parse_MIDI_Zero
;	-increment MsgLen to value 3.						;Parse_MIDI_Zero
;	-set isMsgRdy flag in MsgFlags.						;Parse_MIDI_Zero
;	-return to main code.							;Parse_MIDI_Zero
;  Main code will send three consecutive bytes starting at RunCmd.		;Parse_MIDI_Zero
;  The message now includes the command byte missing from RunningStatus msg.	;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
; * some MIDI controller keyboards use Run-Status msgs when chords are		;Parse_MIDI_Zero
;   played, like the Roland PC150.						;Parse_MIDI_Zero
;   RunCmd is almost always 0x9n, 0xBn, or 0xEn, rarely 0xCn or 0xDn		;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP0_g:	; MsgLen=0, RunCmd != 0x00.  NMB is first byte of RunStat msg	;*g	;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte						;Parse_MIDI_Zero
	ldi	ZH, high (MsgBuff0)						;Parse_MIDI_Zero
	ldi	ZL, low  (MsgBuff0)						;Parse_MIDI_Zero
	st	z, temp			; NMB stored at MsgBuff[0]		;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	ldi	temp, 1								;Parse_MIDI_Zero
	sts	MsgLen0, temp							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	lds	temp2, RunCmd0  						;Parse_MIDI_Zero
	andi	temp2, 0xf0	; separate cmd from channel			;Parse_MIDI_Zero
	cpi	temp2, 0xc0	; Program change				;Parse_MIDI_Zero
	breq	zMP0_g1								;Parse_MIDI_Zero
	cpi	temp2, 0xd0	; Channel Aftertouch				;Parse_MIDI_Zero
	breq	zMP0_g1								;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
; RunStat msg, NMB is 1st data byte					;*i	;Parse_MIDI_Zero
	clr	temp								;Parse_MIDI_Zero
 	ori	temp, (1 << isThreeByte) | (1 << isRunStatus)			;Parse_MIDI_Zero
	sts	MsgFlags0, temp							;Parse_MIDI_Zero
	rjmp	zMP_exit							;Parse_MIDI_Zero
;------------------------------------						;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP0_g1:	; rare RunningStatus msg with only one data byte 	;*h	;Parse_MIDI_Zero
	ldi	temp, high (RunCmd0)						;Parse_MIDI_Zero
	sts	BuffPtr0H, temp							;Parse_MIDI_Zero
	ldi	temp, low  (RunCmd0)						;Parse_MIDI_Zero
	sts	BuffPtr0L, temp							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	ldi	temp, (1 << isMsgRdy)						;Parse_MIDI_Zero
	sts	MsgFlags0, temp							;Parse_MIDI_Zero
	rjmp	zMP_exit							;Parse_MIDI_Zero
;------------------------------------						;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP0_j: ; new data byte, MsgLen != 0, called by MP0. Determine msg type	;*j	;Parse_MIDI_Zero
	lds	temp, MsgFlags0							;Parse_MIDI_Zero
	sbrc	temp, isSysEx							;Parse_MIDI_Zero
	rjmp	zMP0_k								;Parse_MIDI_Zero
										;Parse_MIDI_Zero
	sbrc	temp, isTwoByte 						;Parse_MIDI_Zero
	rjmp	zMP0_l								;Parse_MIDI_Zero
										;Parse_MIDI_Zero
	sbrc	temp, isThreeByte						;Parse_MIDI_Zero
	rjmp	zMP0_m								;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
;;  ######### DEBUG 	NOTE-ON gets here? no					;Parse_MIDI_Zero
;	cbi	LED_PORT, LED_DEBUG 	;turn on LED 1/32 sec			;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	rjmp	zMP_exit ; should never get here				;Parse_MIDI_Zero
;-----------------------							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP0_k:		; new SysEx data byte						;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte	; store NMB into MsgBuff	;*k	;Parse_MIDI_Zero
	ldi	ZH, high (MsgBuff0)	; MsgBuff in SRAM			;Parse_MIDI_Zero
	ldi	ZL, low  (MsgBuff0)	; MsgBuff in SRAM			;Parse_MIDI_Zero
	lds	temp2, MsgLen0							;Parse_MIDI_Zero
	add	ZL, temp2  							;Parse_MIDI_Zero
	adc	ZH, ZeroReg 							;Parse_MIDI_Zero
	st	z, temp 							;Parse_MIDI_Zero
	inc	temp2 								;Parse_MIDI_Zero
	sts	MsgLen0, temp2							;Parse_MIDI_Zero
	rjmp	zMP_exit							;Parse_MIDI_Zero
;-----------------------							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP0_l:		; data byte of a two byte message (0xc0 or 0xd0)	;*l	;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
;;  ######### DEBUG 	NOTE-ON gets here? no					;Parse_MIDI_Zero
;	cbi	LED_PORT, LED_DEBUG 	;turn on LED 1/32 sec			;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte	; store NMB into MsgBuff		;Parse_MIDI_Zero
	ldi	ZH, high (MsgBuff0 + 1 )	; MsgBuff in SRAM		;Parse_MIDI_Zero
	ldi	ZL, low  (MsgBuff0 + 1 )	; MsgBuff in SRAM		;Parse_MIDI_Zero
	st	z, temp								;Parse_MIDI_Zero
	ldi	temp, 2								;Parse_MIDI_Zero
	sts	MsgLen0, temp 							;Parse_MIDI_Zero
	ldi	temp, high (MsgBuff0)						;Parse_MIDI_Zero
	sts	BuffPtr0H, temp							;Parse_MIDI_Zero
	ldi	temp, low  (MsgBuff0)						;Parse_MIDI_Zero
	sts	BuffPtr0L, temp							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
 	ldi	temp, (1 << isMsgRdy)						;Parse_MIDI_Zero
 	sts	MsgFlags0, temp							;Parse_MIDI_Zero
	rjmp	zMP_exit 							;Parse_MIDI_Zero
;-----------------------							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP0_m:	; MsgLen !=0, isThreeByte set					;*m	;Parse_MIDI_Zero
	lds	temp, MsgLen0 							;Parse_MIDI_Zero
	cpi	temp, 1		; is first data byte of 3-byte command msg?	;Parse_MIDI_Zero
	brne	zMP0_m1		; or second data byte of RunStat msg?		;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	lds	temp, MsgFlags0	; MsgLen=1, first data byte new msg	;*n	;Parse_MIDI_Zero
	sbrc	temp, isRunStatus						;Parse_MIDI_Zero
	rjmp	zMP0_m2	; jump if isRunStatus set				;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte	; 1st data of Normal command 	;*o	;Parse_MIDI_Zero
	ldi	ZH, high (MsgBuff0)	; MsgBuff in SRAM			;Parse_MIDI_Zero
	ldi	ZL, low  (MsgBuff0)	; MsgBuff in SRAM			;Parse_MIDI_Zero
	lds	temp2, MsgLen0							;Parse_MIDI_Zero
	add	ZL, temp2							;Parse_MIDI_Zero
	adc	ZH, ZeroReg							;Parse_MIDI_Zero
	st	z, temp								;Parse_MIDI_Zero
	ldi	temp2, 2							;Parse_MIDI_Zero
	sts	MsgLen0, temp2							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	rjmp	zMP_exit							;Parse_MIDI_Zero
;-----------------------							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP0_m1:									;Parse_MIDI_Zero
	cpi	temp, 2							;*m	;Parse_MIDI_Zero
	breq	zMP0_m1a							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	rjmp	zMP_exit			; should never get here		;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP0_m1a:	; last data byte of normal 3-byte message, not RunStat	;*p	;Parse_MIDI_Zero
	ldi	ZH, high (MsgBuff0 + 2)	; MsgBuff in SRAM			;Parse_MIDI_Zero
	ldi	ZL, low  (MsgBuff0 + 2)	; MsgBuff in SRAM			;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte 						;Parse_MIDI_Zero
	st	z, temp								;Parse_MIDI_Zero
	ldi	temp, 3								;Parse_MIDI_Zero
	sts	MsgLen0, temp 							;Parse_MIDI_Zero
	ldi	temp, high (MsgBuff0)						;Parse_MIDI_Zero
	sts	BuffPtr0H, temp							;Parse_MIDI_Zero
	ldi	temp, low  (MsgBuff0)						;Parse_MIDI_Zero
	sts	BuffPtr0L, temp							;Parse_MIDI_Zero
	ldi	temp, (1 << isMsgRdy)						;Parse_MIDI_Zero
	sts	MsgFlags0, temp							;Parse_MIDI_Zero
	rjmp	zMP_exit	 						;Parse_MIDI_Zero
;------------------------							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP0_m2:	; isRunStatus = TRUE, MsgLen = 1			;*q	;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte	; store NMB into MsgBuff		;Parse_MIDI_Zero
	ldi	ZH, high (MsgBuff0 + 1)	; MsgBuff in SRAM			;Parse_MIDI_Zero
	ldi	ZL, low  (MsgBuff0 + 1)	; MsgBuff in SRAM			;Parse_MIDI_Zero
	st	z, temp 							;Parse_MIDI_Zero
	ldi	temp, 3								;Parse_MIDI_Zero
	sts	MsgLen0, temp							;Parse_MIDI_Zero
	ldi	temp, high (RunCmd0)						;Parse_MIDI_Zero
	sts	BuffPtr0H, temp							;Parse_MIDI_Zero
	ldi	temp, low  (RunCmd0)						;Parse_MIDI_Zero
	sts	BuffPtr0L, temp							;Parse_MIDI_Zero
	ldi	temp, (1 << isMsgRdy)	; only one flag is set			;Parse_MIDI_Zero
	sts	MsgFlags0, temp							;Parse_MIDI_Zero
	rjmp	zMP_exit							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_Zero
; When the New MIDI byte has the bit 7 set, it is a MIDI command. 		;Parse_MIDI_Zero
;  This section is the command parser.						;Parse_MIDI_Zero
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_Zero
zMP_doCmd:								;*a	;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte						;Parse_MIDI_Zero
	andi	temp, 0xf0							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
; is it a MIDI System command?  0xf0-0xff?				;*c	;Parse_MIDI_Zero
	cpi	temp, 0xf0							;Parse_MIDI_Zero
	breq	zMP1_a0								;Parse_MIDI_Zero
	rjmp	zMP1_b								;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP1_a0:									;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte						;Parse_MIDI_Zero
	cpi	temp, 0xf0		; is it start of sysex?			;Parse_MIDI_Zero
	brne	zMP1_a1	 							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	ldi	ZH, high (MsgBuff0)	;  Yes, 0xF0 start-of-sysex		;Parse_MIDI_Zero
	ldi	ZL, low  (MsgBuff0)						;Parse_MIDI_Zero
	st	z, temp			; MsgBuff is in SRAM			;Parse_MIDI_Zero
  	ldi	temp, 1								;Parse_MIDI_Zero
  	sts	MsgLen0, temp							;Parse_MIDI_Zero
	ldi	temp, (1 << isSysEx)						;Parse_MIDI_Zero
	sts	MsgFlags0, temp							;Parse_MIDI_Zero
	rjmp	zMP_exit							;Parse_MIDI_Zero
; -----------------------------------						;Parse_MIDI_Zero
										;Parse_MIDI_Zero
zMP1_a1:									;Parse_MIDI_Zero
	cpi	temp, 0xf7							;Parse_MIDI_Zero
	brne	zMP1_a2	 							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	ldi	ZH, high (MsgBuff0)	; 0xF7 End-of-sysex			;Parse_MIDI_Zero
	sts	BuffPtr0H, ZH							;Parse_MIDI_Zero
	ldi	ZL, low  (MsgBuff0)						;Parse_MIDI_Zero
	sts	BuffPtr0L, ZL		; ZH:ZL <<  startaddr of MsgBuff	;Parse_MIDI_Zero
	lds	temp2, MsgLen0		; MsgLen doesn't include 0xf7		;Parse_MIDI_Zero
	add	ZL, temp2							;Parse_MIDI_Zero
	adc	ZH, ZeroReg							;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte	; 0xf7					;Parse_MIDI_Zero
	st	z, temp								;Parse_MIDI_Zero
	inc	temp2								;Parse_MIDI_Zero
	sts	MsgLen0, temp2							;Parse_MIDI_Zero
	ldi	temp, (1 << isMsgRdy)						;Parse_MIDI_Zero
	sts	MsgFlags0, temp							;Parse_MIDI_Zero
	rjmp	zMP_exit							;Parse_MIDI_Zero
; -----------------------------------						;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP1_a2:									;Parse_MIDI_Zero
	cpi	temp, 0xf2		; song start?				;Parse_MIDI_Zero
	brne	zMP1_a3	 							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	ldi	temp, (1 << isTwoByte)						;Parse_MIDI_Zero
	sts	MsgFlags0, temp							;Parse_MIDI_Zero
	ldi	ZH, high (MsgBuff0)	; 0xF2 song-start			;Parse_MIDI_Zero
	ldi	ZL, low  (MsgBuff0)						;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte						;Parse_MIDI_Zero
	st	z, temp			; MsgBuff is in SRAM			;Parse_MIDI_Zero
	ldi	temp, 1								;Parse_MIDI_Zero
	sts	MsgLen0, temp							;Parse_MIDI_Zero
	rjmp	zMP_exit							;Parse_MIDI_Zero
; -----------------------------------						;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP1_a3:									;Parse_MIDI_Zero
	cpi	temp, 0xf3		; song pointer				;Parse_MIDI_Zero
	brne	zMP1_a4	 							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	ldi	temp, (1 << isThreeByte)					;Parse_MIDI_Zero
	sts	MsgFlags0, temp							;Parse_MIDI_Zero
	ldi	ZH, high (MsgBuff0)	; 0xF3 song-pointer			;Parse_MIDI_Zero
	ldi	ZL, low  (MsgBuff0)						;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte						;Parse_MIDI_Zero
	st	z, temp			; MsgBuff is in SRAM			;Parse_MIDI_Zero
	ldi	temp, 1								;Parse_MIDI_Zero
	sts	MsgLen0, temp							;Parse_MIDI_Zero
	rjmp	zMP_exit							;Parse_MIDI_Zero
; -----------------------------------						;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP1_a4:	; not a valid system command (0xfx), don't send to MIDIout	;Parse_MIDI_Zero
	clr	temp		; reset the MsgLen variable for next MIDIin	;Parse_MIDI_Zero
	sts	MsgLen0, temp							;Parse_MIDI_Zero
	sts	MsgFlags0, temp	; clear all flags after 1-byte command 		;Parse_MIDI_Zero
	rjmp	zMP_exit							;Parse_MIDI_Zero
;------------------------------------						;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
zMP1_b:	cpi	temp, 0xc0	; check for 2-byte message		;*b	;Parse_MIDI_Zero
	breq	zMP1_b1								;Parse_MIDI_Zero
	cpi	temp, 0xd0							;Parse_MIDI_Zero
	breq	zMP1_b1								;Parse_MIDI_Zero
	rjmp	zMP1_c								;Parse_MIDI_Zero
										;Parse_MIDI_Zero
zMP1_b1: 									;Parse_MIDI_Zero
	ldi	temp, (1 << isTwoByte)	; yes, it's a two byter (1 cmd:1 data)	;Parse_MIDI_Zero
	sts	MsgFlags0, temp							;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte						;Parse_MIDI_Zero
	sts	RunCmd0, temp							;Parse_MIDI_Zero
	ldi	ZH, high (MsgBuff0)	; 0xc0 or 0xd0				;Parse_MIDI_Zero
	ldi	ZL, low  (MsgBuff0)						;Parse_MIDI_Zero
	st	z, temp			; MsgBuff is in SRAM			;Parse_MIDI_Zero
	ldi	temp, 1								;Parse_MIDI_Zero
	sts	MsgLen0, temp							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	rjmp	MP_exit								;Parse_MIDI_Zero
; -----------------------------------						;Parse_MIDI_Zero
										;Parse_MIDI_Zero
zMP1_c:		; 3-byte standard message, not RunningStatus			;Parse_MIDI_Zero
	ldi	temp, (1 << isThreeByte)				;*d	;Parse_MIDI_Zero
	sts	MsgFlags0, temp							;Parse_MIDI_Zero
	lds	temp, NewMIDI0byte						;Parse_MIDI_Zero
	sts	RunCmd0, temp							;Parse_MIDI_Zero
	ldi	ZH, high (MsgBuff0)	; 0xc0 or 0xd0				;Parse_MIDI_Zero
	ldi	ZL, low  (MsgBuff0)						;Parse_MIDI_Zero
	st	z, temp			; MsgBuff is in SRAM			;Parse_MIDI_Zero
	ldi	temp, 1								;Parse_MIDI_Zero
	sts	MsgLen0, temp							;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
	rjmp	zMP_exit							;Parse_MIDI_Zero
; -----------------------------------						;Parse_MIDI_Zero
										;Parse_MIDI_Zero
zMP_exit: ; whew! finally finished.  Return MsgBuff0[], MsgLen0, & MsgFlags0	;Parse_MIDI_Zero
	pop	ZH								;Parse_MIDI_Zero
	pop	ZL								;Parse_MIDI_Zero
	pop	temp2								;Parse_MIDI_Zero
	pop	temp								;Parse_MIDI_Zero
	ret									;Parse_MIDI_Zero
;										;Parse_MIDI_Zero
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_One
; Parse_MIDI_One    Called by DoNewMIDIin whenever the main loop detects that	;Parse_MIDI_One
;    the InQ_1's IN pointer is not the same as the OUT pointer.  It is sent the	;Parse_MIDI_One
;    value pointed to by the InQueue's OUT pointer.  This is the new MIDI byte	;Parse_MIDI_One
;    that has just arrived.							;Parse_MIDI_One
;										;Parse_MIDI_One
;    Booleans:	isMsgRdy	checked by caller on return, always bit 7	;Parse_MIDI_One
; in MsgFlags	isError		checked by caller on return			;Parse_MIDI_One
;		isRunStatus	volatile, keeps value between calls		;Parse_MIDI_One
;		isSysEx		volatile, keeps value between calls		;Parse_MIDI_One
;		isTwoByte	volatile, keeps value between calls		;Parse_MIDI_One
;		isThreeByte	volatile, keeps value between calls		;Parse_MIDI_One
;										;Parse_MIDI_One
;	SRAM:	MsgBuff1[]	32 bytes					;Parse_MIDI_One
;		RunCmd1		 1 byte 					;Parse_MIDI_One
;		MsgLen1		 1 byte 					;Parse_MIDI_One
;										;Parse_MIDI_One
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_One
ParseMIDI_One:									;Parse_MIDI_One
	push	temp								;Parse_MIDI_One
	push	temp2								;Parse_MIDI_One
	push	ZL								;Parse_MIDI_One
	push	ZH		; pops at MP_exit line 3597			;Parse_MIDI_One
;										;Parse_MIDI_One
	lds	temp, MsgFlags1	; clear these two flags before each parse	;Parse_MIDI_One
	andi	temp, ~ ( (1<<isMsgRdy) | (1<<isError) ) 			;Parse_MIDI_One
	sts	MsgFlags1, temp							;Parse_MIDI_One
;										;Parse_MIDI_One
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_One
; Command or Data new MIDI byte?						;Parse_MIDI_One
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_One
	lds	temp, NewMIDI1byte						;Parse_MIDI_One
	tst	temp								;Parse_MIDI_One
	brpl	MP0								;Parse_MIDI_One
	rjmp	MP_doCmd							;Parse_MIDI_One
;										;Parse_MIDI_One
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_One
; New Data MIDI byte 								;Parse_MIDI_One
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_One
;   Labels start with MP0_.  This data byte could be part of a			;Parse_MIDI_One
; running-status message. If a command byte had been sent before		;Parse_MIDI_One
; this data byte, it's a standard MIDI message.					;Parse_MIDI_One
; If MsgLen is zero, then check if a previous command has			;Parse_MIDI_One
; established a	Running-Status command byte. If there is no			;Parse_MIDI_One
; previous command byte in the Running-Status-command variable			;Parse_MIDI_One
; (in .dseg static RAM as RunCmd), then possibly this device			;Parse_MIDI_One
; came out of RESET into a MIDI data steam.  This byte just			;Parse_MIDI_One
; received could be a data byte from a standard message.			;Parse_MIDI_One
;										;Parse_MIDI_One
MP0:									;*e	;Parse_MIDI_One
; first test if there has been a valid message command byte			;Parse_MIDI_One
; received after the last completed MIDI message. MsgLen=1 if yes.		;Parse_MIDI_One
	lds	temp, MsgLen1  							;Parse_MIDI_One
	tst	temp								;Parse_MIDI_One
	breq	MP0_f								;Parse_MIDI_One
	rjmp	MP0_j								;Parse_MIDI_One
										;Parse_MIDI_One
;  test whether NMB data byte has arrived before ANY command byte		;Parse_MIDI_One
MP0_f:	;MsgLen=0, NMB is data byte					;*f	;Parse_MIDI_One
	lds	temp, RunCmd1							;Parse_MIDI_One
	tst	temp								;Parse_MIDI_One
	breq	MP0_f1								;Parse_MIDI_One
	rjmp	MP0_g								;Parse_MIDI_One
;										;Parse_MIDI_One
MP0_f1:	; **Error  NMB data byte has arrived before ANY command byte		;Parse_MIDI_One
	lds	temp, MsgFlags1							;Parse_MIDI_One
	sbr	temp, (1 << isError)						;Parse_MIDI_One
	sts	MsgFlags1, temp							;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
;										;Parse_MIDI_One
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_One
;  Running-Status message handler						;Parse_MIDI_One
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_One
; NMB is a data byte; MsgLen=0; RunCmd != 0x00.  A previous Cmd byte received.	;Parse_MIDI_One
; This is a Running-Status msg.							;Parse_MIDI_One
; 1> store NMB into MsgBuff[MsgLen].						;Parse_MIDI_One
; 2> Increment MsgLen. 								;Parse_MIDI_One
; 3> Determine if 1st or 2nd data byte of a Running-Status msg.			;Parse_MIDI_One
; 4a> If this is a rare 1-byte RS message (RunCmd==0xCn or 0xDn)		;Parse_MIDI_One
;	-load the RunCmd position into pointer BuffPtrH:L			;Parse_MIDI_One
;	-increment MsgLen to value 2.						;Parse_MIDI_One
;	-set isMsgRdy flag in MsgFlags.						;Parse_MIDI_One
;	-return to main code.							;Parse_MIDI_One
;  Main code will send two consecutive bytes starting at RunCmd.		;Parse_MIDI_One
;										;Parse_MIDI_One
; 4b> If this is normal 2-byte RS message (RunCmd !=0xCn or 0xDn)		;Parse_MIDI_One
;	-load the address of RunCmd into pointer BuffPtrH:L			;Parse_MIDI_One
;	-increment MsgLen to value 3.						;Parse_MIDI_One
;	-set isMsgRdy flag in MsgFlags.						;Parse_MIDI_One
;	-return to main code.							;Parse_MIDI_One
;  Main code will send three consecutive bytes starting at RunCmd.		;Parse_MIDI_One
;  The message now includes the command byte missing from RunningStatus msg.	;Parse_MIDI_One
;										;Parse_MIDI_One
; * some MIDI controller keyboards use Run-Status msgs when chords are		;Parse_MIDI_One
;   played, like the Roland PC150.						;Parse_MIDI_One
;   RunCmd is almost always 0x9n, 0xBn, or 0xEn, rarely 0xCn or 0xDn		;Parse_MIDI_One
;										;Parse_MIDI_One
MP0_g:	; MsgLen=0, RunCmd != 0x00.  NMB is first byte of RunStat msg	;*g	;Parse_MIDI_One
	lds	temp, NewMIDI1byte						;Parse_MIDI_One
	ldi	ZH, high (MsgBuff1)						;Parse_MIDI_One
	ldi	ZL, low  (MsgBuff1)						;Parse_MIDI_One
	st	z, temp			; NMB stored at MsgBuff[0]		;Parse_MIDI_One
;										;Parse_MIDI_One
	ldi	temp, 1								;Parse_MIDI_One
	sts	MsgLen1, temp							;Parse_MIDI_One
;										;Parse_MIDI_One
	lds	temp2, RunCmd1  						;Parse_MIDI_One
	andi	temp2, 0xf0	; separate cmd from channel			;Parse_MIDI_One
	cpi	temp2, 0xc0	; Program change				;Parse_MIDI_One
	breq	MP0_g1								;Parse_MIDI_One
	cpi	temp2, 0xd0	; Channel Aftertouch				;Parse_MIDI_One
	breq	MP0_g1								;Parse_MIDI_One
;										;Parse_MIDI_One
; RunStat msg, NMB is 1st data byte					;*i	;Parse_MIDI_One
	clr	temp								;Parse_MIDI_One
 	ori	temp, (1 << isThreeByte) | (1 << isRunStatus)			;Parse_MIDI_One
	sts	MsgFlags1, temp							;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
;------------------------------------						;Parse_MIDI_One
;										;Parse_MIDI_One
MP0_g1:		; rare RunningStatus msg with only one data byte 	;*h	;Parse_MIDI_One
	ldi	temp, high (RunCmd1)						;Parse_MIDI_One
	sts	BuffPtr1H, temp							;Parse_MIDI_One
	ldi	temp, low  (RunCmd1)						;Parse_MIDI_One
	sts	BuffPtr1L, temp							;Parse_MIDI_One
;										;Parse_MIDI_One
	ldi	temp, (1 << isMsgRdy)						;Parse_MIDI_One
	sts	MsgFlags1, temp							;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
;------------------------------------						;Parse_MIDI_One
;										;Parse_MIDI_One
MP0_j: ; new data byte, MsgLen != 0, called by MP0. Determine msg type	;*j	;Parse_MIDI_One
	lds	temp, MsgFlags1							;Parse_MIDI_One
	sbrc	temp, isSysEx							;Parse_MIDI_One
	rjmp	MP0_k								;Parse_MIDI_One
										;Parse_MIDI_One
	sbrc	temp, isTwoByte 						;Parse_MIDI_One
	rjmp	MP0_l								;Parse_MIDI_One
										;Parse_MIDI_One
	sbrc	temp, isThreeByte						;Parse_MIDI_One
	rjmp	MP0_m								;Parse_MIDI_One
;										;Parse_MIDI_One
;;  ######### DEBUG 	NOTE-ON gets here? no					;Parse_MIDI_One
;	cbi	LED_PORT, LED_DEBUG 	;turn on LED 1/32 sec			;Parse_MIDI_One
;										;Parse_MIDI_One
	rjmp	MP_exit ; should never get here					;Parse_MIDI_One
;-----------------------							;Parse_MIDI_One
;										;Parse_MIDI_One
MP0_k:		; new SysEx data byte						;Parse_MIDI_One
	lds	temp, NewMIDI1byte	; store NMB into MsgBuff	;*k	;Parse_MIDI_One
	ldi	ZH, high (MsgBuff1)	; MsgBuff in SRAM			;Parse_MIDI_One
	ldi	ZL, low  (MsgBuff1)	; MsgBuff in SRAM			;Parse_MIDI_One
	lds	temp2, MsgLen1							;Parse_MIDI_One
	add	ZL, temp2  							;Parse_MIDI_One
	adc	ZH, ZeroReg 							;Parse_MIDI_One
	st	z, temp 							;Parse_MIDI_One
	inc	temp2 								;Parse_MIDI_One
	sts	MsgLen1, temp2							;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
;-----------------------							;Parse_MIDI_One
;										;Parse_MIDI_One
MP0_l:		; data byte of a two byte message (0xc0 or 0xd0)	;*l	;Parse_MIDI_One
;										;Parse_MIDI_One
	lds	temp, NewMIDI1byte	; store NMB into MsgBuff		;Parse_MIDI_One
	ldi	ZH, high (MsgBuff1 + 1)	; MsgBuff in SRAM			;Parse_MIDI_One
	ldi	ZL, low  (MsgBuff1 + 1)	; MsgBuff in SRAM			;Parse_MIDI_One
	st	z, temp								;Parse_MIDI_One
	ldi	temp, 2								;Parse_MIDI_One
	sts	MsgLen1, temp 							;Parse_MIDI_One
	ldi	temp, high (MsgBuff1)						;Parse_MIDI_One
	sts	BuffPtr1H, temp							;Parse_MIDI_One
	ldi	temp, low  (MsgBuff1)						;Parse_MIDI_One
	sts	BuffPtr1L, temp							;Parse_MIDI_One
;										;Parse_MIDI_One
 	ldi	temp, (1 << isMsgRdy)						;Parse_MIDI_One
 	sts	MsgFlags1, temp							;Parse_MIDI_One
	rjmp	MP_exit 							;Parse_MIDI_One
;-----------------------							;Parse_MIDI_One
;										;Parse_MIDI_One
MP0_m:	; MsgLen !=0, isThreeByte set					;*m	;Parse_MIDI_One
	lds	temp, MsgLen1 							;Parse_MIDI_One
	cpi	temp, 1		; is first data byte of 3-byte command msg?	;Parse_MIDI_One
	brne	MP0_m1		; or second data byte of RunStat msg?		;Parse_MIDI_One
;										;Parse_MIDI_One
	lds	temp, MsgFlags1	; MsgLen=1, first data byte new msg	;*n	;Parse_MIDI_One
	sbrc	temp, isRunStatus						;Parse_MIDI_One
	rjmp	MP0_m2	; jump if isRunStatus set				;Parse_MIDI_One
	lds	temp, NewMIDI1byte	; 1st data of Normal command 	;*o	;Parse_MIDI_One
	ldi	ZH, high (MsgBuff1)	; MsgBuff in SRAM			;Parse_MIDI_One
	ldi	ZL, low  (MsgBuff1)	; MsgBuff in SRAM			;Parse_MIDI_One
	lds	temp2, MsgLen1							;Parse_MIDI_One
	add	ZL, temp2							;Parse_MIDI_One
	adc	ZH, ZeroReg							;Parse_MIDI_One
	st	z, temp								;Parse_MIDI_One
	ldi	temp2, 2							;Parse_MIDI_One
	sts	MsgLen1, temp2							;Parse_MIDI_One
;										;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
;-----------------------							;Parse_MIDI_One
;										;Parse_MIDI_One
MP0_m1:										;Parse_MIDI_One
	cpi	temp, 2							;*m	;Parse_MIDI_One
	breq	MP0_m1a								;Parse_MIDI_One
;										;Parse_MIDI_One
	rjmp	MP_exit			; should never get here			;Parse_MIDI_One
;										;Parse_MIDI_One
MP0_m1a:	; last data byte of normal 3-byte message, not RunStat	;*p	;Parse_MIDI_One
	ldi	ZH, high (MsgBuff1 + 2)	; MsgBuff in SRAM			;Parse_MIDI_One
	ldi	ZL, low  (MsgBuff1 + 2)	; MsgBuff in SRAM			;Parse_MIDI_One
	lds	temp, NewMIDI1byte 						;Parse_MIDI_One
	st	z, temp								;Parse_MIDI_One
	ldi	temp, 3								;Parse_MIDI_One
	sts	MsgLen1, temp 							;Parse_MIDI_One
	ldi	temp, high (MsgBuff1)						;Parse_MIDI_One
	sts	BuffPtr1H, temp							;Parse_MIDI_One
	ldi	temp, low  (MsgBuff1)						;Parse_MIDI_One
	sts	BuffPtr1L, temp							;Parse_MIDI_One
	ldi	temp, (1 << isMsgRdy)						;Parse_MIDI_One
	sts	MsgFlags1, temp							;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
;------------------------							;Parse_MIDI_One
;										;Parse_MIDI_One
MP0_m2:	; isRunStatus = TRUE, MsgLen = 1				;*q	;Parse_MIDI_One
	lds	temp, NewMIDI1byte	; store NMB into MsgBuff		;Parse_MIDI_One
	ldi	ZH, high (MsgBuff1 + 1)	; MsgBuff in SRAM			;Parse_MIDI_One
	ldi	ZL, low  (MsgBuff1 + 1)	; MsgBuff in SRAM			;Parse_MIDI_One
	st	z, temp 							;Parse_MIDI_One
	ldi	temp, 3								;Parse_MIDI_One
	sts	MsgLen1, temp							;Parse_MIDI_One
	ldi	temp, high (RunCmd1)						;Parse_MIDI_One
	sts	BuffPtr1H, temp							;Parse_MIDI_One
	ldi	temp, low  (RunCmd1)						;Parse_MIDI_One
	sts	BuffPtr1L, temp							;Parse_MIDI_One
	ldi	temp, (1 << isMsgRdy)	; only one flag is set			;Parse_MIDI_One
	sts	MsgFlags1, temp							;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
;										;Parse_MIDI_One
;										;Parse_MIDI_One
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_One
; When the New MIDI byte has the bit 7 set, it is a MIDI command. 		;Parse_MIDI_One
;  This section is the command parser.						;Parse_MIDI_One
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Parse_MIDI_One
MP_doCmd:								;*a	;Parse_MIDI_One
	lds	temp, NewMIDI1byte						;Parse_MIDI_One
	andi	temp, 0xf0							;Parse_MIDI_One
;										;Parse_MIDI_One
; is it a MIDI System command?  0xf0-0xff?				;*c	;Parse_MIDI_One
	cpi	temp, 0xf0							;Parse_MIDI_One
	breq	MP1_a0								;Parse_MIDI_One
	rjmp	MP1_b								;Parse_MIDI_One
;										;Parse_MIDI_One
MP1_a0:										;Parse_MIDI_One
	lds	temp, NewMIDI1byte						;Parse_MIDI_One
	cpi	temp, 0xf0		; is it start of sysex?			;Parse_MIDI_One
	brne	MP1_a1	 							;Parse_MIDI_One
;										;Parse_MIDI_One
	ldi	ZH, high (MsgBuff1)	;  Yes, 0xF0 start-of-sysex		;Parse_MIDI_One
	ldi	ZL, low  (MsgBuff1)						;Parse_MIDI_One
	st	z, temp			; MsgBuff is in SRAM			;Parse_MIDI_One
  	ldi	temp, 1								;Parse_MIDI_One
  	sts	MsgLen1, temp							;Parse_MIDI_One
	ldi	temp, (1 << isSysEx)						;Parse_MIDI_One
	sts	MsgFlags1, temp							;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
; -----------------------------------						;Parse_MIDI_One
										;Parse_MIDI_One
MP1_a1:										;Parse_MIDI_One
	cpi	temp, 0xf7							;Parse_MIDI_One
	brne	MP1_a2	 							;Parse_MIDI_One
;										;Parse_MIDI_One
	ldi	ZH, high (MsgBuff1)	; 0xF7 End-of-sysex			;Parse_MIDI_One
	sts	BuffPtr1H, ZH							;Parse_MIDI_One
	ldi	ZL, low  (MsgBuff1)						;Parse_MIDI_One
	sts	BuffPtr1L, ZL		; ZH:ZL <<  startaddr of MsgBuff	;Parse_MIDI_One
	lds	temp2, MsgLen1		; MsgLen doesn't include 0xf7		;Parse_MIDI_One
	add	ZL, temp2							;Parse_MIDI_One
	adc	ZH, ZeroReg							;Parse_MIDI_One
	lds	temp, NewMIDI1byte	; 0xf7					;Parse_MIDI_One
	st	z, temp								;Parse_MIDI_One
	inc	temp2								;Parse_MIDI_One
	sts	MsgLen1, temp2							;Parse_MIDI_One
	ldi	temp, (1 << isMsgRdy)						;Parse_MIDI_One
	sts	MsgFlags1, temp							;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
; -----------------------------------						;Parse_MIDI_One
;										;Parse_MIDI_One
MP1_a2:										;Parse_MIDI_One
	cpi	temp, 0xf2		; song start?				;Parse_MIDI_One
	brne	MP1_a3	 							;Parse_MIDI_One
;										;Parse_MIDI_One
	ldi	temp, (1 << isTwoByte)						;Parse_MIDI_One
	sts	MsgFlags1, temp							;Parse_MIDI_One
	ldi	ZH, high (MsgBuff1)	; 0xF2 song-start			;Parse_MIDI_One
	ldi	ZL, low  (MsgBuff1)						;Parse_MIDI_One
	lds	temp, NewMIDI1byte						;Parse_MIDI_One
	st	z, temp			; MsgBuff is in SRAM			;Parse_MIDI_One
	ldi	temp, 1								;Parse_MIDI_One
	sts	MsgLen1, temp							;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
; -----------------------------------						;Parse_MIDI_One
;										;Parse_MIDI_One
MP1_a3:										;Parse_MIDI_One
	cpi	temp, 0xf3		; song pointer				;Parse_MIDI_One
	brne	MP1_a4	 							;Parse_MIDI_One
;										;Parse_MIDI_One
	ldi	temp, (1 << isThreeByte)					;Parse_MIDI_One
	sts	MsgFlags1, temp							;Parse_MIDI_One
	ldi	ZH, high (MsgBuff1)	; 0xF3 song-pointer			;Parse_MIDI_One
	ldi	ZL, low  (MsgBuff1)						;Parse_MIDI_One
	lds	temp, NewMIDI1byte						;Parse_MIDI_One
	st	z, temp			; MsgBuff is in SRAM			;Parse_MIDI_One
	ldi	temp, 1								;Parse_MIDI_One
	sts	MsgLen1, temp							;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
; -----------------------------------						;Parse_MIDI_One
;										;Parse_MIDI_One
MP1_a4:	; not a valid system command (0xfx), don't send to MIDIout		;Parse_MIDI_One
	clr	temp		; reset the MsgLen variable for next MIDIin	;Parse_MIDI_One
	sts	MsgLen1, temp							;Parse_MIDI_One
	sts	MsgFlags1, temp	; clear all flags after 1-byte command 		;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
;------------------------------------						;Parse_MIDI_One
;										;Parse_MIDI_One
MP1_b:	cpi	temp, 0xc0	; check for 2-byte message		;*b	;Parse_MIDI_One
	breq	MP1_b1								;Parse_MIDI_One
	cpi	temp, 0xd0							;Parse_MIDI_One
	breq	MP1_b1								;Parse_MIDI_One
	rjmp	MP1_c								;Parse_MIDI_One
										;Parse_MIDI_One
MP1_b1: 									;Parse_MIDI_One
	ldi	temp, (1 << isTwoByte)	; yes, it's a two byter (1 cmd:1 data)	;Parse_MIDI_One
	sts	MsgFlags1, temp							;Parse_MIDI_One
	lds	temp, NewMIDI1byte						;Parse_MIDI_One
	sts	RunCmd1, temp							;Parse_MIDI_One
	ldi	ZH, high (MsgBuff1)	; 0xc0 or 0xd0				;Parse_MIDI_One
	ldi	ZL, low  (MsgBuff1)						;Parse_MIDI_One
	st	z, temp			; MsgBuff is in SRAM			;Parse_MIDI_One
	ldi	temp, 1								;Parse_MIDI_One
	sts	MsgLen1, temp							;Parse_MIDI_One
;										;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
; -----------------------------------						;Parse_MIDI_One
										;Parse_MIDI_One
MP1_c:		; 3-byte standard message, not RunningStatus			;Parse_MIDI_One
	ldi	temp, (1 << isThreeByte)				;*d	;Parse_MIDI_One
	sts	MsgFlags1, temp							;Parse_MIDI_One
	lds	temp, NewMIDI1byte						;Parse_MIDI_One
	sts	RunCmd1, temp							;Parse_MIDI_One
	ldi	ZH, high (MsgBuff1)	; 0xc0 or 0xd0				;Parse_MIDI_One
	ldi	ZL, low  (MsgBuff1)						;Parse_MIDI_One
	st	z, temp			; MsgBuff is in SRAM			;Parse_MIDI_One
	ldi	temp, 1								;Parse_MIDI_One
	sts	MsgLen1, temp							;Parse_MIDI_One
;										;Parse_MIDI_One
	rjmp	MP_exit								;Parse_MIDI_One
; -----------------------------------						;Parse_MIDI_One
										;Parse_MIDI_One
MP_exit: ; whew! finally finished.  Return MsgBuff1[], MsgLen1, & MsgFlags1	;Parse_MIDI_One
	pop	ZH								;Parse_MIDI_One
	pop	ZL								;Parse_MIDI_One
	pop	temp2								;Parse_MIDI_One
	pop	temp								;Parse_MIDI_One
	ret									;Parse_MIDI_One
;										;Parse_MIDI_One
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



;############################################################################################


ID:	;0123456789abcdef
.db	"                ****  Mrg1634.asm  v0.1  alan_probandt@yahoo.com"
; normal little-ended char alignment for binary program ID.

;############################################################################################


                                                                                            
;;####################### DEBUG ########################################                    
;; verify that MIDI1 output system works at 31250 baud (32uS interval)                      
;	ldi	temp,  (1<<TXEN0) | (1<<RXEN1) | (1<<RXCIE1)                                
;	sts	UCSR1B, temp		; 0x78                                              
;	ldi	temp, 0x55                                                                  
;UUU_loop1:                                                                                 
;	lds	temp3, UCSR1A	 ; UCSR1A = 0x79                                            
;	andi    temp3, (1 << UDRE1)                                                         
;	breq	UUU_loop1                                                                   
;	sts	UDR1, temp    	 ; UDR1	= 0x73                                              
;                                                                                           
;; verify that MIDI0 output system works at 31250 baud (32uS interval)                      
;;	ldi	temp, 0x55                                                                  
;UUU_loop0:                                                                                 
;	in	temp3, UCSR0A	 ; UCSR0A = 0x26 [38 dec]                                   
;	andi    temp3, (1 << UDRE0)  ; can't use sbis A,b because 0<=A<=31, 0<=b<=7         
;	breq	UUU_loop0                                                                   
;	out	UDR0, temp    	 ; UDR0	= 0x20                                              
;	rjmp	UUU_loop1                                                                   
;;####################### DEBUG ########################################                    
                                                                                            
                                                                                            