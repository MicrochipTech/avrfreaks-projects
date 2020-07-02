
void Real_num_string_from_KBD(char*);
void decimal_part_from_KBD (char*, char);
long Fixed_pt_number_from_KBD(char*);
long fpn_from_KBD(char*, signed char*);

void Decimal_to_PC(char, unsigned long, int);				//was sendDecimal
char decimalOverflow(char, unsigned long, int);	
void RealNum_to_PC(long, int);
void fpn_to_PC(long, signed char);



/*****************Real_from_KBD*****************************/
void Real_num_string_from_KBD(char digits[]){		//Acquires numerical string
char keypress;
for(int n = 0; n<=7; n++) digits[n] = 0; 
keypress =  waitforkeypress();
if(decimal_digit(keypress) || (keypress == '-'))

{digits[0] = keypress;

while(1){

if((keypress == '\r') || (keypress == '\n'))
{if(isCharavailable(1))receiveChar();break;}

if ((decimal_digit(keypress)) || (keypress == '.')|| (digits[0] == '-')){
I2C_Tx_8_byte_array(digits);
if(digits[0] == '.')break;
if(!(digits[7])) 
{for(int n = 7; n>=1; n--)
digits[n] = digits[n-1];}
else break;}
keypress =  waitforkeypress();
if ((decimal_digit(keypress)) || (keypress == '.'))
digits[0] = keypress; else digits[0] = 'x';}
decimal_part_from_KBD (digits,keypress);}}



/*****************decimal_part_from_KBD*****************************/
void decimal_part_from_KBD (char digits[],char keypress){	//Acquires numerical string
while(1){

if((keypress == '\r') || (keypress == '\n'))
{if(isCharavailable(1))receiveChar();break;}

if ((decimal_digit(keypress)) || (digits[0] == '.')){

I2C_Tx_8_byte_array(digits);
if(digits[7])break;
for(int n = 7; n>=1; n--)
digits[n] = digits[n-1];}

keypress =  waitforkeypress();
if (decimal_digit(keypress))digits[0] = keypress; else digits[0] = 'x';}

if (digits[0] == 'x') {shift_digits_right;}}



/*****************Fixed_pt_number_from_KBD*****************************/
long Fixed_pt_number_from_KBD(char* digits)
{unsigned long num_1_RHS=0,Denominator=1, RHS_of_BP;
long number_1, num_1_LHS=0;
char sign = '+';

Real_num_string_from_KBD(digits); if(!(digits[0])){return 0;}		//Ilegal character?
											//Scan the display from the LHS
{int m=8; while(!(digits[m-1]))m--;		//Stop at the first chatracter
if (digits[m-1] == '-')sign = '-'; 		//If it is '-' a negative number is to be entered
else num_1_LHS = digits[m-1] - '0'; m--;	//Convert the digit to a number
while (m && (digits[m-1] != '.'))			//Continue scaning untill a '.' is detected or the end of the display is reached
{num_1_LHS = 
(10*num_1_LHS) + digits[m-1] - '0'; m--;}	//Continue converting digits aqnd building up the LHS of number
if(m)m--; 
while (m){num_1_RHS = 						//Repeat for the RHS of the number
(10*num_1_RHS) + digits[m-1] - '0'; m--; 
Denominator *=10;}}							//Calculate denominator used to convert RHS to decimal

RHS_of_BP = Fraction_to_Binary_Unsigned
(num_1_RHS, Denominator);
											//Obtain the decimal part of the number			
number_1 = 
(num_1_LHS << 16) + (RHS_of_BP>>16);		//Assemble the LHS & RHS parts to form a real number	
if(sign == '-')	number_1 = ~number_1;		//If the numver is negative invert all the bits
return number_1;}



