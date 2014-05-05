/* defining basic constraints for project */
#define DEFINES_H

#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

/* CPU frequency */
#define F_CPU 16000000UL

/* UART baud rate */
#define UART_BAUD 38400U
#define BAUD UART_BAUD

/* Timer 1 prescaler 10ms @ 16MHz*/
#define Timer1_prescale_default 64U

#define INCLUDE_RTC
 #define PRODUCTION_BOARD
/* #define HEARTBEAT_LED */
/* #define PANMUFINEK */

/* mask definitions for flagi2 byte (program flags) */
#define COLUMN_IS_DELTA_FLAG		0x80
#define HEAD_IS_DELTA_FLAG		0x40
#define LIQUID_AS_PERCENT_FLAG		0x20
#define	ALARM_1_CLEARED_FLAG		0x10
#define	ALARM_2_CLEARED_FLAG		0x08
#define	ALARM_3_CLEARED_FLAG		0x04
#define	ALARM_4_CLEARED_FLAG		0x02
#define	COLUMN_ALARM_ENABLED_FLAG	0x01

/* mask definitions for flagi byte (program flags) */
#define	HEAD_ALARM_ENABLED_FLAG		0x80
#define MS500_FLAG			0x40
#define S1_FLAG				0x20
#define	FLAG_SERIAL_RX_DATA		0x10
#define	FLAG_SERIAL_TX_TO_SEND		0x08
#define MS100_FLAG 			0x04
#define	CONFIG_CLEARED_FLAG		0x02
#define IN_CONFIG_FLAG			0x01

#define	SOFT_KEY_1_MASK			0x01
#define	SOFT_KEY_2_MASK			0x02
#define	SOFT_KEY_3_MASK			0x04
#define	SOFT_KEY_4_MASK			0x08
#define	SOFT_KEY_5_MASK			0x10
#define	SOFT_KEY_6_MASK			0x20
#define SOFT_KEY_3_OLD_MASK		0x40
#define SOFT_KEY_4_OLD_MASK		0x80

/* in SOFT_KEYS2 */
#define	SOFT_KEY_1_OLD_MASK		0x01
#define	SOFT_KEY_2_OLD_MASK		0x02
#define	SOFT_KEY_5_OLD_MASK		0x04
#define	SOFT_KEY_6_OLD_MASK		0x08
#define	SOFT_KEY_IMP_MASK		0x10
#define	SOFT_KEY_IMP_OLD_MASK		0x20


#if defined PRODUCTION_BOARD 
    #define	KEY_1_PORT			PORTC
    #define	KEY_1_PINR			PINC
    #define	KEY_1_BIT			PC4
    #define	KEY_2_PORT			PORTC
    #define	KEY_2_PINR			PINC
    #define	KEY_2_BIT			PC0
    #define	KEY_3_PORT			PORTB
    #define	KEY_3_PINR			PINB
    #define	KEY_3_BIT			PB5
    #define	KEY_4_PORT			PORTB
    #define	KEY_4_PINR			PINB
    #define	KEY_4_BIT			PB4
    #define	KEY_5_PORT			PORTB
    #define	KEY_5_PINR			PINB
    #define	KEY_5_BIT			PB3
    #define	KEY_6_PORT			PORTB
    #define	KEY_6_PINR			PINB
    #define	KEY_6_BIT			PB2

    #define	ENCODER_PLUS_PORT		PORTC
    #define	ENCODER_PLUS_PIN		PINC
    #define	ENCODER_PLUS_BIT		PC1
    #define	ENCODER_MINUS_PORT		PORTC
    #define	ENCODER_MINUS_PIN		PINC
    #define	ENCODER_MINUS_BIT		PC3
    #define	ENCODER_SWITCH_PORT		PORTC
    #define	ENCODER_SWITCH_PIN		PINC
    #define	ENCODER_SWITCH_BIT		PC2
    /* LED / buzzer definitions */
    #define	LED_1_PORT			PORTB
    #define	LED_1_BIT			PB1
    #define	LED_1_DDR			DDRB
#else
    /* key definitions to make code portable */
    #define	KEY_1_PORT			PORTB
    #define	KEY_1_PINR			PINB
    #define	KEY_1_BIT			PB0
    #define	KEY_2_PORT			PORTD
    #define	KEY_2_PINR			PIND
    #define	KEY_2_BIT			PD7
    #define	KEY_3_PORT			PORTD
    #define	KEY_3_PINR			PIND
    #define	KEY_3_BIT			PD2
    /* LED / buzzer definitions */
    #define	LED_1_PORT			PORTB
    #define	LED_1_BIT			PB1
    #define	LED_1_DDR			DDRB
    #define	LED_2_PORT			PORTB
    #define	LED_2_BIT			PB2
    #define	LED_2_DDR			DDRB
#endif

/* mask definitions for flags in eeprom byte */
#define	LCD_OPERATION_MODE_FLAG		0x80
#define	LCD_4LINE_FLAG			0x40

#define LCD_DEFAULT_FLAGS		LCD_4LINE_FLAG

/* error codes definitions */
#define	EEPROM_DATA_CORRUPTED		1
#define NOT_IMPLEMENTED			2

/*-------------------------------------------------------------*/
