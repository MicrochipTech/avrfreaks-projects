
/*Text and hex bootloader with autoCal for the Atmega 328
Enables the internal RC clock to be reliably used provided that a  32,768 watch crystal is available for calibration purposes 

The Bootloader starts at address 0x7000 and occupies all the space up to address 0x6FFF.
The autocal routinte is loaded into flash at address 0x6C80
Applications are programmed into flash starting a address zero.
Text is programmed into flash starting at address 0x6C7F.
Hex and text files share 27.776kBytes out of the total 32.768KBytes.


This program consists of cut down versions of the 328 bootloader and the text programer previously posted on AVR freaks. 
In each case verification has been abandoned.  It was found in practice that the programmer either worked all the time or 
none of the time. Verification which was essential during the development process is no longer realy required.

Once it has been loaded into the boot section of the Atmega 328 starting at address 0x7000 it can be used to
program applications and text into the first 27kByte of flash using a terminal program such as Tera Term or Br@y++.
The baud rate is 57600, there are 8 data bits and one stop bit. There is no handshaking and no parity.


If the size of the hex file is more than 50% that of the application section it may be necessary to split the hex
file into two files of approximately equal size.  This is because of limitations in the terminal emmulator.  Having
initiated the download of the first half wait for the downoad to stop (i.e. the led if implemented stops flashing).
Then send the second file.

Like the 328 bootloader an external reset is required to trigger the programmer.  The resulting user prompt is h/t/r enabling the 
user to program hex, text or run the application by echoing the prompt.  It is also possible tolock the device to further programming
by making key presses 'L' and 'Y'. Once this has been done the user prompt is replaced by a single '!' and program controll jumps straight
to the user application.

Once the device has been locked it must be reprogrammed with the text/hex bootloader to clear the lock bits.
Note: if using the USBasp programmer it is necessary to reprogram the flash before the lock byte can be reprogrammed.


Note: The compiler produces 3 identical warnings "assignment makes integer from pointer without a cast".  These are due to the way in which
pointers are passed to the assembly routines and are OK.


The Atmega 328 bootloader runs under configuration bytes:
Extended	0xFD	This sets the BOD settings which are non critical to this application
High		0xD0	This configures the Reset pin, WDT, Boot size and reset vector and ensures that the EEPROM is preserved during chip erase
Fuse		C2		This configures the clock and SUT.  An 8MHz clock is assumed but its souce is not critical
Lock		EF		To protect the boot space
			EB 		To protect both boot and application regions


Calibrating the devices:
Holding the Atmega 328 in reset while applying power generates combined POR and Ext reset flags.  
This causes program control to jump to location 0x6C80 and the calibration routine then runs,writing the new calibration byte to locations 0x3FE and 0x3FF.
This jump is initiated by the bootloader and does not rely in any way on code in the user application.
(Note: once power has been applied continue to hold the device in reset for a moment. The calibration process then requires a second for the
crystal to stabilise.) 


An external reset always causes the bootloader to run.
Following the autocal routine both the WDT and EXT reset flags are set. This ensures that the bootloader runs so that the application responsible 
for the cal disruption can be cleared.

User applications will either register a WDT reset, BOR or no reset.  If it registers no reset then the reset was actually POR.  
The PORF is deleted by the bootloader to prevent the autoCal routine running after every POR the first time that the device is reset.

*/



/*
*Compile it using optimisation level s ONLY
Rx/Tx work at 57.6k with no handskaking, no parity, 8 data bits and 1 stop bit

EEPROM reservations: 
0x3FF	user cal if set
0x3FE	user cal if set
0x3FD	Default cal supplied by Atmel
*/






#include "Hex_txt_programmer.h"
#define Version "Hex_txt_programmer_V1 "



char mode;													//'h' for hex file, 't' for text file


