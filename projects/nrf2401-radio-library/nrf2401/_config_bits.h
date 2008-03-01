/*
 * nrf2401_config_bits.h
 *  Definitions, etc. for configuration settings of Nordic nRF2401 based
 *   radio modules 
 *
 *		(c) 2004-2008 Tymm Twillman <tymmothy@gmail.com>
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
 *
 */

#ifndef NRF2401_CONFIG_BITS_H
#define NRF2401_CONFIG_BITS_H

/* 
 * Default configuration settings 
 */

/* Default data length (bits) -- 24 bytes */
#define NRF2401_DEFAULT_DATA2_W 192 
#define NRF2401_DEFAULT_DATA1_W 192

/* Crystal freq (3 = 16 Mhz; that's how the trf24g comes) */
#define NRF2401_DEFAULT_XO_F 3


/*
 * Defines to help accessing bits in configuration
 */


/* Address width (0-40 bits) */
#define NRF2401_ADDR_W_SHIFT 2
#define NRF2401_ADDR_W_MASK 0xfc

/* CRC length (0: 8bit 1: 16bit) */
#define NRF2401_CRC_L_SHIFT 1
#define NRF2401_CRC_L_MASK 0x02

/* CRC enable (0: disable 1: enable) */
#define NRF2401_CRC_EN_SHIFT 0
#define NRF2401_CRC_EN_MASK 0x01

/* Enable RX channel 2 (0: disable 1:enable) */
#define NRF2401_RX2_EN_SHIFT 7
#define NRF2401_RX2_EN_MASK 0x80

/* Shockburst mode enable (0:disable 1:enable)
 *  This package does not support direct mode
 */
#define NRF2401_CM_SHIFT 6
#define NRF2401_CM_MASK 0x40

/* Speed select (0: 250kbit 1: 1mbit) */
#define NRF2401_RFDR_SB_SHIFT 5
#define NRF2401_RFDR_SB_MASK 0x20

/* Crystal selection */
#define NRF2401_XO_F_SHIFT 2
#define NRF2401_XO_F_MASK 0x1c

/* Power level (0: -20dB 1: -10dB 2: -5dB 3: 0dB) */
#define NRF2401_RF_PWR_SHIFT 0
#define NRF2401_RF_PWR_MASK 0x3

/* Channel (0-127) */
#define NRF2401_RF_CH_SHIFT 1
#define NRF2401_RF_CH_MASK 0xfe

/* RX enable (0: transmit 1: receive) */
#define NRF2401_RXEN_SHIFT 0
#define NRF2401_RXEN_MASK 1


/*
 * Configuration information; structure as defined in
 *  nRF2401 data sheet
 */

typedef struct {
    uint8_t data2_w;
    uint8_t data1_w;
    uint8_t addr2[5];
    uint8_t addr1[5];
    
    uint8_t addr_w_crc;  /* addr_w:6 crc_l:1 crc_en:1 (hi-lo) */
    uint8_t misc;        /*  rx2_en:1 cm:1 rfdr_sb:1 xo_f:3 rf_pwr:2 (hi-lo) */
    uint8_t rf_ch_rxen;  /* rf_ch:7 rxen:1 (hi-lo) */
} __attribute__ ((__packed__)) nrf2401_config_t;


/*
 * Master configuration structure
 */
extern nrf2401_config_t nrf2401_config;


#endif /* #ifndef NRF2401_CONFIG_BITS_H */
