

//Proj_1C1:  Target practice: Testing your reaction time		Press sw2 to shoot the LEDs down


#include "Proj_1C1_header_file.h"


unsigned int PORT_1;		        						//Variables used by both the main routine and also by the Interrupt Service Routine
unsigned int mask;											//"mask" is continuously adjusted as LEDs are shot down
char switch_control;                        				//Prevents rapid switch presses from shooting leds down to quickly



int main (void){
	setup_UNO_extra;
	config_sw1_and_sw2_for_PCI;                   			//Setup switches for "Pin Change Interrupt". SW1 is not used here.

	mask = 0xFFFF;                                			//0xFFFF = 0b1111111111111111 indicating that none of the leds have yet been shot down
	switch_control = 0;
	sei();                                       			//Enable all interrupts

	while(mask){                                  			//Exit the "while-loop" as soon as mask gets set to zero
		PORT_1=1;                                     		//Initialise display to 0000 0000 0000 0001

		for(int m = 1; m <= 16; m++){                 		//Repeat "for-loop" 16 times
			if(m == 1)switch_control = 0;                 	//Set switch_control to zero at the start of each sweep so that a LED can be shot down
			I2C_Tx_2_integers
			(PORT_1 & mask, (~mask) ^ PORT_1);				//LOGIC: "Dead" leds are transferred to the bottom row
			
			Timer_T0_10mS_delay_x_m(10);                 	//Program execution spends most time waiting here, so this is where the interrupt almost always occurs
			PORT_1 = (PORT_1 << 1);                       	//Move on to next display location
		}}

		I2C_Tx_2_integers(0, 0xFFFF);              			//When all leds are dead illuminate all the bottom leds and then
		Timer_T0_10mS_delay_x_m(100);						//pause for 1 sec before starting all over again.
	SW_reset;}



	/**************ISR Routine executed every time that switch 3 is  operated**************************************/

	ISR(PCINT2_vect) {										//This ISR momentarily interrupts the main routine
		if(switch_2_up)return;								//It notes which LED has just been shot down and
		if (switch_control) return;							//returns early if "switch_control" is 1
		mask &= ~PORT_1;									//Write zero at its location in the "mask" register
	switch_control = 1;}									//Set switch control to 1.
