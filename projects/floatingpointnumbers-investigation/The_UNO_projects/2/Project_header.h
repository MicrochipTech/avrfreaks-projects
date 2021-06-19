
#include <avr/wdt.h>

char keypress;                                                 
 
#define positive 1
#define negative 0



/***************************************************************************************************/
#define setup_328_HW \
setup_watchdog;\
ADMUX |= (1 << REFS0);\
initialise_IO;\
Serial.begin(38400);\
while (!Serial);



/***************************************************************************************************/
#define initialise_IO \
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFF;\
PORTD = 0xFF;\




/***************************************************************************************************/
#define wdr()  __asm__ __volatile__("wdr")

#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define SW_reset {wdt_enable(WDTO_30MS);while(1);}



/******************************************************************************************************/
#include "Resources_UNO_template\ASKII_subroutines_V1.c"
#include "Resources_UNO_template\FPN_subroutines.c"
