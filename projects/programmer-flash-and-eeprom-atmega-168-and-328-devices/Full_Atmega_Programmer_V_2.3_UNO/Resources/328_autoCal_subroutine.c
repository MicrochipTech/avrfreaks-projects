
unsigned char Auto_cal_routine(void);
void initialise_timers_for_cal_error(void);
void start_timers_for_cal_error(void);
int cal_error(void);
int get_error (unsigned char);



unsigned char Auto_cal_routine(void){
unsigned char  calibrate=0;															
int error;

MCUSR = 0;

initialise_timers_for_cal_error();
		start_timers_for_cal_error();
		
		for (int m = 0; m < 128; m++){							//1 second delay for crystal warm up
		while(!(TIFR2 & (1 << TOV2)));							//128 x 256 = 32,768
		TIFR2 |= (1 << TOV2);}
		
		initialise_timers_for_cal_error();
		start_timers_for_cal_error();
		
		while (!calibrate){
		
		error = cal_error(); 												
		if (error > 75) OSCCAL -=1;							//Positive error too great: reduce OSCCAL
		else {if (error*(-1) > 75) OSCCAL +=1;					//Negative error too great: Increase OSCCAL
			else calibrate = 1;}}								//Error within bounds: set "calibrate" and exit loop
		
	initialise_timers_for_cal_error();							//Reset and halt timers									 	
	eeprom_write_byte((uint8_t*)0x3FE, OSCCAL);	 			//Save results to EEPROM for later use by the "cal" macro
	eeprom_write_byte((uint8_t*)0x3FF, OSCCAL);	
	
	ASSR &= (~(1 << AS2)); 									//dissable crystal
	return 1;}




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
		{char EA_counter = 0;
		while(!(TIFR2 & (1 << TOV2)));							//Wait for T2 to overflow for first time
		TIFR2 |= (1 << TOV2);									//Reset overflow flag
		get_error(EA_counter);									//get result and ignore it
		EA_counter = 1;											//increment counter
		while(!(TIFR2 & (1 << TOV2)));							//Wait for T2 to overflow for second time
		TIFR2 |= (1 << TOV2);									//Reset overflow flag
		return (get_error(EA_counter));	}						//Get error and return it.
		
		
		
		
/*********************************************************************************************/
int get_error (unsigned char EA_counter){

int TCNT1_BKP, target_res;

target_res = 7812;																
TCCR1B = 0;														//Halt T1
TCNT1_BKP = TCNT1;												//Copy the value of TCNT1
TCNT1 = 0;														//Clear TCNT1
TCCR1B = 2;														//Get T1 running again ASAP (Note T2 has not stopped running)

if(!EA_counter)return 0;										//Ignore first result to allow for warm up.
else
return (TCNT1_BKP - target_res);}								//return second error result


	