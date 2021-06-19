//void print_long(long );


/**********************************************************************************************/
char isCharavailable (int m){int n = 0;										//For m = 1 waits a maximun of 7.8mS
while (!(Serial.available())){n++;											//before returning zero
if (n>4000) {m--; n = 0;}if (m == 0)return 0;}								//Returns 1 immediately that a char is received
return 1;}



/**********************************************************************************************/
char waitforkeypress(void){
while (!(Serial.available()));
return Serial.read();}



/***************************************************************************************************************************************/
char decimal_digit (char data){
if (((data > '9') || (data < '0')) )return 0;
else return 1;}



/***************************************************************************************************************************************/
char wait_for_return_key(void){  											//Returns keypress
char keypress, temp;														//but converts \r\n, \r or \n to \r
while (!(Serial.available())); 
keypress = Serial.read();
if((keypress == '\r') || (keypress == '\n')){
if (isCharavailable(1)){temp = Serial.read();}keypress = '\r';}
return keypress;}




/***************************************************************************************************************************************/
char Scientific_num_from_KBD(char* array, char* sign){     					//Acquires an integer string from the keyboard and returns the binary equivalent
char keypress;
char array_ptr;
char dp_ptr;																//Holds location of the decimal point
char E_ptr;																	//Holds location of the E char.  If zero there is no exponent.

dp_ptr = 0;
array_ptr = 0;
E_ptr = 0;

for(int n = 0; n<=14; n++) array[n] = 0;                           			//Clear the buffer used for the string

do
{while (!(Serial.available())); 											//Wait for keypress
keypress = Serial.read();} 
while ((!(decimal_digit(keypress)))
&& (keypress != '-'));														//Ignore illegal keypresses including a decimal point at this stage

Serial.write(keypress); 													//Echo keypress

if (keypress == '-') *sign = negative;
else array[array_ptr++] = keypress;											//Save keypress to buffer
 
while(1){																	//continue statement brings program flow back here 
if ((keypress = wait_for_return_key())  =='\r')break;               		//Detect return key press (i.e \r or\r\n)

if ((decimal_digit(keypress)) || (keypress == '.')							//Acceptable keypresses
|| (keypress == '\b') || (keypress == '-')
|| (keypress == 'E') || (keypress == 'e'))

{if(keypress == '\b'){array[--array_ptr] = 0; Serial.write('\b');			//Del key pressed
continue;}																	//Go straight to top of while loop															

{if (keypress == '.')dp_ptr = array_ptr;									//Save location of decimal point


if ((keypress == 'E') || (keypress == 'e')){								//dp requred at end of numeric string if one has not already been entered
if(!(dp_ptr)){array[array_ptr] = '.'; dp_ptr = array_ptr++;}
E_ptr = array_ptr;}

array[array_ptr++] = keypress;}												//Save keypress to string

Serial.write(keypress);}}                                                   //Update display includes "cr_keypress"                                                 

if(!(dp_ptr)){array[array_ptr] = '.'; dp_ptr = array_ptr;}					//Terminate array in dp if one has not already been entered


Serial.write('\t');
return E_ptr;}																//Return location of the first decimal place




/**************************************************************************************************************************/
void Round_print_buff(char * array)

{char sign = positive;
int expt_10 = 0;
char E_ptr = 0, dp_ptr = 0;
long num;
char string_length;


if (array[0] == '-') {sign = negative;									//If negative number save sign
for(int m = 0; m <= 13; m++) array[m] = array[m+1];}					//and shift array one place left

if (array[0] == '0')													//If leading zero present: remove it
{for(int m = 0; m <= 13; m++)array[m] = array[m+1]; 
array[14] = 0;}

for(int m = 0; m <= 14; m++) 
{if(array[m] == 'E'){E_ptr = m; break;}}								//Scan array for exponent
if(E_ptr){expt_10 = atoi(array + E_ptr + 1);							//and save it
for(int m = E_ptr; m <= 14; m++) array[m] = 0;}							//Clear the exponent

for(int m = 0; m <= 13; m++) 											//Scan array for dp
{if(array[m] == '.'){dp_ptr = m; break;}}								//and save its location
for(int m = dp_ptr; m <= 13; m++)array[m] = array[m+1];					//Overwrite the dp
array[14] = 0;

string_length = strlen(array); 											//String length: No dp, no sign, no exponent  and no leading zero.

num = atol(array);														//Convert string to number
num += 5;																//round final digit
num /= 10.0;															//and remove it

ltoa(num, array, 10);													//Convert number back to a string

if(string_length == strlen(array)) {dp_ptr += 1; }						//If leading one generated shift dp one place

for (int m = 14; m > dp_ptr; m--)array[m] = array[m - 1];				//Reinsert the dp
array[dp_ptr] = '.';

if(sign == negative){for(int m = 14; m; m--)array[m] = array[m-1];		//Reinsert the negative sign if needed
array[0] = '-';}

for (int m = 0; m <= 14; m++){											//Reinsert the exponent
if (!(array[m])){E_ptr = m; break;}}
if (expt_10){
array[E_ptr] = 'E';
itoa(expt_10, array + E_ptr + 1, 10);}

if (array[0] == '.')													//Reinstate leading zero if needed
{for(int m = 14; m; m--)array[m] = array[m-1]; array[0] = '0';}}

