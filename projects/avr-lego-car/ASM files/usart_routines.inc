INIT_USART:
push temp
ldi temp,0b00011000
out UCSRB,temp
ldi temp,25		;9600 for 4MHz
out ubrrl,temp
ldi temp,0b00000000
out ubrrh,temp
ldi temp,0b10000110
out UCSRC,temp
pop temp
ret

RECEIVE_BYTE:
wait_for_receive:
sbis UCSRA,RXC
rjmp wait_for_receive
in usart_byte,udr
ret

TRANSMITE_BYTE:
wait_for_free_port:
sbis UCSRA,5
rjmp wait_for_free_port
out UDR,usart_byte
ret
