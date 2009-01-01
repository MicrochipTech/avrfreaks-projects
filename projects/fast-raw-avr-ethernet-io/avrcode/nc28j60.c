// Written by Rogier Schouten http://www.rogiershikes.tk
// Based on code by Guido Socher http://www.tuxgraphics.org
// License: GPL V2
#include <avr/io.h>
#ifndef F_CPU
#define F_CPU 12500000UL  // 12.5 MHz
#endif
#include <util/delay.h>
#include "ethernet.h"
#include "enc28j60.h"

// ENC28J60 Control Registers
// Control register definitions are a combination of address,
// bank number, and Ethernet/MAC/PHY indicator bits.
// - Register address        (bits 0-4)
// - Bank number        (bits 5-6)
// - MAC/PHY indicator        (bit 7)
#define ADDR_MASK        0x1F
#define BANK_MASK        0x60
#define SPRD_MASK        0x80
// All-bank registers
#define EIE              0x1B
#define EIR              0x1C
#define ESTAT            0x1D
#define ECON2            0x1E
#define ECON1            0x1F
// Bank 0 registers
#define ERDPTL           (0x00|0x00)
#define ERDPTH           (0x01|0x00)
#define EWRPTL           (0x02|0x00)
#define EWRPTH           (0x03|0x00)
#define ETXSTL           (0x04|0x00)
#define ETXSTH           (0x05|0x00)
#define ETXNDL           (0x06|0x00)
#define ETXNDH           (0x07|0x00)
#define ERXSTL           (0x08|0x00)
#define ERXSTH           (0x09|0x00)
#define ERXNDL           (0x0A|0x00)
#define ERXNDH           (0x0B|0x00)
#define ERXRDPTL         (0x0C|0x00)
#define ERXRDPTH         (0x0D|0x00)
#define ERXWRPTL         (0x0E|0x00)
#define ERXWRPTH         (0x0F|0x00)
#define EDMASTL          (0x10|0x00)
#define EDMASTH          (0x11|0x00)
#define EDMANDL          (0x12|0x00)
#define EDMANDH          (0x13|0x00)
#define EDMADSTL         (0x14|0x00)
#define EDMADSTH         (0x15|0x00)
#define EDMACSL          (0x16|0x00)
#define EDMACSH          (0x17|0x00)
// Bank 1 registers
#define EHT0             (0x00|0x20)
#define EHT1             (0x01|0x20)
#define EHT2             (0x02|0x20)
#define EHT3             (0x03|0x20)
#define EHT4             (0x04|0x20)
#define EHT5             (0x05|0x20)
#define EHT6             (0x06|0x20)
#define EHT7             (0x07|0x20)
#define EPMM0            (0x08|0x20)
#define EPMM1            (0x09|0x20)
#define EPMM2            (0x0A|0x20)
#define EPMM3            (0x0B|0x20)
#define EPMM4            (0x0C|0x20)
#define EPMM5            (0x0D|0x20)
#define EPMM6            (0x0E|0x20)
#define EPMM7            (0x0F|0x20)
#define EPMCSL           (0x10|0x20)
#define EPMCSH           (0x11|0x20)
#define EPMOL            (0x14|0x20)
#define EPMOH            (0x15|0x20)
#define EWOLIE           (0x16|0x20)
#define EWOLIR           (0x17|0x20)
#define ERXFCON          (0x18|0x20)
#define EPKTCNT          (0x19|0x20)
// Bank 2 registers
#define MACON1           (0x00|0x40|0x80)
#define MACON2           (0x01|0x40|0x80)
#define MACON3           (0x02|0x40|0x80)
#define MACON4           (0x03|0x40|0x80)
#define MABBIPG          (0x04|0x40|0x80)
#define MAIPGL           (0x06|0x40|0x80)
#define MAIPGH           (0x07|0x40|0x80)
#define MACLCON1         (0x08|0x40|0x80)
#define MACLCON2         (0x09|0x40|0x80)
#define MAMXFLL          (0x0A|0x40|0x80)
#define MAMXFLH          (0x0B|0x40|0x80)
#define MAPHSUP          (0x0D|0x40|0x80)
#define MICON            (0x11|0x40|0x80)
#define MICMD            (0x12|0x40|0x80)
#define MIREGADR         (0x14|0x40|0x80)
#define MIWRL            (0x16|0x40|0x80)
#define MIWRH            (0x17|0x40|0x80)
#define MIRDL            (0x18|0x40|0x80)
#define MIRDH            (0x19|0x40|0x80)
// Bank 3 registers
#define MAADR1           (0x00|0x60|0x80)
#define MAADR0           (0x01|0x60|0x80)
#define MAADR3           (0x02|0x60|0x80)
#define MAADR2           (0x03|0x60|0x80)
#define MAADR5           (0x04|0x60|0x80)
#define MAADR4           (0x05|0x60|0x80)
#define EBSTSD           (0x06|0x60)
#define EBSTCON          (0x07|0x60)
#define EBSTCSL          (0x08|0x60)
#define EBSTCSH          (0x09|0x60)
#define MISTAT           (0x0A|0x60|0x80)
#define EREVID           (0x12|0x60)
#define ECOCON           (0x15|0x60)
#define EFLOCON          (0x17|0x60)
#define EPAUSL           (0x18|0x60)
#define EPAUSH           (0x19|0x60)
// PHY registers
#define PHCON1           0x00
#define PHSTAT1          0x01
#define PHHID1           0x02
#define PHHID2           0x03
#define PHCON2           0x10
#define PHSTAT2          0x11
#define PHIE             0x12
#define PHIR             0x13
#define PHLCON           0x14

