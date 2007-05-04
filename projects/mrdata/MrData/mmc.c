/*
 * a.lp_mp3 - Open Source Atmel AVR based MP3 Player
 * Copyright (c) 2003-2004 K. John '2B|!2B' Crispin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
 *
 * Feedback, Bugs.... mail john{AT}phrozen.org
 *
 */ 


// this code includes a few functions that were originally made for a ti msp340. 
// i found it on www.mikrocontroller.net once upon a time
// the ripped functions are
// MMC_get_R1
// MMC_get_R2
// the cs_deselect; spi_io(0xff); CS_select; in the while loop in MMC_init doesn't work without
// unfortunatley i lost the link.
// if you know where this code can be found, let me know so i can put a link here.

#include <avr/io.h>
#include <avr/interrupt.h>

#include "types.h"
#include "delay.h"
#include "spi.h"
#include "mmc.h"
#include "fat16.h"


#define MMC_CS_DDR	DDRB
#define MMC_CS_PIN	PINB
#define MMC_CS_PORT	PORTB
#define PIN_MMC_CS PB2

// look in datasheet for complete list of commands
#define MMC_CMD_0_GO_IDLE				0
#define MMC_CMD_1_SEND_OP_COND			1
#define MMC_CMD_9_SEND_CSD				9
#define MMC_CMD_10_SEND_CID				10
#define MMC_CMD_12_STOP					12
#define MMC_CMD_13_SEND_STATUS			13
#define MMC_CMD_16_BLOCKLEN				16
#define MMC_CMD_17_READ_SINGLE			17
#define MMC_CMD_18_READ_MULTIPLE		18
#define MMC_CMD_24_WRITE_SINGLE			0x18
#define MMC_CMD_25_WRITE_MULTI			0x19

// bit definitions for R1
#define MMC_R1_IN_IDLE 	0x01
#define MMC_R1B_BUSY_BYTE 0x00

// different defines for tokens etc ...
#define MMC_START_TOKEN_SINGLE  0xfe
#define MMC_START_TOKEN_MULTI   0xfc


// starts the read process of a sector
static void MMC_get_sec_start(u16 sectorh, u16 sectorl);
// starts the read process of a part of a sector 
static void MMC_get_part_sec_start(u16 sectorh, u16 sectorl, u16 offset, u16 length);
// gets the next byte from the MMC card
static u08 MMC_get_sec_next(void);
// stop read process of a sector
static void MMC_get_sec_stop(void);

// deselcets the MMC
static void MMC_CS_deselect(void);
//selects the MMC for transmission
static void MMC_CS_select(void);

// sends receives a byte on the spi bus
static u08 MMC_io(u08 data);

static void MMC_cleanup(void);


//---------------------------------------


u32 last_sector;
static u16 current_blocklen = 0;
u08 mmc_open=0;

// sends a block of data from the mem over the spi bus
static void spi_io_mem(u08* data, u16 length){
	// transmit 'length' bytes over spi
	while(length){
		spi_io(*data);
		data++;
		length--;
	};
};

// send cmd + arguments + default crc for init command. once in spi mode 
// we dont need crc so we can keep this constant
static void MMC_send_cmd(u08 cmd, u16 datah, u16 datal){
	// default command sequence
	u08 buffer[6] ;
	// fill sequence with our specific data
	buffer[0]=0x40 | cmd;
	buffer[1]=(datah&0xff00)>>8;
	buffer[2]=datah&0xff;
	buffer[3]=(datal&0xff00)>>8;
	buffer[4]=datal&0xff;
	buffer[5]=0x95;
	// dispach data
	spi_io_mem(buffer,6);
};

// gets a 1 byte long R1
static u08 MMC_get_R1(void){
	u08 retval;
	u08 max_errors = 128;
	// wait for first valid response byte
	do{
		retval = spi_io(0xff);
		max_errors--;
	}while(  (retval & 0x80) && (max_errors>0));
	return retval;
};

// gets a 2 byte long R2 
static u16 MMC_get_R2(void){
	u16 retval;
	u08 max_errors = 64;
	// wait for first valid response byte
	do{
		retval = spi_io(0xff);
		max_errors--;
	}while(  (retval & 0x80) && (max_errors>0) );
	// move data to upper byte
	retval = (retval << 8);
	// get second byte;
	max_errors = spi_io(0xff);
	retval += max_errors;
	return retval;
};

// sends the start data block token to the MMC
static void MMC_send_start_data_token(void){
  spi_io(MMC_START_TOKEN_SINGLE);
};

// waits for the card to send the start data block token
static void MMC_wait_for_start_token(u08 max_errors){
  u08 retval;
  do{
    // get a byte from the spi bus
    retval = spi_io(0xff);
    // keep track of the trys
    max_errors--;   
  }while((retval != MMC_START_TOKEN_SINGLE) );
};

