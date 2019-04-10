

/*
Segments of a client program required to print out strings from the EEPROM and acquire 
the long, integer and character numbers that have been stored in it.

Subroutines required to read text and data from the EEPROM are also included.

Note: The header file contains the addresses in EEPROM at which the strings and data are stored.
This information was supplied by the programmer when the text file containing the strings and numbers was
saved to the EEPROM.
See "Sample EEPROM file.txt" stored in the "Full_Atmega_Programmer" directory for an example of a file.


Note: LEDS were connected between PB0 and PB1 and 0V on the development pcb for this program. 
These LEDS are not used here but for stable operation it was found necessary to set their ports to
output low.  
Watch crystals were also connnected between ports PB6 & 7 which were configured appropriately.
All other unused ports were configured for weak pull up. 
*/


#include "EEPROM_Client_program_for_328.h"

long print_array (char, int, char);



int main (void){ 

char cal_factor=0;												//Either default or user supplied	
int I_num[4];
long L_num[4], sum;
char Ch_num[4];


setup_328_HW;													//see "Resources\ATMEGA_Programmer.h"


while(1){do{sendString("Y  ");
timer_T1_sub(T1_delay_1sec);}									//User prompt
while((isCharavailable() == 0));								//Sends YYY to indicte activity
if ((receiveChar() == 'Y'))break;}								//Waits for user respose: Press -Y-


/*while(1){do{sendString("Z  ");
timer_T1_sub(T1_delay_1sec);}									//Alternative user prompt required for						
while((isCharavailable() == 0));								//programer development to prevent
if ((receiveChar() == 'Z'))break;}								//UART contention issues
*/

newline();newline();newline();
Text_from_EEPROM(message_1); waitforkeypress();				//Print out the strings
Text_from_EEPROM(message_2); waitforkeypress();
Text_from_EEPROM(message_3); waitforkeypress();
Text_from_EEPROM(message_4); waitforkeypress();
Text_from_EEPROM(message_5); waitforkeypress();
Text_from_EEPROM(message_6); waitforkeypress();
Text_from_EEPROM(message_7); waitforkeypress();

for(int m = 0; m < 4; m++){									//Read data from the EEPROM
I_num[m] = I_num_from_EEPROM(I_array + (2*m));
L_num[m] = L_num_from_EEPROM(L_array + (4*m));
Ch_num[m] = Char_from_EEPROM(Ch_array + m);}




/*******************The remainder of the main program is included for test purposes only**********************************/
sum = 0;
for(int m = 0; m < 4; m++)										//Print out the integer numbers and their sum
{newline(); Hex_to_PC(I_num[m]);sendString("\t\t");
Num_to_PC(10, I_num[m]); sum += I_num[m];
if(!m)sendString("\t\tHex and Decimal numbers"); 				//Note: sum = 0x96 + 0xFA + 0xFFFF D9AC + 0xFFFF FCD8
if(m == 3){sendString("\t\tSum to\t");Num_to_PC(16,sum);		//			= 0xFFFF D814 = -10,220 in decimal or -0x27EC
sendString("\t"); Num_to_PC(10, sum);}}
newline();

sum = 0;
for(int m = 0; m < 4; m++)										//Print out the long numbers and their sum
{newline();Num_to_PC_U(16, L_num[m]); sendChar('\t');
Num_to_PC(10, L_num[m]); sum += L_num[m];
if(!m)sendString("\tHex and Decimal numbers"); 
if(m == 3){sendString("\tSum to\t");Num_to_PC(16,sum);
sendString("\t"); Num_to_PC(10, sum);}}
newline();

sum = 0;
for(int m = 0; m < 4; m++)
{newline();Hex_to_PC(Ch_num[m]);sendString("\t\t");			//Print out the character numbers and their sum
Num_to_PC_U(10,Ch_num[m]);sum += Ch_num[m];
if(!m)sendString("\t\tHex and Decimal numbers");
if(m == 3){sendString("\t\tSum to\t");Num_to_PC(16,sum);
sendString("\t"); Num_to_PC(10, sum);}}
newline();


newline();
sendString("Press any key\r\n");
waitforkeypress();
newline();

sum = print_array (18, I_data, 1);								//Print 18 decimal numbers
Num_to_PC(10,sum);newline();

sum = print_array (18, I_data+36, 2);							//Print 18 decimal numbers
Num_to_PC(10,sum);newline();

sum = print_array (10, I_data+72, 3);							//Print 10 hex numbers
Num_to_PC(16,sum);newline();

sum = print_array (10, I_data+92, 4);							//Print 10 hex numbers in negative form								
Num_to_PC(16,sum);newline();

sum = print_array (8, L_data, 5);
Num_to_PC(16,sum);newline();

sum = print_array (8, L_data+32, 6);
Num_to_PC(16,sum);newline();

sum = print_array (12, T_data, 7);
Num_to_PC(16,sum);newline();

sum = print_array (12, T_data+24, 8);
Num_to_PC(16,sum);newline();

sum = print_array (6, large_nos, 9);
Num_to_PC(16,sum);newline();

wdt_enable(WDTO_1S); while(1);}




