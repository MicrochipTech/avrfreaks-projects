


//Proj_6E_bitwise_ops


/*
At the user prompt “R…...R…...” press
R or r and follow the instructions.
*/



#include "Proj_6E_header_file.h"


char Op(char, char);


char digits[8];
char BWOp, comp;


int main (void){
	char op_code;
	char X = 0;
	char User_response, keypress;
	char invalid_key_press;

	setup_UNO_extra;

	User_prompt;

	switch (User_response)
	{ case 'R':

		String_to_PC("\r\nSelect logical operation \r\n\
		\b\b1 for << X	Shifts a number left\r\n\
		\b\b2 for >> X	Shifts a number right \r\n");
		String_to_PC("\
		\b\b3 for << X	Rotate a number left\r\n\
		\b\b4 for << X	Rotate a number right\r\n");
		String_to_PC("\
		\b\b5 for | (1 << X)	Changes just one bit by setting it to 1.\r\n\
		\b\b6 for &(~(1 << X)	Changes just one bit by setting it to 0.\r\n");
		String_to_PC("\
		\b\b7 for XOR(1 << X)	Changes just one bit by reversing it.\r\n\
		\b\b8 for &(1 << X)	Tests a single bit\r\n\
		\b\bPress x for new mode\r\n");

		waitforchar();
		while(1){
			op_code = receiveChar();

			switch(op_code){
				case '1':
				case '2':
				case '3':
				case '4': String_to_PC("Mode "); Char_to_PC(op_code);	String_to_PC("\tX? 1-7");
				X = waitforkeypress()-'0'; break;

				case '5':
				case '6':
				case '7':
				case '8': String_to_PC("Mode "); Char_to_PC(op_code); X = 1;  break;
			default: Char_to_PC('?');waitforchar();continue;break;}							//Illegal key press: Return to top of while loop
			
			String_to_PC("\tPress any key   ");
			dig_0 = PRN_8bit_GEN();															//Use random numbers
			dig_1 = X ;
			dig_2 = dig_0;

			do{
				I2C_Tx_BWops(digits);														//update the display
				keypress=waitforkeypress();
				
				dig_2 = logical_op(X,dig_0, op_code); 										//perform logical op and update dig_2

				switch (op_code){
					case '1':
					case '2':
					case '3':
					case '4': X += dig_1; break;											//update X for op-codes 1 to 4
				default: dig_1 = X; X = (X <<1)%255;	break;}								//update X and dig_1 for op-codes 5 to 8
			}while(keypress != 'x');

			I2C_Tx_any_segment_clear_all();
			if(keypress == 'x')
			String_to_PC("New mode?\r\n");
		waitforchar();}break;
		
		
		case 'r':
		
		String_to_PC("\r\nSelect OP: 	| 	^ 	& 	~| 	~^ 	or	~&   \r\n\
		\b\bThen press any key (x to escape).\r\n\r\n");									//2 backspaces required because of auto indent function
		while(1){

			comp = 0;																		//Complement i.e. NOT AND

			while(1)
			{invalid_key_press = 0;
				switch(BWOp = waitforkeypress())											//Bit Wise Operation
				{
					case '~': comp = 1;break;
					case '|':String_to_PC("\r\nOR");break;
					case '^': String_to_PC("\r\nExclusive-OR");break;
					case '&':String_to_PC("\r\nAND");break;
				default: invalid_key_press = 1;break;}

				if (!(invalid_key_press))break;
			else Char_to_PC('?');}

			if (comp == 1)																	//A second keypress required
			{while(1)
				{invalid_key_press = 0;
					switch(BWOp = waitforkeypress())

					{case '|': String_to_PC("\r\nNOR");break;
						case '^': String_to_PC("\r\nExclusive-NOR");break;
						case '&': String_to_PC("\r\nNAND");break;
					default: invalid_key_press = 1;break;}

					if (!(invalid_key_press))break;
				else Char_to_PC('?');}}


				while (waitforkeypress() !='x'){
					digits[0] = PRN_8bit_GEN();
					digits[1] = PRN_8bit_GEN();
					digits[2] =  Op(digits[0] , digits[1]);
				I2C_Tx_BWops(digits);}

				I2C_Tx_any_segment_clear_all();

			String_to_PC("\tAnother_OP");}break;}}
			




			/*************************************************************************************/
			char logical_op(char X, char Y, char op_code){
				char result = 0, n = 0, num;
				switch (op_code){

					case '1': result = Y << X;break;
					case '2': result = Y >> X;break;
					case '3': Rotate_Right_cyclical; result = Y; break;
					case '4': Rotate_left_cyclical; result = Y; break;

					case '5': result = Y | X; break;
					case '6': result = Y &(~(X)); break;
					case '7': result = Y ^ (X); break;
				case '8': result = Y & (X); break;}
			return result;}




			/*************************************************************************************/
			char Op(char A, char B)
			{char result=0;
				switch (BWOp){
					case '|': result = A | B; break;
					case '^': result = A ^ B; break;
				case '&': result = A & B; break;}
				if (comp == 1) result = ~result;
			return result;}

