/*******************************************************************/
/*          Header file fo FAT.c for  MP3 Player                   */
/*                                                                 */
/* Platform   : AVRStudio4.13 b528 + WinAVR20070122                */
/*              optimize -0s                                       */
/* Author     : bozai(Zhang Qibo)                                  */
/* E-mail     : sudazqb@163.com                                    */
/* MSN        : zhangqibo_1985@hotmail.com                         */
/* Date       : 2006-05-09                                         */
/*******************************************************************/
/*2006-05-04: fix bug of FAT init, add read capacity check         */ 
/*******************************************************************/

#ifndef __FAT_H__
#define __FAT_H__

#define CHAR	char
#define BYTE	unsigned char
#define WORD	unsigned int
#define DWORD	unsigned long

#define FIX_DIRECTORY 0		/* 1 means use fix directory, 0 for any directory */

#if FIX_DIRECTORY==0

#define  RECORD_ADDR_START 0	/* eeprom start address */
#define  RECORD_ADDR_END  512	/* eeprom end address */

#include<avr/eeprom.h>
#include<avr/pgmspace.h>
#include<stdio.h>

#endif

//#include "UART.H"
#include <stdlib.h>

//声明外部器件读写函数
//external hardware operating function
extern BYTE MMC_SD_ReadSingleBlock(DWORD sector, BYTE* buffer);
extern BYTE MMC_SD_WriteSingleBlock(DWORD sector, BYTE* buffer);
extern DWORD MMC_SD_ReadCapacity(void);


#define MSDOSFSROOT     0               // cluster 0 means the root dir
#define CLUST_FREE      0               // cluster 0 also means a free cluster
#define MSDOSFSFREE     CLUST_FREE
#define CLUST_FIRST     2             	// first legal cluster number
#define CLUST_RSRVD     0xfff6      	// reserved cluster range
#define CLUST_BAD       0xfff7     		// a cluster with a defect
#define CLUST_EOFS      0xfff8     		// start of eof cluster range
#define CLUST_EOFE      0xffff      	// end of eof cluster range


struct partrecord // length 16 bytes
{			
	BYTE	prIsActive;					// 0x80 indicates active partition
	BYTE	prStartHead;				// starting head for partition
	WORD	prStartCylSect;				// starting cylinder and sector
	BYTE	prPartType;					// partition type (see above)
	BYTE	prEndHead;					// ending head for this partition
	WORD	prEndCylSect;				// ending cylinder and sector
	DWORD	prStartLBA;					// first LBA sector for this partition
	DWORD	prSize;						// size of this partition (bytes or sectors ?)
};

        
struct partsector
{
	CHAR	psPartCode[512-64-2];		// pad so struct is 512b
	BYTE	psPart[64];					// four partition records (64 bytes)
	BYTE	psBootSectSig0;				// two signature bytes (2 bytes)
	BYTE	psBootSectSig1;
#define BOOTSIG0        0x55
#define BOOTSIG1        0xaa
};

struct extboot {
	CHAR	exDriveNumber;				// drive number (0x80)//0x00 for floopy disk 0x80 for hard disk
	CHAR	exReserved1;				// reserved should always set 0
	CHAR	exBootSignature;			// ext. boot signature (0x29)
#define EXBOOTSIG       0x29
	CHAR	exVolumeID[4];				// volume ID number
	CHAR	exVolumeLabel[11];			// volume label "NO NAME"
	CHAR	exFileSysType[8];			// fs type (FAT12 or FAT)
};

struct bootsector50 {
	BYTE	bsJump[3];					// jump inst E9xxxx or EBxx90
	CHAR	bsOemName[8];				// OEM name and version
	CHAR	bsBPB[25];					// BIOS parameter block
	CHAR	bsExt[26];					// Bootsector Extension
	CHAR	bsBootCode[448];			// pad so structure is 512b
	BYTE	bsBootSectSig0;				// boot sector signature byte 0x55 
	BYTE	bsBootSectSig1;				// boot sector signature byte 0xAA
#define BOOTSIG0        0x55
#define BOOTSIG1        0xaa
};