int main (void){ 											//Loaded at address 0x7000, the start of the bootloader section
char L_Byte;
int L_Byte_ptr;

setup_HW;													//Uart, WDT etc

if((MCUSR & (1 << EXTRF))\
&& (MCUSR & (1 << PORF)))									//Combined POR and Ext Reset 	
{MCUSR &= (~(1 << PORF));									//Clear POR reset flag and leave the Ext reset flag set
asm("jmp 0x6C80");}											//Jump to 0x6C80 and run the calibration routine

if(MCUSR & (1 << EXTRF));									//Ext reset flag set: skip jump to zero
else{MCUSR &= (~(1 << PORF));								//Clear unwanted PORF if present
asm("jmp 0x0000");}											//Jump to location zero and run the aplication

cal_device;	                                                //Check for user calibration byte and apply if present

MCUCR = (1<<IVCE);  										//Select the interrupt vector table at the start of boot section
MCUCR = (1<<IVSEL);
MCUSR = 0;													//Clears outstanding reset flags

while(1){	

if(Lock_byte() == 0xEB)                                     //Exit programming mode if device is locked
{sendChar ('!');wdt_enable(WDTO_60MS); while(1);}

do{sendString("h/t/r  ");}
while((isCharavailable(300) == 0));                         //User prompt
switch (receiveChar()){                         
case 'h': hex_programmer(); break;
case 't': text_programmer(); break;
case 'r': wdt_enable(WDTO_60MS); while(1);break;           //application code starting at 0x0000

case 'L': sendString ("LB?\r\n");			    	        //Offer user the opportunity to protect the device from reprogramming
if (waitforkeypress() == 'Y'){L_Byte = 0xEB;
L_Byte_ptr = &L_Byte;	        				            //Required to pass a pointer to the .S routine (results in a warning message)
loc_in_mem_H = L_Byte_ptr >> 8;
loc_in_mem_L = L_Byte_ptr;
address_in_flash = 1;
Prog_mem_address_H = address_in_flash >> 8;
Prog_mem_address_L = address_in_flash;
set_lock_byte();}break;

default: break;}                                            //Illegal key presses: repeat while loop
}

return 1;}								                    








/***************************************************************************************************************************************************/
ISR(USART_RX_vect){
	
unsigned char Rx_askii_char;
	
Rx_askii_char = receiveChar();

if (mode == 't')get_text(Rx_askii_char);
if (mode == 'h')get_hex(Rx_askii_char);}
 



/***************************************************************************************************************************************************/
ISR(TIMER0_OVF_vect) {												//Timer0 times out and halts at the end of the text file
if(text_started == 3)												//Ignore timeouts occurring before start of file download
	{endoftext -= 1; TCCR0B = 0;TIMSK0 &= (~(1 << TOIE0));		//Shut timer down
	inc_w_pointer; store[w_pointer] = 0;							//Append two '\0' chars to the end of the text
	inc_w_pointer; store[w_pointer] = 0; }}




/***************************************************************************************************************************************************/
void text_programmer (void){


mode = 't';
w_pointer = 0; r_pointer = 0;
text_started =0; endoftext =3;txt_counter = 0;
Rx_askii_char_old = '0';

sendString("\r\nText?  ");

Timer_T0_sub_with_interrupt(5,0);									//Start Timer0 with interrupt
UCSR0B |= (1<<RXCIE0); 											//Activate UART interrupt
sei();																//Set global interrupt

address_in_flash = 0x6C7E;											//First character will be storred at 0x6C7F not 0x6C7E

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

if (!((0x6C7E - address_in_flash)%128)){							//If page buffer is full
address_in_flash += 2;												//Get the address of the first entry in the page
Prog_mem_address_H = address_in_flash >> 8;							//Prepare the address for the assembly routines
Prog_mem_address_L = address_in_flash;

Page_erase ();														//Assembly routine
page_write();														//Assembly routine
address_in_flash -=2;}												//Restore address_in_flash
if(!(endoftext)) break;}											//Break when two '\0' chars have been appended to text stored in the array

if((0x6C7E - address_in_flash)%128){								//Write remaining chars in partialy full page buffer
address_in_flash += (0x6C7E - address_in_flash)%128 - 126;			//Get address of first character in the page

Prog_mem_address_H = address_in_flash >> 8;
Prog_mem_address_L = address_in_flash;

Page_erase ();
page_write();
}cli();sendString("OK\r\n");}




