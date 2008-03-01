/*
 * nrf2401.c:
 *  Functions for Nordic nRF2401 based radio modules
 *
 *		(c) 2004-2008 Tymm Twillman <tymmothy@gmail.com>
 *
 *  This file is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published 
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This file is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this file.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <stdint.h>
#include <string.h>

#include "nrf2401.h"
#include "nrf2401_hw.h"
#include "nrf2401_config_bits.h"

#include "spin_delay.h"


/* Configuration information (structure defined in
 *  nrf2401_config_bits.h)
 */

nrf2401_config_t nrf2401_config;


/* Send a byte out to the RF module */


static void nrf2401_send_byte(uint8_t byte)
{
    uint8_t i;


    for (i = 0x80; i; i >>= 1) {
        if (byte & i) {
            nrf2401_set_data1_high();
        } else {
            nrf2401_set_data1_low();
        }

        delay_500ns();
        nrf2401_set_clk1_high();
        delay_500ns();
        nrf2401_set_clk1_low();
    }
}


/* Get a byte from the RF module */

static uint8_t nrf2401_recv_byte(uint8_t channel)
{
    uint8_t ret = 0;
    uint8_t i;


#ifdef CONFIG_NRF2401_RX2
     if (channel == 2) {
     	for (i = 0x80; i; i >>= 1) {
            delay_500ns();
            nrf2401_set_clk2_high();
            delay_500ns();
            if (nrf2401_data2_is_high()) {
            	ret |= i;
            }
            nrf2401_set_clk2_low();
        }
     } else {
#endif
    	for (i = 0x80; i; i >>= 1) {
            delay_500ns();
            nrf2401_set_clk1_high();
            delay_500ns();
            if (nrf2401_data1_is_high()) {
            	ret |= i;
            }
            nrf2401_set_clk1_low();
		}
#ifdef CONFIG_NRF2401_RX2
    }
#endif

    return ret;
}


/* Load current configuration into module */

void nrf2401_load_config()
{
    uint8_t *confdata = (uint8_t *)&nrf2401_config;
	int i;
	

    /* Power down RF section */
    nrf2401_set_ce_low();

    /* Set config mode */
    nrf2401_set_cs_high();

    /* Tcs2data (5us) */
    us_spin(5);

    nrf2401_set_data1_output();

	for (i = 0; i < sizeof(nrf2401_config_t); i++) {
        nrf2401_send_byte(confdata[i]);
    }

    /* Exit config mode */
    nrf2401_set_cs_low();

    /* Leave RF section powered down until RX or TX enabled */
}


/* Enable receive/transmit mode */

void nrf2401_set_rxtx(uint8_t rxen)
{
    /* Power down RF section */
    nrf2401_set_ce_low();

    /* Set config mode */
    nrf2401_set_cs_high();

    /* Set data line for output */
    nrf2401_set_data1_output();

    /* Tcs2data (5us) */
    us_spin(5);

    /* Set RX enable bit */
    if (rxen) {
        nrf2401_config.rf_ch_rxen |= NRF2401_RXEN_MASK;
    } else {
        nrf2401_config.rf_ch_rxen &= ~NRF2401_RXEN_MASK;
    }

    /* Send channel/RX enable bit */
    nrf2401_send_byte(nrf2401_config.rf_ch_rxen);

    /* Exit config mode */
    nrf2401_set_cs_low();

    /* Set data line for input so there's no collisions, if rx mode */
    if (rxen) {
        nrf2401_set_data1_input();

        /* Power up RF section */
        nrf2401_set_ce_high();
    }
}


/* Returns !=0 when data ready for channel 1 */

uint8_t nrf2401_available()
{
    return nrf2401_dr1_is_high();
}


/* Receive a packet (call when data is ready).  
 *  Returns # bytes received.
 */

uint8_t nrf2401_recv(uint8_t *packet)
{
    uint8_t i;


    /* Tce2data */
    us_spin(5);

    /* Clock in the bits... */
    for (i = 0; i < (nrf2401_config.data1_w + 7) / 8; i++) {
        packet[i] = nrf2401_recv_byte(1);
    }

    return i;
}


#ifdef CONFIG_NRF2401_RX2

/* Returns !=0 when data ready for channel 2 */

uint8_t nrf2401_available2()
{
    return nrf2401_dr2_is_high();
}


/* Receive a packet (call when data is ready).  
 *  Returns # bytes received.
 */

uint8_t nrf2401_recv2(uint8_t *packet)
{
    uint8_t i;


    /* Tce2data */
    us_spin(5);

    /* Clock in the bits... */
    for (i = 0; i < (nrf2401_config.data1_w + 7) / 8; i++) {
        packet[i] = nrf2401_recv_byte(2);
    }

    return i;
}


