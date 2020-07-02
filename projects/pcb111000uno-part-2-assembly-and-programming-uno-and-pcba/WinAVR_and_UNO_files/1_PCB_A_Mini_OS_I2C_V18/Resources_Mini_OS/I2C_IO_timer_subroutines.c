
void timer_T0_sub_with_interrupt(char, unsigned char);
void timer_T1_sub_with_interrupt(char, unsigned int);
void Timer_T1_sub(char, unsigned int);

void Disp_Hex(int);
void Disp_CharS(signed char);
void Disp_CharU(unsigned char);
void Disp_Long(unsigned long, signed char);
void Display_num(long);
void Display_real_num(long);
void displayLongNum(char, long);
void SBtoAL(char*, long, char);
void displayNumericString(char*);
void displayDecimal(char, unsigned long, int);
void float_to_askii(long, signed char, char*);
char decimalOverflow(char, unsigned long, int);
void display_float(char*);
void string_to_slave_I2C(char *);


unsigned long Binary_points_to_Decimal_L (unsigned int, unsigned int);
long Binary_points_to_Decimal_Signed (long);






/*****************************************************************************************/
void timer_T0_sub_with_interrupt(char Counter_speed, unsigned char St_point){ 
TCNT0 = St_point;
TCCR0B = Counter_speed;}



/*****************************************************************************************/
void timer_T1_sub_with_interrupt(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point & 0x00FF;
TCCR1B = Counter_speed;}



/*****************************************************************************************/
void Timer_T1_sub(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point & 0x00FF;
TIFR1 = 0xFF;
TCCR1B = Counter_speed;
while(!(TIFR1 & (1<<TOV1)));
TIFR1 |= (1<<TOV1); 
TCCR1B = 0;}




/*********************************************************************/
void Disp_Hex(int n){ 
unsigned char i; int sign;
for(int m = 0; m <= 5; m++){display_buf[m] = '\0';}
if (n == -32768) {display_buf[0] = '8';display_buf[1] = '6';display_buf[2] = '7';
display_buf[3] = '2'; display_buf[4] = '3'; display_buf[5] = '-';}
else{
if ((sign = n) < 0) n = -n;
i=0; 
do{
display_buf[i] = n % 10  +  '0';
i++;
}while((n/=10)>0);
if (sign < 0)display_buf[i] = '-';}}



/*********************************************************************/
void Disp_CharS(signed char n){ 
unsigned char i; signed char sign;
for(int m = 0; m <= 3; m++){display_buf[m] = '\0';}
if (n == -128) {display_buf[0] = '8';display_buf[1] = '2';display_buf[2] = '1';display_buf[3] = '-';}
else{
if ((sign = n) < 0) n = -n;
i=0; 
do{
display_buf[i] = n % 10  +  '0';
i++;
}while((n/=10)>0);
if (sign < 0)display_buf[i] = '-';}}



/*********************************************************************/
void Disp_CharU(unsigned char n){ 
unsigned char i; 
for(int m = 0; m <= 3; m++){display_buf[m] = '\0';}
i=0; 
do{
display_buf[i] = n % 10  +  '0';
i++;
}while((n/=10)>0);}



/*********************************************************************/
void Disp_Long(unsigned long num, signed char exponent){ 
unsigned char i, display_mode, result[12]; 
signed char   zero_counter, digit_counter, string_length, string_shift;

for(int m = 0; m <= 7; m++){display_buf[m] = '\0';}
for(int m = 0; m <= 9; m++){result[m] = '\0';}

if (exponent >= 8)display_mode = 4;
else{

{i=0; do{
result[i] = num % 10  +  '0'; i++;}while((num/=10)>0);				//convert number to string

if (i > 8) {for(int m = 0; m <= 7; m++){result[m] = result[m+i-8];}}  
if (i < 8) exponent++;}				

zero_counter = 0; while (result[zero_counter] == '0')zero_counter++;	// count the number of trailing zeros
digit_counter = 8 - zero_counter;

if (exponent > 0) display_mode = 1;
if (exponent < 0) {display_mode = 2; exponent = exponent * (-1);}
if (exponent == 0) display_mode = 3;}

switch (display_mode){
case 1:
if(exponent <= (8 - zero_counter))
{for(int m = zero_counter; m <=7; m++) display_buf[m-zero_counter] = result[m];	//shift right to remove all zeros
display_mask = 0; for(int m = 0; m <= (8 - zero_counter - exponent); m++)display_mask = (display_mask << 1) + 1; 
display_mask = display_mask >> 1;}
else{
{for (int m=0; m<exponent; m++)display_buf[m] = result[8 - exponent + m];}}break;
case 2:
case 3:
string_length = digit_counter + exponent;
string_shift = string_length - 6; if(string_shift < 0)string_shift = 0;
for(int m = zero_counter; m <=7; m++) result[m-zero_counter] = result[m];				//shift out trailing zeros

for (int m = 0; m < (digit_counter - string_shift); m++){display_buf[m] = result[m + string_shift];}
for(int m = 0; m < exponent; m++){display_buf[m + digit_counter - string_shift] = '0';}
display_buf[digit_counter - string_shift + exponent] = '_';
display_buf[digit_counter - string_shift + exponent + 1] = '0';
break;
case 4:
{i=0; do{
result[i] = num % 10  +  '0'; i++;}while((num/=10)>0);}				//convert number to string
for (int m = 0; m<= 4; m++){display_buf[7-m] = result[8-m];} display_buf[2] = 'X';  
display_buf[1] = exponent/10 + '0';
display_buf[0] = exponent%10 + '0';
display_mask = 0b11111000;
break;}

for(int p = 0; p < 8; p++)strobe[p] = 0;}					//synchronise strobe



