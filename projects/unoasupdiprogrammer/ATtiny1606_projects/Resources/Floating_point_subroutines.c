


/*********************************************************************************************************************************/
signed char ftoaL(float Fnum, char FP_string[]){										//See "https://www.geeksforgeeks.org/convert-floating-point-number-string/"
	int afterpoint = 0;
	long ipart, Fnum_int;
	char sign = '+';
	signed char expt;
	
	if (Fnum < 0){sign = '-'; Fnum *= (-1);}									//Convert negative numbers to positive ones and set the sign character
	
	for(int m = 0; m <= 11; m++) FP_string[m] = 0;								//Clear the floating point array
	
	Fnum_int = (long)Fnum;														//Obtain integer part of the number
	
	if (Fnum_int < 10)afterpoint = 5;											//Number of decimal places is matched to display length
	if ((Fnum_int >= 10) && (Fnum_int < 100))afterpoint = 4;
	if ((Fnum_int >= 100) && (Fnum_int < 1000))afterpoint = 3;
	if ((Fnum_int >= 1000) && (Fnum_int < 10000))afterpoint = 2;
	
	expt = 0;																	//Convert large and small numbers to scientific form
	if (Fnum  >= 10000) {while (Fnum >= 10)
	{Fnum /= 10; expt += 1;}afterpoint = 5;}
	
	if(Fnum < 0.01) {while (Fnum < 1){Fnum *= 10; expt -= 1;}}
	
																				
	ipart = (long)Fnum;															//Obtain integer part of FP number
	float fpart = Fnum - (float)ipart;											//Obtain floating part
	long i = longToStr(ipart, FP_string, 0);									//Convert integer part to string

	if (afterpoint != 0){														//Add Decimal part to the string
		FP_string[i] = '.';
		fpart = fpart * pow(10,afterpoint);
	longToStr((long)fpart, FP_string + i + 1, afterpoint);}
	
	if(sign == '-')
	{for(int m = 0; m <= 10; m++)FP_string[11-m] = FP_string[10-m]; 
	FP_string[0] = '-';}
	return expt;
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

	