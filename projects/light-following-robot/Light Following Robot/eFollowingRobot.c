//White Line Following Robot using ATMEGA8 and LDR 
//LDR-1 and LDR-2 are Sensor_1 and Sensor_2 respectively.



#define F_CPU 1000000UL
#define forward 0x05;
#define right 0x01;
#define left 0x04;
//#define sensor_right  
//#define sensor_left 
#include <avr/io.h>
#include <util/delay.h>

void delayms(uint16_t millis) 
{
  while ( millis ) 
  {
    _delay_ms(1);
    millis--;
  }
}

void adc_init(void)
{
   
   ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz

   ADMUX |= (1 << REFS0); // Set ADC reference to AVCC
   ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading

   // No MUX values needed to be changed to use ADC0

   //ADCSRA |= (1 << ADFR);  // Set ADC to Free-Running Mode
   ADCSRA |= (1 << ADEN);  // Enable ADC
   ADCSRA |= (1 << ADSC);  // Start A2D Conversions
}

unsigned char adc_read(unsigned char ch)
{

    ADMUX &= 0xF8; // clear bottom 3 bits
    ADMUX |= ch; // then set bottom 3 bits to channel n 
	ADCSRA |= (1 << ADSC);  // Start A2D Conversions
	while(bit_is_set(ADCSRA,ADSC));
    
    return(ADCH);
}

int main(void)
{
  	unsigned char Sensor_1,Sensor_2;
 	 DDRB=0xFF;
  	 adc_init();
 	 delayms(50);
  	 Sensor_1=adc_read(5);
  	
  	 while(1)
  	 {  
  		Sensor_2=adc_read(4);
		if(Sensor_1==Sensor_2)
		{
			PORTB=forward;
		}
		else
		{
			if(Sensor_1>Sensor_2)
			{
				PORTB=left;
				
			}
			if(Sensor_1<Sensor_2)
			{
				PORTB=right;
				
			}
				
		}
				delayms(100);
}
	return(0);
 }  




