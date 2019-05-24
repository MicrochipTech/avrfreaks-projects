


/********  ATMEGA328 bootloader_V1 runs in the boot space of the ATMEGA328**********/

/*
A development of the In-Circuit programer published by osbornema in "https://community.atmel.com/projects"
on 22 February 2019. The Hard ware routines required to clock data and commands between programmer and target
have been replaced by assembly routines designed to implement the LPM (load program memory) and SPM (store
program memory) commands.  See the Atmega 328 data sheet for prototypes for these subrountines.

Once it has been loaded into the boot section of the Atmega 328 starting at address 0x7000 it can be used to
program the application section via a terminal program such as Tera Term or Br@y++.

The Atmega 328 bootloader runs under configuration bytes:
Extended	0xFD	This sets the BOD settings which are non critical to this application
High		0xD0	This configures the Reset pin, WDT, Boot size and reset vector all of which are critical
Fuse		C2		This configures the clock and SUT.  An 8MHz clock is assumed but its souce is not critical
Lock		EF		To protect the boot space
			EB 		To protect both boot and application regions

Note: Compiling this program results in two warning messages. These occur because of the need to transfer poiners
to the assembly routines which is done in a way that is not standard for applications written exclusively in -C-.

If the size of the hex file is more than 50% that of the application section it may be necessary to split the hex
file into two files of approximately equal size.  This is because of limitations in the terminal emmulator.  Having
initiated the download of the first half wait for the downoad to stop (i.e. the led if implemented stops flashing).
Then send the second file.
*/



/*Compile it using optimisation level s ONLY
Rx/Tx work at 57.6k
*/

/*EEPROM reservations
0x3FF	user cal if set
0x3FE	user cal if set
0x3FD	Default cal supplied by Atmel
*/


#include "Atmega328bootloader_V1.h"



int main (void){ 

char L_Byte, keypress;
int L_Byte_ptr;

setup_HW;

if (!(MCUSR & (1 << EXTRF)) )										//For reset types POR, BOR or watchdog but not the reset switch 
{MCUCR = (1<<IVCE);MCUCR = 0x0;									//select interrupt vector table starting at 0x0000
asm("jmp 0x0000");}													//and opperate the application

cal_device;															//Otherwise initiate the bootloader by checking the calibration status

MCUCR = (1<<IVCE);  												//Select the interrupt vector table starting at start of boot section
MCUCR = (1<<IVSEL);
MCUSR &= (~(1 << EXTRF));  										//Reset the external reset flag



while(1){
while(1){															//User prompt
do{sendString("p/r  ");}while((isCharavailable(300) == 0));
keypress = receiveChar();
if ((keypress == 'p') && (Lock_byte() == 0xEF))break;
if (keypress == 'p')sendString ("\r\nDevice locked.\r\n");
if(keypress == 'r') {wdt_enable(WDTO_60MS); while(1);}}


Clear_flash();    													//Delete the entire application section   
sendString ("\r\nSend file?\r\n");
PORTD |= (1 << PD7);												//Start led activity
PageSZ = 0x40; PAmask = 0x3FC0;										//Define flash memory parameters
FlashSZ=0x4000; AppSZ = 0x3800;


Program_Flash();													//Run programer subroutine
PORTD &= (~(1 << PD7));											//Halt led activity

sendString("0 or AOK");												//Press '0' to read out hex file from flash
print_mode = waitforkeypress() - '0';
Verify_Flash();  


newline();
sendString("Hex_file_size:	");										//Confirm file sizes are all identical
Hex_to_PC(cmd_counter); sendString("  d'loaded:  "); 
Hex_to_PC(prog_counter); sendString(" in:  "); 
Hex_to_PC(read_ops); sendString(" out\r\n");//}


sendString ("Set lock byte Y/N\r\n");								//Offer user the opportunity to protect the device from reprogramming
if (waitforkeypress() == 'Y'){L_Byte = 0xEB;
L_Byte_ptr = &L_Byte;												//Required to pass a pointer to the .S routine (results in a warning message)
loc_in_mem_H = L_Byte_ptr >> 8;
loc_in_mem_L = L_Byte_ptr;
address_in_flash = 1;
Prog_mem_address_H = address_in_flash >> 8;
Prog_mem_address_L = address_in_flash;
set_lock_byte();}													//Subroutine provided in assembly file


sendString("Config bytes:\r\nFuses E, H, L and lock B\r\n");		//Print out device configuration bytes
clear_read_block();	
address_in_flash = 0;
for(int m = 0; m<4; m++){
switch(m){
case 0: address_in_flash = 2; break;								//Print Fuse byte Extended
case 1: address_in_flash = 3; break;								//Print Fuse byte High
case 2: address_in_flash = 0; break;								//Print Fuse byte
case 3: address_in_flash = 1; break;}								//Print lock byte

Prog_mem_address_H = address_in_flash >> 8;
Prog_mem_address_L = address_in_flash;
read_config_bytes();
short_num_to_PC(Flash_readout);sendChar('\t');		
}newline();


}return 1;}




