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
 * Configuration header file.
 */


#ifndef _TWI_M_CONF_H
#define _TWI_M_CONF_H

/** Soft. delay between bit frames (in CPU_cycles/3). */
#define  TWI_SW_DELAY	14

/** TWI timeout. Probably in ms (depends on which timer decrements cntr_twi)*/
#define	TWI_TIMEOUT	13

/** 
 * TW EEPROM timeout. Probably in ms (depends on which timer decrements 
 * cntr_twi)
 */
#define TWEEP_TIMEOUT	3

/** Set to 1 to use the software TWI(i2c) driver (NOT THE AVR HARDWARE). */
#define TWI_USE_SOFT_DRIVER	1

/** Hardware TWI module -> issue command. */ 
#define tw_cmd(CMD)	(TWCR = (CMD)|_BV(TWINT)|_BV(TWEN))

#ifdef TWI_USE_SOFT_DRIVER
/* IO pin/port definitions for the software driver.*/
#define	bSDA		PC1
#define	bSCL		PC0
#define	TWI_DDR_SDA	DDRC
#define	TWI_PORT_SDA	PORTC
#define	TWI_PIN_SDA	PINC
#define	TWI_DDR_SCL	DDRC
#define	TWI_PORT_SCL	PORTC
#endif //TWI_USE_SOFT_DRIVER

/** TWI EEPROM chip select mask.*/
#define TWIEEP_ADRMASK	0x00

#endif	//_TWI_M_CONF_H


