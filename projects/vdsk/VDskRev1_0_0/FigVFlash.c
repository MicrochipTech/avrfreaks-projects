/**
 *
 * FigVFlash.c is part of the FIGnition firmware.
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
 * See FigVFlash.h for usage.
 */

#include "FigVFlash.h"

ushort VDskBaseBlock()
{
	ushort base=0;
	switch(SerialFlashID()) {
	case kAmicFlashID4MbBot:
		base= 256;	// 256 blocks * 256 bytes, 64Kbytes.
		break;
	case kAmicFlashID4MbSym:
	case kAmicFlashID8MbSym:
		base= 128;	// 128 block * 256 bytes, 32Kbytes.
		break;
	}
	return base;
}

ushort VDskEraseBlocks()
{
	ushort eraseSize=0;
	switch(SerialFlashID()) {
	case kAmicFlashID4MbBot:
		eraseSize= 256;	// 64Kb
		break;
	case kAmicFlashID4MbSym:
	case kAmicFlashID8MbSym:
		eraseSize= 16;	// 4Kb
		break;
	}
	return eraseSize;
}

ushort VDskSize()
{
	ushort vDskSize=0;
	switch(SerialFlashID()) {
	case kAmicFlashID4MbBot:
	case kAmicFlashID4MbSym:
		vDskSize= (1<<11);	// 256 blocks * 256 bytes, 64Kbytes.
		break;
	case kAmicFlashID8MbSym:
		vDskSize= (1<<12);	// 128 block * 256 bytes, 32Kbytes.
		break;
	}
	return vDskSize-VDskBaseBlock();
}


#define VTablePageToEntry(page) ((page)>>1)
#define VTableEntries() VTablePageToEntry(VDskSize())

#define VTableNonPurgeLimit() VTablePageToEntry(VDskSize()-VDskEraseBlocks())

ushort VDskPhysPageForEntry(ushort entry)
{
	entry&=~VDskTableWrapMask;
	if (entry>=VDskSize()) {
		entry-=VDskSize();
	}
	return (entry<<1)+VDskBaseBlock();
}

void VDskErase()
{
	// erase the disk.
	ushort ePage;
	byte symmetrical=SerialFlashID()!=kAmicFlashID4MbBot;
	ushort eSize=VDskEraseBlocks();
	for(ePage=0;ePage<VDskSize()+VDskBaseBlock();ePage+=16) {
		if(symmetrical || ePage==0 || ePage==16 || ePage==32 || ePage==64 ||
				(ePage&(eSize-1))==0) {
			PrintAt(0,21);
			DotHex(ePage);
			SerialFlashEraseSector(ePage);
		}
	}
}

#define VEntryRd ((ushort*)buff)

/**
 *   0: Initially both VTables are erased.
 *      VTable1's last entry is 0xffff (not full), so we start with
 *      VTable0.
 *      When VTable0 is full, the VTable base is still VTable0, but
 *         we overflow VTable reading into VTable1.
 *      When VTable1's last entry is full, we erase VTable0; now
 *      VTable1 is the base table.
 *   Given VDskSize. The number of Flash pages used in a VTable
 *   will be: VDskSize/VDskEntriesPerBlk.
 *   So, the number of Tables per VTableSector will be:
 *     kVDskTables=kVDskVTableEntries/VDskSize();
 *   So, the last VTable entry is: kVDskLastTableEntry=kVDskTables*VDskSize()-1.
 *   Which is block
 **/
ushort VDskEndTable(byte *buff,ushort base)
{
	ushort tmp=VTableEntries()-1;
	SerialFlashReadBlock(base+(tmp>>kSerialFlashBlockEntriesBits),buff); // read the first block.
	return VEntryRd[tmp&(VDskEntriesPerBlk-1)];
}

ushort VDskBaseTable(byte *buff)
{
	InterruptSpi();
	ushort tmp=VDskEndTable(buff,0);
	if(tmp==VDskEmptyBlk && VEntryRd[(VTableEntries()-2)
						&(VDskEntriesPerBlk-1)]!=VDskEmptyBlk) {
		// it could be the special case where the previous
		// entry isn't VDskEmptyBlk, but the VDskVTable0's
		// last entry is, but we invert it here.
		tmp=VDskEndTable(buff,VDskVTable0);
		return (tmp==VDskEmptyBlk)? 0:VDskVTable0;		
	}
	return (tmp==VDskEmptyBlk)? VDskVTable0:0;
}

