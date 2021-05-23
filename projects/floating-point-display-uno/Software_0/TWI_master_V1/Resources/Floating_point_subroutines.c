

signed char Format_for_Display(char*, char, signed char);
void ftoaL(float, char *);														//Local version of the float to askii routine
void reverse(char *, int);
long longToStr(long , char *, int );




/*********************************************************************************************************************************/
signed char Format_for_Display(char* array, char sign, signed char expt){		//Receives FP_strings with MSB or sign in location 0 (RH end of string/display)
																				//Takes positive numbers ONLY but adds a negative sign bit if necessary
	int array_ptr, LSB_ptr,dp_ptr;
	char E_space;																//Space required on display for exponential notation
	
	if ((array[0] == '9') || ((array[0] == '.') && (array[1] == '9'))){			//If array zero is 9 shift it one place to the right
		for(int m = 0; m <= 14; m++)array[15-m] = array[14-m];					//to allow for the possibility of rounding
	array[0] = '0'; expt += 1;}													//Exponent must be incremented
	
	
	for (int m = 15; m; m--)													//Convert trailing zeros to null characters
	{if (array[m] == 0) continue;
		if (array [m] == '0') array[m] = 0;
	else break;}

	for (int m = 15; m; m--)													//Identify location of the least significant digit
	{LSB_ptr = m;
		if (array[m] == 0) continue;
	else  break;}
	
	for (int m = 0; m <= 15; m++)												//Identify location of the decimal point
	{dp_ptr = 15 - m;
		if (array[15 - m] != '.') continue;
	else  break;}
	
	if (array[LSB_ptr] == '.') {array [LSB_ptr + 1] = '0';	}					//Rounding is not to be implemented
	

	/*
	Round last decimal digit only.
	Only keep rounding if digit 9 is involved: i.e. 0.679996 becomes 0.68
	On the RHS of the decimal point replace trailing zeros with a null char
	On the LHS of the decimal point replace trailing zeros with a '0' char
	*/

	else{
				
		if(LSB_ptr >= 5){
			
			if(array[LSB_ptr] > '5')											//Round least significant digit
			{array[LSB_ptr] = 0; array[LSB_ptr-1] += 1;}	
			if(array[LSB_ptr] < '5'){array[LSB_ptr] = 0;}	
			if(array[LSB_ptr] != '5')LSB_ptr -= 1;
			
			array_ptr = LSB_ptr;
			
			while (array_ptr > dp_ptr)											//Round on RHS of decimal point
			{if (array[array_ptr] == ':')											//'9' + 1 = ':'
				{array[array_ptr] = 0;array[array_ptr-1] += 1;}
			array_ptr -= 1;}
						
			
			if ((array[array_ptr +1] == '9')&&
			(array[array_ptr +2] == '9')&&
			(array[array_ptr +3] == '9'))											//Avoid displaying .999
			{for (int m = array_ptr +1; m <= 15; m++)
			array[m] = 0; array[array_ptr] += 1;}
			
			
			if ((array[array_ptr +1] == '0')&&
			(array[array_ptr +2] == '0')&&
			(array[array_ptr +3] == '0'))											//Avoid displaying .000
			{for (int m = array_ptr +1; m <= 15; m++)
			array[m] = 0;}
			
			
			
			if (array[array_ptr] == ('.' + 1))									//if decimal point is incremented restore it
			{if (array[0]=='0')expt -= 1;
				array[array_ptr--] -= 1;
			array[array_ptr] += 1;}
			
			while (array_ptr > 0)													//Round on LHS of decimal point
			{if (array[array_ptr] == ':')
				{array[array_ptr] = '0';array[array_ptr-1] += 1;}
			array_ptr -= 1;}
		
		for (int m = 15; m; m--)													//Convert additional trailing zeros to null characters
		{if (array[m] == 0) continue;
			if (array [m] == '0') array[m] = 0;
		else break;}
		
		}}
		
		
		/***********************************************************************/
		
		array_ptr = 0;
		if (array[0] == '.')array[0] = '0' | 0x80;								//Convert a decimal point in location zero to "0."
		
		else
		{for(int m = 0; m <= 15; m++)											//Locate decimal point
			{if (array[m] != '.' )continue;
			else array_ptr = m; break;}											//Combine it with the preceding digit
			array[array_ptr-1] |= 0x80;
		for (int m = array_ptr; m <=14; m++)array[m] = array[m+1];}				//Shift array to fill the space left by the decimal point
		
		if (array[0] == '0'){expt -= 1;											//Location zero still empty:  Shift array one place to the right
		for (int m = 0; m <= 14; m++) array [m] = array[m+1];}					//Restore exponent to its original value
		
		if(sign == '-')
		{for(int m = 0; m <= 14; m++)array[15-m] = array[14-m];					//For negative numbers shift the array once place to the right
		array[0] = '-';}														//and add the minus sign
		
		for(int m = 0; m <= 15; m++)Non_exp_array[m] = 0;						//Clear array (Contains FPN but no exponent)
				
		if (!(expt)){															//If there is no exponent right justify string on display
			for(int m = 0; m <= 15; m++)										//First get the length of the string
			{array_ptr = m; if (array[m]  == 0)break;}
			array_ptr = 8 - array_ptr;											//Then shift it.
			while(array_ptr){for (int m = 15; m; m--)
				{array[m] = array[m-1];} array[0] = 0; array_ptr -= 1;}}
				
				else{															//If there is an exponent overwrite the RH display digits
					E_space = 2;												//with the Exponential notation
					if (expt >= 10) E_space = 3;								//and leave the number at the left hand end of the display
					if (expt <= -10)E_space = 4;
					if ((expt < 0) && (expt > (-10)))E_space = 3;
					
					for(int m = 0; m <= 15; m++)Non_exp_array[m] = array[m];	//Save array before overwriting with exponent
					
					switch (E_space){
						case 2:	array[7] = expt + '0';array[6] = 'E';break;		//E1 to E9
						case 3:	if (expt > 0)									//E10, E11, E12......etc
						{array[7] = (expt%10) + '0';
						array[6] = ((expt/10)%10) + '0';array[5] = 'E';}
						if (expt < 0)											//E-1 to E-9
						{array[7] = expt*(-1) + '0';
						array[6] = '-';array[5] = 'E';}
						break;
						case 4:	array[7] = ((expt*(-1))%10) + '0';				//E-10, E-11, E-12....etc
						array[6] = (((expt*(-1))/10)%10) + '0';
						array[5] = '-';array[4] = 'E';
						break;
					}}
					
					Display_mode = 1;											//Displays a number permanently		
					
										
					if (expt){													//Shift the number right until there is only one blank
					array_ptr = 15;												//digit between it and the E sign
					while (array[array_ptr] != 'E'){array_ptr -= 1;}
					if (!(array[array_ptr-1])){									//Exit if there are no blanks
					array_ptr -= 2;
					while ((!(array[array_ptr])) || (array[array_ptr]) == '0')
					{int ptr = 0;
					for(int m = 0; m < array_ptr; m++)
					{array[array_ptr-m] = array[array_ptr-m-1];}
					array[ptr++] = 0;}}}
					
							
					return expt;}








