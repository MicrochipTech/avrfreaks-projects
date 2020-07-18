

//Proj_1A1:  A Basic first sample program

//Suggestion Click on Tools/Options.../Environment/Fonts and Colors/VA Brace Matching 
//Select a really bright colour like red
//For VA Brace Error select something different like blue or olive																							



#include "Proj_1A1_header_file.h"




int main (void){										//When the program runs it always starts with the "main" routine.

	unsigned long PORT_1, PORT_2;						//Reserve two addresses in memory and give then the names PORT_1 and PORT_2
	

	setup_UNO_extra;									//Basic UNO device configuration: the same for all these projects

	while(1)											//An infinite loop follows
	{PORT_1=1; PORT_2 = 0x80000000;						//0x80000000 is the same as 1000 0000 0000 0000 0000 0000 0000 0000 in binary

		for(int m = 1; m <= 32; m++){					//Code between the {} brackets is repeated 32 times
		I2C_Tx_2_integers(PORT_1, PORT_2);				//Use the values of PORT_1 and PORT_2 to drive the LED display

		Timer_T0_10mS_delay_x_m(6);						//This subroutine generates a 10mS delay that is repeated 6 times
		PORT_1 = PORT_1 << 1;							//The contents of address "PORT_1" are shifted one place left and
		PORT_2 = PORT_2 >> 1;							//the contents of address "PORT_2" are shifted one place right
	}													//every time that the "for-loop" is executed
}}

