
long Num_from_KBD(char*);
long Hex_from_KBD(void);											//?????CHANGED was char * to digits[]
long Num_from_KBD_with_echo(char*);
void Binary_to_PC(long);						//was sendBinary()
char binary_char_from_KBD(void);

char decimal_digit (char);
char hex_digit (char);
long I2C_displayToNum(void);	
void I2C_Tx_8_byte_array(char*);



/*******************************************************************************/
long Num_from_KBD(char digits[]){
char keypress;
for(int n = 0; n<=7; n++) digits[n] = 0; 

do
{keypress =  waitforkeypress();} 
while ((!(decimal_digit(keypress)))&& (keypress != '-'));						//(non_decimal_char(keypress));  //Not -,0,1,2,3,4,5,6,7,8 or 9
digits[0] = keypress;
I2C_Tx_8_byte_array(digits);

while(1){
if ((keypress = wait_for_return_key())  =='\r')break;
if (decimal_digit (keypress))						//012345678or9	:Builds up the number one keypress at a time
{for(int n = 7; n>=1; n--)
digits[n] = digits[n-1];							//Shifts display left for each keypress
digits[0] = keypress;
I2C_Tx_8_byte_array(digits);}}
						
return I2C_displayToNum();}


/*******************************************************************************/
long Hex_from_KBD(void){
char keypress, sign = '+';
long number;

do
{keypress =  waitforkeypress();} 
while ((!(hex_digit(keypress)))&& (keypress != '-'));//(non_hex_char(keypress));

newline(); Char_to_PC(keypress);

if(keypress == '-') {sign = '-'; number = 0;}

else {keypress -= '0';
if (keypress > 9)keypress -= 7;
number =  keypress;}

while(1){
if ((keypress = wait_for_return_key())  =='\r')break;
if (hex_digit (keypress))	
{Char_to_PC(keypress);
keypress -= '0';
if (keypress > 9)keypress -= 7;

number = number * 0x10 + keypress;}}

if(sign == '-')number*= -1;					
return number;}



/*******************************************************************************/
long Num_from_KBD_with_echo(char digits[]){
char keypress, sign = '+';
long number;
for(int n = 0; n<=7; n++) digits[n] = 0; 

do
{keypress =  waitforkeypress();} 
while((!(decimal_digit(keypress)))&& (keypress != '-'));	// (non_hex_char(keypress)); 

newline(); Char_to_PC(keypress);

if(keypress == '-') {sign = '-'; number = 0;}

else {
keypress -= '0';
number =  keypress;}

while(1){
if ((keypress = wait_for_return_key())  =='\r')break;
if (decimal_digit (keypress))		
{Char_to_PC(keypress);
keypress -= '0';

number = number * 10 + keypress;}}

if(sign == '-')number*= -1;					
return number;}



/*******************************************************************************/
void Binary_to_PC(long num){
char counter = 0; unsigned int x;
for (unsigned int m = 16; m; m--)
{if ((num>>16) & (1 << (m-1))) Char_to_PC ('1'); else Char_to_PC('0'); 
counter++; if (!(counter%4))Char_to_PC(' ');}

x=num;
for (unsigned int m = 16; m; m--)
{if (x & (1 << (m-1))) Char_to_PC ('1'); else Char_to_PC('0'); 
counter++;if (!(counter%4))Char_to_PC(' ');}}



/*******************************************************************************/
char binary_char_from_KBD(void){
char keypress=0, LSB, array[8];

for(int m = 0; m<=7; m++)array[m]=0;
do{
LSB = wait_for_return_key(); 
if (LSB == '\r')break;
if((LSB != '0') && (LSB != '1'));

else {keypress = (keypress << 1) + LSB - '0';
array[0] = keypress;
I2C_Tx_BWops(array);
}} while(1);
return keypress;}

