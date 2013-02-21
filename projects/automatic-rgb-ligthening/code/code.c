/F_CPU must be defined before including the delay macros.
//#define F_CPU 9600000	   //Full speed for internal oscillator on ATTiny13

#include <avr/interrupt.h> //Equips the interrupt handlers for doing softPWM
#include <avr/io.h>	   //General I/O functionality
#include <util/delay.h>	   //Here for convience, delay macros to adjust timescale
#include <compat/deprecated.h>		//HEADER FILE FOR FUNCTIONS LIKE SBI AND CBI



#define DDRB_MASK 0x0F //Configure PB0,1,2,3 as outputs, other pins are input
#define PORTB_MASK 0x0F //Set only the output pins
#define FADEDELAY 2 //a delay to set an attractive speed.
#define FADEDELAY1 50 //a delay to set an attractive speed.
#define POSTDELAY 500 //a delay to make the POST function visible but not tedious.

//DECLARATION OF FUNCTION
void init(void);

void select_colour(void);
void post(void);
void PWM_Timer1_Start(void);
void PWM_Timer2_Start(void);
void PWM_Timer0_Start(void);

//ANYTHING ACCESSED IN AN ISR MUST BE MARKED VOLATILE WHEN USING -Os
//The compare values for the three color channels
volatile uint8_t Rcnt = 0;
volatile uint8_t Gcnt = 0;
volatile uint8_t Bcnt = 0;

static volatile uint8_t pwmcnt = 0x00;
static volatile uint8_t nextB = 0x03;
static volatile uint8_t ISRcnt = 0;

const uint8_t RED[] = {128, 128, 255,  253, 255, 255, 255, 255, 255, 128, 64, 128, 128, 
						0, 0, 0, 0, 0, 0, 128, 128, 255, 45, 160, 0, 131, 250, 247, 124, 82 };
const uint8_t GREEN[] = {128, 64, 128, 128, 255, 255, 128, 0, 64, 0, 0, 255, 255, 255, 
						128, 128, 0, 255, 0, 128, 0, 0, 210, 95, 0, 197, 243, 9, 12, 7 };
const uint8_t BLUE[] = {0, 0, 0, 64, 0, 128, 128, 0, 64, 0, 0, 128, 0, 0, 0, 128, 255, 
						255, 128, 255, 128, 255, 95, 160, 160, 197, 5, 240, 121, 80 };

static uint8_t cc = 0;

void init(void)
{
	DDRB=DDRB_MASK; //Set the pins as inputs/outputs as needed
	sbi(PORTB, 0);
   
}
void PWM_Timer1_Start(void)
{
	ICR1 = 255;
	OCR1A = BLUE[cc];
	OCR1B =	GREEN[cc];
	TCCR1A |= (1<<COM1A1) |(0<<COM1A0);//non-inverted in OC1A output
	TCCR1A |= (1<<COM1B1) | (0<<COM1B0);//inverted mode in OC1B output
	TCCR1A |= (1<<WGM11) | (0<<WGM10);
	
	TCCR1B |= (1<<WGM13) | (1<<WGM12); //FAST PWM with ICR1 top value
	TCCR1B |= (0<<CS12) | (0<<CS11) | (1<<CS10);//Clock / 1 PRESCALER
	TCNT1 = 0;
//	TIMSK |= (1<<TOIE1); 
}

void PWM_Timer2_Start(void)
{
	//TIMER 2 CONFIGURATION

	OCR2 = RED[cc];
	TCCR2 |= (0<<CS22) | (0<<CS21) | (1<<CS20);//Clock / 1 PRESCALER
	TCCR2 |= (1<<WGM21) | (1<<WGM20);// MODE
	TCCR2 |= (1<<COM21) | (0<<COM20);//inverted mode
	TCNT2 = 0;

	TIMSK |= (1 << TOIE2); //ENABLE INTERRUPT FOR COMPARE OUTPUT
	
	sei();

}

void PWM_Timer0_Start(void)
{
	//TCNT0 = 0;
	TCCR0 |= (0<<CS02) | (1<<CS01) | (0<<CS00);//Clock / 1 PRESCALER


	TIMSK |= (1 << TOIE0);

}

/* pulsewhite pulses "white" (R+G+B in synch)*/
void select_colour(void)
{
	//Start with all off.
	//Red Channel Selection
	if(OCR2 > Rcnt)
	{
		do
		{
			OCR2--;
			_delay_ms(FADEDELAY);		

		}while(OCR2 > Rcnt);
	}
	else if(OCR2 < Rcnt)
	{
		do
		{
			OCR2++;
			_delay_ms(FADEDELAY);		

		}while(OCR2 < Rcnt);
	}
	// Green Channel Selection
	if(OCR1B > Gcnt)
	{
		do
		{
			OCR1B--;
			_delay_ms(FADEDELAY);		

		}while(OCR1B > Gcnt);
	}
	else if(OCR1B < Gcnt)
	{
		do
		{
			OCR1B++;
			_delay_ms(FADEDELAY);		

		}while(OCR1B < Gcnt);
	}
	//Blue Channel Selection
	if(OCR1A > Bcnt)
	{
		do
		{
			OCR1A--;
			_delay_ms(FADEDELAY);		

		}while(OCR1A > Bcnt);
	}
	else if(OCR1A < Bcnt)
	{
		do
		{
			OCR1A++;
			_delay_ms(FADEDELAY);		
			
		}while(OCR1A < Bcnt);
	}

	
}

void post(void)
{
	cli(); //Interrupts off for the tests
	//A friendly little POST to verify all LEDs
	PORTB = 0x08; //Flash Red
	_delay_ms(POSTDELAY);
	PORTB = 0x04; //Flash Green
	_delay_ms(POSTDELAY);
	PORTB = 0x02; //Flash Blue
	_delay_ms(POSTDELAY);
//	PORTB = 0x0E; //All On
//	nextB = 0x03;//Make sure we start out blank after exit
//	_delay_ms(POSTDELAY);
	sei();//Interrupts back on
}

/*MAIN*/
int main(void)
{
	
	init();//Set up the ports
	post();//Demonstrate that everything is hooked up properly
	
	//BEHAVIOR GOES HERE
//	PWM_Timer0_Start();
	PWM_Timer1_Start();
	PWM_Timer2_Start();
	sbi(PORTB, 0);
	
//	sei();
	
	while(1)
	{
		
		select_colour();
		_delay_ms(POSTDELAY);
//		autospectrum();
	
		
	} //Demonstrate
	
}

/*This ISR triggers every time Timer0 overflows, and handles PWM to the desired values*/
ISR (TIMER2_OVF_vect)  // timer0 overflow interrupt
{
	//TOV0 is automagically cleared when entering the vector
//	TCNT0 += 6;
	if(++ISRcnt==1) //Increment the modulo 256 counter, see if it overflowed

	{	
		
		if (++pwmcnt == FADEDELAY1)
		{
			Rcnt = RED[cc];   //OCR2
			Gcnt = GREEN[cc]; //OCR1B
			Bcnt = BLUE[cc];  //OCR1A

			if (++cc == 30)
			{
				cc = 0;
			}

			pwmcnt = 0 ;
			ISRcnt = 0;
		}

		

	}
	

}