/*********************************************************************************************************/
void hex_programmer(void){

mode = 'h';
PageSZ = 0x40; PAmask = 0x3FC0;										//Define flash memory parameters

prog_led_control = 0;  record_length_old=0;
Flash_flag = 0;  HW_address = 0;  section_break = 0; orphan = 0; 
w_pointer = 0; r_pointer = 0; short_record=0;

sendString("\r\nHex?  ");

UCSR0B |= (1<<RXCIE0); sei();										//Receive interrupts now active

new_record();  														//Start reading first record which is being downloaded to array "store" 
start_new_code_block(); 											//Initialise new programming block (usually starts at address zero but not exclusivle so)
Program_record();													//Coppy commands from array "store" to the page_buffer														
			
		
while(1){		
new_record();														//Continue reading subsequent records
if (record_length==0)break; 										//Escape when end of hex file is reached


if (Hex_address == HW_address){										//Normal code: Address read from hex file equals HW address and lines contains 8 commands
switch(short_record){
case 0: if (space_on_page == (PageSZ - line_offset))				//If starting new page
			{page_address = (Hex_address & PAmask);}break;			//get new page address
			

case 1:	start_new_code_block();										//Short line with no break in file (often found in WinAVR hex files).
		short_record=0;break;}}
		
		
if(Hex_address != HW_address){										//Break in file
	if (section_break){												//Section break: always found when two hex files are combined into one 										
		if((Flash_flag) && (!(orphan)))
		{write_page_SUB(page_address);}								//Burn contents of the partially full page buffer to flash
		if(orphan) write_page_SUB(page_address + PageSZ);} 		//Burn outstanding commands to the next page in flash			
		
	if(page_break)													//In practice page breaks and short jumps are rarely if ever found											
		{if((Flash_flag) && (!(orphan)))							//Burn contents of the partially filled page buffer to flash
		{write_page_SUB(page_address);}														
		orphan = 0;}
		
	start_new_code_block();											//A new code block is always required where there is a break in the hex file.
	short_record=0;}
		
Program_record();}													//Continue filling page_buffer
		

UCSR0B &= (~(1<<RXCIE0));											//download complete, disable UART Rx interrupt
LED_2_off;				
while(1){if (isCharavailable(5)==1)receiveChar();else break;}		//Clear last few characters of hex file
	
if((Flash_flag) && (!(orphan))){write_page_SUB(page_address);}	//Burn final contents of page_buffer to flash
if(orphan) {write_page_SUB(page_address + PageSZ);}cli();
sendString("OK\r\n");}




/*********************************************************************************************************/
void get_text(char Rx_askii_char){

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

TCNT0 = 0;

if(!(txt_counter%2)){												//Each location in array "store" holds two askii characters
store[w_pointer] = Rx_askii_char;
store[w_pointer] = store[w_pointer] << 8;}							//"w_pointer" gives address in array of the next free location
else{store[w_pointer] += Rx_askii_char;
inc_w_pointer;}
txt_counter = (txt_counter + 1);									//"txt_counter" gives the number of characers downloaded
Rx_askii_char_old = Rx_askii_char;} 



/*********************************************************************************************************/
void get_hex(char Rx_askii_char){

unsigned char Rx_Hex_char=0;
int local_pointer;
	
if (Rx_askii_char == ':')counter = 0;										//First char of hex file is ':'
else {if (Rx_askii_char <= '9')
	Rx_Hex_char = Rx_askii_char - '0'; 										//Convert chars '0' to '9' to numbers 0 to 9
	else Rx_Hex_char = Rx_askii_char - '7';}								//and chars 'A' to 'F' to numbers 0xA to 0xF

switch (counter){
case 0x0:  	break;															//Detect -:- at start of new line
case 0x1: 	tempInt1 = Rx_Hex_char<<4;  break;								//Acquire first digit 
case 0x2: 	tempInt1 += Rx_Hex_char;  										//Acquire second digit and combine with first to obtain number of commands in line
			char_count = 9 + ((tempInt1) *2); 								//Calculate line length in terms of individual characters
			local_pointer = w_pointer++; 									//Update pointer to array "store"
			store[local_pointer] = tempInt1; break;							//Save the number of commands in the line to the array  
case 0x3: 	tempInt1 = Rx_Hex_char<<4;  break;								//Next 4 digits give the address of the first command in the line
case 0x4:	tempInt1 += Rx_Hex_char; tempInt1=tempInt1<<8; break;			//Acquire second digit and combine it with first 
case 0x5:	tempInt1 += Rx_Hex_char<<4;  break;							//Continue for third digit
case 0x6: 	tempInt1 += Rx_Hex_char; 										//Acquire final digit and caculate address of next command 
			local_pointer = w_pointer++; 									//Update pointers to array "store"
			store[local_pointer] = tempInt1; break;							//Save address of next command to array "store"
case 0x7: 	break;															//chars 7 and 8 are not used
case 0x8: 	break;
default: 	break;}

if ((counter > 8)&&(counter < char_count)){								//Continue to acquire, decode and store commands
if ((counter & 0x03) == 0x01){tempInt1 = Rx_Hex_char<<4;}					//Note: Final two chars at the end of every line are ignored
if ((counter & 0x03) == 0x02)  {tempInt1 += Rx_Hex_char;}
if ((counter & 0x03) == 0x03)  {tempInt2 = Rx_Hex_char<<4;}
if ((counter & 0x03) == 0x0)  	{tempInt2+= Rx_Hex_char; 
								tempInt2=tempInt2<<8;tempInt1+=tempInt2;
local_pointer = w_pointer++; 	
store[local_pointer] = tempInt1; }}

counter++;
w_pointer = w_pointer & 0x1F;	}  											//Overwrites array after 32 entries




