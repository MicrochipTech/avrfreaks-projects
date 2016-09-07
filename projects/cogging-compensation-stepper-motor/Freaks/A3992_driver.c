#include <avr/io.h>  
#include <avr/interrupt.h>   
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <math.h>

uint8_t offset;
uint8_t increment;
uint8_t PUL, DIR, ENA;
uint8_t SW4, SW5, SW6, shift;
uint8_t i, mode, n, p;
uint8_t DAC[256];
uint32_t k;
float pi=3.14159265;

void send_WORD0(void);
void send_WORD1(void);
void read_uSTEP_switches(void);
void setup_table(float, float, uint8_t);
void flash_LED(void);

ISR(TIM1_COMPA_vect)
{
	read_uSTEP_switches();
	send_WORD1();				// send WORD1 to A3992 	
}

//Interrupt service routine for PUL
ISR(PCINT0_vect)
{
	PUL = 255;
	for (i=0;i<10;i++)			// Check 10 times before believing
	{
		PUL &= PINA & 0b00000100; 	// PA2
	}
	if (PUL != 0)
	{	
		send_WORD0();
	}
}

//Read microstep setting

void read_uSTEP_switches(void)			// ON = LOW, OFF = HIGH 
{
	SW4 = (PINA & 0b10000000) >> 7; 	// PA7
	SW5 = (PINB & 0b00000100) >> 1;		// PB2
	SW6 = (PINA & 0b01000000) >> 4;		// PA6
	shift = (SW4|SW5|SW6);
	if (shift > 6) {shift = 6;}  		// cope with all switches OFF
	increment = 64 >> shift;		// set no. of usteps
}


int main(void)
{
//Set clock prescaler
  CLKPR = (1<<CLKPCE); 		// enable change
  CLKPR = 0;			// 8MHz (CLKPR = (1<<CLKPS0) for 4MHz)
  
//Initialise Timer1
  TCCR1A = 0; // CTC mode
  TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10);// CTC, prescaler = 1024, f=7.8125kHz
  TIMSK1 = (1<<OCIE1A);
  OCR1A = 255;			// 30.5Hz (8MHz)  
  
//Set PA3, PA4, PA5 as output (DATA, CLK, STROBE)
  DDRA = 0b00111000;
  DDRB = 0b00000000;
  
//Activate pull-up resistors
  PORTA = 0b11000111;
  PORTB = 0b11111111;
  
//Enable pin change interrupts
  GIMSK = (1<<PCIE0);
  PCMSK0 = (1<<PCINT2); 	// Enable PUL (PA2)

  sei();			// enable interrupts
  
  offset = 0;
  increment = 1; 		// start with 64 usteps
 
  setup_table(0.0,0.05,3); 

  for (;;)
  {	
  }
	 
  return 1;
}


//Set up table
void setup_table(float a, float b, uint8_t mode)
{
	float phi, s, smax = 1.0, h;
	
	if (mode == 0) {smax = 1.0;}
	if (mode == 1) {smax = 1.0 - b;}
	if (mode == 2) {smax = 1.0;}
	if (mode == 3) {smax = 1.0;}

	for (int i=0;i<256;i++)
	{
	    phi = pi * ((float)i)/128.0;
		phi += a * sin(2*phi);
		s  = sin(phi);		
		if (mode == 1) {s += b * sin(3*phi);}
		if (mode == 2) {s += b * s * fabs(sin(2*phi)) / fabs(s);}
		if (mode == 3) {
		    s += b * sin(4*phi) * cos(phi)/ fabs(cos(phi)) * exp(-3*fabs(s));
		 }
		h = (s/smax) * 63.5;
		if (h >  63) {h=63;}
		if (h < -63) {h=-63;}
		if (h >= 0) 
		{
			DAC[i] = (uint8_t)h;
		}
		else
		{
			DAC[i] = (uint8_t)(-h) + 128;  // add negative sign
		}
	}
}
