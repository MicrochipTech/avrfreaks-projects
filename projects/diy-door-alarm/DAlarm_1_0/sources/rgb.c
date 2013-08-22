/*
 * rgb.c
 *
 * Created:
 * Author: PjV
 * Version: 1.0 
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "defines.h"
#include <util/delay.h>

#include "main.h"
#include "rgb.h"

static unsigned char red_comp_value;
static unsigned char grn_comp_value;
static unsigned char blu_comp_value;

static volatile unsigned char red_step;
static volatile unsigned char grn_step;
static volatile unsigned char blu_step;

/*********************************************************************************
 * 
 * Timer1 is used to generate PWM waveform
 *********************************************************************************/
void RGB_InitTimer(void){

	red_comp_value = RED_CMP_MATCH;
	grn_comp_value = GRN_CMP_MATCH;
	blu_comp_value = BLU_CMP_MATCH;
	red_step = 0;
	grn_step = 0;
	blu_step = 0;

    /*Timer/counter1 (8-bit)****************************************/
	
	PLLCSR = 0X00; // default
	
	// COM1A[1:0] 0b10, Cleared on Compare Match when up-counting, 
	// Set on Compare Match when down-counting. /OCpin\ disconnected
	// COM1B[1:0] 0b10, Cleared on Compare Match when up-counting, 
	// Set on Compare Match when down-counting. /OCpin\ disconnected
	TCCR1A |=	(1<<COM1A1)|(1<<COM1B1);
	//TCCR1A |=	(1<<PWM1A);		// Red on
	//TCCR1A |=	(1<<PWM1B);		// Green on
	
	/* Select PWM frequency */			
	//TCCR1B |= (1<<CS12);				//Clock Select 8 prescaler
	//TCCR1B |= (1<<CS12)|(1<<CS10);	//Clock Select 16 prescaler 
	TCCR1B |= (1<<CS12)|(1<<CS11);		//Clock Select 32 prescaler 
	//TCCR1B |= (1<<CS13)|(1<<CS10);	//Clock Select 256 prescaler 
	
	TCCR1C |= (1<<COM1D1);		//d-channel, same configurations as a/b-channels above
	//TCCR1C |= (1<<PWM1D);		//blue on, 
	
	//TCCR1D |= (1<<WGM10);		//Phase & Frequency Correct PWM
	TCCR1E = 0x00;				//default
	
	TC1H = 0X00;	// clears
	TCNT1 = 0X00;	// clears
	
	OCR1A = red_comp_value;		// compare this...
	OCR1B = grn_comp_value;		// compare this...
	OCR1D = blu_comp_value;		// compare this...
	
	DT1 = 0X00; 			// Dead Time Value, default
	
	TIFR  = 0xff;	// Clear pending interrupts 
	TIMSK |= (1<<TOIE0);// |		/* Enable Overflow interrupts timer0 */ 
						  //(1<<OCIE1A)|(1<<OCIE1B)|(1<<OCIE1D);	/* Enable PWM-output Compare int A/B/D */
}

/********************************************************************************
 * 
 ********************************************************************************/
void RGB_Off(void){

	TCCR1A &= ~(1<<PWM1A);	/*red off*/
	TCCR1A &= ~(1<<PWM1B);	/*green off*/
	TCCR1C &= ~(1<<PWM1D);	/*blue off*/
	
	TIMSK &= ~(1<<OCIE1A);	/*Red, CCA interrupt disabled */
	TIMSK &= ~(1<<OCIE1B);	/*Green, CCB interrupt disabled */
	TIMSK &= ~(1<<OCIE1D);	/*Blue, CCD interrupt disabled */
	
	OCR1A = RED_CMP_MATCH;	/*reload default value*/
	OCR1B = GRN_CMP_MATCH;	/*reload default value*/
	OCR1D = BLU_CMP_MATCH;	/*reload default value*/
}
void RGB_RedUpDown(void){
	red_step = 0;			//down to up 
	red_comp_value = 0;		//down to up
	OCR1A = red_comp_value;
	Red_ON();				//start PWM
	TIMSK |= (1<<OCIE1A);	/* red on, CCA interrupt enabled */
}
void RGB_GreenUpDown(void){
	grn_step = 0;			//down to up
	grn_comp_value = 0;		//down to up
	OCR1B = grn_comp_value;
	Green_ON();				//start PWM
	TIMSK |= (1<<OCIE1B);	/* red on, CCA interrupt enabled */	
}
void RGB_BlueUpDown(void){
	blu_step = 0;
	blu_comp_value = 0;
	OCR1D = blu_comp_value;
	Blue_ON();				// start PWM
	TIMSK |= (1<<OCIE1D);	/* blue on, CCD interrupt enabled */
}


