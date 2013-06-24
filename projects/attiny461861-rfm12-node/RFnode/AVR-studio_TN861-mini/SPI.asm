



init_spi:
	sbi	SSpin			; make sure SS high
	cbi	porta,2			; always leave/start sck line in LOW
	cbi	porta,1			; and data line LOW
	ldi	temp1,1
	out	USIPP,temp1		; moves USI to port-A pins (0,1,2)
	ret


spi_send:
	push	temp2
	out	USIDR,temp1
	ldi	temp1,(1<<USIWM0)|(0<<USICS0)|(1<<USITC)
	ldi	temp2,(1<<USIWM0)|(0<<USICS0)|(1<<USITC)|(1<<USICLK)

	out	USICR,temp1	; MSB
	nop
	out	USICR,temp2
	nop

	out	USICR,temp1
	nop
	out	USICR,temp2
	nop

	out	USICR,temp1
	nop
	out	USICR,temp2
	nop

	out	USICR,temp1
	nop
	out	USICR,temp2
	nop

	out	USICR,temp1
	nop
	out	USICR,temp2
	nop

	out	USICR,temp1
	nop
	out	USICR,temp2
	nop

	out	USICR,temp1
	nop
	out	USICR,temp2	; LSB
	nop

	out	USICR,temp1
	nop
	out	USICR,temp2
	nop

	in	temp1,USIDR
	cbi	porta,2		; always leave sck line in LOW
	cbi	porta,1		; and data line LOW
	pop	temp2
	ret
