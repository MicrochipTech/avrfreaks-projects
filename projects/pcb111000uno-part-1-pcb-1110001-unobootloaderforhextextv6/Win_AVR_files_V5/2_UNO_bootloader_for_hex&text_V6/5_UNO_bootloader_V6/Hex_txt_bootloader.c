

/*
This "Hex_text programmer" runs on the UNO 328 device in place of the standard Arduino bootloader
It is designed to work alongside the pcb-bootloader that runs on pcb_a the plug in version of PCB 111000.
It programs both hex and text files to flash and has two modes, one simply programs, the other also echos
the files which is particularly useful when developing text files. 

The "Hex_text programmer" is loaded in the bootloader partition at 	0x7000 using the PCB_A bootloader
It works alongside a default user applications loaded at adddresses	0x5E60 and 0x5C90
A hex file verification routine loaded at address					0x6200
A text file verification routine loaded at address					0x6C70
Control is always passed back to the "Hex_text programmer" using a WDTout and EEP loaction 0x3FC.
Other WDTouts required to lauch the user app at address 0x0000 can therefore be identified.


Reset control is managed using two EEPROM locaions, 0x3FC and 0x3F7.
0x3FC contains the value of MCUSR relevent to the user application
POR and BOR resets cause an immediate jump to the user application.
MCUSR is copied directly to 0x3FC
WDTouts are used by both programmer and user applications.
They are used to exit the text and hex verification routines.  
These WDTouts are identified by writing 1 to bit 7 of 0x3FC.
WDTouts are also used when mode r (run) is selectd at the h/t/r/D user prompt
or when the PCB_A reset switch is pressed.
They are identified by setting 0x3F7 to zero as a result of which 0x3FC is set to 1 << EXTRST.
For user app generated WDTouts MCUSR is copied directly to 0x3FC and program control jumps straight to location zero.



PCB_A reset signalling line:	The pcb_a reset switch signals lines PD5 or PD6 (for CA/CC displays) 
								and also puts the UNO device into reset for several mS.

								Depending on the line state program control either jumps to the user app or 
								the programmer user prompt. 
								(Both lines are connected to simplify pcb development.)
						
								A single click of pcb_a reset switch returns control to the user app
								A double click returns controll to the hex/text programmer and also enables 
								the pcb bootloader to run so that the UNO can be reprogrammed.
						
Note: 	Assumes PCB_A is loaded with Common Cathode displays for which the signalling line (PB2) defaults to high.


This version runs on the UNO device from a 16MHz low power crystal. Config bytes are as follows:

Extended	0x05	2.7V BOD as also set by Arduino
High		0xD0	This configures the WDT, Boot size and reset pin/vector and ensures that the EEPROM is preserved during chip erase
Fuse		FF		LP crystal with 65mS SUT as also set by Arduino	
Lock		EF		Hex_text_programmer cannot program the boot space (i.e. overwrite itself in any way)

Because all programmer algorithms have been developed to run at 8MHz, the system clock prescaler is used to convert the 16MHz crystal
frequency to an 8MHz clock. 

Compile it using optimisation level s ONLY
Note the normal warning messages when C and assembly files are combined (i.e.integer from pointer without a cast).
Rx/Tx work at 57.6k with no handskaking, no parity, 8 data bits and 1 stop bit

EEPROM reservations: 
0x3FF	user cal if set	No longer used
0x3FE	user cal if set No longer used
0x3FD	Default cal supplied by Atmel No longer used
0x3FC	Copy of MCUSR and also indicates WDTout hex/text verification routines 
0x3FB/A	prog_counter for use by verifiication routines
0x3F9/8	cmd_counter for use by verifiication routines
0x3F7	Reset Control (set to zero to indicate that user app is to be launched.)
0x3F6	Reserved for use by Pseudo Random Noise generator
0x3F5	Second PRN reservation
0x3F4	flash text string pointer
0x3F2	PRN generator random number HB
0x3F1	PRN generator random number LB
*/


