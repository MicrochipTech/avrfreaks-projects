/*
 * macros.h
 *
 * Created: 11.4.2014 11:48:49
 *  Author: PjV
 *
 * Version: 1.0 
 */ 

#ifndef MACROS_H_
#define MACROS_H_ 


/**
 * Send debug messages to usart? 
 */
#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG 1

#if DEBUG
#pragma message "DEBUG ON"
#else
#pragma message "DEBUG Turned OFF"
#endif

/**
 * Use this if you want to configure the u-blox NEO-6M GPS-module, 
 * otherwise the module to use the default settings.
 * Do not use if you have a different type of module as NEO6-6M.
 */
#define USE_NEO6_GPS_MODULE	1

#if USE_NEO6_GPS_MODULE
#pragma message "Using NEO6 GPS Module"
#else
#pragma message "Not configure GPS"
#endif

#undef F_CPU
#define F_CPU		16000000UL

/**
 * rename USART1 RX-Complete vector 
 */
#define GPS_NMEA_RX_Complete	USART1_RX_vect

/** 
 * Usart1 baudrate - u-blox neo-6 GPS-module default baudrate 38.4k 
 */
#define GSMRATE 38400UL
#define GSM_BAUDRATE (F_CPU/(GSMRATE*16)-1)
/**
 * Usart0 baudrate - defined for debug messages 
 */
#define DEBUGRATE 38400UL
#define DB_BAUDRATE (F_CPU/(DEBUGRATE*16)-1)

enum BOOLEAN { FALSE, TRUE };

#define SETBIT(ADDRESS,BIT)		(ADDRESS |= (1<<BIT))
#define CLEARBIT(ADDRESS,BIT)	(ADDRESS &= ~(1<<BIT))
#define TOGGLEBIT(ADDRESS,BIT)	(ADDRESS ^= (1<<BIT))

/*Check Pin Change ISR for pushbutton in pcint.c*/
#define BUTTON_PRESSED		((PINA & 0b01000000)==0)
#define GREEN_LED_PORT		PORTC
#define YELLOW_LED_PORT		PORTA
#define GREEN_LED_PIN		7
#define YELLOW_LED_PIN		7
#define GREEN_LED_ON	( PORTC &= ~(1<<GREEN_LED_PIN))
#define GREEN_LED_OFF	( PORTC |= (1<<GREEN_LED_PIN))
#define YELLOW_LED_ON	( PORTA &= ~(1<<YELLOW_LED_PIN))
#define YELLOW_LED_OFF	( PORTA |= (1<<YELLOW_LED_PIN))

/*NOTE! Check sd/mmc pin mapping! Configure SPI pins in sd_raw_config.h*/
#define SELECT_CARD		PORTC &= ~(1 << PORTC6)
#define UNSELECT_CARD	PORTC |= (1 << PORTC6)

#if defined(__AVR_ATmega644P__)
#warning "check sd/mmc pin mapping!"
#endif

#endif /* MACROS_H_ */