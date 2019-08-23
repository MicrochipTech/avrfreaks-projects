

/*
328_autoCal_subroutine: 
This routine is combined with the "Hex_txt_programmer" for reliable use of the Atmega 328 when clocked with its internal RC oscillator.


It must be compiled to start at address 0x6C80.  The last two lines of the hex file:
		:0400000300006C800D  	which initiates a jump from location zero to location 0x6C80
		:00000001FF				which indicats the end oh the file.
 should be deleted.
 
The hex file for the "Hex_txt_programmer" should be copied into the hex file for 328_autoCal_subroutine so that the autoCal routine is
programmed first and the bootloader second.  The last line but one of the bootloader:
		:040000030000700089 	which initiates a jump from location zero to location 0x7000
should be deleted.
 
The autoCal routine uses two timers T1 and T2.
T1 and clocked at 1MHz using the internal 8HMz RC clock.
T2 is clocked using a 32.768KHz watch crystal.

T2 counts from zero to 255 and overflows every 7.8125mS during which time T1 counts 
up to 7,812 if the RC clock is running at exactly 8MHz.  OSCCAL is adjusted until the error is 1% or less.


Tested using the fuse settings applied to the "328 Bootloader for hex&text" which are
Extended 	0x5 (0xFD)		This sets the BOD settings which are non critical to this application
High		0xD0			This configures the Reset pin, WDT, Boot size and reset vector and ensures that the EEPROM is preserved during chip erase
Low 		0xC2			This configures the 8MHz internal RC clock and SUT.  

EEPROM reservations
0x3FF	user cal if set
0x3FE	user cal if set
0x3FD	Default cal supplied by Atmel
*/




#include "328_autoCal_subroutine.h"


int main (void){ 

char  calibrate=0;															
int error;

setup_HW;

ASSR = (1 << AS2); 												//initialise T2 for crystal

		initialise_timers_for_cal_error();
		start_timers_for_cal_error();
		
		for (int m = 0; m < 128; m++){								//1 second delay for crystal warm up
		while(!(TIFR2 & (1 << TOV2)));								//128 x 256 = 32,768
		TIFR2 |= (1 << TOV2);}
		
		initialise_timers_for_cal_error();
		start_timers_for_cal_error();
		
		while (!calibrate){
		
		error = cal_error(); 							
		if (error > 100) OSCCAL -=1;							//Positive error too great: reduce OSCCAL
		else {if (error*(-1) > 100) OSCCAL +=1;				//Negative error too great: Increase OSCCAL
			else calibrate = 1;}}								//Error within bounds: set "calibrate" and exit loop
		
	initialise_timers_for_cal_error();							//Reset and halt timers									 	
	eeprom_write_byte((uint8_t*)0x3FE, OSCCAL);	 			//Save results to EEPROM for later use by the "cal" macro
	eeprom_write_byte((uint8_t*)0x3FF, OSCCAL);	
	
	ASSR &= (~(1 << AS2)); 									//dissable crystal


wdt_enable(WDTO_30MS);while(1);}								//SW_reset returns program control to the bootoader 






/*********************************************************************************************/
		void initialise_timers_for_cal_error(void){
		TCNT1=0;TCCR1B = 0;										//Reset and halt T1
		TCCR2B =  0x0;	while(ASSR & (1 << TCR2BUB));			//Halt T2
		TCCR2A = 0; while(ASSR & (1 << TCR2AUB));				//Reset T2 
		TCNT2=0; while(ASSR & (1 << TCN2UB));	}				//Reset TCNT2



/*********************************************************************************************/		
		void start_timers_for_cal_error(void)
		{TCCR2B = 1; 											//32,768Hz clock derived from watch crystal	
		while(ASSR & (1 << TCR2BUB));							//overflows every 7.8125mS
		TCCR1B = 2;}											//1MHz clock counts to 7,812 in 7.8125mS		



/*********************************************************************************************/		
		int cal_error(void)										//Compute error for each value of OSCCAL
		{int EA_counter = 0;
		while(!(TIFR2 & (1 << TOV2)));							//Wait for T2 to overflow for first time
		TIFR2 |= (1 << TOV2);									//Reset overflow flag
		get_error(EA_counter);									//get result and ignore it
		EA_counter = 1;											//increment counter
		while(!(TIFR2 & (1 << TOV2)));							//Wait for T2 to overflow for second time
		TIFR2 |= (1 << TOV2);									//Reset overflow flag
		return (get_error(EA_counter));	}						//Get error and return it.
		
		
		
		
/*********************************************************************************************/
int get_error (char EA_counter){

int TCNT1_BKP, target_res;

target_res = 7812;																
TCCR1B = 0;														//Halt T1
TCNT1_BKP = TCNT1;												//Copy the value of TCNT1
TCNT1 = 0;														//Clear TCNT1
TCCR1B = 2;														//Get T1 running again ASAP (Note T2 has not stopped running)

if(!EA_counter)return 0;										//Ignore first result to allow for warm up.
else
return (TCNT1_BKP - target_res);}								//return second error result


	