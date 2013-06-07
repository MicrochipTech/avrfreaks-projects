/*
 * CX80_Ff.h
 *
 * Created: 25/03/2013 14:38:18
 *  Author: Calogiuri Enzo Antonio
 */ 


#ifndef CX80_FF_H_
#define CX80_FF_H_

#include "CX80.h"
#include "CX80_FfConf.h"

#if _MULTI_PARTITION		/* Multiple partition configuration */
typedef struct {
	byte pd;	/* Physical drive number */
	byte pt;	/* Partition: 0:Auto detect, 1-4:Forced partition) */
} PARTITION;
extern PARTITION VolToPart[];	/* Volume - Partition resolution table */
#define LD2PD(vol) (VolToPart[vol].pd)	/* Get physical drive number */
#define LD2PT(vol) (VolToPart[vol].pt)	/* Get partition index */

#else						/* Single partition configuration */
#define LD2PD(vol) (vol)	/* Each logical drive is bound to the same physical drive number */
#define LD2PT(vol) 0		/* Always mounts the 1st partition or in SFD */

#endif

#if _LFN_UNICODE			/* Unicode string */
#if !_USE_LFN
#error _LFN_UNICODE must be 0 in non-LFN cfg.
#endif
#ifndef _INC_TCHAR
typedef WCHAR TCHAR;
#define _T(x) L ## x
#define _TEXT(x) L ## x
#endif

#else						/* ANSI/OEM string */
#ifndef _INC_TCHAR
typedef char TCHAR;
#define _T(x) x
#define _TEXT(x) x
#endif

#endif

/* File system object structure (FATFS) */

typedef struct {
	byte	fs_type;		/* FAT sub-type (0:Not mounted) */
	byte	drv;			/* Physical drive number */
	byte	csize;			/* Sectors per cluster (1,2,4...128) */
	byte	n_fats;			/* Number of FAT copies (1,2) */
	byte	wflag;			/* win[] dirty flag (1:must be written back) */
	byte	fsi_flag;		/* fsinfo dirty flag (1:must be written back) */
	word	id;				/* File system mount ID */
	word	n_rootdir;		/* Number of root directory entries (FAT12/16) */
#if _MAX_SS != 512
	word	ssize;			/* Bytes per sector (512, 1024, 2048 or 4096) */
#endif
#if _FS_REENTRANT
	_SYNC_t	sobj;			/* Identifier of sync object */
#endif
#if !_FS_READONLY
	DWord	last_clust;		/* Last allocated cluster */
	DWord	free_clust;		/* Number of free clusters */
	DWord	fsi_sector;		/* fsinfo sector (FAT32) */
#endif
#if _FS_RPATH
	DWord	cdir;			/* Current directory start cluster (0:root) */
#endif
	DWord	n_fatent;		/* Number of FAT entries (= number of clusters + 2) */
	DWord	fsize;			/* Sectors per FAT */
	DWord	fatbase;		/* FAT start sector */
	DWord	dirbase;		/* Root directory start sector (FAT32:Cluster#) */
	DWord	database;		/* Data start sector */
	DWord	winsect;		/* Current sector appearing in the win[] */
	byte	win[_MAX_SS];	/* Disk access window for Directory, FAT (and Data on tiny cfg) */
} FATFS;

/* File object structure (FIL) */

typedef struct {
	FATFS*	fs;				/* Pointer to the owner file system object */
	word	id;				/* Owner file system mount ID */
	byte	flag;			/* File status flags */
	byte	pad1;
	DWord	fptr;			/* File read/write pointer (0 on file open) */
	DWord	fsize;			/* File size */
	DWord	sclust;			/* File start cluster (0 when fsize==0) */
	DWord	clust;			/* Current cluster */
	DWord	dsect;			/* Current data sector */
#if !_FS_READONLY
	DWord	dir_sect;		/* Sector containing the directory entry */
	byte*	dir_ptr;		/* Ponter to the directory entry in the window */
#endif
#if _USE_FASTSEEK
	DWord*	cltbl;			/* Pointer to the cluster link map table (null on file open) */
#endif
#if _FS_SHARE
	uInt	lockid;			/* File lock ID (index of file semaphore table) */
#endif
#if !_FS_TINY
	byte	buf[_MAX_SS];	/* File data read/write buffer */
#endif
} FIL;

/* Directory object structure (DIR) */

typedef struct {
	FATFS*	fs;				/* Pointer to the owner file system object */
	word	id;				/* Owner file system mount ID */
	word	index;			/* Current read/write index number */
	DWord	sclust;			/* Table start cluster (0:Root dir) */
	DWord	clust;			/* Current cluster */
	DWord	sect;			/* Current sector */
	byte*	dir;			/* Pointer to the current SFN entry in the win[] */
	byte*	fn;				/* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
#if _USE_LFN
	WCHAR*	lfn;			/* Pointer to the LFN working buffer */
	word	lfn_idx;		/* Last matched LFN index number (0xFFFF:No LFN) */
#endif
} DIR;

/* File status structure (FILINFO) */

