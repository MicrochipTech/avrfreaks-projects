
/***********Send byte in polling mode**********************
*@INPUT: argument
*/
rs232_send_byte:
	; Wait for empty transmit buffer
	sbis UCSRA,UDRE
	rjmp rs232_send_byte
	; Put data into buffer, sends the data
	out UDR,argument
ret
