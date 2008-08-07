/************************************************************************/
/*                                                                      */
/*                      Interrupt driven buffered UART                  */
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                                                                      */
/************************************************************************/
#ifndef _uart0_h_
#define _uart0_h_

					// size must be in range 2 .. 256
#define RX0_SIZE	10		// usable: RX0_SIZE + 2 (4 .. 258)
#define TX0_SIZE	8		// usable: TX0_SIZE + 1 (3 .. 257)

#define	uputs0(x)	uputs0_((u8*)(x))	// avoid char warning


u8 ukbhit0( void );			// 0 = rx buffer empty
u8 ugetchar0( void );			// get received byte
u8 utx0_ready( void );			// 0 = tx still busy
void uputchar0( u8 c );			// send byte
void uputs0_( u8 *s );			// send string from SRAM
void init_uart0( u16 bauddivider );


#define	UTX0_IEN	SBIT( UCSR0B, UDRIE0 )
#define	URX0_IEN	SBIT( UCSR0B, RXCIE0 )


/******************************	Resolve AVR naming chaos ****************/

//-----------------------------	Interrupt vectors -----------------------/
#ifndef	USART0_RX_vect
#if defined 	USART_RX_vect
#define		USART0_RX_vect	USART_RX_vect
#elif defined	USART_RXC_vect
#define		USART0_RX_vect	USART_RXC_vect
#elif defined	USART0_RXC_vect
#define		USART0_RX_vect	USART0_RXC_vect
#endif
#endif

#if !defined USART0_UDRE_vect && defined USART_UDRE_vect
#define	USART0_UDRE_vect USART_UDRE_vect
#endif

//-----------------------------	Register names --------------------------/
#ifndef	UCSR0A
#define	UCSR0A	UCSRA
#endif
#ifndef	UCSR0B
#define	UCSR0B	UCSRB
#endif
#ifndef	UCSR0C
#define	UCSR0C	UCSRC
#endif
#ifndef	UDR0
#define	UDR0	UDR
#endif
#ifndef	UBRR0L
#define	UBRR0L	UBRRL
#endif
#ifndef	UBRR0H
#define	UBRR0H	UBRRH
#endif

//-----------------------------	Bit names -------------------------------/
#ifndef	UCSZ00
#define	UCSZ00	UCSZ0
#endif
#ifndef	UCSZ01
#define	UCSZ01	UCSZ1
#endif
#if !defined URSEL0 && defined URSEL
#define	URSEL0	URSEL
#endif
#ifndef	RXEN0
#define	RXEN0	RXEN
#endif
#ifndef	TXEN0
#define	TXEN0	TXEN
#endif
#ifndef	UDRIE0
#define	UDRIE0	UDRIE
#endif
#ifndef	RXCIE0
#define	RXCIE0	RXCIE
#endif

#endif
