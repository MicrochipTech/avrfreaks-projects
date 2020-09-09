

/***********************************************************************************/
//Only include if using Atmega 32 or device with similar UART

#if(mmcu == atmega32)
#define UCSR0B UCSRB
#define RXEN0 RXEN
#define UDRE0 UDRE
#define UBRR0H UBRRH
#define UBRR0L UBRRL
#define UDR0 UDR
#define UCSR0A UCSRA
#define U2X0 U2X
#define TIMSK2 TIMSK
#define TIMSK1 TIMSK
#define TCR2BUB TCR2UB
#define TCCR2A TCCR2
#define TCCR2B TCCR2
#define TIFR0 TIFR
#define TIFR1 TIFR
#define  RXC0 RXC
#define  TCCR0B TCCR0
#define TXEN0 TXEN
#define WDTCSR WDTCR
#define WDCE WDTOE
#define UCSR0C UCSRC
#define UCSZ00 UCSZ0
#define UCSZ01 UCSZ1
#define TCR2AUB TCR2UB
#endif
#define URSEL 7
/*************************************************************************************/

#include "Cal_resources/Basic_HW_setup.h"
#include "Cal_resources/Basic_IO_and_timer_subs.c"
#include "Cal_resources/Cal_subroutines.c"









	