typedef struct {
	DWord	fsize;			/* File size */
	word	fdate;			/* Last modified date */
	word	ftime;			/* Last modified time */
	byte	fattrib;		/* Attribute */
	TCHAR	fname[13];		/* Short file name (8.3 format) */
#if _USE_LFN
	TCHAR*	lfname;			/* Pointer to the LFN buffer */
	uInt 	lfsize;			/* Size of LFN buffer in TCHAR */
#endif
} FILINFO;

/* File function return code (FRESULT) */

typedef enum {
	FR_OK = 0,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occured in the low level disk I/O layer */
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NOT_READY,			/* (3) The physical drive cannot work */
	FR_NO_FILE,				/* (4) Could not find the file */
	FR_NO_PATH,				/* (5) Could not find the path */
	FR_INVALID_NAME,		/* (6) The path name format is invalid */
	FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	FR_EXIST,				/* (8) Access denied due to prohibited access */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			/* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				/* (16) The operation is rejected according to the file shareing policy */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
	FR_INVALID_PARAMETER	/* (19) Given parameter is invalid */
} FRESULT;

/*--------------------------------------------------------------*/
/* FatFs module application interface                           */

FRESULT f_mount(byte, FATFS*);						/* Mount/Unmount a logical drive */
FRESULT f_open(FIL*, const TCHAR*, byte);			/* Open or create a file */
FRESULT f_read(FIL*, void*, uInt, uInt*);			/* Read data from a file */
FRESULT f_lseek(FIL*, DWord);						/* Move file pointer of a file object */
FRESULT f_close(FIL*);								/* Close an open file object */
FRESULT f_opendir(DIR*, const TCHAR*);				/* Open an existing directory */
FRESULT f_readdir(DIR*, FILINFO*);					/* Read a directory item */
FRESULT f_stat(const TCHAR*, FILINFO*);			/* Get file status */
FRESULT f_write(FIL*, const void*, uInt, uInt*);	/* Write data to a file */
FRESULT f_truncate(FIL*);							/* Truncate file */
FRESULT f_sync(FIL*);								/* Flush cached data of a writing file */
FRESULT f_unlink(const TCHAR*);					/* Delete an existing file or directory */
FRESULT	f_mkdir(const TCHAR*);						/* Create a new directory */
FRESULT f_rename(const TCHAR*, const TCHAR*);		/* Rename/Move a file or directory */
FRESULT f_chdir(const TCHAR*);						/* Change current directory */
FRESULT f_getcwd(TCHAR*, uInt);					/* Get current directory */

#define f_eof(fp) (((fp)->fptr == (fp)->fsize) ? 1 : 0)
#define f_error(fp) (((fp)->flag & FA__ERROR) ? 1 : 0)
#define f_tell(fp) ((fp)->fptr)
#define f_size(fp) ((fp)->fsize)

#ifndef EOF
#define EOF (-1)
#endif

/* File access control and file status flags (FIL.flag) */

#define	FA_READ				0x01
#define	FA_OPEN_EXISTING	0x00
#define FA__ERROR			0x80

#if !_FS_READONLY
#define	FA_WRITE			0x02
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define FA__WRITTEN			0x20
#define FA__DIRTY			0x40
#endif


/* FAT sub type (FATFS.fs_type) */

#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3


/* File attribute bits for directory entry */

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */
#define AM_MASK	0x3F	/* Mask of defined bits */


/* Fast seek feature */
#define CREATE_LINKMAP	0xFFFFFFFF



/*--------------------------------*/
/* Multi-byte word access macros  */

#if _WORD_ACCESS == 1	/* Enable word access to the FAT structure */
#define	LD_WORD(ptr)		(word)(*(word*)(byte*)(ptr))
#define	LD_DWORD(ptr)		(DWord)(*(DWord*)(byte*)(ptr))
#define	ST_WORD(ptr,val)	*(word*)(byte*)(ptr)=(word)(val)
#define	ST_DWORD(ptr,val)	*(DWord*)(byte*)(ptr)=(DWord)(val)
#else					/* Use byte-by-byte access to the FAT structure */
#define	LD_WORD(ptr)		(word)(((word)*((byte*)(ptr)+1)<<8)|(word)*(byte*)(ptr))
#define	LD_DWORD(ptr)		(DWord)(((DWord)*((byte*)(ptr)+3)<<24)|((DWord)*((byte*)(ptr)+2)<<16)|((DWord)*((byte*)(ptr)+1)<<8)|*(byte*)(ptr))
#define	ST_WORD(ptr,val)	*(byte*)(ptr)=(byte)(val); *((byte*)(ptr)+1)=(byte)((word)(val)>>8)
#define	ST_DWORD(ptr,val)	*(byte*)(ptr)=(byte)(val); *((byte*)(ptr)+1)=(byte)((word)(val)>>8); *((byte*)(ptr)+2)=(byte)((DWord)(val)>>16); *((byte*)(ptr)+3)=(byte)((DWord)(val)>>24)
#endif

#endif /* CX80_FF_H_ */