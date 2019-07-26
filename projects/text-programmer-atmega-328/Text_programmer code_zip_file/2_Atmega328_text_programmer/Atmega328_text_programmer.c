
/*Text programmer for Atmega 328

This program saves strings for use by an application, directly to flash 
avoiding the need for them to be copied to SRAM at run time.
This makes large amounts of space for text available to the application. 
*/



/*
To make use of this program first compile the application program.
Then append the text programmer hex file to that of the application.
Program the target device with the resulting hex file.

After programming or at any reset, the target device control jumps to 0x7000
and starts running the "text_programmer" code.
This starts by checking the flash at the start of the text section.
If it contains 0xFFFF, "text_programmer" continues and requests a text file.
Otherwise control jumps to 0x0000 and the application code runs.

Before the file containing the text strings is downloaded Timer T0 is started.
As each character of the text file is received T0 is reset.  The end of the file 
is detected when T0 times out.  

The text file contains three sections:
	The first, before a row of * characters contains an optional introduction to the data
	The second immediately after the row of * characters contains optional carriage returns
	The third contains the text strings to be saved to flash (which may contain additional * characters).

The download process is controlled using two variables: 
	"text_started" which increments from zero to three as the text file dowload is detected and the preliminary data is processed.
	"end_of_text" decrements from three to zero as the file download times out and two '\0' chars are appended.

Note: a single '\0' char indicates the end of a string and two indicate the end of the text data.


As the text file is downloaded its characters are temporarily stored in an array from  where they are copied to a page buffer.
When the buffer is full its contents are written to flash, a proccess that takes several ms.


subroutines "print_string_num()" and "string_counter()" are also called by the application code.

The assembly file used to read from flash and write to it was initially copied from the 328 bootloader application.  There was 
a problem however: unexpected gaps appeared in the text. This was rectified by adding cli/sei to the "do_spm" subroutine.


Note: 
The line "address_in_sram = store + r_pointer;" generates a warning.
A line of this type is unusual for an all C implementation
However in this case a .S file is included.
*/


/*Compile it using optimisation level s ONLY
Rx/Tx work at 57.6k
*/

/*EEPROM reservations: these are optional.
0x3FF	user cal if set
0x3FE	user cal if set
0x3FD	Default cal supplied by Atmel
*/






#include "Atmega328_text_programmer.h"
#define Version "Atmega 328 text programmer_V1.8 "


volatile char endoftext, text_started;
volatile char Rx_askii_char_old;									//Required to check for a \r\n string

char errror_counter;


int main (void){ 													//Loaded at address 0x7000, the start of the bootloader section
char keypress;

setup_HW;
cal_device;	


address_in_flash = 0x6FFF;											//Read the contents of the flash at location 0x6FFF
Prog_mem_address_H = address_in_flash >> 8;
Prog_mem_address_L = address_in_flash;
read_flash (); 
if(Flash_readout != 0xFF)											//If the flash has been programmed
{MCUCR = (1<<IVCE);MCUCR = 0x0;									//select interrupt vector table at 0x0000 and jump to
asm("jmp 0x0000");}													//location 0x0000 (i.e. to the application). Otherwise


MCUCR = (1<<IVCE);  												//select the interrupt vector table at the start of boot section
MCUCR = (1<<IVSEL);												//and proceed to program the  flash


while(1){															//User prompt
do{sendString("?  ");}
while((isCharavailable(300) == 0));
keypress = receiveChar(); 
if (keypress == 'V')												//Press 'V' to check the program version
{sendString (Version);newline();
wdt_enable(WDTO_60MS); while(1);}
if (keypress == '?')break;}
   
sendString ("\r\nSend and then echo file \r\n\r\n");

while(1){ 

Program_Flash();													//Run programer subroutine
Verify_Flash(); 													//Echo text file

if(!(errror_counter)){												//No errors detected, but file can still be rejected

sendString("\r\nOK? Y or N  ");										//Note:  Errors resulted from an earlier version of 
do 																	//the .S file and are no longer anticipated 
	{keypress = waitforkeypress();
	if(keypress == 'N')
	{sendString ("\r\nSend it again!\r\n");break;}					//'N': break out of do-loop
	else sendString("?  ");}										//unwanted key: repeat do-loop
while(keypress != 'Y');												//'Y': break out of while loop
if(keypress == 'Y')break;}

else {short_num_to_PC(errror_counter); 								//Errors detected but file can be acepted if wanted
	sendString ("  Errors detected. OK Y or N\r\n");
do 
	{keypress = waitforkeypress();
	if(keypress == 'N')
	{sendString ("\r\nSend it again!\r\n");break;}					//'N': break out of do-loop
	else sendString("?  ");}										//unwanted key: repeat do-loop
while(keypress != 'Y');												//'Y': break out of while loop
if(keypress == 'Y')break;}

} 	

newline();wdt_enable(WDTO_60MS); 									//Do a SW reset and jump to the 
while(1); return 1;}												//application code starting at 0x0000








