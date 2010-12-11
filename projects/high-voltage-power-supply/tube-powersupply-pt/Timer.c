/****************************************************************************
 *
 *  Module:       $HeadURL$
 *  Author:       $Author$
 *  Revision:     $Revision$
 *  Date:         $Date$
 *
 *  Copyright:    (C) 2008 by Dipl.-Ing. Stefan Heesch
 *                address:  Bollstrasse 14, CH-5619 Büttikon, Switzerland
 *                callsign: DB7HS, HB9TWS  
 *                email:    radio@heesch.net
 *
 *  Description:  Timer Module
 *
 ****************************************************************************/

/****************************************************************************
 * Include files
 ****************************************************************************/
 #include "config.h"
 #include "Timer.h"


/****************************************************************************
 *
 * Function PWM_Initialize()
 *
 * Initialize the PWM hardware
 *
 ****************************************************************************/
 void PWM_Initialize()
 {
   /* Set timer 0 to fast PWM ( mode 3 )
    */  
	TCCR0A |=   1 << WGM00;
	TCCR0A |=   1 << WGM01;
    TCCR0B &= ~(1 << WGM02);

   /* clear pin OC0A on compare match, set 0C0A at TOP
    */
	TCCR0A &= ~(1 << COM0A0);
	TCCR0A |=   1 << COM0A1;

   /* clear pin OC0B on compare match, set 0C0B at TOP
    */
	TCCR0A &= ~(1 << COM0B0);
	TCCR0A |=   1 << COM0B1;
 
 
   /* select clock : use system clock, PWM clock is system clock divided by 256
    */	
    TCCR0B |=   1 << CS00;
    TCCR0B &= ~(1 << CS01);
	TCCR0B &= ~(1 << CS02);

   /* enable OC0A
    */
    DDRB |= 1<< PINB2; 

   /* enable OC0B
    */
    DDRD |= 1<< PIND5; 
 
 }




/****************************************************************************
 *
 * Function PWM_Set_A
 *
 * Set PWM value for channel A, value can be in range [0..255]
 *
 ****************************************************************************/
 void PWM_Set_A( unsigned char value )
 {
    OCR0A = value;
 }

 
/****************************************************************************
 *
 * Function PWM_Set_B
 *
 * Set PWM value for channel B, value can be in range [0..255]
 *
 ****************************************************************************/
 void PWM_Set_B( unsigned char value )
 {
    OCR0B = value;
 }
