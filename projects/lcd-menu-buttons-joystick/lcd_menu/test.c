#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>

#define SCREEN_SIZE 16

#include "lcd.h"
#include "lcd_menu.h"
#include "keypad4x4.h"
#include "adc.h"
#include "joystick.h"


#define keypad_port PORTB
#define keypad_ddr DDRB
#define keypad_pin PINB

#define lcd_port PORTC
#define lcd_ddr DDRC
#define lcd_pin PINC

//Joystick definitions
uint16_t x=0,y=0;
uint8_t joy_direction;

void creator_info()
{
LCDClear();
LCDWriteStringXY(0,0, "SOFTWARE CREATOR");
LCDWriteStringXY(3,1, "PANOSCURE");
_delay_ms(4000);
LCDClear();
initial_menu(0);
}



void first_sub_menu()
{
initial_menu(1);

uint8_t button_pressed,arrow_possition;
	while(1)
	{

	    x=ReadADC(0);  //Read ADC Channel 0
      	y=ReadADC(1);  //Read ADC Channel 1

		joy_direction = find_direction_from_xy(x,y);

		if(find_direction_from_xy(x,y)==3)
		{rotate_menu(1,1); _delay_ms(200);}

		if(find_direction_from_xy(x,y)==1)
		{rotate_menu(0,1); _delay_ms(200);}




	button_pressed=check_if_button_pressed();

			if(button_pressed=='A')
			{		
			rotate_menu(0,1); //Moves the menu up			
			}

			else if(button_pressed=='B')
			{
			rotate_menu(1,1); //Moves menu down
			}

			else if(button_pressed=='*')
			{
			initial_menu(0);
			return 0;
			}

			else if(button_pressed=='#')
			{
			LCDClear();
			arrow_possition = rotate_menu(2,1);//Finds the actual possition of the "<"

			if(arrow_possition==0) 
			{
			LCDWriteStringXY(0,0,"1st Poss Submenu");
			_delay_ms(3000);
			initial_menu(1);
			}

			if(arrow_possition==1)
			{ 
			LCDWriteStringXY(0,0,"2nd Poss Submenu");
			_delay_ms(3000);
			initial_menu(1);
			}

			if(arrow_possition==2)
			{ 
			LCDWriteStringXY(0,0,"3rd Poss Submenu");
			_delay_ms(3000);
			initial_menu(1);
			}

			if(arrow_possition==3) 
			{
			LCDWriteStringXY(0,0,"4th Poss Submenu");
			_delay_ms(3000);
			initial_menu(1);
			}
			
			}

			


	}//end of while
}//end of function





int main()
{
	MCUCSR = 0x80;			//disable jtag to make portC to work
	MCUCSR = 0x80;



uint8_t button_pressed=0;	
uint8_t arrow_possition;

//high nibble for output(columns) low for input(rows);
keypad_ddr=0xF0;
//enable internal pullups for PB0-PB3
keypad_port=0x0F;
//keypad_pin=0xFF;


//Port D for indication only
lcd_ddr=0xFF;
lcd_port=0xFF;

//Initialize LCD module
InitLCD(LS_BLINK|LS_ULINE);
LCDClear();





initial_menu(0);





while (1) //loop key check forever
	{
      	x=ReadADC(0);  //Read ADC Channel 0
      	y=ReadADC(1);  //Read ADC Channel 1

		joy_direction = find_direction_from_xy(x,y);



if(find_direction_from_xy(x,y)==3)
{rotate_menu(1,0); _delay_ms(200);}

if(find_direction_from_xy(x,y)==1)
{rotate_menu(0,0); _delay_ms(200);}


button_pressed=check_if_button_pressed();
			


			if(button_pressed=='A')
			{		
			rotate_menu(0,0); //Moves the menu up
			}

			else if(button_pressed=='B')
			{
			rotate_menu(1,0); //Moves menu down
			}
		                                                                                                                                                                                                                                                                                                                                                                                                     
			if(button_pressed=='#')
			{
			LCDClear();
			arrow_possition = rotate_menu(2,0);//Finds the actual possition of the "<"

			if(arrow_possition==0)
			{ 
			LCDWriteStringXY(0,0,"1st Possition");
			_delay_ms(3000);
			initial_menu(0);			
			}

			if(arrow_possition==1) 
			first_sub_menu();

			if(arrow_possition==2)
			 	{
				LCDWriteStringXY(0,0,"3rd Possition");
				_delay_ms(3000);
				initial_menu(0);
				}
				
			if(arrow_possition==3) 
				{
				LCDWriteStringXY(0,0,"4th Possition");
				_delay_ms(3000);
				initial_menu(0);
				}

			if(arrow_possition==4) 
			creator_info();
			

			if(arrow_possition==5) 
				{
				LCDWriteStringXY(0,0,"6th Possition");
				_delay_ms(3000);
				initial_menu(0);
				}
			}


	}
}
