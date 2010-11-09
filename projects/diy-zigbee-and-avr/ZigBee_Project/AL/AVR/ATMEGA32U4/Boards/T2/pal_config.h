/**
 * @file pal_config.h
 *
 * @brief PAL configuration for AT90USB1286
 *
 * This header file contains configuration parameters for AT90USB1286.
 *
 * $Id: pal_config.h 20009 2010-01-25 17:07:05Z sschneid $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2009, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
 // 9/18/2010  Modified from Teensy2++ version for Teensy2.    -jkl
 //    - changed the LED numbers.
 // Note T2PP change to undo!!!!!
 // 9/13/2010  Modified setting of SPI clock to slow it down.   -jkl
 // Need to speed back up!!!!!
 // 9/20/2010  Made clock source in awake same as in sleep.   -jkl
 

/* Prevent double inclusion */
#ifndef PAL_CONFIG_H
#define PAL_CONFIG_H

/* === Includes =============================================================*/

#include "pal_boardtypes.h"

#if (BOARD_TYPE == T2)

/*
 * This header file is required since a function with
 * return type retval_t is declared
 */
#include "return_val.h"

/* === Types ================================================================*/

typedef enum port_type_tag
{
    PORT_A,
    PORT_B,
    PORT_C,
    PORT_D,
    PORT_E,
    PORT_F,
    PORT_G
} port_type_t;

typedef enum port_pin_direction_type_tag
{
    DDR_A,
    DDR_B,
    DDR_C,
    DDR_D,
    DDR_E,
    DDR_F,
    DDR_G
} port_pin_direction_type_t;

/* Enumerations used to idenfify LEDs */
typedef enum led_id_tag
{
    LED_0,
    LED_1,
    LED_2,
    LED_3
} led_id_t;

#define NO_OF_LEDS                      (4)


/* Enumerations used to idenfify buttons */
typedef enum button_id_tag
{
    // Dummy button required for the type definition, but not existent
    BUTTON_NOT_EXISTENT
} button_id_t;

#define NO_OF_BUTTONS                   (0)

/* === Externals ============================================================*/


/* === Macros ===============================================================*/

/** The default CPU clock */
#define F_CPU                   (8000000UL)

/*
 * IRQ macros for AT90USB1286
 */

/*
 * AT86RF230:
 *
 * TRX_MAIN_IRQ_HDLR_IDX       TRX interrupt mapped to MCU input capture pin
 * TRX_TSTAMP_IRQ_HDLR_IDX       Not used
 */

/* Number of used TRX IRQs in this implementation */
#define NO_OF_TRX_IRQS                  (1)

/* Mapping of TRX IRQs to MCU pins */
#define TRX_IRQ                         (TIMER1_CAPT_vect)

/** Enables the transceiver interrupts. */
#define ENABLE_TRX_IRQ(trx_irq_num)     do {    \
    if (trx_irq_num == TRX_MAIN_IRQ_HDLR_IDX)          \
    {                                           \
        /* Enable RX TIME STAMP interrupt */    \
        TIMSK1 |= _BV(ICIE1);                   \
    }                                           \
} while (0)

/** Disables the transceiver interrupts. */
#define DISABLE_TRX_IRQ(trx_irq_num)     do {   \
    if (trx_irq_num == TRX_MAIN_IRQ_HDLR_IDX)          \
    {                                           \
        /* Disable RX TIME STAMP interrupt */   \
        TIMSK1 &= ~(_BV(ICIE1));                \
    }                                           \
} while (0)

/** Clears the transceiver interrupts. */
#define CLEAR_TRX_IRQ(trx_irq_num)     do {     \
    if (trx_irq_num == TRX_MAIN_IRQ_HDLR_IDX)          \
    {                                           \
        /* Clear RX TIME STAMP interrupt */     \
        TIFR1 = _BV(ICF1);                      \
    }                                           \
} while (0)

/** Enables the global interrupts. */
#define ENABLE_GLOBAL_IRQ()             sei()

/** Disables the global interrupts. */
#define DISABLE_GLOBAL_IRQ()            cli()

/*
 * This macro saves the global interrupt status
 */
#define ENTER_CRITICAL_REGION()         {uint8_t sreg = SREG; cli()

/*
 *  This macro restores the global interrupt status
 */
#define LEAVE_CRITICAL_REGION()         SREG = sreg;}

/*
 * This macro saves the trx interrupt status and disables the trx interrupt.
 */
#define ENTER_TRX_REGION()      { uint8_t irq_mask = TIMSK1; TIMSK1 &= ~(_BV(ICIE1))

/*
 *  This macro restores the transceiver interrupt status
 */
#define LEAVE_TRX_REGION()      TIMSK1 = irq_mask; }


