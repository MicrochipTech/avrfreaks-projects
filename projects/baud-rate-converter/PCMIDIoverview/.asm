; PCMIDI644_oneQueue.asm    using a two-UART microcontroller to transfer data to and from a PC and a MIDI music synthesizer through the PC serial port.
;
;  no code in Main; three interrupt routines
;
; Alan Probandt    alanprobandt@gmail.com  Portland, Oregon USA    March 22, 2010
; initial file: July 10, 2007   version 1.00   Atmel AVR Mega164; Mega324; Mega664
; Played the file [fun.mid] without 'illegal data' message on Yamaha MU50.  No PCMIDI using a softUART could do this.
;
;  A sysex dump block of 1500 bytes takes 0.39 seconds to transmit at 38400 baud from the PC.  This program will resend 1215 bytes (31.25K MIDI baud) in that
;  same 0.39s period.   A block of 1500 MIDI bytes will take 0.48 sec to be sent to the synthesizer.  The queue needs to hold 300 bytes 
;  from the PC for about 0.1 second.  In the other direction, the AVR RS232 USART will have completed retransmitting (in 260uSeconds- 10 bits at 38.4Kbaud) 
;  each byte from the synth before the next synth byte fully arrives (320uS - 10 bits at 31.25Kbaud).  For that reason, only one large (> 256 bytes) queue
;  is needed for bidirectional communication between the RS232 PC and the MIDI synth.
;
;        	Atmel AVR Mega644 microcontroller  64K Flash/4K SRAM/2K EEPROM
;                                 _______
;       		     PB0  |1  40| PA0/ADC0/PCint0
;       		     PB1  |2  39| PA1/ADC1/PCint1
;       		     PB2  |3  38| PA2/ADC2/PCint2
;       		     PB3  |4  37| PA3/ADC3/PCint3
;       		     PB4  |5  36| PA4/ADC4/PCint4
;               	MOSI/PB5  |6  35| PA5/ADC5/PCint5
;              		MISO/PB6  |7  34| PA6/ADC6/PCint6
;               	 SCK/PB7  |8  33| PA7/ADC7/PCint7
;                          \RESET |9  32| AREF
;                            VCC  |10 31| GND
;                            GND  |11 30| AVCC
;                8.0 MHz   Xtal2  |12 29| PC7
;                8.0 MHz   Xtal1  |13 28| PC6
;        	   MIDIin    RXD0 |14 27| PC5
;         	   MIDIout   TXD0 |15 26| PC4
;	    fromPC Max232#9  RXD1 |16 25| PC3
;  	     toPC Max232#10  TXD1 |17 24| PC2
;      			     PD4  |18 23| PC1
;     			     PD5  |19 22| PC0
;           		     PD6  |20 21| PD7
;          		          -------
.nolist
.include "m644Pdef.inc"
.list

.equ	clock 		= 8000000   ; adjust OSCCAL if using internal system clock
.equ	MIDIbaud	= ((clock/16)/31250)-1  ; 0.0% error
.equ	PCbaud 		= ((clock/16)/38400)-1  ; 0.2% error

;***************************************************************************

; register alias names for USARTs
.equ	UCSRA_MIDI	= UCSR0A  ; data going to and from synth using MIDI
.equ	UCSRB_MIDI	= UCSR0B
.equ	UDR_toMIDI	= UDR0
.equ	UDR_fromMIDI	= UDR0
.equ	UBRRL_MIDI	= UBRR0L
.equ	UBRRH_MIDI	= UBRR0H

.equ	UCSRA_PC	= UCSR1A  ; data going to and from PC in RS-232
.equ	UCSRB_PC	= UCSR1B
.equ	UDR_toPC	= UDR1
.equ	UDR_fromPC	= UDR1
.equ	UBRRL_PC	= UBRR1L
.equ	UBRRH_PC	= UBRR1H
;---------------------------------------
; bit alias names for USARTs
.equ	RXEN_PC		= RXEN0
.equ	TXEN_PC		= TXEN0
.equ	UDRIE_PC	= UDRIE0
.equ	RXCIE_PC	= RXCIE0

.equ	RXEN_MIDI	= RXEN1
.equ	TXEN_MIDI	= TXEN1
.equ	UDRIE_MIDI	= UDRIE1
.equ	RXCIE_MIDI	= RXCIE1

.equ	QUEUESIZE	= 512           ; queue size must be 2^n
.equ	QUEUEMASKLO	= (low  (QUEUESIZE) - 1)	; not used.  Only needed if queue is 128, 64, or 32 bytes in size
.equ	QUEUEMASKHI	= (high (QUEUESIZE) - 1)

;****************************************************************************
; register usage
.def	SREGtemp	= r1

; upper registers
.def	temp		= r16
.def	irqtemp		= r17	; interrupts ALWAYS use a seperate temp register from the temp used by main code
.def	fromPC_In_Low	= r26 ; XL
.def	fromPC_In_High	= r27 ; XH
.def	toMIDI_Out_Low	= r28 ; YL
.def	toMIDI_Out_High	= r29 ; YH

