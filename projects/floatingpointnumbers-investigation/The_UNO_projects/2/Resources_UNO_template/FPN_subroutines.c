char int_length(long );
void Binary_to_PC(long );
void FNP_in_binary_to_PC(float );
long decimal_to_binary(long,char,char);
void binary_places_to_decimal(long, int, int, char*, char*);





/*********************************************************************************************************************************/
float Real_Num_to_FPN(char * array, char sign){

float FPN;																	//Floating point number
unsigned long FPN_LF;														//Floating point number read in long number format
char LHS_length;                                                    		//Number of binary characters on LHS of binary point
char num_leading_zeros;                                             		//The number of zeros immediately following the dp.

char next_decimal_place;                                            		//Location of decimal places comming after the decimal point
long binary_places;                                                 		//Binary equivalent of decimal places

int expt_2;                                                         		//Exponent to which binary number is raised.
long LHS_of_dp, RHS_of_dp;
char dp_location = 1;														//This is no longer a variable


/********Process the integer part of the non-integer number**************************/
LHS_of_dp = atol(array);                                          			//Convert integer part to binary
LHS_length = int_length(LHS_of_dp);                                   		//See below for this subroutine
expt_2 = 127 + (LHS_length - 1);                                      		//Floating point format (exponent = length - 1)


/********Procces the fractional part of the number**********************************/
RHS_of_dp = atol(array + dp_location + 1);                       			//Convert the decimal places to an integer number
num_leading_zeros = 0;                                                		//Check for the presence of leading zeros (i.e. 7.0005)
next_decimal_place = dp_location + 1;                                 		//Location of first decimal place
while ((array[next_decimal_place++]) == '0')
num_leading_zeros += 1;                                               		//Count the number of leading zeros   
binary_places = 
decimal_to_binary(RHS_of_dp,LHS_length,num_leading_zeros);          		//Convert the integer to the max allowed num of binary places 


/********Start assembly of the FPN_LF***********************************************/
if (LHS_length){FPN_LF = LHS_of_dp << (25 - LHS_length);                 	//Shift MSB to bit 24
FPN_LF = FPN_LF | binary_places;}                                        	//Start building the FPN_LF

else {FPN_LF = binary_places;                                            	//Special case: There are no bits on the LHS of the binary point
while (!(FPN_LF & (( 0x80000000 >> 7))))                          			//LHS_of_dp is zero                 
{FPN_LF = (FPN_LF << 1);expt_2 -= 1;}}                                   	//Shift MS '1' of binary places to bit 24


/********Round up: Add one to LSB & vacate bit 23************************************/
FPN_LF += 1;     
FPN_LF = FPN_LF >> 1;                                                    	//Vacate bit 24
if (FPN_LF & (0x80000000 >> 7))                                          	//Special case: round up caused bit 25 to be occupied 
{FPN_LF = FPN_LF >> 1; expt_2 += 1;LHS_length += 1;}

FPN_LF = FPN_LF & (~(0x800000));                                        	//Clear bit 23: The MSB of the integer number
FPN_LF = FPN_LF | ((unsigned long)expt_2 << 23);                        	//Copy in the exponent

if (sign == negative)FPN_LF |= 0x80000000;                             		//Add in the sign bit

FPN = *(float*)&FPN_LF;														//Read the unsigned long number as a floating point one
return FPN;}





