

//#include "../../Resources/Header_files/I_O_from switches.h"

void fraction_to_decimal_string(long, long, char*);
unsigned long Fraction_to_Binary_Unsigned(unsigned long,unsigned long);
unsigned long Binary_points_to_Decimal_Unsigned (unsigned long);
long Fraction_to_Binary_Signed( long, long);
long Binary_points_to_Decimal_Signed (long);

void Timer_T0_10mS_delay_x_m(int);
void Char_to_PC(char);


/******************************************************************************/
void fraction_to_decimal_string(long remainder, long denominator, char digits[]){
long digit;
int m=6;

clear_digits;

digits[7]= '0';
digits[6] = '_';
String_to_PC("0.");

while(switch_2_up){
digit = (remainder *10/denominator);
remainder = ((remainder *10)%denominator);

Char_to_PC(digit+'0');
if(m){digits[m-1] = (digit+'0');m--;I2C_Tx_8_byte_array(digits);Timer_T0_10mS_delay_x_m(12);}
Timer_T0_10mS_delay_x_m(6);}}



/******************************************************************************/
unsigned long Fraction_to_Binary_Unsigned(unsigned long rem,unsigned long Den){	
unsigned long Result = 0; 
unsigned int res_LB=0; 
char mode = 1;

if((rem >= 0x80000000) || (Den >= 0x80000000))
{rem /= 2; Den /= 2;}

if(mode == 1){
for(int n = 0; n <= 15; n++){					//bit 0 is reserved for sign bit
if ((2*(rem))/Den)(Result) |= (1 << (15-n));
rem = (2*(rem))%Den; }
Result = Result << 16;
for(int n = 0; n <= 15; n++){
if ((2*(rem))/Den)(res_LB) |= (1 << (15-n));
rem = (2*(rem))%Den;}}

if(mode == 0){
for(int n = 0; n <= 15; n++){					//bit 0 is reserved for sign bit
if ((2*(rem))/Den){(Result) |= (1 << (15-n));Char_to_PC('1');} else Char_to_PC('0');
rem = (2*(rem))%Den; }
Result = Result << 16;
for(int n = 0; n <= 15; n++){
if ((2*(rem))/Den){(res_LB) |= (1 << (15-n));Char_to_PC('1');} else Char_to_PC('0');
rem = (2*(rem))%Den;}Char_to_PC('\t');}

Result += res_LB;								//Result is not affected by the signs of rem and Den
return Result;}


/******************************************************************************/
unsigned long Binary_points_to_Decimal_Unsigned (unsigned long RHSofBP)
{unsigned long  RHSofDP=0;
unsigned int RHSofBP_LB;
RHSofBP_LB = RHSofBP;

for(int n = 0; n <= 15; n++){switch(n){
case 0: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 500000000; break;
case 1: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 250000000; break;
case 2: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 125000000; break;
case 3: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 62500000; break;
case 4: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 31250000; break;
case 5: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 15625000; break;
case 6: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 7812500; break;
case 7: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 3906250; break;
case 8: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 1953125; break;
case 9: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 976563; break;
case 10: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 488281; break;
case 11: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 244141; break;
case 12: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 122070; break;
case 13: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 61035; break;
case 14: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 30518; break;
case 15: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 15259; break;}}
for(int n = 0; n <= 15; n++){switch (n){
case 0: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 7629; break;
case 1: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 3815; break;
case 2: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 1907; break;
case 3: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 954; break;
case 4: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 477; break;
case 5: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 238; break;
case 6: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 119; break;
case 7: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 60; break;
case 8: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 30; break;
case 9: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 15; break;
case 10: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 7; break;
case 11: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 4; break;
case 12: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 2; break;
case 13: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 1; break;
case 14: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 0; break;
case 15: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 0; break;}}
return RHSofDP;}


/******************************************************************************/
long Fraction_to_Binary_Signed(long rem, long Den){
long Result = 0; 						
unsigned int res_LB=0; 
char sign = '+';

if((rem >= -0x40000000) && (rem < 0x40000000) &&
(Den >= -0x40000000) && (Den < 0x40000000));
else{rem /= 2; Den /= 2;}

if ((rem < 0) || (Den < 0)) sign = '-';
if ((rem < 0) && (Den < 0)) sign = '+';

for(int n = 1; n <= 15; n++){					//bit 0 is reserved for sign bit
if ((2*(rem))/Den)(Result) |= (1 << (15-n));
rem = (2*(rem))%Den; }
Result = Result << 16;
for(int n = 0; n <= 15; n++){
if ((2*(rem))/Den)(res_LB) |= (1 << (15-n));
rem = (2*(rem))%Den;}
Result += res_LB;								//Result is not affected by the signs of rem and Den
if(sign == '-')Result = ~Result;				//Inverting the sign bit is equivalent to adding -1
return Result;}


/******************************************************************************/
long Binary_points_to_Decimal_Signed (long RHSofBP)
{long  RHSofDP=0;
unsigned int RHSofBP_LB;
RHSofBP_LB = RHSofBP;

for(int n = 0; n <= 15; n++){switch(n){						//bit zero is sign bit
case 0: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP -= 1000000000; break;
case 1: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 500000000; break;
case 2: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 250000000; break;
case 3: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 125000000; break;
case 4: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 62500000; break;
case 5: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 31250000; break;
case 6: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 15625000; break;
case 7: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 7812500; break;
case 8: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 3906250; break;
case 9: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 1953125; break;
case 10: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 976563; break;
case 11: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 488281; break;
case 12: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 244141; break;
case 13: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 122070; break;
case 14: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 61035; break;
case 15: if (RHSofBP>>16 & (1 << (15-n)))RHSofDP += 30518; break;}}
for(int n = 0; n <= 15; n++){switch (n){
case 0: if (RHSofBP_LB & (1 << (15-n)))RHSofDP +=  15259; break;
case 1: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 7629; break;
case 2: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 3815; break;
case 3: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 1907; break;
case 4: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 954; break;
case 5: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 477; break;
case 6: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 238; break;
case 7: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 119; break;
case 8: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 60; break;
case 9: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 30; break;
case 10: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 15; break;
case 11: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 7; break;
case 12: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 4; break;
case 13: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 2; break;
case 14: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 1; break;
case 15: if (RHSofBP_LB & (1 << (15-n)))RHSofDP += 0; break;}}	
return RHSofDP;}




