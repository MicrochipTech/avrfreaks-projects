
/****************************************************************************
 Title  :   C  include file for the SRF04 FUNCTIONS library (srf04.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      20/Aug/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/
/* The driver uses timer0 */

#ifndef SRF04_H
#define SRF04_H   1

/*############################################################################################*/
/* CONFIGURATION BLOCK STARTS HERE. Change these definitions to adapt setting */
/*############################################################################################*/

#ifndef  F_CPU
#define  F_CPU                   3686400
#endif
#define SPEED_OF_SOUND           347260L      /* SPEED OF SOUND @ 26 degrees Celsius (in millimeters)  */

#define SRF04_UNITS_USED         1            /*  1= SRF04 unit 0 is active */


/* ALL THE SRF04 PORTS MUST BE BIDIRECTIONAL! */
/* IF YOU PLAN TO USE ONE PORT FOR ALL SRF04 UNITS JUST EDIT THE BELOW LINE */
#define SRF04_UNITS_PORT         D            

#define SRF04_UNIT_0_PORT        SRF04_UNITS_PORT
#define SRF04_UNIT_0_PING_PIN    0
#define SRF04_UNIT_0_ECHO_PIN    1

#define SRF04_UNIT_1_PORT        SRF04_UNITS_PORT
#define SRF04_UNIT_1_PING_PIN    2
#define SRF04_UNIT_1_ECHO_PIN    3

#define SRF04_UNIT_2_PORT        SRF04_UNITS_PORT
#define SRF04_UNIT_2_PING_PIN    4
#define SRF04_UNIT_2_ECHO_PIN    5

#define SRF04_UNIT_3_PORT        SRF04_UNITS_PORT
#define SRF04_UNIT_3_PING_PIN    6
#define SRF04_UNIT_3_ECHO_PIN    7

/*############################################################################################*/
/*                            CONFIGURATION BLOCK ENDS HERE.                                  */
/*############################################################################################*/
/* Keyword definitions */

#define SRF04_ALL  0
#define SRF04_0    1
#define SRF04_1    2
#define SRF04_2    3
#define SRF04_3    4



/* Function declarations */

extern void         srf04_init(void);
#if SRF04_UNITS_USED == 1
extern unsigned int srf04_ping(void);                      /* returns the range in centimeters */
#elif SRF04_UNITS_USED > 1
extern unsigned int srf04_ping(unsigned char srf04_unit); /* returns the range in centimeters */
#endif

/* Macro definitions */




#endif  /* #ifndef SRF04_H */
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

       

