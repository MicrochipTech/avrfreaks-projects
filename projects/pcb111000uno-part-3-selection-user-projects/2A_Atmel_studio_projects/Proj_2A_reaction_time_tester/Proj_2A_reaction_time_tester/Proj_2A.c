

//Proj_2A:  Another reaction time tester				Press sw2 and get the display to flash



#include "Proj_2A_header_file.h"



int main (void){
	unsigned int rand;                     				//random number
	unsigned int PORT_1, PORT_2;
	char mode;

	setup_UNO_extra;
	
	PORT_1=1;
	PORT_2=1;

	I2C_Tx_2_integers(PORT_1, PORT_2);					//Initialise display

	while(switch_2_up);                     			//wait for switch 2 to be pressed before starting


	while (1){                              			//Outer while loop: An infinite loop.
		mode = 0;
		while(!(mode)){                           		//Inner while loop: Remains in loop as long as mode equals zero.

			while(switch_2_up);                       	//halt if switch 2 is released
			rand = (PRN_16bit_GEN(0))%3;	          	//generate a random number (1,2 or 3)

			for (int m = 0; m <= rand; m++)				//Rotate the top row of the display left
			{Rotate_Port_1;}              				//by one place, 2 places or 3 places
			Rotate_Port_2;                				//Rotate the bottom row left but only by one place
			I2C_Tx_2_integers(PORT_1, PORT_2);         	//update the display


			if(PORT_1 == PORT_2)                      	//If a single vertical line in illuminated
			{Timer_T0_10mS_delay_x_m(35);				//start a timer and pause for 350mS
				if(switch_2_up)mode = 1;                //If switch 2 has been released exit the inner while loop
			else while(switch_2_down);}               	//else wait for it to be released


			if(PORT_1 != PORT_2)                      	//If disjointed segments are illuminated
			{Timer_T0_10mS_delay_x_m(8);}}           	//start a timer and pause for 80mS before repeating the inner loop
			
			while(switch_2_up)flash_leds;				//Part of the outer while loop.

		}}           									//When it is pressed again return to the outer while loop