/***************************************************************************************************************************************************/
ISR(USART_RX_vect){
	
unsigned char Rx_askii_char;
	
Rx_askii_char = receiveChar();


if((Rx_askii_char != '*') && (!(text_started)))return;			//Ignore header text occuring before a line of * characters
if((Rx_askii_char == '*')&&(!(text_started)))						//The number of * characters is not critical only one is required
{ text_started = 1;return;}
if((Rx_askii_char == '*')&&(text_started == 1))return; 			//Ignore carriage returns occurring before the first text string
else {if (text_started == 1)text_started = 2;}
if((text_started == 2) && ((Rx_askii_char == '\r')
 || (Rx_askii_char == '\n')))return; else text_started = 3;		//Enter main area of text file containing strings to be written to flash


if ((Rx_askii_char == '\r') || (Rx_askii_char == '\n')){			//Check for '\r' and or '\n' and convert to a single '\0' character
if (Rx_askii_char_old == '\0') return;
else Rx_askii_char = '\0';}

TCNT0 = 0;															//Reset the timer. An interrrupt is only required at the end of the file

if(!(counter%2)){													//Each location in array "store" holds two askii characters
store[w_pointer] = Rx_askii_char;
store[w_pointer] = store[w_pointer] << 8;}							//"w_pointer" gives address in array of the next free location
else{store[w_pointer] += Rx_askii_char;
inc_w_pointer;}
counter = (counter + 1);											//"counter" gives the number of characers downloaded
Rx_askii_char_old = Rx_askii_char;} 
 



/***************************************************************************************************************************************************/
ISR(TIMER0_OVF_vect) {												//Timer0 times out and halts at the end of the text file
if(text_started == 3)												//Ignore timeouts occurring before start of file download
	{endoftext -= 1; TCCR0B = 0;TIMSK0 &= (~(1 << TOIE0));		//Shut timer down
	inc_w_pointer; store[w_pointer] = 0;							//Append two '\0' chars to the end of the text
	inc_w_pointer; store[w_pointer] = 0; }}



