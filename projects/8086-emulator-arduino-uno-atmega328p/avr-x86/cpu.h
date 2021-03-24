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

#include <stdint.h>
#include <Arduino.h>
#include "outputserial.cpp"
#include "ram.h"

void decodeflagsword( word x);
void modregrm();
void write86 (uint32_t addr32, byte value);
void writew86 (uint32_t addr32, word value);
void flag_szp8 (byte value);
void flag_szp16 (word value);
void flag_log8 (byte value);
void flag_log16 (word value);
void flag_adc8 (byte v1, byte v2, byte v3);
void flag_adc16 (word v1, word v2, word v3);
void flag_add8 (byte v1, byte v2);
void flag_add16 (word v1, word v2);
void flag_sbb8 (byte v1, byte v2, byte v3);
void flag_sbb16 (word v1, word v2, word v3);
void flag_sub8 (byte v1, byte v2);
void flag_sub16 (word v1, word v2);
void op_adc8();
void op_adc16();
void op_add8();
void op_add16();
void op_and8();
void op_and16();
void op_or8();
void op_or16();
void op_xor8();
void op_xor16();
void op_sub8();
void op_sub16();
void op_sbb8();
void op_sbb16();
void getea (byte rmval);
void push (word pushval);
void reset86();
void writerm16 (byte rmval, word value);
void writerm8 (byte rmval, byte value);
void intcall86 (byte intnum);
void op_div8 (word valdiv, byte divisor);
void op_idiv8 (word valdiv, byte divisor);
void op_grp3_8();
void op_div16 (uint32_t valdiv, word divisor);
void op_idiv16 (uint32_t valdiv, word divisor);
void op_grp3_16();
void op_grp5();
void init86 ();
void exec86 (uint32_t execloops);
byte read86 (uint32_t addr32);
byte readrm8 (byte rmval);
word readw86 (uint32_t addr32);
byte op_grp2_8 (byte cnt);
