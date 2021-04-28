# temperature controlled dc fan using atmega8

Uploaded by onkar_arj on 2021-04-28 14:28:00 (rating 0 out of 5)

## Summary

#include<avr/io.h>  

#include<util/delay.h>  

void  

init\_adc()  

{



    ADMUX = (1<<REFS0);


    ADCSRA = (1<<ADPS1) | (1<<ADPS2) | (1<<ADEN);


}


uint16\_t read\_adc(uint8\_t ch)


{


    init\_adc();


    ch = ch & 0b00000111;   

    ADMUX | = ch; ADCSRA | = (1<<ADSC);


    while(!(ADCSRA & (1<<ADIF)));



    ADCSRA | = (1<<ADIF);


    return (ADC);


}


int main(void)


{


    int x; init\_adc(); DDRB=0xFF;  

    while(1)


    {


        x = read\_adc(0); if (x>102.4)  

        PORTB = 0b00010000;


        else if (x<21)


        PORTB = 0b00000010;


        else


        PORTB = 0b00000000;


    }


    return 0;


}

## Compilers

- Atmel Studio 7

## Tags

- Complete code
- Atmel Studio 7
