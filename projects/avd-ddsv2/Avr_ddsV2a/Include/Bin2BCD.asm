;BIN2BCD
;Converts a 24-bit binary number in Addr (AddrH,AddrM, AddrL) into 
;packed bcd in bcd2, bcd1, bcd0.  For example, if AddrH=BYTE3(12345),
;AddrM=high(12356), AddrL=low(12356)calling to the routine will 
;produce bcd2=$01, bcd1=$23, bcd0=$45.
;
bin2bcd:
	push	addrl					;save registers
	push	addrm
	push	addrh
	clc
	ldi		temp,24
	clr		bcd0
	clr		bcd1
	clr		bcd2
bitloop:
	rol		addrl
	rol		addrm
	rol		addrh
	rol		bcd0
	rol		bcd1
	rol		bcd2
	dec		temp
	brne	bitadj

	pop		addrh					;restore registers
	pop		addrm
	pop		addrl
	ret

bitadj:
	mov		temp2,bcd0
	rcall	adjbit
	mov		bcd0,temp2
	mov		temp2,bcd1
	rcall	adjbit
	mov		bcd1,temp2
	mov		temp2,bcd2
	rcall	adjbit
	mov		bcd2,temp2
	rjmp	bitloop

adjbit:
	mov		bcdt,temp2
	subi	bcdt,-3					;add 3
	sbrc	bcdt,3
	mov		temp2,bcdt
	mov		bcdt,temp2
	subi	bcdt,-(0x30)			;add 0x30
	sbrc	bcdt,7
	mov		temp2,bcdt
	ret
