

#include "Plot_Cal_Atmega_header_file.h"


int main (void)

{char User_response;
	
setup_HW;

OSCCAL_WV = OSCCAL;
User_prompt;
sendString("\r\nCalibration plot for Atmega ");
sendString (Device_type[device_ptr]);
newline();

ASSR = (1 << AS2);													//initialise T2 for crystal
Plot_cal();
SW_reset;}




/************************************************************************************************/
void Plot_cal (void){
	unsigned char counter_1;
	long  error_mag;
	Timer_T1_sub(T1_delay_1sec);											//Crystal warm up time

	sei();
	cal_mode = 5;
	Get_ready_to_calibrate;
	
	counter_1 = 0x0E;
	while(counter_1 < 0xF0){
		counter_1 += 1;
		OSCCAL = counter_1; error_mag = compute_error(0,cal_mode,1);

		OSCCAL = OSCCAL_WV;
		Num_to_PC(10,counter_1);
		sendChar('\t');Num_to_PC(10,error_mag);
		newline();
		Timer_T0_sub(delay_2ms);}}



	/**********************************************************************************************************************/
	ISR(TIMER2_OVF_vect) {
		long TCNT1_BKP, overflow = 0, target_res;

		target_res = 62500;
		TCCR1B = 0;																//Halt T1
		TCNT1_BKP = TCNT1;														//Copy the value of TCNT1
		TCNT1 = 0;																//Clear TCNT1
		TCCR1B = 1;																//Get T1 running again ASAP (Note T2 has not stopped running)
		
		if(EA_counter < cal_mode)T1_OVF = 0;									//Ignore first few results
		else
		{
			switch(T1_OVF){
				case 0: overflow = 0; break;
				case 1: overflow = 65536; break;
			case 2: overflow = 131072; break;}
			error_SUM = error_SUM + (TCNT1_BKP - target_res + overflow);
		T1_OVF = 0;}
	EA_counter++;}




	/************************************************************************************************************************************/
	ISR(TIMER1_OVF_vect) {T1_OVF += 1;}


	