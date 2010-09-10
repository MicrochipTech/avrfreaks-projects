///////////////////////////////////////////////////////////////////
//
//	RPM Meter with 10 LEDS
//
//	0.0	Emissione	frequency refresh 250ms 
//					OCR1A = 244; LEDS_ON=(frequency_IN*240)/1000;
//
//					micro ATTINY24A 8MHZ clock CKDIV8 ON --> clock 1MHz
//
//					
//
///////////////////////////////////////////////////////////////////



#include <avr/io.h>
#include <avr/interrupt.h>

#define LED0_ON		(PORTA |=  (1<<PA0))
#define LED0_OFF	(PORTA &= ~(1<<PA0))
#define LED1_ON		(PORTA |=  (1<<PA1))
#define LED1_OFF	(PORTA &= ~(1<<PA1))
#define LED2_ON		(PORTA |=  (1<<PA2))
#define LED2_OFF	(PORTA &= ~(1<<PA2))
#define LED3_ON		(PORTA |=  (1<<PA4))
#define LED3_OFF	(PORTA &= ~(1<<PA4))
#define LED4_ON		(PORTA |=  (1<<PA5))
#define LED4_OFF	(PORTA &= ~(1<<PA5))
#define LED5_ON		(PORTB |=  (1<<PB0))
#define LED5_OFF	(PORTB &= ~(1<<PB0))
#define LED6_ON		(PORTB |=  (1<<PB1))
#define LED6_OFF	(PORTB &= ~(1<<PB1))
#define LED7_ON		(PORTB |=  (1<<PB2))
#define LED7_OFF	(PORTB &= ~(1<<PB2))
#define LED8_ON		(PORTA |=  (1<<PA7))
#define LED8_OFF	(PORTA &= ~(1<<PA7))
#define LED9_ON		(PORTA |=  (1<<PA6))
#define LED9_OFF	(PORTA &= ~(1<<PA6))


void io_Setup(void)
{	
	// PA7, PA6, PA5, PA4, PA2, PA1, PA0 outputs LED
	PORTA	= 0b00000000;
	DDRA	= 0b11110111;
	PORTA	= 0b00000000;
	// PB2, PB1, PB0 outputs LED
	PORTB	= 0b00000000;
	DDRB	= 0b00000111;
	PORTB	= 0b00000000;
}

// timer0
void frequency_IN_Setup(void)
{
	TCNT0 = 0;						// reset timer
	TCCR0B = 0;						// stop
	TCCR0A = 0;						// normal operation
	TIFR0 = 0x08;					// reset alla flags
	TCCR0B = (1<<CS02)|(1<<CS01);	// External clock source on T0 pin. Clock on falling edge.
}
/////////////////////////////////////////////////////////////////////////////////////////
//	timer1 2Hz frequency
void freq_2Hz_Setup(void)
{	
	TCCR1B = 0;						// stop
	TCCR1A = 0;						// normal operation,
	TCNT1 = 0;						// reset timer
	OCR1A = 244;					// Output compare every 250ms
	TIFR1 = 0x2F;					// reset all flags
	TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10);	// start timer, Freq/1024, CTC
}

unsigned int frequency_IN=0;
unsigned int LEDS_ON=0;

void Gestione_LEDS(unsigned int value);

int main(void)
{
	io_Setup();
	frequency_IN_Setup();
	freq_2Hz_Setup();

	OCR1A = 61;

	// routine power on
	// test all leds
	do
	{
		if (TIFR1 & (1<<OCF1A))
		{
			TIFR1 |= (1<<OCF1A);
			frequency_IN++;
			
			Gestione_LEDS(frequency_IN);
		}
	}while (frequency_IN<10);

	OCR1A = 244;
	
	while(1)
	{
		if (TIFR1 & (1<<OCF1A))
		{
			TIFR1 |= (1<<OCF1A);

			frequency_IN=TCNT0;

			LEDS_ON=(frequency_IN*240)/1000;	// display every 250ms or 1/4 second so: RPM/minute = frequency_IN *(60seconds*4)
			
			Gestione_LEDS(LEDS_ON);

			TCNT0 = 0;
		}
	}
}

void Gestione_LEDS(unsigned int value)
{
		switch (value)
		{
			case 0:
				LED0_ON;
				LED1_OFF;
				LED2_OFF;
				LED3_OFF;
				LED4_OFF;
				LED5_OFF;
				LED6_OFF;
				LED7_OFF;
				LED8_OFF;
				LED9_OFF;
				break;
			
			case 1:
				LED0_ON;
				LED1_ON;
				LED2_OFF;
				LED3_OFF;
				LED4_OFF;
				LED5_OFF;
				LED6_OFF;
				LED7_OFF;
				LED8_OFF;
				LED9_OFF;
				break;
			
			case 2:
				LED0_ON;
				LED1_ON;
				LED2_ON;
				LED3_OFF;
				LED4_OFF;
				LED5_OFF;
				LED6_OFF;
				LED7_OFF;
				LED8_OFF;
				LED9_OFF;
				break;
			
			case 3:
				LED0_ON;
				LED1_ON;
				LED2_ON;
				LED3_ON;
				LED4_OFF;
				LED5_OFF;
				LED6_OFF;
				LED7_OFF;
				LED8_OFF;
				LED9_OFF;
				break;
			
			case 4:
				LED0_ON;
				LED1_ON;
				LED2_ON;
				LED3_ON;
				LED4_ON;
				LED5_OFF;
				LED6_OFF;
				LED7_OFF;
				LED8_OFF;
				LED9_OFF;
				break;
			
			case 5:
				LED0_ON;
				LED1_ON;
				LED2_ON;
				LED3_ON;
				LED4_ON;
				LED5_ON;
				LED6_OFF;
				LED7_OFF;
				LED8_OFF;
				LED9_OFF;
				break;
				
			case 6:
				LED0_ON;
				LED1_ON;
				LED2_ON;
				LED3_ON;
				LED4_ON;
				LED5_ON;
				LED6_ON;
				LED7_OFF;
				LED8_OFF;
				LED9_OFF;
				break;
				
			case 7:
				LED0_ON;
				LED1_ON;
				LED2_ON;
				LED3_ON;
				LED4_ON;
				LED5_ON;
				LED6_ON;
				LED7_ON;
				LED8_OFF;
				LED9_OFF;
				break;
				
			case 8:
				LED0_ON;
				LED1_ON;
				LED2_ON;
				LED3_ON;
				LED4_ON;
				LED5_ON;
				LED6_ON;
				LED7_ON;
				LED8_ON;
				LED9_OFF;
				break;
				
			case 9:
				LED0_ON;
				LED1_ON;
				LED2_ON;
				LED3_ON;
				LED4_ON;
				LED5_ON;
				LED6_ON;
				LED7_ON;
				LED8_ON;
				LED9_ON;
				break;
		}
}