/*
#define MMC_CS_select() cbi(MMC_CS_PORT, PIN_MMC_CS)
#define MMC_CS_deselect() sbi(MMC_CS_PORT, PIN_MMC_CS)
*/
// selects the CS for spi xfer
static void MMC_CS_select(void){
	// pull down the MMC CS line
	cbi(MMC_CS_PORT, PIN_MMC_CS);
};

// deselects the CS for spi xfer
static void MMC_CS_deselect(void){
	// pull up the MMC CS card
	sbi(MMC_CS_PORT, PIN_MMC_CS);
};


// stops the MMC transmission and sends the clock cycles needed by the mmc for cleanup
static void MMC_cleanup(void){
	// deselect the MMC card
	MMC_CS_deselect();
	// pulse the SCK 16 times
	spi_io(0xff);
	spi_io(0xff);
};

// gets n bytes plus crc from spi bus
static void MMC_get_data(u08* ptr_data, u16 length){
  MMC_wait_for_start_token(128);
  while(length){
    *ptr_data = spi_io(0xff);
    //USART_sendint(*ptr_data);
	//USART_send(' ');
	length--;
    ptr_data++;
  };
  // get the 2 CRC bytes
  spi_io(0xff);
  spi_io(0xff);
};


// reads the CID reg from the card
static void MMC_get_CID(u08 *ptr_data){
  // select card
	MMC_CS_select();
	// tell the MMC card that we want to know its status
	MMC_send_cmd(MMC_CMD_10_SEND_CID,0x0,0x0);
	// get the response
	MMC_get_R1();
	// get the register data
	MMC_get_data(ptr_data, 16);
	// cleanup behind us
	MMC_cleanup();
};



// reads the CSD reg from the card
static void MMC_get_CSD(u08 *ptr_data){
	// select card
	MMC_CS_select();
	// tell the MMC card that we want to know its status
	MMC_send_cmd(MMC_CMD_9_SEND_CSD,0x0,0x0);
	// get the response
	MMC_get_R1();
	// get the register data
	MMC_get_data(ptr_data, 16);
	// cleanup behind us
	MMC_cleanup();
};

// set the BLOCKLEN for transmissions
static void MMC_set_blocklen(u16 blocklen){
	// make sure this block len is not already set
	if(current_blocklen != blocklen){
		current_blocklen= blocklen;
		// select card
		MMC_CS_select();
		// tell the MMC card that we want to know its status
		MMC_send_cmd(MMC_CMD_16_BLOCKLEN,0x0,blocklen);
		// get the response
		MMC_get_R1();
		// cleanup behind us
		MMC_cleanup();
	};
};




// returns the :
// 		size of the card in MB ( ret * 1024^2) == bytes
// 		sector count and multiplier MB are in u08 == C_SIZE / (2^(9-C_SIZE_MULT))
// 		name of the media 
static void MMC_get_volume_info(void) {//VOLUME_INFO* vinf){
	u08 data[16];
	// read the CSD register
	MMC_get_CSD(data);
	// get the C_SIZE value. bits [73:62] of data
	// [73:72] == data[6] && 0x03
	// [71:64] == data[7]
	// [63:62] == data[8] && 0xc0
	last_sector = data[6] & 0x03;
	last_sector <<= 8;
	last_sector += data[7];
	last_sector <<= 2;
	last_sector += (data[8] & 0xc0) >> 6;
};

// sets up the pins used by the MMC interface
void MMC_hw_init(void){
	// set MMC CS pin high output 
	sbi(MMC_CS_PORT,PIN_MMC_CS);
	sbi(MMC_CS_DDR, PIN_MMC_CS);
};

// starts up the MMC card
u08 MMC_init(void){
	u08 i;
	u08 res;	
	// the data sheet says that the MMC needs 74 clock pulses to startup
	// 10*8== 80; 80>76
	for( i = 0; i < 100; i++){
		spi_io(0xff);
	};
	
	// select card
	MMC_CS_select();
	// put MMC in idle
	MMC_send_cmd(MMC_CMD_0_GO_IDLE,0x0,0x0);
	// get the response
	res = MMC_get_R1();
	
	//if(res != 0x01) USART_sendstring("Error R1-init",0x01);
	if(res != 0x01) 
		return 0;
	
	// tell the MMC to start its init process by sending the MMC_CMD_1_SEND_OP_COND comand
	// until the response has the idle bit set to 0
	while(res==0x01){
		// deselect card
		MMC_CS_deselect();
		// send 32 clock pulses
		spi_io(0xff);
		spi_io(0xff);
		spi_io(0xff);
		spi_io(0xff);
		// select card
		MMC_CS_select();
    // send wake up signal s.t. MMC leaves idle state and switches to operation mode
		MMC_send_cmd(MMC_CMD_1_SEND_OP_COND,0x0,0x0);
    // get response
		res = MMC_get_R1();
	};
	// cleanup behind us
	MMC_cleanup();
	  
	// now increase MMC card interface speed to 4MHz
//	UBRR0 = 1;

	// find out some info on card
	MMC_get_volume_info();

	MMC_set_blocklen(512);
	mmc_open = 0;
	return 1;
};

