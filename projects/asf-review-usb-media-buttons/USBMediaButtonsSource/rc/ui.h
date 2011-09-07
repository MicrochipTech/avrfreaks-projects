/**
 * \file
 *
 * \brief Common User Interface for HID Keyboard application
 *
 * Copyright (C) 2009 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef _UI_H_
#define _UI_H_

void ui_init(void);
void ui_powerdown(void);

void ui_wakeup_enable(void);
void ui_wakeup_disable(void);
void ui_wakeup(void);

//! This process is called called each 1ms
//! It is called only if HID Keyboard interface is enable.
//! It is called by SOF interrupt.
//! 
//!
void ui_process(uint16_t framenumber);

void ui_kbd_led(uint8_t value);

typedef enum
{
	KeyPlay = 1 << 0,
	KeyPause = 1 << 1,
	KeyFF = 1 << 2,
	KeyRew = 1 << 3,
	KeyNext = 1 << 4,
	KeyPrev = 1 << 5,
	KeyStop = 1 << 6,
	KeyPlayPause = 1 << 7,
	KeyMute = 1 << 8,
	KeyVolUp = 1 << 9,
	KeyVolDown = 1 << 10
} mediaDescriptor;

#endif // _UI_H_
