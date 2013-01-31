#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include<compat/deprecated.h>
#include <avr/sleep.h>

unsigned int read_adc(unsigned char ch);
unsigned int convertadc3accurate(unsigned int val);

void init_timer(void);
void stop_timer(void);

unsigned int pwm=10;
unsigned int adc_val, cal_adc_int;
float f_adc_val;

// function starts from here

unsigned int convertadc3accurate(unsigned int val)
{
	f_adc_val = val/1023.0*5;
	cal_adc_int = f_adc_val * 100;
	return cal_adc_int;

}


unsigned int read_adc(unsigned char ch)
{

	ADMUX = ch;						// Select pin ADC0 using MUX		
	ADCSRA |= _BV(ADSC);			// Start conversion 		
	while (ADCSRA & _BV(ADSC) ) 	// wait until converstion completed
	{
	}
	return ADCW;
}

void init_timer(void)
{
	
	OCR1A = pwm; 
	//OCR1B = 0;
    //OCR2 = 0;
   
	//TCCR2 = 0x7A;
	TCCR1A = 0xB1;
	TCCR1B = 0x0A;
	TIMSK = (1<<TOIE1); 

}

void stop_timer(void)
{
	TCCR1A = 0x00;
}
ISR (TIMER1_OVF_vect) 
{ 
   // static uint16_t pwm;      //pwm counter var 
     
				//LED3 OFF

	if (bit_is_clear(PIND, 1))	//IF SW3 IS PRESSED
		{
			do
			{
//				adc_val = read_adc(0);
//				adc_val = convertadc3accurate(adc_val);
//				if(adc_val <= 300)
//				{
					
					if (pwm >= 10 && pwm <= 254)
					{	
						pwm=pwm+1;
                        if(pwm>254) pwm=254;
					}
//				}
						
				OCR1A = pwm; 
				_delay_ms(10);
			}while(bit_is_clear(PIND, 1));
			_delay_ms(10);
		}
     if (bit_is_clear(PIND, 2))	//IF SW3 IS PRESSED
		{       
         	do
			{
				pwm = pwm-1;
				if(pwm < 10)
				{
					pwm = 10;
				}
			OCR1A = pwm;
			_delay_ms(10);
			}while(bit_is_clear(PIND, 2));
			_delay_ms(10);
		 
		 }   

    else
    {
        OCR1A = pwm;
        _delay_ms(10);
    }
    
} 

int main(void)
{
	ADCSRA = _BV(ADEN) | _BV(ADPS2);    // Activate ADC with Prescaler 16 --> 1Mhz/16 = 62.5kHz
	
	DDRB=0x01;						//SET DATA DIRECTION REGISTER
	DDRC = 0x00;					//SET 1 for OUTPUT PORT
									//SET 0 FOR INPUT PORT
									//ONLY PORTB.0 IS OUTPUT
									//CONFIGURE AS PER REQUIREMENT
									//DEFAULT ALL OTHERS ARE INPUT
									//CHANGE ACCORDING TO USE
	
	DDRD=0xF0;						//SET DATA DIRECTION REGISTER
									//SET 1 for OUTPUT PORT
									//SET 0 FOR INPUT PORT
									//ONLY PORTD.7 AND PORTD.6 ARE OUTPUT
									//CONFIGURE AS PER REQUIREMENT
									//DEFAULT ALL OTHERS ARE INPUT
									//CHANGE ACCORDING TO USE
	
	sbi(PORTB,0);					//LED1 ON (INDICATION FOR READY TO USE)
	
	sbi(PORTD,0);					//ENABLE PULL UP FOR SW1
	sbi(PORTD,1);					//ENABLE PULL UP FOR SW2
	sbi(PORTD,2);					//ENABLE PULL UP FOR SW1
	sbi(PORTD,3);					//ENABLE PULL UP FOR SW2
	DDRB |= (1 << DDB3) | (1 << DDB1);// | (1 << DDB2);
    // PB3 is now an output
	

	sei();
   
	while(1)						//START INFINITE LOOP TO SCAN INPUT FROM PORT
	{
		if (bit_is_clear(PIND, 0))	//IF SW1 IS PRESSED
		{
			init_timer();
		}

		else if (bit_is_clear(PIND, 3))	//IF SW3 IS PRESSED
		{
			stop_timer();
			
		
		} 
	}
	return(0);
}