;***** Static RAM data *****************
.dseg

fromPC_Queue:	.byte	 QUEUESIZE


;***********************************
.cseg
; interrupt and power-on vectors
.org 0x0000
	jmp reset ; power-on
; mega644 interrupt vector table -- each entry is two bytes in size
	reti nop		;0x0002	; External Interrupt Request 0
	reti nop		;0x0004	; External Interrupt Request 1
	reti nop		;0x0006	; External Interrupt Request 2
	reti nop		;0x0008	; Pin Change Interrupt Request 0
	reti nop		;0x000a	; Pin Change Interrupt Request 1
	reti nop		;0x000c	; Pin Change Interrupt Request 2
	reti nop		;0x000e	; Pin Change Interrupt Request 3
	reti nop		;0x0010	; Watchdog Time-out Interrupt
	reti nop		;0x0012	; Timer/Counter2 Compare Match A
	reti nop		;0x0014	; Timer/Counter2 Compare Match B
	reti nop		;0x0016	; Timer/Counter2 Overflow
	reti nop		;0x0018	; Timer/Counter1 Capture Event
	reti nop		;0x001a	; Timer/Counter1 Compare Match A
	reti nop		;0x001c	; Timer/Counter1 Compare Match B
	reti nop		;0x001e	; Timer/Counter1 Overflow
	reti nop		;0x0020	; Timer/Counter0 Compare Match A
	reti nop		;0x0022	; Timer/Counter0 Compare Match B
	reti nop		;0x0024	; Timer/Counter0 Overflow
	reti nop		;0x0026	; SPI Serial Transfer Complete
	jmp RxC_FromMIDI	;0x0028 ; USART0, Rx Complete.  Get data from MIDI. Send to PC. No queueing needed. 
	jmp UDRE_toMIDI		;0x002a ; USART0 Data register Empty.  Pull data from Queue and send byte to MIDI USART, then update Out_Pointer.
	reti nop		;0x002c	; USART0, Tx Complete
	reti nop		;0x002e	; Analog Comparator
	reti nop		;0x0030	; ADC Conversion Complete
	reti nop		;0x0032	; EEPROM Ready
	reti nop		;0x0034	; 2-wire Serial Interface
	reti nop		;0x0036	; Store Program Memory Read
	jmp RxC_FromPC		;0x0038	; USART1 RX complete.   Get byte from PC and put it into queue. 
	reti nop		;0x003a	; USART1 Data Register Empty
	reti nop		;0x003c	; USART1 TX complete


reset:
; initialize stack     -- Stack on Mega164P self-initializes to 0x10ff.  
	ldi	temp, high (RAMEND)
	out	SPH, temp
	ldi	temp, low  (RAMEND)
	out	SPL, temp

; initialize USART0
	ldi	temp,  high (MIDIbaud)
	sts 	UBRRH_MIDI, temp
	ldi	temp, low (MIDIbaud)
	sts 	UBRRL_MIDI, temp
	ldi 	temp, (1<<TXEN_MIDI) | (1<<RXEN_MIDI) | (1<<RXCIE_MIDI) ; USART0 - data to and from MIDI synth
	sts 	UCSRB_MIDI, temp

; initialize USART1
	ldi	temp, high (PCbaud)
	sts 	UBRRH_PC, temp
	ldi	temp, low (PCbaud)
	sts 	UBRRL_PC, temp
	ldi 	temp, (1<<TXEN_PC) | (1<<RXEN_PC) | (1<<RXCIE_PC)  ; USART1 - data to and from PC
	sts 	UCSRB_PC, temp

; enable global interrupts
 	sei

; initialize application
	clr	XL   ; fromPC_In_Low   pointer :: all pointers start at zero
	clr	XH   ; fromPC_In_High  pointer :: all pointers start at zero
	clr	YL   ; toMIDI_Out_Low  pointer :: all pointers start at zero
	clr	YH   ; toMIDI_Out_High pointer :: all pointers start at zero   
	
	
	

;***********************************************************
;    MAIN CODE would go here
;***********************************************************

MainLoop:
	rjmp	MainLoop



;************************************
;      interrupt routines
;************************************
;UDRE_toMIDI:   Get the byte that is to be sent out to the MIDI synthesizer from the FromPC queue (under the Out pointer).
;  Put the byte into the MIDI_USART's UDR register.  Increment Out pointer and check if it is now equal to In pointer.
;  If yes, then disable this interrupt.  If no, then exit this IRQ.  If still enabled, then this IRQ will trigger again
;  when the MIDI_USART has finished transmitting the byte.
;  Out Ptr == The first byte in, becoming the first byte out.  Locates where bytes will be taken out of the queue.
;  In Ptr ==  Locates where bytes will be put into the queue when they arrive into the RS232 USART.

