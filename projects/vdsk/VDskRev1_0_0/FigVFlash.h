/**
 *
 * FigVFlash.h is part of the FIGnition firmware.
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
 *
 * Introduction:
 * *************
 *
 * The FigVFlash.h provides a rudimentary, but compact wear-levelling
 * reclaimable block flash driver. It is currently compatible with the
 * Amic A25L40, A25L040 and A25L080 serial Flash, but can be easily
 * adapted to different devices.
 *
 * FigVFlash compiles (with -Os) to about 1.6Kb of code and requires
 * 0.5Kb of RAM for its basic operation.
 *
 * VDskErase Erases an entire chip.
 * VDskFind Finds the physical page for the given block using buff as
 *           a workspace.
 * VDskRead Copies a virtual block to RAM returning its physical page.
 * VDskWrite Copies a virtual block in RAM at buff to the Flash chip
 *           with the given physical page and prepares the Flash
 *           chip so that at least one more block can be written.
 *           buff needs to be >=512b and is used as a workspace.
 *
 * The correct procedure for writing a block is therefore:
 *
 * // Find a spare block.
 * destinationPhysicalPage=VDskFind(VDskEmptyBlk,buff);
 * VDskWrite(virtualBlockId,destinationPhysicalPage,buff);
 *
 *           
 */

#ifndef _FIGVFLASH_H

#define _FIGVFLASH_H

#include "CoreDefs.h"

typedef enum {
	kAmicFlashID4MbBot=0x2013,
	kAmicFlashID4MbSym=0x3013,
	kAmicFlashID8MbSym=0x3014
}tAmicFlashIDs;

//#define VDskBaseBlock 32*2			// 64 *.5Kb blocks in.
ushort VDskBaseBlock();

// Every VDskBlk entry takes 2 bytes.
#define kVDskVTableSize 4096
#define VDskEntriesPerBlk (kSerialFlashBlockSize/sizeof(ushort))
#define kSerialFlashBlockEntriesBits (kSerialFlashBlockSizeBits-1)
#define kVDskVTableEntries (kVDskVTableSize/sizeof(ushort))
#define VDskAltTable 0x4000
#define VDskTableWrapMask 0xc000

#define VDskEmptyBlk 0xffff
#define VDskPurgeNeeded 0xfffe
#define kVDskFull 0xfffd
#define VDskVTable1 0
#define VDskVTable0	16	// VTable2 is at 4Kb.

extern  ushort VDskSize();	// was #define VDskSize ((1024-36)*2)		// 768	// 768 * 0.5Kb blocks.
#define VDskVBlks (VDskSize()>>1)

extern void VDskErase();

extern ushort VDskBaseTable(byte *buff);

extern ushort VDskFindEntry(ushort block, byte *buff);

#define VDskEntryToPhys(entry) (VDskBaseBlock()+((entry&~VDskTableWrapMask)<<1))
extern ushort VDskFind(ushort block, byte *buff);

extern ushort VDskRead(ushort block, byte *dest);

extern void VDskPrepWrite(ushort block, byte *buff);

extern void VDskWrite(ushort vBlock, ushort physBlock, byte *src);

#define __TESTVTABLE__

#ifdef __TESTVTABLE__

extern void TestVTable(void);

#else

#define TestVTable()

#endif


#endif