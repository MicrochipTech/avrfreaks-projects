

/********  ATMEGA programmer runs on ATMEGA168 or 328**********/


/*Terminal port settings 57600 Baud, 8 data bits, no parity, 1 stop bit and no handshaking.

ON_CHIP EEPROM reservation
	for Atmega 168
		0x1F7	cal byte
		0x1F8	cal byte repeated
	for Atmega 328
		0x3FE	cal byte
		0x3FF	cal byte repeated*/


#include "ATMEGA_Programmer.h"


int main (void){ 

char cal_factor;																			//Either default or user supplied	
unsigned int target_type = 0,  target_type_M;												//Device signature bytes			
char  op_code_1,   op_code_2=0;

//setup_168_HW;																				//Run code on Atmega168 or 328
setup_328_HW;																				//see "Resources\ATMEGA_Programmer.h"
	
/*****************Power-up and make contact with target****************************/
while(1){
sendChar('?'); timer_T1_sub(T1_delay_1sec);												//Sends ??? to indicte activity
if (isCharavailable())break;}																//Waits for user respose: Press any key				
receiveChar();

Define_programmining_pins;																	//see "Resources\ATMEGA_Programmer.h"
Atmel_powerup_and_target_detect;   															//Leave target in programming mode 													    


sendString ("\r\n\r\nATMEGA");
switch (target){

case 1681: sendString ("168A"); target = 168; break;
case 1682: sendString ("168PA"); target = 168; break;
case 3281: sendString ("328"); target = 328; break;
case 3282: sendString ("328P"); target = 328; break;
default: wdt_enable(WDTO_1S); while(1);break;}

switch (target){
case 168: PageSZ = 0x40; PAmask = 0x1FC0; FlashSZ=0x2000; EEPROM = 0x200; text_start = 0x5; break;
case 328: PageSZ = 0x40; PAmask = 0x3FC0; FlashSZ=0x4000; EEPROM = 0x400; text_start = 0x5; break;}


sendString(" detected\r\nPress P to program target, R to run target application or -X- to escape");


while(1){
op_code_1 = waitforkeypress();
		
switch (op_code_1){
case 'R': Reset_H; sendString(" -R  \r\nPress ^H to halt target\r\n");						//Allow target to run; press ^H to halt it

	while(1){ while (waitforkeypress() != '^'); 											//Wait here while target runs
	if (waitforkeypress() == 'H') 
	{sendString(" -H\r\nP, R or X?\r\n");Reset_L;break;}}									//Reset target
	break;
case 'X': wdt_enable(WDTO_120MS);while(1);break;
case 'P':
sendString("\r\n8 MHz internal clock.");
break;
default: break;} 

if (op_code_1 == 'P')  {Atmel_powerup; Atmel_config(Prog_enable_h, 0);}
if (op_code_1 == 'P')  break;}		

sendString("\r\nPress -P- to send a program file or -E- to send a text file.");

op_code_2 = waitforkeypress();

switch (op_code_2){
case 'E':   Prog_EEPROM(); break;
case 'P':  break;
default: wdt_enable(WDTO_1S); while(1); break;}


/***Erase target flash and program target config space***/
(Atmel_config(Chip_erase_h, 0)); 		
switch (target){
case 168:
case 328: 																		//Critical parameters for both devices
Atmel_config(write_extended_fuse_bits_h,0xFF );									//BOD dissabled	
Atmel_config(write_fuse_bits_H_h,0xD7);											//WDT under program control, EEPROM preserved during chip erase
Atmel_config(write_fuse_bits_h,0xE2 );											//8MHz internal RC clock, 65ms SUT
Atmel_config(write_lock_bits_h,0xFF );											//No lock bits set
break;
default: wdt_enable(WDTO_1S); while(1);}

sendString("\r\nSend Program file.\r\n");

Program_Flash();
Verify_Flash();  

sendString (Version);

switch (target){
case 168:
case 328:
{sendString("Config bytes: Fuses extended, high, low and lock\r\n");
sendHex(16, Atmel_config(read_extended_fuse_bits_h, 0));
sendHex(16, Atmel_config(read_fuse_bits_H_h,0));	
sendHex(16, Atmel_config(read_fuse_bits_h, 0));
sendHex(16, Atmel_config(read_lock_bits_h, 0));
}break;}

sendString("    on-chip cal bit:  ");

if (cal_factor==1) sendHex(16, OSCCAL);
sendHex(16, OSCCAL);
newline();
sendString("Hex_file_size:	");
sendHex(10,cmd_counter); sendString("  d'loaded:  "); 
sendHex(10,prog_counter); sendString(" in:  "); 
sendHex(10,read_ops); sendString(" out\r\n");//}
wdt_enable(WDTO_60MS); while(1);
return 1;  }







