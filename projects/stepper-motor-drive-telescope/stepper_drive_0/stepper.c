// File: stepper.c

//#include <stdlib.h>
//#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

//#include <avr/delay.h>
#include "encoder.h"

void READ_encoders(void); //asm("READ_encoders");
void READ_RA_keys(void); 
void READ_DEC_keys(void); 
void STATUS_check(void); 
void TIMER0_init(void);
void TIMER1_init(void);
void TIMER2_init(void);
void INT1_init(void);
void WAIT(void);
void USART_Init(void);
void RA_SLEW(void);
void DEC_SLEW(void);
void PULSE(void);


int main (void)
	{

//Initialise Timer0 (compare match A)
	TIMER0_init();
	
//Initialise Timer1 (compare match A)
	TIMER1_init(); 
	
//Initialise Timer2
	TIMER2_init();
	
//Initialise INT1
	INT1_init();

// set PORT B pins for input (0) or output (1)

//    7     6     5     4     3     2     1     0
//  -----------------------------------------------
// |  x  |  x  | ENA | DIR | PUL | RA+ | RA- |  x  |
//  -----------------------------------------------

	PORTB = 0b00000110 ;	// enable pull-ups for RA+/- keys
	DDRB =  0b00111000 ;	// set PB3, PB4, PB5 as output	
	
// set PORT D pins for input (0) or output (1)

//    7     6     5     4     3     2     1     0
//  -----------------------------------------------
// | DE+ | PUL | ENA | DIR | INT1| DE- |  x  |  x  |
//  -----------------------------------------------
	PORTD = 0b10001100 ;	// enable pull-ups on PD2, PD7 (DEC-/+ keys) and PD3 (INT1)
	DDRD  = 0b01110000 ;	// set direction (what about USART on PD0, PD1?)

// Encoder input

   	PORTC = 0			; 	//set all Port C pins tri-stated
	DDRC = 0			; 	//set all Port C pins as input (encoder)

    	guiding_offset = 0;
	guiding_counter = 0;
	RA = 0				;	// Clear RA registers
	DEC = 0				;	// Clear Declination registers (X reg)

// Default stepping limits

	RA_STEP_max = 66327;	// 1 degree (64 usteps)
	DEC_STEP_max = 66327;	// 2 degrees
	
// Not slewing 
	RA_accel = 0;
	DEC_accel = 0;
	
//Initialise_USART
	USART_Init();
	
// Enable Dec motor 
	PULSE();
	
	sei();	// enable interrupts

	prev_RA_keys = 0b00000110 ;
	prev_DEC_keys = 0b10000100 ;
	STATUS_byte = 0b00000000;

  for (;;)
	{
	READ_encoders();
	READ_RA_keys();
	READ_DEC_keys();
//	STATUS_check();
	}

return 1;
}



