#include "init_Pins.h"


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void init_Pins (void)
{
	//bit_set(MCUCR,BIT(PUD));	//Disable Pull ups
	bit_clear(MCUCR,BIT(PUD));	//Enable  Pull ups

	//PORTB = 0b00000000;		//Set before direction
	//PORTC = 0b00000000;		//if High-> wont be glitch
	//PORTD = 0b00000000;

	/*
	//To pull ups for I2C, works tested
	bit_set(PORTC,BIT(4));		//to enable pull ups
	bit_set(PORTC,BIT(5));
	*/
	
	/*
	//Enabled for common HW!
	//To pull ups for Buttons
	bit_set(PORTC,BIT(3));		//to enable pull ups
	bit_set(PORTC,BIT(2));
	*/
	
	//Pull up for UART!, make it input HW will change it when needed
	//Pull up for uart doesn't work!!!! tested
	//bit_set(PORTD,BIT(0));		//to enable pull ups
	//bit_set(PORTD,BIT(1));

	//Output:1 Input:0
	DDRB = 0b11111111;			
	DDRC = 0b11111111;
	DDRD = 0b11111110;

}	//init_Pins

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
