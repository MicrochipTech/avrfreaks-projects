#ifndef _DISPLAY_H
	#define _DISPLAY_H
	#ifdef __GNUC__ 
		#include <avr/io.h>
	#else
		#include <avr/iom16.h>
	#endif
	#include <avr/pgmspace.h>
	#include <util/delay.h>

    void RefreshDisplay(void);
	void write_temp(unsigned int value);
	void InitDisplay(void);
	#define _7SEG_PORT      PORTD
	#define _7SEG_CA_PORT   PORTA

	#define _7SEG_CA_0      1 /* PORTB.1*/
	#define _7SEG_CA_1      0 /* PORTB.0*/
	#define _7SEG_CA_2      2 /* PORTB.2*/
	#define _7SEG_CA_3      0 /* PORTC.0*/

	#define LED_0           0
	#define LED_1           1

	/* On PORTC */
    #define  F           3 /* 'F' SEGMENT IS CONNECTED TO _7SEG_PORT.0 */
	#define  A           2 /* 'A' SEGMENT IS CONNECTED TO _7SEG_PORT.1 */ 

	/* On PORTB */

	#define  G           2 /* 'G' SEGMENT IS CONNECTED TO _7SEG_PORT.2 */
	#define  B           3 /* 'B' SEGMENT IS CONNECTED TO _7SEG_PORT.3 */
	#define  DB          4 /* 'DB'SEGMENT IS CONNECTED TO _7SEG_PORT.4 */
	#define  C           5 /* 'C' SEGMENT IS CONNECTED TO _7SEG_PORT.5 */
	#define  D           6 /* 'D' SEGMENT IS CONNECTED TO _7SEG_PORT.6 */
	#define  E           7 /* 'E' SEGMENT IS CONNECTED TO _7SEG_PORT.7 */

	#define SET_7SEG_CA_0()   PORTB |= (1 << _7SEG_CA_0)
	#define SET_7SEG_CA_1()   PORTB |= (1 << _7SEG_CA_1)
	#define SET_7SEG_CA_2()   PORTB |= (1 << _7SEG_CA_2)
	#define SET_7SEG_CA_3()   PORTA |= (1 << _7SEG_CA_3)

	#define CLR_7SEG_CA_0()   PORTB &= ~(1 << _7SEG_CA_0)
	#define CLR_7SEG_CA_1()   PORTB &= ~(1 << _7SEG_CA_1)
	#define CLR_7SEG_CA_2()   PORTB &= ~(1 << _7SEG_CA_2)
	#define CLR_7SEG_CA_3()   PORTA &= ~(1 << _7SEG_CA_3)

	#define __toggle_led0() (PORTC & (1<<LED_0)) ? (PORTC &= ~(1<<LED_0)) : (PORTC |= (1<<LED_0))
	#define __toggle_led1() (PORTC & (1<<LED_1)) ? (PORTC &= ~(1<<LED_1)) : (PORTC |= (1<<LED_1))

	#define EnableDigit0()   ( SET_7SEG_CA_0() ,\
							   CLR_7SEG_CA_1() ,\ 
							   CLR_7SEG_CA_2() ,\ 
							   CLR_7SEG_CA_3() ) 

	#define EnableDigit1()   ( SET_7SEG_CA_1() ,\
							   CLR_7SEG_CA_0() ,\ 
							   CLR_7SEG_CA_2() ,\ 
							   CLR_7SEG_CA_3() ) 

	#define EnableDigit2()   ( SET_7SEG_CA_2() ,\
							   CLR_7SEG_CA_1() ,\ 
							   CLR_7SEG_CA_0() ,\ 
							   CLR_7SEG_CA_3() ) 

	#define EnableDigit3()   ( SET_7SEG_CA_3() ,\
							   CLR_7SEG_CA_1() ,\ 
							   CLR_7SEG_CA_2() ,\ 
							   CLR_7SEG_CA_0() ) 

#define _disp_delay_time      5 //5 msec between each refresh

#endif

