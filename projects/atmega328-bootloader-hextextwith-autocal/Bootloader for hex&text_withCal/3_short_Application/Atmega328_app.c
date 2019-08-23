/*

Sample application program for use with the combined hex/text bootloader.


This program contains two subroutines, one to count the number of text strings and one to print out
any given string.  The first string starts at location 0x6C7F, and occupies addresses below 0x6C7F.
The application and its text srings share the flash from addresses zero to 0x6C7F (28kB).

The appliation runs under the folowing configuration byte settings.

extended_fuse: 0xFD				Sets BOD (This is non-critical)
fuse_H:  0xD0					max boot size, resets to boot partion, WDT under program control, Ext reset enabled. EEPROM preserved
fuse: 0xC2						8MHz internal RC clock


The important settings are the size of the boot partition, external reset, the reset vector and the Watch Dog Timer (WDT).
EEPROM must also be preserved since the final 3 locations are used to store clock calibration data.
*/


/*Compile it using optimisation level s ONLY
Rx/Tx work at 57.6k
*/

/*EEPROM reservations
0x3FF	user cal if set
0x3FE	user cal if set
0x3FD	Default cal supplied by Atmel

*/





#include "Atmega328_app.h"


int char_counter;


char string_counter(int);
void print_string_num(int, int);



int main (void){ 

char keypress, Num_strings;															//The number of strings written to flash
int  start_address; 																//Address in flash of first character in a string,
int  text_num;																		//The number of the string to be printed out



setup_HW;

char_counter = 0;																	//counts the number of characters in the text file (excludes \r & \n)
cal_device;																			//checks cal status and adopts user cal bytes if present (Optional)


while(1){																			//User prompt
do{sendString("w/s  ");}while((isCharavailable(300) == 0));
keypress = receiveChar();
if (keypress == 'w') break;															//press "w" to continue with program execution
if(keypress == 's') {wdt_enable(WDTO_60MS); while(1);}}

newline(); 
start_address = 0x6C7F;																//start adddress of text
Num_strings = string_counter(start_address);										//Count the number of strings
sendString("Total numbers of strings & characters are  ");
Num_to_PC(10,Num_strings); sendChar('\t');
Num_to_PC(10,char_counter);

sendString("\r\nEnter string number or zero to exit");								//Request string

while(1){
text_num = Num_from_KBD();
if(!(text_num))break;
if (text_num > Num_strings){sendString("\r\nNo string!"); continue;}
sendString("\r\n");
Num_to_PC(10,text_num);sendString("\r\n");
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
if(Flash_readout)sendChar(Flash_readout); else break;
line_length += 1;
if ((Flash_readout == ' ') && (line_length > 90))
{sendString("\r\n");line_length = 0;}
start_address -= 1; }}


