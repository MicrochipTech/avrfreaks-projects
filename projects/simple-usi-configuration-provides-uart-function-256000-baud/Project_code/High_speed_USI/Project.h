

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>




//Baud rate set for 38400: replace 38400 with wanted baud rate

#define Tx_clock						Tx_clock_38400
#define Rx_clock						Rx_clock_38400
#define Start_clock						Start_clock_38400
#define Half_Rx_clock					Half_Rx_clock_38400

char buffer_size = 47;					//User can set this value to maximum string length to be sent from PC


//Settings for ATtiny44 and 84 devices
/******************************************************************************************
#define DI_pin				6
#define DO_pin				5
#define USCK_pin			4
#define setup_IO			setup_IO_44
******************************************************************************************/



//Settings for ATtiny461 and 861 devices
/******************************************************************************************/
#define TIFR0				TIFR
#define TCNT0				TCNT0L
#define WDTCSR				WDTCR
#define TIFR1				TIFR
#define DI_pin				0
#define DO_pin				1
#define USCK_pin			2
#define setup_IO			setup_IO_461
/******************************************************************************************/



#define DDR_USI				DDRA
#define PIN_USI				PINA
#define PORT_USI			PORTA

#define WPU_on_DI_pin \
DDR_USI &= (!(1 << DI_pin));\
PORT_USI |= 1 << DI_pin;

#define Configure_DO_pin_as_Output \
DDR_USI |= (1 << DO_pin);

#define start_bit \
!(PIN_USI & (1 << DI_pin))

#define DI_paused \
(PIN_USI & (1 << DI_pin))

#define WPU_on_DO_pin \
DDR_USI &= (~(1 << DO_pin));



#include "../Resources/USI_header.h"
#include "../Resources/USI_subroutines.c"
#include "../Resources/IO_subroutines.c"
#include "../Resources/Text_reader_subs.c"

const char message_1[] PROGMEM = "\r\n\r\nProgram running on ";
const char message_2[] PROGMEM = "\r\nEcho single keypresses: Return key to escape.\r\n";
const char message_3[] PROGMEM = "\r\nSend strings to USI\r\n";
const char message_4[] PROGMEM = "\r\nNo text!\r\n";
const char message_5[] PROGMEM = "\r\nReading strings.";
const char message_6[] PROGMEM = "\r\nString number? 0 to exit";
const char message_7[] PROGMEM = "\r\nNo string!\r\n";