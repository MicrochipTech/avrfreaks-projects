#ifndef _ENC28J60_H_
#define _ENC28J60_H_

#include "config.h"

#if USE_ENC28J60	

	unsigned char mymac[6];
	
	#include <avr/io.h>
	#include <util/delay.h>
	#include <avr/pgmspace.h>


	#define LO8(x) ((x)&0xFF)
	#define HI8(x) (((x)>>8)&0xFF)

	#define ENC28J60_DDR		DDRB
	#define ENC28J60_PORT		PORTB
	
	#define ENC28J60_PIN_SCK	7
	#define ENC28J60_PIN_MISO	6
	#define ENC28J60_PIN_MOSI	5
	#define ENC28J60_PIN_SS		3
	#define ENC28J60_PIN_CS		4

	#define ENC28J60_CS_LO()    ENC28J60_PORT &= ~(1<<ENC28J60_PIN_CS);
	#define ENC28J60_CS_HI()    ENC28J60_PORT |=  (1<<ENC28J60_PIN_CS);
	
	#define delay(x) for(unsigned long a=0;a<x;a++){asm("nop");};
	
	#if defined (__AVR_ATmega32__)
		#define ETH_INT_ENABLE 	GICR |= (1<<INT2)
		#define ETH_INT_DISABLE GICR &= ~(1<<INT2)
	#endif

	#if defined (__AVR_ATmega644__) || defined (__AVR_ATmega644P__)
		#define ETH_INT_ENABLE 	EIMSK |= (1<<INT2)
		#define ETH_INT_DISABLE EIMSK &= ~(1<<INT2)
	#endif
	
	#ifdef SPDR0
		#define SPDR	SPDR0
		#define SPCR	SPCR0
		#define SPSR	SPSR0
	
		#define SPIF	SPIF0
		#define MSTR	MSTR0
		#define CPOL	CPOL0
		#define DORD	DORD0
		#define SPR0	SPR00
		#define SPR1	SPR01
		#define SPI2X	SPI2X0
		#define SPE		SPE0
	#endif

	#define ENC28J60_OP_BFC    0xA0
	#define ENC28J60_OP_BFS    0x80
	#define ENC28J60_OP_READ_CTRL_REG  0x00
	#define ENC28J60_OP_WRITE_CTRL_REG 0x40
	#define ENC28J60_OP_READ_BUF_MEM   0x3A
	#define ENC28J60_OP_WRITE_BUF_MEM  0x7A

	//bank0
	#define ENC28J60_REG_ERDPTL   0x00
	#define ENC28J60_REG_ERDPTH   0x01
	#define ENC28J60_REG_EWRPTL   0x02
	#define ENC28J60_REG_EWRPTH   0x03
	#define ENC28J60_REG_ETXSTL   0x04
	#define ENC28J60_REG_ETXSTH   0x05
	#define ENC28J60_REG_ETXNDL   0x06
	#define ENC28J60_REG_ETXNDH   0x07
	#define ENC28J60_REG_ERXSTL   0x08
	#define ENC28J60_REG_ERXSTH   0x09
	#define ENC28J60_REG_ERXNDL   0x0A
	#define ENC28J60_REG_ERXNDH   0x0B
	#define ENC28J60_REG_ERXRDPTL 0x0C
	#define ENC28J60_REG_ERXRDPTH 0x0D
	#define ENC28J60_REG_ERXWRPTL 0x0E
	#define ENC28J60_REG_ERXWRPTH 0x0F

	#define ENC28J60_REG_EREVID   (0x72) //bank 3

	#define ENC28J60_REG_EIE      0x1B
	#define ENC28J60_EIE_INTIE    7
	#define ENC28J60_EIE_PKTIE    6
	#define ENC28J60_EIE_DMAIE    5
	#define ENC28J60_EIE_LNKIE    4 //LINKIE
	#define ENC28J60_REG_EIR      0x1C
	#define ENC28J60_REG_ESTAT    0x1D
	#define ENC28J60_REG_ECON2    0x1E
	#define ENC28J60_REG_ECON1    0x1F

	//bank1
	#define ENC28J60_REG_ERXFCON	(0x18 | 0x20)
	#define ENC28J60_REG_EPKTCNT	0x39

	//bank2
	#define ENC28J60_REG_MACON1		(0x40 | 0x80)
	#define ENC28J60_REG_MACON2		(0x41 | 0x80)
	#define ENC28J60_REG_MACON3		(0x42 | 0x80)
	#define ENC28J60_REG_MACON4		(0x43 | 0x80)
	#define ENC28J60_REG_MABBIPG	(0x44 | 0x80)
	#define ENC28J60_REG_MAIPGL		(0x46 | 0x80)
	#define ENC28J60_REG_MAIPGH		(0x47 | 0x80)
	#define ENC28J60_REG_MAMXFLL	(0x4A | 0x80)
	#define ENC28J60_REG_MAMXFLH	(0x4B | 0x80)
	#define ENC28J60_REG_MICMD		(0x52 | 0x80)
	#define ENC28J60_REG_MIREGADR	(0x54 | 0x80)
	#define ENC28J60_REG_MIWRL		(0x56 | 0x80)
	#define ENC28J60_REG_MIWRH		(0x57 | 0x80)
	#define ENC28J60_REG_MIRDL		(0x58 | 0x80)
	#define ENC28J60_REG_MIRDH		(0x59 | 0x80)
	//bank3
	#define ENC28J60_REG_MAADR1		(0x60 | 0x80)
	#define ENC28J60_REG_MAADR0		(0x61 | 0x80)
	#define ENC28J60_REG_MAADR3		(0x62 | 0x80)
	#define ENC28J60_REG_MAADR2		(0x63 | 0x80)
	#define ENC28J60_REG_MAADR5		(0x64 | 0x80)
	#define ENC28J60_REG_MAADR4		(0x65 | 0x80)
	#define ENC28J60_REG_MISTAT		(0x6A | 0x80)

	#define ENC28J60_BIT_ECON1_BSEL0 0
	#define ENC28J60_BIT_ECON1_BSEL1 1
	#define ENC28J60_BIT_MIIRD       0
	#define ENC28J60_BIT_MISTAT_BUSY 0

	#define ENC28J60_BIT_LOOPBACK 4
	#define ENC28J60_BIT_TXPAUS   3
	#define ENC28J60_BIT_RXPAUS   2
	#define ENC28J60_BIT_PASSALL  1
	#define ENC28J60_BIT_MARXEN   0

	#define ENC28J60_BIT_PADCFG2  7
	#define ENC28J60_BIT_PADCFG1  6
	#define ENC28J60_BIT_PADCFG0  5
	#define ENC28J60_BIT_TXCRCEN  4
	#define ENC28J60_BIT_PHDRLEN  3
	#define ENC28J60_BIT_HFRMEN   2
	#define ENC28J60_BIT_FRMLNEN  1
	#define ENC28J60_BIT_FULDPX   0
		
	#define ENC28J60_BIT_HDLDIS   8

	#define ENC28J60_BIT_TXRST    7
	#define ENC28J60_BIT_PKTIF    6
	#define ENC28J60_BIT_TXRTS    3
	#define ENC28J60_BIT_RXEN     2

	#define ENC28J60_PHY_PHCON1   0x00
	#define ENC28J60_PHY_PHCON2   0x10
	#define ENC28J60_PHY_PHLCON   0x14 //PHy Led CONtrol
	#define ENC28J60_BIT_PKTDEC 6
	
	#define ENC28J60_SOFT_RESET		0xFF

	//rx buffer gets the rest:
	#define ENC28J60_RX_BUFFER_START ((unsigned int)0x0000)
	#define ENC28J60_RX_BUFFER_END   ((unsigned int)0x19FF)
	#define ENC28J60_TX_BUFFER_START ((unsigned int)0x1A00)
	#define ENC28J60_TX_BUFFER_LEN   ((unsigned int)0x0600)
	#define ENC28J60_TX_BUFFER_END   ((unsigned int)0x1FFF)

	//store config bank0 reg0x00-reg0x0F in progmem
	extern unsigned char enc28j60_config[] PROGMEM;
	extern unsigned char enc28j60_revision;

	#define ETH_INTERRUPT 		INT2_vect
	#define ETH_INIT			   enc28j60_init
	#define ETH_PACKET_RECEIVE	enc28j60_receive_packet
	#define ETH_PACKET_SEND		enc28j60_send_packet

	//initialise spi & enc28j60
	void enc28j60_init(void);
	unsigned int enc28j60_receive_packet(unsigned int maxlen, unsigned char *buffer);
	void enc28j60_send_packet(unsigned int len, unsigned char *buffer);
	
	unsigned int enc28j60_read_phy(unsigned char addr);
	void enc28j60_write_phy(unsigned char address, unsigned int data);
	void enc28j60_read_buffer(unsigned char *buffer, unsigned int len);
	void enc28j60_write_buffer(unsigned char *buffer, unsigned int len);
	unsigned char enc28j60_read_address(unsigned char address);
	void enc28j60_write_address(unsigned char address, unsigned char data);
	unsigned char enc28j60_spi_read_byte(unsigned char op, unsigned char addr);
	void enc28j60_spi_write_word(unsigned char hi, unsigned char lo);

#endif //USE_ENC28J60
#endif