/*
 * GPIO macros for ATMEGA32U4
 */

/* Port Identifier */

/*
 * This board uses an SPI-attached transceiver.
 */
#define PAL_USE_SPI_TRX                 (1)

/*
 * PORTB is transceiver port
 */
#define TRX_PORT1_ID                    (PORT_B)

/* Actual Ports */
/*
 * PortB is data direction register for the transceiver
 */
#define TRX_PORT1_DDR                   (DDRB)

/*
 * PORTB is transceiver port
 */
#define TRX_PORT1                       (PORTB)

/*
 * Slave select pin is PORTB 0
 */
#define SEL                             (PINB0)
#define SEL_PORT                        (TRX_PORT1_ID)

/*
 * SPI serial clock pin is PORTB 1
 */
#define SCK                             (PINB1)
#define SCK_PORT                        (TRX_PORT1_ID)

/*
 * SPI Bus Master Output/Slave Input pin is PORTB 2
 */
#define MOSI                            (PINB2)
#define MOSI_PORT                       (TRX_PORT1_ID)

/*
 * SPI Bus Master Input/Slave Output pin is PORTB 3
 */
#define MISO                            (PINB3)
#define MISO_PORT                       (TRX_PORT1_ID)

/*
 * Sleep Transceiver pin is PORTB 4
 */
#define SLP_TR                          (PINB4)
#define SLP_TR_PORT                     (TRX_PORT1_ID)

/*
 * RESET pin is PORTB 5
 */
#define RST                             (PINB5)
#define RST_PORT                        (TRX_PORT1_ID)

/*
 * Set TRX GPIO pins.
 */
#define RST_HIGH()                      (TRX_PORT1 |= _BV(RST))
#define RST_LOW()                       (TRX_PORT1 &= ~_BV(RST))
#define SLP_TR_HIGH()                   (TRX_PORT1 |= _BV(SLP_TR))
#define SLP_TR_LOW()                    (TRX_PORT1 &= ~_BV(SLP_TR))

/*
 * PORT where LEDs are connected
 */
#define LED_PORT_LED_0                  (PORTD)
#define LED_PORT_DIR_LED_0              (DDRD)
#define LED_PORT_LED_1                  (PORTD)
#define LED_PORT_DIR_LED_1              (DDRD)
#define LED_PORT_LED_2                  (PORTC)
#define LED_PORT_DIR_LED_2              (DDRC)
#define LED_PORT_LED_3                  (PORTC)
#define LED_PORT_DIR_LED_3              (DDRC)

/*
 * PINs where LEDs are connected
 */
#define LED_PIN_0                       (PIND7)
#define LED_PIN_1                       (PIND6)
#define LED_PIN_2                       (PINC7)
#define LED_PIN_3                       (PINC6)

/*
 * Value of an external PA gain.
 * If no external PA is available, the value is 0.
 */
#define EXTERN_PA_GAIN                  (0)

/*
 * Timer macros for AT90USB1286
 */

/*
 * These macros are placeholders for delay functions for high speed processors.
 *
 * The following delay are not reasonbly implemented via delay functions,
 * but rather via a certain number of NOP operations.
 * The actual number of NOPs for each delay is fully MCU and frequency
 * dependent, so it needs to be updated for each board configuration.
 *
 * AT90USB1286 @ 8MHz
 */
/*
 * Wait for 65 ns.
 * time t7: SLP_TR time (see data sheet or SWPM).
 * In case the system clock is slower than 32 MHz we do not need
 * to have this value filled.
 */
#define PAL_WAIT_65_NS()                // empty

/* Wait for 500 ns. */
#define PAL_WAIT_500_NS()               {nop(); nop(); nop(); nop(); }

/* Wait for 1 us. */
#define PAL_WAIT_1_US()                 {nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();}

/*
 * The smallest timeout in microseconds
 */
#define MIN_TIMEOUT                     (0x80)

/*
 * The largest timeout in microseconds
 */
#define MAX_TIMEOUT                     (0x7FFFFFFF)

/**
 * Minimum time in microseconds, accepted as a delay request
 */
#define MIN_DELAY_VAL           (5)

/*
 * Timer clock source while radio is awake.
 */
//#define TIMER_SRC_DURING_TRX_AWAKE() 
//        (TCCR1B = ((1 << ICES1) | (1 << CS12) | (1 << CS11) | (1 << CS10)))
#define TIMER_SRC_DURING_TRX_AWAKE() \
		(TCCR1B = (1 << ICES1) | (1 << CS11))

/*
 * Timer clock source while radio is sleeping.
 */
#define TIMER_SRC_DURING_TRX_SLEEP() \
        (TCCR1B = (1 << ICES1) | (1 << CS11))

