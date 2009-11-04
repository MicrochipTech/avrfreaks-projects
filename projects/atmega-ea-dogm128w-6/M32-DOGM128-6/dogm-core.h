/**
 * @file dogm-core.h
 * libdogm
 *
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

#ifndef _DOGM_CORE_H_

/*
 * -------------------------------------------------------------- includes --
 */

/*
 * --------------------------------------------------------------- defines --
 */

#define _DOGM_CORE_H_

/*
 * -------------------------------------------------------------- typedefs --
 */

/*
 * --------------------------------------------------------------- globals --
 */

/*
 * ------------------------------------------------------------- functions --
 */

void dogm_core_set_remore(void (*remote_spi_write)(unsigned char), void (*remote_set_a0)(uint8_t), void (*remote_set_cs)(uint8_t), void (*remote_delay_us)(u32));
void dogm_core_cmd(char cmd);
void dogm_core_data(char c);

#endif // _DOGM_CORE_H_