/********************************************************************************
SW_Version: UNO_bootloader_for_hex&text_V6
SW information:
  Compatible with 
	AVR and UNO user projects,
	PCB_A Mini-OS: I2C_V18_CC & CA
  Programmed using PCB_A_Bootloader V5_CC of CA or USBasp programmer
********************************************************************************/



#include "Hex_txt_bootloader.h"
#define Version "Hex_txt_bootloader_V1 "



char mode;													//'h' for hex file, 't' for text file

int main (void){ 											//Loaded at address 0x7000, the start of the bootloader section
char MCUSR_copy;


CLKPR = (1 << CLKPCE);										//Prescaler control: convert 16MHZ crystal to 8MHz clock
CLKPR = (1 << CLKPS0);

MCUSR_copy = MCUSR;											//Save MCUSR in MCUSR_copy
if (eeprom_read_byte((uint8_t*)0x3FC) & 0x80)				//Verification routines set bit 7 of EEPROM 0x3FC
MCUSR_copy |= 0x80;											//Make bit 7 of MCUSR_copy the same as bit 7 of EEPROM 0x3FC



if(!(eeprom_read_byte((uint8_t*)0x3F7)))
{eeprom_write_byte((uint8_t*)0x3FC, (1 << EXTRF));			//Save MCUSR in EEPROM 0x3FC for use by App (clears bit 7).
eeprom_write_byte((uint8_t*)0x3F7, 0xFF);}
else eeprom_write_byte((uint8_t*)0x3FC, MCUSR);


MCUSR = 0;													//Clear all reset flags


switch(MCUSR_copy){
case 2:														//Ext reset: Set by PCB_A programmer or single/double click of PC_A reset switch
case 0x88: break;											//WDTout + return from Hex/Text verification
default: 													//POR, WDTout, BOR
		Prog_mem_address_H = 0;
		Prog_mem_address_L = 0;
		read_flash ();
		if (Flash_readout == 0xFF)asm("jmp 0x5DE0");
		else asm("jmp 0x0000");break;}						//WDT due to user program, POR or BOR,mode r or D which also generate a WDTout
	
setup_HW;													//Initialises all IO to week pull up; UART introduces 5mS delay

MCUCR = (1<<IVCE);  										//Select the interrupt vector table at the start of boot section
MCUCR = (1<<IVSEL);


//For CA displays PIND6 is controlled using PCB_A PB1 which drives a segment on the display and defaults to high when the display is cleared
//For CC displays PIND6 is controlled using PCB_A PB2 which drives a digit on the display and defaults to high when the display is cleared
//A link on PCB_A is used to differentiate between CC and CA versions of PCB_A.


if (!(PIND & (1 << PIND6)))								//Reset control pin.  If set low by PCB_A control passes to the user_app
{eeprom_write_byte((uint8_t*)0x3F7,0);
wdt_enable(WDTO_30MS); while(1);}							  

while(1){													//Returns here following programming with/without verification or 
do{sendString("h/t/r/D      ");}								//double click of PCB_A reset switch
while((isCharavailable(255) == 0));                        //User prompt

switch (receiveChar()){ 
	
case 'r':	Prog_mem_address_H = 0;
			Prog_mem_address_L = 0;
			read_flash ();
			if (Flash_readout == 0xFF)asm("jmp 0x5DE0");	//Detect the absense of an User App and run default app.
			eeprom_write_byte((uint8_t*)0x3F7,0);			//Indicates user program is being launched using a WDTout
			wdt_enable(WDTO_15MS); 							//Run the user application (WDTout triggers jump to 0x0000)
			while(1); 

case 't': 	mode = 't'; text_programmer();
case 'h':	mode = 'h';hex_programmer();					//Hex file download with optional verification
			asm("jmp 0x61B0");	

case 'T': 	mode = 't'; text_programmer(); 					//Text file download with verification
			asm("jmp 0x6C00");								

case 'D':	Prog_mem_address_H = 0;							//Erase start of user app and trigger default app.
			Prog_mem_address_L = 0;
			Page_erase ();							
			wdt_enable(WDTO_15MS);while(1);

default: 	break;}}

return 1;}								                    