/*********************************************************************************************************************************/
void FPN_to_askii(float FPN, char * print_buff, char sign, char num_dec_places, int expt_10){

long binary_places;                                             			//Binary equivalent of decimal places
long binary_digits;                                             			//Includes binary digits on both sides of the binary point
int expt_2;                                                     			//Exponent to which binary number is raised.
long LHS_of_dp;
unsigned long FPN_LF;														//FPN in unsigned long format
char print_buff_ptr;



/*********Obtain the twos exponent and use it to determine LHS of the binary point******************/
FPN_LF = *(unsigned long*)&FPN;												//The FPN_to_askii routine treats the FPN as an unsigned long integer
expt_2 = ((FPN_LF >> 23) & 0x1FF) - 127;                        			//Recover the exponent
binary_digits = FPN_LF & (~(0xFF800000));                       			//Mask of exponent and sign, leaving bits on both the LHS & RHS of the point.
binary_digits |= 0x800000;                                      			//Reinstate the missing 1 (the most significant bit)
LHS_of_dp = binary_digits >> (23 - expt_2);                     			//Remove binary places leaving just the LHS of the binary point




/*********Convert the LHS of the binary point to decimal string and save it in print_buff***********/
for(int m = 0; m<=14; m++)print_buff[m] =0;									//Clear the buffer
ltoa(LHS_of_dp, print_buff, 10);											//Convert LHS_of_dp to askii and save the string to print_buff
if(sign == negative)
{for(int m = 14; m; m--)print_buff[m] = print_buff[m-1]; 					//Add negatve sign if necessary
print_buff[0] = '-';}
print_buff_ptr = 0;
while (print_buff[print_buff_ptr++]);										//Find the end of the string
print_buff[print_buff_ptr-1] = '.';											//and add the decimal point



/********Isolate the binary digits on the RHS of the binary point******************************************************************/
binary_places = 
binary_digits  & (~(LHS_of_dp << (23 - expt_2)));               			//Remove bits on the LHS of the binary point 

binary_places_to_decimal
(binary_places, expt_2, num_dec_places, print_buff, &print_buff_ptr );		//Adds decimal places to print_buff

if (expt_10) {print_buff[print_buff_ptr++] = 'E'; 							//Adds the exponent if required
itoa(expt_10, (print_buff+print_buff_ptr), 10);}} 




/*********************************************************************************************************************************/
void binary_places_to_decimal(long binary_points, int expt, int Num_dps, char* array, char * array_ptr){
long denominator = 1;                                                       
char result;                                                                //Takes the binary places as an integer number
long remainder;                                                             //converts it to a fraction and
                                                                            //converts this to decimal places
if (expt >= -4)                                                           
{denominator = (unsigned long)0x80000000 >> (8 + expt);                     //Fails for shifts of less than 4
remainder = binary_points;}
else
{denominator = (unsigned long)0x80000000 >> (4);                            //If the exponent is less than -4
remainder = binary_points >> -expt - 4;}                                    //we must also shift the binary points

for (int m = 0; m < Num_dps; m++){
result = ((remainder * 10)/ denominator);
remainder = (remainder * 10) % denominator;
array[(*array_ptr)++] = result + '0';}}




/*************************************************************************************************************************************/
char int_length(long num){
char len = 1;

if (!(num))return 0;
while (num = (num >> 1)) len += 1;
return len;}



 

/*************************************************************************************************************************************/
void Binary_to_PC(long num){                                              //Sends a binary number to the screen as ones and zeros
  int HB, LB;                                                             //High byte and low byte
  
  HB = num >> 16;
  LB = num;
for (int m = 0; m <= 15; m++){if ((m) && !(m%4))Serial.write(' ');
  if (HB & (1 << (15 - m)))Serial.write('1'); else Serial.write('0');}
  Serial.write(' ');
  for (int m = 0; m <= 15; m++){if ((m) && !(m%4))Serial.write(' ');
  if (LB & (1 << (15 - m)))Serial.write('1'); else Serial.write('0');}}



/***************************************************************************************************************************************/
void FNP_in_binary_to_PC(float num){                                  
unsigned long num_LF;
char counter = 0;

num_LF = * (unsigned long * )&num;											//Read num in unsigned long format.


for (unsigned long m = 0x80000000; m >= 1; m = m >> 1){
  if (num_LF & m)Serial.write('1'); else Serial.write('0');counter += 1;
  if ((counter == 1) || (counter == 9) || (counter == 12)
  || (counter == 16) || (counter == 20) || (counter == 24)
  || (counter == 28))  Serial.write(' ');}}




/**************************************************************************************************************************************/
long decimal_to_binary(long num,char LHS_length,char num_leading_zeros){       //Takes decimal points as integer number
long denominator = 1;                                                          //converts it to a fraction
long result = 0;                                                               //and converts the fraction to binary places
long remainder;

if (!(num))return 0;
while(denominator <= num) denominator *= 10;
while (num_leading_zeros--)denominator *= 10;

remainder = num;
for (int m = 0; m <= (24 - LHS_length); m++){                                 //Maximum num binary places + 1 for rounding
result = (result << 1) | ((remainder * 2)/ denominator);
remainder = (remainder * 2) % denominator;}

return result;}                                                               //Returns one additional binary place




/*****************************************************************************************************************************************/
char data_from_KBD(void){
while (!(Serial.available()));
return Serial.read();}





