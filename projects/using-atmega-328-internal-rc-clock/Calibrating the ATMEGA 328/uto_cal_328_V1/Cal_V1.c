

/*The following fuse bytes are required:
extended_fuse byte: 0xFF	(left in its default state)
Fuse byte high:		0xD7	(WDT under control of program and EEPROM prserved through chip erase)
Fuse byte:			0xE2	(*MHz internal RC clock selected)
The device was programmed using the serial port
*/


#include "../Resources/Basic_HW_setup.h"
#include "../Resources/Basic_HW_plus_I_O.c"

void initialise_timers_for_cal_error(void);
void start_timers_for_cal_error(void);
long plot_cal_error(void);


volatile int EA_counter;				//Used to obtain average error for each OSCCAL setting
volatile long error_SUM;
volatile char T1_OVF;


int main (void){

long error, error_mag; 
setup_HW;

ASSR = (1 << AS2); 						//initialise T2 for crystal
TCNT2 = 0;									//Reset counter register

Timer_T0_sub(T0_delay_10ms);									//Allows printing to finish before OSCCAL changes
		sei();
		TIMSK2 |= (1 << TOIE2);								//Set Timer 2: interrupt on overflow
		TIMSK1 |= (1 << TOIE1);								//Set Timer 1: interrupt on overflow
		initialise_timers_for_cal_error();
		start_timers_for_cal_error();
		
		for(int m = 0; m < 5; m++)plot_cal_error();			//Warm up time
		
		for(int m = 0x0F; m <= 0xF0; m++){OSCCAL = m; 
		error = plot_cal_error();  
		sendChar('.');
		if (error < 0) error_mag = error * (-1); 
		else error_mag = error;
		if(error_mag < 500)break;}								//Vaue can be reduced for better accuracy
		
		
		initialise_timers_for_cal_error();
		TIMSK2 &= (~(1 << TOIE2));								//Dissable Timer 2: interrupt on overflow
		TIMSK1 &= (~(1 << TOIE1));								//Dissable Timer 1: interrupt on overflow
		
		newline();
		
		eeprom_write_byte((uint8_t*)0x3FE, OSCCAL); 			//Save OSCCAL value to EEPROM 
		eeprom_write_byte((uint8_t*)0x3FF, OSCCAL);
	
	sendString("\r\nCal values saved to EEPROM\t");				//Echo values back from the EEPROM
	sendLongNum(10,eeprom_read_byte((uint8_t*)0x3FE));
	sendString ("    ");
	sendLongNum(10,eeprom_read_byte((uint8_t*)0x3FF));
	sendString("\terror\t");sendLongNum(10,error_mag);
	sendString("\r\nAK to repeat\r\n");
	waitforkeypress();SW_reset;}								//keypress to repeat cal process
		
		

/*************************************************************/
ISR(TIMER2_OVF_vect) {
long TCNT1_BKP, overflow = 0, target_res;

target_res = 62500;
TCCR1B = 0;							//Halt T1
TCNT1_BKP = TCNT1;					//Copy the value of TCNT1
TCNT1 = 0;							//Clear TCNT1
TCCR1B = 1;							//Get T1 running again ASAP (Note T2 has not stopped running)
									//Get T1 running again ASAP (Note T2 has not stopped running)
if(EA_counter < 5)T1_OVF = 0;		//Ignore first 5 results
else
{switch(T1_OVF){
case 0: overflow = 0; break;
case 1: overflow = 65546; break;
case 2: overflow = 131072; break;}
error_SUM = error_SUM + (TCNT1_BKP - target_res + overflow);
T1_OVF = 0;}
EA_counter++;}


ISR(TIMER1_OVF_vect) {T1_OVF += 1;}



/*************************************************************/
void initialise_timers_for_cal_error(void){
		TCNT1=0;TCCR1B = 0;									//Reset and halt T1
		TCCR2B =  0x0;	while(ASSR & (1 << TCR2BUB));		//Halt T2
		TCCR2A = 0; while(ASSR & (1 << TCR2AUB));			//Reset T2 
		TCNT2=0; while(ASSR & (1 << TCN2UB));	}			//Reset TCNT2	

		
		void start_timers_for_cal_error(void)
		{TCCR2B = 1; 	
		while(ASSR & (1 << TCR2BUB));
		TCCR1B = 1;}	


long plot_cal_error()		
		{long error;
		EA_counter = 0;									//Compute error for each value of OSCCAL 10 times
		error_SUM = 0;
		while(EA_counter < 15);							//Execution halts here untill T2 interrupts increase EA_counter to 15
		{EA_counter = 0;}
		error = error_SUM;
		return error/10;}		