//#define __TESTVBLKFIND__
//#define __TESTVBLKFIND2__

/**
 *   VDskFind is a fairly complex algorithm. It is..
 *   currently very badly documented :-(
 *
 *   Vars:
 *   vTableEntry : The absolute Vtable entry.
 **/
#define kVDskWrapIncrement 0x4000
#define kVDskWrapMax 0x8000
#define kVDskPurgeSrcBuff (buff+256)
#define VPurgeEntryRd ((ushort*)kVDskPurgeSrcBuff)

byte VDskReadVTableBlock(ushort entry, byte *buff)
{
	if(VDskBaseTable(buff))
		entry^=VDskAltTable;// convert to abs entry.
	byte vTableBlk= (byte)(((entry&~VDskTableWrapMask)>>
					 kSerialFlashBlockEntriesBits)+
					((entry&VDskAltTable)?VDskVTable0:0));
	SerialFlashReadBlock(vTableBlk,buff);
	return vTableBlk;
}


ushort VDskFindEntry(ushort block, byte *buff)
{
	ushort vBlk=0,found=kVDskWrapMax;
	ushort vTableEntry=0;
	ushort tableEntries=VTableEntries();
	do{
		// Do we need to read from a new VTable block?
		if((vTableEntry&~VDskTableWrapMask)>=tableEntries) {
			// Start on the next table.
			vTableEntry=(vTableEntry&VDskTableWrapMask)+kVDskWrapIncrement;	
		}
		if((vTableEntry&(VDskEntriesPerBlk-1))==0) {
			VDskReadVTableBlock(vTableEntry,buff);
		}
		vBlk=VEntryRd[vTableEntry&(VDskEntriesPerBlk-1)];
#ifdef __TESTVBLKFIND__
#warning __TESTVBLKFIND__
		PrintAt(0,23);
		DotHex(block);
		DotHex(vTableEntry);
		DotHex(vBlk);
		Key();
#endif
		if(vBlk==block || (vBlk==VDskEmptyBlk && found==kVDskWrapMax)) {
			found=vTableEntry;
		}
		vTableEntry++;
	}while(vBlk!=VDskEmptyBlk && vTableEntry<kVDskWrapMax);
	/**
	 * We've got to the end of the used Flash. 
	 **/
	return found;
}

ushort VDskFind(ushort block, byte *buff)
{
	InterruptSpi();
	block=VDskEntryToPhys(VDskFindEntry(block,buff));
#ifdef __TESTVBLKFIND2__
		//PrintAt(0,24);
		DotHex(block);
		Key();
#endif
	return block;
}


ushort VDskRead(ushort block, byte *dest)
{
	ushort phys;
	if(block&0x4000) {
		InterruptSpi();
		phys=block&~VDskTableWrapMask;
	}
	else {
		phys=VDskFind(block, dest);	// it'll interrupt SPI.
		SerialFlashReadBlock(phys+1,dest+256);
	}
	SerialFlashReadBlock(phys,dest);
	SramFlush();
	return phys;
}

/**
 *   The purge algorithm works as follows:
 *   If we're using VDskVTable0 (the one that's actually second on the
 *   Flash disk), then blocks have been building up and the last sector
 *   is free (and erased). So we copy backwards. Our 256 bytes in the second
 *   half of our buffer give us 2048 bits of memory with which to mark blocks
 *   as being purged.
 *
 *   Or maybe I'm going to go for the round-robin purge algorithm.
 *   Here, we purge the first sector into the erased sector and then
 *   erase the purged sector. We repeat this operation until at least one
 *   block has been freed or we're back where we started (and the disk is
 *   full). To do this we also want to modify the normal writeback algorithm
 *   to zero dirty sectors... or do we need to? The Algorithm is O^2 for
 *   a purge since we'd have to read the vblock table.
 *   The VBlock table also needs to be purged from time to time.
 *   On a 4MBit Flash, 4Kb has room for 2K entries which means it'll
 *   fill up after a second complete re-write (1536 entries). But on an
 *   8Mb Flash, we'll need to purge the old VTable when we get to the new
 *   one.
 *   So, block searching is more complex. There's now several cases:
 *   0: Initially both VTables are erased.
 *      VTable1's last entry is 0xffff (not full), so we start with
 *      VTable0.
 *   1: VTable0 is being filled and VTable1 is erased.
 *      As above.
 *   2: VTable0 is full and VTable1 is being filled.
 *      As above, we overflow our VTable reading into VTable1 after VTable0.
 *   3: VTable0 is full, VTable1 has become full.
 *       This is an ambiguous state - when VTable1 is about to be full,
 *       VTable0 should be erased.
 *   4: VTable0 is being filled, VTable1 is full.
 *       VTable1's last entry isn't 0xffff, so we start with VTable1.
 *   5: VTable0 has become full, VTable1 is full.
 *       This is an ambiguous state - when VTable0 is about to be full,
 *       VTable1 should be erased. This will work even at the end of case 1.
 *   
 *   With this algorithm we also solve the problem of having to purge the
 *   VTables themselves, since when one VTable is full, all the
 *   entries in the other VTable will have been naturally purged.
 *
 *   Note also: this means that purging is the default mode.
 **/