/***************************************************************************************************************************************************/
ISR(USART_RX_vect){
	
unsigned char Rx_askii_char;
	
Rx_askii_char = receiveChar();

if (mode == 't') get_text(Rx_askii_char);
if (mode == 'h') get_hex(Rx_askii_char);}
 



/***************************************************************************************************************************************************/
ISR(TIMER0_OVF_vect) {												//Timer0 times out and halts at the end of the text file download
if(text_started == 3)												//Ignore timeouts occurring before start of file download
	{endoftext -= 1; TCCR0B = 0;TIMSK0 &= (~(1 << TOIE0));		//Shut timer down
	inc_w_pointer; store[w_pointer] = 0;							//Append two '\0' chars to the end of the text
	inc_w_pointer; store[w_pointer] = 0; }}




/***************************************************************************************************************************************************/
void text_programmer (void){


w_pointer = 0; r_pointer = 0;										//Initialise variables
text_started =0; endoftext =3;txt_counter = 0;
Rx_askii_char_old = '0';

sendString("\r\nText_F?");
//sendString("T_F?");


Timer_T0_sub_with_interrupt(5,0);									//Start Timer0 with interrupt
UCSR0B |= (1<<RXCIE0); 											//Activate UART interrupt
sei();																//Set global interrupt

address_in_flash = 0x5BFE;//0x5C7E;											//First character will be storred at 0x5BFF not 0x5BFE

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

if (!((0x5BFE - address_in_flash)%128)){							//If page buffer is full
address_in_flash += 2;												//Get the address of the first entry in the page
Prog_mem_address_H = address_in_flash >> 8;							//Prepare the address for the assembly routines
Prog_mem_address_L = address_in_flash;

Page_erase ();														//Assembly routine
page_write();														//Assembly routine
address_in_flash -=2;}												//Restore address_in_flash
if(!(endoftext)) break;}											//Break when two '\0' chars have been appended to text stored in the array

if((0x5BFE - address_in_flash)%128){/////////						//Write remaining chars in partialy full page buffer
address_in_flash += (0x5BFE - address_in_flash)%128 - 126;			//Get address of first character in the page

Prog_mem_address_H = address_in_flash >> 8;
Prog_mem_address_L = address_in_flash;

Page_erase ();
page_write();
}UCSR0B &= (~(1<<RXCIE0));cli();
clear_read_block();													//Subroutine provided in assembly file  (Not required for mode 't'??)
eeprom_write_byte((uint8_t*)0x3F4,0x40);							//Reset string pointer
}
/*********************************************************************************************************/
void hex_programmer(void){


PageSZ = 0x40; PAmask = 0x3FC0;										//Define flash memory parameters

prog_led_control = 0;  record_length_old=0; prog_counter = 0;		//Initialise variables
Flash_flag = 0;  HW_address = 0;  section_break = 0; orphan = 0; 
w_pointer = 0; r_pointer = 0; short_record=0;  cmd_counter = 0;

sendString("\r\nHex_F?");
//sendString("H_F?");

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
		

UCSR0B &= (~(1<<RXCIE0));	cli();									//download complete, disable UART Rx interrupt
LED_2_off;				
while(1){if (isCharavailable(5)==1)receiveChar();else break;}		//Clear last few characters of hex file
	
if((Flash_flag) && (!(orphan))){write_page_SUB(page_address);}	//Burn final contents of page_buffer to flash
if(orphan) {write_page_SUB(page_address + PageSZ);}cli();


clear_read_block();													//Subroutine provided in assembly file

eeprom_write_byte((uint8_t*)0x3FB, prog_counter >> 8);				//Save "prog_counter"		Commands counted by the programmer
eeprom_write_byte((uint8_t*)0x3FA, prog_counter);	

eeprom_write_byte((uint8_t*)0x3F9, cmd_counter >> 8);				//Save "cmd_counter"		Commands counted by the ISR
eeprom_write_byte((uint8_t*)0x3F8, cmd_counter);}




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
store[local_pointer] = tempInt1; cmd_counter++;}}

counter++;
w_pointer = w_pointer & 0x1F;	}  											//Overwrites array after 32 entries




