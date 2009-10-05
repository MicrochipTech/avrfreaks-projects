//***********************************************************************************************//
//*******************************************  main.c  ******************************************//
//***********************************************************************************************//
/*
******************************************************************
	
	TEST OSCILLATOR

	Simple oscillator timer based with 6 output: 16, 32, 64, 128, 256, 512 Hz
	Frequency 8MHz internal oscillator
	ATTINY26L but all other AVR must go, check register names for your AVR

	Used for test hardware input of any board

	0.0	05-10-2009	Born

******************************************************************
*/
#include <avr\io.h>
#include <avr\interrupt.h>

// Declaration of counters
unsigned char CNT512;	// 8MHz/256=31250/512/2=30
unsigned char CNT256;	// 8MHz/256=31250/256/2=61
unsigned char CNT128;	// 8MHz/256=31250/128/2=122
unsigned char CNT64;	// 8MHz/256=31250/64/2=244
unsigned int CNT32;		// 8MHz/256=31250/32/2=488
unsigned int CNT16;		// 8MHz/256=31250/16/2=976
unsigned int CNT2;		// 8MHz/256=31250/2/2=7812
// Declaration of max counting finded sperimentally around the calculated values above
#define CNT512MAX	29
#define CNT256MAX	59
#define CNT128MAX	119
#define CNT64MAX	239
#define CNT32MAX	479
#define CNT16MAX	958
#define CNT2MAX		7812

// Hardware initialization
void hw_init(void)
{
	// PA0...PA5 as output, PA6 PA7 high impedance
	PORTA	= 0b00000000;
	DDRA	= 0b00111111;
	PORTA	= 0b00000000;
	// PB3 as output for signaling oscillation blink at 2 Hz
	PORTB	= 0b00000000;
	DDRB	= 0b00001000;
	PORTB	= 0b00000000;	

	// Initialize Timer
	// prescaler 1 interrupt on overflow
	TCCR0=(1<<CS00);
	TIMSK=(1<<TOIE0);
}

// Timer action
SIGNAL (TIMER0_OVF0_vect)
{
	if (CNT512<CNT512MAX) CNT512++;
	else 
	{
		CNT512=0;
		// Toggle output
		if (PORTA & (1<<PA5)) PORTA &= ~(1<<PA5);
		else PORTA |= (1<<PA5);
	}
	if (CNT256<CNT256MAX) CNT256++;
	else 
	{
		CNT256=0;
		// Toggle output
		if (PORTA & (1<<PA4)) PORTA &= ~(1<<PA4);
		else PORTA |= (1<<PA4);
	}
	if (CNT128<CNT128MAX) CNT128++;
	else 
	{
		CNT128=0;
		// Toggle output
		if (PORTA & (1<<PA3)) PORTA &= ~(1<<PA3);
		else PORTA |= (1<<PA3);
	}
	if (CNT64<CNT64MAX) CNT64++;
	else 
	{
		CNT64=0;
		// Toggle output
		if (PORTA & (1<<PA2)) PORTA &= ~(1<<PA2);
		else PORTA |= (1<<PA2);
	}
	if (CNT32<CNT32MAX) CNT32++;
	else 
	{
		CNT32=0;
		// Toggle output
		if (PORTA & (1<<PA1)) PORTA &= ~(1<<PA1);
		else PORTA |= (1<<PA1);
	}
	if (CNT16<CNT16MAX) CNT16++;
	else 
	{
		CNT16=0;
		// Toggle output
		if (PORTA & (1<<PA0)) PORTA &= ~(1<<PA0);
		else PORTA |= (1<<PA0);
	}
	// Toggle LED Signal
	if (CNT2<CNT2MAX) CNT2++;
	else 
	{
		CNT2=0;
		// Toggle output
		if (PORTB & (1<<PB3)) PORTB &= ~(1<<PB3);
		else PORTB |= (1<<PB3);
		PINB=(1<<PINB3);
	}
}

// ***********************************************************
// Main program
int main(void)
{	
	hw_init();

	sei();

	// Infinite loop;
	while(1)
	{}
}
