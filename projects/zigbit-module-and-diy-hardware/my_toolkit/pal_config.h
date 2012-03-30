/**
 * @file pal_config.h
 *
 * @brief PAL configuration for ATmega1281
 *
 * This header file contains configuration parameters for ATmega1281 and ZigBit ATZB-23_A2 module.
 *
 * $Id: pal_config.h 22917 2010-08-13 10:15:57Z sschneid $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2012, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/*****************************
ZIGBIT SIGNAL            ATmega1281
PIN    NAME              PIN
------------------------------
01     SPI_SCLK          SCK 
02     SPI_MISO          DSO 
03     SPI_MOSI          DSI 
04     GPIO0             PB5 
05     GPIO1             PB6 
06     GPIO2             PB7 
07     OSC32K_OUT        TMC 
08     RESET             RST 
10     CPU_CLK           CLK 
11     I2C_CLK           PD0 
12     I2C_DATA          PD1 
13     UART_TXD          PD2 
14     UART_RXD          PD3 
15     UART_RTS          PD4 
16     UART_CTS          PD5 
17     GPIO6             PD6 
18     GPIO7             PD7 
19     GPIO3             PG0 
20     GPIO4             PG1 
21     GPIO5             PG2 
26     JTAG_TMS          PF5 
27     JTAG_TDI          PF7 
28     JTAG_TDO          PF6 
29     JTAG_TCK          PF4 
30     ADC_INPUT3        PF3 
31     ADC_INPUT2        PF2 
32     ADC_INPUT1        PF1 
33     BAT               PF0 
34     A_VREF            REF 
36     1WR               1WR 
37     UART_DTR          PE4 
38     USART0_RXD        PE0 
39     USART0_TXD        PE1 
40     USART0_EXTCLK     PE2 
41     GPIO8             PE3 
42     IRQ6              PE6 
43     IRQ7              PE7 
*******************************/


/* Prevent double inclusion */
#ifndef PAL_CONFIG_H
#define PAL_CONFIG_H

/* === Includes =============================================================*/

#include "pal_boardtypes.h"

#if (BOARD_TYPE == MY_BOARD)

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

/* Enumerations used to identify LEDs */

/* the new stuff for one LED*/

typedef enum led_id_tag
{
    LED_0
   
} led_id_t;

#define NO_OF_LEDS                      (1)

/* Enumerations used to identify buttons */
typedef enum button_id_tag
{
    BUTTON_0
} button_id_t;

#define NO_OF_BUTTONS                   (1)

/* === Externals ============================================================*/


/* === Macros ===============================================================*/

/** The default CPU clock */
#ifndef F_CPU
#define F_CPU                           (8000000UL)
#endif

/*
 * IRQ macros for ATmega1281
 */

/* Mapping of TRX interrupts to ISR vectors */
#define TRX_IRQ                         (INT5_vect)


/** Enables the main transceiver interrupt. */
#define ENABLE_TRX_IRQ()                (EIMSK |= _BV(INT5))

/** Disables the main transceiver interrupt. */
#define DISABLE_TRX_IRQ()               (EIMSK &= ~(_BV(INT5)))

/** Clears the main transceiver interrupt. */
#define CLEAR_TRX_IRQ()                 (EIFR = _BV(INTF5))


/** Enables the global interrupts. */
#define ENABLE_GLOBAL_IRQ()             sei()

/** Disables the global interrupts. */
#define DISABLE_GLOBAL_IRQ()            cli()

/** This macro saves the global interrupt status. */
#define ENTER_CRITICAL_REGION()         {uint8_t sreg = SREG; cli()

/** This macro restores the global interrupt status. */
#define LEAVE_CRITICAL_REGION()         SREG = sreg;}

/** This macro saves the trx interrupt status and disables the trx interrupt. */
#define ENTER_TRX_REGION()      { uint8_t irq_mask = EIMSK; EIMSK &= ~(_BV(INT5))

/**  This macro restores the transceiver interrupt status. */
#define LEAVE_TRX_REGION()      EIMSK = irq_mask; }


/*
 * GPIO macros for ATmega1281
 */

/*
 * This board uses an SPI-attached transceiver.
 */
#define PAL_USE_SPI_TRX                 (1)

/* Actual Ports */
/*
 * PORTB and PORTA DDR are data direction registers for the transceiver
 */
#define TRX_PORT1_DDR                   (DDRB)
#define TRX_PORT2_DDR                   (DDRA)

/*
 * PORTB and PORTA are transceiver ports
 */
#define TRX_PORT1                       (PORTB)
#define TRX_PORT2                       (PORTA)

/*
 * Slave select pin is PORTB 0
 */
#define SEL                             (PB0)

/*
 * SPI serial clock pin is PORTB 1
 */
#define SCK                             (PB1)

/*
 * SPI Bus Master Output/Slave Input pin is PORTB 2
 */
#define MOSI                            (PB2)

/*
 * SPI Bus Master Input/Slave Output pin is PORTB 3
 */
#define MISO                            (PB3)

/*
 * Sleep Transceiver pin is PORTB 4
 */
#define SLP_TR                          (PB4)

/*
 * RESET pin is PORTA 7
 */
#define RST                             (PA7)

/*
 * Set TRX GPIO pins.
 */
#define RST_HIGH()                      (TRX_PORT2 |= _BV(RST))
#define RST_LOW()                       (TRX_PORT2 &= ~_BV(RST))
#define SLP_TR_HIGH()                   (TRX_PORT1 |= _BV(SLP_TR))
#define SLP_TR_LOW()                    (TRX_PORT1 &= ~_BV(SLP_TR))

