
/*
Introduces subroutines necessary to read text from EEPROM and Flash
that has been programmed using the "8_UNO_programmer".

At user prompt press 'r' to read flash and 'R' to read EEPROM.
*/


#include "Text_reader.h"

unsigned char line_length = 0;
char next_char;



int main (void){

unsigned char  Num_strings;											//The number of strings written to flash or EEPROM
int  next_address;													//Address in memory of next character,
int  text_num;														//The number of the string to be printed out
int index=0;														//Address in EEPROM of the first string pointer

setup_HW;
User_prompt;

sendString("\r\nProgram running on ");
sendString (Device_family[family_ptr]);
sendString (Device_type[device_ptr]);


if(User_response == 'r')

{char_counter = 0;													//counts the number of characters in the text file (excludes \r & \n)
next_address = FlashSZ*2 - 1;										//start address of text

if(Char_from_flash(next_address) == 0xFF){
	sendString("\r\nNo text available in flash!\r\n");
	wdt_enable(WDTO_60MS); while(1);}
sendString("\r\nReading strings from flash.");

Num_strings = string_counter(next_address);							//Count the number of strings
sendString("\r\nTotal numbers of strings & characters are  ");
Num_to_PC(10,Num_strings); sendChar('\t');
Num_to_PC(10,char_counter);
sendString("\r\nEnter string number or zero to exit");              //Request string

while(1){
	text_num = Num_from_KBD();
	if(!(text_num))break;
	if (text_num > Num_strings){sendString("\r\nNo string!\r\n"); continue;}
	sendString("\r\n");
	Num_to_PC(10,text_num);sendString("\r\n");
	print_string_num(text_num,next_address);}}




if(User_response == 'R'){

index = (eeprom_read_byte((uint8_t*)(0)) << 8) +
 (eeprom_read_byte((uint8_t*)(1)));									//Address of first data item (pointer to first string)
Num_strings = 	eeprom_read_byte((uint8_t*)(5));					//Number of text strings

if(Num_strings == 0xFF){
	sendString("\r\nNo text available in EEPROM!\r\n");
wdt_enable(WDTO_60MS); while(1);}
sendString("\r\nReading strings from EEPROM.");

sendString("\r\nTotal numbers of strings is  ");
Num_to_PC(10, (Num_strings));
sendString("\r\nEnter string number or zero to exit\r\n");			//Request string


while((text_num = Num_from_KBD())){									//User enters string number
if (text_num > Num_strings)
{sendString("\r\nNo string!\r\n");continue;}

next_address = 
(eeprom_read_byte((uint8_t*)(index + (text_num-1)*2)) << 8) +
 eeprom_read_byte((uint8_t*)(index +(text_num - 1)*2 + 1));

newline();
line_length = 0;
while(1)															//Print characters until '\0' is encountered.
{next_char = eeprom_read_byte((uint8_t*)(next_address++));
if(next_char)sendChar(next_char); else break;
line_length += 1;	
if ((next_char == ' ') && (line_length > 90))						//avoid long lines of print
{sendString("\r\n");line_length = 0;}
}newline();
	
}}

wdt_enable(WDTO_60MS); while(1);
return 1;}



	/***************************************************************************************************************************************************/
	unsigned char string_counter(int next_address){                //Scroll through text section of flash counting the '\0' chars
		char next_char, previous = 0;								//until '\0' '\0' is detected to indicate the end of the last string
		unsigned char counter = 0;
		
		while(1){          
			next_char = Char_from_flash(next_address);             //result provided by assembly subroutine
			if(next_char == 0){counter += 1; char_counter += 1;
				if (!(previous)) return counter-1;}
			else char_counter += 1;
			previous = next_char;
		next_address -= 1;}}



		/***************************************************************************************************************************************************/
		void print_string_num(int text_num, int next_address){      //scroll through text section of flash counting '\0' chars
			int null_counter = 1;									 //until the start of the required string
			
			line_length = 0;
			while(1){
				if(null_counter == text_num)break;
				next_char = Char_from_flash(next_address);
				if(next_char == 0)null_counter += 1;
			next_address -= 1;}

			while(1){												//Print string; exit at '\0' character
				next_char = Char_from_flash(next_address);
				if(next_char)sendChar(next_char); else break;
				line_length += 1;
				if ((next_char == ' ') && (line_length > 90))		//avoid long lines of print
				{sendString("\r\n");line_length = 0;}
			next_address -= 1; }}





			char Char_from_flash(int next_address){

				Prog_mem_address_H = next_address >> 8;
				Prog_mem_address_L = next_address;

				asm volatile ("push r0") ;									//Save registers before inserting assembly code
				asm volatile ("push r31") ;
				asm volatile ("push r30") ;

				asm volatile ("lds r31, Prog_mem_address_H");				//load ZH & L pointer from static RAM 
				asm volatile ("lds r30, Prog_mem_address_L");

				asm volatile ("lpm r0, Z");									//load program memory into register r0

				asm volatile ("sts Flash_readout,  r0");					//Store contents of r0 in static RAM

				asm volatile ("pop r30");									//Restore registers
				asm volatile ("pop r31");
				asm volatile ("pop r0");

			return Flash_readout;}

