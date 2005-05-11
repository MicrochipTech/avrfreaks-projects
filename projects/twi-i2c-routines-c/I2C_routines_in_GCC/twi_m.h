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
 * 
 * NON Interrupt based.
 * 
 * Header file.
*/


#ifndef _TWI_M_H
#define _TWI_M_H

/************************* Exported variables ****************************/

/************************* Function prototypes ****************************/

/** TWI init routine. Must be called once before calling other TWI routines.*/
extern void tw_init(void);

/** Generate start condition.*/
extern uint8_t tw_start(void);

/** Generate repeated start condition.*/
extern uint8_t tw_rep_start(void);

/** Generate STOP condition.*/
extern void tw_stop(void);

/** Send a byte to the slave.*/
extern uint8_t tw_send_byte( uint8_t byte );

/** Send an address byte to the slave.*/
extern uint8_t tw_send_addr( uint8_t byte );

/** Receive a byte from the slave. */
extern int tw_recv_byte( uint8_t ack );


/** Set TWI EEPROM for read operation at <address>.*/
extern void tweep_setaddr_r(uint16_t addr);

/** Set TWI EEPROM for write operation at <address>.*/
extern void  tweep_setaddr_w(uint16_t addr);

/** Wait TWI EEPROM.*/
extern void tweep_wait(void);


#endif	//_TWI_M_H


