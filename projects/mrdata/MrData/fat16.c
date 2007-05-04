#include <avr/pgmspace.h>
#include "types.h"
#include "mmc.h"
#include "delay.h"
#include "fat16.h"

struct fat_filedata_st fat_filedata;
u16 fat_pos, fat_size, file_pos, root_pos, sect_offs, last_rdsect=0, clcnt, startfatoffs, startfatcnt;
u08 cluster_size, fat_cnt, last_rdoffs, maxfilenum=0;
u08 filemode=0, fat_directaccess=0;
u08 fat_cardfull=0;
#define FILEMODE_CLOSED 0

extern unsigned char sharedmem[];
extern u16 file_cnt, bufpos;
extern u32 last, sect;

prog_uchar FILEENTRY[32-11-4-2] = {
0x20, 0x00, 0xc2, 0xb3, 0x92, 0x84, 0x36, 0x84, 0x36, 0,0, 0x91, 0x92, 0x84, 0x36};

//Block Size in Bytes
#define BlockSize			512

//Master Boot Record
#define MASTER_BOOT_RECORD_SECTOR	0

//Volume Boot Record location in Master Boot Record
#define VBR_ADDR 			0x1C6

//define ASCII
#define SPACE 				0x20
#define DIR_ENTRY_IS_DELETED   0xE5
#define FIRST_LONG_ENTRY	0x01
#define SECOND_LONG_ENTRY	0x42

//define DIR_Attr
#define ATTR_LONG_NAME		0x0F
#define ATTR_READ_ONLY		0x01
#define ATTR_HIDDEN			0x02
#define ATTR_SYSTEM			0x04
#define ATTR_VOLUME_ID		0x08
#define ATTR_DIRECTORY		0x10
#define ATTR_ARCHIVE		0x20

//! Usage requires little endian compiler!
struct BootSec_st 
{
	u08 BS_jmpBoot[3];
	u08 BS_OEMName[8];
	u16 BPB_BytesPerSec; //2 bytes
	u08 BPB_SecPerClus;
	u16 BPB_RsvdSecCnt; //2 bytes
	u08 BPB_NumFATs;
	u16 BPB_RootEntCnt; //2 bytes
	u16 BPB_TotSec16; //2 bytes
	u08 BPB_Media;
	u16 BPB_FATSz16; //2 bytes
	u16 BPB_SecPerTrk; //2 bytes
	u16 BPB_NumHeads; //2 bytes
	u32 BPB_HiddSec; //4 bytes
	u32 BPB_TotSec32; //4 bytes
};

//FAT12 and FAT16 Structure Starting at Offset 36
#define BS_DRVNUM			36
#define BS_RESERVED1		37
#define BS_BOOTSIG			38
#define BS_VOLID			39
#define BS_VOLLAB			43
#define BS_FILSYSTYPE		54

//FAT32 Structure Starting at Offset 36
#define BPB_FATSZ32			36
#define BPB_EXTFLAGS		40
#define BPB_FSVER			42
#define BPB_ROOTCLUS		44
#define BPB_FSINFO			48
#define BPB_BKBOOTSEC		50
#define BPB_RESERVED		52

#define FAT32_BS_DRVNUM		64
#define FAT32_BS_RESERVED1	65
#define FAT32_BS_BOOTSIG	66
#define FAT32_BS_VOLID		67
#define FAT32_BS_VOLLAB		71
#define FAT32_BS_FILSYSTYPE	82
//End of Boot Sctor and BPB Structure

//! Usage requires little endian compiler!
struct DirEntry_st {
	u08	DIR_Name[11];     //8 chars filename
	u08	DIR_Attr;         //file attributes RSHA, Longname, Drive Label, Directory
	u08	DIR_NTRes;        //set to zero
	u08	DIR_CrtTimeTenth; //creation time part in milliseconds
	u16	DIR_CrtTime;      //creation time
	u16	DIR_CrtDate;      //creation date
	u16	DIR_LastAccDate;  //last access date
	u16	DIR_FstClusHI;    //first cluster high word                 
	u16	DIR_WrtTime;      //last write time
	u16	DIR_WrtDate;      //last write date
	u16	DIR_FstClusLO;    //first cluster low word                 
	u32	DIR_FileSize;     
};