/***************************************************************************************************************************************************/
void Program_Flash (void){


w_pointer = 0; r_pointer = 0;
text_started =0; endoftext =3;counter = 0;
Rx_askii_char_old = '0';

for(int m = 0; m<=0x1F; m++)store[m] = 0;							//clear contents of array "store" (optional)

Timer_T0_sub_with_interrupt(5,0);									//Start Timer0 with interrupt
UCSR0B |= (1<<RXCIE0); 											//Activate UART interrupt
sei();																//Set global interrupt

address_in_flash = 0x6FFE;											//First character will be storrred at 0x6FFF not 0x6FFE

while (1){
while (r_pointer == w_pointer);										//wait for w_pointer to be incremented

if(endoftext != 3)endoftext -= 1;									//Indicates that the timer has been shut down

address_in_sram = store + r_pointer;								//Address in SRAM of characters to be written to flash
loc_in_mem_H = address_in_sram >> 8;								//Variables used to pass the address to the assembly routines
loc_in_mem_L = address_in_sram;

Prog_mem_address_H = address_in_flash >> 8;							//Pass address in flash to the assembly routines
Prog_mem_address_L = address_in_flash;								//at which the text is to be written

write_to_page_buffer();												//assembly routine (note: these always leave the global interrupt flag set)
	
address_in_flash -= 2;												//next address in flash
store[r_pointer] = 0;												//clear the contents of the location in array store
inc_r_pointer;														//restore the value of "r_pointer" to that of "w_pointer"

if (!((0x6FFE - address_in_flash)%128)){							//If page buffer is full
address_in_flash += 2;												//Get the address of the first entry in the page
Prog_mem_address_H = address_in_flash >> 8;							//Prepare the address for the assemble routines
Prog_mem_address_L = address_in_flash;

Page_erase ();														//Assembly routine
page_write();														//Assembly routine
address_in_flash -=2;}												//Restore address_in_flash
if(!(endoftext)) break;}											//Break when two '\0' chars have been appended to text stored in the array

if((0x6FFE - address_in_flash)%128){								//Write remaining chars in partialy full page buffer
address_in_flash += (0x6FFE - address_in_flash)%128 - 126;			//Get address of first character in the page

Prog_mem_address_H = address_in_flash >> 8;
Prog_mem_address_L = address_in_flash;

Page_erase ();
page_write();}}






/***************************************************************************************************************************************************/
void Verify_Flash (void){											//Echo the entire contents of the flash
int start_address = 0x6FFF;
char Num_strings;

errror_counter = 0;

clear_read_block();													//Required to enable user to read the flash

Num_strings = string_counter(start_address);						//Check the number of strings
for(int m = 0; m < Num_strings; m++)								//Print them out one at a time
{print_string_num(m+1,start_address);
sendString("\r\n\r\n");

cli();																//Required after the last assemble routine has ran.

}}



/***************************************************************************************************************************************************/
void print_string_num(int text_num, int start_address){				//scroll through text section of flash counting the'\0' chars
int null_counter = 1; 												//untill the start of the required string is encountered
char next,line_length = 0;		

while(1){	
if(null_counter == text_num)break;
Prog_mem_address_H = start_address >> 8;
Prog_mem_address_L = start_address;
read_flash ();	
next = Flash_readout;
if(next == 0)null_counter += 1;
start_address -= 1;}

while(1){	
Prog_mem_address_H = start_address >> 8;							//Scroll through the required string printing and   
Prog_mem_address_L = start_address;									//counting the characters
read_flash ();														 

if(Flash_readout == 0xFF)errror_counter += 1;						//check the string for errors due to failure to program the flash location

if(Flash_readout)sendChar(Flash_readout); else break;				
line_length += 1;													//If the string contains more than 90 chars insert a carriage return
if ((Flash_readout == ' ') && (line_length > 90))					//before the start of the next word.
{sendString("\r\n");line_length = 0;}
start_address -= 1; }}



/***************************************************************************************************************************************************/
char string_counter(int start_address){								//Scroll through text section of flash counting '\0' chars
char counter = 0, next, previous = 0; 								//untill '\0' '\0' is detected to indicate the end of the
int char_counter;													//last string.  (Note: "char_counter" is only used in the "application")

while(1){															

Prog_mem_address_H = start_address >> 8;
Prog_mem_address_L = start_address;
read_flash ();														//assembly subroutine
next = Flash_readout;												//result provided by assembly subroutine

if(next == 0){counter += 1; 
if (!(previous)) return counter-1;}								//Detect the '\0','\0' sequence.
else char_counter += 1;

previous = next;
start_address -= 1;}}


























