.CSEG
.ORG	URXCaddr
	rjmp	UART_RX_C

.ORG	UTXCaddr
	rjmp	UART_TX_C

.ORG	UDREaddr
	rjmp	UART_DR_E

.EXIT