/** Calc Cluster from given Sector
    @param Sector
    @return Cluster
*/
static u16 sector2cluster(u32 s) {
	u32 temp = s - file_pos;
	if (cluster_size == 1)
		return temp+2;
	else if (cluster_size == 2)
		return (temp>>1)+2;
	else if (cluster_size == 4)
		return (temp>>2)+2;
	else if (cluster_size == 8)
		return (temp>>3)+2;
	else if (cluster_size == 16)
		return (temp>>4)+2;
	else if (cluster_size == 32)
		return (temp>>5)+2;
	else if (cluster_size == 64)
		return (temp>>6)+2;
	return (temp>>7)+2;
}


/** Calc Sector from given Cluster
    @param Cluster
    @return Sector
*/
static u32 cluster2sector(u16 c) {
	c -= 2;
	if (cluster_size == 1)
		{}
	else if (cluster_size == 2)
		c = c<<1;
	else if (cluster_size == 4)
		c = c<<2;
	else if (cluster_size == 8)
		c = c<<3;
	else if (cluster_size == 16)
		c = c<<4;
	else if (cluster_size == 32)
		c = c<<5;
	else if (cluster_size == 64)
		c = c<<6;
	else if (cluster_size == 128)
		c = c<<7;
	return c + file_pos;
}


/** Read Long File Name
    Reads data from sharedmem
    Stores result in global struct fat_filedata.name
*/
static void extractlfn(void) {
	u08 i, spos = 1, pos;

	pos = ((sharedmem[0]&0x3f)-1)*13;

	for (i=0; i<13; i++) {
		if (sharedmem[spos] == 0 || sharedmem[spos] == 0xff)
			return;
		if (pos < NAMELEN)
			fat_filedata.name[pos++] = sharedmem[spos];
		else
			return;
		spos += 2;
		if (spos == 11)
			spos = 14;
		else if (spos == 26)
			spos = 28;
	}
}


/** Filesystem-Init
    @return success [=true]
*/
u08 fat_init(void) {
	u16 root_size;
	struct BootSec_st *bs;

	mmc_read_sector(MASTER_BOOT_RECORD_SECTOR, sharedmem); // MBR lesen
	sect_offs = sharedmem[VBR_ADDR] | (sharedmem[VBR_ADDR+1]<<8); // Erste Partition: Startsektor bestimmen
	if (sect_offs > 1000 || sect_offs == 0) // Ungültiger Startsektor
		sect_offs = 0;  // Kein MBR? Annahme: SuperFloppy-Format (ohne Partitionen)

	mmc_read_sector(sect_offs, sharedmem); // Bootsektor lesen
	bs = (struct BootSec_st *)sharedmem; // Strukturinterpretation anbringen
	//fat_size = sharedmem[0x16] | (sharedmem[0x17]<<8); // alt
	fat_size = bs->BPB_FATSz16; // FAT-Größe auslesen
	if (fat_size == 0) // Ist nicht angegeben... wohl FAT32 oder ganz was Anderes
		return 0;

	//cluster_size = sharedmem[0x0d];
	cluster_size = bs->BPB_SecPerClus; // Sektoren pro Cluster auslesen
	//fat_cnt = sharedmem[0x10];
	fat_cnt = bs->BPB_NumFATs; // Anzahl FATs lesen
	if (fat_cnt == 0) // FAT-Anzahl ungültig
		return 0;

	fat_pos = sect_offs + bs->BPB_RsvdSecCnt; // Reservierte Sektoren aufaddieren -> FAT-Startsektor //(sharedmem[0x0e] | (sharedmem[0x0f]<<8)); // alt
	root_pos = fat_pos + fat_size * fat_cnt; // Rootdirectory-Startsektor berechnen
	root_size = bs->BPB_RootEntCnt; // Anzahl reservierte Rootdir-Einträge //sharedmem[0x11] | (sharedmem[0x12]<<8); // alt
	if (root_size == 0) // Ungültige Anzahl
		return 0;
	file_pos = root_pos + (root_size>>4); // Erster Datensektor

	return 1;
}


/** Char to Number Conversion, always [3 chars] to [byte]
    @param String
    @return Number
*/
static u08 atoi(unsigned char *s) {
	return (s[0]-'0')*100+(s[1]-'0')*10+(s[2]-'0');
}


