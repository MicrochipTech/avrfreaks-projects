/*
 * CX80_DiskIO.h
 *
 * Created: 20/03/2013 11:09:34
 *  Author: Calogiuri Enzo Antonio
 */ 


#ifndef CX80_DISKIO_H_
#define CX80_DISKIO_H_

#include "CX80.h"

/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */

/* Generic command (mandatory for FatFs) */
#define CTRL_SYNC			0	/* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT	1	/* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE		2	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (for only f_mkfs()) */

typedef byte DSTATUS;

typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;

//int assign_drives (int, int);
DSTATUS disk_initialize (void);
DSTATUS disk_status (void);
DRESULT disk_read (byte*, DWord, byte);
DRESULT disk_write (const byte*, DWord, byte);
DRESULT disk_ioctl (byte, void*);

#endif /* CX80_DISKIO_H_ */