/***************************************************************************************************************************************************/
ISR(USART_RX_vect){
	unsigned char Rx_Hex_char=0;
	unsigned char Rx_askii_char;
	int local_pointer;
	
Rx_askii_char = receiveChar();
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
store[local_pointer] = tempInt1; cmd_counter++;}}

counter++;
w_pointer = w_pointer & 0b00001111;	}  										//Overwrites array after 16 entries





/***************************************************************************************************************************************************/
void Program_Flash (void){

prog_counter=0;  prog_led_control = 0; cmd_counter = 0; record_length_old=0;
Flash_flag = 0;  HW_address = 0;  section_break = 0; orphan = 0; 
w_pointer = 0; r_pointer = 0; short_record=0;


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
		

cli();	
UCSR0B &= (~(1<<RXCIE0));											//download complete, disable UART Rx interrupt
LED_2_off;				
while(1){if (isCharavailable(5)==1)receiveChar();else break;}		//Clear last few characters of hex file
	
if((Flash_flag) && (!(orphan))){write_page_SUB(page_address);}	//Burn final contents of page_buffer to flash
if(orphan) {write_page_SUB(page_address + PageSZ);}}









/***************************************************************************************************************************************************/
void Verify_Flash (void){
int line_no;														//Refers to the .hex file
signed int phys_address;											//Address in flash memory

clear_read_block();													//Subroutine provided in assembly file

phys_address = 0;  read_ops=0; 
line_no = 0;  

while(1){ 		
		
while(1) {															//Start reading the flash memory searching for the next hex command
Hex_cmd = readCMD(phys_address);
phys_address++;        
if (phys_address == AppSZ)break;									//No more memory? Quit if yes
if (Hex_cmd != 0xFFFF) break;}										//If the hex command is 0xFFFF remain in this loop otherwise exit.

if (phys_address == AppSZ)break;									//Exit when there is no more flash to read
		
if(!(print_mode))
{newline(); Hex_to_PC ((phys_address-1)*2);		
sendString("   "); 
Hex_to_PC (Hex_cmd);sendChar(' ');}										//Print first command in askii or hex
read_ops++;															//Value to be sent to PC for comparison with the hex filer size

for(int m=0; m<7; m++){    										//Read the next seven locations in the flash memory   

Hex_cmd = readCMD(phys_address); 
phys_address++; 
if(Hex_cmd == 0xFFFF)break;											//Read 0xFFFF: return to start of print out loop

if(!(print_mode))
{five_msec_delay;	
Hex_to_PC (Hex_cmd);sendChar(' ');}
read_ops++;

if(phys_address==AppSZ)break;}

line_no++;
if((!(line_no%8))&&(!(print_mode)))newline();
if (phys_address == AppSZ)break;}

} 



