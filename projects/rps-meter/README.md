# RPS meter

Uploaded by uttshakhan on 2012-11-03 15:44:00 (rating 0 out of 5)

## Summary

components:  

ATmega32,37segment display,IR pair,7805 IC ,resistors no BCD driver  

code(internal 8MHz clock):  

#include   

#include   

#include   

volatile uint16\_t i=0;


volatile uint16\_t j=0;  

volatile uint16\_t k=0;  

int sd(int z)  

{  

 if(z==0)  

 return 0b00111111;  

 else if(z==1)  

 return 0b00000110;  

 else if(z==2)  

 return 0b01011011;  

 else if(z==3)  

 return 0b01001111;  

 else if(z==4)  

 return 0b01100110;  

 else if(z==5)  

 return 0b01101101;  

 else if(z==6)  

 return 0b01111101;  

 else if(z==7)  

 return 0b00000111;  

 else if(z==8)  

 return 0b01111111;  

 else if (z==9)  

 return 0b01101111;  

 else  

 return 0b01001001;  

}  

int main (void)


{  

 DDRA&=(1<<1);  

 DDRC=DDRD=DDRB=0xFF;


 TCCR1B|=((1<<WGM12)|(1<<CS12)|(1<<CS10));


 OCR1A=7811;


 TIMSK|=(1<<OCIE1A); //Output compare 1A interrupt enable  

 sei();  

 ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); 


 ADMUX |= (1 << REFS0); // Set ADC reference to AVCC  

 //ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading


 // No MUX values needed to be changed to use ADC0


 //ADCSRA |= (1 << ADFR); // Set ADC to Free-Running Mode  

 ADCSRA |= (1 << ADEN); // Enable ADC  

 ADCSRA |= (1 << ADSC); // Start A2D Conversions  

if(PINA||(1<<1))  

 {for(;;) // Loop Forever  

 {  

 if(ADC > 512)  

 {


 if (i < 9) i++;  

 else {  

 i = 0;  

 j++;


 }  

 if(j>9) {j=0;k++;}  

 while(ADC>512)  

 {  

 ADCSRA |= (1 << ADSC); // Start A2D Conversions  

 };  

 }


 ADCSRA |= (1 << ADSC); // Start A2D Conversions


}}  

else  

{ {for(;;) // Loop Forever  

 {  

 if(ADC < 512)  

 {


 if (i < 9) i++;  

 else {  

 i = 0;  

 j++;


 }  

 if(j>9) {j=0;k++;}  

 while(ADC<512)  

 {  

 ADCSRA |= (1 << ADSC); // Start A2D Conversions  

 };  

 }


 ADCSRA |= (1 << ADSC); // Start A2D Conversions


}


}}


}  

ISR(TIMER1\_COMPA\_vect)  

{  

 //CPU Jumps here every 1 sec exactly!  

 PORTC=sd(i);  

 PORTD=sd(j);  

 PORTB=sd(k);


 i=j=k=0;  

}

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