/***************************************************************************************************************************************************/
ISR(USART_RX_vect){
	unsigned char Rx_Hex_char=0;
	unsigned char Rx_askii_char;
	int local_pointer;
		
Rx_askii_char = receiveChar();
switch (Rx_askii_char){
case '0':  Rx_Hex_char = 0x00; break;										//Convert askii chars received from hex file to binary digits
case '1':  Rx_Hex_char = 0x01; break;
case '2':  Rx_Hex_char = 0x02; break;
case '3':  Rx_Hex_char = 0x03; break;
case '4':  Rx_Hex_char = 0x04; break;
case '5':  Rx_Hex_char = 0x05; break;
case '6':  Rx_Hex_char = 0x06; break;
case '7':  Rx_Hex_char = 0x07; break;
case '8':  Rx_Hex_char = 0x08; break;
case '9':  Rx_Hex_char = 0x09; break;
case 'A':  Rx_Hex_char = 0x0A; break;
case 'B':  Rx_Hex_char = 0x0B; break;
case 'C':  Rx_Hex_char = 0x0C; break;
case 'D':  Rx_Hex_char = 0x0D; break;
case 'E':  Rx_Hex_char = 0x0E; break;
case 'F':  Rx_Hex_char = 0x0F; break;
case ':':  counter = 0;  break;
default: break;}

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
w_pointer = w_pointer & 0b00111111;	}  										//Overwrites array after 64 entries





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
LEDs_off;				
while(1){if (isCharavailable()==1)receiveChar();else break;}		//Clear last few characters of hex file
	
if((Flash_flag) && (!(orphan))){write_page_SUB(page_address);}	//Burn final contents of page_buffer to flash
if(orphan) {write_page_SUB(page_address + PageSZ);}}


	

/***************************************************************************************************************************************************/
void Verify_Flash (void){


int  print_line = 0;												//Controls printing of hex file													
int line_no;														//Refers to the .hex file
signed int phys_address;											//Address in flash memory
signed int prog_counter_mem;										//Initialised with size of .hex file used for programming
char print_out_mode = 0;											//Print out flash contents as hex or askii characters
char skip_lines[4];													//Enter number to limit the print out

phys_address = 0;  read_ops=0; 
line_no = 0; prog_counter_mem = prog_counter; 

sendString("Integer(0-FF)?  ");										//0 prints no lines -1-, every line, -8- prints every eighth line etc... 
skip_lines[0] = '0';												//Acquire integer between 0 and FF
skip_lines[1] = waitforkeypress();
skip_lines[2] = '\0';
timer_T1_sub(T1_delay_500mS);	
if (isCharavailable()){skip_lines[0] = skip_lines[1]; 
skip_lines[1] = receiveChar();}	
binUnwantedChars();	
print_line = askiX2_to_hex(skip_lines);
sendHex (16,print_line); sendString("   ");


if (print_line == 0); 												//hex file print out not required
else {sendString("1/2?\r\n");										//else -1- sends file as askii, -2- sends it as hex
print_out_mode =  waitforkeypress(); binUnwantedChars();			
newline();}



while(1){ if(!(prog_counter_mem))break;							//print out loop starts here, exit when finished
		
while(1) {															//Start reading the flash memory searching for the next hex command

Hex_cmd = Read_write_mem('L',phys_address, 0x0);							
Hex_cmd = (Hex_cmd<<8) + (Read_write_mem('H',phys_address, 0x0)); 
phys_address++;        
if (phys_address == FlashSZ)break;									//No more memory? Quit if yes
if (Hex_cmd != 0xFFFF) break;										//If the hex command is 0xFFFF remain in this loop otherwise exit.
LEDs_on;}

LEDs_off;
if (phys_address == FlashSZ)break;									//Exit when there is no more flash to read

if ((print_line == 0)  && (!(line_no%10)))sendChar('*');			//Print out of hex file not required
																	//Print a -*- every tenth line of the file
if(print_line && (!(line_no%print_line)))							//Print out required: Print all lines or just a selection			
{newline(); sendHex (16, (phys_address-1)*2);		
sendString("   "); if(print_out_mode == '1'){send_as_askii;} 		//Start with the address of the first command in the line
else sendHex (16, Hex_cmd);}										//Print first command in askii or hex
read_ops++;															//Value to be sent to PC for comparison with the hex filer size
prog_counter_mem--;													//"prog_counter_mem" decrements to zero when the end of the file is reached


for(int m=0; m<7; m++){    										//Read the next seven locations in the flash memory   

Hex_cmd = Read_write_mem('L',phys_address, 0x0);				
Hex_cmd = (Hex_cmd<<8) + (Read_write_mem('H',phys_address, 0x0)); 
phys_address++; 
if(Hex_cmd == 0xFFFF)break;											//Read 0xFFFF: return to start of print out loop
prog_counter_mem--;

if(phys_address & 0x0040) {LEDs_on;	} else {LEDs_off;} 			       

if(print_line && (!(line_no%print_line)))
{timer_T0_sub(T0_delay_5ms);										//5ms delay prevents PC from getting overwhelmed with characters	
if(print_out_mode == '1'){send_as_askii;} 
else sendHex (16, Hex_cmd);}
read_ops++;

if(phys_address==FlashSZ)break;}

line_no++;
if (phys_address == FlashSZ)break;}

LEDs_off;

newline();newline(); } 






        
/***************************************************************************************************************************************************/
void Prog_EEPROM(void){sendString("\r\nCode to be posted shortly\r\n");	
wdt_enable(WDTO_1S); while(1);}				
