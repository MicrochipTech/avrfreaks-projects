;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Half duplex software UART in assembly compatible with ATTiny15L.
;
; Targeted at AVR chips without SRAM, this code uses registers
; in place of SRAM.  should work with minimal modification on 
; virtually all AVR chips.	( see REQUIREMENTS & USAGE below )
;
; 3 Byte receive buffer.  Its not too hard to modify the code to use any
; length provided the registers are available for it.
;
; source is formatted with a tab size of 3.  I suggest using that
; to read/modify it.
;
; Written using WinAVR.
; Version 1.0
; Oct 2005
; Tyler Colbert (tyler@colberts.us)
; variable Baud, 7 or 8 data bits, no parity, 1 or 2 stop bits
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; headers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Use the standard header.  This helps make it all more portable
#include <avr/io.h>
#include "UART.h"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; INTERNAL UART stuff.  Dont modify
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; nbr associated by Timer0 to 1 bit length 
#define	N 	((CLK*CLOCK_MULTIPLIER)/(CLOCK_DIVIDER*BAUD))
; adjusted start bit length    
#define START_BIT_LENGTH	((N + N/2) - ((80*CLOCK_MULTIPLIER)/CLOCK_DIVIDER))
; adjusted data bit length 
#define DATA_BIT_LENGTH		(N - ((56*CLOCK_MULTIPLIER)/CLOCK_DIVIDER))
; total nbr. of bits
#define TOTAL_BITS			(DATA_BITS + STOP_BITS)

; have to fit the longest period into the timer.
#if START_BIT_LENGTH > 0xFF
#error CLOCK_DIVIDER too low or CLOCK_MULTIPLIER too high.  Change defines.
#endif

; we need to be able to represent the length of a data bit.
#if DATA_BIT_LENGTH == 0
#error CLOCK_DIVIDER too high or CLOCK_MULTIPLIER too low.  fix it :)

; I think we need a minimum of about 40 cycles per bit to make this thing work
#elif ( (DATA_BIT_LENGTH*CLOCK_DIVIDER)/CLOCK_MULTIPLIER < 40)
#error BAUD rate too high for selected CPU clock.  Change defines.

; we also need an error < 1%
#elif DATA_BIT_LENGTH < 152
#warning Clock error will probably be unacceptable.  CLOCK_DIVIDER too high or CLOCK_MULTIPLIER too low.
#endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Da Code
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.global UART_init
.global SIGEXTINT
.global SIGCOMPARE
.global UART_putchar
.global UART_getchar

UART_init:

#ifndef NO_RECEIVE
	; pullup RX pin
	sbi		RX_PORT,				RX_BIT
	; set RX pin input
	cbi		RX_DDR,				RX_BIT

	; External Interrupt 0 initialization (for RX)
	; this should set the type of sensitivy to low level sensitive
	in			tempA,				EXTINTTYPE
	andi		tempA,				~EXTINTTYPEMASK
	ori		tempA,				EXTINTTYPEVAL
	out		EXTINTTYPE,			tempA

	clr		rx_buffer_valid
#endif

#ifndef NO_TRANSMIT
	; set TX pin high
	sbi		TX_PORT,				TX_BIT
	; set TX pin output
	sbi		TX_DDR,				TX_BIT
#endif

	; setup timer.
	ldi		tempA,				TIMER_TCCR_VAL
	out		TIMER_TCCR,			tempA

	clr		status

	rjmp		UART_IDLE_MODE

#ifndef NO_RECEIVE
SIGEXTINT:
	in			sreg_save,			_SFR_IO_ADDR(SREG)

	; set rxbusy flag
	sbr		status,				(1<<RX_BUSY)

	; clear UART buffer
	clr		buffer

	; start a transfer
	rjmp UART_BEGIN_TRANSFER
#endif

SIGCOMPARE:
	in			sreg_save,			_SFR_IO_ADDR(SREG)

#ifndef TIMER_AUTOCLEAR
	; clear TCNT
	clr		tempA
	out		TIMER_TCNT,			tempA
#endif

	; set timer reload value to the length of a data bit
	ldi		tempA,				DATA_BIT_LENGTH
	out		TIMER_OCR,			tempA

	; count a bit
	inc		bit_counter

#if !(defined(NO_RECEIVE)||defined(NO_TRANSMIT))
	; send or receive?
	sbrs		status,				TX_BUSY
	rjmp		SIGCOMPARE_RX
#endif

#ifndef NO_TRANSMIT
SIGCOMPARE_TX:
	; if we still have bits to trasnsfer...
	cpi		bit_counter,	DATA_BITS
	brge		SIGCOMPARE_TX_STOPBIT

	; get a bit
	lsr		buffer
	; set the port pin appropriately.
	brcs		SIGCOMPARE_TX_HIGHBIT

	; transmit a low bit
	SIGCOMPARE_TX_LOWBIT:
	cbi		TX_PORT,				TX_BIT
	rjmp		UART_RETI

	; transmit a high bit
	SIGCOMPARE_TX_HIGHBIT:
	sbi		TX_PORT,				TX_BIT
	rjmp		UART_RETI

	; transmit a stop bit
	SIGCOMPARE_TX_STOPBIT:
	sbi		TX_PORT,				TX_BIT
	cpi		bit_counter,		TOTAL_BITS
	breq		UART_IDLE_MODE
	rjmp		UART_RETI
