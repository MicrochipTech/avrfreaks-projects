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

#define RAM_LENGTH 512 // Set the amount of RAM - in byte. You must set it to fit your vram file
#define RAM_BUFFER 64 // Set the RAM buffer size when load disk image to virtual RAM.
#define RAM_FILE "RAM.BIN" // Set the virtual RAM File location

// Boot file settings
//#define BOOT_PROMPT // Comment this to disable boot prompt

#ifndef BOOT_PROMPT
#define BOOT_FILE "BF.COM" // If you disable boot prompt, you must set a boot file
#endif

//#define ONE_USE // Don't use this. This is basicly use the input file as ram file (direct RAM)

#define CHIP_SELECT 10 // Chip select pin for SD card
