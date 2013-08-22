/*
 * main.h
 *
 * Created:
 * Author: PjV
 */ 

#ifndef GARAGE_H
#define GARAGE_H

/* Program states */
typedef enum
{
	INIT,
	DOOR_CLOSED,
	SWITCH_BYPASS,
	OPENING_ALARM_DELAY,
	CLOSING_ALARM_DELAY,
	ALARM
}STATE_t;

/* ADC pin reads status of switch sensor */
typedef enum
{
	UNKNOWN,
	DOOR_OPEN_ALARM,	/* ADC voltage ~0V */
	DOOR_CLOSED_OK,		/* ADC voltage ~1V */
	DOOR_BYPASS_PRESSED		/* ADC voltage ~4.75 */
}ADC_STATUS_t;


/****************************************
* Extern Global variables
****************************************/

/* ADC */
extern volatile unsigned char ADCnewSampleReady_g;

//Timers, comparators match values
extern volatile unsigned int delay_counter_g;

#endif /* GARAGE_H */ 
