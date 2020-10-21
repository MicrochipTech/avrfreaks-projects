

/*
Because of the time required to burn data to EEPROM a file cannot simply be downloaded at 57.6kB
The speed can be reduced considerably or the file can be downloaded several times.
It was felt that the latter approach generated less hassle for the user and has been adopted here.
Each time the file is downloaded in full but only 256 chars or less are stored to an array in SRAM.
When this array is full its contents are copied to EEPROM.

The text/data file contains several sections.  Explanatory text if required appears first.
Strings are then placed between a pair of -"- characters.
After this data can be apended and all text such as data names for example is ignored.

The following variables are used to keep a track of this process:


"EEPROM_buffer"	Array into which file text and data is downloaded before being saved to EEPROM
"EEP_pointer"	and 	"*ptr_EEP_pointer"		Address of next free location in the EEPROM
"file_pointer"	and		"*ptr_file_pointer"		Address in text/data file being downloaded from PC
"array_pointer	and		"*ptr_array_pointer"	Address of next free location in array (temp storrage for file)
"data_counter"	and		"*ptr_data_counter"		Counts data items as they are downloaded	
"DL_flag"		and		"*ptr_DL_flag"			Status of download
"DL_status"		and		"ptr_DL_status"			Status of download

"reservation"	Used to enter the amount of EEPROM required by the application which should not be used for text/data
"op_code_1"		Read, write or delete
"op_code_2"		Specifies format used for reading back numbers from the EEPROM


Note the following bug:		It appears that text strings are always downloaded consistently.
							If numeric data is also stored there seem to be up to 3 consecutive addresses for the first byte.
							The middle address appears to be used most of the time
							But every now and then one of the other adresses is used.
							Obviously this recks the operation of the client program used to read the EEPROM unless it is noticed
							and an appropriate offset of plus or minus 1 added to the data addresses.

*/



void Prog_EEPROM(void);
char Download_text(int *,int *,char *,int *,  unsigned char *);
char Download_data(int *,int *,char *,int *,int *, unsigned char*);
int decimal_conversion (char, int*);
int Hex_conversion (char, int*);
void Upload_text(int);
void Upload_data(int, int);
void Upload_data_1(int, int);
void Upload_data_2(int, int);
char next_char_from_PC(void);


int RBL = 399;	
unsigned char string_counter = 1;



