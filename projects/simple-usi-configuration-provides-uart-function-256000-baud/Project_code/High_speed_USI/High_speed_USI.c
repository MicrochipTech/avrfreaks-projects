	/*
	Basic Project that uses the High speed USI to communicate with a PC via a USB bridge.
	Recommendation for exploring the operation of this code:
	Use the UNO_AVR_Programmer_V2 to program a target device.
	First upload "Cal_ATtiny_44_461_V2.hex" and calibrate the target.
	Then upload "High_speed_USI.hex" to the target.
	In each case open Project.h and check that the correct device type is selected
	and at user prompt "Text_file? y or n" press n.
	
	At the "R    R   R......." prompt press "r" (may need pressing twice).
	Send individual keypresses or strings as prompted.
	If this works try uploading a text file at the "Text_file? y or n".
	Then press "R"
	
	Note: for ATtiny 861 and 461 devices pins 18, 19 and 20 are used for the USI port. 
	
	This program uses the compiler to save strings to flash.
	These share space also used by text files saved to flash and the maximum size
	of the text files must be reduced appropriately.
	*/
	
	
							

	#include "Project.h"
	
	int main (void)
	{
		unsigned char Keypress = 'A', char_buffer[buffer_size];				//Receive buffer for 45 chars
		unsigned char  Num_strings;											//The number of strings written to flash
		int  next_address;													//Address in memory of next character,
		int  text_num;														//Number of string to be printed out
								
		setup_ATtiny_HW;													//Configures USI as a transmitter
		User_prompt;														//Respond by pressing R or r.

								
		set_device_type_and_memory_size;									//confirm device type and set EEPROM size
		Flash_String_to_USI(message_1);										//\r\n\r\nProgram running on 
		String_to_USI("\r\nATtiny");
		String_to_USI (Device_type[device_ptr]);
		if (device_ptr == 7)while(1);
		newline();
	
	
	/***********************************************Basic test for the USI****************************************/
	if(User_response == 'r')
								
		{Flash_String_to_USI(message_2);									//\r\nEcho single keypresses: Return key to escape.\r\n
		while(( Keypress = wait_for_return_key()) != '\r'){
		Char_to_USI(Keypress);}
								
		Flash_String_to_USI(message_3);										//\r\nSend strings to USI\r\n
			
		text_num = 10;						
		while(text_num--){
			String_from_USI (char_buffer);									//Use Bray++ to assemble a string
		Echo_string(char_buffer);}
		}
		
		
		
		/***********************************************Reads Test from Flash****************************************/
		if(User_response == 'R'){
		char_counter = 0;													//counts the number of characters in the text file (excludes \r & \n)
		next_address = FlashSZ*2 - 1;										//start address of text

		if(Char_from_flash(next_address) == 0xFF){
			Flash_String_to_USI(message_4);									//\r\nNo text!\r\n
			wdt_enable(WDTO_60MS); while(1);}								//SW reset
		
		Flash_String_to_USI(message_5);										//\r\nReading strings.

		Num_strings = string_counter(next_address);							//Count the number of strings
		
		Flash_String_to_USI(message_6);										//\r\nString number? 0 to exit

		while(1){
			text_num = Num_from_KBD();
			if(!(text_num))break;
			if (text_num > Num_strings){
				Flash_String_to_USI(message_7);								//\r\nNo string!\r\n
				continue;}
			newline();
			String_to_USI("\r\n");
			print_string_num(text_num,next_address);
		}}
		
		wdt_enable(WDTO_60MS); while(1);
		
	return 1;}
	
	
	