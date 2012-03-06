/**************************************************************
/
/			DEVBOARD.H
/	
/	Definitions for the devices on board
/	
/
**************************************************************/

#ifndef DEV_BOARD_H
#define DEV_BOARD_H

#include<avr/io.h>


#define _COMBINE(a,b) a##b

#define PORT(x) _COMBINE(PORT,x)
#define DDR(x) _COMBINE(DDR,x)
#define PIN(x) _COMBINE(PIN,x) 
	
	
/*		LEDs 	*/

#define LED1_PORT D		//LED-1 port and position
#define LED1_POS 5
#define LED2_PORT B		//LED-2 port and position
#define LED2_POS 3

/*		SWITCHES		*/

#define SW1_PORT A		//Switch-1 
#define SW1_POS 2
#define SW2_PORT A		//Switch-2
#define SW2_POS 3

#define SW_ON 0		//active low switch ( 1 for active high)


/*	LDR	*/
#define LDR_PORT A
#define LDR_POS 0
#define LDR_CHANNEL LDR_POS

/*	LM35 Temperature Sensor	*/
#define LM35_PORT A
#define LM35_POS 1
#define LM35_CHANNEL LM35_POS 


/*	STATUS	AND CONTROL MACROS	*/
#define SW1_STATUS ( PIN(SW1_PORT) & (1<<SW1_POS) )
#define SW2_STATUS ( PIN(SW2_PORT) & (1<<SW2_POS) )

#define led1_on() ( PORT(LED1_PORT) |= (1<<LED1_POS) )
#define led1_off() ( PORT(LED1_PORT) &= ~(1<<LED1_POS) )
#define led2_on() ( PORT(LED2_PORT) |= (1<<LED2_POS) )
#define led2_off() ( PORT(LED2_PORT) &= ~(1<<LED2_POS) )


/*		FUNCTIONS		*/

void devboard_init(void);

#endif