/***************************************************************************************************************************************/
void ftoaL(float Fnum, char FP_string[]){
	int afterpoint = 0;
	long ipart, Fnum_int;
	char sign = '+';
	signed char expt;
	
	if (Fnum < 0){sign = '-'; Fnum *= (-1);}									//Convert negative numbers to positive ones and set the sign character
	
	for(int m = 0; m <= 15; m++) FP_string[m] = 0;								//Clear the floating point array
	
	Fnum_int = (long)Fnum;														//Obtain integer part of the number
	
	if (Fnum_int < 10)afterpoint = 5;											//Number of decimal places is matched to display length
	if ((Fnum_int >= 10) && (Fnum_int < 100))afterpoint = 4;
	if ((Fnum_int >= 100) && (Fnum_int < 1000))afterpoint = 3;
	if ((Fnum_int >= 1000) && (Fnum_int < 10000))afterpoint = 2;
	
	expt = 0;																	//Convert very large and small numbers to scientific form
	if (Fnum  >= 10000) {while (Fnum >= 10)
	{Fnum /= 10; expt += 1;}afterpoint = 5;}
	
	if(Fnum < 0.01) {while (Fnum < 1){Fnum *= 10; expt -= 1;}}
	
																				//FP to askii routines taken from "https://www.geeksforgeeks.org/convert-floating-point-number-string/"
	ipart = (long)Fnum;															//Obtain integer part of FP number
	float fpart = Fnum - (float)ipart;											//Obtain floating part
	long i = longToStr(ipart, FP_string, 0);									//Convert integer part to string

	if (afterpoint != 0){														//Add Decimal part to the string
		FP_string[i] = '.';
		fpart = fpart * pow(10,afterpoint);
	longToStr((long)fpart, FP_string + i + 1, afterpoint);}
	
	expt = Format_for_Display(FP_string, sign, expt);
}



/***************************************************************************************************************************************/
long longToStr(long x, char str[], int d)
{
	int i = 0;
	while (x)
	{   str[i++] = (x%10) + '0';
	x = x/10; }
	
	while (i < d)
	str[i++] = '0';
	reverse(str, i);
	str[i] = '\0';
return i; }


/***************************************************************************************************************************************/
void reverse(char *str, int len)
{
	int i=0, j=len-1, temp;
	while (i<j)
	{   temp = str[i];
		str[i] = str[j];
		str[j] = temp;
	i++; j--; }}

	