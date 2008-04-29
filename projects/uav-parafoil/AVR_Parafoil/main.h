#include "global.h"
#include "avrlibdefs.h"
#include "avrlibtypes.h"
#include "matrix.h"
#include "kalman.h"
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
 
#define minutes (float)1.0/600000.0				//gps macros
#define baudrate (int)4800
#define delta_time (float)0.02			//20ms
#define sqrt_delta_time (float)0.141			//kalman filter
#define control_gain (float)90.0
#define damping_constant (float)0.2
 
 
#define I_C 0.0001								//control loop
#define P_C 0.02
#define D_C 0.04
#define integral_limit 0.2/I_C
 
#define read_rate (u08)0b10010100				//gyro specific
#define read_temp (u08)0b10011100
#define read_melexis (u08)0x80
#define mask_word (u16)0x07FF
 
#define gyro_null 1009
 
#define altitudelimit 4000						//flight termination conditions
#define timelimit 3600
 
#define targeteast 2.0							//target waypoint
#define targetnorth 52.0
 
#define Rad2deg 180.0/PI						//bloody obvious
#define Deg2rad PI/180.0
 
#define battery_factor (float)8.0/1024			//for measureing battery voltage
#define battery_chan 0x05						//ADC6
 
#define toggle_pin PIND=0x20					//led on port D.5
 
#define bauddiv  (u16)( (F_CPU/(baudrate*16UL)) -1 )		//baud divider

#define radio_cts bit_is_set(PIND,3)			//hardware, however its wired up

#define led_left PORTC = ((PORTC & ~0x18) | 0x08)
#define led_right PORTC = ((PORTC & ~0x18) | 0x10)

#define undead_man !(PIND&0x10)

#define cutter_on PORTD|=(1<<7)					//release mechanism (hot resistor off mosfet)
#define cutter_off PORTD&=~(1<<7)

#define gyro_off PORTB|=1<<2					//SS line
#define gyro_on PORTB&=~(1<<2)
 
#ifdef ADCSRA
	#ifndef ADCSR
		#define ADCSR   ADCSRA
	#endif
#endif
#ifdef ADATE
	#define ADFR    ADATE
#endif
 
typedef struct
{
	volatile float longitude;
	volatile float latitude;
	volatile float altitude;
	volatile float speed;
	volatile float heading;
	volatile u08 packetflag;							//packetflag lets us see when our packet has been updated
	volatile u08 status;
} gps_type;
 
int put_char(char c, FILE * stream);					//talk to the UART
int get_char( FILE * stream);
