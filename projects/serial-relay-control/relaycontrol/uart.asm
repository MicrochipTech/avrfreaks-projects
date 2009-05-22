;Code from AVR304: Half Duplex Interrupt Driven Software UART

;***** BAUD-rate settings

				;BAUD-RATES @1MHz XTAL AND R=1
.equ	N=104			; 9600
;.equ	N=52			;19200
;.equ	N=26			;38400
.equ	C=1			;Divisor
.equ	R=1			;R=1 when C=1



;***** UART Global Registers

.def	u_buffer	=r14	;Serial buffer
.def	u_sr		=r15	;Status-register storage
.def	u_tmp		=r16	;Scratchregister
.def	u_bit_cnt	=r17	;Bit counter
.def	u_status	=r18	;Status buffer
.def	u_reload	=r19	;Reload-register (internal - do not use)
.def	u_transmit	=r20	;Data to transmit


;***** Bit positions in the Status-register

.equ	RDR=0			;Receive data ready bit
.equ	TD=6			;Transmitting data (internal - read-only)
.equ	BUSY=7			;Busy-flag (internal - read-only)



;**************************************************************************
;*
;* EXT_INT0 - External Interrupt Routine 0
;*
;*
;* DESCRIPTION
;* This routine is executed when a negative edge on the incoming serial
;* signal is detected. It disables further external interrupts and enables
;* timer interrupts (bit-timer) because the UART must now receive the
;* incoming data. 
;*
;* This routine sets bits in the GIMSK, TIFR and TIMSK registers. In this
;* code when the bits are set, it overwrites all other bits. This is done
;* because of the lack of available cycles when it operates at low clock
;* rate and high baudrates.
;*
;*
;* Total number of words	: 12
;* Total number of cycles	: 15 (incl. reti)
;* Low register usage		: 1 (u_sr)
;* High register usage		: 4 (u_bit_cnt,u_tmp,u_status,u_reload)
;*
;**************************************************************************
ext_int0:
	in	u_sr,SREG		;Store Status Register

	ldi	u_status,1<<BUSY	;Set busy-flag (clear all others)

	ldi	u_tmp,(256-(N+N/2)+(29/C));Set timer reload-value (to 1.5
	out	TCNT0,u_tmp		;  bit len). 29 = time delay that
					;  have already been used in this
					;  interrupt plus the time
					;  that will be used by the time
					;  delay between timer interrupt request
					;  and the actual sampling of the first
					;  data bit.

	ldi	u_tmp,1<<TOIE0		;Set bit 1 in u_tmp
	out	TIFR,u_tmp		;   to clear T/C0 overflow flag
	out	TIMSK,u_tmp		;   and enable T/C0 overflow interrupt

	clr	u_bit_cnt		;Clear bit counter
	out	GIMSK,u_bit_cnt		;Disable external interrupt

	ldi	u_reload,(256-N+(8/C))	;Set reload-value (constant).

	out	SREG,u_sr		;Restore SREG
	reti


;**************************************************************************
;*
;* TIM0_OVF - Timer/Counter 0 Overflow Interrupt
;*
;*
;* DESCRIPTION
;* This routine coordinates the transmition and reception of bits. This 
;* routine is automatically executed at a rate equal to the baud-rate. When
;* transmitting, this routine shifts the bits and sends it. When receiving,
;* it samples the bit and shifts it into the buffer.
;*
;* The serial routines uses a status register (u_status): READ-ONLY.
;*		BUSY	This bit indicates whenever the UART is busy
;*		TD	Transmit Data. Set when the UART is transmitting
;*		RDR	Receive Data Ready. Set when new data has arrived
;*			and it is ready for reading.
;*
;* When the RDR flag is set, the (new) data can be read from u_buffer.
;*
;* This routine also sets bits in TIMSK and TIMSK. See ext_int0 description
;*
;* 
;* Total number of words	: 35
;* Total number of cycles	: min. 18, max. 28 - depending on if it is
;*				  receiving or transmitting, what bit is
;*				  pending, etc.
;* Low register usage		: 2 (u_sr,u_buffer)
;* High register usage		: 4 (u_bit_cnt,u_tmp,u_status,u_reload)
;* 
;**************************************************************************
tim0_ovf:
	in		u_sr,SREG		;Store statusregister

	out		TCNT0,u_reload		;Reload timer

	inc		u_bit_cnt		;Increment bit_counter
	sbrs	u_status,TD		;if transmit-bit set
	rjmp	tim0_receive		;    goto receive

	sbrc	u_bit_cnt,3		;if bit 3 in u_bit_cnt (>7) is set
	rjmp	tim0_stopb		;    jump to stop-bit-part

	sbrc	u_buffer,0		;if LSB in buffer is 1
	sbi		PORTB,TX		;    Set transmit to 1
	sbrs	u_buffer,0		;if LSB in buffer is 0
	cbi		PORTB,TX		;    Set transmit to 0
	lsr		u_buffer		;Shift buffer right

	ldi 	u_tmp,INTF0 
	out 	GIFR,u_tmp ; clear pending ext. int. 
	
	out		SREG,u_sr		;Restore SREG
	reti