/********************************************************************************************************************************************/
void Prog_EEPROM(void){	
unsigned char  EEPROM_buffer[400];		
int EEP_pointer = 0,   file_pointer = 0,  array_pointer = 0,  data_counter = 0;
char key_press, DL_flag = 0, DL_status, op_code_1, op_code_2;	    
char reservation[4];
int App_reservation;

EEP_pointer = text_start;												//Start saving user strings/data at address 0x05

sendString\
("\r\nPress -w- or -r- to write to or read from EEPROM");




while(1){sendString("?  ");	
if (isCharavailable(255)) break;} 										//User prompt  
op_code_1 = receiveChar();								



/********************************************************************************************************************************************/
switch (op_code_1){														//Read the contents of the EEPROM
case 'R':
case 'r':
newline();

if(((Read_write_mem('O', 0x0, 0)) ==0xFF)  &&\
((Read_write_mem('O', 0x1, 0)) ==0xFF) &&\
((Read_write_mem('O', 0x2, 0)) ==0xFF))
{sendString("No Data!"); break;}										//check for unprogrammed EEPROM

sendString("1,2 or 3\r\n");

op_code_2 =  waitforkeypress();		binUnwantedChars();		
EE_top = (((Read_write_mem('O', 0x3, 0))<<8 ) +\
(Read_write_mem('O', 0x4, 0)));											//Determine bottom of application reservation
Upload_text((Read_write_mem('O', 0x0, 0x0) << 8)\
+ Read_write_mem('O', 0x1, 0x0));										//Read text strings from EEPROM	

switch (op_code_2){
case '1':																//Display numeric data in decimal notation
		Upload_data_1 (((Read_write_mem('O', 0x0, 0x0) << 8)\
		+ Read_write_mem('O', 0x1, 0x0)),\
		Read_write_mem('O', 0x2, 0x0)); break;
case '2':																//Display numeric data in hex notation	
		Upload_data_2 (((Read_write_mem('O', 0x0, 0x0) << 8)\
		+ Read_write_mem('O', 0x1, 0x0)),\
		Read_write_mem('O', 0x2, 0x0)); break;	
default:																//Display numeric data in decimal and hex notations
		Upload_data (((Read_write_mem('O', 0x0, 0x0) << 8) +\
		Read_write_mem('O', 0x1, 0x0)),\
		Read_write_mem('O', 0x2, 0x0)); break;}

if ((((Read_write_mem('O', 0x3, 0)) <<8 ) +\
(Read_write_mem('O', 0x4, 0)))  == 0xFFFF); 							//No space reserved for application: End of the read case
else																	//Print out start address of the Space reserved for the application
{if((((Read_write_mem('O', 0x3, 0)) <<8 ) +\
(Read_write_mem('O', 0x4, 0))) < 0x200)
{sendString("\r\nApp: Start addr's  ");
sendHex(16,(((Read_write_mem('O', 0x3, 0)) <<8 ) +\
(Read_write_mem('O', 0x4, 0))));

if ((((Read_write_mem('O', 0x3, 0)) <<8 ) +\
(Read_write_mem('O', 0x4, 0))) == EE_size - 3)sendString("No space reserved.");
newline();}}															//Note: as a result of the cal bytes some space is always reserved 
break;		


/********************************************************************************************************************************************/
case 'W':
case 'w':
sendString("\r\nInteger(0-FF)?");										//Reserves space at top of EEPROM for use by the application

reservation[0] = '0';													//Enter most significant digit first
reservation[2] = '\0';													//Enter zero and pause for no reservation
while(1){
reservation[1] = waitforkeypress();
if(non_numeric_char (reservation[1]))sendChar ('?'); else break;}		//Only accepts legal keypresses 0 to 9 and A to F

if(isCharavailable(100)){while(1){										//If keypress made acquire it else skip to "askiX2_to_hex" subroutine
key_press = receiveChar();
if (non_numeric_char (key_press)) {sendChar ('?'); 						//Illegal keypress
while (!(UCSR0A & (1 << RXC0)));}else break;}binUnwantedChars();		//Insists on legal keypress: 	
reservation[0] = reservation[1]; 
reservation[1] = key_press;}
	
App_reservation =  askiX2_to_hex(reservation);							//Convert entry to binary form

if(App_reservation > 0){
sendString("\r\nApp: Start addr's  ");
sendHex(16, (EE_top - App_reservation));newline();						//Display first address to be used by the application
sendString ("X to escape or AOK\r\n");
if(waitforkeypress() == 'X') 
{binUnwantedChars();	wdt_enable(WDTO_1S); while(1);}}				//Accidental key press: Press X to start again.
binUnwantedChars();		
EE_top = EE_top - App_reservation;										//Variable "EEPROM" stores highest address available for user strings and data. 
Read_write_mem('I', 0x3, (EE_top >> 8)); 								//Save the variable "EEPROM" at the start of the EEPROM in addresses 3 and 4. 
Read_write_mem('I', 0x4, (EE_top & 0x00FF));



//Repeat file download as many times as necessary to save text and data until end of file reached or the EEPROM allocation is all used up.
//This saves the need to lower the baud rate to allow time to write to EEPROM (which was considered to more awkward of the two options).



sendString("\r\nSend text file.");

do 																		//Remain in do-loop until text all downloaded
	{DL_status = (Download_text(&EEP_pointer,\
	&file_pointer, 	&DL_flag, &array_pointer,\
	EEPROM_buffer));													//Text saved to EEPROM buffer, then copied to EEPROM
	if (!(DL_status))													//If buffer full and space available in EEPROM
		{DL_flag = 0;  													//reset "DL_flag"
		LEDs_off;														//and request subsequent download
		sendString("  Again!");																		
		}
	}while (!(DL_status));												//Exit at end of text (-"- detected) 
LEDs_off;	

if (DL_flag == 2)														//Download data (if any) until EEPROM buffer is full
	{while (!(Download_data(&EEP_pointer, &file_pointer,\
	&DL_flag, &data_counter,&array_pointer,\
	EEPROM_buffer)))													//Save text and data to EEPROM			
		{LEDs_off;														//If EEPROM buffer fills						
		sendString("  Again!");											//request additional download until EEPROM is full
		}
	}
binUnwantedChars ();
LEDs_off;

sendString("   AK?\r\n");	binUnwantedChars();		
Read_write_mem('I', 0x0, (EEP_pointer >> 8));  	
Read_write_mem('I', 0x1, (EEP_pointer & 0x00FF));						//Save address in EEPROM available for first data item
Read_write_mem('I', 0x2, data_counter);									//Save number of data items (each occupy 16 bits)	
waitforkeypress();
Upload_text(EEP_pointer); 

Read_write_mem('I', 0x5, --string_counter);
sendHex(10, string_counter);

 
if (data_counter > 0) Upload_data (EEP_pointer, data_counter);  
break;  




/********************************************************************************************************************************************/
case 'C':																//Check the cal bytes have not been corrupted
case 'c':
sendString("\r\nUser/Default calibration backup values\t");
{sendHex(10, Read_write_mem('O', EE_size - 1, 0));
sendChar('\t');sendHex(10, Read_write_mem('O', EE_size - 2, 0));
sendHex(10, Read_write_mem('O', EE_size - 3, 0));
newline();
}
break;

default: break;}

SW_reset;

}



