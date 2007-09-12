/*
header ini khusus diggunakan untuk kristal 11.0592Mhz, karena perhitungannya pada nilai kristal tersebut

create by buron - ricky rawung - email : henry_ryq@yahoo.com



*/

#include <avr/io.h>             // Definisi register

#define     timer_prescaler 0x05    // tclck/1024

// definisi nilai tundaan untuk tiap fungsi
#define     timer_1000ms     0xD5CF  // 1s @ 11.0592Mhz
#define     timer_500ms      0xEAE7
#define     timer_50ms       0xFDE3
#define     timer_20ms       0xFE97
#define     timer_10ms       0xFF93
#define     timer_5ms        0xFFC9
#define     timer_1ms        0xFFF4

// declarasi fungsi 
void delay1000ms(void);
void delay500ms(void);
void delay50ms(void);
void delay20ms(void);
void delay10ms(void);
void delay5ms(void);
void delay1ms(void);


void delay1000ms()
{
    TCNT1 = timer_1000ms;
    
    TCCR1B = timer_prescaler;
    
    while(!(TIFR&(1<<TOV1)));

    TIFR = (1<<TOV1);
}

void delay500ms()
{
    TCNT1 = timer_500ms;
    
    TCCR1B = timer_prescaler;
    
    while(!(TIFR&(1<<TOV1)));

    TIFR = (1<<TOV1);
}

void delay50ms()
{
    TCNT1 = timer_50ms;
    
    TCCR1B = timer_prescaler;
    
    while(!(TIFR&(1<<TOV1)));

    TIFR = (1<<TOV1);
}

void delay20ms()
{
    TCNT1 = timer_20ms;
    
    TCCR1B = timer_prescaler;
    
    while(!(TIFR&(1<<TOV1)));

    TIFR = (1<<TOV1);
}

void delay10ms()
{
    TCNT1 = timer_10ms;
    
    TCCR1B = timer_prescaler;
    
    while(!(TIFR&(1<<TOV1)));

    TIFR = (1<<TOV1);
}

void delay5ms()
{
    TCNT1 = timer_5ms;
    
    TCCR1B = timer_prescaler;
    
    while(!(TIFR&(1<<TOV1)));

    TIFR = (1<<TOV1);
}

void delay1ms()
{
    TCNT1 = timer_1ms;
    
    TCCR1B = timer_prescaler;
    
    while(!(TIFR&(1<<TOV1)));

    TIFR = (1<<TOV1);
}
