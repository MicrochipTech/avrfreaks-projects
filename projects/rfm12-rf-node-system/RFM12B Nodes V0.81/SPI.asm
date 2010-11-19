







init_spi:
	sbi	SSpin			; make sure SS high
	sbi	ddrb,2			; ensure pin is an output
	ldi	temp1,0b01010000	; SPI on, MSb 1st, master, clk/4, idle low, xfer on +ve edge
	out	spcr,temp1		; write to config register
	ret



spi_send:
	push	temp1			; save data
	out	spdr,temp1		; write to TX register
spi_wait:
	in	temp1,spsr
	sbrs	temp1,SPIF
	rjmp	spi_wait
	pop	temp1			; restore data
	ret



