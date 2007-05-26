
usart0_init_9600:

	ldi temp,$0c
	out ubrr0l,temp
	ldi temp,$00
	out ubrr0h,temp

	sbi ucsr0a,u2x0

	sbi ucsr0b,txen0
	sbi ucsr0b,rxen0

	ldi temp,$86
	out ucsr0c,temp

	ret

usart1_init_9600:

	ldi temp,$0c
	out ubrr1l,temp
	ldi temp,$00
	out ubrr1h,temp

	sbi ucsr1a,u2x1

	sbi ucsr1b,txen1
	sbi ucsr1b,rxen1

	ldi temp,$86
	out ucsr1c,temp

	ret

usart0_receive:
	sbis ucsr0a,rxc0
	rjmp usart0_receive
	in data,udr0
	ret


usart1_transmit:
	out udr1,data
usart1_transmita:
	sbis ucsr1a,udre1
	rjmp usart1_transmita
	ret



