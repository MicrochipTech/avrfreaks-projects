/*================================================================================
	2.4 Ghz R/C Control
	rc_global.h

	Copyright ©2007 Ajay Grewal

THIS SOFTWARE AND DOCUMENTATION IS PROVIDED "AS IS," AND COPYRIGHT HOLDER MAKES NO
REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO,
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR THAT THE
USE OF THE SOFTWARE OR DOCUMENTATION WILL NOT INFRINGE ANY THIRD PARTY PATENTS,
COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.

COPYRIGHT HOLDER WILL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL OR
CONSEQUENTIAL DAMAGES ARISING OUT OF ANY USE OF THE SOFTWARE OR DOCUMENTATION.

You may freely copy and redistribute this software if no fee is charged for use,
copying or distribution.  Such redistributions must retain the above copyright
notice and disclaimer.
================================================================================*/
#ifndef _RC_GLOBAL_H_
#define _RC_GLOBAL_H_

// Debug
#define RC_SERIAL_BAUD			9600
#define RC_SERIAL_BUF_SIZE 		128
#define RC_DEBUG_PIN_1			PB1
#define RC_DEBUG_PIN_2			PD5

// TIMER
#define RC_TIMER0_TOP			160

// HMI
#define RC_HMI_READCOUNT	   	1000
#define RC_HMI_TOTAL_INPUTS		6
#define RC_HMI_NONCOND_INPUTS	2
#define RC_HMI_AVG_BUF_SIZE		4
#define RC_HMI_JOY_X_BIAS		30
#define RC_HMI_JOY_Y_BIAS		30
#define RC_HMI_N_BIAS_SPAN		5
#define RC_HMI_NUM_CHANNELS		5

// RF
#define RC_RF_ADDR_LENGTH   	5
#define RC_RF_TX_PLOAD_WIDTH  	5
#define RC_RF_RX_PLOAD_WIDTH  	5
#define RC_RF_PIN_CE 			PB0
#define RC_RF_PIN_IRQ			PD2
#define RC_RF_CH				40
#define RC_RF_SETUP				0x0E
#define RC_RF_CONFIG			0x0E
#define RC_RF_NUM_RETR			0x0A

// SPI
#define RC_SPI_DDR				DDRB
#define RC_SPI_DD_MOSI			DDB3
#define RC_SPI_DD_SCK			DDB5
#define RC_SPI_DD_SS 			DDB2
#define RC_SPI_PIN_SS 			PB2

#endif
