/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * pjv [at] post.com wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 *
 * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
 * OR IMPLIED. ANY USE IS AT YOUR OWN RISK.
 */

/*  
 * AVRstudio 6.1
 * AVR/GNU C Compiler : 3.4.2
 * ATTiny861A
 * F_CPU = 1000000UL 
 * -Opt=s
 *
 * Author: PjV
 * Version: 1.0
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include "defines.h"
#include <util/delay.h>
#include "main.h"
#include "rgb.h"
#include "adc.h"

/**********************************
* Static functions
**********************************/
static void MainInitIO(void);
static void MainTestIOs(void);
static unsigned char MainReadDelay(void);
static ADC_STATUS_t MainCheckDoor(void);

/**********************************
* Global variables
**********************************/

volatile unsigned int delay_counter_g;
volatile unsigned char ADCnewSampleReady_g;

int main()
{
	STATE_t state = INIT;
	ADC_STATUS_t adc_status = UNKNOWN;
	unsigned int alarm_delay = 0;
	delay_counter_g = 0;
	ADCnewSampleReady_g = FALSE;
	
	MainInitIO();

	alarm_delay = MainReadDelay();
	RGB_InitTimer();
	sei(); 
	MainTestIOs(); //test leds and buzzer
	ADC_Init(); //start first conversion
	//Now the ADC conversion is running
	RGB_RedUpDown();

	while(1)
	{
		switch( state )
		{
			case INIT:	adc_status = MainCheckDoor(); // Switch (Door) should be closed correctly 
						if( adc_status == DOOR_CLOSED_OK ){
							Blue_ON();
							state = DOOR_CLOSED;
						}							
						break;
			
			case DOOR_CLOSED:
						
						if( ADCnewSampleReady_g )	//If new ADC sample available
						{		
							adc_status = ADC_ProcessData();// process adc data
								
							if( adc_status == DOOR_OPEN_ALARM ){
								/* wait delay before start alarm */
								RGB_StartCounting();
								RGB_Off();
								Yellow_ON();
								state = OPENING_ALARM_DELAY;
							}
							else if( adc_status == DOOR_BYPASS_PRESSED ){
								// when door is closed, bypass switch cannot be pressed => ALARM
								RGB_Off();
								state = ALARM;
							}
						}
						break;
			
			case OPENING_ALARM_DELAY:
						
						/* If new ADC sample available */
						if( ADCnewSampleReady_g )
						{ 				
							adc_status = ADC_ProcessData(); 
							/* if alarm switch is bypassed => change state */
							if( adc_status == DOOR_BYPASS_PRESSED ){
								RGB_StopCounting();
								RGB_Off();
								RGB_GreenUpDown();
								state = SWITCH_BYPASS;
							}
						}
							
						/* if delayed time has elapsed => ALARM */
						// timer0 overflow increases counter
						if( delay_counter_g > alarm_delay /* 15 ~ 1sec */ ){	
							cli();
							RGB_Off();
							state = ALARM;
						}							
						break;	
						
			case SWITCH_BYPASS:
			
						if( ADCnewSampleReady_g )
						{		
							adc_status = ADC_ProcessData(); 
							/* if bypass switch is released */
							if( adc_status != DOOR_BYPASS_PRESSED ){
								RGB_StartCounting();
								RGB_Off();
								Yellow_ON();
								state = CLOSING_ALARM_DELAY;
							}
						}
						break;

			case CLOSING_ALARM_DELAY:
			
						if( ADCnewSampleReady_g )	//If new ADC sample is available
						{
							adc_status = ADC_ProcessData(); 	
		
							if( adc_status == DOOR_BYPASS_PRESSED ){
								RGB_StopCounting();
								RGB_Off();
								RGB_GreenUpDown();								
								state = SWITCH_BYPASS;
							}
						}
						// timer0 overflow increases counter
						if( delay_counter_g > alarm_delay /* 15 ~ 1sec */ )	
						{	
							RGB_StopCounting();
								
							if( adc_status == DOOR_CLOSED_OK ){
								RGB_Off();
								Blue_ON();
								RGB_RedUpDown();
								state = DOOR_CLOSED;
							}else{	
								cli();
								RGB_Off();							
								state = ALARM;
							}								
						}	
						break;
							
			case ALARM:	Red_ON();
						/* Do some uproar */
						for( unsigned int ind=0; ind<500; ind++ ){
							TOGGLEBIT(PORTB, BUZZER);
							_delay_us(250);
						}
						Red_OFF();
						_delay_ms(400);
						break;
			default:	break;
		
		}
	}
}

