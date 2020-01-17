
/**************************************************************************************************

Low level subroutines called by subroutine Program_Flash () which is called by the main routine


WinAVR generates hex files that must be downloaded to the programmer and sorted into addresses and data
which is then burned into the flash memory.  (Note: The devices use 16 bit commands, but the hex file assumes
8 bit words.)

Flash memory is programed in blockes called pages.  There are 256 pages and each one has space for 64 commands (128 bytes).
As they are downloaded hex commands are saved to a page bufer and when this is full its contents are burned to flash.

Programming normally starts at address 0000 the start of the first page. When the page buffer is fully loaded
and the data it contains has been "burned" to flash the programmer starts filling the buffer again and its contents
will be burned to the second page that starts at byte address 0x80.


The address of a command can be split into that of the page on which it occurs plus its address within the page.
The address of the page is the address of the first command on the page. For any command with address "Hex_address"
the page address is given by "Hex_address & PAmask" and the address on the page is given by  "Hex_address - Page_address".

Hex files consist of a number of lines known as records.  Each record starts with a -:- character.  This is followed by
the number of commands in the record, then the address of the first one and then the commands themselves.
Note: Hex files and their records are generated using bytes.  However each command consists of two bytes.     

Hex files do not necessarily fit into an exact number of pages.  There may also in some circumstances be several code sections.

Several variables are therefore required to keep programming on track:

Hex_address:		An address obtained from the hex file; usually the address of the first command in a line of 8
HW_address:			This address will track the Hex_Address provided that there are no gaps in the hex file.

Line_length:		This is the number of commands present in a single line of the hex file.
Line_length_old:	The program needs to know the line length of the curent line and the previous one.	

Record:				The hex file is processed by the programmer one line at a time.  Each line is also known as a record.

section_break:		This is set to 1 when at least one page of flash memory will be unused.
page_break:			This is set to 1 when a page is only partialy filled before programming of the next page starts
orphan:				This is set to 1 when the commands in a record are shared between one page and the next
**********************************************************************************************************************/





/***********************************************************************************************/
void timer_T0_sub(char Counter_speed, unsigned char Start_point){ 
TCNT0 = Start_point;
TCCR0B = Counter_speed;
while(!(TIFR0 & (1<<TOV0)));
TIFR0 |= (1<<TOV0); TCCR0B = 0;}



/**********************************************************************************************/
void Timer_T0_sub_with_interrupt(char Counter_speed, unsigned char Start_point){ 
TIMSK0 |= (1 << TOIE0);
TCNT0 = Start_point;
TCCR0B = Counter_speed;}



/**********************************************************************************************/
void new_record(void){int Local_r_pointer;								//Start of new record from the hex file
while (r_pointer == w_pointer);											//Wait for new entry in array "store"
Local_r_pointer = r_pointer;
record_length =  store[Local_r_pointer]; 								//Obtain the length of the new record
inc_r_pointer;  														//Increment the read pointer
Count_down = record_length/2; 											//Initialised to record lenghth (in commands) 
if (record_length_old < 0x10)short_record=1;							//Check for "record length" (<8 commands)
record_length_old = record_length;										//Save record length

while (r_pointer == w_pointer);											//Wait for next entry in array "store"
Local_r_pointer = r_pointer;
Hex_address  =  (store[Local_r_pointer]); 								//Get the address of the first command in the new record
inc_r_pointer;  														//Increment the read pointer
Hex_address  = Hex_address/2;											//Convert address from byte to word value
prog_led_control++;

if (Hex_address == HW_address)orphan = 0;								//New record follows on immediately from the old
else{if (Hex_address == 0);											//Start of hex file: address is zero
		else{if ((Hex_address & PAmask) > (page_address + PageSZ))		//Long jump: At least one page is unused
				{section_break=1;page_break=0;}
			if((Hex_address & PAmask) == (page_address + PageSZ))		//Page jump: Jump to next page without filling current one
				{section_break=0;page_break=1; }
			if ((Hex_address & PAmask) == page_address)				//Short jump: Jump within page
				{section_break=0;page_break=0; orphan = 0;}}}

if(prog_led_control & 0b00001000) {LED_2_on;}	
else {LED_2_off;}}  

 


/***********************************************************************************************/
void start_new_code_block(void){
HW_address = Hex_address;												//Initialise HW_address
page_address = (Hex_address & PAmask);									//Obtain page address
write_address = Hex_address - page_address;								//Initialise write_address: zero to Page size -1
page_offset = write_address;											//"page_offset" is the initial value of "write_address"											
line_offset = Hex_address & 0x0007;										//Not all lines start with addresses that are multiples of 8
space_on_page = (PageSZ - page_offset);}								//Initialise "space_on_page"




/***********************************************************************************************/
void Program_record(void){
while(Count_down){ 														//Initially contains the number of commands in the line
Count_down--;															//Decrement "count_down" each time a command is written to the page buffer
Flash_flag = 1;  														//Indicates that page buffer containes commands that will need burning to flash 
copy_cmd_to_page_buffer();  												 
write_address++;
HW_address++;
space_on_page--;
prog_counter++;
if (write_address == PageSZ){											//If page_buffrer is now full:
write_page_SUB(page_address);											//Burn contents of page buffer to flash
Flash_flag = 0;															//Buffer now contains no data to burn to flash
write_address = 0;														//"while loop" continues if there is a line offset
space_on_page = PageSZ;
page_offset = line_offset;
if (line_offset) orphan = 1;}}}										//One or more commands in current record will be on next page





/***********************************************************************************************/ 
void copy_cmd_to_page_buffer(void){
int ptr_to_cmd;
ptr_to_cmd = &Hex_cmd;													//results is a warning message (ptr_to_cmd is not defined as a pointer)
get_next_hex_cmd();

Prog_mem_address_H = (write_address*2) >> 8;
Prog_mem_address_L = (write_address*2);

loc_in_mem_H = ptr_to_cmd >> 8;
loc_in_mem_L = ptr_to_cmd;
write_to_page_buffer();}												//Subroutine provided in assembly file




/***********************************************************************************************/
void get_next_hex_cmd(void){int Local_r_pointer;
while (r_pointer == w_pointer);
Local_r_pointer = r_pointer;
Hex_cmd = store[Local_r_pointer];
inc_r_pointer;}



/***********************************************************************************************/ 
void write_page_SUB(signed int address_in_flash){						
Prog_mem_address_H = (address_in_flash*2) >> 8;
Prog_mem_address_L = address_in_flash*2;
Page_erase ();
page_write();															//Subroutine provided in assembly file
Flash_flag = 0;}

