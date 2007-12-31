/*================================================================================
	2.4 Ghz R/C Control
	rc_rf.h

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
#ifndef _RF_API_H_
#define _RF_API_H_
 
// SPI(nRF24L01) commands
#define L01_CMD_READ_REG        0x00  // Define read command to register
#define L01_CMD_WRITE_REG       0x20  // Define write command to register
#define L01_CMD_RD_RX_PLOAD     0x61  // Define RX payload register address
#define L01_CMD_WR_TX_PLOAD     0xA0  // Define TX payload register address
#define L01_CMD_FLUSH_TX        0xE1  // Define flush TX register command
#define L01_CMD_FLUSH_RX        0xE2  // Define flush RX register command
#define L01_CMD_REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define L01_CMD_NOP             0xFF  // Define No Operation, might be used to read status register

// SPI(nRF24L01) registers(addresses)
#define L01_REG_CONFIG          0x00  // 'Config' register address
#define L01_REG_EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define L01_REG_EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define L01_REG_SETUP_AW        0x03  // 'Setup address width' register address
#define L01_REG_SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define L01_REG_RF_CH           0x05  // 'RF channel' register address
#define L01_REG_RF_SETUP        0x06  // 'RF setup' register address
#define L01_REG_STATUS          0x07  // 'Status' register address
#define L01_REG_OBSERVE_TX      0x08  // 'Observe TX' register address
#define L01_REG_CD              0x09  // 'Carrier Detect' register address
#define L01_REG_RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define L01_REG_RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define L01_REG_RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define L01_REG_RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define L01_REG_RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define L01_REG_RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define L01_REG_TX_ADDR         0x10  // 'TX address' register address
#define L01_REG_RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define L01_REG_RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define L01_REG_RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define L01_REG_RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define L01_REG_RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define L01_REG_RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define L01_REG_FIFO_STATUS     0x17  // 'FIFO Status Register' register address

#define L01_MASK_RX_P_NO		0x0E
#define L01_MASK_IRQ_FLAGS      0x70
#define L01_MASK_RX_DR_FLAG     0x40
#define L01_MASK_TX_DS_FLAG     0x20
#define L01_MASK_MAX_RT_FLAG    0x10
#define L01_MASK_RX_FIFO_EMPTY  0x01

// Init SPI
void 	SPI_Init		(); 

// Single SPI read/write	
uint8_t SPI_RW			(uint8_t byte);

// Read/Write one byte from nRF24L01
uint8_t SPI_Read		(uint8_t reg);                         
uint8_t SPI_RW_Reg		(uint8_t reg, uint8_t value);

// Read/Write multiple bytes to a register   
uint8_t SPI_Read_Buf	(uint8_t reg, uint8_t *pBuf, uint8_t bytes);
uint8_t SPI_Write_Buf	(uint8_t reg, uint8_t *pBuf, uint8_t bytes); 

// nRF24L01  APIs
uint8_t 	L01_GET_CH			(void);
void 		L01_SET_CH			(uint8_t rf_ch);

void 		L01_SETUP_RETR		(uint8_t val);
void 		L01_SETUP_RF		(uint8_t val);

void 		L01_WR_AW			(uint8_t aWidth);

void 		L01_WR_TX_ADDR		(uint8_t *pBuf, int8_t aWidth);
void 		L01_WR_RX_ADDR_n	(uint8_t pipe, uint8_t *pBuf, int8_t aWidth);

uint8_t 	L01_RD_RX_PW_n		(uint8_t pipe);
void 		L01_WR_RX_PW_n		(uint8_t pipe, uint8_t plWidth);

uint16_t 	L01_RD_RX_Pload		(uint8_t *pBuf);
void 		L01_WR_TX_Pload		(uint8_t *pBuf, uint8_t plWidth);

void 		L01_EN_AA_n			(uint8_t pipe);
void 		L01_EN_RXADDR_n		(uint8_t pipe);

void 		L01_FLUSH_RX		(void);
void 		L01_FLUSH_TX		(void);

void 		L01_WR_CONFIG		(uint8_t config);

uint8_t 	L01_GET_STATUS		(void);
uint8_t	 	L01_GET_FIFO		(void);
uint8_t 	L01_GET_CURR_PIPENO	(void);

uint8_t 	L01_CLEAR_IRQ		(uint8_t irq_flag);

void 		RF_RX_Init			(void);
void 		RF_TX_Init			(void);

extern volatile uint8_t nRfIRQ;
#endif
