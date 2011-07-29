#include "larslib.h"

// Seconds delay, pure lazyness
void _delay_s(uint8_t sec)
{
    uint8_t i;
    for(i=0; i<sec; i++)
    {
        _delay_ms(250);
        _delay_ms(250);
        _delay_ms(250);
        _delay_ms(250);
    }
}

// Blinking function, 300ms per on/off 
void blink(uint8_t bl)
{
	//BLINKDDR |= (1<<BLINKPIN);
    uint8_t i;
    for(i=0; i<bl; i++)
    {
        _delay_ms(150);
        BLINKPORT ^= (1<<BLINKPIN);
        _delay_ms(150);
        BLINKPORT ^= (1<<BLINKPIN);
    }
}

// This debug function outputs a byte
//  as pulses for reading through the scope
void scopeout(uint8_t byte)
{
    uint8_t i;
    for(i=0; i<8; i++)
    {
        BLINKPORT |= (1<<BLINKPIN);
        if( byte & (1<<i) ) _delay_ms(1);
        _delay_ms(1);
        BLINKPORT &= ~(1<<BLINKPIN);
        _delay_ms(1);
    }

}

/*
// Output voltage for debugging purposes (range 1 - 50)
// (Uses 40% of available memory for some reason)
void outvolt(uint8_t volt)
{ 
    uint8_t i, neg;
    
    if(volt>50) volt = 50;

    neg = 50 - volt;
     
    for(i=0; i<100; i++) // 5 sec
    {
        BLINKPORT |= (1<<BLINKPIN);
        _delay_ms(volt);
        BLINKPORT &= ~(1<<BLINKPIN);
        _delay_ms(neg);
    }
}
*/
// TODO: Lage en funksjon som skriver ut "tall" til voltmeteret
//  via PWM?

