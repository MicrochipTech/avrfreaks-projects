/*================================================================================
	2.4 Ghz R/C Control
	rc_rf.c

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

#include <stdint.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "rc_global.h"
#include "rc_rf.h"

uint8_t addr[RC_RF_ADDR_LENGTH]  = {0x40,0x50,0x60,0x70,0x80}; 
volatile uint8_t nRfIRQ = 0;

// Initializes SPI subsystem
void SPI_Init()
{
	// Set MOSI and SCK output, all others input
	RC_SPI_DDR = _BV(RC_SPI_DD_MOSI) | _BV(RC_SPI_DD_SCK) | _BV(RC_SPI_DD_SS);

	// Enable SPI, Master, set clock rate fck/4
	SPCR = _BV(SPE) | _BV(MSTR);
}

//  Writes one byte to nRF24L01, and return the byte read
//  from nRF24L01 during write, according to SPI protocol
uint8_t SPI_RW(uint8_t byte)
{
	// Start transmission 
	SPDR = byte;

	// Wait for transmission complete
	while(!(SPSR & _BV(SPIF)));

	// Return status register
    return SPDR;
}

//  Read one byte from nRF24L01 register, 'reg'
uint8_t SPI_Read(uint8_t reg)
{
	uint8_t reg_val;

	// Start SPI comm, set CSN/SS low
	PORTB &= ~(_BV(RC_SPI_PIN_SS));

	// Select register to read from..
	SPI_RW(reg);                                    

	// ..then read register value
	reg_val = SPI_RW(0);

	// Stop SPI comm, set CSN/SS high
	PORTB |= _BV(RC_SPI_PIN_SS);

	return(reg_val);
}

//  Writes value 'value' to register 'reg'
uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;

	// Start SPI comm, set CSN/SS low
	PORTB &= ~(_BV(RC_SPI_PIN_SS));

	// Select register
	status = SPI_RW(reg);

	// Write value to it
	SPI_RW(value); 

	// Stop SPI comm, set CSN/SS high
	PORTB |= _BV(RC_SPI_PIN_SS);

	return(status);
}

//  Writes contents of buffer '*pBuf' to nRF24L01
uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status,
			i;

	// Start SPI comm, set CSN/SS low
	PORTB &= ~(_BV(RC_SPI_PIN_SS));

	// Select register to write to and read status byte
	status = SPI_RW(reg); 

	// Write buffer
	for(i = 0; i < bytes; i++) {
		SPI_RW(*pBuf++);
	}

	// Stop SPI comm, set CSN/SS high
	PORTB |= _BV(RC_SPI_PIN_SS);

	return(status);
}

//  Reads 'bytes' #of bytes from register 'reg'
uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status,
			i;

	// Start SPI comm, set CSN/SS low
	PORTB &= ~(_BV(RC_SPI_PIN_SS));

	// Select register to write to and read status byte
	status = SPI_RW(reg);                           

	for(i = 0; i < bytes; i++) {
		pBuf[i] = SPI_RW(0);                   
	}

	// Stop SPI comm, set CSN/SS high
	PORTB |= _BV(RC_SPI_PIN_SS);

	return(status);
}

void L01_SET_CH(uint8_t rf_ch)                                
{
	SPI_RW_Reg(L01_CMD_WRITE_REG + L01_REG_RF_CH, rf_ch);
}

uint8_t L01_GET_CH(void)                                      
{
	return SPI_Read(L01_REG_RF_CH);
}

void L01_SETUP_RETR(uint8_t val)
{
	SPI_RW_Reg(L01_CMD_WRITE_REG + L01_REG_SETUP_RETR, val);
}
void L01_SETUP_RF(uint8_t val)
{
	SPI_RW_Reg(L01_CMD_WRITE_REG + L01_REG_RF_SETUP, val);
}

void L01_WR_AW(uint8_t aWidth)
{
	SPI_RW_Reg(L01_CMD_WRITE_REG + L01_REG_SETUP_AW, aWidth);
}

void L01_WR_TX_ADDR(uint8_t *pBuf, int8_t aWidth)
{
	SPI_Write_Buf(L01_CMD_WRITE_REG + L01_REG_TX_ADDR, pBuf, aWidth);
}

void L01_WR_RX_ADDR_n(uint8_t pipe, uint8_t *pBuf, int8_t aWidth)
{
	SPI_Write_Buf(L01_CMD_WRITE_REG + L01_REG_RX_ADDR_P0 + pipe, pBuf, aWidth);
}

uint8_t L01_RD_RX_PW_n(uint8_t pipe)                                  
{
	return SPI_Read(L01_REG_RX_PW_P0 + pipe);
}

void L01_WR_RX_PW_n(uint8_t pipe, uint8_t plWidth)                    
{
	SPI_RW_Reg(L01_CMD_WRITE_REG + L01_REG_RX_PW_P0 + pipe, plWidth);
}

uint16_t L01_RD_RX_Pload(uint8_t *pBuf)                              
{
	uint8_t plWidth, 
			pipe;

	// Read current pipe's payload width
	plWidth = L01_RD_RX_PW_n(pipe = L01_GET_CURR_PIPENO()); 

	// Then get RX data
	SPI_Read_Buf(L01_CMD_RD_RX_PLOAD, pBuf, plWidth); 

	// return pipe# & pipe#.plWidth
	return ((pipe << 8) + plWidth);                               
}

void L01_WR_TX_Pload(uint8_t *pBuf, uint8_t plWidth)               
{
	SPI_Write_Buf(L01_CMD_WR_TX_PLOAD, pBuf, plWidth);
}

void L01_EN_AA_n(uint8_t pipe)
{
	SPI_RW_Reg(L01_CMD_WRITE_REG + L01_REG_EN_AA, (1<<pipe)); 
}

void L01_EN_RXADDR_n(uint8_t pipe)
{
	SPI_RW_Reg(L01_CMD_WRITE_REG + L01_REG_EN_RXADDR, (1<<pipe));
}

void L01_FLUSH_RX(void)                                         
{
	SPI_RW_Reg(L01_CMD_FLUSH_RX,0);
}

void L01_FLUSH_TX(void)                                         
{
	SPI_RW_Reg(L01_CMD_FLUSH_TX, 0);
}

void L01_WR_CONFIG(uint8_t config)
{
	SPI_RW_Reg(L01_CMD_WRITE_REG + L01_REG_CONFIG, config); 
}

uint8_t L01_GET_STATUS(void)                                       
{
	return SPI_Read(L01_REG_STATUS);
}

uint8_t L01_GET_FIFO(void)
{
	return SPI_Read(L01_REG_FIFO_STATUS);
}

uint8_t L01_GET_CURR_PIPENO(void)                              
{
	return ((L01_GET_STATUS() & L01_MASK_RX_P_NO) >> 1);
}

uint8_t L01_CLEAR_IRQ(uint8_t irq_flag)                               
{
	return SPI_RW_Reg(L01_CMD_WRITE_REG + L01_REG_STATUS, irq_flag);
}

void RF_RX_Init(void)
{   
	// Inilialize SPI comm with nRF24L01
	SPI_Init();

	// Set CE pin as output
	DDRB |= _BV(RC_RF_PIN_CE);

	// Enable pull-up on INT0 pin
	PORTD |= _BV(RC_RF_PIN_IRQ);

	// Enable INT0 on falling edge
	EICRA |= _BV(ISC01);

	// Enable INT0
	EIMSK |= _BV(INT0);

	// Disable CE
	PORTB &= ~_BV(RC_RF_PIN_CE);

	// Configure TX/RX address length
	L01_WR_AW(RC_RF_ADDR_LENGTH-2);

	// Configure tx/rx address on rx pipe 0
	L01_WR_RX_ADDR_n(0, addr, RC_RF_ADDR_LENGTH);

	// Configure PTX tx payload width
	L01_WR_RX_PW_n(0, RC_RF_TX_PLOAD_WIDTH);

	// Enable Auto-Ack on rx pipe 0
	L01_EN_AA_n(0);

	// Enable rx pipe 0
	L01_EN_RXADDR_n(0);

	// Set RF channel
	L01_SET_CH(RC_RF_CH);

	// RF setup, 1Mpbs, 0 dBM
	L01_SETUP_RF(RC_RF_SETUP);

	// Enable CRC, CRC=2 byte, Power UP, RX mode
	L01_WR_CONFIG(RC_RF_CONFIG);

	// Flush rx FIFO
	L01_FLUSH_RX();

	// Clear IRQ flags in STATUS
	L01_CLEAR_IRQ(L01_MASK_IRQ_FLAGS);

	// Enable CE
	PORTB |= _BV(RC_RF_PIN_CE);
}

void RF_TX_Init(void)
{
	// Inilialize SPI comm with nRF24L01
	SPI_Init();

	// Set CE pin as output
	DDRB |= _BV(RC_RF_PIN_CE);

	// Enable pull-up on INT0 pin
	PORTD |= _BV(RC_RF_PIN_IRQ);

	// Enable INT0 on falling edge
	EICRA |= _BV(ISC01);

	// Enable INT0
	EIMSK |= _BV(INT0);

	// Disable CE
	PORTB &= ~_BV(RC_RF_PIN_CE);

	// Configure TX/RX address length
	L01_WR_AW(RC_RF_ADDR_LENGTH-2);

	// Configure tx address
	L01_WR_TX_ADDR(addr, RC_RF_ADDR_LENGTH);

	// Configure rx address on rx pipe 0
	L01_WR_RX_ADDR_n(0, addr, RC_RF_ADDR_LENGTH);

	// Enable Auto-Ack on rx pipe 0
	L01_EN_AA_n(0);

	// Enable rx pipe 0
	L01_EN_RXADDR_n(0);

	// Setup re-transmit
	L01_SETUP_RETR(RC_RF_NUM_RETR);

	// Set RF channel
	L01_SET_CH(RC_RF_CH);

	// RF setup, 1Mpbs, 0 dBM
	L01_SETUP_RF(RC_RF_SETUP);

	// Enable CRC, CRC=2 byte, Power UP, TX mode
	L01_WR_CONFIG(RC_RF_CONFIG);

	// Clear IRQ flags in STATUS
	L01_CLEAR_IRQ(L01_MASK_IRQ_FLAGS);
}

ISR (INT0_vect)
{

#ifdef _DEBUG
    PORTB |= _BV(RC_DEBUG_PIN_1); 
#endif

	switch(L01_GET_STATUS() & L01_MASK_IRQ_FLAGS) {

	case L01_MASK_RX_DR_FLAG:
		nRfIRQ |= L01_MASK_RX_DR_FLAG; 
    break;

    case L01_MASK_TX_DS_FLAG:
#ifdef _DEBUG
		PORTD |= _BV(RC_DEBUG_PIN_2); 
#endif
		nRfIRQ |= L01_MASK_TX_DS_FLAG;                                         
    break;

	case L01_MASK_MAX_RT_FLAG:
		nRfIRQ |= L01_MASK_MAX_RT_FLAG;
    break;

	default:
		  L01_CLEAR_IRQ(L01_MASK_IRQ_FLAGS);
    break;
	}
}