/*
Subroutine Download_text() returns	"DL_status" and sets "DL_flag"

Download_text() returns 0	"DL_flag" = 1	Text partially downloaded and saved to EEPROM. Request additional download.
Download_text() returns 1	"DL_flag" = 2	End of text encountered.  Text saved to EEPROM_buffer but not to EEPROM. Call "download_data()"
Download_text() returns 1	"DL_flag" = 3	Text download halted: EEPROM is full.

if "DL_flag" == 2  subroutine "Download_data()" is called to finish saving text to EEPROM and start saving data (if any)

download_data() returns	1	"DL_flag" = 2			File download complete with or without data
download_data() returns	1	"DL_flag" = 3			Data download stopped EEPROM allocation all used up
download_data() returns	0	"DL_flag" = 0			Request additional download
download_data() returns	1	"DL_flag" = 4			Data download stopped EEPROM allocation all used up
*/




/********************************************************************************************************************************************/
char Download_text(int *ptr_EEP_pointer,\
int *ptr_file_pointer,\
char *ptr_DL_flag,\
int *ptr_array_pointer, unsigned char EEPROM_buffer[]){	
int UART_counter = 0;
char text_char;	


//Ignore short preliminary text section until the first -"- is encounter which signals the start of the first string to be saved to EEPROM


text_char = waitforkeypress();												//Count characters as they are downloaded from the PC									
if (text_char !=  '\0')UART_counter++;										//Arduino seems to down load several spurious nulls during download.
while(1){text_char = next_char_from_PC(); 
if ((text_char !=  '\0'))UART_counter++; 									//Ignore characters until a -"- is detected, then place them
if (text_char == '"') break;}												//in an EEPROM buffer if being downloaded for the first time	


//Save text to EEPROM_buffer until it is full or second -"- is encountered 
//signaling the end of the text strings and start of data (if any)

	
while(!(*ptr_DL_flag))													//EEPROM buffer full? No: Enter loop 1. Yes: skip Loop 1. 
{if((text_char = next_char_from_PC()) == '\0')continue; 				//Ignore null chars loop back to top of the while loop
else UART_counter++; 													//Loop 1: Acquire text characters and increment "UART_counter"

if (text_char != '"')													//Check for -"-? No: Enter Loop 2: Yes: skip loop 2.
	{if (UART_counter > *ptr_file_pointer)								//Loop 2: Text downloaded for first time? Yes: Enter Loop 3. No: skip loop 3
		{switch (text_char)												//Loop 3: Detect carriage return and new line:
			{case '\n':													//combine them if necessary and replace with '\0'
			case '\r':
				if (EEPROM_buffer[((*ptr_array_pointer)-1)] != '\0')
					{text_char = '\0';}
				else {((*ptr_array_pointer)--); text_char = '\0';}
			default: break;
			}
		EEPROM_buffer[((*ptr_array_pointer)++)] = text_char;			//Place characters as they are received in the EEPROM buffer

		if((*ptr_array_pointer >= RBL) && (text_char != '\r') &&\
				 (text_char != '\0'))									//If EEPROM buffer fills up part way through a string 
			*ptr_DL_flag = 1;											//set "*ptr_DL_flag" to force exit from Loop 1
		}																//Loop 3																														
	}																	//Loop 2
	else break;															//Second -"- received: Exit Loop 1.	 																					
}																		//Loop 1: Exit when EEPROM buffer is full 

//EEPROM buffer full: This usually means that the text download 
//is not complete

if (*ptr_DL_flag == 1)													//If EEPROM buffer is full: Text download is not complete													
	{binUnwantedChars(); 												//Ignore remaining characters in the EEPROM text/data file														
	*ptr_file_pointer = UART_counter;									//Save the "UART_counter" in "file_pointer"
	EEPROM_buffer[(*ptr_array_pointer)++] = '\0';						//Terminate the EEPROM_buffer with a '\0'.

	LEDs_on;
	{int n=0;															//Copy characters from "EEPROM_buffer" to the EEPROM
	while (((*ptr_EEP_pointer) +n) < EE_top)							//Stop copying when space reserved for the application is reached
		{Read_write_mem('I', ((*ptr_EEP_pointer) + n),\
		EEPROM_buffer[n]); 
		n++;
		if(n==(*ptr_array_pointer))break;}								//Exit when the end of the "EEPROM_buffer" is reached

	if (((*ptr_EEP_pointer)+n) == EE_top)								//EEPROM allocation all used? Yes:
		{*ptr_DL_flag = 3; 												//Set DL_flag to 3.																	
		Read_write_mem('I', (EE_top-1), '\0'); 							//Terminate EEPROM in '\0'
		*ptr_EEP_pointer = EE_top; 										//Set EEP_pointer to EEPROM 
		sendString("\r\nEEPROM is full!");
		return 1;}														//Return setting the "DL_status" to 1

	(*ptr_EEP_pointer) += n-1; 											//Set "EEP_pointer" to next available address in the EEPROM
	*ptr_array_pointer = 0;}  											//Get ready to start filling the EEPROM buffer again.
	return 0;}															//Return setting the "DL_status" to 0


//End of text section of the file encountered: Any data is appended.


else 																	//-"- char encountered to terminate text input
	LEDs_on;							
	EEPROM_buffer[((*ptr_array_pointer)++)] = '\0';						//Terminate final string in a '\0'.
	*ptr_file_pointer = UART_counter;									//Save the "UART_counter" in "file_pointer"
	(*ptr_EEP_pointer) += (*ptr_array_pointer); 						//Set "EEP_pointer" to the address available for the first number 
	if((*ptr_EEP_pointer) >= EE_top)									//If saving the entire string would cause the EEPROM allocation to be exceeded
		{*ptr_DL_flag = 3; 												//set the download flag to 3
			{int n=0;
			while (((*ptr_EEP_pointer) - (*ptr_array_pointer) +n)\
			< (EE_top-1))
				{Read_write_mem('I', ((*ptr_EEP_pointer) -\
				(*ptr_array_pointer) + n),EEPROM_buffer[n]);			//Copy as much as possible of the EEPROM buffer to the EPPROM
				n++;
				}
			}
		Read_write_mem('I', (EE_top-1), '\0'); 							//Terminate the EEPROM in '\0'
		*ptr_EEP_pointer = EE_top; 
		sendString("\r\nEEPROM is full!");
		return 1;														//Return setting the "DL_status" to 1
		}
	*ptr_DL_flag = 2; 													//If overflow is not a problem set "DL_flag" to 2
	return 1;}															//Return setting the "DL_status" to 1






