
/*
Introduces subroutines necessary to read text from Flash
that has been programmed using the "UNO_UPDI_programmer".
*/
#define device_type 1606
#define text_bytes  2048

//#define device_type 806
//#define text_bytes  1024


#include "Text_reader.h"


int main (void){

unsigned char  Num_strings;											//The number of strings written to flash or EEPROM
int  text_num;														//The number of the string to be printed out

setup_HW;
User_prompt;

text_start_address = flash_start + ((FlashSZ*2) - text_bytes);
txt_ptr = (char*)text_start_address;
char_counter = 0;													//counts the number of characters in the text file (excludes \r & \n)


if(*(txt_ptr + 1) == 0xFF){
string_to_PC("\r\nNo text available in flash!\r\n");
wdt_enable(WDTO_60MS); while(1);}
string_to_PC("\r\nReading strings from flash.");

Num_strings = string_counter();										//Count the number of strings
string_to_PC("\r\nTotal numbers of strings & characters are  ");
Num_to_PC(10,Num_strings); char_to_PC('\t');
Num_to_PC(10,char_counter);
string_to_PC("\r\nEnter string number or zero to exit");			//Request string

while(1){
text_num = Num_from_KBD(Int_array);									//User enters number of text string that is required
if(!(text_num))break;
if (text_num > Num_strings){string_to_PC("\r\nNo string!\r\n"); 
continue;}
string_to_PC("\r\n");
Num_to_PC(10,text_num);string_to_PC("\r\n");
print_string_num(text_num);}

wdt_enable(WDTO_60MS); while(1);
return 1;}



/***************************************************************************************************************************************************/
unsigned char string_counter(void){						 //Scroll through text section of flash counting the '\0' chars
char next_char, previous = 0;							//until '\0' '\0' is detected to indicate the end of the last string
unsigned char counter = 0;

txt_ptr = (char*)text_start_address;
next_char_ptr = 0;
		
	while(1){          
		next_char = get_next_char();
											
		if(next_char == 0){counter += 1; char_counter += 1;
			if (!(previous)) return counter-1;}
		else char_counter += 1;
		previous = next_char;}}



/***************************************************************************************************************************************************/
void print_string_num(int text_num){				//scroll through text section of flash counting '\0' chars
int null_counter = 1;								//until the start of the required string
			
txt_ptr = (char*)text_start_address;				//Start of text section
next_char_ptr = 0;
line_length = 0;
while(1){
if(null_counter == text_num)break;
next_char = get_next_char();
if(next_char == 0)null_counter += 1;}

while(1){											//Print string; exit at '\0' character
next_char = get_next_char();
if(next_char)char_to_PC(next_char); else break;
line_length += 1;
if ((next_char == ' ') && (line_length > 90))		//avoid long lines of print
{string_to_PC("\r\n");line_length = 0;}
next_char = get_next_char();
			
if(next_char)char_to_PC(next_char); else break;
line_length += 1;
if ((next_char == ' ') && (line_length > 90))		//avoid long lines of print
{string_to_PC("\r\n");line_length = 0;}
}}



/************************************************************************************************************/
char get_next_char(void){							//Chars cannot simply be printed out in the order in which they are saved 
															
switch (next_char_ptr){
	case 0: text_charL = *txt_ptr;					//text_charL must be printed before text_charH
			txt_ptr += 1;
			text_charH = *txt_ptr; 
			next_char_ptr = 1;
			return text_charH; break;
	case 1:  
			next_char_ptr = 0;
			txt_ptr += 1;
			return text_charL;break;}
return 0;}
	