struct bpb50 {
        WORD	bpbBytesPerSec; // bytes per sector				//512 1024 2048 or 4096
        BYTE	bpbSecPerClust; // sectors per cluster			// power of 2
        WORD	bpbResSectors;  // number of reserved sectors	//1 is recommend
        BYTE	bpbFATs;        // number of FATs				// 2 is recommend
        WORD	bpbRootDirEnts; // number of root directory entries
        WORD	bpbSectors;     // total number of sectors
        BYTE	bpbMedia;       // media descriptor				//0xf8 match the fat[0]
        WORD	bpbFATsecs;     // number of sectors per FAT
        WORD	bpbSecPerTrack; // sectors per track
        WORD	bpbHeads;       // number of heads
        DWORD	bpbHiddenSecs;  // # of hidden sectors
        DWORD	bpbHugeSectors; // # of sectors if bpbSectors == 0
};

struct bootsector710 {
	BYTE	bsJump[3];					// jump inst E9xxxx or EBxx90
	CHAR	bsOemName[8];				// OEM name and version
	CHAR	bsBPB[53];					// BIOS parameter block
	CHAR	bsExt[26];					// Bootsector Extension
	CHAR	bsBootCode[418];			// pad so structure is 512b
	BYTE	bsBootSectSig2;				// boot sector signature byte 0x00 
	BYTE	bsBootSectSig3;				// boot sector signature byte 0x00
	BYTE	bsBootSectSig0;				// boot sector signature byte 0x55 
	BYTE	bsBootSectSig1;				// boot sector signature byte 0xAA
#define BOOTSIG0        0x55
#define BOOTSIG1        0xaa
#define BOOTSIG2        0x00
#define BOOTSIG3        0x00
};

struct bpb710 {
		WORD	bpbBytesPerSec;	// bytes per sector
		BYTE	bpbSecPerClust;	// sectors per cluster
		WORD	bpbResSectors;	// number of reserved sectors
		BYTE	bpbFATs;		// number of FATs
		WORD	bpbRootDirEnts;	// number of root directory entries
		WORD	bpbSectors;		// total number of sectors
		BYTE	bpbMedia;		// media descriptor
		WORD	bpbFATsecs;		// number of sectors per FAT
		WORD	bpbSecPerTrack;	// sectors per track
		WORD	bpbHeads;		// number of heads
		DWORD	bpbHiddenSecs;	// # of hidden sectors
// 3.3 compat ends here
		DWORD	bpbHugeSectors;	// # of sectors if bpbSectors == 0
// 5.0 compat ends here
		DWORD     bpbBigFATsecs;// like bpbFATsecs for FAT32
		WORD      bpbExtFlags;	// extended flags:
#define FATNUM    0xf			// mask for numbering active FAT
#define FATMIRROR 0x80			// FAT is mirrored (like it always was)
		WORD      bpbFSVers;	// filesystem version
#define FSVERS    0				// currently only 0 is understood
		DWORD     bpbRootClust;	// start cluster for root directory
		WORD      bpbFSInfo;	// filesystem info structure sector
		WORD      bpbBackup;	// backup boot sector
		// There is a 12 byte filler here, but we ignore it
};