/********************************************************************************************************************************************/
char Download_data(int *ptr_EEP_pointer,int *ptr_file_pointer,\
char *ptr_DL_flag, int *ptr_data_counter,\
int *ptr_array_pointer,\
unsigned char EEPROM_buffer[])

{int data_int=0, UART_counter=0, ADDR_last_string=0, next_data_address=0;
char data_text;

if(*ptr_array_pointer == 0) 											//Text/data file: New file download: start refiling EEPROM_buffer at locataion zero
	{next_data_address = *ptr_EEP_pointer + *ptr_data_counter +\
	*ptr_data_counter;
	data_text = waitforkeypress();	UART_counter++;	
	while(UART_counter < *ptr_file_pointer) 							//Download and ignore section of EEPROM file that has already been saved
		{data_text = next_char_from_PC(); UART_counter++;
		}
	}
else																	//Text/data file: EEPROM_buffer partially full (i.e."DL_flag" = 2)
	{UART_counter = *ptr_file_pointer; 
	ADDR_last_string = *ptr_EEP_pointer - *ptr_array_pointer;			//Address of last text character in EEPROM
	}
	
while((*ptr_DL_flag ==0) || (*ptr_DL_flag ==2)) 						//EEPROM_bufer contains end of text plus data (if any) or only contains data
	{data_text = next_char_from_PC(); UART_counter++; 					//Loop 1: down load data
	if(!data_text)break;												//Times out and then exits at end of file
	switch (data_text)
	{case '-': case '+': case '0': case '1': case '2':  case '3': 		//Acquire decimal number (integer or char) 
	case '4': case '5':  case '6': case '7': case '8': case '9':  
		data_int = decimal_conversion(data_text, &UART_counter); 		//Increments UART counter as appropriate
		EEPROM_buffer[(*ptr_array_pointer)++] = (data_int >> 8);		//save each number in EEPROM buffer as integer
		EEPROM_buffer[(*ptr_array_pointer)++] = (data_int & 0x00FF);
		(*ptr_data_counter)++;
		if((*ptr_array_pointer >= RBL))
			{if(*ptr_DL_flag == 2) *ptr_DL_flag = 3;
			if(*ptr_DL_flag == 0) *ptr_DL_flag = 4;
			}break;

	case '$':  
		data_int = Hex_conversion(data_text, &UART_counter);
		EEPROM_buffer[(*ptr_array_pointer)++] = (data_int >> 8);
		EEPROM_buffer[(*ptr_array_pointer)++] = (data_int & 0x00FF);
		(*ptr_data_counter)++;
		if((*ptr_array_pointer >= RBL))							
			{if(*ptr_DL_flag == 2) *ptr_DL_flag = 3;
			if(*ptr_DL_flag == 0) *ptr_DL_flag = 4;
			}break;

		default: break;}
	} 																	//LOOP 1

binUnwantedChars();	

switch (*ptr_DL_flag)															
	{case 2:															//Text download complete without causing EEPROM overflow 
		LEDs_on;														//Subsequent data (if any) all fits into the EEPROM_buffer
		{int n=0;\
		while ((ADDR_last_string +n) < (EE_top))
			{Read_write_mem('I', (ADDR_last_string + n),\
			EEPROM_buffer[n]);											//Save remaining text and data to EEPROM until the EEPROM is full
			n++;														//or until the EEPROM_buffer is empty (i.e.the download is complete)
			if (n>=(*ptr_array_pointer))break;
			}
		} 
		return 1; break;
case 3: 																//Similar to case 2 but the data occupies all the remaining  
	LEDs_on;															//space in the EEPROM_buffer	
	{int n=0;																																					
	while ((ADDR_last_string +n) < (EE_top))
		{Read_write_mem('I', (ADDR_last_string + n),EEPROM_buffer[n]); 
		n++;
		if (n>=(*ptr_array_pointer))break;								//Exit when the EEPROM is full or the end of the EEPROM buffer is reached
		
		}
	if((ADDR_last_string +n) == (EE_top)) 								//If the EEEPROM is full return 1
		{LEDs_off;																		
		return 1;
		} 
	} 																											
	(*ptr_array_pointer) = 0;  											//EEPROM_buffer is full: return 0. New file download required
	*ptr_DL_flag = 0; 													//Go to case 0 next time
	*ptr_file_pointer = UART_counter;
	return 0; break;

case 0:																	//New file download	Contains data only
	LEDs_on;															//RAM buffer only partially full:  End of file Numbers only
	{int n=0;
	while ((next_data_address +n) < (EE_top))							//Exit if EEPROM fills
		{Read_write_mem('I', (next_data_address + n),\
		EEPROM_buffer[n]); 	
		n++;
		if (n>=(*ptr_array_pointer))break;
		}
	}
	(*ptr_array_pointer) = 0;											//If there is more data go to case 4 for all subsequent downloads
	return 1;


case 4: 																//RAM buffer completely full:  Numbers only
	LEDs_on;
	{int n=0;
	while ((next_data_address +n) < (EE_top))
		{Read_write_mem('I', (next_data_address + n),\
		EEPROM_buffer[n]);  
		n++;
		if (n>=(*ptr_array_pointer))break;
		}
	if((next_data_address +n) == (EE_top))  
		{return 1;
		}
	}  																	//EEPROM overflow occurs: Save text and exit
	(*ptr_array_pointer) = 0; 
	*ptr_DL_flag = 0; 
	*ptr_file_pointer = UART_counter;
	return 0;
	}
return 0;
}