/*********************************************************************************
 * At the start point the door must be closed and bypass-switch open. Otherwise
 * the program do not step forward and the alarm is not turned on.  
 *********************************************************************************/
static ADC_STATUS_t MainCheckDoor(void){

	ADC_STATUS_t return_value = UNKNOWN;

	while(!ADCnewSampleReady_g);// wait ADC conversion
	return_value = ADC_ProcessData(); 	// samples are processed
	
	return return_value;
}

/********************************************************************************
 * test RGB-leds and BUZZER
 *******************************************************************************/
static void MainTestIOs(void){

	Red_ON();
	// do some noise
	for( unsigned int i=1; i<500; i++){
		TOGGLEBIT( PORTB, BUZZER );
		for( unsigned int j=500; j>i; j--){
			asm(""); // compiler should not optimize this loop
		}			
	}
	RGB_Off();
	Yellow_ON();	
	_delay_ms(500);
	RGB_Off();
	Green_ON();
	_delay_ms(500);
	RGB_Off();
	Blue_ON();		
	_delay_ms(500);
	RGB_Off();		// dim
}

/*********************************************************************************
 * Read rotary switch value => alarm delay. 
 * Return: alarm_delay factor
 *********************************************************************************/
static unsigned char MainReadDelay(void){

	unsigned char byte = 0;
	unsigned char returnvalue = 0;
	
	byte = ~PINA; //inverted
	byte >>= 4;
	
	switch (byte)
	{
		/* Timer0 8-bit Overflow increases delay_counter_g in ~65.5ms period (1us*256prescaler (1MHz clock)).
		   Timer0 8-bit Overflow increases delay_counter_g in ~260ms period (1us*1024prescaler (1MHz clock)).	
		   ex. 65.5ms*15= ~1 sec. 
		*/

		case 0: returnvalue = 0;  /* no delay */  
				break;
		case 1: returnvalue = 15; /* 1 sec */
				break;
		case 2: returnvalue = 30; /* 2 sec */
				break;
		case 3: returnvalue = 45; /* 3 sec */
				break;
		case 4: returnvalue = 60;
				break;
		case 5: returnvalue = 75;
				break;
		case 6: returnvalue = 90;
				break;
		case 7: returnvalue = 105;
				break;
		case 8: returnvalue = 120;
				break;
		case 9: returnvalue = 135;
				break;
		case 10: returnvalue = 150;
				break;
		case 11: returnvalue = 165;
				break;
		case 12: returnvalue = 180; 
				break;
		case 13: returnvalue = 195; 
				break;
		case 14: returnvalue = 210; 
				break;
		case 15: returnvalue = 225; /* 15 sec */
				break;
		default: returnvalue = 0; /* no delay */
				break;
	}
	return( returnvalue );
}

/*********************************************************************************
 ** Note! Initialize PWMs as output
 ** CKDIV8 fuse is programmed => fosc=1MHz
 *********************************************************************************/
static void MainInitIO(void){

	/*
	0 - ADC alarm
	1 - NC input pulled up
	2 - NC input pulled up
	3 - NC input pulled up
	4 - Rotary switch '1', input pulled up
	5 - Rotary switch '2', input pulled up
	6 - Rotary switch '4', input pulled up
	7 - Rotary switch '8', input pulled up
	*/	
	DDRA = 0b00000000; // Port A initialization IN,
	PORTA = 0b11111110; // pins hi, except adpin0 

	/*
	0 - MOSI, input pulled up
	1 - MISO/PWM-Red, output low - led off
	2 - CLK, input pulled up
	3 - PWM-Green, output low - led off
	4 - NC, input pulled up
	5 - PWM-Blue, output low - led off
	6 - Buzzer, output low
	7 - RST, input pulled up
	*/	
	DDRB  = 0b01101010; // 1-out
	PORTB = 0b10010101; // out -> low, leds off
}
