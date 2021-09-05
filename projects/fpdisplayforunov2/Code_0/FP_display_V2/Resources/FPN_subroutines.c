
void ftoaL(float);
long longToStr(long, char*, int);
void reverse(char *, int);
float Local_Round_function(float, char);
void Add_exponent (void);



/***************************************************************************************************************************************/
void ftoaL(float Fnum){
	int afterpoint = 0;
	long ipart, Fnum_int;
	
	
	sign = '+';
	if (Fnum < 0){sign = '-'; Fnum *= (-1);}										//Convert negative numbers to positive ones and set the sign character
	
	clear_temp_buffer;
	clear_display_buffer;
	
	Fnum_int = (long)Fnum;															//Obtain integer part of the number
	
	if (Fnum_int < 10)afterpoint = 5;												//Number of decimal places is matched to display length
	if ((Fnum_int >= 10) && (Fnum_int < 100))afterpoint = 4;
	if ((Fnum_int >= 100) && (Fnum_int < 1000))afterpoint = 3;
	if ((Fnum_int >= 1000) && (Fnum_int < 10000))afterpoint = 2;
	
	expt = 0;																		//Convert very large and small numbers to scientific form
	if (Fnum  >= 10000) {while (Fnum >= 10)
		{Fnum /= 10; expt += 1;}afterpoint = 5;}
		
		if(Fnum < 0.01) {while (Fnum < 1){Fnum *= 10; expt -= 1;}}
		
		Fnum = Local_Round_function(Fnum, afterpoint);
		
		//FP to askii routines taken from "https://www.geeksforgeeks.org/convert-floating-point-number-string/"
		
		ipart = (long)Fnum;															//Obtain integer part of FP number
		float fpart = Fnum - (float)ipart;											//Obtain floating part
		long i = longToStr(ipart, temp_buffer, 0);									//Convert integer part to string
				
		if (afterpoint != 0){														//Add Decimal part to the string
			temp_buffer[i] = '.';
			
			
			fpart = fpart * pow(10,afterpoint);
			
			longToStr((long)fpart, temp_buffer + i + 1, afterpoint);}
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


		
/***************************************************************************************************************************************/
float Local_Round_function(float num, char afterpoint){
	float inc = 1;
	for(int m = 0; m < afterpoint; m++){
	inc = inc/10.0;}
	inc = inc*0.55;																	//Attempt to avoid trailing nines
	num = num + inc;
	return num;}



/***************************************************************************************************************************************/
void Add_exponent (void)
{
	char E_space;
																					//If there is an exponent overwrite the RH display digits
	E_space = 2;																	//with the Exponential notation
	if (expt >= 10) E_space = 3;													//and leave the number at the left hand end of the display
	if (expt <= -10)E_space = 4;
	if ((expt < 0) && (expt > (-10)))E_space = 3;
	
	
	switch (E_space){
		case 2:	display_buffer[0] = expt + '0';display_buffer[1] = 'E';break;		//E1 to E9
		case 3:	if (expt > 0)														//E10, E11, E12......etc
		{display_buffer[0] = (expt%10) + '0';
		display_buffer[1] = ((expt/10)%10) + '0';display_buffer[2] = 'E';}
		if (expt < 0)																//E-1 to E-9
		{display_buffer[0] = expt*(-1) + '0';
		display_buffer[1] = '-';display_buffer[2] = 'E';}
		break;
		case 4:	display_buffer[0] = ((expt*(-1))%10) + '0';							//E-10, E-11, E-12....etc
		display_buffer[1] = (((expt*(-1))/10)%10) + '0';
		display_buffer[2] = '-';display_buffer[3] = 'E';
		break;}}