
unsigned int read_flash(int);


/***************************************************************************************************************************************************/
void Verify_Flash_Hex (void){


int  line_counter = 0, print_line = 0;								//Controls printing of hex file                         
int line_no;														//Refers to the .hex file
signed int phys_address;											//Address in flash memory
signed int prog_counter_mem;										//Initialise with size of .hex file used for programming
unsigned char print_out_mode = 0;									//Print out flash contents as hex or askii characters
char skip_lines[4];													//Enter number to limit the print out
int FFFF_counter;

sendString("\r\nVerifying flash: AK to continue");
waitforkeypress();

if(prog_counter == 0)prog_counter = 0x4000;

sendString("\r\nPress 0 or AOK to escape\r\n");
if(waitforkeypress() != '0')return;

phys_address = flash_start;  read_ops=0; 
line_no = 0; prog_counter_mem = prog_counter; 

sendString("Integer(0-FF)?  ");										//0 prints no lines -1-, every line, -8- prints every eighth line etc... 
skip_lines[0] = '0';												//Acquire integer between 0 and FF
skip_lines[1] = waitforkeypress();
skip_lines[2] = '\0';
Timer_T1_sub(T1_delay_500ms); 
if (isCharavailable(1)){skip_lines[0] = skip_lines[1]; 
skip_lines[1] = receiveChar();} 
binUnwantedChars(); 
print_line = askiX2_to_hex(skip_lines);
sendHex (16,print_line); sendString("   ");



while(1){ if(!(prog_counter_mem))break;							//print out loop starts here, exit when finished

FFFF_counter = 0;  
while(1) {															//Start reading the flash memory searching for the next hex command
Hex_cmd = read_flash(phys_address);
phys_address += 2;        
if (phys_address == FlashSZ)break;									//No more memory? Quit if yes
if (Hex_cmd != 0xFFFF) break;										//If the hex command is 0xFFFF remain in this loop otherwise exit.
else FFFF_counter += 1;
if (FFFF_counter >= 64)break;
}

if (FFFF_counter >= 64)break;
if (phys_address == FlashSZ)break;									//Exit when there is no more flash to read

if ((print_line == 0)  && (!(line_no%5)))
sendChar('*');														//Print out of hex file not required
																	//Print a -*- every tenth line of the file
if(print_line && (!(line_no%print_line)))							//Print out required: Print all lines or just a selection     
{sendString("\r"); 
sendHex (16, ((phys_address-2)-flash_start));   
sendString(" "); line_counter++;  

sendHex (16, Hex_cmd);}												//Print first command in askii or hex
read_ops += 1;														//Value to be sent to PC for comparison with the hex filer size
prog_counter_mem -= 1;												//"prog_counter_mem" decrements to zero when the end of the file is reached


for(int m=0; m<7; m++){Timer_T2_sub(T2_delay_5ms);					//Read the next seven locations in the flash memory   

Hex_cmd = read_flash(phys_address); 
phys_address += 2; 
if(Hex_cmd == 0xFFFF)break;											//Read 0xFFFF: return to start of print out loop
prog_counter_mem -= 1;


if(print_line && (!(line_no%print_line)))

sendHex (16, Hex_cmd);

read_ops += 1;

if(phys_address==FlashSZ)break;}
if ((print_line)&&(!(line_no%print_line)) && (!(line_counter%8)))sendString("\r\n");
line_no++;
if (phys_address == FlashSZ)break;}


newline(); 

sendString("\r\nHex_file_size:  ");
sendHex(10,prog_counter); sendString(" in:  "); 
sendHex(10,read_ops); sendString(" out\r\n");

}





/********************************************************************************************************/
unsigned int read_flash(int flash_add){								//Read integer from flash
  unsigned int Hexcmd;

synch_pulse;
data_byte_Tx = (LDS_from | word_address); transmit_byte;
data_byte_Tx = (flash_add); transmit_byte;
data_byte_Tx = (flash_add >> 8); transmit_byte;
receive_byte; Stop_clock;
Hexcmd = data_byte_Rx;
Hexcmd = Hexcmd << 8;

flash_add += 1;														//Next address in flash
 
synch_pulse;
data_byte_Tx = (LDS_from | word_address); transmit_byte;
data_byte_Tx = (flash_add); transmit_byte;
data_byte_Tx = (flash_add >> 8); transmit_byte;
receive_byte; Stop_clock;
Hexcmd |=  data_byte_Rx;

return Hexcmd;
}