/**
 * \file
 *
 * \brief Sample table header file.
 *
 * Copyright (C) 2010 Atmel Corporation. All rights reserved.
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

#ifndef _SND_SAMPLES_H
#define _SND_SAMPLES_H

#include <compiler.h>
#include <progmem.h>

extern PROGMEM_DECLARE(uint16_t, snd_sine[256]);

extern PROGMEM_DECLARE(uint8_t, snd_drum[4221]);
extern PROGMEM_DECLARE(uint8_t, snd_sym[14562]);
extern PROGMEM_DECLARE(uint8_t, snd_drumroll[6000]);
extern PROGMEM_DECLARE(uint8_t, snd_bell[2624]);
extern PROGMEM_DECLARE(uint8_t, snd_sax[10474]); 
extern PROGMEM_DECLARE(uint8_t, snd_synth1[7704]);
extern PROGMEM_DECLARE(uint8_t, snd_synth2[9326]);
extern PROGMEM_DECLARE(uint8_t, snd_synth3[14223]);
extern PROGMEM_DECLARE(uint8_t, snd_youwin[19756]);
extern PROGMEM_DECLARE(uint8_t, snd_fanfare[28672]);

#endif
