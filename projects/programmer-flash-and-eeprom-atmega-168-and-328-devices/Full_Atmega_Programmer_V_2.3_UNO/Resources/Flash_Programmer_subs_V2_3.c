/**************************************************************************************************

Low level subroutines called by subroutine Program_Flash ().  This is called by the main routine


WinAVR generates hex files that must be downloaded to the programmer and sorted into addresses and data
which is then burned into the flash memory.  (Note: The devices use 16 bit commands, but the hex file assumes
8 bit words.)

Flash memory is programed in blocks called pages.  Each page has space for 64 commands (128 bytes).
The Atmega168 has 128 pages of flash and the 328 has 256 pages. As they are downloaded hex commands are saved 
to a page bufer and when this is full its contents are burned to flash.

Programming normally starts at address 0000 the start of the first page. When the page buffer is fully loaded
and the data it contains has been "burned" to flash the programmer starts filling the buffer again and its contents
will be burned to the second page that starts at byte address 0x80.

After programming the final address, command 0x3FFF for the Atmega 328 or  0x1FFF for the Atmega 168 
programming continues at location 0x0000.

The address of a command can be split into that of the page on which it occurs plus its address within the page.
The address of the page is the address of the first command on the page. For any command with address "Hex_address"
the page address is given by "Hex_address & PAmask" and the address on the page is given by  "Hex_address - Page_address".

Hex files consist of a number of lines known as records.  Each record starts with a -:- character.  This is followed by
the number of commands in the record, then the address of the first one and then the commands themselves.
Note: Hex files and their records are generated using bytes.  However each command consists of two bytes.     

Hex files do not usually fit into an exact number of pages.  There can also be several code sections, for example 
one starting at location zero and one at the start of the boot partition.

Several variables are therefore required to keep programming on track:

Hex_address:		An address obtained from the hex file; usually the adress of the first command in a line of 8
HW_address:			This address will track the Hex_Address provided that there are no gaps in the hex file.

Line_length:		This is the number of commands present in a single line of the hex file.
Line_length_old:	The program needs to know the line length of the curent line and the previous one.	

Record:				The hex file is processed by the programmer one line at a time.  Each line is also known as a record.

section_break:		This is set to 1 when at least one page of flash memory will be unused.
page_break:			This is set to 1 when a page is only partialy filled before programming of the next page starts
orphan:				This is set to 1 when the commands in a record are shared between one page and the next



The array store[]:	5mS is allowed for burning the contents of the page buffer to flash.  During this time data continues to
be received by the UART.  The UART settings are 1 start bit, 8 data bits and 1 stop bit (i.e. 10 bits per askii char).
The baud rate is 57.6Kb which equates to 5.76 askii chars per mS.  
Therefore during the 5mS for which "burning" takes place 29 askii characters are recieved. 
32 askii chars equates to the hex commands in one line of a hex file which holds 8 16 bit commands

The array store[] must therefore hold at least 8 integers.
In the past it has usually been assigned 32 integers and this practice is continued here.





**********************************************************************************************************************/



#define inc_r_pointer \
r_pointer++;\
r_pointer = r_pointer & 0b00011111;


void waitforchar(void);
void new_record(void);
void start_new_code_block(void);
void Program_record(void);
void copy_cmd_to_page_buffer(void);
void get_next_hex_cmd(void);
void write_page_SUB(int);

unsigned char Read_write_mem(char, int, char);
void Load_page(char, int, unsigned char);


/**********************************************************************************************************/
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

if(prog_led_control & 0b00001000) {LEDs_on;}	
else {LEDs_off;}}  



/**********************************************************************************************************/
void start_new_code_block(void){
HW_address = Hex_address;												//Initialise HW_address
page_address = (Hex_address & PAmask);									//Obtain page address
write_address = Hex_address - page_address;								//Initialise write_address: zero to Page size -1
page_offset = write_address;											//"page_offset" is the initial value of "write_address"											
line_offset = Hex_address & 0x0007;										//Not all lines start with addresses that are multiples of 8
space_on_page = (PageSZ - page_offset);}								//Initialise "space_on_page"






/**********************************************************************************************************/
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
Read_write_mem('W', page_address, 0x0);									//Burn contents of page buffer to flash
Flash_flag = 0;															//Buffer now contains no data to burn to flash
write_address = 0;														//"while loop" continues if there is a line offset
space_on_page = PageSZ;
page_offset = line_offset;
if (line_offset) orphan = 1;}}}										//One or more commands in current record will be on next page


/**********************************************************************************************************/
void copy_cmd_to_page_buffer(void){
get_next_hex_cmd();
Load_page('L', write_address, Hex_cmd & 0x00FF); 
Load_page('H', write_address, (Hex_cmd >> 8));}


/**********************************************************************************************************/
void get_next_hex_cmd(void){int Local_r_pointer;
while (r_pointer == w_pointer);
Local_r_pointer = r_pointer;
Hex_cmd = store[Local_r_pointer];
inc_r_pointer;}



/**********************************************************************************************************/
void write_page_SUB(signed int page_address){
Read_write_mem('W',  page_address, 0x0);
Flash_flag = 0;}