/*****************************************
 * Timer/Counter1, Ascending/descending Red
 ****************************************/
ISR( TIMER1_COMPA_vect )
{
	
	switch( red_step )
	{
		case 0:	OCR1A = red_comp_value++;
				if (red_comp_value == 60){
					red_step = 1;
				}
				break;
				
		case 1: OCR1A = --red_comp_value;
				if (red_comp_value == 0){
					red_step = 0;
				}
				break;
		default:break;
	}	
	
	
	/* here is another way */
	//static unsigned char delay=0;
	//
	//switch (red_step)
	//{
		///* dim to lit */
		//case 0: OCR1A = red_comp_value++;
				//red_step = 2;
				//if (red_comp_value == 50){
					//red_step = 1;	
				//}
				//break;
		///* lit to dim */
		//case 1: OCR1A = --red_comp_value;
				//red_step = 3;
				//if (red_comp_value == 0){
					//red_step = 4;
				//}			
				//break;
		///* delays */
		//case 2:	if (delay == 7){
					//red_step = 0;
					//delay = 0;
				//}else delay++;
				//break;
				//
		//case 3:	if (delay == 7){
					//red_step = 1;
					//delay = 0;
				//}else delay++;
				//break;
//
		//case 4:	if (delay == 255){
					//red_step = 0;
					//delay = 0;
				//}else delay++;
				//break;
			//
		//default:break;
	//}
}

/*****************************************
 * Timer/Counter1  
 * Green LED, Ascending/descending 
 ****************************************/
ISR( TIMER1_COMPB_vect ){
	
	switch ( grn_step )
	{
		case 0:	OCR1B = grn_comp_value++;
				if (grn_comp_value == 60){
					grn_step = 1;
				}
				break;
		case 1: OCR1B = --grn_comp_value;
				if (grn_comp_value == 0){
					grn_step = 0;
				}
				break;
		default:break;
	}
}

/*****************************************
 * Timer/Counter1, Blue 
 ****************************************/
ISR( TIMER1_COMPD_vect )
{
	if ( blu_step == 10 ){
		OCR1D = blu_comp_value++;
		blu_step = 0;
	}
	else {
		blu_step++;
	}	
}



/********************************************************************************
 * Timer0 counting alarm delay in seconds. Timer0 Overflow interrupt is used. 
 * 
 * Delay functions.
 *
 * StartCounting/StopCounting. 
 * 
 ********************************************************************************/
void RGB_StartCounting(void){
	TCNT0L = 0x00;			// Clear counter0
	TCNT0H = 0x00;			// Clear counter0
	
	//TCCR0B |= (1<<CS00); 				// prescaler 1, turn on
	//TCCR0B |= (1<<CS01); 				// prescaler 8, turn on
	//TCCR0B |= (1<<CS01) | (1<<CS00); 	// prescaler 64, turn on
	TCCR0B |= (1<<CS02); 				// prescaler 256, turn on
	//TCCR0B |= (1<<CS02) | (1<<CS00); 	// prescaler 1024, turn on
	delay_counter_g = 0;
}

void RGB_StopCounting(void){
	TCCR0B &= ~(1<<CS02);// & ~(1<<CS01) & ~(1<<CS00); 	// turn off
	delay_counter_g = 0;	
}
/********************************************************************************
 * Timer/Counter0 Overflow for Delay counting
 ********************************************************************************/
 ISR( TIMER0_OVF_vect ){
		delay_counter_g++;
}