/** Fileentry-Reader
    Reads sequential MMC data, must be set up to start sector of root dir
    Stores information in global struct fat_filedata
    @return success [=true]
*/
static u08 read_fileentry(void) {
	struct DirEntry_st *de;
	u32 rootsect;
	u08 lfn=0, i, c=0, e=0, rootsectoffs, fnum;

	for (i=0; i<NAMELEN; i++)
		fat_filedata.name[i] = ' ';
	fat_filedata.rootsect = 0;
	do {
		rootsect = sect;
		rootsectoffs = bufpos>>5;
		mmc_read_block(sharedmem, 32);
		de = (struct DirEntry_st *)sharedmem;
		if (sect >= file_pos)
			return 0;
		else if (sharedmem[0] == 0) {
			if (last_rdsect == 0) {
				last_rdsect = rootsect;
				last_rdoffs = rootsectoffs;
			}
			continue;
		}
		else if (sharedmem[0] == DIR_ENTRY_IS_DELETED || de->DIR_Attr == ATTR_VOLUME_ID) // Gelöscht oder Volumebezeichner
			continue;
		else if (de->DIR_Attr == ATTR_LONG_NAME) {
			if (fat_filedata.rootsect == 0) {
				fat_filedata.rootsect = rootsect;
				fat_filedata.rootsectoffs = rootsectoffs;
			}
			extractlfn();
			lfn = 1;
			e++;
		}
		else {
			fat_filedata.entries = e+1;
			if (!lfn) {
				fat_filedata.rootsect = rootsect;
				fat_filedata.rootsectoffs = rootsectoffs;
				for (i=0; i<8; i++)
					fat_filedata.name[c++] = sharedmem[i]; // de->DIR_Name[i]
				fat_filedata.name[c++] = '.';
				for (i=8; i<11; i++)
					fat_filedata.name[c++] = sharedmem[i];
				if (fat_filedata.name[0] == 'M' && fat_filedata.name[1] == 'D' 
				  && fat_filedata.name[2] >= '0' && fat_filedata.name[2] <= '2' 
				  && fat_filedata.name[3] >= '0' && fat_filedata.name[3] <= '9' 
				  && fat_filedata.name[4] >= '0' && fat_filedata.name[4] <= '9') {
				  	fnum = atoi(&fat_filedata.name[2]);
				  	if (fnum > maxfilenum)
						maxfilenum = fnum;
				}
			}
			fat_filedata.len = de->DIR_FileSize; //sharedmem[0x1c] | (sharedmem[0x1d]<<8) | ((u32)sharedmem[0x1e]<<16) | ((u32)sharedmem[0x1f]<<24);
			fat_filedata.startsect = cluster2sector(de->DIR_FstClusLO); //cluster2sector(sharedmem[0x1a] | (sharedmem[0x1b]<<8));
			if (de->DIR_Attr & ATTR_DIRECTORY)
			{
				lfn = 0; c = 0; e = 0; // War keine Datei, also weitersuchen
				fat_filedata.rootsect = 0;
			}
			else {
				return 1;
			}
		}
	} while (1);
}


/** Searches for a file entry with a given index
    @param File Number
    @return File found [=true]
*/
u08 fat_read_filedata(u08 nr) {	// new file data in global struct
	if (nr >= file_cnt)
		return 0;
	fat_directaccess = 1;
	mmc_load_start(root_pos);
	if (!read_fileentry()) {
		fat_directaccess = 0;
		return 0;
	}
	while (nr--)
		if (!read_fileentry()) {
			fat_directaccess = 0;
			return 0;
		}
	mmc_complete_read();
	fat_directaccess = 0;
	return 1;
}


/** Count Midi Files
    Stores result in global var file_cnt.
    Detects next free sector (cluster) after last file
*/
void fat_count_files(void) {
	fat_directaccess = 1;
	mmc_load_start(root_pos);
	file_cnt = 0;
	while (read_fileentry()) {
		file_cnt++;
	}
	mmc_complete_read();
	fat_directaccess = 0;
}


