
void Fraction_to_Binary_D(unsigned long,unsigned long, unsigned int*,unsigned int*);
unsigned long Binary_points_to_Decimal_L (unsigned int, unsigned int);
unsigned long divide_scientific(signed char *, unsigned int  *,unsigned int *);
unsigned long divide_real(unsigned long*, signed char*);
unsigned long multiply_real(unsigned long *, signed char *);
unsigned long product_loop(unsigned long *, signed char *, char );
unsigned long any_root (unsigned long *, signed char *);
unsigned long any_power (unsigned long *, signed char *);
long Binary_points_to_Decimal_Signed (long);



/**************************************************************************************************/
void Fraction_to_Binary_D(unsigned long rem,unsigned long Den, unsigned int* Res_LB,unsigned int*  Res_HB){		//Positive fraction: Askii to Binary
(*Res_LB) = 0; (*Res_HB) = 0;
for(int n = 0; n <= 15; n++){
if ((2*(rem))/Den) {(*Res_HB) |= (1 << (15-n));}	
(rem) = (2*(rem))%Den;}
for(int n = 0; n <= 15; n++){
if ((2*(rem))/Den) {(*Res_LB) |= (1 << (15-n));} 	
(rem) = (2*(rem))%Den;}}



/**************************************************************************************************/
unsigned long Binary_points_to_Decimal_L (unsigned int RHSofBP_LB, unsigned int RHSofBP_HB)
{unsigned long  RHSofDP;
RHSofDP = 0;
for(int n = 0; n <= 15; n++){switch(n){
case 0: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 500000000; break;
case 1: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 250000000; break;
case 2: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 125000000; break;
case 3: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 62500000; break;
case 4: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 31250000; break;
case 5: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 15625000; break;
case 6: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 7812500; break;
case 7: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 3906250; break;
case 8: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 1953125; break;
case 9: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 976563; break;
case 10: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 488281; break;
case 11: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 244141; break;
case 12: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 122070; break;
case 13: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 61035; break;
case 14: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 30518; break;
case 15: if (RHSofBP_HB & (1 << (15-n)))RHSofDP += 15259; break;}}
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
RHSofDP += 0x00000008; 	RHSofDP = RHSofDP & 0xFFFFFFF0;  											//Round to 8 decimal places	
return RHSofDP;}



/**************************************************************************************************/
unsigned long divide_scientific(signed char *exponent, unsigned int  *RHSofBP_LB,unsigned int *RHSofBP_HB){
unsigned long  Denominator, Numerator;
unsigned int res_LHS_BP;
unsigned long  RHSofDP;

Numerator = 	RHSofBP_HB[0]; Numerator = 	 (Numerator << 16) + RHSofBP_LB[0];
Denominator = 	RHSofBP_HB[1]; Denominator  = (Denominator << 16) + RHSofBP_LB[1];
if(RHSofBP_HB[1] & (1 << 15)) {Numerator = Numerator >> 1;Denominator = Denominator >> 1;}		// detect overflow of long

res_LHS_BP = Numerator / Denominator;		//Obtain LHS of product
Fraction_to_Binary_D(Numerator % Denominator, Denominator, &RHSofBP_LB[2], &RHSofBP_HB[2] );		//Obtain RHS of product
RHSofDP = Binary_points_to_Decimal_L ( RHSofBP_LB[2], RHSofBP_HB[2]);								//Convert RHS to decimal notation


if(res_LHS_BP){RHSofDP = RHSofDP/10;
switch (res_LHS_BP){
case 1:  RHSofDP = RHSofDP + 100000000;break;
case 2:  RHSofDP = RHSofDP + 200000000;break;
case 3:  RHSofDP = RHSofDP + 300000000;break;
case 4:  RHSofDP = RHSofDP + 400000000;break;
case 5:  RHSofDP = RHSofDP + 500000000;break;
case 6:  RHSofDP = RHSofDP + 600000000;break;
case 7:  RHSofDP = RHSofDP + 700000000;break;
case 8:  RHSofDP = RHSofDP + 800000000;break;
case 9:  RHSofDP = RHSofDP + 900000000;break;}
exponent[2]++; res_LHS_BP = 0;}
return RHSofDP;}



