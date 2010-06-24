
;RS232 USART module
;
;
;(c)2010 TS-Labs inc.


//----------------- Setup -----------------

#define		ubrr	(f_clk*1000/16/baud-1)


//----------------- Main -----------------

U_INIT:		;USART Initialisation
			ldi a,high(ubrr)				;Set Baud Rate
			sts UBRR1H,a
			ldi a,low(ubrr)
			sts UBRR1L,a

			ldi a,(1<<RXCIE1)|(1<<RXEN1)|(1<<TXEN1)		;Enable Rx and Tx, RXC Interrupt
			sts UCSR1B,a

			ldi a,(3<<UCSZ10)				;Set frame format: 8 data, 1 stop bit
			sts UCSR1C,a
			
			ret

U_WT:		;USART Wait Tx Ready
UW1:		lds r0,UCSR1A				;Wait for empty transmit buffer
			sbrs r0,UDRE1
			rjmp UW1
			ret

U_MSG:		;USART Send Message
UM2:
			call U_WT
			lpm a,Z+
			tst a
			breq UM1
			sts UDR1,a					;Put data (r16) into buffer, sends the data
			rjmp UM2
UM1:
			ret

U_RX:		;USART Rx INT Handler
			push a
			lds a,UDR1
			sts UDR1,a
			pop a
			reti

M_INTRO:
			.db $d,"AVR MOD Player, v.003",$d,"(c)2010  TS-Labs inc.",$d,$d
#if			chip==16
			.db "ATmega16, 128kB ROM, 4kB RAM",$d,$d,0,0
#elif		chip==128
			.db "ATmega128, 128kB ROM, 4kB RAM",$d,$d,0
#endif

M_HELP:
			.db "Help on commands:",$d
			.db "PLAY STOP INFO MOD CH<n>+/- LIST",$d,$d,0,0
