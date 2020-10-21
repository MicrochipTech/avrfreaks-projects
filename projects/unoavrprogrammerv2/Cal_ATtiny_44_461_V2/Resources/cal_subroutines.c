


/************************************************************************************************/
long compute_error(char Num_Av, char sign)
{long error;
	
	error_sum = 0;
	int_counter = 0;													//Initialise all registers
	TCNT1_sum = 0;
	TCCR1B = 0;															//Ensure T1 is halted
	TCNT1 = 0;															//clear Timer 1
	enable_PCI_on_SCK_pin;
	set_PCI_mask_on_SCK;	
	Enable_Timer_1_Interrupt;
	while (int_counter < Num_Av);										//Pause here for interrupts: Average the result over several 32.768mS periods
	disable_PCI_on_SCK_pin;
	clear_PCI_mask_on_SCK;	
	Disable_Timer_1_Interrupt;
	error = error_sum/Num_Av;											//Obtain average result
	if (!(sign) && error < 0) error = error * (-1);						//Set sign if required
	return error;
	}




/************************************************************************************************/
void save_cal_values(unsigned char OSCCAL_user){
	eeprom_write_byte((uint8_t*)(EE_size - 1), OSCCAL_user);
	eeprom_write_byte((uint8_t*)(EE_size - 2), OSCCAL_user);
eeprom_write_byte((uint8_t*)(EE_size - 3), OSCCAL_DV);}




/********************************************************************************************************************************/
void printout_cal_values(void){
	String_to_USI("\r\nValues saved to EEPROM  ");
	Num_to_PC(10,eeprom_read_byte((uint8_t*)(EE_size-3))); String_to_USI("  ");
	Num_to_PC(10,eeprom_read_byte((uint8_t*)(EE_size-2))); String_to_USI("  ");
Num_to_PC(10,eeprom_read_byte((uint8_t*)(EE_size-1))); newline();}