/*
 * PORT where LEDs are connected
 */
#define LED_PORT                        (PORTB)
#define LED_PORT_DIR                    (DDRB)

/*
 * PINs where LEDs are connected
 */

#define LED_PIN_0                       (PB6)


#define BUTTON_PORT                     (PORTB)
#define BUTTON_PORT_DIR                 (DDRB)
#define BUTTON_INPUT_PINS               (PINB)

#define BUTTON_PIN_0                    (PB5)

/**
 * Value of an external PA gain.
 * If no external PA is available, the value is 0.
 */
#define EXTERN_PA_GAIN                  (0)

/*
 * Timer macros for ATmega1281
 */

/*
 * This value is used by the calibration routine as target value of the timer
 * to be calibrated. At the end of a calibration cycle this value is compared
 * with the counter result.
 * TARGETVAL_CALIBRATION = CLK(Cal) * Count(Ref) / CLK(Ref) = 8,000,000 * 256 / 32,768
 *
 * CLK(Cal): Frequency of clock source of timer to be calibrated
 * CLK(Ref): Frequency of clock source of reference timer
 * Count(Ref): Value of reference timer after 1 full cycle (until overflow)
 *             for timer width of 8 bit
 */
#if (F_CPU == (8000000UL))
#define TARGETVAL_CALIBRATION           (62500)
#elif  (F_CPU == (4000000UL))
#define TARGETVAL_CALIBRATION           (31250)
#elif  (F_CPU == (2000000UL))
#define TARGETVAL_CALIBRATION           (15625)
#elif  (F_CPU == (1000000UL))
#define TARGETVAL_CALIBRATION           (7813)
#else
#error "Unsupported F_CPU value"
#endif

/** This value is the loop counter for calibration routine. */
#define MAX_CAL_LOOP_CNT                (100)

/*
 * These macros are placeholders for delay functions for high speed processors.
 *
 * The following delay are not reasonbly implemented via delay functions,
 * but rather via a certain number of NOP operations.
 * The actual number of NOPs for each delay is fully MCU and frequency
 * dependent, so it needs to be updated for each board configuration.
 *
 * ATmega1281 @ 8MHz
 */
#if (F_CPU == (8000000UL))
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

#else
#error "Unsupported F_CPU value"
#endif
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
#define MIN_DELAY_VAL                   (5)

/*
 * Timer clock source while radio is awake.
 */
#define TIMER_SRC_DURING_TRX_AWAKE() TCCR1B &= ~0xF8; TCCR1B |= (1<<CS11)

/*
 * Timer clock source while radio is sleeping.
 */
#define TIMER_SRC_DURING_TRX_SLEEP() TCCR1B &= ~0xF8; TCCR1B |= (1<<CS11)

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
 * TRX Access macros for ATmega1281
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
 * Dummy value written in SPDR to retrieve data form it
 */
#define SPI_DUMMY_VALUE                 (0x00)

/*
 * TRX Initialization
 */
#define TRX_INIT()                      do {                        \
    /* Enable the SPI and make the microcontroller as SPI master */ \
    SPCR = _BV(SPE) | _BV(MSTR);                                    \
    SPSR = _BV(SPI2X); /* Double the SPI clock frequency */         \
    /* Select the serial clock SCK to be (FOSC/4)                   \
     * For Example for ATmega1281, FOSC = 8Mhz                      \
     * (Microcontroller operating frequency)                        \
     * SPI clock is thus set to (8/4)*2 = 4 Mhz                     \
     */                                                             \
    SPCR &= ~(_BV(SPR0) | _BV(SPR1));                               \
                                                                    \
    /* Set SEL pin which is on PortB to high */                     \
    TRX_PORT1 |= _BV(SEL);                                          \
} while (0)

/*
 * SPI Interrupt enable macro
 */
#define SPI_IRQ_ENABLE()        (SPCR |= _BV(SPIE))

/*
 * SPI Interrupt disable macro
 */
#define SPI_IRQ_DISABLE()       (SPCR &= ~_BV(SPIE))

/*
 * SPI Interrupt clear macro
 */
#define SPI_IRQ_CLEAR()         (SPSR &= ~_BV(SPIF))

/*
 * SIO macros for ATmega1281
 */
/*
 * UART0 and UART1 non-generic (board specific) initialization part.
 *
 * Although this PAL is specifically designed for ZigBit modules based
 * on the MeshBean2 platform (where only UART1 is used - either directly
 * via UART or via USB), UART0 is handled here as well, to allow other
 * ZigBit based platforms using UART0.
 */
/* Data Length is 8 bit */
#define UART_0_INIT_NON_GENERIC()   (UCSR0C = (1 << UCSZ01) | (1 << UCSZ00))
#define UART_1_INIT_NON_GENERIC()   (UCSR1C = (1 << UCSZ11) | (1 << UCSZ10))


#ifndef EXTERN_EEPROM_AVAILABLE
#define EXTERN_EEPROM_AVAILABLE            (0)
#endif


/*
 * Alert initialization
 */
#define ALERT_INIT()                    do {    \
        LED_PORT = 0;                           \
        LED_PORT_DIR = 0xFF;                    \
} while (0)

/*
 * Alert indication
 */
#define ALERT_INDICATE()                (LED_PORT = (uint8_t)(~(uint16_t)(LED_PORT)))


/* === Prototypes ===========================================================*/
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ATZB_24_MN2 */

#endif  /* PAL_CONFIG_H */
/* EOF */
