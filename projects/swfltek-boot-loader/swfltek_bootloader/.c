#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

/* .eeprom section */
const uint8_t edata[] EEMEM ={0,1,2,3,4,5,6,7,8,9};

/* .text section */
int main(){
    int i, c;
    uint32_t p;

    MCUSR = 0;

    /*
	 * ===================================================
	 * Configure USART0, using the avr-libc 'setbaud' macros
	 * USART0 will be configured for BAUD/N/8/1
	 *
	 */
#define BAUD 38400
#include <util/setbaud.h>
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0A = 0;
#if USE_2X > 0
	UCSR0A = _BV(U2X0);
#endif
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);

    for(i=0;i<10;i++){
        c = eeprom_read_byte(&edata[i]);
        UDR0 = c + '0';
        do{
            c = UCSR0A & _BV(UDRE0);
        } while(c == 0);
    }

    UDR0 = 13;

    do{} while(1);
}