/*********************************************************************/
void Display_num(long n){
unsigned char i; long sign;
for(int m = 0; m <= 7; m++){display_buf[m] = '\0';}
if ((n <= 99999999) && (n >= -9999999))
{
if ((sign = n) < 0) n = -n;
i=0; 
do{
display_buf[i] = n % 10  +  '0';
i++;
}while((n/=10)>0);
if (sign < 0)display_buf[i] = '-';}
else 
{for(int m = 0; m <= 7; m++)display_buf[m] = '_';}}



/*********************************************************************/
void Display_real_num(long number_1){
unsigned int RHS_of_BP_LB, RHS_of_BP_HB;
unsigned long RHSDP;

if(number_1 < 0)RHS_of_BP_HB = ~number_1 & 0x0000FFFF;
else RHS_of_BP_HB = number_1 & 0x0000FFFF;
RHS_of_BP_LB = 0;

RHSDP = Binary_points_to_Decimal_L (RHS_of_BP_LB,  RHS_of_BP_HB);

if(number_1 < 0){
if (!((number_1 >> 16)+1)){display_buf[7] = '-'; display_buf[6] = '0';}	
else displayLongNum(10, ((number_1 >> 16)+1));}

else displayLongNum(10, (number_1 >> 16));
displayDecimal(10, RHSDP, 5);}



/*********************************************************************/
void displayLongNum(char radix, long long_num){
char array[12];							//Long has 10 chars + sign + null terminator	
SBtoAL(array, long_num, radix);			//calls the Binary to askii subroutine
displayNumericString(array);}				//Prints characters in reverse order



/*********************************************************************/
void SBtoAL(char array[], long num, char radix){					//Signed Binary to Askii
int m=0;
long sign;

if (num == 0x80000000){
switch(radix){
case 10: array[0] = '8';array[1] = '4'; array[2] = '6';		//0x80000000 * -1 = 0
array[3] = '3';array[4] = '8';array[5] = '4'; array[6] = '7';
array[7] = '4';array[8] = '1';array[9] = '2';
array[10] = '-'; array[11] = '\0'; break;

case 16: array[0] = '0';array[1] = '0'; array[2] = '0';		//0x80000000 * -1 = 0
array[3] = '0';array[4] = '0';array[5] = '0'; array[6] = '0';
array[7] = '8';array[8] = '\0';array[9] = '\0';
array[10] = '\0'; array[11] = '\0'; break; } return;}

for (int n = 0; n <=11; n++)array[n] = 0;	
if ((sign = num) < 0) num = num * (-1);

do {array[m] = num % radix;
if(array[m] < 10)array[m] += '0'; else array[m] += '7';
m++;} while ((num = num/radix) > 0);
if (sign < 0) {array[m] = '-';m++;}}



/*********************************************************************/
void displayNumericString(char* s){					
int n=0;
while (s[n] != '\0')n++;							//scroll to end of string counting the number of characters
for(int m = n; m; m--)display_buf[7+m-n] = (*(s + m-1));}		//print last character first