#endif


/* Wait for data to be transmitted (static wait time, just 1us per bit) */

void nrf2401_tx_wait()
{
    /* 195 uS for Tsby2tx, 8 bits preamble, address, data, crc
     *  quicker to just say 16 than to calculate whether to wait 8
     *  or 16 for crc
     */
    us_spin(195 
            + 8
            + ((nrf2401_config.addr_w_crc & NRF2401_ADDR_W_MASK) 
                                         >> NRF2401_ADDR_W_SHIFT)
            + nrf2401_config.data1_w
            + 16);
}


/* Transmit a packet.  Returns # bytes sent. */

uint8_t nrf2401_send(uint8_t *address, uint8_t *data)
{
    uint8_t i;


#ifndef CONFIG_NRF2401_MANUAL_TX
	nrf2401_set_rxtx(0);
#endif

    /* Enable RF section */
    nrf2401_set_ce_high();

    /* Tce2data (5 us) */
    us_spin(5);

    /* Clock out address */
    for (i = 0; 
         i < (((nrf2401_config.addr_w_crc & NRF2401_ADDR_W_MASK) 
                                         >> NRF2401_ADDR_W_SHIFT) + 7) / 8;
         i++)
    {
        nrf2401_send_byte(address[i]);
    }

    /* Clock out data */
    for (i = 0; i < nrf2401_config.data1_w / 8; i++) {
        nrf2401_send_byte(data[i]);
    }

    /* Trigger transmission */
    nrf2401_set_ce_low();

#ifndef CONFIG_NRF2401_MANUAL_TX
    nrf2401_tx_wait();
    nrf2401_set_rxtx(1);
#endif

    return i;
}


/* Initialize config information, initialize IO lines
 *  and send initial configuration to module
 */

#ifdef CONFIG_NRF2401_RX2
void nrf2401_init(uint8_t channel, uint8_t *addr1, uint8_t *addr2, uint8_t opt)
#else
void nrf2401_init(uint8_t channel, uint8_t *addr1, uint8_t opt)
#endif
{		
    /* Give at least 3ms after power on before calling */


    /* Set I/O lines up */

    nrf2401_set_ce_low();
    nrf2401_set_cs_low();
    nrf2401_set_ce_output();
    nrf2401_set_cs_output();
    nrf2401_set_dr1_input();
    nrf2401_set_data1_low();
    nrf2401_set_clk1_low();
    nrf2401_set_clk1_output();
    nrf2401_set_data1_input();

#ifdef CONFIG_NRF2401_RX2
    nrf2401_set_dr2_input();
    nrf2401_set_data2_low();
    nrf2401_set_clk2_low();
    nrf2401_set_clk2_output();
    nrf2401_set_data2_input();

    memcpy(nrf2401_config.addr2, addr2, sizeof(nrf2401_config.addr2));
#endif

    memcpy(nrf2401_config.addr1, addr1, sizeof(nrf2401_config.addr1));


    /* Load configuration from option byte (inverted bits so a decent config comes
     *  from just passing a 0)
     */

	opt ^= (uint8_t)0xff;
	
	/* Data lengths require more thought; just default it & if someone knows
	 *  what they're doing they can handle tweaking the configuration
	 */
    nrf2401_config.data2_w = NRF2401_DEFAULT_DATA2_W;
    nrf2401_config.data1_w = NRF2401_DEFAULT_DATA1_W;
    
    /* See nrf2401_config_bits.h for more details on layout.  Dependent on
     *  bit ordering in option byte, so pay attention to option values in nrf2401.h
     *  if you play with this stuff & make sure everything lines back up right.
     */
        
    nrf2401_config.addr_w_crc = ((opt & NRF2401_OPT_ADDRLEN_MASK) << 3) 
    							| (opt & NRF2401_OPT_CRC_MASK);


    nrf2401_config.misc = 		(1 << NRF2401_CM_SHIFT) /* ShockBurst requ'd */
#ifdef CONFIG_NRF2401_RX2
								| (1 << NRF2401_RX2_EN_SHIFT)
#endif
								| (NRF2401_DEFAULT_XO_F << NRF2401_XO_F_SHIFT)
								| (opt & NRF2401_OPT_SPEED_MASK)
    							| ((opt & NRF2401_OPT_PWR_MASK) >> 6);
                          
    nrf2401_config.rf_ch_rxen = (channel << NRF2401_RF_CH_SHIFT) | 1; /* 1 for RX EN */

    nrf2401_load_config();
}


/* Power down RF section. */

void nrf2401_sleep()
{
    /* Setting ce low disables RF section. */
    nrf2401_set_ce_low();
}


void nrf2401_wake()
{
    /* Only use for waking up in RX mode */
    nrf2401_set_ce_high();
}