tim0_stopb:
	sbi		PORTB,TX		;Generate stop-bit

	sbrs	u_bit_cnt,0		;if u_bit_cnt==8 (stop-bit)
	rjmp	tim0_ret		;      jump to exit

tim0_complete:
	ldi		u_tmp,1<<INTF0 
	out 	GIFR,u_tmp ; clear pending ext. int. 
	ldi		u_tmp,1<<INT0		;(u_bit_cnt==9):
	out		GIMSK,u_tmp		;Enable external interrupt
	clr		u_tmp 
	out		TIMSK,u_tmp		    ;Disable timer interrupt
	cbr		u_status,(1<<BUSY)|(1<<TD)  ;Clear busy-flag and transmit-flag

tim0_ret:
	out		SREG,u_sr		;Restore status register
	reti


tim0_receive:
	sec					;Set carry
	sbis	PINB,RX		;if PB6=LOW         <=== SAMPLE HERE
	clc				;    clear carry
	ror		u_buffer		;Shift carry into data
	in		u_tmp,SREG		;Store SREG

	cpi		u_bit_cnt,9		;if u_bit_cnt!=9 (must sample stop-bit)
	brne	tim0_ret		;   exit interrupt

	out		SREG,u_tmp		;Get old SREG
	rol		u_buffer		;Rotate back data (to get rid of the stop-bit)

	sbr		u_status,1<<RDR		;Clear busy-flag
	rjmp	tim0_complete



;**************************************************************************
;*
;* uart_init - Subroutine for UART initialization
;*
;*
;* DESCRIPTION
;* This routine initializes the UART. It sets the timer and enables the
;* external interrupt (receiving). To enable the UART the global interrupt
;* flag must be set (with SEI).
;*
;*
;* Total number of words	: 8
;* Total number of cycles	: 11 (including the RET instructions)
;* Low register usage		: None
;* High register usage		: 2 (u_tmp,u_status)
;* 
;**************************************************************************
uart_init:	
	ldi		u_tmp,R
	out		TCCR0,u_tmp		;Start timer and set clock source
	ldi		u_tmp,1<<INT0
	out		GIMSK,u_tmp		;Enable external interrupt 0
	ldi		u_tmp,1<<ISC01
	out		MCUCR,u_tmp		;On falling edges
	clr		u_status		;Erase status-byte
	ret



;**************************************************************************
;*
;* uart_transmit - Subroutine for UART transmittal
;*
;*
;* DESCRIPTION
;* This routine initialize the UART to transmit data. The data to be sent
;* must be located in u_transmit. 
;*
;* Warning: This routine cancels all other transmittions or receptions.
;* So be careful. If a byte is being received and the timer interrupt is
;* currently running, the tranmittion may be corrupted. By checking the
;* READY-bit and/or TD-bit in the u_status register for safe transmissions.
;*
;* This routine also sets bits in TIMSK and TIMSK. See ext_int0 description
;*
;*
;* Total number of words	: 13
;* Total number of cycles	: 17 (including RET)
;* Low register usage		: 1 (u_buffer)
;* High register usage		: 4 (u_bit_cnt,u_tmp,u_transmit,u_reload)
;* 
;**************************************************************************
uart_transmit:
	ldi		u_status,(1<<BUSY)|(1<<TD)   ;Set only busy- and transmit flag

	clr		u_tmp
	out		GIMSK,u_tmp		;Disable external interrupt

	ser		u_bit_cnt		;Erase bit-counter (set all bits)
	mov		u_buffer,u_transmit	;Copy transmit-data to buffer

	ldi		u_tmp,1<<TOIE0		;Set bit 1 in u_tmp
	out		TIFR,u_tmp		;  to clear T/C0 overflow flag
	out		TIMSK,u_tmp		;  and enable T/C0 overflow interrupt
	
	ldi		u_reload,(256-N+(8/C))	;Set reload-value
	ldi		u_tmp,(256-N+(14/C))	;Set timer delay to first bit
	out		TCNT0,u_tmp		;Set timer reload-value (1 bit)

	cbi		PORTB,TX		;Clear output (start-bit)
	ret