/*********************************************************************/
void displayDecimal(char radix, unsigned long Hex, int No_dps){ char print_out_string[12];
unsigned long inc;
Hex += 1000000000;				//define location decimal point
inc = 1;
for(int k = 0; k < (8-No_dps); k++)inc = inc*10;
inc = inc*5;
Hex += inc;
SBtoAL(print_out_string, Hex, radix);
for(int k = 0; k <= (8-No_dps); k++)print_out_string[k] = ' ';
print_out_string[9] = '.'; 

{int m=8, n=0; while(display_buf[m-1]) m--; while(m) {display_buf[m-1] = print_out_string[9-n];m--, n++;}}} 



/*********************************************************************/
void float_to_askii(long number, signed char expnt, char *print_out_string)
{char expnt_string[12];
long inc = 1, RHSDP;
int space_counter = 0; 
int char_counter=0;
int No_dps = 7;
char overflow = 1;

for(int m = 0; m<=15; m++)print_out_string[m] = 0;

RHSDP = Binary_points_to_Decimal_Signed(number);			//to a decimal number
if(number < 0)RHSDP = ~RHSDP;

if (decimalOverflow(10, RHSDP, No_dps))
{RHSDP += 1000000000;				//define location decimal point
overflow = 0;
for(int k = 0; k < (8-No_dps); k++)inc = inc*10;
inc = inc*5;
RHSDP += inc;

SBtoAL(print_out_string, RHSDP, 10);
for(int k = 0; k <= (8-No_dps); k++)print_out_string[k] = '\0';}

else print_out_string[8] = '0';

print_out_string[9] = '.';
print_out_string[10] = '0';

if(number < 0)print_out_string[11] = '-';
if (overflow == 1){print_out_string[10] = '1'; }

while ((!(print_out_string[space_counter])) || (print_out_string[space_counter]== '0')) space_counter++;


{int m = 0;
do{print_out_string[m] = print_out_string[m + space_counter]; m++;}
while(m <= (15- space_counter));}


if(expnt){SBtoAL(expnt_string, expnt, 10);
while (expnt_string[char_counter])char_counter++;
char_counter +=2;

{int m = 0;
do{print_out_string[15 - m] = print_out_string[15 - m - char_counter]; m++;}
while(m <= (15- char_counter));}

{int m = 0; while(m < (char_counter-2)) {print_out_string[m] = expnt_string[m]; m++;}
print_out_string[m] = 'X';
print_out_string[m+1] = ' ';}}

Sc_Num_string_length =0; while(Sc_Num_string[Sc_Num_string_length])Sc_Num_string_length++;
Sc_Num_string_pointer=0;
display_char_skip_counter = 0;}



/*********************************************************************/
char decimalOverflow(char radix, unsigned long Hex, int No_dps){
long inc=1;

Hex += 1000000000;				//define location decimal point
for(int k = 0; k < (8-No_dps); k++)inc = inc*10;
inc = inc*5;
Hex += inc;
if(Hex >= 2000000000)return 0; else return 1;}



/*********************************************************************/
void display_float(char Sc_Num_string[])
{char display_mode;

Initialise_I2C_master_read;
TWCR = (1 << TWINT) | (1 << TWEN);				// Acquire single char: display mode
while (!(TWCR & (1 << TWINT)));
display_mode = TWDR;
TWCR =  (1 << TWINT) | (1 << TWEN);

switch (display_mode){
case  '0':
for(int p = 8; p; p--)display_buf[p] = display_buf[p-1];

if(Sc_Num_string_length-Sc_Num_string_pointer)
{display_buf[0] = Sc_Num_string[Sc_Num_string_length-Sc_Num_string_pointer-1];
Sc_Num_string_pointer++; }

else
{if (display_char_skip_counter <=2){display_buf[0] = 'a'; display_char_skip_counter++;} 
else{display_char_skip_counter = 0;Sc_Num_string_pointer=0; }}

break;
case '1': break;
case '2': mode = 0; 
timer_2_counter=0;				//Initialsise timer_2_counter to zero
TCNT2 = 0;						//Reset counter register
TCCR2B = 0;						//Stop the clock
TIMSK2 &= (~(1 << TOIE2));
break;}}

void string_to_slave_I2C(char strg_ptr[]){
int String_length;
String_length = 0; while (strg_ptr[String_length])String_length++;
	while(1){
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	TWDR = 0x02;
	TWCR = (1 << TWINT) | (1 << TWEN); 
	while (!(TWCR & (1 << TWINT)));
	if (TWSR == 0x18)break;	}
	TWDR = (String_length/10)+'0';
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	TWDR = (String_length%10)+'0';
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	for(int m = 0; m<=(String_length - 1); m++)
	{TWDR = strg_ptr[m];
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));}
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);}

