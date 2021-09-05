/*
Program runs on Attiny1606
and is compiled using default Studio 7 compiler settings
 */ 


#include "Project.h"
int mag(int);


int main(void){
		
	char cal_factor;											//Used to adjust internal oscillator frequency
	char null_bit_counter;										//Counts number of blank display digits
	int array_ptr;												//Points to display digits
	
	
	CPU_CCP = 0xD8;												//Config change protection key
	CLKCTRL_MCLKCTRLB = CLKCTRL_PDIV_8X_gc | 1;					//Generates 2MHz Peripheral clock
	
	cal_factor = CLKCTRL_OSC20MCALIBA - 1;						//Adjust calibration factor if necessary
	CPU_CCP = 0xD8;
	CLKCTRL_OSC20MCALIBA = cal_factor;
	
	brightness_control = 500;									//Selects low brightness at reset
	
	clear_display_buffer;										//Generate test display
	for(int m = 0; m <= 7; m++)									//Display 75311357 on reset:  Confirms that display is working correctly
	display_buffer[m] = mag(7 - (2*m)) + '0';
	Set_display_ports;
	   
   PORTC.DIR &= ~PIN3_bm;										//Configure comm port as input
   PORTC.OUT &= ~(PIN3_bm);										//I/O pin low when configured as output
   PORTC.PIN3CTRL |= PORT_PULLUPEN_bm;							//Pull-up enabled
      
   Start_TCA0();												//Display (2mS) tick rate
   sei();
	
	while(1){
		while(!(transaction_complete));							//Wait here for requests from the UNO
		transaction_complete = 0;
		busy_flag = 1;											//Do not contact UNO until data processing is complete
		
	switch (transaction_type){									//Transaction complete: Process the data
		
	case 'A':													//When "cr" detected convert string from UNO to long number
	if(cr_keypress == 1){
	cr_keypress = 0;
	display_buffer2temp;										//Reverse string prior to conversion to binary
	Long_Num_to_UNO = atol(temp_buffer);	
	for(int m = 0; m <= 3; m++)
	data_byte[m] = Long_Num_to_UNO >> (8*(3-m));}				//Split long number into 4 bytes for re-transmission
	break;
		
	case 'B':													//When "cr" detected convert string from UNO to float
	if(cr_keypress == 1){
	cr_keypress = 0;
	check_for_dp;
	Add_dp_if_missing;											//FP arithmetic requires numbers with a dp
	display_buffer2temp;										//Reverse string prior to conversion to a FPN
	extract_dp;													//Display merges the dp with a digit  											
	Float_Num_to_UNO = atof(temp_buffer);
	char_ptr = (char*)&Float_Num_to_UNO;						//Split the number into bytes ready for return to the UNO
	for (int m = 0; m <= 3; m++)
	{data_byte[m] = *char_ptr;
	char_ptr += 1;}
	}break;
		
		
	case 'C':													//Convert long from UNO to string
	clear_display_buffer;
	clear_temp_buffer;
	ltoa(Long_Num_from_UNO, temp_buffer, 10);
	temp2display_buffer;										//Reverse string prior to display	
	break;
			
	case 'D': 													//Convert float from UNO to string
	Float_Num_from_UNO  = *float_ptr;
	ftoaL(Float_Num_from_UNO);
	Combine_dp;		 
	Insert_sign;
	Remove_lagging_zeros;
	temp2display_buffer;										//Reverse string prior to display
	if (expt){Left_justify_number;
	Add_exponent ();}
	break;
		
	case 'E':													//Return binary number (long or float) to UNO
	for(int m = 0; m <= 7; m++)
	temp_buffer[m] = display_buffer[m];							//Flash display
	clear_display_buffer;
	_delay_ms(100);
	for(int m = 0; m <= 7; m++)
	display_buffer[m] = temp_buffer[m];
	break;
	
	case 'G': break;											//Toggles brightness
	
	}
		
	transaction_type = 0;
	busy_flag = 0;
	}}



