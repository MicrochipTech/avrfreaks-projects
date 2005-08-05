/*
 * Doug's first independent AVR C test
 *
 * When the user presses a button, make the LED light up.
 * For the STK500. Connect PORTB to LEDs and PORTD to SWITCHes.
 */

#include <avr/io.h>

int main(void)
{
    unsigned char *in_port = (unsigned char*)&PIND;
    unsigned char *out_port = (unsigned char*)&PORTB;
    DDRD = 0; /* set for input. */
    DDRB = 0xff; /* enable as output */

    while(1)
        *out_port = *in_port;

    return 0;
}
