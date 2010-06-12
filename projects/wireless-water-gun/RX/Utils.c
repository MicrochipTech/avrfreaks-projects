#include "Utils.h"

//**************INITIALIZE*******************
//Initialize ports
void initialize_PORTS()
{
	DDRB = 0xFF;  //set PORTB to 11111111
	DDRC = 0x00;  //set PORTC to 00000000
	DDRD = 0xFE;  //set PORTD to 11111110

	PORTB = 0;
	PORTD = 0;
}

void initialize_ADC()
{
	disable_interrupts;

	//ADCSRA |= (1 << ADPS0) | (1 << ADPS1);  //ADC frequency = RC oscillator / 8 (/8 prescaler)
	ADMUX |= (1 << REFS0);  //AVCC with external capacitor at AREF pin
	ADMUX |= (1 << ADLAR); //Left Adjust Result
	//ADCSRA |= (1 << ADFR);  // Set ADC to Free-Running Mode  	
	ADCSRA |= (1 << ADEN);  //ADC enable
	//ADCSRA |= (1 << ADIE);  // Enable ADC Interrupt
	
	//Initialize Interrupts
	//MCUCR &= ~(1 << ISC10) & ~(1 << ISC11);  //The low level of INT1 generates an interrupt request
	//GICR |= (1 << INT1);  //External Interrupt Request 1 Enable

	//PORTC |= (1 << 4);

	enable_interrupts;
}

void initialize_PWM()
{
	//disable_interrupts;

	// set up PB1 and PB2 with 8bit PWM
	TCCR1A = (1 << WGM10);  //| (1 << COM1A1);  // 8 bit fast PWM, set OC1A on compare match, clear them at top

	TCCR1B = (1 << CS10) | (1 << WGM12);  // enable timer1 - clkI/O/1 (No prescaling), fast PWM

	OCR1A = 0;  //set wave to 0% duty cycle

	//enable_interrupts;
}


//**************DELAY CYCLES******************
//wait for X amount of cycles
void delay_cycles(uint64_t cycles)
{
	while(cycles > 0)
	{
		cycles--;
	}
}


//***************READ ADC*********************
//read 8-bit analog to digital conversion
unsigned int read_ADC(char ADC_input)
{
	//ADMUX &= ~(0x0E);
	//ADMUX |= PIN;

	ADMUX = ADC_ADMUX_mask | ADC_input;

	ADCSRA |= (1 << ADSC);  
	while ((ADCSRA & (1 << ADIF)) == 0);
	{}
	ADCSRA |= (1 << ADIF);
	
	//unsigned int temp;
	//unsigned int tempH;

	//temp = ADCL;
	//tempH = ADCH | 0x03;
	//temp |= ((unsigned int)tempH << 8);
	//return temp;
	
	return ADCH;  //return 8-bit Analog to Digital Conversion
	
}


//*****************RANGE**********************
//round the data into the byte range
unsigned char byte_range(signed int value)
{
	if ((value >= 0)&&(value <= 255))
	{
		return value;
	}
	else if (value < 0)
	{
		return 0;
	}
	else
	{
		return 255;
	}
}	


//****************GET REFERENCE****************
//get current analog read of pot
unsigned char get_reference(char pin)
{
	while(1)
	{
		if ((PINC & (1<<1)) == 0)
		{
			PORTB |= (1<<pin);
			unsigned char reference = read_ADC(0);
			delay_cycles(2000);
			PORTB &= ~(1<<pin);
			return reference;
		}
	}
}


//*****************SLEEP**********************
void sleep()
{
	disable_interrupts;

	TCCR1B &= ~(1 << CS10);  //disable timer1

	PORTB &= ~(1 << 1);  //set OC1A pin LOW

	enable_interrupts;
}


//*****************WAKE***********************
void wake()
{
	disable_interrupts;	

	TCCR1B = (1 << CS10); //enable timer1

	enable_interrupts;
}