/*****************************************************************************************************************************/
	void Start_TCA0(void){										//Clock that controls display and FW comms port
	display_ptr = 0;
	TCA0_SINGLE_CNT = 0;										//Initialise counter
	TCA0_SINGLE_CMP0 = display_tick;							//2mS period for 2MHz clock
	TCA0_SINGLE_CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | 1;			//Start clock with 2MHz clock
	TCA0_SINGLE_INTCTRL |= TCA_SINGLE_CMP0EN_bm;}				//Interrupt flag on compare match/*****************************************************************************************************************************/	ISR (TCA0_CMP0_vect){										//ISR which responds to clock ticks	TCA0_SINGLE_INTFLAGS |= TCA_SINGLE_CMP0EN_bm;				//Clear the interrupt flag	if(display_ptr <= 7)										//Drive the 8 display digits	{Display_driver();	if(display_ptr < 7){inc_display_clock;}						//Setup 2mS clock tick for first 7 digits	else TCA0_SINGLE_CMP0 += 1800;								//setup 0.9mS clock tick for last digit	display_ptr += 1;	return;}				display_ptr += 1;				cmp0_bkp = TCA0_SINGLE_CMP0 + 2200;							//Save 1.1mS clock tick for first digit	if (!(busy_flag))	{inc_comms_clock;											//only poll UNO if no transactions are ongoing	PORTC.DIR |= PIN3_bm;										//initiate start pulse	comms_transaction();}										//Poll the UNO	TCA0_SINGLE_CMP0 = cmp0_bkp;								//Reinstate the 1.1ms clock tick	display_ptr = 0;}											//Set display_pointer to the first digit									/*****************************************************************************************************************************/	void Display_driver(void){				switch(display_ptr){			case 0: digit_0; break;			case 1: digit_1; break;			case 2: digit_2; break;			case 3: digit_3; break;			case 4: digit_4; break;			case 5: digit_5; break;			case 6: digit_6; break;		case 7: digit_7; break;}		Char_definition();		Start_TCB0(brightness_control);}									//TCB0 controls the brightness
								/*****************************************************************************************************************************/	void Char_definition()
	{switch (display_buffer[display_ptr]){
	case '0': zero; break;
	case '1': one; break;
	case '2': two; break;
	case '3': three; break;
	case '4': four; break;
	case '5': five; break;
	case '6': six; break;
	case '7': seven; break;
	case '8': eight; break;
	case '9': nine; break;
	case '-': minus; break;
			
	case 'E': case 'e': exponent; break;
	case ('0' | 0x80): zero_point; break;
	case ('1' | 0x80): one_point; break;
	case ('2' | 0x80): two_point; break;
	case ('3' | 0x80): three_point; break;
	case ('4' | 0x80): four_point; break;
	case ('5' | 0x80): five_point; break;
	case ('6' | 0x80): six_point; break;
	case ('7' | 0x80): seven_point; break;
	case ('8' | 0x80): eight_point; break;
	case ('9' | 0x80): nine_point; break;
	case ('-' | 0x80): minus_point; break;}}
	



	/*****************************************************************************************************************************/	void Start_TCB0(int BC){						//Controls the display intensity
	TCB0_CNT = 0;									//Initialise counter
	TCB0_CCMP = BC;									//1.75mS for full brightness 0.25ms for low brightness
	TCB0_CTRLA = 1;									//Start the clock	TCB0_INTCTRL = 1;}								//Interrupt flag on compare match
		
	ISR (TCB0_INT_vect){							//Clear display
	TCB0_INTFLAGS = 1;								//Clear interrupt flag
	TCB0_CTRLA = 0;									//Stop the clock	
	clear_digits; clear_display;}	
	
	
	int mag(int m){									//Convert negative number to positive
		int n = 0;
		if (m < 0)n = m * (-1);
		else n = m;
		return n;}