/********************************************************************************************************************************************/
int decimal_conversion (char data_text, int *ptr_UART_counter){
int char_counter=0;
char data_string[7], num_char;
data_string[char_counter++] = data_text;
while(1){
num_char = next_char_from_PC(); (*ptr_UART_counter)++;
if (num_char == ' ') break;
if (num_char == '\r') break;
if (num_char == '\n') break;
if (num_char == '\0') break;
if (num_char == '\t') break;
else data_string[char_counter++] = num_char;}
data_string[char_counter++] = '\0';
return my_atoi_with_sign (data_string);}





/********************************************************************************************************************************************/
int Hex_conversion (char data_text, int *ptr_UART_counter){
int char_counter=0;
char data_string[7], num_char;
while(1){
num_char = next_char_from_PC(); (*ptr_UART_counter)++;
if (num_char == ' ') break;
if (num_char == '\r') break;
if (num_char == '\n') break;
if (num_char == '\0') break;
if (num_char == '\t') break;
if (num_char == ',') break;
else data_string[char_counter++] = num_char;}
data_string[char_counter++] = '\0';
return askiX4_to_hex_V2 (data_string);}



/********************************************************************************************************************************************/
void Upload_text(int EEP_pointer)
{char string_char;
char char_counter = 0;
int EEP_mem_counter = 0;

EEP_mem_counter = text_start;
while(EEP_mem_counter < EEP_pointer)
	{sendString("\r\n$");
	sendHex (16, EEP_mem_counter); 
	string_counter++;
	sendChar ('\t');
	while(1)
		{string_char = Read_write_mem('O',(EEP_mem_counter++),0);
		if(string_char == '\0') break;
		char_counter += 1; 
		if((char_counter >= 90) &&  (string_char == ' ')){sendString("\r\n\t"); char_counter = 0;}
		else sendChar(string_char);		
		Timer_T0_sub(T0_delay_5ms);
		}newline();char_counter = 0;
	}newline();
}




