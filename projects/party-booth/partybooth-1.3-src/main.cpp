#include "common.h"
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

static const uint8_t LED = PB0;
static const uint8_t BTN = PB1;
static const uint8_t BZZ = PB2;
static const uint8_t SHT = PB3;

static uint8_t state = 0;
static uint16_t counter = 0;

ISR(TIM0_COMPA_vect) {
    OCR0A ^= 1;
    ++counter;
    switch (state) {
        case 2:
        case 4:
        case 6:
        case 8:
            if (counter >= 3200) { // 3200 tics = 800ms
                counter = 0;
                ++state;
                PORTB |= 1 << LED;
            }
            break;
        case 1:
        case 3:
        case 5:
        case 7:
            if (counter >= 800) { // 800 tics = 200ms
                counter = 0;
                ++state;
                PORTB &= ~((1 << LED) | (1 << BZZ));
            } else {
                PORTB ^= 1 << BZZ;
            }
            break;
        case 9:
            if (counter >= 4000) { // 4000 tics = 1s
                counter = 0;
                ++state;
                PORTB &= ~((1 << LED) | (1 << BZZ));
                PORTB |= 1 << SHT;
            } else {
                PORTB ^= 1 << BZZ;
            }
            break;
        case 10:
            if (counter >= 2000) {
                counter = 0;
                state = 0;
                PORTB &= ~(1 << SHT);
            }
            break;
    }
}

ISR(INT0_vect) {
    counter = 0;
    state = 1;
    PORTB &= ~(1 << SHT);
    PORTB |= 1 << LED;
}

int main() {
    DDRB |= (1 << LED) | (1 << BZZ) | (1 << SHT); // LED, BZZ, SHT output
    // Setup timer 0 for a frequency of 4kHz
    TCCR0A = 2; // top = OCRA
    TCCR0B = 2; // /8 prescale in addition to the /8 system clock prescale
    OCR0A = 37; // alternate between 36 and 37 (the correct count is 36.5)
    TCNT0 = 0;
    TIMSK0 = 4; // Enable interrupt for compare match A

    // Setup BTN interrupt
    MCUCR |= 3; // interrupt on the rising edge
    GIMSK |= 1 << INT0; // enable interrupt from INT0

    // Now enable the interrupts, then go to sleep and let the interrupt
    // handlers do the rest
    sei();
    while (true) {
        sleep_mode();
    }
}
