/*
 * nrf2401.h
 *  Header file for Nordic nRF2401 based radio modules
 *
 *		(c) 2004-2008 Tymm Twillman <tymmothy@gmail.com>
 *
 *
 * Package configuration options:
 *
 *  CONFIG_NRF2401_RX2
 *   If defined, enable second RX channel (including support functions, 
 *   extra parameter to nrf2401_init).  Requires extra pins for communication,
 *   greater power consumption.  Second RX channel always 8mhz above
 *   configured first channel.
 *
 *  CONFIG_NRF2401_MANUAL_TX
 *   If defined, don't automatically switch in/out of TX mode when
 *   calling nrf2401_transmit(), and don't automatically wait for the
 *   TX guard time after transmitting.  This allows greater control
 *   over transmit timing & overhead, but also increases the level of
 *   complexity in the calling code.
 *
 *
 * Usage:
 *
 *   (general case, assuming only 1 RX channel and automatic TX
 *   switching, default packet data sizes, etc.)
 *
 *	 call nrf2401_init(channel, opt, address) to begin, where
 *     channel: Channel to transmit/receive on (0-127)
 *     opt:     Configuration options (0 is generally a safe bet; see
 *                NRF2401_OPT_* definitions, below, for other things to
 *                OR together to form an option byte)
 *     address: Local address (0-5 bytes long, depending on options)
 *
 *   call nrf2401_available() to poll for incoming data ready
 *
 *   call nrf2401_recv(data) to get incoming data (recommended to
 *    always call with a 32-byte buffer since this will be big enough
 *    despite configuration, though will only fill up to configured # 
 *    of data bytes per packet, default is 24).  This will return the
 *    # of bytes received.
 *
 *   call nrf2401_send(address, data) to send data.  Will always send
 *    a full packet (configured # of data bytes per packet), so make sure
 *    to pad your buffer properly, etc.
 *   
 *	 That's mostly it.  Always remember that packet data lengths & address
 *    lengths are not dynamic, and must be set equally on all units
 *    communicating with each other.
 *
 *
 *   Special configuration defaults (to change packet data length, crystal
 *    frequency, more advanced stuff): see nrf2401_config_bits.h
 *
 *
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
 */

#ifndef NRF2401_H
#define NRF2401_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Configuration Notes: (see nrf2401_config_bits.h)
 *  Package is hardwired for "ShockBurst" use, 250k or 1m
 *  Default address length == 5 bytes
 *  Default data length == 24 bytes
 *  Default CRC == 16 bits
 *
 * Channels are 1,2 (not 0,1)!
 */


/* ------- Options (to be passed to nrf2401_init) ------ */


/* CRC enable/bits */
#define NRF2401_OPT_CRC_NONE     0x03
#define NRF2401_OPT_CRC_8        0x01
#define NRF2401_OPT_CRC_16       0x00

/* Address length (in bytes) */
#define NRF2401_OPT_ADDRLEN_5    0x00
#define NRF2401_OPT_ADDRLEN_4    0x04
#define NRF2401_OPT_ADDRLEN_3    0x08
#define NRF2401_OPT_ADDRLEN_2    0x0c
#define NRF2401_OPT_ADDRLEN_1    0x10
#define NRF2401_OPT_ADDRLEN_0    0x14

/* Radio speed (bps) */
#define NRF2401_OPT_SPEED_250K   0x20
#define NRF2401_OPT_SPEED_1M     0x00

/* Power Level */
#define NRF2401_OPT_PWR_MIN      0xc0   /* -20dB */
#define NRF2401_OPT_PWR_LO       0x80   /* -10dB */
#define NRF2401_OPT_PWR_MED      0x40   /* - 5dB */
#define NRF2401_OPT_PWR_MAX      0x00   /*   0dB */


#define NRF2401_OPT_CRC_MASK     0x03
#define NRF2401_OPT_ADDRLEN_MASK 0x14
#define NRF2401_OPT_SPEED_MASK   0x20
#define NRF2401_OPT_PWR_MASK     0xc0


/* ---------------- Function Prototypes ---------------- */


/*
 * In most cases you need these
 */


/* Returns !=0 when data ready for RX channel 1 */
uint8_t nrf2401_available(void);

/* Receive a packet on RX channel 1 (call when data is ready); packet length
 *  determined by current configuration.  32 bytes max.  Returns # bytes
 *  received.
 */
uint8_t nrf2401_recv(uint8_t *packet);

/* Transmit a packet; packet length determined by current configuration.
 *  32 bytes max.  Returns # bytes sent.
 */
uint8_t nrf2401_send(uint8_t *address, uint8_t *data);

/* Initialize config information, initialize IO lines
 *  and send initial configuration to module
 */
#ifdef CONFIG_NRF2401_RX2
void nrf2401_init(uint8_t rf_channel, uint8_t *addr1, uint8_t *addr2, uint8_t opt);
#else
void nrf2401_init(uint8_t rf_channel, uint8_t *addr1, uint8_t opt);
#endif

/* Power down RF section.  Only need use in RX mode. */
void nrf2401_sleep(void);

/* Power up RF section.  Only use when in RX mode */
void nrf2401_wake(void);


/*
 * In more advanced situations you might need these (also see
 *  nrf2401_config_bits.h for low-level configuration layout & access)
 */


/* Load current configuration into module */
void nrf2401_load_config(void);

/* Enable transmit/receive mode (1 = rx) */
void nrf2401_set_rxtx(uint8_t rxen);

/* Wait for data to be transmitted (static wait time, just 1us per bit) */
void nrf2401_tx_wait(void);


/*
 * If using second RX channel you'll need these
 */
 

#ifdef CONFIG_NRF2401_RX2

/*
 * Routines for using receive channel 2
 */

/* Returns !=0 when data ready for channel 2 */
uint8_t nrf2401_available2(void);

/* Receive a packet on channel 2 (call when data ready on ch2) */
uint8_t nrf2401_recv2(uint8_t *packet)

#endif


#ifdef __cplusplus
};
#endif

#endif /* #ifndef NRF2401_H ... */