// Structure of a dos directory entry.
struct direntry {
		BYTE		deName[8];      	// filename, blank filled
#define SLOT_EMPTY      0x00            // slot has never been used
#define SLOT_E5         0x05            // the real value is 0xE5
#define SLOT_DELETED    0xE5            // file in this slot deleted
#define SLOT_DIR		0x2E			// a directorymmm
		BYTE		deExtension[3]; 	// extension, blank filled
		BYTE		deAttributes;   	// file attributes
#define ATTR_NORMAL     0x00            // normal file
#define ATTR_READONLY   0x01            // file is readonly
#define ATTR_HIDDEN     0x02            // file is hidden
#define ATTR_SYSTEM     0x04            // file is a system file
#define ATTR_VOLUME     0x08            // entry is a volume label
#define ATTR_LONG_FILENAME	0x0F		// this is a long filename entry			    
#define ATTR_DIRECTORY  0x10            // entry is a directory name
#define ATTR_ARCHIVE    0x20            // file is new or modified
		BYTE        deLowerCase;    	// NT VFAT lower case flags  (set to zero)
#define LCASE_BASE      0x08            // filename base in lower case
#define LCASE_EXT       0x10            // filename extension in lower case
		BYTE        deCHundredth;   	// hundredth of seconds in CTime
		BYTE        deCTime[2];     	// create time
		BYTE        deCDate[2];     	// create date
		BYTE        deADate[2];     	// access date
		WORD        deHighClust; 		// high bytes of cluster number
		BYTE        deMTime[2];     	// last update time
		BYTE        deMDate[2];     	// last update date
		WORD        deStartCluster; 	// starting cluster of file
		DWORD       deFileSize;  		// size of file in bytes
};


// number of directory entries in one sector
#define DIRENTRIES_PER_SECTOR	0x10	//when the bpbBytesPerSec=512 

// Structure of a Win95 long name directory entry
struct winentry {
		BYTE			weCnt;			// 
#define WIN_LAST        0x40
#define WIN_CNT         0x3f
		BYTE		wePart1[10];
		BYTE		weAttributes;
#define ATTR_WIN95      0x0f
		BYTE		weReserved1;
		BYTE		weChksum;
		BYTE		wePart2[12];
		WORD       	weReserved2;
		BYTE		wePart3[4];
};

#define WIN_ENTRY_CHARS	13      // Number of chars per winentry

// Maximum filename length in Win95
// Note: Must be < sizeof(dirent.d_name)
#define WIN_MAXLEN      255

// This is the format of the contents of the deTime field in the direntry
// structure.
// We don't use bitfields because we don't know how compilers for
// arbitrary machines will lay them out.
#define DT_2SECONDS_MASK        0x1F    // seconds divided by 2
#define DT_2SECONDS_SHIFT       0
#define DT_MINUTES_MASK         0x7E0   // minutes
#define DT_MINUTES_SHIFT        5
#define DT_HOURS_MASK           0xF800  // hours
#define DT_HOURS_SHIFT          11

// This is the format of the contents of the deDate field in the direntry
// structure.
#define DD_DAY_MASK				0x1F	// day of month
#define DD_DAY_SHIFT			0
#define DD_MONTH_MASK			0x1E0	// month
#define DD_MONTH_SHIFT			5
#define DD_YEAR_MASK			0xFE00	// year - 1980
#define DD_YEAR_SHIFT			9



// Stuctures
struct FileInfoStruct
{
	unsigned long StartCluster;			//< file starting cluster for last file accessed
	unsigned long Size;					//< file size for last file accessed
	unsigned char Attr;					//< file attr for last file accessed
	//unsigned short CreateTime;			//< file creation time for last file accessed
	//unsigned short CreateDate;			//< file creation date for last file accessed
	unsigned long Sector;				//<file record place
	unsigned int Offset;				//<file record offset
};



extern unsigned char FAT_Init();//初始化
extern unsigned long FAT_NextCluster(unsigned long cluster);//查找下一簇号
extern unsigned int FAT_FindItem(unsigned long cluster, BYTE *name, struct FileInfoStruct *FileInfo);//查找文件
extern unsigned long FAT_OpenDir(BYTE * dir);//打开目录

#if FIX_DIRECTORY
	extern unsigned char Search(BYTE *dir,struct direntry *MusicInfo,WORD *Count,BYTE *type);//查找音乐文件
#else
	extern BYTE SearchInit(void);
	extern unsigned char Search(struct direntry *MusicInfo,WORD *Count,BYTE *type);//查找音乐文件
#endif

extern unsigned char FAT_LoadPartCluster(unsigned long cluster,unsigned part,BYTE * buffer);//加载文件

#endif
