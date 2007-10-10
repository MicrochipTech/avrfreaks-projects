;****************************************
UART_RX_interrupt:
	in	temp,UDR	;receive data from PD1
	rcall	USART_TX
	ldi	flags,1
	reti
;****************************************
USART_Init:
	clr	temp2
	out	UBRRH, temp2
	ldi	temp, 25		; set up baud rate 9600
	out	UBRRL, temp		; @4 MHZ	
	
;use this one for interrupts
	ldi	temp,(1<<RXEN)|(1<<TXEN)|(1<<RXCIE)
;use this one without interrupts
;	ldi	temp, (1<<RXEN) | (1<<TXEN)	;Enable RX & TX
	out	UCSRB, temp				;set frame format	
	ldi	temp,(1<<URSEL) | (1<<USBS) | (3<<UCSZ0)	
	out	UCSRC, temp				;8 data bits, 2 stop bits
	ret
;****************************************
;send a byte to the serial port
USART_TX:
	sbis	UCSRA,UDRE		;wait for empty TX Buffer
	rjmp	USART_TX

	out	UDR, temp		;send data byte
	clr	flags
	ret
;****************************************
;receive a byte from serial port
USART_RX:
	sbis	UCSRA,RXC		;wait for data to be received
	rjmp	USART_RX

	in	temp,UDR		;get and return received byte from buffer
	ret
;****************************************