/********************************************************************************************************************************************/
void Upload_data(int address_first_data_item, int data_counter)
{int output_counter=0;
int data_item = 0 ;


newline();
sendHex (16, (address_first_data_item));
	{int n=0; 
	while((address_first_data_item+n+1) < EE_top)
		{data_item = ((Read_write_mem('O', address_first_data_item+n, 0x0) << 8)\
		+ Read_write_mem('O',(address_first_data_item+n+1), 0x0));
		if((output_counter) && (!(output_counter%4)))
			{if(output_counter==4)sendString("\tAK to continue"); 
			waitforkeypress(); newline();
			sendHex (16, (address_first_data_item+n));
			}
		sendChar('\t'); sendHex (16, data_item); 
		sendChar ('\t'); sendsignedHex (data_item); 
		output_counter++; n+=2; 
		if(n>2*(data_counter-1))break; 
		}
	binUnwantedChars();	
	}
}



/********************************************************************************************************************************************/
void Upload_data_1(int address_first_data_item, int data_counter)
{char output_counter=0;
int data_item = 0 ;


newline();
sendHex (16, (address_first_data_item));
	{int n=0; 
	while((address_first_data_item+n+1) < EE_top)
		{data_item = ((Read_write_mem('O', address_first_data_item+n, 0x0) << 8)\
		+ Read_write_mem('O',(address_first_data_item+n+1), 0x0));
		if((output_counter) && (!(output_counter%8)))
			{if(output_counter==8)sendString("\tAK to continue"); 
			waitforkeypress(); newline();
			sendHex (16, (address_first_data_item+n));
			}
		sendChar ('\t'); sendsignedHex (data_item); 
		output_counter++; n+=2; 
		if(n>2*(data_counter-1))break; 
		}
	binUnwantedChars();
	}
}



/********************************************************************************************************************************************/
void Upload_data_2(int address_first_data_item, int data_counter)
{char output_counter=0;
int data_item = 0 ;


newline();
sendHex (16, (address_first_data_item));
	{int n=0; 
	while((address_first_data_item+n+1) < EE_top)
		{data_item = ((Read_write_mem('O', address_first_data_item+n, 0x0) << 8)\
		+ Read_write_mem('O',(address_first_data_item+n+1), 0x0));
		if((output_counter) && (!(output_counter%8)))
			{if(output_counter==8)sendString("\tAK to continue"); 
			waitforkeypress(); newline();
			sendHex (16, (address_first_data_item+n));
			}
		sendChar('\t'); sendHex (16, data_item); 
		output_counter++; n+=2; 
		if(n>2*(data_counter-1))break; 
		}
	binUnwantedChars();
	}
}



/********************************************************************************************************************************************/
char next_char_from_PC(void){
unsigned int n = 0;
while (!(UCSR0A & (1 << RXC0))){n++;
if (n > 15000) return 0;}													
return UDR0;}