/**************************************************************************************************/
unsigned long divide_real(unsigned long *data, signed char *exponent ){		
unsigned int  RHSofBP_LB[3], RHSofBP_HB[3];
unsigned long  RHSofDP;

data[2] = 1; while (data[0] >= data[2]) {data[2] = data[2] * 10; exponent[0]++;}
Fraction_to_Binary_D(data[0], data[2],  &RHSofBP_LB[0], &RHSofBP_HB[0]);

data[3] = 1; while (data[1] >= data[3]) {data[3] = data[3] * 10; exponent[1]++;}
Fraction_to_Binary_D(data[1], data[3],  &RHSofBP_LB[1], &RHSofBP_HB[1]);

exponent[2] = exponent[0] - exponent[1];

RHSofDP = divide_scientific(exponent,  RHSofBP_LB, RHSofBP_HB);
return RHSofDP;}



/**************************************************************************************************/
unsigned long multiply_real(unsigned long *data, signed char *exponent ){		
unsigned int  RHSofBP_LB[3], RHSofBP_HB[3];
unsigned long  RHSofDP;

//**********Convert data[1] to scientific notation**********

data[2] = 1; while (data[1] >= data[2]) {data[2] = data[2] * 10; exponent[1]++;}
Fraction_to_Binary_D(data[1], data[2],  &RHSofBP_LB[0], &RHSofBP_HB[0]);

//**********Convert reciprocal of data[0] to scientific notation**********

data[3] = 1; while (data[0] >= data[3]) {data[3] = data[3] * 10; exponent[0]++;} data[3] = data[3]/10; exponent[0]--;	

if(data[0] == data[3]){data[0] = data[0] * 10; exponent[0]--; }						//detect values of 1,10,100,1000 etc for Num_1

Fraction_to_Binary_D(data[3], data[0], &RHSofBP_LB[1],  &RHSofBP_HB[1]);

exponent[2] = exponent[1] + exponent[0];
RHSofDP = divide_scientific(exponent,  RHSofBP_LB, RHSofBP_HB);

return RHSofDP;}



/**************************************************************************************************/
unsigned long product_loop(unsigned long *data, signed char *exponent, char power ){
unsigned int  RHSofBP_LB[3], RHSofBP_HB[3], res_LHS_BP;
unsigned long  Denominator, Numerator, numerator_update, scale_factor,data_store;
unsigned long  RHSofDP;
char Overflow_1;
Overflow_1 =0;
RHSofDP = 0;

/**********Convert data[1] to scientific notation**********/

data[2] = 1; while (data[1] >= data[2]) {data[2] = data[2] * 10; exponent[1]++;}
Fraction_to_Binary_D(data[1], data[2],  &RHSofBP_LB[1], &RHSofBP_HB[1]);

/**********Convert reciprocal of data[0] to scientific notation**********/
data_store = data[0];		//subroutine must not be allowed to change data[0];

data[3] = 1; while (data[0] >= data[3]) {data[3] = data[3] * 10; exponent[0]++;} data[3] = data[3]/10; exponent[0]--;	
if(data[0] == data[3]){data[0] = data[0] * 10; exponent[0]--; }					//detect values of 1,10,100,1000 etc for Num_1
Fraction_to_Binary_D(data[3], data[0], &RHSofBP_LB[0],  &RHSofBP_HB[0]);

data[0] = data_store;																//restore value of data[0] which may have been changed

Denominator = 	RHSofBP_HB[0]; Denominator  = (Denominator << 16) + RHSofBP_LB[0];
if(RHSofBP_HB[0] & (1 << 15)) {Denominator = Denominator >> 1; Overflow_1 =1;}

for(int p = 0; p <= (power-2); p++){ //setup repeated multiplication

/****************Divide Num_2 by the reciprocal of Num_1 to get power****************/

Numerator = 	RHSofBP_HB[1]; Numerator = 	 (Numerator << 16) + RHSofBP_LB[1];
if ((Overflow_1 ==1) )Numerator = Numerator >> 1;		//&& (p == 0)
exponent[2] = exponent[1] + exponent[0];

res_LHS_BP = Numerator / Denominator;																//Obtain LHS of product
Fraction_to_Binary_D(Numerator % Denominator, Denominator, &RHSofBP_LB[2], &RHSofBP_HB[2] );		//Obtain RHS of product
RHSofDP = Binary_points_to_Decimal_L ( RHSofBP_LB[2], RHSofBP_HB[2]);				//Convert RHS to decimal notation
if(res_LHS_BP){RHSofDP = RHSofDP/10;												//shift LHS of product to RHS to obtain scientific notation
switch (res_LHS_BP){
case 1:  RHSofDP = RHSofDP + 100000000;break;
case 2:  RHSofDP = RHSofDP + 200000000;break;
case 3:  RHSofDP = RHSofDP + 300000000;break;
case 4:  RHSofDP = RHSofDP + 400000000;break;
case 5:  RHSofDP = RHSofDP + 500000000;break;
case 6:  RHSofDP = RHSofDP + 600000000;break;
case 7:  RHSofDP = RHSofDP + 700000000;break;
case 8:  RHSofDP = RHSofDP + 800000000;break;
case 9:  RHSofDP = RHSofDP + 900000000;break;}
exponent[2]++; res_LHS_BP = 0;}
numerator_update = RHSofDP; 
scale_factor = 1; while (numerator_update >= scale_factor) {scale_factor = scale_factor * 10; } 
Fraction_to_Binary_D(numerator_update, scale_factor,  &RHSofBP_LB[1], &RHSofBP_HB[1]);
exponent[1] = exponent[2];}															//Update numerator
return RHSofDP;}