;reset:   IO   (use column mode or Terminal font for character alignment here)
;UDREoff  |
; Queue:  00  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f
;
;        Out  In  The RxC IRQ put the byte from the PC (b1) into Q location that In pointed to, then it incremented In and enabled this UDRE_toMIDI IRQ.
;UDREon   |   |
; Queue:  b1  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f
;
; Usually interrupt routines are enabled by the AVR's initialization code, and remain inactive until triggered.  But the Transmit Data Register Empty IRQ triggers
; as soon as it is enabled and runs again and again until it is specifically disabled.  When enabled, UDRE_toMIDI gets the byte pointed to by the queue Out pointer
; and sends it out of the USART by putting it into UDR.   Next it increments the Out pointer to the next location in the queue.   Then it checks if the Out pointer
; equals the In pointer.  If yes, then there are no more new bytes from the PC to send to the MIDI, and it disables itself by turning off the interrupt enable switch.
; If Out is still behind the In, then it will trigger again when the MIDI_USART has finished transmitting the byte. It will retrigger until the Out ptr equals the In ptr.
;
UDRE_toMIDI:
	in	SREGtemp, SREG
	push	ZH
	push	ZL
	
	ldi	ZH, high (fromPC_Queue)    
	ldi	ZL, low  (fromPC_Queue)    
	add	ZL, YL	; YL is the toMIDI_Out_Low
	adc	ZH, YH	; YH is the toMIDI_Out_High

	ld	irqtemp, Z
	sts	UDR_toMIDI, irqtemp

	adiw	YH:YL, 1	; increment toMIDI pointer. Out pointer is ANDed with 0000 0001 1111 1111
	andi	YH, QUEUEMASKHI	; 0x01 for buffer size = 512 (0x0010 0000 0000)  Incrementing 511 rolls over (after ANDi of high byte) back to 0x000.

	cpse	XL, YL		; test if the new value of the Out Pointer matches the In Pointer
	rjmp	U2M_exit	; no, so there is more data to send in the queue.  Exit IRQ and re-enter the IRQ when USART has finished transmitting the current byte.
	cpse	XH, YH		; since the queue is bigger than 255 bytes, the 8-bit registers that hold the IN and OUT pointers may have the same value and not be
	rjmp	U2M_exit	; pointing to the same location in the queue.  If IN and OUT low ptrs are equal, test high pointers.  If IN and Out high ptrs are equal, 
				; then disable the IRQ code.  Otherwise, simply exit the IRQ code after restoring registers.  IRQ with happen again when byte is finished transmit.

	lds	irqtemp, UCSRB_MIDI ; turn off the transmitter to MIDI (this IRQ code)
	andi	irqtemp,  ~(1<<UDRIE_MIDI) ; if in ptr equals out ptr
	sts	UCSRB_MIDI, irqtemp  ; no more bytes in output queue to be sent

U2M_exit:
	pop	ZL
	pop	ZH
	out	SREG, SREGtemp
	reti



;-------------------------------------------------------------------------------------
;  Data from the PC has to be put into a queue because it is arriving faster than the
;  program can retransmit it to the synth.
;
;  Here is the queue after reset but before any data arrives into the USART
;              OI  (out, in)
;               |
;      Queue:  00  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f
;
;
;  The queue is a first in; first out data structure.
;  Here is the queue after a new byte arrives into the USART, and the RxC_IRQ exits.
;               O   In
;               |   |
;      Queue:  b1  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f
;-------------------------------------------------------------------------------------

RxC_FromPC:	; USART1
	in	SREGtemp, SREG
	push	ZH
	push	ZL


	ldi	ZH, high (fromPC_Queue)
	ldi	ZL, low (fromPC_Queue)
	add	ZL, XL				; fromPC_In_Low	  XL
	adc	ZH, XH				; fromPC_In_High  XH
                                        	
	lds	irqtemp, UDR_fromPC		; get byte from USART
	st	Z, irqtemp			; store byte in queue at In Pointer location
                                        	
	adiw	XH:XL, 1			; increment In Pointer and roll over if at end of queue
	andi	XH, QUEUEMASKHI

	lds	irqtemp, UCSRB_MIDI		; turn on the transmitter to MIDI by enabling the UART Data Register Empty interrupt
	ori	irqtemp, (1<<UDRIE_MIDI)	; this calls the interrupt routine that
	sts	UCSRB_MIDI, irqtemp		; puts the new byte from the queue to the UART.

	pop	ZL
	pop	ZH
	out	SREG, SREGtemp
	reti


;---------------------------------------------------------------------------------------------------------
; Because the output from this program to the PC (RS232 at 38.4K baud) is operating faster
;  than the synthesizer can transmit data (31.25K baud), the UDR_toPC will always be ready to retransmit 
;  each new byte that the synth sends.  So there is no need to buffer this data into a queue.
RxC_FromMIDI:  ; USART0
	in	SREGtemp, SREG
	lds	irqtemp, UDR_fromMIDI
	sts	UDR_toPC, irqtemp
	out	SREG, SREGtemp



; ASCII program ID in the code.  Some hex viewers (like the SP12 AVR programmer) reverse byte pairs.

.db	"****  CPIMID46_4noQeeueua.ms  1v0. 0a alpnorabdn@tmgia.loc m"
.db	"****  PCMIDI644_oneQueue.asm  v1.00  alanprobandt@gmail.com "