





void comms_transaction (void){if (!(transaction_type))											//No transaction in progress
{Rx_symbol = Receive_data_byte();									//Poll the UNO
if((unsigned char)Rx_symbol == 0xFF)return;							//No response from UNO
if (!(byte_counter))												//Response from UNO initiates new transaction
{byte_counter = 1;													
transaction_type = Rx_symbol;										//First symbol received defines transaction type

if(transaction_type == 'F')
{CCP = 0xD8; WDT.CTRLA = 3;	while(1);}								//32mS watch dog period

if(transaction_type == 'G'){
if(brightness_control == 3500)brightness_control = 500;
else brightness_control = 3500;
transaction_complete = 1;
byte_counter = 0;

}

return;}}
	
	
switch (transaction_type){											//Transaction in progress									
case 'A':															//Receiving long number string
case 'B':															//Receiving FPN string
Rx_symbol = Receive_data_byte();
if(Rx_symbol == 1){Rx_symbol = 0; cr_keypress = 1;}					//Carriage return detected
		
display_buffer[byte_counter - 1] = Rx_symbol;						//Put symbol in display buffer
byte_counter += 1;
if(byte_counter == 10)												//Display full: end transaction
{transaction_complete = 1; byte_counter = 0; }
break;

		
case 'C':															//Receive long binary number
Rx_symbol = Receive_data_byte();
Long_Num_from_UNO = (Long_Num_from_UNO << 8) | 
Rx_symbol; byte_counter += 1;
if (byte_counter == 5)												//Four bytes received
{transaction_complete = 1; byte_counter = 0;}	break;

			
case 'D':															//Receive FPN binary number
if (byte_counter == 1)												//First byte:set pointers
{float_ptr = &Float_Num_from_UNO;
char_ptr = (char*)&Float_Num_from_UNO;}								//Receive byte
Rx_symbol = Receive_data_byte();
*char_ptr = Rx_symbol;												//Save byte in FPN location
byte_counter += 1;
char_ptr += 1;
if (byte_counter == 5)												//Four bytes received
{transaction_complete = 1; byte_counter = 0;}break;
			
			
case 'E':															//Return binary FPN/long number in binary
Transmit_data_byte(data_byte[data_byte_ptr++]);
byte_counter += 1;
if(byte_counter == 5)												//Four bytes transmitted
{transaction_complete = 1; byte_counter = 0;
data_byte_ptr = 0;}
break;
}}



char Receive_data_byte (void){
	char Rx_char = 0;
	wait_for_clock_tick;	PORTC.DIR &= ~PIN3_bm;											//Restore WPU	inc_comms_clock;	wait_for_clock_tick;		Rx_char = 0;	for(int m = 0; m <= 7; m++){									//Receive character	inc_comms_clock;	wait_for_clock_tick;	Rx_char = Rx_char << 1;											//Shift left one bit	if (PORTC_IN & PIN3_bm)	Rx_char |= 1;}							//Add new bit	return Rx_char;}



		
	/****************************************************************************************************************************/	
	void Transmit_data_byte (char symbol){				for(int m = 0; m <= 7; m++){								//Transmit first character
		wait_for_clock_tick;
		if (symbol & (1 << (7-m)))PORTC.DIR &= ~PIN3_bm;			//WPU
		else PORTC.DIR |= PIN3_bm;									//Output low
		inc_comms_clock;}		wait_for_clock_tick;
	PORTC.DIR &= ~PIN3_bm;}
	
	
		
		
		
		
		
		
		