#ifdef __TESTVTABLE__

void TestVTable(void)
{
	ushort tablePage;
	InterruptSpi();
	Cls();
	for(tablePage=0;tablePage<VDskVBlks;tablePage++) {
		PrintAt(0,21);
		DotHex(tablePage+VDskBaseTable(gVideoBuff));
		SerialFlashReadBlock(tablePage+VDskVTable0,gVideoBuff);
		Key();
	}
	SramFlush();
}

#endif

/**
 * We need to purge when the current sector is on a boundary
 * and the following sector is being used, and we need to read
 * this from the VTable, ie. the part of the vtable corresponding
 * to the next sector is being used.
 **/
byte VDskNeedsPurge(ushort entries, byte *buff)
{
	byte needsPurge=0;
	// is it on a boundary?
	if(entries>=VTableNonPurgeLimit() &&
	   (entries&VTablePageToEntry(VDskEraseBlocks()-1))
							==0)	// But we only need to purge when
		needsPurge=1;					// we're on a sector boundary.
	return needsPurge;
}

//#define __DEBUGPAUSEMSG__

#ifdef __DEBUGPAUSEMSG__

void DebugPauseMsg(char msg,ushort val)
{
	PrintAt(0,23);
	Emit('%');
	Emit(msg);
	DotHex(val);
	Emit(' ');
	Key();
}

#else

#define DebugPauseMsg(msg,val)

#endif

/**
 * VDskPurge purges a single source sector into the next available dest sector.
 * dstSectorBlk. The simple algorithm is as follows.
 * Input: block contains the virtual block number.
 *        Entry contains the physical entry into the table for the blocks
 *          we're purging into. We need to erase it first.
 *        baseTable contains the current base table block number.
 *        buff contains the pointer to our 512b internal buffer.
 *
 * When we call VDskPurge it's because there's exactly 1 erased sector left.
 * Therefore the source sector will be the next sector: dstSectorBlk+
 * VDskEraseBlocks();, unless it's at the end, in which case it's the first
 * block.
 *
 * The source sector corresponds to a page in the Vtable itself. For large
 * sectored Flash devices, 64Kb or more, there's 128 VTable entries in the
 * sector, which corresponds to a whole page in the VTable, so it'll be
 * page-aligned. For 4Kb sectors there's 8 VTable entries in the sector.
 *
 * The source sector entry is always entry-VDskSize(), but this is equivalent
 * to the same entry in the other VTable+VDskEraseBlocks() unless we're at the
 * end of a VTable, in which case it's the beginning of the VTable.
 *
 **/
//#define __DEBUGPURGE__ 

/**
 * When purging a sector the srcSectorBlk begins at sector after the
 * destination sector. That's because purging only happens when
 * the Flash is almost full and there's only 1 erased sector, the
 * destination sector. Therefore the next proper sector will be
 * the least recently written sector, sector '0' when the erased
 * sector is the last one, then sector '1' when the erased sector
 * is sector '0', then sector '2' when the erased sector is sector
 * '1' etc.
 *
 * The srcEntry is the place within the VTable corresponding to
 * the srcBlock.
 *
 * I think the only difference is that srcSectorBlock
 **/
//#define __DEBUGNONPURGE__

