/*
 * Simple traffic light controller (italian system) with ATtiny2313
 * (c) 2008, http://telpar.altervista.org/                       
 * Francesco Parisi <fparisi [at] gmail.com>                      
 * 
 * HW: myAVR USB programmer (avr911) + breadboard
 * SW: AVRstudio 4.12 + WinAVR 
 *
 * This file is provided AS IS with no warranty and only for educational purposes
*/

/******************* (  EXTERNAL, PUBLIC, INTERFACE ) ****************/

#ifndef F_CPU
#define F_CPU 4430000UL /* We use a clock of 4,43MHz */
#endif

#include <avr/io.h>
#include <util/delay.h>    /* contains _delay_ms() */

/* Def. PIN register where is connected the switch for the selection of mode (night or day) */
#define PINSW PORTD
#define SW 2            // PD2 

/* Def. PORT register and relative DDR register where are connected the LED  */
#define PORTALED PORTB  // Porta dati dove sono collegati i LED
#define REGDIRLED DDRB  // Registro direzione della porta dei LED

#define LEDR1 5 // Def. Pin of PORTALED where is connected the red LED dir. 1
#define LEDY1 6 // Def. Pin of PORTALED where is connected the yellow LED dir. 1
#define LEDG1 7 // Def. Pin of PORTALED where is connected the green LED dir. 1

#define LEDR2 4 // Def. Pin of PORTALED where is connected the red LED dir. 2
#define LEDY2 3 // Def. Pin of PORTALED where is connected the yellow LED dir. 2
#define LEDG2 2 // Def. Pin of PORTALED where is connected the green LED dir. 2

/* Bitmask of LED out - Lights direction 1 (i.e. NS and SN) */
#define R1 (1<<LEDR1)
#define Y1 (1<<LEDY1)
#define G1 (1<<LEDG1)

/* Bitmask of LED out - Lights direction 2 (i.e. OE and EO) */
#define R2 (1<<LEDR2)
#define Y2 (1<<LEDY2)
#define G2 (1<<LEDG2)

/* Time delays definitions */
#define DELTAT   1000         /* Minimum time delay in ms */
#define T	     16*DELTAT    /* Total period: must alwasy equal at 16*DELTAT */

#define TG1      5*DELTAT     /* Time delay of green dir.1: 5...12*DELTAT */
#define TY1      DELTAT       /* Time delay of yellow dir.1 : 1*DELTAT */
#define TR1      T-(TG1+TY1)  /* Time delay of red dir.1 */

#define TY2      TY1          /* Time delay of yellow dir.2 (is equal to yellow dir.1) */
#define TG2      TR1-TY2      /* Time delay of green dir.2: 5...12 times DELTAT */
#define TR2      TG1+TY1      /* Time delay of red dir.2 */

#define TN		 DELTAT		  /* Time delay of yellow blinking (night mode) */


/* Configure the I/O ports */
void port_init (void);

/* Lights initialize after reset: all lamps to red */
void light_init(); 

/* Working mode on day */
void day();

/* Working mode on night: blinks only the yellow lamps */
void night();

/************************************ (  USER PROGRAM ) **************************************/

int main(void) {

    /* I/O ports configuration */
    port_init();

    /* Initialize the lights */
    light_init();
 
    /* Infinite loop */ 

    while (1) {	

	   if bit_is_clear(PINSW, SW) 
	      day();
       else
	      night();
    
	}

}	


/******************************** ( PRIVATE SECTION, IMPLEMENTATION, DEFINITIONS ) ****************************/


///////////////////////////////////////////////////////////////////////////////////////////

void port_init (void) {

	/* Configure the pin connected to the LED as out pin */
	REGDIRLED |= (R1|Y1|G1|R2|Y2|G2); 
 
    
}

///////////////////////////////////////////////////////////////////////////////////////////

void light_init() {
   
   /* After reset, power on only the two reds, for TR2 ms  */
   PORTALED = (R1|R2);
   _delay_ms (TR2);
}

///////////////////////////////////////////////////////////////////////////////////////////

void day() {
     
	 	/* Power on RED2 and GREEN1 for TG1 ms */
		PORTALED = (R2|G1);
		_delay_ms (TG1);	
	
		/* Power on YELLOW1 and GREEN2 for TY1 ms */
		PORTALED = (Y1|R2);
		_delay_ms (TY1);	
	
		/* Power on RED1 and GREEN2 for TG2 ms */
		PORTALED  = (R1|G2);
		_delay_ms (TG2);		
	
		/* Power on RED1 and YELLOW2 for TY2 ms */
		PORTALED  = (R1|Y2);
		_delay_ms (TY2);	
 }

///////////////////////////////////////////////////////////////////////////////////////////

void night() {

        /* Power on YELLOW1 and YELLOW2 */
        PORTALED = (Y1|Y2);
		_delay_ms (TN);
		
		/* Power off all the lights */
		PORTALED = 0;
		_delay_ms (TN);
}
///////////////////////////////////////////////////////////////////////////////////////////
