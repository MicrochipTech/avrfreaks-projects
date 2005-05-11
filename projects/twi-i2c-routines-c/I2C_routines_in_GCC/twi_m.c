/***************************************************************************
 *   Copyright (C) 2005 by Dimitar Dimitrov                                *
 *   dinuxbg at gmail.com                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/** 
 * Master Two Wire Interface module.
 * NON Interrupt based.
 */

#include <inttypes.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/twi.h>

#include "main.h"
#include "twi_m.h"
#include "twi_m_conf.h"

/** Decremented by some timer-counter ISR. Check also TWI_TIMEOUT. */
volatile uint8_t cntr_twi;

/** Decremented by some timer-counter ISR. Check also TWEEP_TIMEOUT. */
volatile uint8_t cntr_tweep_wait;


#if TWI_USE_SOFT_DRIVER

/********************** Low level routines  - sw **********************/


/*Shortcuts to the pins*/
#define	SCL_1()		cbi(TWI_DDR_SCL, bSCL)
#define	SCL_0()		sbi(TWI_DDR_SCL, bSCL)
#define	SDA_1()		cbi(TWI_DDR_SDA, bSDA)
#define	SDA_0()		sbi(TWI_DDR_SDA, bSDA)
#define	RELEASE_SDA()	cbi(TWI_DDR_SDA, bSDA)

void tw_init(void)
{
	cbi(TWI_DDR_SDA,bSDA);
	cbi(TWI_PORT_SDA,bSDA);
	cbi(TWI_DDR_SCL,bSCL);
	cbi(TWI_PORT_SCL,bSCL);
}

uint8_t tw_start(void)
{
	RELEASE_SDA();
	SCL_1();
	_delay_loop_1(TWI_SW_DELAY);
	if(bit_is_clear(TWI_PIN_SDA,bSDA)) {
		return RC_FAIL;
	}
	SDA_0(); 
	_delay_loop_1(TWI_SW_DELAY);
	SCL_0(); 
	_delay_loop_1(TWI_SW_DELAY);
	return RC_SUCCESS;
}

uint8_t tw_rep_start(void)
{
	return tw_start();
}


void tw_stop(void)
{
	SDA_0(); 
	SCL_1();
	_delay_loop_1(TWI_SW_DELAY);
	SDA_1(); 
	_delay_loop_1(TWI_SW_DELAY);
	SCL_0(); 
	_delay_loop_1(TWI_SW_DELAY);
	//we need to free the bus
	RELEASE_SDA();
	SCL_1();
	_delay_loop_1(TWI_SW_DELAY);
}


uint8_t tw_send_byte(uint8_t data)
{
	register uint8_t i;
	
	
	for (i = 0; i <= 7; i++) {
		if (data & 0x80) {
			SDA_1();
		} else {
			SDA_0(); 
		}
		SCL_1(); 
		_delay_loop_1(TWI_SW_DELAY);
		SCL_0(); 
		_delay_loop_1(TWI_SW_DELAY);
		data = data << 1;
	}
	RELEASE_SDA();
	SCL_1(); 
	_delay_loop_1(TWI_SW_DELAY);
	if(bit_is_set(TWI_PIN_SDA, bSDA)) {
		SCL_0();
		_delay_loop_1(TWI_SW_DELAY);
		return 0;		//Acknowledge NOT received
	}
	SCL_0();
	_delay_loop_1(TWI_SW_DELAY);
	return 1;
}


int tw_recv_byte(uint8_t ack)
{
	uint8_t i, data;

	RELEASE_SDA();
	data = 0;
	for (i = 0; i <= 7; i++) {
		SCL_1(); 
		_delay_loop_1(TWI_SW_DELAY);
		data <<= 1;
		if (bit_is_set (TWI_PIN_SDA, bSDA))
			data++;
		SCL_0(); 
		_delay_loop_1(TWI_SW_DELAY);
	}
	if (!ack) {	
		SDA_1();	/*Set NO ACK*/
	} else {		
		SDA_0(); 	/*Set ACK*/
	}
	SCL_1();
	_delay_loop_1(TWI_SW_DELAY);
	SCL_0(); 
	_delay_loop_1(TWI_SW_DELAY);
	return data;
}

