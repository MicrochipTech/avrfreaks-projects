


//Proj_3E:  Let the mini-OS deal with displaying the numbers




#include "Proj_3E_header_file.h"



int main (void){
	long Num_1;
	char digits[8];
	int counter = 0;

	setup_UNO_extra;


	String_to_PC("\r\nEnter positive number & terminate \
	\bwith Return key. Then press AK.\r\n");

	Num_1 = Num_from_KBD_Local(digits);								//Acquires number from the keyboard

	do{
		Num_to_PC_local(10,++counter); Char_to_PC('\t');
		Num_to_PC_local(10,Num_1); newline();   					//Converts numbers strings and sends them to the pc.

		I2C_Tx_long(Num_1);											//Sends number to the display

		waitforkeypress();
	Num_1 = (Num_1 / 2) *3;} while (Num_1 < 66666666);				//Do some arithmetic

	Num_1 = (Num_1 / 3) *2;

	do
	{Num_1 = (Num_1 / 3) *2;										//Do the arithmetic in reverse

		Num_to_PC_local(10,--counter); Char_to_PC('\t');
		Num_to_PC_local(10,Num_1); newline();

		I2C_Tx_long(Num_1);											//Note: division looses the remainder
	waitforkeypress();} while (counter-1);}							//and is not as accurate as multiplication





	/******************************************************************************************************************************/
	long Num_from_KBD_Local(char digits[]){							//Local version, ignores negative number
		unsigned char keypress;										//Resources version does not.
		for(int n = 0; n<=7; n++) digits[n] = 0;					//Clear array

		do
		{keypress =  waitforkeypress();}							//wait for first keypress
		while (!(decimal_digit(keypress)));							//Ignore illegal characters

		digits[0] = keypress;										//Save first one to array
		I2C_Tx_8_byte_array(digits);								//send array to display
		while ((keypress = wait_for_return_key()) != '\r'){			//Continue until return key is pressed

			if (decimal_digit (keypress))							//Ignore illegal characters
			{for(int n = 7; n>=1; n--)digits[n] = digits[n-1];		//Shifts display left for each keypress
				digits[0] = keypress;								//Places new digit in array[0]
			I2C_Tx_8_byte_array(digits);}}							//Updates the display

			newline();
		return I2C_displayToNum();}									//Mini-OS converts display to a long number and sends it to the UNO.




/******************************************************************************************************************************/
	void Num_to_PC_local(char radix, long long_num){
		char array[12];												//Long has 10 chars + sign + null terminator
		SBtoAL_local(array, long_num, radix);						//calls the Binary to ASKII subroutine
	NumericString_to_PC_local(array);}								//Prints characters in reverse order



/******************************************************************************************************************************/
	void SBtoAL_local(char array[], long num, char radix){			//Signed Binary to ASKII	 Radix is usually 10 or 16
	int m=0;

		for (int n = 0; n <=11; n++) array[n] = 0;					//Clear space to save the number in string form
		do {array[m] = num % radix;									//Repeatedly divide the number by the radix to get the digits 
			if(array[m] < 10)array[m] += '0';						//starting with the least significant and convert them to chars 
			else array[m] += '7';									//Hex chars A, B, C, D, E and F
		m++;} while ((num = num/radix) > 0);
		}


/******************************************************************************************************************************/
	void NumericString_to_PC_local(char* s){
	int n=0;
	while (s[n] != '\0')n++;										//Scroll to end of string counting the number of characters
	for(int m = n; m; m--)Char_to_PC(*(s + m-1));}					//Print last character first