byte VDskPurge(ushort entry,byte *buff)
{
	byte eraseEntries=VTablePageToEntry(VDskEraseBlocks());
	char unpurgedEntries=eraseEntries;
	ushort srcEntry=VTablePageToEntry(VDskSize())-eraseEntries;
	if((entry&~VDskTableWrapMask)>=srcEntry) {
		srcEntry=entry-srcEntry; // it fits on the current VTable.
	}
	else  // next position in the other table.
		srcEntry=(entry+eraseEntries)^VDskAltTable;

	ushort srcSectorBlk=VDskPhysPageForEntry(srcEntry);
	ushort dstSectorBlk=VDskPhysPageForEntry(entry);
	// Convert the srcEntry to a block to load, based on the baseTable
	// and the srcEntry.
	// So, that's 16*2048 = 32768, which will work for 4 and 8Mb devices.
	VDskReadVTableBlock(srcEntry,kVDskPurgeSrcBuff);
	// OK. Finally we have the correct source block loaded into the
	// kVDskPurgeSrcBuff and the correct offset, so we can start purging.
	do{
		// Get a potential Virtual Block entry.
		ushort testVBlock=VPurgeEntryRd[(srcEntry&(VDskEntriesPerBlk-1))];
		// Get the real entry for this virtual page.
		ushort cmpEntry=VDskFindEntry(testVBlock,buff);
		if(srcEntry==cmpEntry) {	// it's clean, so copy.
#ifdef __DEBUGNONPURGE__
			printf(">>0x%x 0x%x 0x%x",testVBlock,cmpEntry,dstSectorBlk);
#endif
			DebugPauseMsg('C',testVBlock);
			DebugPauseMsg('D',cmpEntry);
			DebugPauseMsg('E',dstSectorBlk);
			cmpEntry=VDskEntryToPhys(cmpEntry);
			SerialFlashReadBlock(cmpEntry,buff); // read the first part.
			SerialFlashWriteBlock(dstSectorBlk++,buff);
			SerialFlashReadBlock(cmpEntry+1,buff); // read the 2nd part.
			SerialFlashWriteBlock(dstSectorBlk++,buff);
			VDskPrepWrite(testVBlock,buff); // update the table, without purge.
			unpurgedEntries--;
		}
		srcEntry++;
	}while(--eraseEntries);
	// Finally we can erase the original source sector. If every srcEntry was clean,
	// Then there's been a write for every block and we'll need to purge the
	// next sector.
	DebugPauseMsg('A',srcSectorBlk);
	SerialFlashEraseSector(srcSectorBlk);
	// finally, super purge, purge the current base table if necessary.
	return unpurgedEntries<=0;
}

void VDskPrepWrite(ushort block, byte *buff)
{
	ushort entry=VDskFindEntry(VDskEmptyBlk, buff);	// find the next empty sector.
	if(entry>=VTablePageToEntry(VDskSize())-1+VDskAltTable) {
#ifdef __DEBUGNONPURGE__
		printf("->0x%x",VDskBaseTable(buff));
#endif
		DebugPauseMsg('B',VDskBaseTable(buff));
		SerialFlashEraseSector(VDskBaseTable(buff));
		entry^=VDskAltTable; // change the entry's Basetable.
	}
	// Bit 15 is 1 if it's in the second block.
	// We need to purge a block if entry>=VTableEntries(), or if
	// either block's last entry is not 0xffff i.e. if we've ever moved
	// on to a different vTable.
	// Finally it's OK to rewrite the table.
	byte vTableBlk=VDskReadVTableBlock(entry, buff);
	entry &=(VDskEntriesPerBlk-1);
	VEntryRd[entry]=block;
	SerialFlashWriteBlock(vTableBlk,buff);
}

void VDskWrite(ushort vBlock, ushort physBlock, byte *src)
{
	InterruptSpi();
	SerialFlashWriteBlock(physBlock,src);
	SerialFlashWriteBlock(physBlock+1,src+256);
	VDskPrepWrite(vBlock,src); // prepare to write to an empty block
	byte purgeAllowed=1;
	do {
		ushort purgeEntry=VDskFindEntry(VDskEmptyBlk, src);
		if(VDskNeedsPurge(purgeEntry,src+256)==0)
			purgeAllowed=0;
		if(purgeAllowed) {
#ifdef __DEBUGPURGE__
			Emit('%');
			Key();
#endif
			purgeAllowed=VDskPurge(purgeEntry,src);
#ifdef __DEBUGNONPURGE__
			if (purgeAllowed!=0) {
				printf("&%d",purgeAllowed);
			}
#endif

#ifdef __DEBUGPURGE__
			TestVTable();
#endif
		}
	} while (purgeAllowed);
	SramFlush();
}