/*********Floating point number from keyboard*************************/
long fpn_from_KBD(char * digits, signed char *expnt){
long num_1=0,Denominator=1;
long RHS_of_BP;

char sign = '+';
*expnt = 0;
Real_num_string_from_KBD(digits); 
if(!(digits[0])){return 0;}							//Ilegal character?
														//Scan the display from the LHS
{int m=8; while(!(digits[m-1]))m--;					//Stop at the first chatracter
if (digits[m-1] == '-')sign = '-'; 					//If it is '-' a negative number 
else {num_1 = digits[m-1] - '0'; 						//is to be entered
if(num_1){Denominator *=10;(*expnt)++;}}m--;	
while (m && (digits[m-1] != '.'))						//Continue scaning untill a '.' is detected 
{num_1 = 												//or the end of the display is reached
(10*num_1) + digits[m-1] - '0'; 
Denominator *=10;(*expnt)++; m--;}						//Continue converting digits and
														//building up the LHS of number
if(m)m--; 
while (m){num_1 = 										//Repeat for the RHS of the number
(10*num_1) + digits[m-1] - '0'; m--; 
if(num_1)Denominator *=10; if(!(num_1))(*expnt)--;	}	//Calculate denominator used to convert RHS to decimal

while(Denominator/num_1 > 10)
{Denominator /= 10; (*expnt)--;}}						//Ensures -0.05 ends up as -0.5E-1 and not -0.05E0


if(sign == '-')	num_1 = -num_1;							//Std negation: complement and add 1
RHS_of_BP = Fraction_to_Binary_Signed(num_1, Denominator);	//Obtain the decimal part of the number			
return RHS_of_BP;}




/******************sendDecimal***************************/
void Decimal_to_PC(char radix, unsigned long Hex, int No_dps){ char print_out_string[12];
long inc = 1;
char space_counter = 0;

if(Hex == 0) {String_to_PC(".0");return;}
Hex += 1000000000;				//define location decimal point

for(int k = 0; k < (8-No_dps); k++)inc = inc*10;
inc = inc*5;
Hex += inc;

SBtoAL(print_out_string, Hex, radix);
for(int k = 0; k <= (8-No_dps); k++)print_out_string[k] = ' ';
print_out_string[9] = '.'; 

{int m = 0;while((print_out_string[m] == '0')||(print_out_string[m] == ' '))
{print_out_string[m] = ' ';m++; space_counter++;}}
NumericString_to_PC(print_out_string);
for(int m = 0; m<space_counter; m++)Char_to_PC('\b');}



/*****************DetectDecimalOverflow*****************************/
char decimalOverflow(char radix, unsigned long Hex, int No_dps){
long inc=1;

Hex += 1000000000;				//define location decimal point
for(int k = 0; k < (8-No_dps); k++)inc = inc*10;
inc = inc*5;
Hex += inc;
if(Hex >= 2000000000)return 0; else return 1;}



/*****************sendRealNum*****************************/
void RealNum_to_PC(long number_1, int No_dps){
unsigned long RHSDP, RHS_of_BP = 0;
long LHS_of_BP; 
char sign = '+';

if(number_1 < 0){sign = '-'; Char_to_PC('-');
number_1 = ~number_1;}else sign = '+';
LHS_of_BP = number_1 >> 16;
RHS_of_BP = (number_1)<<16;
RHSDP = Binary_points_to_Decimal_Unsigned(RHS_of_BP);

if (!(decimalOverflow(10, RHSDP, No_dps))){RHSDP = 0; LHS_of_BP++; }
Num_to_PC(10, LHS_of_BP); Decimal_to_PC(10, RHSDP, No_dps);}



/******************Send floating point number********************/
void fpn_to_PC(long number, signed char expnt){			//Receives fpn as +/-0._ _ _ _ .... 
long  RHSDP;
char No_dps = 7;											//with a decimal exponent
														//Convert the RHS of the binary point
RHSDP = Binary_points_to_Decimal_Signed(number);			//to a decimal number
if(number < 0){String_to_PC("-0"); RHSDP = ~RHSDP;}
else Char_to_PC('0');	
if (!(decimalOverflow(10, RHSDP, No_dps))){RHSDP = 0;
Char_to_PC('\b');Char_to_PC('1');}
Decimal_to_PC(10, RHSDP, No_dps);Exp_to_PC(expnt);}





