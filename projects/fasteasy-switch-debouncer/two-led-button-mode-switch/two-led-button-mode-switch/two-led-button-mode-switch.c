/*
Full write-up here:

http://www.friday.com/bbum/2008/04/05/using-a-vertical-stack-counter-to-debounce-switches/

*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define bit_set_8(var, mask)   ((var) |= (uint8_t)(mask))
#define bit_clear_8(var, mask)   ((var) &= (uint8_t)~(mask))
#define bit_toggle_8(var, mask)   ((var) ^= (uint8_t)(mask))
#define bit_read_8(var, mask)   ((var) & (uint8_t)(mask))

#define BIT(x)   (1 << (x))

volatile unsigned char ledState = 0;

static unsigned char debounceB0,debounceB1,debouncedBState;
// vertical stacked counter based debounce
unsigned debounceB(unsigned char sample)
{
    unsigned char delta, changes;
    
    // Set delta to changes from last sample
    delta = sample ^ debouncedBState;
    
    // Increment counters
    debounceB1 = debounceB1 ^ debounceB0;
    debounceB0  = ~debounceB0;
    
    // reset any unchanged bits
    debounceB0 &= delta;
    debounceB1 &= delta;
    
    // update state & calculate returned change set
    changes = ~(~delta | debounceB0 | debounceB1);
    debouncedBState ^= changes;
    
    return changes;
}

// timer 0 overflow interrupt handler
ISR(TIMER0_OVF_vect) {
    unsigned char sample = PINC;
    unsigned char changes = debounceB(sample);
    
    if (bit_read_8(changes, BIT(PC0)) &&
        (!bit_read_8(sample, BIT(PC0)))) {
        ledState ++;
        ledState = ledState % 8;
    }
    if (bit_read_8(changes, BIT(PC1)) &&
        (!bit_read_8(sample, BIT(PC1)))) {
        ledState --;
        ledState = ledState % 8;
    }
}

int main(void) {
    DDRB = 0xFF; // Make all PB* -- PORT B -- pins output.
    PORTB = 0xFF; // turn all PB* -- PORT B -- pins off.
    
    bit_clear_8(DDRC, BIT(PC0) | BIT(PC1)); // PC0, PC1 is an input
    bit_set_8(PORTC, BIT(PC0) | BIT(PC1)); // Turn on pull-up resistor on PC0, PC1
    
    bit_set_8(TCCR0B, BIT(CS01));
    bit_set_8(TIMSK0, BIT(TOIE0)); // turn on overflow interrupt for timer 0
    
    sei(); // turn on interrupts
    
    while (1) {
        PORTB = ~ledState;
    }
}
