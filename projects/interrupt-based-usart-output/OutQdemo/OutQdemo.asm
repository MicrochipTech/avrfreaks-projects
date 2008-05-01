; OutQdemo.asm  - Short demo of an output queue for an AVR USART.

; This program shows how to use an output queue and the Transmit_Buffer_Empty interrupt.
; This technique allows the main program to run at full speed when transmitting multi-byte messages out of a USART.
; Sample code for USARTs invariably show the main program polling the USART output_ready flag when transmitting.
; While that technique works, it suspends the operation of the program for the duration of the USART output. 
; The code below shows how to write the output bytes to a queue at full speed and return to the main code.
; The USART transmit interrupt routine then outputs each byte in the background.
;
; There are two routines used for this process (40 instructions total).  PutOutByte is called with the byte to be sent. 
; It puts the byte into the queue and turns on the USART transmitter.  The other routine is the interrupt that occurs 
; when the transmitter is able to accept a new byte for output.  It switches itself off when all of the bytes have been sent.

.nolist
.include "m48def.inc"
.list

; Constant declarations *********
.equ	clock		= 9216000; my crystal (or use 8000000 ; internal RC speed for modern AVRs) 
.equ	RS232_baud      =  9600	; 9600 baud, which is relatively slow
.equ	BaudLoad 	=  (clock / (RS232_baud * 16) ) - 1 ; AVR USART baud reg formula
.equ	QUEUESIZE	=  0x40 ; must by 2^n and less than 256, such as 16,32,64,or 128
.equ	QUEUESIZE_WRAPMASK = QUEUESIZE-1  ; used to wrap the queue pointer to the beginning of the queue

; Register declarations ***********
.def	SREGtemp	= r0
.def	TxQueueHead	= r1
.def	TxQueueTail	= r2
.def	OutByte		= r3
; high register section
.def	temp		= r16
.def	irqtemp		= r17 ; temp only used in interrupts 

; Static RAM declarations ***********
.dseg
TXQueue:  .byte   QUEUESIZE 

; Interrupt Vector table
.cseg
.org 0
;; interrupt vectors for Mega48
	rjmp	reset ; = 0x000 ; Reset handler    
	reti    ;INT0  = 0x001  ; External Interrupt Request 0
	reti	;INT1  = 0x002  ; External Interrupt Request 1
	reti	;PCI0  = 0x003  ; Pin Change Interrupt Request 0
	reti	;PCI1  = 0x004  ; Pin Change Interrupt Request 0
	reti	;PCI2  = 0x005  ; Pin Change Interrupt Request 1
	reti	;WDT   = 0x006  ; Watchdog Time-out Interrupt
	reti	;OC2A  = 0x007  ; Timer/Counter2 Compare Match A
	reti	;OC2B  = 0x008  ; Timer/Counter2 Compare Match A
	reti	;OVF2  = 0x009  ; Timer/Counter2 Overflow
	reti	;ICP1  = 0x00a  ; Timer/Counter1 Capture Event
	reti	;OC1A  = 0x00b  ; Timer/Counter1 Compare Match A
	reti	;OC1B  = 0x00c  ; Timer/Counter1 Compare Match B
	reti	;OVF1  = 0x00d  ; Timer/Counter1 Overflow
	reti	;OC0A  = 0x00e  ; TimerCounter0 Compare Match A
	reti	;OC0B  = 0x00f  ; TimerCounter0 Compare Match B
	reti	;OVF0  = 0x010  ; Timer/Counter0 Overflow Interrupt	
	reti	;SPI   = 0x011  ; SPI Serial Transfer Complete
	reti	;URXC  = 0x012  ; USART Rx Complete
	rjmp	UDRE_IRQ ;= 0x013  ; USART, Data Register Empty
	reti	;UTXC  = 0x014  ; USART Tx Complete
	reti	;ADCC  = 0x015  ; ADC Conversion Complete
	reti	;ERDY  = 0x016  ; EEPROM Ready
	reti	;ACI   = 0x017  ; Analog Comparator
	reti	;TWI   = 0x018  ; Two-wire Serial Interface
	reti	;SPMR  = 0x019  ; Store Program Memory Read
	
;====================================================
; program initialization code starts here...
reset:
 
; initialize stack
	ldi	temp, low (RAMEND)
	out	SPL, temp
	ldi	temp, high (RAMEND)
	out	SPH, temp
	
; initialize USART
	ldi	temp, high (BaudLoad)
	sts	UBRR0H, temp
	ldi	temp, low (BaudLoad)
	sts	UBRR0L, temp
	ldi	temp, (1 << TXEN0) ; enable USART transmitter, but not the UDRE interrupt
 	sts	UCSR0B, temp
 	    
; initialize application variables
	clr	TxQueueHead
	clr	TxQueueTail
 	
        sei
 	
; *************** main code *****************
main:
	ldi 	ZH, high (HelloMsg * 2)  ; r27
	ldi	ZL, low  (HelloMsg * 2)  ; r26
