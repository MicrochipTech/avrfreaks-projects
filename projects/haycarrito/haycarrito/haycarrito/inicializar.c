#include "inicializar.h"
#include <avr/io.h>

void espera(void)
{
    TCCR1B = 0b00001100;
    OCR1A = 64000;
    while ((TIFR & (1 << OCF1A)) == 0) {}
    OCR1A = 0;
    TIFR = (1 << OCF1A);
}

void inicializar_pwm(void)
{
    DDRB |= (1 << 5);
    DDRB |= (1 << 6);
    DDRE |= (1 << 3);
    DDRE |= (1 << 4);
    PORTB |= (1 << 5);
    PORTB |= (1 << 6);
    PORTE |= (1 << 3);
    PORTE |= (1 << 4);
    ICR1 = 65530;
    OCR1A = 0;
    OCR1B = 0;
    OCR3A = 0;
    OCR3B = 0;
    TCCR1B = 0b00010010;
    TCCR1A = 0b11110000;
    TCCR3B = 0b00010010;
    TCCR3A = 0b11110000;
}
