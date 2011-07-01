/**
 *
 * CoreDefs.h is part of the FIGnition firmware.
 *
 * The FIGnition firmware is the built-in software for the
 * FIGnition DIY 8-bit computer and compatible computers.
 *
 * Copyright (C) 2011  Julian Skidmore.
 *
 * The FIGnition firmware is free software: you can redistribute it and/or modify
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
 *
 * Version. Date (DD/MM/YYY)
 * ************************
 *
 * 1.0.0.  01/07/2011. Released as part of the FIGnition VDsk Flash Driver.
 *
 * Contact
 * *******
 * TheOriginalSnial@Gmail.com
 *
 */

#ifndef _COREDEFS_H

#define _COREDEFS_H

#ifndef EEMEM

#define EEMEM __attribute__((section(".eeprom")))

#define GetPgmByte(x) pgm_read_byte(&x)
#define GetPgmWord(x) pgm_read_word(&x)

#endif

#define kTrue 1
#define kFalse 0

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned long ulong;


#define __VideoGenPAL__
//#define __VideoGenNTSC__

#ifdef __VideoGenPAL__

// HSync Init.
#if F_CPU == 16000000
#define kHSyncScan (128-1)
#define kHSyncPulse4us (9-1)
#define kHSyncScanShort (64-1)
#define kHSyncPulse2us (4-1)
//#define kFrameVideoMarginLeft (18-1)
// Working version.
#define kFrameVideoMarginLeft (15-1)
// Add to see bit data go high.
//#define kFrameVideoMarginLeft (19-1)
#define kVideoBuffWidth 20
//#define kVideoBuffWidth 40
#endif

#if F_CPU == 20000000
#define kHSyncScan (160-1)
#define kHSyncPulse4us (12-1)
#define kHSyncScanShort (80-1)
#define kHSyncPulse2us (6-1)
#define kFrameVideoMarginLeft (18-1) // actually about 4c later or .2us or 2 pixels.
#define kVideoBuffWidth 25
//#define kVideoBuffWidth 48
#endif

#endif

#ifdef __VideoGenNTSC__

// only 20MHz supported.

#if F_CPU == 20000000
#define kHSyncScan (159-1)
#define kHSyncPulse4us (12-1)
#define kHSyncScanShort (80-1)
#define kHSyncPulse2us (6-1)
#define kFrameVideoMarginLeft (18-1) // actually about 4c later or .2us or 2 pixels.
#define kVideoBuffWidth 25
//#define kVideoBuffWidth 48
#endif


#endif

#define kVideoBuffHeight 24
#define kChrSetChrs 128
#define kUdgChrs 16
#define kChrSetBytesPerChar 8

typedef struct {
	byte *gCur;
	byte gCurX;	// not sure if we need gCurY.
} tSysVars;

extern tSysVars gSysVars;

extern byte gVideoBuff[kVideoBuffWidth*kVideoBuffHeight+kUdgChrs*kChrSetBytesPerChar];
#define gUDGBase (gVideoBuff+kVideoBuffWidth*kVideoBuffHeight)
extern volatile byte *gVPtr;	// defined in assembler.

//extern byte *gCur;
extern byte gCurY;

extern volatile short gClock;
extern void Cls();
extern void PrintAt(byte x, byte y);
extern void DotHex(ushort x);
extern void Dot(int x);
extern void Emit(char ch);
extern void _cmove(ushort src, ushort dst, ushort len);
extern void SetLed(byte state);

// Flash definitions
// This size is correct for 8MBit and 4MBit Amic Flash chips.
#define kSerialFlashBlockSizeBits 8
#define kSerialFlashBlockSize (1<<kSerialFlashBlockSizeBits)
extern void SerialFlashEraseSector(ushort sector);
extern void SerialFlashReadBlock(ushort block, byte *dest);
extern void SerialFlashWriteBlock(ushort block, byte *src);
extern ushort SerialFlashID(void);

extern ushort gRamCache;
#define SramFlush() gRamCache=0;	// can't read from ROM. 
extern void InterruptSpi();

// Keycode definitions.

#define kKeyLeft	8
#define kKeyUp 		11
#define kKeyRight	9
#define kKeyDown	10
#define kKeyEnter   '\r'
#define kKeyCopy	(0x80+'C')
#define kKeyMark	(0x80+'M')
#define kKeyDel     7
#define kKeyCmd     (0x80+'+')
#define kKeyCtrl    (0x80+'^')
#define kKeyEsc     27

extern byte KeyP(void);
extern byte Key(void);



#endif