// ENC28J60 ERXFCON Register Bit Definitions
#define ERXFCON_UCEN     0x80
#define ERXFCON_ANDOR    0x40
#define ERXFCON_CRCEN    0x20
#define ERXFCON_PMEN     0x10
#define ERXFCON_MPEN     0x08
#define ERXFCON_HTEN     0x04
#define ERXFCON_MCEN     0x02
#define ERXFCON_BCEN     0x01
// ENC28J60 EIE Register Bit Definitions
#define EIE_INTIE        0x80
#define EIE_PKTIE        0x40
#define EIE_DMAIE        0x20
#define EIE_LINKIE       0x10
#define EIE_TXIE         0x08
#define EIE_WOLIE        0x04
#define EIE_TXERIE       0x02
#define EIE_RXERIE       0x01
// ENC28J60 EIR Register Bit Definitions
#define EIR_PKTIF        0x40
#define EIR_DMAIF        0x20
#define EIR_LINKIF       0x10
#define EIR_TXIF         0x08
#define EIR_WOLIF        0x04
#define EIR_TXERIF       0x02
#define EIR_RXERIF       0x01
// ENC28J60 ESTAT Register Bit Definitions
#define ESTAT_INT        0x80
#define ESTAT_LATECOL    0x10
#define ESTAT_RXBUSY     0x04
#define ESTAT_TXABRT     0x02
#define ESTAT_CLKRDY     0x01
// ENC28J60 ECON2 Register Bit Definitions
#define ECON2_AUTOINC    0x80
#define ECON2_PKTDEC     0x40
#define ECON2_PWRSV      0x20
#define ECON2_VRPS       0x08
// ENC28J60 ECON1 Register Bit Definitions
#define ECON1_TXRST      0x80
#define ECON1_RXRST      0x40
#define ECON1_DMAST      0x20
#define ECON1_CSUMEN     0x10
#define ECON1_TXRTS      0x08
#define ECON1_RXEN       0x04
#define ECON1_BSEL1      0x02
#define ECON1_BSEL0      0x01
// ENC28J60 MACON1 Register Bit Definitions
#define MACON1_LOOPBK    0x10
#define MACON1_TXPAUS    0x08
#define MACON1_RXPAUS    0x04
#define MACON1_PASSALL   0x02
#define MACON1_MARXEN    0x01
// ENC28J60 MACON2 Register Bit Definitions
#define MACON2_MARST     0x80
#define MACON2_RNDRST    0x40
#define MACON2_MARXRST   0x08
#define MACON2_RFUNRST   0x04
#define MACON2_MATXRST   0x02
#define MACON2_TFUNRST   0x01
// ENC28J60 MACON3 Register Bit Definitions
#define MACON3_PADCFG2   0x80
#define MACON3_PADCFG1   0x40
#define MACON3_PADCFG0   0x20
#define MACON3_TXCRCEN   0x10
#define MACON3_PHDRLEN   0x08
#define MACON3_HFRMLEN   0x04
#define MACON3_FRMLNEN   0x02
#define MACON3_FULDPX    0x01
// ENC28J60 MACON4 Register Bit Definitions
#define MACON4_DEFER     0x40
#define MACON4_BPEN   	 0x20
#define MACON4_NOBKOFF   0x10

