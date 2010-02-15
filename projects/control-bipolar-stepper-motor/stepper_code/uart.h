
#ifndef UART_H
#define UART_H

// UART Buffer Defines
#define UART_RX_BUFFER_SIZE 32 // 2,4,8,16,32,64,128 or 256 bytes
#define UART_RX_BUFFER_MASK ( UART_RX_BUFFER_SIZE - 1 )
#if ( UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK )
  #error RX buffer size is not a power of 2
#endif

#define UART_TX_BUFFER_SIZE 64 // 2,4,8,16,32,64,128 or 256 bytes
#define UART_TX_BUFFER_MASK ( UART_TX_BUFFER_SIZE - 1 )
#if ( UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK )
  #error TX buffer size is not a power of 2
#endif
 // UDR Empty Interrupt
#define SET_UDRIE (UCSR0B |=  (1<<UDRIE0))
#define CLR_UDRIE (UCSR0B &= ~(1<<UDRIE0))

void InitUART(void);
void uart_SendByte(unsigned char data);
void uart_SendString(unsigned char Tab[]);
void uart_SendInt(int Tall);
void uart_FlushRxBuffer(void);

//! Buffer with received string from uart.
extern unsigned char UART_RxBuffer[UART_RX_BUFFER_SIZE];

#pragma vector=USART_RX_vect
__interrupt void UART_RX_interrupt( void );
#pragma vector=USART_UDRE_vect
__interrupt void UART_TX_interrupt( void );

#endif
