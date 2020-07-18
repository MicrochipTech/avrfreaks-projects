


//Proj_2G:  A slightly better way of entering numbers


/*Send a number when requested.
Notice that the digits must be reversed to give the wanted number
Press PCB_A reset switch to re-initialise the display
*/


#include "Proj_2G_header_file.h"        									//See this file for the definitions of the digits 0 to 9



int main (void){

	char   digit;
	int digit_num=0;                          								//defines number of next digit on the display
	const char* string_ptr = 0;              								//"string_pointer" will be loaded with the address of an array

	setup_UNO_extra;

	String_to_PC("\r\nSend digits?");

	while(1){                               								//Infinite loop
		digit_num = 0;                          							//First digit on display
		while (digit_num <= 7){                								//start of "while();" loop used to fill up the display with 8 digits
			digit = waitforkeypress();              						//user enters digit (0 to 9) at the PC keyboard

			switch(digit){                          						//The appropriate address is loaded into location "string_pointer"
				case '0': string_ptr = zero; break;     					//The address of array zero is loaded into location "string_ptr"
				case '1': string_ptr = one; break;
				case '2': string_ptr = two; break;
				case '3': string_ptr = three; break;
				case '4': string_ptr = four; break;
				case '5': string_ptr = five; break;
				case '6': string_ptr = six; break;
				case '7': string_ptr = seven; break;
				case '8': string_ptr = eight; break;
				case '9': string_ptr = nine; break;
			default: continue; break;}										//Illegal key press: Go immediately to the start of the inner while loop

			display_num_string(string_ptr, digit_num);						//Send the address of the required string to subroutine "display_num_string();"
		digit_num++;} 														//End of "while();" loop

	waitforkeypress();I2C_Tx_any_segment_clear_all();}}						//clear display and repeat






	/******************************************************************************************************************************/
	void display_num_string (const char* s, int digit_num){					//Subroutine requires a pointer to the string
		int char_ptr=0;														//containing segments used to define a digit
		char letter;

		while(1){
			letter = *(s + char_ptr);// (s[char_ptr]);						//Note these two expressions are equivalent
			switch(letter){													//Work through the segments contained in the
				case 'a':                                                   //string until '\0' is encountered
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
				case 'g': I2C_Tx_any_segment(letter, digit_num);break;      //Update display one segment at a time
				case 0:  return; break;                                     //zero indicates the end of the string
			default: break;}char_ptr++;}}									//incrementing "char_ptr" steps through the string
																			//selecting segment letters in turn

			
