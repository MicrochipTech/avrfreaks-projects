


//Proj_4B:  Acquiring data from the user switches



/*USER INSTRUCTIONS

sw2 to populate digits[0]
sw1 to shift display left
sw3 to enter the number
sw2 to multiply the number by 10
sw1 to divide it by 10
press sw3 before sw1 or 2 to repeat*/



#include "Proj_4B_header_file.h"



long number_from_IO(void);


char  digits[8];																//stores the digits to be displayed
volatile char Data_Entry_complete;												//This variable is not used in the main() routine


int main (void){
	long num;

	setup_UNO_extra;

	String_to_PC("\r\nDATA FROM I/O");
	String_to_PC(message_1);

	while(1){String_to_PC("?\r\n");

		while((switch_1_down) || (switch_2_down) ||(switch_3_down));			//wait for switch release
		
		num = number_from_IO();													//Subroutine returns a number entered using the switches
		Num_to_PC(10, num);														//Echo the number to the PC
		newline();

		if(!(num)){SW_reset;}													//If the number is zero reset

		while(1){
			while((switch_1_up) && (switch_2_up));								//Wait for sw1 or 2 to be pressed
			if(switch_3_down)break;												//If sw3 is already down request a new number
			if(switch_2_down)num *=10;											//If sw2 was pressed multiply the number by 10
			else num /=10;														//If sw1 was pressed divide it by 10
			I2C_Tx_long(num);													//send the result to the display
			Num_to_PC(10, num);													//Send the result to the PC
			newline();
		Timer_T0_10mS_delay_x_m(25);}}}



		/********************************************************************************************************************/
		long number_from_IO(void){
			char copy_of_SREG;													//Bit 7 of register SREG is set by sei() and cleared by cli().
			setup_and_enable_PCI;
			Init_display_for_pci_data_entry;                                 	//Set digit[0] to zero and display it.
			copy_of_SREG = SREG;                                             	//Save interrupt state
			sei();                                                           	//Enable interrupts
			while(!(Data_Entry_complete));                                   	//Line A. wait here for PCI interrupts used to enter data
			Data_Entry_complete = 0;
			SREG = copy_of_SREG;                                             	//Restore original interrupt state
		return I2C_displayToNum();}                                      		//Acquire binary value of the display and return it.



		/********************************************************************************************************************/
		ISR(PCINT2_vect) {                                              		//input number: store keypresses in array -digits
			while(switch_2_down){                                           	//Use switch 2 to select keypress 0...9
				switch(digits[0]){												//Test digits[0]
					case '9': if(!(digits[1]))									//If its 9 scroll onto "-"
					digits[0] = '-';											//but only if this is the first entry (i.e. digits[1] is 0.
					else digits[0] = '0';break;									//otherwise scroll from zero to 9
					case '-': digits[0] = '0';break;							//If digits[0] is "-" scroll onto '0'.
				default: digits[0]++; break;}									//Otherwise increment it
				I2C_Tx_8_byte_array(digits);									//Send value to the display
			Timer_T0_10mS_delay_x_m(25);}                                   	//return to Line A when sw1 is released

			if(switch_1_down) {                                            		//Use switch 1 to shift display left
				shift_digits_left; digits[0] = '0';                            	//shift the display and initialise digit[0] (make it display '0')
			I2C_Tx_8_byte_array(digits);}}                                 		//update the display and return to Line A



			/********************************************************************************************************************/
			ISR(PCINT0_vect){
				char disp_bkp[8];
				if(switch_3_up)return;											//Ignore switch release

				for(int m = 0; m<=7; m++)
				{disp_bkp[m]=digits[m];
				digits[m]=0;}I2C_Tx_8_byte_array(digits);
				Timer_T0_10mS_delay_x_m(25);									//Flash display
				for(int m = 0; m<=7; m++)
				{digits[m]=disp_bkp[m];}
				I2C_Tx_8_byte_array(digits);
			Data_Entry_complete=1;}                                      		//Return to Line A
