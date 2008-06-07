/* UART Buffer Defines */
#define USART_RX_BUFFER_SIZE 32     /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART_TX_BUFFER_SIZE 32     /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART_RX_BUFFER_MASK ( USART_RX_BUFFER_SIZE - 1 )
#define USART_TX_BUFFER_MASK ( USART_TX_BUFFER_SIZE - 1 )
#if ( USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK )
	#error RX buffer size is not a power of 2
#endif
#if ( USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK )
	#error TX buffer size is not a power of 2
#endif

#ifndef P
#define P(s) ({static const char c[] __attribute__ ((progmem)) = s;c;})
#endif


#define USART_FRAME_ERROR      0x08              /* Framing Error by UART       */
#define USART_OVERRUN_ERROR    0x04              /* Overrun condition by UART   */
#define USART_BUFFER_OVERFLOW  0x02              /* receive ringbuffer overflow */
#define USART_NO_DATA          0x01              /* no receive data available   */

#define usart_puts_P(__s)       usart_puts_p(P(__s))


void usart_init(unsigned int);
unsigned char usart_getc(void);
void usart_putc(unsigned char);
void usart_puts(const char *);
void usart_puts_p(const char *);
void usart_puti( const int);
void usart_puthex_nibble(const unsigned char);
void usart_puthex_byte(const unsigned char);
unsigned char buffer_stat(void);


