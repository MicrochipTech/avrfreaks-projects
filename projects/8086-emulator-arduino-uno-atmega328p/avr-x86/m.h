/*
    avr-x86 - x86 emulator running on AVR Arduino!
    Copyright (C) 2021 @raspiduino

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

/* Fat16 */
#include <Fat16.h>

#include <string.h>

#include "outputserial.cpp"
#include "config.h"

byte ramread(uint32_t addr);
void ramwrite(uint32_t addr, byte value);
void ramload();
void sdinit();

extern char filepath[12];
extern SdCard card;
extern Fat16 file;
void fileinput();
