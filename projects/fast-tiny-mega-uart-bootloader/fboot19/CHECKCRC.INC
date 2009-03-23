;-------------------------------------------------------------------------
;				Check CRC16
;-------------------------------------------------------------------------
	rcall	getchar			; read CRC low
	rcall	getchar			; read CRC high
	or	crcl, crch		; now CRC = 0x0000 ?
	breq	main_ok			; yes
	movw	crch:crcl, zeroh:zerol	; clear CRC
	rjmp	main_error
;-------------------------------------------------------------------------