doMsg:	lpm	OutByte, z+
	tst	OutByte
	breq	done
	rcall	PutOutByte ; puts all the bytes of the message into the output queue quickly
	rjmp	doMsg      ; The queue must be large enough to hold the entire output string
	
done:	rjmp	done

HelloMsg: .db "An interrupt-based UART output that actually works!",0x0d,0x0a,0

	
;;*********************************************************
;; putOutByte  -  UART TX IRQ version
;;  Send one byte only out the USART
;;  Put output byte into queue at tail position, 
;;    increment tail, and turn on UDRE interrupt
;;
;;  send: OutByte (data byte for transmission)
;;  return: nothing
;;  uses: TxQueue, TXQueueTail, YH, YL
;;*********************************************************
PutOutByte:
	ldi	YH, high (TxQueue)
	ldi	YL, low (TxQueue)
	add	YL, TXQueueTail
	brcc	POb_0
	inc	YH  ; output queue may cross a 256-byte boundary in SRAM
POb_0:	st	y, OutByte
	mov	temp, TXQueueTail
	inc	temp
	andi	temp, QUEUESIZE_WRAPMASK ; wrap around to the beginning of the queue
	mov	TXQueueTail, temp

	lds	temp, UCSR0B   ; enable the USART transmit (UDRE) interrupt
	ori	temp, (1 << UDRIE0)
	sts	UCSR0B, temp
	ret



;%%%%%%%%%%%%%%%%%%%%%%%%%%%
; 	INTERRUPTS	%%%%
;%%%%%%%%%%%%%%%%%%%%%%%%%%%


;;*******************************************************************************************************************************
;; UARTout:   Put the byte that is to be send out of the USART into the output queue
;;  then enable the UDRE interrupt.  UDRE interrupt will immediately happen, unless the UART is
;;  currently transmitting a character sequence and is not empty.
;;  Head== the first byte in, becoming the first byte out.  Locates where bytes will be taken out of the queue.
;;  Tail == later bytes in.  Locates where bytes will be put into the queue
;;
;;reset:   HT
;;UDREoff  |
;; OutQ:   00  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f 
;; ^^^ state of TxQueue when PutMIDIbyte gets called
;;
;;         H   T  put b1 into RAM that Qtail points to,then increment tail and enable UDREirq
;;UDREon   |   |
;; OutQ:   b1  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f
;; ^^^ state of TxQueue when UDRE_toMIDI is entered
;;
;;
;; Usually interrupt routines are enabled by the initialization code, and remain inactive until triggered.  
;; But the Transmit Data Register Empty IRQ starts as soon as it is enabled and runs until it is specifically disabled.  
;; When enabled, UDRE_IRQ gets the byte pointed to by the queue head and sends it out of the USART by putting it 
;; into UDR.   Next it increments the head pointer to the next location in the queue.  Then it checks if the head pointer
;; equals the tail pointer.  If yes, then there is no more new bytes from the PC to send to the MIDI, and it disables 
;; itself by turning off the interrupt enable switch.  If head is still behind the tail, then it will trigger again 
;; (as soon as the UDRE flag is set, and after one main code instruction) until the head equals the tail of the queue.
;*******************************************************************************************************************************
UDRE_IRQ:         
	in	SREGtemp, SREG
	push	ZH            ; preserve ZH:ZL in case they are being used by the main program
	push	ZL
	ldi	ZH, high (TxQueue)
	ldi	ZL, low (TxQueue)
	add	ZL, TxQueueHead ; Locates where the new byte will taken out of the queue.
	brcc	UI0  ; this code doesn't check whether the Qhead equals Qtail, it assumes that the enabler has set up the queue.
	inc	ZH  ; output queue may cross a 256-byte boundary in SRAM
UI0:	ld	irqtemp, Z
	sts	UDR0, irqtemp ; send the byte, that is pointed to by QHead, to the USART
	
	mov	irqtemp, TxQueueHead ; tail and head pointers in low registers.  ANDI uses high regs only.
	inc	irqtemp ; point to where the data will be read from on the next UDRE interrupt
	andi	irqtemp, QUEUESIZE_WRAPMASK  ; wrap around to Q begin if at Q end by clearing higher bits of Q ptr
	mov	TxQueueHead, irqtemp; when at end of queue, the Head pointer rolls over to beginning of the queue
	
	cpse	TxQueueTail, TxQueueHead
	rjmp	UI_exit  ; more data in queue (head<>tail), so just exit without disabling UDRE_IRQ.
	lds	irqtemp, UCSR0B ; turn off the USART transmitter
	andi	irqtemp,  ~ (1 << UDRIE0) ; if head equals tail
	sts	UCSR0B, irqtemp  ; because there are no more bytes in the output queue to be sent
UI_exit:
	pop	ZL
	pop	ZH
	out	SREG, SREGtemp
	reti


;PutOutByte:  ; standard polled version of USART output
;	sts	UDR0, OutByte
;POB1:	lds	temp, UCSR0A
;	sbrs 	temp,UDRE0
;	rjmp	POB1
;	ret