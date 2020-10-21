
/*
Program designed to calibrate the ATtiny  44 and 84 461 and 861 devices.
"UNO_AVR_Programmer_V2" code running on the UNO is used to program the ATtiny devices.
A 65.536mS square wave on the UNO clock programming pin is used to calibrate them.

The ATtiny devices come with a Universal Serial Interface in place of the more usual UART.  
The USI routines used in this project are taken from Atmel Application note AVR307 and the accompanying
source code.  See http://becomingmaker.com/usi-serial-uart-attiny85/ for the links.
(Note: The author of this web page is also an Osborne, a different Osborne however from the one posting this project).   
 
While the target device is being programmed its USI Rx/Tx lines must be temporarily disconnected.
They must then be reconnected as user input is required to complete the calibration process.

Cut and paste could be used to developed cut down versions of this program that would run faster and 
more efficiently and even do without user input. Such a version could be  could be used to program 24 and 261 devices

The Project.h file contains two sections, one for the 44 family and one for the 461 family.   

*/



#include "Project.h"



int main (void)
{long error;
	unsigned char  OSCCAL_WV, OSCCAL_min = 0, offset, OSCCAL_UV;
	
	char counter = 0;
	unsigned char Rx_data_byte;
	int EEP_MAX = 0x2000;
	
	setup_Attiny_HW;
	
	sei();																//Required by USI and cal subroutines
	
	Char_from_USI(0);													//Pause while Rx/Tx lines are connected
	
	OSCCAL_DV = OSCCAL;													//Default OSCCAL
	OSCCAL_WV = OSCCAL;													//Value automatically selected
	OSCCAL_UV = OSCCAL;													//Value selected by user
	OSCCAL = 0xF0;
	while (OSCCAL>= 0x0F){OSCCAL;
		if(!(OSCCAL%3))Char_to_USI('.');								//Print . while stepping through OSCCAL values
		error = compute_error(1,0);										//All error values are positive
		if(error <1000) OSCCAL_WV = OSCCAL;
		if(error < 1000) counter += 1; else counter =0;					//Exit for viable value of OSCCAL
		OSCCAL -= 2;
	if (counter == 4)break;	}
	
	newline();
	set_device_type_and_memory_size;									//Confirm device type
	Flash_String_to_USI(message_1);
	String_to_USI (Device_type[device_ptr]);
	if (device_ptr == 7)while(1);										//Device not recognized: Halt
	
	Flash_String_to_USI(message_2);
	
	Num_to_PC(10, OSCCAL_DV);  String_to_USI("  ");
	Num_to_PC(10, OSCCAL_WV);String_to_USI("  ");
	Num_to_PC(10, eeprom_read_byte((uint8_t*)(EE_size - 2)));
	newline();
	
	counter = 0;
	for (int m = -15; m <=15; m++)										//Print out results
	{Timer_T0_sub(T0_delay_5ms);
		OSCCAL = OSCCAL_WV + m;
		error = compute_error(2,1);										//Signed error values
		if ((error < -1000) ||
		(error > 1000))counter = 0; else counter += 1;
		if(counter == 1)OSCCAL_min = OSCCAL;
		
		OSCCAL = OSCCAL_WV;
		
		Num_to_PC(16, counter); Char_to_USI('\t');
		Num_to_PC(10, OSCCAL_WV + m); Char_to_USI('\t');
		Num_to_PC(10, error);
	newline();}
	
	Flash_String_to_USI(message_3);										//User chooses actual value for OSCCAL
	
	
	while(1){
		if ((Rx_data_byte = Char_from_USI (0)) == 'x')break;
		offset = Rx_data_byte - '1';
		if(offset > 9)offset -= 7;
		OSCCAL_UV = OSCCAL_min + offset;
		Num_to_PC(10, OSCCAL_UV);
		newline();
	}
	save_cal_values(OSCCAL_UV);
	printout_cal_values();
	while(1);
return 1;}



/************************************************************************************************/
ISR(TIM0_COMPA_vect)												//Clock signal for USI: shifts data in the USIDataRegister
{
	if(Transmitter_active)OCR0A = Tx_clock;
	else OCR0A = Rx_clock;											//Necessary because receiver initially sets half the baud rate
TCNT0 = 0;}															//Reset T0



/************************************************************************************************/
ISR(USI_OVF_vect)													//USI counter overflows indicating the end of a transmission or reception

{USISR |= (1 << USIOIF);											//Clear USI interrupt flag

	if(Transmitter_active)
	char_transmitted = 1;
	else
char_received = 1;}													//USI receiver active




/************************************************************************************************/
ISR (PCINT0_vect){													//Pin change interrupt on DI pin or SCK pins
										
	if(calibration_active){											// Calibration subroutine active (PCI onSCK pin)
		if (!(TCCR1B)) {
			TCNT1_sum = 0;
			TCNT1 = 0;
			TCCR1B = prescaller_setting;							//1MHz clock
		}
		
		else {TCCR1B = 0; int_counter += 1;
			error_sum = error_sum + TCNT1_sum - 32768 + TCNT1;
		}
	}
	
	else{															//USI receiver active: start bit detected (PCI on DI pin)
		if(DI_pin_low)
		{TCNT0 = 0;
			OCR0A = Rx_clock/2;
			Start_clock;											//Start baud rate clock (Half period)
			TIFR0 = (1 << OCF0A);									//Clear spurious interrupts
			
			TIMSK0 |= (1 << OCIE0A);								//Enable interrupt on output compare
			USICR |= ( 1 << USIOIE);								//Enable USI counter interrupt
			USICR |= (1 << USIWM0);									//Select USI 3-wire mode
			USICR |= (1 << USICS0);									//Select USI clock source (timer0 compare match)

			char_received = 0;

			USISR = 0xF0 | 0x07;									//8 data bits + start bit
		Disable_PCI_on_DI_pin;}}
	}

	

ISR(TIM1_COMPA_vect){TCNT1_sum += 0x100; }