// ENC28J60 MICMD Register Bit Definitions
#define MICMD_MIISCAN    0x02
#define MICMD_MIIRD      0x01
// ENC28J60 MISTAT Register Bit Definitions
#define MISTAT_NVALID    0x04
#define MISTAT_SCAN      0x02
#define MISTAT_BUSY      0x01
// ENC28J60 PHY PHCON1 Register Bit Definitions
#define PHCON1_PRST      0x8000
#define PHCON1_PLOOPBK   0x4000
#define PHCON1_PPWRSV    0x0800
#define PHCON1_PDPXMD    0x0100
// ENC28J60 PHY PHSTAT1 Register Bit Definitions
#define PHSTAT1_PFDPX    0x1000
#define PHSTAT1_PHDPX    0x0800
#define PHSTAT1_LLSTAT   0x0004
#define PHSTAT1_JBSTAT   0x0002
// ENC28J60 PHY PHSTAT2H Register Bit Definitions
#define PHSTAT2H_LSTAT    0x04
// ENC28J60 PHY PHCON2 Register Bit Definitions
#define PHCON2_FRCLINK   0x4000
#define PHCON2_TXDIS     0x2000
#define PHCON2_JABBER    0x0400
#define PHCON2_HDLDIS    0x0100

// ENC28J60 Packet Control Byte Bit Definitions
#define PKTCTRL_PHUGEEN  0x08
#define PKTCTRL_PPADEN   0x04
#define PKTCTRL_PCRCEN   0x02
#define PKTCTRL_POVERRIDE 0x01

// SPI operation codes
#define ENC28J60_READ_CTRL_REG       0x00
#define ENC28J60_READ_BUF_MEM        0x3A
#define ENC28J60_WRITE_CTRL_REG      0x40
#define ENC28J60_WRITE_BUF_MEM       0x7A
#define ENC28J60_BIT_FIELD_SET       0x80
#define ENC28J60_BIT_FIELD_CLR       0xA0
#define ENC28J60_SOFT_RESET          0xFF

// Atmel port/pin definitions
#define ENC28J60_DDR  		DDRB
#define ENC28J60_PORT 		PORTB
#define ENC28J60_PIN_CS 	PB2
#define ENC28J60_PIN_MOSI 	PB3
#define ENC28J60_PIN_MISO 	PB4
#define ENC28J60_PIN_SCK 	PB5

// Buffer memory allocation within controlller.
// Assuming the controller is slower than the network, we allocate one transmit 
// packet and leave the rest for receiving.
// The RXSTART_INIT should be zero. See Rev. B4 Silicon Errata
#define RXSTART_INIT     0x0
#define RXSTOP_INIT      (0x1FFF-0x0600-2) // note: MUST be odd (see errata point 13)
#define TXSTART_INIT     (0x1FFF-0x0600)

// Maximum packet length: this software has a limit of 255 payload data bytes
// and then there is some ethernet overhead (srd, dst, len, fcs)
#define ENC28J60_MAX_PACKET_LEN ((uint16_t)273)

/// SPI handling macros
#define CSACTIVE ENC28J60_PORT &= ~_BV(ENC28J60_PIN_CS)
#define CSPASSIVE ENC28J60_PORT |= _BV(ENC28J60_PIN_CS)
#define waitspi() while (!( SPSR & (1 << SPIF)))

// -----------------------------------------------------------------------------

static uint8_t gBank;
static uint8_t gLastTransmitPayloadLen;
static uint16_t gNextPacketPtr; // position where next packet will be written

// -----------------------------------------------------------------------------

/// Read a control register in the controller assuming that the currently
/// selected bank is correct
uint8_t 
enc28j60ReadOp(
	uint8_t op, 
	uint8_t address)
{
	CSACTIVE;
	// issue read command
	SPDR = op | (address & ADDR_MASK);
	waitspi();
	// read data
	SPDR = 0x00;
	waitspi();
	// do dummy read if needed (for mac and mii, see datasheet page 29)
	if(address & 0x80)
	{
		SPDR = 0x00;
		waitspi();
	}
	// release CS
	CSPASSIVE;
	return(SPDR);
}

#define enc28j60ReadReg(ADRESS) 	enc28j60ReadOp(ENC28J60_READ_CTRL_REG, ADRESS)

// -----------------------------------------------------------------------------