/** Deletes current file
    Removes FAT entries as well
*/
void fat_delete_file(void) {
	u16 startcl = sector2cluster(fat_filedata.startsect);	// Rechnet den Cluster der zu löschenden Datei aus
	u16 fatsect, fatcl, nfatsect;
	u08 offscl, i;

	fat_directaccess = 1;
	// FATs bearbeiten
	for (i=0; i<fat_cnt; i++) {
		fatsect = fat_pos + (startcl>>8) + fat_size*i;	// Sektor der FAT mit dem Startcluster der zlD
		offscl = (u08)startcl;	// Eintragsposition im FAT-Sektor bestimmen
		mmc_read_sector(fatsect, sharedmem);	// FAT-Sektor lesen
		while (1) {
			fatcl = sharedmem[offscl<<1];	// FAT-Eintrag lesen
			fatcl |= sharedmem[(offscl<<1)+1] << 8;
			sharedmem[offscl<<1] = 0;	// Eintrag löschen (frei markieren)
			sharedmem[(offscl<<1)+1] = 0;
			if (fatcl < 0xfff0 && fatcl > 0) {	// Wenn Eintrag auf weiteren FAT-Eintrag zeigt
				nfatsect = fat_pos + (fatcl>>8);	// Neuen FAT-Sektor bestimmen
				if (i == 1)
			 		nfatsect += fat_size;	// Kopie der FAT
				offscl = (u08)fatcl;	// Eintragsposition im FAT-Sektor bestimmen
				if (nfatsect != fatsect) {	// Falls nächster Eintrag in einem anderen FAT-Sektor liegt
					mmc_write_sector(fatsect, sharedmem);	// Geänderten FAT-Sektor wegschreiben
					fatsect = nfatsect;	// Neuen Sektor festlegen
					mmc_read_sector(fatsect, sharedmem);	// Diesen FAT-Sektor auslesen
				}
			}
			else {
				break;	// Letzter FAT-Eintrag gewesen: Schleife verlassen
			}
		}
		mmc_write_sector(fatsect, sharedmem);	// geänderten FAT-Sektor wegschreiben
	}

/*lcd_setcur(0,0);
lcd_hex_u16(fat_filedata.rootsect);
delay_ms(2000);*/
	// Rootdir ändern
	fatsect = fat_filedata.rootsect;
	mmc_read_sector(fatsect, sharedmem);	// Betroffenen Rootdirsektor laden
	offscl = fat_filedata.rootsectoffs;
/*lcd_data(',');
lcd_hex_u16(fatsect);
lcd_hex_u08(offscl);
delay_ms(2000);*/
	for (i=0; i<fat_filedata.entries; i++) {	// Für jeden Root-Eintrag dieser Datei tue:
		if (offscl >= 16) {	// Sektorgrenze überschritten
			mmc_write_sector(fatsect, sharedmem);	// Rootdirsektor speichern
			fatsect++;	// Nächsten Rootdirsektor bestimmen
			offscl = 0;
			mmc_read_sector(fatsect, sharedmem);	// Betroffenen Rootdirsektor laden
		}
		sharedmem[offscl*32] = DIR_ENTRY_IS_DELETED;	// Erstes Byte im Dateinamen auf "gelöscht" setzen
		offscl++;
	}
	mmc_write_sector(fatsect, sharedmem);
	fat_directaccess = 0;
}


/** Convert Number to String
    @param (OUT) String
    @param (IN) Number
*/
static void wrstr(unsigned char *s, unsigned char d) {
	u08 zehner=0, hunderter=0;
	while (d >= 100) {
		d -= 100;
		hunderter++;
	}
	while (d >= 10) {
		d -= 10;
		zehner++;
	}
	*s = hunderter+'0'; s++;
	*s = zehner+'0'; s++;
	*s = d+'0';
}