// starts the read process of a sector
static void MMC_get_sec_start(u16 sectorh, u16 sectorl){
	spi_io(0xff);
	spi_io(0xff);
	// turn sectors into byte addr
	sectorh = (sectorh << 9) + (sectorl >> 7);
	sectorl = sectorl << 9;
	// select card
	MMC_CS_select();
	mmc_open = 1;
	// tell the MMC card that we want to know its status
	MMC_send_cmd(MMC_CMD_17_READ_SINGLE, sectorh, sectorl);
	// get the response
	if (MMC_get_R1()) {
		MMC_cleanup();
		return;
	}
	// wait till the mmc starts sending data
	MMC_wait_for_start_token(255);
};

// starts the write process of a sector
static void MMC_write_sec_start(u16 sectorh, u16 sectorl){
	spi_io(0xff);
	spi_io(0xff);
	// turn sectors into byte addr
	sectorh = (sectorh << 9) + (sectorl >> 7);
	sectorl = sectorl << 9;
	// select card
	MMC_CS_select();
	mmc_open = 1;
	// tell the MMC card that we want to know its status
	MMC_send_cmd(MMC_CMD_24_WRITE_SINGLE, sectorh, sectorl);
	// get the response
	if (MMC_get_R1()) {
		MMC_cleanup();
		return;
	}
	// send start byte
	spi_io(0xfe);
};

// gets the next byte from the MMC card
static inline u08 MMC_get_sec_next(void){
	u08 tmp = spi_io(0xff);
	return tmp;
};

// writes the next byte to the MMC card
static inline void MMC_write_sec_next(u08 b){
	spi_io(b);
};

// stop read process of a sector
static inline void MMC_get_sec_stop(void){
	if (mmc_open) {
		// get the 2 CRC bytes
		spi_io(0xff);
		spi_io(0xff);
		// give enough time
		MMC_cleanup();
		mmc_open = 0;
	}
};


// stop write process of a sector
static inline void MMC_get_sec_stop_w(void){
	if (mmc_open) {
		// get the 2 CRC bytes
		spi_io(0xff);
		spi_io(0xff);
	    // wait while card is busy
	    while (spi_io(0xff) != 0xff);
		// give enough time
		MMC_cleanup();
		mmc_open = 0;
	}
};



//-------------------------------------------------------------
// High Level MMC-Funktionen

u32 sect;
u32 file_len;
u16 bufpos=0;

extern u08 fat_directaccess;


void mmc_read_sector(u32 s, u08 *buf) {
	u16 i;
	MMC_get_sec_start((u16)(s>>16), (u16)s);
	for (i=0; i<512; i++)
		buf[i] = MMC_get_sec_next();
	MMC_get_sec_stop();
}

void mmc_write_sector(u32 s, u08 *buf) {
	u16 i;
	MMC_write_sec_start((u16)(s>>16), (u16)s);
	for (i=0; i<512; i++)
		MMC_write_sec_next(buf[i]);
	MMC_get_sec_stop_w();
}

// Lesen beginnen
void mmc_load_start(u32 s) {
	sect = s;
	MMC_get_sec_start((u16)(s>>16), (u16)s);
	bufpos = 0;
}


// Schreiben beginnen
void mmc_write_start(u32 s) {
	sect = s;
	MMC_write_sec_start((u16)(s>>16), (u16)s);
	bufpos = 0;
}


// Byte lesen
u08 mmc_fetch_byte(void) {
	if (bufpos == 512) {
		MMC_get_sec_stop();
		if (!fat_directaccess)
			fat_getnextsector();
		else
			sect++;
		mmc_load_start(sect);
	}
	bufpos++;
	return MMC_get_sec_next();
}


// Byte schreiben
void mmc_write_byte(u08 b) {
	if (bufpos == 512) {
		MMC_get_sec_stop_w();
		if (!fat_directaccess)
			fat_getnextsector();
		else
			sect++;
		mmc_write_start(sect);
	}
	bufpos++;
	MMC_write_sec_next(b);
	file_len++;
}


// Lesen, um die 512 voll zu machen
void mmc_complete_read(void) {
	while (bufpos < 512) {
		MMC_get_sec_next();
		bufpos++;
	}
	MMC_get_sec_stop();
}


// Schreiben, um die 512 voll zu machen
void mmc_complete_write(void) {
	while (bufpos < 512) {
		MMC_get_sec_next();
		bufpos++;
	}
	MMC_get_sec_stop_w();
}


void mmc_read_block(u08 *buf, u16 cnt) {
	while (cnt--) {
		*buf = mmc_fetch_byte();
		buf++;
	}
}


void mmc_write_block(u08 *buf, u16 cnt) {
	while (cnt--) {
		mmc_write_byte(*buf);
		buf++;
	}
}

