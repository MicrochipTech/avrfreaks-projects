/*
 * Copyright 2013, 2014 Chris Rhodin <chris@notav8.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

/*
 * Terminal attributes.
 *
 *  ICRNL     - Translate received CR to NL.
 *  ECHO      - Echo received characters to transmitter.
 *  ICANON    - Enable canonical mode processing.
 *  INONBLOCK - Do not block until operation is complete.
 *  ONLCR     - Translate transmitted NL to CR-NL.
 */
#define ICRNL     RB_SPARE0_MSK
#define ECHO      RB_SPARE1_MSK
#define ICANON    RB_SPARE2_MSK
#define INONBLOCK RB_SPARE3_MSK
#define ONLCR     (RB_SPARE3_MSK << 8)

uint16_t console_getattr(void);
void console_setattr(uint16_t attr);
int console_putchar(char c, struct __file * stream);
int console_getchar(struct __file * stream);
void console_init(void);

#endif /* _CONSOLE_H_ */