/** Create New File
    Uses information from global struct fat_filedata
*/
void fat_new_file(void) {
	u16 startcl = sector2cluster(fat_filedata.startsect);	// Rechnet den Cluster der zu erstellenden Datei aus
	u16 fatsect, fatcl, lastfs, cnt;
	u08 offscl, i, lastco, dirty;

	fat_directaccess = 1;
//	last = cluster2sector(sector2cluster(last)+1);	// Nächster Sektor für neue Datei
	fat_filedata.name[0] = 'M';
	fat_filedata.name[1] = 'D';
	wrstr(&(fat_filedata.name[2]), ++maxfilenum);
	fat_filedata.name[5] = ' ';
	fat_filedata.name[6] = ' ';
	fat_filedata.name[7] = ' ';
	fat_filedata.name[8] = 'T';
	fat_filedata.name[9] = 'X';
	fat_filedata.name[10] = 'T';
	fat_filedata.name[11] = 0;

	// Rootdireintrag erstellen
	fat_filedata.entries = 1;
	fat_filedata.rootsect = last_rdsect;
	fat_filedata.rootsectoffs = last_rdoffs;
	fatsect = last_rdsect;
	mmc_read_sector(fatsect, sharedmem);	// Betroffenen Rootdirsektor laden
	offscl = last_rdoffs;
	for (i=0; i<11; i++)
		sharedmem[offscl*32+i] = fat_filedata.name[i];
	for (i=0; i<15; i++)
		sharedmem[offscl*32+i+11] = (u08)pgm_read_byte( &(FILEENTRY[i]) );
	sharedmem[offscl*32+31] = (u08)(fat_filedata.len >> 24);
	sharedmem[offscl*32+30] = (u08)(fat_filedata.len >> 16);
	sharedmem[offscl*32+29] = (u08)(fat_filedata.len >> 8);
	sharedmem[offscl*32+28] = (u08)fat_filedata.len;
	sharedmem[offscl*32+27] = (u08)(startcl >> 8);
	sharedmem[offscl*32+26] = (u08)startcl;
	mmc_write_sector(fatsect, sharedmem);

/*lcd_setcur(0,0);
lcd_hex_u16(last_rdsect);
lcd_hex_u08(last_rdoffs);
delay_ms(1000);*/
	last_rdoffs++;
	if (last_rdoffs == 16) {
		last_rdoffs = 0;
		last_rdsect++;
	}
/*lcd_data(',');
lcd_hex_u16(last_rdsect);
lcd_hex_u08(last_rdoffs);
delay_ms(1000);*/
/*
	// FATs bearbeiten
	for (i=0; i<fat_cnt; i++) {
		startcl = sector2cluster(fat_filedata.startsect);	// Rechnet den Cluster der zu erstellenden Datei aus
		clcnt = sector2cluster(last)-startcl;
		fatsect = fat_pos + (startcl>>8) + fat_size*i;	// Sektor der FAT mit dem Startcluster der zlD
		offscl = (u08)startcl;	// Eintragsposition im FAT-Sektor bestimmen
		mmc_read_sector(fatsect, sharedmem);	// FAT-Sektor lesen
		while (clcnt) {
			startcl++;
			if (clcnt == 1)
				val = 0xffff;
			else
				val = startcl;
			sharedmem[offscl<<1] = (u08)val;	// FAT-Eintrag schreiben
			sharedmem[(offscl<<1)+1] = val >> 8;
			offscl++;
			if (offscl == 0 && clcnt > 1) {
				mmc_write_sector(fatsect, sharedmem);	// FAT-Sektor speichern
				fatsect++;	// Nächsten FAT-Sektor bestimmen
				mmc_read_sector(fatsect, sharedmem);
			}
			clcnt--;
		}
		mmc_write_sector(fatsect, sharedmem);	// geänderten FAT-Sektor wegschreiben
	}*/
	// FATs bearbeiten
	for (i=0; i<fat_cnt; i++) {
		startcl = sector2cluster(fat_filedata.startsect);	// Rechnet den Cluster der zu erstellenden Datei aus
		fatsect = fat_pos + (startcl>>8) + fat_size*i;	// Sektor der FAT mit dem Startcluster der zlD
		offscl = (u08)startcl;	// Eintragsposition im FAT-Sektor bestimmen
		cnt = clcnt;
//		cnt = (fat_filedata.len-1) / 512 / cluster_size + 1;

		mmc_read_sector(fatsect, sharedmem);	// FAT-Sektor lesen
		if (cnt == 1) {	// Datei braucht bloß 1 Cluster
			sharedmem[offscl<<1] = 0xff;	// FAT-Eintrag schreiben
			sharedmem[(offscl<<1)+1] = 0xff;
		}
		else {
			lastfs = fatsect;
			lastco = offscl;
			dirty = 0;
			while (1) {
				offscl++;
				if (offscl == 0) {
					if (dirty) {
						mmc_write_sector(fatsect, sharedmem);
						dirty = 0;
					}
					fatsect++;
					mmc_read_sector(fatsect, sharedmem);
				}
				fatcl = sharedmem[offscl<<1];	// FAT-Eintrag lesen
				fatcl |= sharedmem[(offscl<<1)+1] << 8;
				if (fatcl == 0) {
					if (lastfs != fatsect) {
						if (dirty) {
							mmc_write_sector(fatsect, sharedmem);
							dirty = 0;
						}
						mmc_read_sector(lastfs, sharedmem);
					}
					if (cnt > 1) {
						sharedmem[lastco<<1] = offscl;	// FAT-Eintrag schreiben
						sharedmem[(lastco<<1)+1] = fatsect - fat_pos - i*fat_size;
						cnt--;
					}
					else {
						sharedmem[lastco<<1] = 0xff;	// FAT-Eintrag schreiben
						sharedmem[(lastco<<1)+1] = 0xff;
						break;
					}
					dirty = 1;
					if (lastfs != fatsect) {
						mmc_write_sector(lastfs, sharedmem);
						dirty = 0;
						mmc_read_sector(fatsect, sharedmem);
					}
					lastfs = fatsect;
					lastco = offscl;
				}
			}
		}
		mmc_write_sector(fatsect, sharedmem);	// geänderten FAT-Sektor wegschreiben
	}
	fat_directaccess = 0;
}


