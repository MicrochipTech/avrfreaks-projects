



/***************************************************************************************************************************************************/
unsigned char string_counter(int next_address){							//Scroll through text section of flash counting the '\0' chars
	char next_char, previous = 0;										//until '\0' '\0' is detected to indicate the end of the last string
	unsigned char counter = 0;
	
	while(1){
		next_char = Char_from_flash(next_address);						//result provided by assembly subroutine
		if(next_char == 0){counter += 1; char_counter += 1;
		if (!(previous)) return counter-1;}
		else char_counter += 1;
		previous = next_char;
	next_address -= 1;}}
	
	
	
	/***************************************************************************************************************************************************/
	void print_string_num(int text_num, int next_address){				//scroll through text section of flash counting '\0' chars
		int null_counter = 1;											//until the start of the required string
		
		line_length = 0;
		while(1){
			if(null_counter == text_num)break;
			next_char = Char_from_flash(next_address);
			if(next_char == 0)null_counter += 1;
		next_address -= 1;}

		while(1){														//Print string; exit at '\0' character
			next_char = Char_from_flash(next_address);
			if(next_char)Char_to_USI(next_char); else break;
			line_length += 1;
			if ((next_char == ' ') && (line_length > 90))				//avoid long lines of print
			{String_to_USI("\r\n");line_length = 0;}
		next_address -= 1; }}




		/***************************************************************************************************************************************************/
		char Char_from_flash(int next_address){

			Prog_mem_address_H = next_address >> 8;
			Prog_mem_address_L = next_address;

			asm volatile ("push r0") ;									//Save registers before inserting assembly code
			asm volatile ("push r31") ;
			asm volatile ("push r30") ;

			asm volatile ("lds r31, Prog_mem_address_H");				//load ZH & L pointer from static RAM
			asm volatile ("lds r30, Prog_mem_address_L");

			asm volatile ("lpm r0, Z");									//load program memory into register r0

			asm volatile ("sts Flash_readout,  r0");					//Store contents of r0 in static RAM

			asm volatile ("pop r30");									//Restore registers
			asm volatile ("pop r31");
			asm volatile ("pop r0");

		return Flash_readout;}

		
		