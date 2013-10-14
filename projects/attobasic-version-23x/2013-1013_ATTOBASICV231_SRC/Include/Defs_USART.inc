;****************************************************************
;				UART SETUP
;****************************************************************

#if  FCLK == 1000000	;Set for 0.16% error
 .message "Using baud rate of 4800 for FCLK = 1MHz"
 .set	baudrate 		=  4800	;choose a baud rate
#elif  FCLK == 2000000	;Set for 0.16% error
 .message "Using baud rate of 9600 for FCLK = 2MHz"
 .set	baudrate 		=  9600	;choose a baud rate
#elif  FCLK == 4000000	;Set for 0.16% error
 .message "Using baud rate of 19200 for FCLK = 4MHz"
 .set	baudrate 		=  19200	;choose a baud rate
#else	;Set for 0.16% error
 .message "Using baud rate of 19200 for FCLK > 4MHz"
; .set	baudrate 		=  19200	;choose a baud rate
 .set	baudrate 		=  38400	;choose a baud rate
#endif
;
;these are redefs for Mega88/168 and Mega32U4 devices since the 
; USARTs have different names

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__)
	.equ	UART_PRR	= PRR
	.equ	UART_PRUART	= PRUSART0	;USART in PRR

	.equ	UBRRH		= UBRR0H
	.equ	UBRRL		= UBRR0L
	.equ	UDR			= UDR0

; UCSRxA - USART Control and Status Register A
	.equ	UCSRA		= UCSR0A
	;UCSR0A bits
	.equ	MPCM		= MPCM0
	.equ	U2X			= U2X0
	.equ	UPE			= UPE0
	.equ	DOR			= DOR0
	.equ	FE			= FE0
	.equ	UDRE		= UDRE0
	.equ	TXC			= TXC0
	.equ	RXC			= RXC0

; UCSRxB - USART Control and Status Register B
	.equ	UCSRB		= UCSR0B
	;UCSR0B bits
	.equ	TXB8		= TXB80
	.equ	RXB8		= RXB80
	.equ	UCSZ2		= UCSZ02
	.equ	TXEN		= TXEN0
	.equ	RXEN		= RXEN0
	.equ	UDRIE		= UDRIE0
	.equ	TXCIE		= TXCIE0
	.equ	RXCIE		= RXCIE0

; UCSRxC - USART Control and Status Register C
	.equ	UCSRC		= UCSR0C
	;UCSR0C bits
	.equ	UCPOL		= UCPOL0
	.equ	UCSZ0		= UCSZ00
	.equ	UCPHA		= UCPHA0
	.equ	UCSZ1		= UCSZ01
	.equ	UDORD		= UDORD0
	.equ	USBS		= USBS0
	.equ	UPM0		= UPM00
	.equ	UPM1		= UPM01
	.equ	UMSEL_0		= UMSEL00
	.equ	UMSEL_1		= UMSEL01

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#elif defined(__ATmega32U4__) || \
	defined(__AT90USB1286__) 
	.equ	UART_PRR	= PRR1
	.equ	UART_PRUART	= PRUSART1	;USART in PRR

	.equ	UBRRH		= UBRR1H
	.equ	UBRRL		= UBRR1L
	.equ	UDR			= UDR1

; UCSRxA - USART Control and Status Register A
	.equ	UCSRA		= UCSR1A
	;UCSR1A bits
	.equ	MPCM		= MPCM1
	.equ	U2X			= U2X1
	.equ	UPE			= UPE1
	.equ	DOR			= DOR1
	.equ	FE			= FE1
	.equ	UDRE		= UDRE1
	.equ	TXC			= TXC1
	.equ	RXC			= RXC1

; UCSRxB - USART Control and Status Register B
	.equ	UCSRB		= UCSR1B
	;UCSR1B bits
	.equ	TXB8		= TXB81
	.equ	RXB8		= RXB81
	.equ	UCSZ2		= UCSZ12
	.equ	TXEN		= TXEN1
	.equ	RXEN		= RXEN1
	.equ	UDRIE		= UDRIE1
	.equ	TXCIE		= TXCIE1
	.equ	RXCIE		= RXCIE1

; UCSRxC - USART Control and Status Register C
	.equ	UCSRC		= UCSR1C
	;UCSR1C bits
	.equ	UCPOL		= UCPOL1
	.equ	UCSZ0		= UCSZ10
	.equ	UCPHA		= UCPHA1
	.equ	UCSZ1		= UCSZ11
	.equ	UDORD		= UDORD1
	.equ	USBS		= USBS1
	.equ	UPM0		= UPM10
	.equ	UPM1		= UPM11
	.equ	UMSEL_0		= UMSEL10
	.equ	UMSEL_1		= UMSEL11

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#elif defined(__ATmega2560__)
	.equ	UART_PRR	= PRR0
	.equ	UART_PRUART	= PRUSART0	;USART in PRR

	.equ	UBRRH		= UBRR0H
	.equ	UBRRL		= UBRR0L
	.equ	UDR			= UDR0

; UCSRxA - USART Control and Status Register A
	.equ	UCSRA		= UCSR0A
	;UCSR1A bits
	.equ	MPCM		= MPCM0
	.equ	U2X			= U2X0
	.equ	UPE			= UPE0
	.equ	DOR			= DOR0
	.equ	FE			= FE0
	.equ	UDRE		= UDRE0
	.equ	TXC			= TXC0
	.equ	RXC			= RXC0

; UCSRxB - USART Control and Status Register B
	.equ	UCSRB		= UCSR0B
	;UCSR1B bits
	.equ	TXB8		= TXB80
	.equ	RXB8		= RXB80
	.equ	UCSZ2		= UCSZ02
	.equ	TXEN		= TXEN0
	.equ	RXEN		= RXEN0
	.equ	UDRIE		= UDRIE0
	.equ	TXCIE		= TXCIE0
	.equ	RXCIE		= RXCIE0

; UCSRxC - USART Control and Status Register C
	.equ	UCSRC		= UCSR0C
	;UCSR1C bits
	.equ	UCPOL		= UCPOL0
	.equ	UCSZ0		= UCSZ00
	.equ	UCPHA		= UCPHA0
	.equ	UCSZ1		= UCSZ10
	.equ	UDORD		= UDORD0
	.equ	USBS		= USBS0
	.equ	UPM0		= UPM00
	.equ	UPM1		= UPM01
	.equ	UMSEL_0		= UMSEL00
	.equ	UMSEL_1		= UMSEL01
;
#else
#error	"!! Invalid Target MCU !!"
#endif