uint8_t tw_send_addr( uint8_t byte )
{
	return tw_send_byte(byte);
}

#else	//Use the hardware TWI interface

/** Wait until TWI is free OR we timeout.*/
#define twi_wait(RET_VALUE)	cntr_twi=TWI_TIMEOUT;\
				while( (TWCR & _BV(TWINT)) == 0) {\
					if(!cntr_twi) {\
						return RET_VALUE;\
					}\
				}

void tw_init(void)
{
	/* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
	TWSR = 0;
	//TWBR = 47;	//CPU=11.0592 MHz,  SCL=100 KHz
	TWBR = (CPU_CLOCK / 100000UL - 16) / 2;

	//No TWI interrupt - operation is poll-based
	//TWCR = (1<<TWEA)|(1<<TWEN)|(1<<TWINT)|(0<<TWIE);
}


/********************** Low level routines  - hw **********************/

uint8_t tw_start(void)
{
	uint8_t status;
	
	tw_cmd(_BV(TWSTA));
	twi_wait(RC_FAIL);

	status = TW_STATUS;
	if( (status != TW_START) && (status != TW_REP_START) ) {
		return RC_FAIL1;
	}
	return RC_SUCCESS;
}

uint8_t tw_rep_start(void)
{
	uint8_t status;
	
	tw_cmd(_BV(TWSTA));
	twi_wait( RC_FAIL );

	status = TW_STATUS;
	if( (status != TW_START) && (status != TW_REP_START) ) {
		return RC_FAIL1;
	}
	return RC_SUCCESS;
}

void tw_stop(void)
{
	tw_cmd(_BV(TWSTO));
}

int tw_recv_byte( uint8_t ack )
{
	uint8_t status;
	
	if(!ack) {
		//NACK
		tw_cmd(0<<TWEA);
	} else {
		//ACK
		tw_cmd(1<<TWEA);
	}
	
	twi_wait(-1);
	status = TW_STATUS;
	if( ((status!=TW_MR_DATA_ACK) && ack)
	    || ((status!=TW_MR_DATA_NACK) && !ack)) {
		return -1;
	}
	return TWDR;
}

uint8_t tw_send_addr( uint8_t byte )
{
	uint8_t status;
	
	TWDR = byte;
	tw_cmd(0);
	twi_wait(0);
	status = TW_STATUS;
	if( status != TW_MT_SLA_ACK ) {
		return 0;
	} 
	return 1;
}

uint8_t tw_send_byte( uint8_t byte )
{
	uint8_t status;
	
	TWDR = byte;
	tw_cmd(0);
	twi_wait( RC_FAIL );
	status = TW_STATUS;
	if(status!=TW_MT_DATA_ACK) {
		return RC_FAIL1;
	} 
	return RC_SUCCESS;
}

#endif //TWI_USE_SOFT_DRIVER


/********************** Mid-level routines **********************/

void tweep_setaddr_r(uint16_t addr)
{
	tweep_wait();
	tw_start();
	/* Address  EEPROM (dummy write)*/
	tw_send_addr(0xA0 | TW_WRITE | TWIEEP_ADRMASK);	
	/*WRITE MSB internal memory latch*/
	tw_send_byte((addr>>8)&0xff);
	/*Write LSB internal memory latch*/
	tw_send_byte(0x00ff & addr);				
	tw_rep_start();
	tw_send_addr(0xA0 | TW_READ | TWIEEP_ADRMASK);		/*Set to READ*/
}

void  tweep_setaddr_w(uint16_t addr)
{
	tweep_wait();
	tw_start();
	/* Address  EEPROM */
	tw_send_addr(0xA0 | TW_WRITE | TWIEEP_ADRMASK);	
	/*WRITE MSB internal memory latch*/
	tw_send_byte((addr>>8)&0xff);
	/*Write LSB internal memory latch*/
	tw_send_byte(0x00ff & addr);	
}

void tweep_wait(void)
{
	uint8_t ack;
	
	cntr_tweep_wait = TWEEP_TIMEOUT;
	do {
		tw_start();
		ack = tw_send_addr(0xA0 | TW_WRITE | TWIEEP_ADRMASK);
		if(!cntr_tweep_wait) {
			break;
		}
	} while( !ack );
	if(ack) tw_stop();
}