/** Find next free cluster
    @return Cluster Number
*/
static u16 find_free_cluster(void) {
	u16 fatsect, fatcl, cnt=clcnt-startfatcnt, pcnt=startfatcnt;
	u08 offscl=0;

	// FAT bearbeiten
	fatsect = fat_pos+startfatoffs;	// Sektor der FAT
	if (startfatoffs == 0)
		offscl = 2;
	fat_directaccess = 1;
	mmc_read_sector(fatsect, sharedmem);	// FAT-Sektor lesen
	fat_directaccess = 0;
	while (1) {
		fatcl = sharedmem[offscl<<1];	// FAT-Eintrag lesen
		fatcl |= sharedmem[(offscl<<1)+1] << 8;
		if (fatcl == 0) {
			pcnt++;
			cnt--;
			if (cnt == 0)
				return ((fatsect-fat_pos)<<8) + offscl;
		}
		offscl++;
		if (offscl == 0) {
			fatsect++;
			startfatoffs++;
			startfatcnt = pcnt;
			if (fatsect == root_pos)	// Volle Karte!
				fat_cardfull = 1;
//				return -1;
			fat_directaccess = 1;
			mmc_read_sector(fatsect, sharedmem);	// FAT-Sektor lesen
			fat_directaccess = 0;
		}
	}
}


/** Opens a File
    @param mode
    @param file number (not needed for write operation)
    @return success
*/
u08 fat_openfile(u08 mode, u08 nr)
{
	if (filemode != FILEMODE_CLOSED)
		return 0;
	if (mode == FILEMODE_READ) {
		if (nr >= file_cnt)
			return 0;
		if (!fat_read_filedata(nr))
			return 0;
		mmc_load_start(fat_filedata.startsect);
		filemode = FILEMODE_READ;
		return 1;
	}
	else if (mode == FILEMODE_WRITE) {
		filemode = FILEMODE_WRITE;
		clcnt = 1;
		startfatoffs = 0;
		startfatcnt = 0;
		fat_filedata.startsect = cluster2sector(find_free_cluster());
	}
	return 0;
}


/** Closes Current File
    @return success
*/
u08 fat_closefile(void)
{
	if (filemode == FILEMODE_CLOSED)
		return 0;
	if (filemode == FILEMODE_READ) {
		mmc_complete_read();
		filemode = FILEMODE_CLOSED;
		return 1;
	}
	else if (filemode == FILEMODE_WRITE) {
		filemode = FILEMODE_CLOSED;
		fat_new_file();
	}
	return 0;
}


/** Finds next Sector for Read/Write
    Works on global var sect
    @return success
*/
u08 fat_getnextsector(void)
{
	u16 startcl = sector2cluster(sect);	// Rechnet den aktuellen Cluster der Datei aus

	if (filemode == FILEMODE_READ) {
		u16 fatsect, fatcl;
		u08 offscl;

		if (startcl == sector2cluster(sect+1)) {
			sect++;
			return 1;
		}
		fatsect = fat_pos + (startcl>>8);	// Sektor der FAT mit dem Cluster der Datei
		offscl = (u08)startcl;	// Eintragsposition im FAT-Sektor bestimmen
		fat_directaccess = 1;
		mmc_read_sector(fatsect, sharedmem);	// FAT-Sektor lesen
		fat_directaccess = 0;
		fatcl = sharedmem[offscl<<1];	// FAT-Eintrag lesen
		fatcl |= sharedmem[(offscl<<1)+1] << 8;
		if (fatcl >= 2 && fatcl < 0xfff0) {
			sect = cluster2sector(fatcl);
			return 1;
		}
	}
	else if (filemode == FILEMODE_WRITE) {
		if (startcl == sector2cluster(sect+1)) {
			sect++;
			return 1;
		}
		clcnt++;
		sect = cluster2sector(find_free_cluster());
		return 1;
	}
	return 0;
}

