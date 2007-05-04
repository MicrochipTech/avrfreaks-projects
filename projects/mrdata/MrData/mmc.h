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

#ifndef _MMC_H_
#define _MMC_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "types.h"


// starts the hardware
void MMC_hw_init(void);
// starts up the MMC card
u08 MMC_init(void);

void mmc_read_sector(u32 s, u08 *buf);
void mmc_write_sector(u32 s, u08 *buf);
void mmc_load_start(u32 s);
u08 mmc_fetch_byte(void);
void mmc_write_start(u32 s);
void mmc_write_byte(u08 b);
void mmc_complete_read(void);
void mmc_complete_write(void);
void mmc_read_block(u08 *buf, u16 cnt);
void mmc_write_block(u08 *buf, u16 cnt);

#endif