/************************************************************************************************/
void Text_from_EEPROM(int EEPROM_address){						//Read a text string from the EEPROM
char temp_char;
int m = 0;
while(m < 150){													//Escape if reading an empty EEPROM
temp_char =  eeprom_read_byte((uint8_t*)(EEPROM_address++));   		
if (temp_char != '\0') sendChar(temp_char);
else {newline(); break;} m++;}}



/************************************************************************************************/
char Char_from_EEPROM(int location){							//Read a single EEPROM location
return eeprom_read_byte((uint8_t*) location);}



/************************************************************************************************/
int I_num_from_EEPROM(int location){							//Read an integer number from the EEPROM
return  (Char_from_EEPROM(location) << 8)\
+ (Char_from_EEPROM(location+1));}



/************************************************************************************************/
long L_num_from_EEPROM(int location){							//Read a long number from the EEPROM
long num, temp;
num = (Char_from_EEPROM(location) << 8)\
+ (Char_from_EEPROM(location+1));
temp = (Char_from_EEPROM(location+2) << 8)\
+ (Char_from_EEPROM(location+3));
if(temp < 0) num += 1;
num = (num << 16) + temp;
return num;}



/*******************These subroutines are included for test prposes only**********************************/
long print_array (char length, int start_address, char type){

int int_data;
long long_data;
long sum = 0;

for(int m = 0; m < length; m++){

switch(type){
case 1:
case 2:

int_data = I_num_from_EEPROM(start_address + (2*m));
sum += int_data;
Num_to_PC(10, int_data); break;

case 3:
int_data = I_num_from_EEPROM(start_address + (2*m));
sum += int_data;
Num_to_PC(16, int_data); break;

case 4:
int_data = I_num_from_EEPROM(start_address + (2*m));
sum += int_data;
Num_to_PC(16,int_data); break;

case 5:
long_data = L_num_from_EEPROM(start_address + (4*m));
sum += long_data;
Num_to_PC_U(16, long_data);break;

case 6:
long_data = L_num_from_EEPROM(start_address + (4*m));
sum += long_data;
Num_to_PC(16,long_data); break;

case 7:
case 8:
int_data = I_num_from_EEPROM(start_address + (2*m));
sum += int_data;
Num_to_PC(16,int_data);break;

case 9:
long_data = L_num_from_EEPROM(start_address + (4*m));
sum += long_data;
Num_to_PC(16,long_data);break;

}sendChar(' ');}



switch(type){
case 1:sendString("\tDecimal numbers\t\t\t\t");break;	
case 2: sendString("\t\tDecimal numbers\t\t\t\t");break;	
case 3: sendString("\t\t\t\tHex integers\t\t\t\t");break;	
case 4: sendString("\t\t\tHex signed integer numbers\t\t");break;	
case 5: sendString("\t\t\tHex long numbers\t\t\t");break;	
case 6: sendString("\t\tHex signed long numbers\t\t\t");break;	
case 7: sendString("\t\t\t\tHex integers\t\t\t\t");break;	
case 8: sendString("\t\t\t\tHex integers derived from decimal\t");break;	
case 9: sendString("\t\tLarge numbers\t\t\t\t");
}return sum;
}