/// Write a control register in the controller assuming that the currently
/// selected bank is correct
void 
enc28j60WriteOp(
	uint8_t op, 
	uint8_t address, 
	uint8_t data)
{
	CSACTIVE;
	// issue write command
	SPDR = op | (address & ADDR_MASK);
	waitspi();
	// write data
	SPDR = data;
	waitspi();
	CSPASSIVE;
}

#define enc28j60WriteReg(ADRESS, DATA) 	enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, ADRESS, DATA)

// -----------------------------------------------------------------------------

/// Set the control register bank to use  
void 
enc28j60SetBank(
	uint8_t bank)
{
	// set the bank
	if (bank != gBank)
	{
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
		enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (bank & 0x03));
		gBank = bank;
	}		
}

// -----------------------------------------------------------------------------

/// Read from the Buffer Memory
void 
enc28j60ReadBuffer(
	uint8_t len, 
	uint8_t* data)
{
	CSACTIVE;
	// issue read command
	SPDR = ENC28J60_READ_BUF_MEM;
	waitspi();
	while(len)
	{
		len--;
		// read data
		SPDR = 0x00;
		waitspi();
		*data = SPDR;
		data++;
	}
	CSPASSIVE;
}

// -----------------------------------------------------------------------------

/// Write to the Buffer Memory
void 
enc28j60WriteBuffer(
	uint8_t len, 
	uint8_t* data)
{
	CSACTIVE;
	// issue write command
	SPDR = ENC28J60_WRITE_BUF_MEM;
	waitspi();
	while(len)
	{
		len--;
		// write data
		SPDR = *data;
		data++;
		waitspi();
	}
	CSPASSIVE;
}

// -----------------------------------------------------------------------------

/// Read upper 8 bits of a PHY register
uint8_t 
enc28j60PhyReadH(
	uint8_t address)
{

	// Set the right address and start the register read operation
	enc28j60SetBank(2);
	enc28j60WriteReg(MIREGADR, address);
	enc28j60WriteReg(MICMD, MICMD_MIIRD);
	_delay_us(15);

	// wait until the PHY read completes
	enc28j60SetBank(3);
	while(enc28j60ReadReg(MISTAT) & MISTAT_BUSY)
		;

	// reset reading bit
	enc28j60SetBank(2);
	enc28j60WriteReg(MICMD, 0x00);
	return (enc28j60ReadReg(MIRDH));
}

// -----------------------------------------------------------------------------

/// Write a PHY register
void 
enc28j60PhyWrite(
	uint8_t address, 
	uint16_t data)
{
	enc28j60SetBank(2);
	
	// set the PHY register address
	enc28j60WriteReg(MIREGADR, address);
	// write the PHY data
	enc28j60WriteReg(MIWRL, data);
	enc28j60WriteReg(MIWRH, data>>8);
	// wait until the PHY write completes
	enc28j60SetBank(3);
	while(enc28j60ReadReg(MISTAT) & MISTAT_BUSY)
	{
		_delay_us(15);
	}
}

// -----------------------------------------------------------------------------

