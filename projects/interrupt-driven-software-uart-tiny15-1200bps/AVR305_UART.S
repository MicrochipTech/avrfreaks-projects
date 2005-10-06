#include <avr/io.h>

#define RX_PORT	_SFR_IO_ADDR(PORTB)
#define RX_BIT		PB2
#define RX_PIN		_SFR_IO_ADDR(PINB)

#define TX_PORT	_SFR_IO_ADDR(PORTB)
#define TX_BIT		PB4
#define TX_DDR		_SFR_IO_ADDR(DDRB)

#define DATA_BITS 8
#define STOP_BITS 1

#define DATA_BITS_MASK (0xFF >> (8-DATA_BITS))

#define CLK				1600000
#define BAUD			19200
#define DELAY_C		( CLK / BAUD )
#define DELAY_COUNT	( (DELAY_C - 23) / 6 )
#if DELAY_COUNT <= 0
#error Baud rate too high for this clock speed
#elif DELAY_COUNT > 255
#error Baud rate too low for this clock speed
#else
#define ERRORCYCLES			(CLK - (6*DELAY_COUNT + 23)*BAUD)
#if ERROR < 0
#define ABSERR (-ERRORCYCLES)
#else
#define ABSERR ERRORCYCLES
#endif
#if (ABSERR*100) > (3*CLK)
#warning Greater than 3% error is bad...
#endif
#define ERROR	ABSERR*1600 / CLK
#endif

#define bitcount	r16
#define temp		r17
#define txByte		r18
#define rxByte		r19


;***************************************************************************
;*
;* "putchar"
;*
;* This subroutine transmits the byte stored in the "Txbyte" register
;* The number of stop bits used is set with the sb constant
;*
;* Number of words	:14 including return
;* Number of cycles	:Depens on bit rate
;* Low registers used	:None
;* High registers used	:2 (bitcount, Txbyte)
;* Pointers used	:None
;*
;***************************************************************************

putchar:	
		ldi	bitcount,	1+DATA_BITS+STOP_BITS	;start, data, stop
		com	txByte						;Inverte everything

		#if DATA_BITS < 8
		andi	txByte,		DATA_BITS_MASK
		#endif

		sec									;Start bit

putchar0:	
		brcc	putchar1						;If carry set
		cbi	TX_PORT,		TX_BIT		;    send a '0'
		rjmp	putchar2						;else	

putchar1:	
		sbi	TX_PORT,		TX_BIT		;    send a '1'
		nop

putchar2:	
		rcall UART_delay					;One bit delay
		rcall UART_delay

		lsr	txByte						;Get next bit
		dec	bitcount						;If not all bit sent
		brne	putchar0						;   send next
												;else
		ret									;   return

;***************************************************************************
;*
;* "getchar"
;*
;* This subroutine receives one byte and returns it in the "Rxbyte" register
;*
;* Number of words	:14 including return
;* Number of cycles	:Depens on when data arrives
;* Low registers used	:None
;* High registers used	:2 (bitcount, Rxbyte)
;* Pointers used	:None
;*
;***************************************************************************

getchar:
		ldi 	bitcount,	DATA_BITS+1	;8 data bit + 1 stop bit

getchar1:
		sbic 	RX_PIN,		RX_BIT		;Wait for start bit
		rjmp 	getchar1

		rcall UART_delay					;0.5 bit delay

getchar2:
		rcall UART_delay					;1 bit delay
		rcall UART_delay		

		clc									;clear carry
		sbic 	RX_PIN,		RX_BIT		;if RX pin high
		sec									;   set carry

		dec 	bitcount						;If bit is stop bit
		breq 	getchar3						;   return
												;else
		ror 	rxByte						;   shift bit into Rxbyte
		rjmp 	getchar2						;   go get next

getchar3:
		#if DATA_BITS < 8
		lsr	rxByte
		#endif
		#if DATA_BITS < 7
		#warning < 7 data bits didnt work for me... good luck...
		lsr	rxByte
		#endif
		#if DATA_BITS < 6
		lsr	rxByte
		#endif
		ret


;***************************************************************************
;*
;* "UART_delay"
;*
;* This delay subroutine generates the required delay between the bits when
;* transmitting and receiving bytes. The total execution time is set by the
;* constant "DELAY_COUNT":
;*
;*	3·DELAY_COUNT + 7 cycles (including rcall and ret)
;*
;* Number of words	:4 including return
;* Low registers used	:None
;* High registers used	:1 (temp)
;* Pointers used	:None
;*
;***************************************************************************

UART_delay:
		ldi	temp,			DELAY_COUNT
UART_delay1:
		dec	temp
		brne	UART_delay1

		ret

;***** Program Execution Starts Here

;***** Test program

.global main
main:
		; calibrate
		ldi	temp,			0x60
		out	_SFR_IO_ADDR(OSCCAL),temp

		sbi	TX_PORT,		TX_BIT		;Init port pins
		sbi	TX_DDR,		TX_BIT

		ldi	txByte,		12				;Clear terminal
		rcall	putchar

		; ERROR is fixed point percent error as 4.4 
		ldi	txByte,		(ERROR >> 4)
		rcall outhexnibble
		ldi	txByte,		(ERROR & 0xF)
		rcall outhexnibble
		ldi	txByte,		(DELAY_COUNT >> 4)
		rcall outhexnibble
		ldi	txByte,		(DELAY_COUNT & 0xF)
		rcall outhexnibble

forever:
		rcall	getchar
		mov	txByte,		rxByte
		rcall	putchar						;Echo received char
		rjmp	forever

outhexnibble:
		andi	txByte,		0x0F
		cpi	txByte,		0x0A
		brlt	ADD_TX_0
		ldi	temp,			('A'-0x0A)
		add	txByte,		temp
		rjmp	putchar
		ADD_TX_0:
		ldi	temp,			'0'
		add	txByte,		temp
		rjmp putchar

