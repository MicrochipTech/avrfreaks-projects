

/******************************************************************************************************/
void newline(void){String_to_USI ("\r\n");}



/*******************************************************************************************************/
void String_to_USI(const char* s){									//Send predefined string to PC
	int i = 0;
	while(i < 200){
		if(s[i] == '\0') break;
	Char_to_USI(s[i++]);} }
	

	
	
	/*******************************************************************************************************/
	void Echo_string(unsigned char* s){								//Send variable string to PC
		int i = 0;
		while(i < 200){
			if(s[i] == '\0') break;
		Char_to_USI(s[i++]);} }
		
		
		
		/******************************************************************************************************/
		char wait_for_return_key(void){								//Used to keep receiving keypresses until return is pressed
			char keypress;
			keypress = Char_from_USI (0);							//Wait indefinitely
			if((keypress == '\r') || (keypress == '\n')){			//If carriage return
			Char_from_USI (1);keypress = '\r';}						//Ignore companion char if present
		return keypress;}



		/******************************************************************************************************/
		char isCharavailable (int m){								//Wait for keypress with a timeout
			char keypress = 0;										//Return 0 if there is no keypress
			while (!(keypress = Char_from_USI(1)) && m--);
		return keypress;}




		/******************************************************************************************************/
		char decimal_digit (char data){
			if (((data > '9') || (data < '0')) )return 0;
		else return 1;}



		/********************************************************************************************************/
		int Num_from_KBD(void){
			char keypress;
			int number;

			do
			{keypress =  Char_from_USI(0);}									//Fist digit
				while(!(decimal_digit(keypress)));							//Ignore illegal keypresses

			keypress -= '0';
			number =  keypress;												//Convert keypress to number

			while(1){
				if ((keypress = wait_for_return_key())  =='\r')break;		//Keep accepting keypresses until return is pressed
				if (decimal_digit (keypress))								//Ignore illegal keypresses
				{
					keypress -= '0';
				number = number * 10 + keypress;}}							//Gradually build up the number
			return number;}

			

			/***************************************************************************************/
			void Flash_String_to_USI(const char* pFlashStr){
				int i;
				for(i = 0; pgm_read_byte(&pFlashStr[i]) && i < 60; i++)
			Char_to_USI(pgm_read_byte(&pFlashStr[i]));}


			
			