void 
enc28j60Init(
	uint8_t* macaddr)
{
	gBank = 0xFF; // non-existent 
	
	// Initialize atmel I/O
	ENC28J60_DDR &= ~_BV(ENC28J60_PIN_MISO);
	ENC28J60_DDR |= (_BV(ENC28J60_PIN_CS) | _BV(ENC28J60_PIN_MOSI) | _BV(ENC28J60_PIN_SCK));
	CSPASSIVE; 
	ENC28J60_PORT &= ~_BV(ENC28J60_PIN_MOSI);
	ENC28J60_PORT &= ~_BV(ENC28J60_PIN_SCK);

	// Initialize atmel SPI interface
	// master mode and Fosc/2 clock:
	SPCR = (1<<SPE)|(1<<MSTR);
	SPSR |= (1<<SPI2X);
	
	// Reset controller
	enc28j60WriteOp(ENC28J60_SOFT_RESET, 0x1F, ENC28J60_SOFT_RESET);
	_delay_ms(50); // Note that polling CLKRDY does not work. See Rev. B4 Silicon Errata point. Just wait.

	// A.S.A.P. setup CLKout: 2 is 12.5MHz:
	enc28j60SetBank(3);
	enc28j60WriteReg(ECOCON, 2);
	
	// Do bank 0 stuff
	enc28j60SetBank(0);
	// Initialize receive buffer
	// 16-bit transfers, must write low byte first
	// Set receive buffer start address
	gNextPacketPtr = RXSTART_INIT;
	// Rx start
	enc28j60WriteReg(ERXSTL, RXSTART_INIT&0xFF);
	enc28j60WriteReg(ERXSTH, RXSTART_INIT>>8);
	// Set receive pointer address
	enc28j60WriteReg(ERXRDPTL, RXSTART_INIT&0xFF);
	enc28j60WriteReg(ERXRDPTH, RXSTART_INIT>>8);
	// RX end
	enc28j60WriteReg(ERXNDL, RXSTOP_INIT&0xFF);
	enc28j60WriteReg(ERXNDH, RXSTOP_INIT>>8);
	// TX start
	enc28j60WriteReg(ETXSTL, TXSTART_INIT&0xFF);
	enc28j60WriteReg(ETXSTH, TXSTART_INIT>>8);
	// TX end (initialize for a packet with a payload of 1 byte)
	uint16_t address = (TXSTART_INIT + ETH_HEADER_LEN + 1);
	enc28j60WriteReg(ETXNDL, address & 0xFF);
	enc28j60WriteReg(ETXNDH, address >> 8);
	// Prepare the parts of the transmit packet that never change 
	// Write per-packet control byte (0x00 means use macon3 settings)
	enc28j60WriteReg(EWRPTL, (TXSTART_INIT) & 0xFF);
	enc28j60WriteReg(EWRPTH, (TXSTART_INIT) >> 8);
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
	// Write broadcast address as DST MAC 
	uint8_t i = 0;
	while (i < 6)
	{
		enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0xFF);	
		i++;
	}
	// Set our MAC address as the SRC MAC into the transmit buffer
	// Set the write pointer to start of transmit buffer area
	enc28j60WriteBuffer(6, macaddr);
	// First EtherType/length byte is always 0, initialize second byte to 1 
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);	
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x01);	
	gLastTransmitPayloadLen = 1;
	
	// Do bank 1 stuff, packet filter: 
	enc28j60SetBank(1);
	// Only allow unicast packets destined for us and that have a correct CRC
	enc28j60WriteReg(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN);

	// Do bank 2 stuff
	enc28j60SetBank(2);
	// Enable MAC receive, disable flow control (only needed in full-duplex)
	enc28j60WriteReg(MACON1, MACON1_MARXEN);
	// Bring MAC out of reset
	enc28j60WriteReg(MACON2, 0x00);
	// Enable automatic padding to 60bytes and CRC operations
	// Also, force half-duplex operation
	enc28j60WriteReg(MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
	// Half-duplex only: back-off settings
	enc28j60WriteReg(MACON4, MACON4_DEFER|MACON4_BPEN|MACON4_NOBKOFF);
	// Set the maximum packet size which the controller will accept
	// Do not send packets longer than MAX_FRAMELEN:
	enc28j60WriteReg(MAMXFLL, ENC28J60_MAX_PACKET_LEN & 0xFF);
	enc28j60WriteReg(MAMXFLH, ENC28J60_MAX_PACKET_LEN >> 8);
	// set inter-frame gap (non-back-to-back)
	enc28j60WriteReg(MAIPGL, 0x12);
	enc28j60WriteReg(MAIPGH, 0x0C);
	// set inter-frame gap (back-to-back)
	enc28j60WriteReg(MABBIPG, 0x12);
	
	// Do bank 3 stuff
	enc28j60SetBank(3);
	// Write MAC address; NOTE: MAC address in ENC28J60 is byte-backward
	enc28j60WriteReg(MAADR5, macaddr[0]);
	enc28j60WriteReg(MAADR4, macaddr[1]);
	enc28j60WriteReg(MAADR3, macaddr[2]);
	enc28j60WriteReg(MAADR2, macaddr[3]);
	enc28j60WriteReg(MAADR1, macaddr[4]);
	enc28j60WriteReg(MAADR0, macaddr[5]);
	
	// Do PHY stuff
	// Magjack leds configuration, LEDB=yellow LEDA=green
	// 0x476 is PHLCON LEDA=link status, LEDB=receive/transmit
	// enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
	enc28j60PhyWrite(PHLCON,0x476);
	// No loopback of transmitted frames
	enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);

	// Enable interrutps
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
	// Enable packet reception
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);	
}

// -----------------------------------------------------------------------------

uint8_t 
enc28j60IsLinkUp(void)
{
	return (enc28j60PhyReadH(PHSTAT2) & PHSTAT2H_LSTAT);
}

