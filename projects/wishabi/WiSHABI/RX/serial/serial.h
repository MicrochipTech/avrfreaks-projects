/* Includes */
#include <avr/io.h>
#include <avr/interrupt.h>

/////////////////////////////////////////////////////////////////////////////
//The following Timer/Counter #defines included to allow a timeout option
//in USART_Receive so that code execution will not get stuck waiting for
//a byte part way through receiving a packet.
/////////////////////////////////////////////////////////////////////////////
//Value of counter is found in register TCNT1 - Timer/Counter 1
#define TIMER TCNT1
//With Prescaler set to F/1024, One increment On TIMER = 1/11719 Of A Second
#define TEN_MILLISECONDS 117
//////////////////////////////////////////////////////////////////////////////

/* Prototypes */
extern void USART_Init( unsigned int baudrate );
extern void USART_Transmit( unsigned char data );
extern void USART_Flush (void);

//To Be Removed Eventually.  Only Used for Debugging.
//Final version, WiSHABI_RX will not need USART Transmission capability.
extern unsigned char USART_Receive( void );
extern void SendInitConfirmation (void);