#endif

#ifndef NO_RECEIVE
SIGCOMPARE_RX:
	; still receiving data?
	cpi		bit_counter,		DATA_BITS
	brge		SIGCOMPARE_RX_STOPBIT

	; set carry bit if input bit was set, else clear it.
	clc
	sbic		RX_PIN,				RX_BIT
	sec

	; roll the carry bit into place (and shift the buffer)
	ror		buffer

	rjmp		UART_RETI

	SIGCOMPARE_RX_STOPBIT:

	; check for frame error (stop bit should be high)
	sbis		RX_PIN,				RX_BIT
	rjmp		SIGCOMPARE_RX_FRAME

	; deal with 7 bit transfer needing another shift..
#if DATA_BITS == 6
	lsr		buffer
	lsr		buffer
#elif DATA_BITS == 7
	lsr		buffer
#endif

	; roll the receive buffer
	mov		rx_buffer2,			rx_buffer1
	mov		rx_buffer1,			rx_buffer0
	mov		rx_buffer0,			buffer
	inc		rx_buffer_valid
	mov		tempA,				rx_buffer_valid
	cpi		tempA,				4
	brge		SIGCOMPARE_RX_OVERFLOW
	rjmp		UART_IDLE_MODE

	SIGCOMPARE_RX_OVERFLOW:
	sbr		status,				(1<<RX_OVERFLOW)
	dec		rx_buffer_valid
	rjmp		UART_IDLE_MODE

	SIGCOMPARE_RX_FRAME:
	sbr		status,				(1<<RX_FRAME_ERROR)
;	rjmp		UART_IDLE_MODE
#endif

UART_IDLE_MODE:
	; disable timer 0 interrupt
	in			tempA,				TIMER_TIMSK
	cbr		tempA,				(1<<TIMSK_BIT)
	out		TIMER_TIMSK,		tempA

	; turn off busy status bits
	cbr		status,				(1<<TX_BUSY)|(1<<RX_BUSY)

#ifndef NO_RECEIVE
	; clear external interrupt flag
	ldi		tempA,				(1<<EXTINTBIT)
	out		EXTINTFR,			tempA

	; enable external interrupt
	in			tempA,				EXTINTMSK
	sbr		tempA,				(1<<EXTINTBIT)
	out		EXTINTMSK,			tempA
#endif

	rjmp		UART_RETI

#ifndef NO_RECEIVE
; get a character from the RX buffer. 
; Returns value in getchar_result register defined above
UART_getchar:
	cli
	mov		getchar_result,	rx_buffer0
	mov		rx_buffer0,			rx_buffer1
	mov		rx_buffer1,			rx_buffer2
	dec		rx_buffer_valid
#ifdef UART_GETCHAR_RETADDR
	sei
	rjmp		UART_GETCHAR_RETADDR
#else
	reti
#endif
#endif

#ifndef NO_TRANSMIT
UART_putchar_STALL:
	sei
	; this nop is necessary to allow interrupts to fire before we
	; cli again.  Trust me.. it doesnt work without it :).
	nop
	; fall through to UART_putchar

; output a character.  Receiving takes priority.  Will stall till UART is idle
; gets byte to send from putchar_input register defined above
UART_putchar:
	cli
	; see if BUSY flags are clear...
	bst		status,				TX_BUSY
	sbrc		status,				RX_BUSY
	set
	brts		UART_putchar_STALL

	; at this point, interrupts are off, and UART is idle... thats good :)...

	; set txbusy flag
	sbr		status,				(1<<TX_BUSY)

	; clear the output (start bit)
	cbi		TX_PORT,				TX_BIT

	; load the uart buffer...
	mov		buffer,				putchar_input

	;rjmp		UART_BEGIN_TRANSFER
#ifdef UART_PUTCHAR_RETADDR
	sbr		status,				(1<<PUTCHAR_SPECIAL_RETURN)
#endif
#endif

UART_BEGIN_TRANSFER:
	clr		tempA
	out		TIMER_TCNT,			tempA
	ldi		tempA,				START_BIT_LENGTH ; set timer reload value to 1.5 bit length
	out		TIMER_OCR,			tempA

	; clear timer compare flag
	ldi		tempA,				(1<<TIFR_BIT)
	out		TIMER_TIFR,			tempA

	; enable timer compare interrupt
	in			tempA,				TIMER_TIMSK
	sbr		tempA,				(1<<TIMSK_BIT)
	out		TIMER_TIMSK,		tempA

	; disable external interrupt
	in			tempA,				EXTINTMSK
	cbr		tempA,				(1<<EXTINTBIT)
	out		EXTINTMSK,			tempA

	; erase bit counter (set all bits)	 
	ser		bit_counter

#ifdef UART_PUTCHAR_RETADDR
	sbrs		status,				PUTCHAR_SPECIAL_RETURN
	rjmp		STANDARD_RETURN
	sei
	rjmp	UART_PUTCHAR_RETADDR
STANDARD_RETURN:
#endif

	; return as though this is an interrupt... it might not be, but
	; thats ok.  interrupts are disabled, and we want them enabled.
	;rjmp		UART_RETI

UART_RETI:
	out		_SFR_IO_ADDR(SREG), sreg_save
	reti

