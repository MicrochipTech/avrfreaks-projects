


/*Prints out text files saved to flash but does not include the comentary mode*/


/*

Disconnect the programmer,power up and press the reset button.

At the h/t/r/D prompt press "t" and download the FP_arithmetic commentary file followd by the Text_printer hex file

At each  the next two user prompts press zero

The following print out should be generated

		Hex_file_size:	0x04B5   d'loaded:  0x04B5  in:  0x04B5  out
		UNO Fuses E, H, L and lock:	FD	D0	FF	EF
		
At the h/t/r/D prompt press "r" to run the program

*/







#include "Text_printer_AVR_header.h"
#include "Text_printer_AVR_subroutines.c"

int char_counter;


char string_counter(int);
void print_string_num(int, int);



int main (void){ 

char keypress, Num_strings;															//The number of strings written to flash
int  start_address; 																//Address in flash of first character in a string,
int  text_num;																		//The number of the string to be printed out
char MCUSR_copy;


MCUSR_copy = MCUSR;

setup_HW;

MCUSR = 0;



char_counter = 0;																	//counts the number of characters in the text file (excludes \r & \n)


while(1){																			//User prompt
do{String_to_PC("w/s  ");}while((isCharavailable(255) == 0));
keypress = receiveChar();
if (keypress == 'w') break;															//press "w" to continue with program execution
if(keypress == 's') {wdt_enable(WDTO_60MS); while(1);}}


start_address = 0x5BFF;		
Prog_mem_address_H = start_address >> 8;
Prog_mem_address_L = start_address;
read_flash ();
if (Flash_readout == 0xFF){String_to_PC("No text. Reset UNO\r\n");while(1);}	


String_to_PC("\r\n"); 
start_address = 0x5BFF;																//start adddress of text
Num_strings = string_counter(start_address);										//Count the number of strings
String_to_PC("Total numbers of strings & characters are  ");
Num_to_PC(10,Num_strings); Char_to_PC('\t');
Num_to_PC(10,char_counter);

String_to_PC("\r\nEnter string number or zero to exit");							//Request string

while(1){
text_num = Num_from_KBD();
if(!(text_num))break;
if (text_num > Num_strings){String_to_PC("\r\nNo string!"); continue;}
String_to_PC("\r\n");
Num_to_PC(10,text_num);String_to_PC("\r\n");
print_string_num(text_num,start_address);}

wdt_enable(WDTO_60MS); while(1);

return 1;}



/***************************************************************************************************************************************************/
char string_counter(int start_address){												//Scroll through text section of flash counting the '\0' chars
char counter = 0, next, previous = 0; 												//untill '\0' '\0' is detected to indicate the end of the
while(1){																			//last string

Prog_mem_address_H = start_address >> 8;
Prog_mem_address_L = start_address;
read_flash ();																		//assembly subroutine
next = Flash_readout;																//result provided by assembly subroutine

if(next == 0){counter += 1; if (!(previous)) return counter-1;}
else char_counter += 1;

previous = next;
start_address -= 1;}}



/***************************************************************************************************************************************************/
void print_string_num(int text_num, int start_address){								//scroll through text section of flash counting '\0' chars
int null_counter = 1; 																//untill the start of the required string
char next,line_length = 0;															//Print the characters untill '\0' is detected

while(1){	
if(null_counter == text_num)break;
Prog_mem_address_H = start_address >> 8;
Prog_mem_address_L = start_address;
read_flash ();	
next = Flash_readout;
if(next == 0)null_counter += 1;
start_address -= 1;}

while(1){	
Prog_mem_address_H = start_address >> 8;
Prog_mem_address_L = start_address;
read_flash ();
if(Flash_readout)Char_to_PC(Flash_readout); else break;
line_length += 1;
if ((Flash_readout == ' ') && (line_length > 90))
{String_to_PC("\r\n");line_length = 0;}
start_address -= 1; }}