/*
 * Maximum numbers of software timers running at a time
 */
#define MAX_NO_OF_TIMERS (25)
#if (MAX_NO_OF_TIMERS > 255)
#error "MAX_NO_OF_TIMERS must not be larger than 255"
#endif

/*
 * Hardware register that holds Rx timestamp
 */
#define TIME_STAMP_REGISTER             (ICR1)


/*
 * TRX Access macros for AT90USB1286
 */

/*
 * Bit mask for slave select
 */
#define SS_BIT_MASK                     (1 << SEL)

/*
 * Slave select made low
 */
#define SS_LOW()                        ((TRX_PORT1) &= (~SS_BIT_MASK))

/*
 * Slave select made high
 */
#define SS_HIGH()                       ((TRX_PORT1) |= (SS_BIT_MASK))

/*
 * Mask for SPIF bit in SPSR register
 */
#define SPIF_MASK                       (1 << SPIF)

/*
 * Wait for SPI interrupt flag
 */
#define SPI_WAIT()                      while (!(SPSR & SPIF_MASK))

/*
 * Dummy value written in SPDR to retrieve data from it
 */
#define SPI_DUMMY_VALUE                 (0x00)

/*
 * TRX Initialization
 */
#define TRX_INIT()                      do {                        \
    /* Enable the SPI and make the microcontroller as SPI master */ \
    SPCR = _BV(SPE) | _BV(MSTR);                                    \
    /*SPSR = _BV(SPI2X);  Double the SPI clock frequency */         \
    /* Select the serial clock SCK to be (FOSC/4)                   \
     * For Example for AT90USB1286, FOSC = 8Mhz                     \
     * (Microcontroller operating frequency)                        \
     * SPI clock is thus set to (8/4)*2 = 4 Mhz                     \
	 * Modify to divide by 16 (16MHz clock)							\
     */                                                             \
    SPCR &= ~(_BV(SPR0) | _BV(SPR1));                               \
	SPCR |= _BV(SPR0);                               \
                                                                    \
    /* Set SEL pin which is on PortB to high */                     \
    TRX_PORT1 |= _BV(SEL);                                          \
} while (0)



/*
 * UART0 and UART1 non-generic (board specific) initialization part.
 */
/* Data Length is 8 bit */
#define UART_0_INIT_NON_GENERIC()   (UCSR0C = (1 << UCSZ01) | (1 << UCSZ00))
#define UART_1_INIT_NON_GENERIC()   (UCSR1C = (1 << UCSZ11) | (1 << UCSZ10)))


/*
 * This board does NOT have an external eeprom available.
 */
#ifndef EXTERN_EEPROM_AVAILABLE
#define EXTERN_EEPROM_AVAILABLE            (0)
#endif

/*
 * Alert initialization
 * Sense of LED_0 is active high now.   -jkl
 */
#define ALERT_INIT()                        \
do                                          \
{                                           \
	LED_PORT_LED_0 |= (1 << LED_PIN_0);     \
    LED_PORT_LED_1 |= (1 << LED_PIN_1);     \
    LED_PORT_LED_2 |= (1 << LED_PIN_2);     \
    LED_PORT_LED_3 |= (1 << LED_PIN_3);     \
    LED_PORT_DIR_LED_0 |= (1 << LED_PIN_0); \
    LED_PORT_DIR_LED_1 |= (1 << LED_PIN_1); \
    LED_PORT_DIR_LED_2 |= (1 << LED_PIN_2); \
    LED_PORT_DIR_LED_3 |= (1 << LED_PIN_3); \
}                                           \
while (0)

/*
 * Alert indication
 * Modified so LED_0 is active high   -jkl
 */
#define ALERT_INDICATE()                       \
do                                             \
{                                              \
    if (LED_PORT_LED_0 & (1 << LED_PIN_0))     \
    {                                          \
		LED_PORT_LED_0 &= ~(1 << LED_PIN_0);   \
        LED_PORT_LED_1 &= ~(1 << LED_PIN_1);   \
        LED_PORT_LED_2 &= ~(1 << LED_PIN_2);   \
        LED_PORT_LED_3 &= ~(1 << LED_PIN_3);   \
    }                                          \
    else                                       \
    {                                          \
		LED_PORT_LED_0 |= (1 << LED_PIN_0);    \
        LED_PORT_LED_1 |= (1 << LED_PIN_1);    \
        LED_PORT_LED_2 |= (1 << LED_PIN_2);    \
        LED_PORT_LED_3 |= (1 << LED_PIN_3);    \
    }                                          \
} while (0)



/* === Prototypes ===========================================================*/
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* T2 */

#endif  /* PAL_CONFIG_H */
/* EOF */
