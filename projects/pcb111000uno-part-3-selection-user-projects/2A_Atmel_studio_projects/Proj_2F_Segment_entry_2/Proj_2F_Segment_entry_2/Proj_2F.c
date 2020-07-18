

//Proj_2F:  Display numbers manually


#include "Proj_2F_header_file.h"




#define message_1 "\r\nPress a to g for segments, then x for next digit.\r\n\
Press y or switch 1 for SW reset.\r\n"

#define message_2 "SW_reset?"





int main (void){

	char keypress = 0, digit_num=0;

	setup_UNO_extra;
	config_sw1_and_sw2_for_PCI;


	if (!(watch_dog_reset)) 														//Normal start up i.e. POR or post programming
	String_to_PC(message_1);														//Send normal start-up message


	else {watch_dog_reset = 0;														//Start up following SW_reset (reset the watch_dog_reset flag)
	newline();String_to_PC(message_2);}												//Send abbreviated message


	sei();
	wdt_enable(WDTO_30MS);															//Note: Start WDT


	do{																				//Keep inputting data until "y" is received

		while(1){if(isCharavailable(10))
			{keypress = receiveChar();break;}}										//"isCharavailable()" resets the WDT everytime that it is called

		switch(keypress){															//Test keypress

			case 'a': case 'b': case 'c':											//If a letter illuminate the display
			case 'd': case 'e': case 'f':
			case 'g': I2C_Tx_any_segment(keypress, digit_num); break;

			case 'x': digit_num = (digit_num + 1)%8;								//If "x" increment digit_num

			default: break;															//Ignore AOK press
		}																			//Bottom of switch block return to top of "do-loop"
	}while(keypress != 'y');														//Bottom of "do-loop" with exit condition


while(1);}																			//WDT times out




ISR(PCINT2_vect) 																	//SWitch press
{if (switch_1_down) 																//clears display before executing a  reset
	{I2C_Tx_any_segment_clear_all();while(1);}}




	char isCharavailable_Local (char m)
	{int n = 0;
		while (!(UCSR0A & (1 << RXC0)))												//If a key press is not detected
		{n++;	if (n>8000) 														//Increment "n" from zero to 8000
			{m--;n = 0;																//then reset it, decrement m
			wdr();}																	//and reset WDT to avoid an unwanted reset
		if (m == 0)return 0;}														//Returns a 0 if no keypress is detected
	return 1;}																		//Exit immediately and return 1 when a keypress is detected




