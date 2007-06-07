
/*********************************************************************************************************
 Title  :   Header file for the rc_channel.c library (rc_channel.h)
 Author:    Chris Efstathiou 
 E-mail:    ........................
 Homepage:  ........................
 Date:      02/Feb/2006
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE.

*********************************************************************************************************/

/*
 The driver for remote control signal decoding use timer 0 and/or External Interupt 0 
 This driver is made for the ATtiny26 so some modifications might be needed for other devices
 If the external Interupt is used it must be capable of level change interupt generation.
 The non interupt driven driver can be used with any Port pin.   
*/
 

#ifndef RC_CHANNEL_H
#define RC_CHANNEL_H

/*######################################################################################################*/
/*                              START OF CONFIGURATION BLOCK                                            */
/*######################################################################################################*/
/*
 The driver for remote control signal decoding use timer 0 and/or External Interupt 0 
 This driver is made for the ATtiny26 so some modifications might be needed for other devices
 If the external Interupt is used it must be capable of level change interupt generation.
 The non interupt driven driver can be used with any Port pin.
 The driver is usefull in making rc stuf like ESCs or anything controlled by an rc receiver.
 The driver now works very well and i am happy with it.
*/
   
#ifndef F_CPU
#define F_CPU                       8000000        /* CPU CLOCK FREQUENCY */
#endif

#define RC_NOMINAL_PULSE_WIDTH      1500  /* in microseconds */
#define RC_MIN_PULSE_WIDTH           800  /* in microseconds */
#define RC_MAX_PULSE_WIDTH          2200  /* in microseconds */
#define RC_PULSE_WIDTH_TIMEOUT      30000 /* in microseconds, max 65535 */
#define RC_CHANNEL_SAMPLES          3     /* How many samples to take for a valid level change. */
#define RC_PW_MIN_CODE_SIZE         0     /* Less size but no waveform filtering. */
#define RC_PW_INT_DRIVEN            1     /* If set to 1 the driver uses external interupt 0 */
/* The below definitions are active only in the interupt mode. */
#define RC_PW_INT_PIN_REG           PINB  /* The Externals interupt pin register. */ 
#define RC_PW_INT_PIN               PB6   /* The external interupt port pin. */
#define RC_PW_AVERAGING_SAMPLES     3     /* How many pulses to use for the averaging filter. */

/*######################################################################################################*/
/*                             END OF CONFIGURATION BLOCK                                               */
/*######################################################################################################*/ 

/********************************************************************************************************/
/*                             PREPROCESSOR DIRECTIVES                                                  */
/********************************************************************************************************/





/********************************************************************************************************/
/*                             PUBLIC FUNCTION PROTOTYPES                                               */
/********************************************************************************************************/
#if RC_PW_INT_DRIVEN == 0

unsigned short _rc_get_pw_(volatile unsigned char *port, unsigned char pin);

#elif RC_PW_INT_DRIVEN == 1

unsigned short rc_get_pw(void);

#endif

unsigned char get_error_count(void);


/********************************************************************************************************/
/*                                 MACRO DEFINITIONS                                                    */
/********************************************************************************************************/
#if RC_PW_INT_DRIVEN == 0

#define  rc_get_pw(port, pin)  _rc_get_pw_(&(port), pin)

#endif

#endif  /* RC_CHANNEL_H    */

/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