/**************************************************************************************************/
unsigned long any_root (unsigned long * number, signed char * exponent){

unsigned int  RHSofBP_LB,  RHSofBP_HB, expnt_calculator;
unsigned long  RHSofDP_input, RHSofDP;
signed char expnt_input,expnt_result,temp_0,temp_1;	
signed char root;	

temp_0 = exponent[0];
temp_1 = exponent[1];

RHSofDP=0;
expnt_input = exponent[0];
root = exponent[1];

number[3] = 1; while (number[0] >= number[3]) {number[3] = number[3] * 10; expnt_input++;}

Fraction_to_Binary_D(number[0], number[3],  &RHSofBP_LB, &RHSofBP_HB);
RHSofDP_input = Binary_points_to_Decimal_L (RHSofBP_LB,RHSofBP_HB); 

number[0] = 1; number[1] = 1; exponent[2] = 0;

/******Hole number iteration************/
while(1){
(exponent[0]) = 0; (exponent[1]) = 0;
RHSofDP = product_loop(number,exponent, root);
if((RHSofDP > RHSofDP_input) && (exponent[2] == expnt_input)) break;
if(exponent[2] > expnt_input) break;
number[0]++;number[1]++;}number[0]--; number[1]--;

(exponent[0]) = 0; (exponent[1]) = 0;
RHSofDP = product_loop(number,exponent, root);

(exponent[0]) = 0; (exponent[1]) = 0; 

expnt_result = 0; expnt_calculator=1; while (number[0] >= expnt_calculator) {expnt_calculator *= 10; expnt_result++;}

/******Real number iteration************/

for(int m = 1; m <=6; m++){
number[0] *=10; number[1] *=10;
for(int n = 0; n<10; n++)
{(exponent[0]) = -m; (exponent[1]) = -m; RHSofDP = product_loop(number,exponent, root);


if((RHSofDP > RHSofDP_input) && (exponent[2] == expnt_input)) break;
if(exponent[2] > expnt_input) break;
number[0]++;number[1]++;}
number[0]--; number[1]--;
(exponent[0]) = -m; (exponent[1]) = -m; }

exponent[2] = expnt_result;	
exponent[0] = temp_0;
exponent[1] = temp_1;
return number[0];}



/**************************************************************************************************/
unsigned long any_power (unsigned long * number, signed char * exponent){

unsigned long temp_1, RHSofDP;
signed char temp_0;
char power;

temp_0 = exponent[1];
temp_1 = number[1];

power = exponent[1];
number[1] = number[0];
exponent[1] = exponent[0];

RHSofDP = product_loop(number,exponent, power);

exponent[1] = temp_0;
number[1] = temp_1;

return RHSofDP; }



/**************************************************************************************************/
long Binary_points_to_Decimal_Signed (long RHSofBP)
{long  RHSofDP=0;
unsigned int RHSofBP_LB;
RHSofBP_LB = RHSofBP;

for(int n = 0; n <= 15; n++){switch(n){											//bit zero is sign bit
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


