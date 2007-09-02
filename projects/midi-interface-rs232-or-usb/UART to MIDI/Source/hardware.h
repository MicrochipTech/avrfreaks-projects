#ifndef HARDWARE_H
#define HARDWARE_H
#include <avr/io.h>

// LEDs for showing ringbuffer-usage (LEDs low-active)
    #define LED_PORT        PORTD
    #define LED1            PD5         // green    started using buffer
    #define LED2            PD6         // yellow   buffer reached half 
    #define LED3            PD7         // red      buffer overflowed, midi data lost

// Software-UART for communication with PC
// Must use ICP and OC1A
    #define SRX     PB0     // ICP 
    #define SRXPIN  PINB

    #define STX     PB1     // OC1A
    #define STXDDR  DDRB

    #define	SUART_BAUD	38400
    
    
// Hardware-UART for MIDI-communication
	#define MIDI_RXD				PD0
	#define MIDI_RXD_DDR			DDRD
	#define MIDI_RXD_PORT			PORTD
	#define MIDI_RXD_PIN			PIND
	
	#define MIDI_TXD				PD1
	#define MIDI_TXD_DDR			DDRD
	#define MIDI_TXD_PORT			PORTD
	#define MIDI_TXD_PIN			PIND
	
	#define MIDI_USE_UART			1
	//#define MIDI_USE_UART0		1
	
	// Definitions for devices with only one UART.
	#ifdef MIDI_USE_UART
		#define MIDI_UDR	UDR
		#define	MIDI_UBRRH	UBRRH
		#define MIDI_UBRRL	UBRRL
		#define MIDI_UCSRA	UCSRA
		#define MIDI_UCSRB	UCSRB
		#define MIDI_UCSRC	UCSRC
		#define MIDI_UDRIE	UDRIE
		
		// UCSRA Stuff	(RXC,TXC,UDRE,FE,DOR,PE,U2X,MPCM)
		#define MIDI_RXC		RXC
		#define MIDI_TXC		TXC
		#define MIDI_UDRE		UDRE
		
		// UCSRB Stuff (RXCIE,TXCIE,UDRIE,RXEN,TXEN,UCSZ2,RXB8,TXB8)
		#define MIDI_RXCIE	RXCIE
		#define MIDI_TXCIE	TXCIE
		#define MIDI_RXEN		RXEN
		#define MIDI_TXEN		TXEN	
		
		// UCSRC Stuff (URSEL,UMSEL,UPM1,UPM0,USBS,UCSZ1,UCSZ0,UCPOL)
		#define MIDI_URSEL	URSEL
		#define MIDI_UMSEL	UMSEL
		#define MIDI_UPM1		UPM1
		#define MIDI_UPM0		UPM0
		#define MIDI_USBS		USBS
		#define MIDI_UCSZ1	UCSZ1
		#define MIDI_UCSZ0	UCSZ0
		#define MIDI_UCPOL	UCPOL	

		// Interruptvectors
		#define MIDI_SIG_UART_RECV	SIG_UART_RECV
		#define MIDI_SIG_UART_DATA	SIG_UART_DATA
		#define MIDI_SIG_UART_TRANS	SIG_UART_TRANS
	#endif 

	// Definitions for Devices with multiple UARTs, UART0.
	#ifdef MIDI_USE_UART0
		#define MIDI_UDR		UDR0
		#define	MIDI_UBRRH	UBRR0H
		#define MIDI_UBRRL	UBRR0L
		#define MIDI_UCSRA	UCSR0A
		#define MIDI_UCSRB	UCSR0B
		#define MIDI_UCSRC	UCSR0C
		#define MIDI_UDRIE	UDRIE0
		
		// UCSRA Stuff	(RXC,TXC,UDRE,FE,DOR,PE,U2X,MPCM)
		#define MIDI_RXC		RXC0
		#define MIDI_TXC		TXC0
		#define MIDI_UDRE		UDRE0
		
		// UCSRB Stuff (RXCIE,TXCIE,UDRIE,RXEN,TXEN,UCSZ2,RXB8,TXB8)
		#define MIDI_RXCIE	RXCIE0
		#define MIDI_TXCIE	TXCIE0
		#define MIDI_RXEN		RXEN0
		#define MIDI_TXEN		TXEN0	
		
		// UCSRC Stuff (URSEL,UMSEL,UPM1,UPM0,USBS,UCSZ1,UCSZ0,UCPOL)
		#define MIDI_URSEL	URSEL0
		#define MIDI_UMSEL	UMSEL0
		#define MIDI_UPM1		UPM01
		#define MIDI_UPM0		UPM00
		#define MIDI_USBS		USBS0
		#define MIDI_UCSZ1	UCSZ01
		#define MIDI_UCSZ0	UCSZ00
		#define MIDI_UCPOL	UCPOL0	

		// Interruptvectors
		#define MIDI_SIG_UART_RECV	SIG_USART0_RECV
		#define MIDI_SIG_UART_DATA	SIG_USART0_DATA
		#define MIDI_SIG_UART_TRANS	SIG_USART0_TRANS
	#endif 
	 

/* Definitions for init
 * ============================
 */
	// DDRD = 1 = Output
	#define DDRB_INIT			0b00000010
	#define PORTB_INIT			0b11111111
	#define DDRC_INIT			0b00000000
	#define PORTC_INIT			0b11111111
	#define DDRD_INIT			0b11100010
	#define PORTD_INIT			0b11111111


#endif

