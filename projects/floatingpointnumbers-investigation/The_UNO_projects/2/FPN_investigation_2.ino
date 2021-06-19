
/*
FPN_investigation_1 converted numbers of the type 1234.567 to floating point format
This investigation puts matarial from the previous investigation into a subroutine "Real_Num_to_FPN()"
It takes numbers of the type 1234.567e-15.
Real_Num_to_FPN() is used to convert 1234.567 to floating point format.
Repeated multiplication or division by 10 is then used to amalgamate the exponent.
*/



#include "Project_header.h"

int main (void){ 
  
char data_buff[15];                                                 //Numerical string entered at keyboard
char print_buff[15];                                                //Desination for the float to askii characters
float FPNum;                                                        //Binary equivalent of the characters held in data_buff
int expt_10;
char expt_10_string[6];                                             //Used to echo exponent to the screen

char E_ptr;                                                         //Points to the exponent in data_buff
char sign; 
char num_decimal_places, num_range;                                 //Used to format contents of print_buff                                          
unsigned long *long_ptr;                                            //Used to read FPN in unsigned long format
long_ptr = (unsigned long *) &FPNum;


setup_328_HW;                                                       //see header file"

sei();

while(1){
do{Serial.write("F/f    ");}                                        //User prompt; press F or f to escape
while((isCharavailable(127) == 0));                                 //Waits 1 sec for user response; then resends prompt    
keypress = Serial.read();
if ((keypress  == 'F')||
(keypress  == 'f'))break;}


while(1){

Serial.write("\r\n\r\nFPN conversion to binary\r\n\
\r\nEcho keyboard input\t\t");                                        //User enters floating point number (FPN)

sign = positive;                                                      //Default setting
E_ptr = Scientific_num_from_KBD(data_buff, &sign);                    //Subroutine echoes keypresses to screen

if(E_ptr){expt_10 = atoi(data_buff + E_ptr + 1);                      //Read the exponent if there is one 
data_buff[E_ptr] = 0;}                                                //Ensure the number string is terminated in a zero
else expt_10 = 0;                                                     //Set to zero if no exponent was entered.

expt_10 += Justify_Num_string(data_buff);                             //Shift the dp so that there is only one digit on its LHS
Serial.write("\r\n\r\nPost string justification\t");

itoa(expt_10, expt_10_string, 10);                                    //Echo the string prior to its conversion to a FPN
if (sign == negative)Serial.write ('-');
Serial.write(data_buff); Serial.write ('E');
Serial.write(expt_10_string);



/*********Almalmagate the exponent and real number********************************************************************/

Serial.write("\r\n");
FPNum = Real_Num_to_FPN(data_buff, sign);                           //Reads data_buff and converts the string to a floating point number

if (expt_10 > 0){                                                   //Repeatedly multiply FPNum untill its exponent is zero
while (expt_10){FPNum *= 10; expt_10 -= 1;}}

if (expt_10 < 0){                                                    //Repeatedly divide FPNum untill its exponent is zero
while (expt_10){FPNum /= 10; expt_10 += 1;}}

Serial.write("\r\nFPN equivalent\t\t");
FNP_in_binary_to_PC(FPNum);

Serial.write("\r\n\r\nDoing some arithmetic\t");
FPNum = pow(FPNum, 3);

FNP_in_binary_to_PC(FPNum);                                         //Print out FPN in binary

if  ((*long_ptr == 0x7F800000) ||                                   //Check for overflow in arithmetic
    (*long_ptr == 0xFF800000) | 
    (*long_ptr == 0X0) ||
    (*long_ptr == 0X80000000))

{Serial.write("\r\n\r\nOverflow:Reset UNO to continue\r\n");while(1);}




/********Reverse the proces: Step 1 Set the print format****************************************************************/

Serial.write ("\r\n\r\nFloating point to string\t");
sign = positive;                                              //Default value
if (FPNum < 0){FPNum *= -1.0; sign = negative;}               //Determine sign and clear the sign bit

if (FPNum >= 10000.0)  num_range = 1;                         //Specify the numerical ranges used to define the output format
if ((FPNum < 10000.0) && (FPNum >= 10.0)) num_range = 2;
if ((FPNum < 10.0) && (FPNum >= 0.01)) num_range = 3;
if (FPNum < 0.01)num_range = 4;

expt_10 = 0;                                                  //Calculate the exponents and define the number of decimal places                                                
switch (num_range){                                           
case 1: while(FPNum > 10)
{FPNum /= 10.0; expt_10 += 1;}num_decimal_places = 5; break;
case 2: num_decimal_places = 3; break;
case 3: num_decimal_places = 4; break;
case 4: while(FPNum < 1.0)
{FPNum *= 10.0; expt_10 -= 1;}num_decimal_places = 5; break;}



/********Reverse the proces: Step 2 Call the FPN to askii and roundig subroutines****************************************************/

FPN_to_askii(FPNum, print_buff, sign, num_decimal_places, expt_10 );
Serial.write(print_buff);
Serial.write("\r\n");

Serial.write("\r\nResult after rounding\t\t");

Round_print_buff(print_buff);
Serial.write(print_buff);

Serial.write("\r\n\r\n*****************************************************************************************\r\n\r\n"); }}





/***************************************************************************************************************************/
int Justify_Num_string(char * data_buff ){
char dp_ptr;
char expt_10;
char dp_location;                                                   //Location of decimal point in real number string

dp_ptr = 0;
while (data_buff[dp_ptr] != '.')                                    //Scann the numerical string looking for the dp.
{if(data_buff[dp_ptr++] > '0')                                      //Is a non zero digit encountered first

{dp_ptr = 0;                                                        //If yes 
while (data_buff[dp_ptr++] != '.');                                 //continue untill the dp has been located
for (int m = dp_ptr-1; m >= 2; m--)                                 //Shift all digits bar the first one space right 
data_buff[m] = data_buff[m - 1];                                    //to occuppy the space originally occupied by the dp
data_buff [1] = '.';                                                //Place the dp in the vacated space
expt_10 = dp_ptr - 2;                                               //Shifing the dp requires an adjustment to the tens exponent
break;}}

if (data_buff[dp_ptr] == '.')                                       //There are no non zero digits to the left of the dp  
{expt_10 = 0;
while ((data_buff[dp_ptr + 1])  == '0'){                            //Check for the presence of a non zero digit immediately to the left of the dp                           
{for(int m = dp_ptr + 1; m < 13; m++)                               //Shift all digits comming after the dp one place to the left
data_buff[m] = data_buff[m + 1];}
data_buff[14] = 0;                                                  //Clear the least significant digit
expt_10 -= 1;}                                                       //Adjust the tens exponent

data_buff[dp_ptr] = data_buff[dp_ptr + 1];                          //Move the dp one place to the right allong the string                           
data_buff[dp_ptr + 1] = '.';
expt_10 -= 1;

dp_ptr = 0;
while ((data_buff[dp_ptr++])  == '0'){                              //Check for unwanted zeros at the start of the numeric string                            
{for(int m = 0; m < 13; m++)                                        //Shift all digits left to remove them
data_buff[m] = data_buff[m + 1];}
data_buff[14] = 0;                                                  //Clear the vacated digit
dp_ptr = 0;}}

return expt_10;}
