/*
 Basic_UART.c running at 9600 Baud
 Uses 20MHz clock
 ATtiny has signature bytes 0x1E, 0x94, 0x24
 Default value of CLK_PER is 3.333MHz
 All CLKCTRL registers are left if their default states
 
 TxD pin is PB2 pin 9
 RxD pin is PB3 pin 8
 */

void fraction_to_decimal_string(long, long, char*);
long product_AB(long, long);
long divide_A_by_B(long, long);
long A_modulo_B(long, long);

#define device_type 1606
#define text_bytes  2048

//#define device_type 806
//#define text_bytes  1024




#include "Floating_point_arithmetic.h"

int main(void)
{char keypress, User_response;
	long Num_1, Num_2;
			
	setup_HW; 
	
	User_prompt;
	while(1){
			
	if(User_response == 'R'){
		while(1){
			string_to_PC("\r\nNum_1?  ");
			Num_1 = Num_from_KBD(Int_array);
			if(Num_1)Num_to_PC(10,Num_1); else {SW_reset;} 	

			string_to_PC("\r\nNum_2?  ");
			Num_2 = Num_from_KBD(Int_array);Num_to_PC(10,Num_2);newline();

			string_to_PC("Decimal fraction is   ");

			if (Num_1 > Num_2)fraction_to_decimal_string(Num_2, Num_1, Int_array);
			else fraction_to_decimal_string(Num_1, Num_2, Int_array);
			}
		}
	
		
	if(User_response == 'r'){
		string_to_PC("\r\nEnter floating point number then power then x or AOK\r\n");
		Float_num = Float_from_KBD(Float_array);										//Acquire FPN
		if(!(Float_num)){SW_reset;}		
		power = Float_from_KBD(power_array);											//Plus exponent
				
		while(1){
		if (power){CCP = 0xD8;WDT.CTRLA = 0x07;}										//WDTout if arithmetic result is excessively large or small
		expt = ftoaL(pow(Float_num, power), Float_array);								//Raise FPN to the power of the exponent; and print new FPN; returns the exponent of the result
		CCP = 0xD8;WDT.CTRLA = 0x0;														//Cancel WDTout
		
		itoa(expt, expt_array, 10);														//Print the exponent
		
		string_to_PC("\r\n");
		if (Float_array[0] == '.')char_to_PC ('0'); 
		string_to_PC(Float_array);
		if (expt){string_to_PC(" x 10 pow ");
		string_to_PC(expt_array);}
		keypress = waitforkeypress();
		if (keypress == 0x1B)break;														//Escape keypress
		if (keypress == 'x')power += 1.0;												//Increment the power
		else power -= 1.0;																//Decrement the power
		
		}}}
		}




/**************************************************************************************************/
void fraction_to_decimal_string(long remainder, long denominator, char digits[]){
	long result;
	int m=0;
	char digit;
	
	string_to_PC("0.");

	do{m+= 1;
		result = divide_A_by_B(product_AB(remainder,10),denominator);	//uses local routine for divide function
		remainder = A_modulo_B(product_AB(remainder,10),denominator);	//uses local function to provide the remainder
		digit = result;
		char_to_PC(digit+'0');
		Timer_TCA(TCA_10ms);}while(m <=20);
	}



	/**************************************************************************************************/
	long product_AB(long A, long B){									//Product is simply multiple addition
		long product = 0;
		while(B){product += A; B--;}									//Check with paper and pencil
	return product;}



	/**************************************************************************************************/
	long divide_A_by_B(long A, long B){									//Division is multiple subtraction while result exceeds zero
		long result = 0;
		while(A>0){A-=B;result++;}
		if(!(A))return result;
	else return result-1;}



	/**************************************************************************************************/
	long A_modulo_B(long A, long B){									//Check with paper and pencil
	return (A - product_AB(divide_A_by_B(A,B),B));}


	