// -----------------------------------------------------------------------------

uint8_t enc28j60PollPacket(
	uint8_t maxlen,
	uint8_t* buf)
{
	uint16_t len;
	uint16_t currentPacketPtr = gNextPacketPtr;
	uint16_t address;

	// check if a packet has been received and buffered
	enc28j60SetBank(1);
	if (enc28j60ReadReg(EPKTCNT) == 0)
	{
		return (0);
	}

	enc28j60SetBank(0);
	
	// Somehow, the read pointer is NOT already at the start of the next packet
	// even though we leave it in that state
	enc28j60WriteReg(ERDPTL, (gNextPacketPtr));    
	enc28j60WriteReg(ERDPTH, (gNextPacketPtr)>>8);
	// Read the next packet pointer	
	gNextPacketPtr  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	gNextPacketPtr |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	// Read EtherType (we use this as a length field) (note +6 for receive vectors)
	// Set read pointer (taking care of wrap-around)
	address = currentPacketPtr + ETH_TYPE_H_P + 6;	
	if (address > RXSTOP_INIT)
	{
		address -= (RXSTOP_INIT - RXSTART_INIT + 1);
	}
	enc28j60WriteReg(ERDPTL, (address));
	enc28j60WriteReg(ERDPTH, (address)>>8);
	len = ((uint16_t)enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0))<<8;
	len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	// Drop packet if len > 255 (not our protocol)
	if (0 == (len & 0xFF00))
	{
		// limit retrieve length to maxlen, ignoring anything else
		if (((uint8_t)(len & 0x00FF)) >= maxlen)
		{
			len=maxlen;
		}
		// copy payload data from the receive buffer
		enc28j60ReadBuffer(len, buf);		
	}
	else
	{
		len = 0; // clear return value, packet was not for us
	}

   // Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out.
	// However, compensate for the errata point 13: enver write an even address!
	if ((gNextPacketPtr - 1 < RXSTART_INIT) 
		|| (gNextPacketPtr -1 > RXSTOP_INIT))
	{
		enc28j60WriteReg(ERXRDPTL, (RXSTOP_INIT)&0xFF);
		enc28j60WriteReg(ERXRDPTH, (RXSTOP_INIT)>>8);
	}
	else
	{
		enc28j60WriteReg(ERXRDPTL, (gNextPacketPtr-1)&0xFF);
		enc28j60WriteReg(ERXRDPTH, (gNextPacketPtr-1)>>8);
	}
	// Decrement the packet counter indicate we are done with this packet
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	
   return ((uint8_t)len);
}

//------------------------------------------------------------------------------

void 
enc28j60TransmitPacket(
	uint8_t len,
	uint8_t* buf)
{
	enc28j60SetBank(0);
	
	// (control byte, and SRC MAC have already been set during init)
	
#ifdef ENC28J60_HAS_PENDING_TRANSMIT_ON_TRANSMIT	
	// Check no transmit in progress
	while (enc28j60ReadOp(ENC28J60_READ_CTRL_REG, ECON1) & ECON1_TXRTS)
	{		
		// Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
		if( (enc28j60ReadReg(EIR) & EIR_TXERIF) )
		{
			enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
			enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
		}
	}
#else
	// Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
	if( (enc28j60ReadReg(EIR) & EIR_TXERIF) )
	{
		enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
	}
#endif
	uint16_t address;
	
	// Set the write pointer to the second byte of the EtherType field 
	// (note +1 for the control byte)
	address = TXSTART_INIT + ETH_TYPE_L_P + 1;
	if (gLastTransmitPayloadLen == len)
	{
		address++; // no need to write length
	}
	enc28j60WriteReg(EWRPTL, address & 0xFF);
	enc28j60WriteReg(EWRPTH, address >> 8);
	if (gLastTransmitPayloadLen != len)
	{
		// Write payload length 
		enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, len);
	}
	// Copy the payload into the transmit buffer
	enc28j60WriteBuffer(len, buf);
	// Set the TXND pointer to correspond to the payload size given
	// Optimize for situations where packet lengths will mostly be the same
	if (gLastTransmitPayloadLen != len)
	{
		address = (TXSTART_INIT + ETH_HEADER_LEN + len);
		enc28j60WriteReg(ETXNDL, address & 0xFF);
		enc28j60WriteReg(ETXNDH, address >> 8);
		gLastTransmitPayloadLen = len;
	}
	
	// Send the contents of the transmit buffer onto the network
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
	
}
	

