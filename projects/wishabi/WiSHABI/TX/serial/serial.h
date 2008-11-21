/* Includes */
#include <avr/io.h>
#include <avr/interrupt.h>


/* Prototypes */
extern void USART_Init( unsigned int baudrate );
extern void USART_Transmit( unsigned char data );
extern void USART_Flush (void);



