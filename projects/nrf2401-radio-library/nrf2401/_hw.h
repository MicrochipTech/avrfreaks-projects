/*
 * nrf2401_hw.h:
 *  Header file for uC hardware access for using Nordic nRF2401 radio module
 *
 *         (c) 2004-2008 Tymm Twillman <tymmothy@gmail.com>
 *
 */
 
#ifndef NRF2401_HW_H
#define NRF2401_HW_H

#include <avr/io.h>


/* Register and pin definitions */

# define NRF2401_PORT       PORTB
# define NRF2401_DDR        DDRB
# define NRF2401_PORT_IN    PINB

# define NRF2401_CE_LINE    PB3
# define NRF2401_CS_LINE    PB2
# define NRF2401_DR1_LINE   PB4
# define NRF2401_CLK1_LINE  PB0
# define NRF2401_DATA1_LINE PB1
# define NRF2401_DR2_LINE
# define NRF2401_CLK2_LINE
# define NRF2401_DATA2_LINE

/* IO direction control definitions */

# define nrf2401_set_ce_output()    (NRF2401_DDR |= _BV(NRF2401_CE_LINE))
# define nrf2401_set_cs_output()    (NRF2401_DDR |= _BV(NRF2401_CS_LINE))
# define nrf2401_set_clk1_output()  (NRF2401_DDR |= _BV(NRF2401_CLK1_LINE))
# define nrf2401_set_data1_output() (NRF2401_DDR |= _BV(NRF2401_DATA1_LINE))
# define nrf2401_set_data1_input()  (NRF2401_DDR &= ~_BV(NRF2401_DATA1_LINE))
# define nrf2401_set_dr1_input()    (NRF2401_DDR &= ~_BV(NRF2401_DR1_LINE))
# define nrf2401_set_clk2_output()  (NRF2401_DDR |= _BV(NRF2401_CLK2_LINE))
# define nrf2401_set_data2_output() (NRF2401_DDR |= _BV(NRF2401_DATA2_LINE))
# define nrf2401_set_data2_input()  (NRF2401_DDR &= ~_BV(NRF2401_DATA2_LINE))
# define nrf2401_set_dr2_input()    (NRF2401_DDR &= ~_BV(NRF2401_DR2_LINE))

/* IO level sense & control definitions */

# define nrf2401_data1_is_high()  bit_is_set(NRF2401_PORT_IN, NRF2401_DATA1_LINE)
# define nrf2401_data2_is_high()  bit_is_set(NRF2401_PORT_IN, NRF2401_DATA2_LINE)
# define nrf2401_set_ce_high()    (NRF2401_PORT |= _BV(NRF2401_CE_LINE))
# define nrf2401_set_ce_low()     (NRF2401_PORT &= ~_BV(NRF2401_CE_LINE))
# define nrf2401_set_cs_high()    (NRF2401_PORT |= _BV(NRF2401_CS_LINE))
# define nrf2401_set_cs_low()     (NRF2401_PORT &= ~_BV(NRF2401_CS_LINE))
# define nrf2401_set_data1_high() (NRF2401_PORT |= _BV(NRF2401_DATA1_LINE))
# define nrf2401_set_data1_low()  (NRF2401_PORT &= ~_BV(NRF2401_DATA1_LINE))
# define nrf2401_set_clk1_high()  (NRF2401_PORT |= _BV(NRF2401_CLK1_LINE))
# define nrf2401_set_clk1_low()   (NRF2401_PORT &= ~_BV(NRF2401_CLK1_LINE))
# define nrf2401_dr1_is_high()    bit_is_set(NRF2401_PORT_IN, NRF2401_DR1_LINE)
# define nrf2401_set_data2_high() (NRF2401_PORT |= _BV(NRF2401_DATA2_LINE))
# define nrf2401_set_data2_low()  (NRF2401_PORT &= ~_BV(NRF2401_DATA2_LINE))
# define nrf2401_set_clk2_high()  (NRF2401_PORT |= _BV(NRF2401_CLK2_LINE))
# define nrf2401_set_clk2_low()   (NRF2401_PORT &= ~_BV(NRF2401_CLK2_LINE))
# define nrf2401_dr2_is_high()    bit_is_set(NRF2401_PORT_IN, NRF2401_DR2_LINE)

/* Use 1 nop per 2 mhz system clock */
# define delay_500ns() {                                                   \
      __asm__ __volatile__ ("	nop\r\n	nop\r\n	nop\r\n	nop\r\n	nop\r\n"); \
    }

#endif /* #ifndef NRF2401_HW_H ... */
