#ifndef MCP2515_H_
#define MCP2515_H_

#define MCPDEBUG (0)
#define MCPDEBUG_TXBUF (0)

#include <compat/deprecated.h>
#include <avr/io.h>
#include "spiX.h"
#include "mcp2515_defs.h"
#include "config.h"

#define MCP_N_TXBUFFERS (3)

#define MCP_RXBUF_0 (MCP_RXB0SIDH)
#define MCP_RXBUF_1 (MCP_RXB1SIDH)

#define MCP_TXBUF_0 (MCP_TXB0SIDH)
#define MCP_TXBUF_1 (MCP_TXB1SIDH)
#define MCP_TXBUF_2 (MCP_TXB2SIDH)

typedef struct {
	// identifier CAN_xxxID
	uint8_t  extended_identifier;
	// either extended (the 29 LSB) or standard (the 11 LSB)
	uint32_t identifier;
	// data length:
	uint8_t  dlc;
	uint8_t  dta[8];
	
	// used for receive only:
	// Received Remote Transfer Bit
	//  (0=no... 1=remote transfer request received)
	uint8_t  rtr;
	// Acceptence Filter that enabled the reception
	uint8_t  filhit;
} CanMessage;

#define MCP2515_SELECT()   sbi(MCP2515_DDR, MCP2515_PIN); cbi(MCP2515_PORT, MCP2515_PIN); _delay_us(100);
#define MCP2515_UNSELECT() sbi(MCP2515_DDR, MCP2515_PIN); sbi(MCP2515_PORT, MCP2515_PIN); _delay_us(100);

#define MCP2515_OK         (0)
#define MCP2515_FAIL       (1)
#define MCP_ALLTXBUSY      (2)

void mcp2515_reset(void);

uint8_t mcp2515_readRegister(const uint8_t address);
void mcp2515_setRegisterS(const uint8_t address, 
	const uint8_t values[],
	const uint8_t n);

void mcp2515_setRegister(const uint8_t address, 
	const uint8_t value);
void mcp2515_setRegisterS(const uint8_t address, 
	const uint8_t values[],
	const uint8_t n);
void mcp2515_modifyRegister(const uint8_t address, const uint8_t mask, 
	const uint8_t data);

uint8_t mcp2515_readStatus(void);
uint8_t mcp2515_RXStatus(void);

uint8_t mcp2515_setCANCTRL_Mode(const uint8_t newmode);

uint8_t mcp2515_configRate(const uint16_t canSpeed);
uint8_t mcp2515_init(const uint16_t canSpeed);

void mcp2515_write_can_id( const uint8_t mcp_addr, 
	const uint8_t ext, const uint32_t can_id );
void mcp2515_read_can_id( const uint8_t mcp_addr, 
	uint8_t* ext, uint32_t* can_id );

void mcp2515_write_canMsg( const uint8_t buffer_sidh_addr, 
	const CanMessage* msg);
void mcp2515_read_canMsg( const uint8_t buffer_sidh_addr,
	CanMessage* msg);

void mcp2515_start_transmit(const uint8_t mcp_addr);

uint8_t mcp2515_getNextFreeTXBuf(uint8_t *txbuf_n);

#ifdef MCPDEBUG
void mcp2515_dumpExtendedStatus(void);
#endif

#endif
