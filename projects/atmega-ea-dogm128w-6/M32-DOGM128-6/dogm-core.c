/**
 * @file dogm-core.c
 * libdogm
 *
 * This is the core module to be able to use the dogm lcd
 * http://www.lcd-module.de/eng/dog/dog.htm .
 *
 * Please note that this lib only has been tested with the dogm163.
 *
 * LICENSE:
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * @author JORDACIJEVIC Serz <serz@jordacijevic.name>
 * @date 2008/05/19
 *
 * @version $Revision: 1.0$
 *
 * @todo Nothing to do. - Everything perfect! ;-)
 *
 * URL: $HeadURL: http://serz.jordacijevic.name/?mmenu=project&smenu=libdogm$
 *
 * Last Modified: $Author: Jordacijevic Serz$
 */

/*
 * -------------------------------------------------------------- includes --
 */

#define _GNU_SOURCE

/* std */
#include "global.h"

/* local */
#include "dogm-core.h"

typedef enum {LCD_DATA, LCD_CMD} lcd_type_t;


static void dogm_core_write(unsigned char data, lcd_type_t type);
static void (*pspi_write)(unsigned char);
static void (*pset_a0)(uint8_t);
static void (*pset_cs)(uint8_t);
static void (*pdelay_us)(u32);

/*
 * ------------------------------------------------------------- functions --
 */

/**
 *
 * \brief dogm_core_write()
 *
 * Each cmd/data activities are handled by this finction.
 * This function is used to write the cmd/data to the lcd.
 *
 * \param data the unsigned char that have to be written
 * \param type is the type the data (cmd or data)
 *
 * \return always void
 * \retval void always
 *
 */
static void dogm_core_write(unsigned char data, lcd_type_t type)
{
	pset_a0(type);		// select the register
	pset_cs(1);		// select the lcd
	pspi_write(data);	// write the data to lcd
	pdelay_us(30);		// wait for >26.3us
	pset_cs(0);		// deselect the lcd
}

/**
 *
 * \brief dogm_core_set_remore()
 *
 * is needed to setup the functuon and pins which are needed to control the lcd.
 *
 * \param remote_spi_write is the pointer to the spi_write function.
 * \param remote_set_rs selects the register
 * \param remote_set_cs selects the chip (lcd)
 * \param remote_delay_us is the ref. to the _delay_ms function; delays is micro second (us)
 *
 * \return always void
 * \retval void always
 *
 */
void dogm_core_set_remore(void (*remote_spi_write)(unsigned char), void (*remote_set_a0)(uint8_t), void (*remote_set_cs)(uint8_t), void (*remote_delay_us)(u32))
{
	pspi_write = remote_spi_write;
	pset_a0 = remote_set_a0;
	pset_cs = remote_set_cs;
	pdelay_us = remote_delay_us;
}

/**
 *
 * \brief dogm_core_cmd()
 *
 * is sending the command to the lcd.
 * This is a core function, please do not use it.
 * Use the dogm_data_char() function.
 *
 * \param cmd is of type char
 *
 * \return always void
 * \retval void always
 *
 */
void dogm_core_cmd(char cmd)
{
	dogm_core_write((unsigned char)cmd, LCD_CMD);
}

/**
 *
 * \brief dogm_core_data()
 *
 * is sending data to the lcd.
 * This is a core function, please do not use it.
 * Use one of the dogm_cmd_*() if possible.
 *
 * \param c is of type char
 *
 * \return always void
 * \retval void always
 *
 */
void dogm_core_data(char c)
{
	dogm_core_write((unsigned char)c, LCD_DATA);
}

