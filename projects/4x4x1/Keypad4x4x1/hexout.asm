; Outputs a hexadecimal number to RS-232
; Uses, trashes, TxByte

PUT_HEX:
	push TxByte
	lsr TxByte
	lsr TxByte
	lsr TxByte
	lsr TxByte
	rcall HEX_SHIFT
	subi TxByte, $D0
	rcall PUT_CHAR

	pop TxByte
	andi TxByte, $0F
	rcall HEX_SHIFT
	subi TxByte, $D0
	rcall PUT_CHAR
ret

HEX_SHIFT:	; Moves from funny hex to real readable hex
	cpi TxByte, 10
	brlo HEX_SHIFTa
	subi TxByte, $F9
HEX_SHIFTa:
ret
