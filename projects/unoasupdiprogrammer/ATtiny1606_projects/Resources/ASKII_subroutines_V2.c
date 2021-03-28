

long Num_from_KBD(char*);
char decimal_digit (char);
char wait_for_return_key(void);
float Float_from_KBD(char *);

void float_string_to_display(void);

void Num_to_PC(char, long);		
void SBtoAL(char*, long, char);
void NumericString_to_PC(char*);	





/***************************************************************************************************************************************/
char decimal_digit (char data){
if (((data > '9') || (data < '0')) )return 0;
else return 1;}


/***************************************************************************************************************************************/
char wait_for_return_key(void){  									//Returns keypress
char keypress;														//but converts \r\n, \r or \n to \r
while(!(USART0.STATUS & USART_RXCIF_bm )); 
keypress = char_from_PC();
if((keypress == '\r') || (keypress == '\n')){
if (isCharavailable(1)){char_from_PC();}keypress = '\r';}
return keypress;}





/***************************************************************************************************************************************/
long Num_from_KBD(char digits[]){                                   //Acquires an integer string from the keyboard and returns the binary equivalent
char keypress;
long Int_num;

for(int n = 0; n<=8; n++) digits[n] = 0;            				//Clear the buffer used for the string
do
{while(!(USART0.STATUS & USART_RXCIF_bm ));							//Wait for first keypress
keypress = char_from_PC();} 


while ((!(decimal_digit(keypress)))
&& (keypress != '-'));												//Ignore keypress if it is not OK
digits[0] = keypress;

while(1){
if ((keypress = wait_for_return_key())  =='\r')break;              //Detect return key press (i.e \r or\r\n)
if ((decimal_digit(keypress)) || (keypress == '\b')\
 || (keypress == '-'))

{if (keypress == '\b'){												//Backspace key
for (int n = 0; n <= 7; n++)
digits[n] = digits[n + 1];}

else
{for(int n = 8; n>=1; n--)                                         //Shift display for each new keypress
digits[n] = digits[n-1];
digits[0] = keypress;  }                                           //Add new keypress           
}}                                                
string_length = strlen(Int_array);
reverse(Int_array, string_length);
Int_num = atol (Int_array);

return Int_num;}




/***************************************************************************************************************************************/
float Float_from_KBD(char digits[]){                    		//Acquires an integer string from the keyboard and returns the binary equivalent
char keypress;

for(int n = 0; n<=11; n++) digits[n] = 0;						//Clear the buffer used to the string
do
{while(!(USART0.STATUS & USART_RXCIF_bm )); 
keypress = char_from_PC();} 
while ((!(decimal_digit(keypress)))
&& (keypress != '-')
&& (keypress != '.'));

digits[0] = keypress;

while(1){
if ((keypress = wait_for_return_key())  =='\r')break;			//Detect return key press (i.e \r or\r\n)

if ((decimal_digit(keypress)) || (keypress == '.')
|| (keypress == '\b')|| (keypress == '-')
|| (keypress == 'E') || (keypress == 'e'))

{if(keypress == '\b'){for (int n = 0; n <= 7; n++)
digits[n] = digits[n + 1];}

else
{for(int n = 11; n>=1; n--)										//Shift display for each new keypress except '.'
digits[n] = digits[n-1];
digits[0] = keypress;}											//Add new keypress           
}}                                                  

string_length = strlen(digits);
reverse(digits, string_length);
string_to_PC(digits);
char_to_PC('\t');

return atof(digits);}




/*********************************************************************/
void Num_to_PC(char radix, long long_num){
	char array[12];															//Long has 10 chars + sign + null terminator
	SBtoAL(array, long_num, radix);											//calls the Binary to askii subroutine
NumericString_to_PC(array);}												//Prints characters in reverse order

/*********************************************************************/
void SBtoAL(char array[], long num, char radix){							//Signed Binary to Askii
	int m=0;
	long sign;

	if (num == 0x80000000){
		switch(radix){
			case 10: array[0] = '8';array[1] = '4'; array[2] = '6';				//0x80000000 * -1 = 0
			array[3] = '3';array[4] = '8';array[5] = '4'; array[6] = '7';
			array[7] = '4';array[8] = '1';array[9] = '2';
			array[10] = '-'; array[11] = '\0'; break;

			case 16: array[0] = '0';array[1] = '0'; array[2] = '0';				//0x80000000 * -1 = 0
			array[3] = '0';array[4] = '0';array[5] = '0'; array[6] = '0';
			array[7] = '8';array[8] = '-';array[9] = '\0';
		array[10] = '\0'; array[11] = '\0'; break; } return;}

		for (int n = 0; n <=11; n++)array[n] = 0;
		if ((sign = num) < 0) num = num * (-1);

		do {array[m] = num % radix;
			if(array[m] < 10)array[m] += '0'; else array[m] += '7';
		m++;} while ((num = num/radix) > 0);
		if (sign < 0) {array[m] = '-';m++;}}


		/*********************************************************************/
		void NumericString_to_PC(char* s){
			int n=0;
			while (s[n] != '\0')n++;										//scroll to end of string counting the number of characters
		for(int m = n; m; m--)char_to_PC(*(s + m-1));}						//print last character first






