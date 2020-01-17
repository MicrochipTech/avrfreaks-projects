
/*
This program is used to verify that a hex file has been successully loaded into flash.
It is loaded onto the UNO device at address 0x6200 and works alongside the "Hex_text_programmer".
It is accessed by the "Hex_text_programmer" using an assembly jump command and a WDTout returns 
controll the the "Hex_text_programmer"
The EEPROM is used to share variables between the two programs

Compile it using optimisation level s ONLY
Rx/Tx work at 57.6k
*/


#include "Hex_verification.h"



int main (void){ 

eeprom_write_byte((uint8_t*)0x3FC,	(eeprom_read_byte((uint8_t*)0x3FC) | 0x80));

prog_counter = ((eeprom_read_byte((uint8_t*)0x3FB)) << 8) +\
eeprom_read_byte((uint8_t*)0x3FA);									//EEROM used to pass values from bootloader
cmd_counter = ((eeprom_read_byte((uint8_t*)0x3F9)) << 8) +\
eeprom_read_byte((uint8_t*)0x3F8);

sendString("\t0 to verify or AOK\r\n");								//Press '0' to read out hex file from flash
if (!(waitforkeypress() - '0'))
{Verify_Flash();  
newline();
sendString("Hex_file_size:	0x");									//Confirm file sizes are all identical
Hex_to_PC(cmd_counter); sendString("  d'loaded:  0x"); 
Hex_to_PC(prog_counter); sendString(" in:  0x"); 
Hex_to_PC(read_ops); sendString(" out\r\n");}

sendString("UNO Fuses E, H, L and lock:\t");						//Print out device configuration bytes	
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
short_num_to_PC(Flash_readout);	sendChar('\t');
}newline(); newline();


wdt_enable(WDTO_15MS);
while(1);
return 1;}



/***************************************************************************************************************************************************/

void Verify_Flash (void){

int  line_counter = 0, print_line = 0;								//Controls printing of hex file													
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
Timer_T1_sub(T1_delay_500mS);	
if (isCharavailable(1)){skip_lines[0] = skip_lines[1]; 
skip_lines[1] = receiveChar();}	
binUnwantedChars();	
print_line = askiX2_to_hex(skip_lines);
Hex_to_PC (print_line); sendString("   ");


if (print_line == 0); 												//hex file print out not required
else {sendString("1/2?\r\n");										//else -1- sends file as askii, -2- sends it as hex
print_out_mode =  waitforkeypress(); binUnwantedChars();			
}



while(1){if(!(prog_counter_mem))break;								//print out loop starts here, exit when finished
		
while(1) {															//Start reading the flash memory searching for the next hex command

Hex_cmd = readCMD(phys_address);

phys_address++;        
if (phys_address == FlashSZ)break;									//No more memory? Quit if yes
if (Hex_cmd != 0xFFFF) break;										//If the hex command is 0xFFFF remain in this loop otherwise exit.
LED_1_on;}

LED_1_off;
if (phys_address == FlashSZ)break;									//Exit when there is no more flash to read

if ((print_line == 0)  && (!(line_no%10)))						//Print out of hex file not required
{sendChar('*');Timer_T0_sub(T0_delay_20ms);}																	//Print a -*- every tenth line of the file
if(print_line && (!(line_no%print_line)))							//Print out required: Print all lines or just a selection			
{newline(); Hex_to_PC ((phys_address-1)*2);		
sendString("   "); line_counter++;
if(print_out_mode == '1'){send_as_askii;} 							//Start with the address of the first command in the line
else Hex_to_PC (Hex_cmd);}											//Print first command in askii or hex
read_ops++;															//Value to be sent to PC for comparison with the hex filer size
prog_counter_mem--;													//"prog_counter_mem" decrements to zero when the end of the file is reached


for(int m=0; m<7; m++){    										//Read the next seven locations in the flash memory   

Hex_cmd = readCMD(phys_address);


phys_address++; 
if(Hex_cmd == 0xFFFF)break;											//Read 0xFFFF: return to start of print out loop
prog_counter_mem--;

if(phys_address & 0x0040) {LED_1_on;	} else {LED_1_off;} 			       

if(print_line && (!(line_no%print_line)))
{Timer_T0_sub(T0_delay_5ms);										//5ms delay prevents PC from getting overwhelmed with characters	
if(print_out_mode == '1'){send_as_askii;} 
else Hex_to_PC (Hex_cmd);}
read_ops++;

if(phys_address==FlashSZ)break;}
if ((print_line != 0) && (!(line_no%print_line)) && (!(line_counter%8)))sendString("\r\n");

line_no++;
if (phys_address == FlashSZ)break;}

LED_1_off;

newline(); } 
