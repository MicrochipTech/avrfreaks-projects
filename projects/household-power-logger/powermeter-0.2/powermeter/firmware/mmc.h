/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file  R0.05   (C)ChaN, 2007
/-----------------------------------------------------------------------*/
// modified by Dan Stahlke

#ifndef MMC_H
#define MMC_H

#include <stdint.h>
#include "config.h"

#if DEBUG_MMC_INIT
extern uint8_t show_mmc_debug_init;
#endif

#define _USE_IOCTL	1

extern uint8_t mmc_buf[MMC_BUF_LEN];

typedef enum { FALSE = 0, TRUE } BOOL;

/* Status of Disk Functions */
typedef uint8_t	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize ();
DRESULT disk_read (uint32_t);
DRESULT disk_write (uint32_t);
DRESULT disk_ioctl (uint8_t, void*);
void	disk_timerproc (void);




/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */


/* Command code for disk_ioctrl() */

/* Generic command */
#define CTRL_SYNC			0	/* Mandatory for write functions */
#define GET_SECTOR_COUNT	1	/* Mandatory for only f_mkfs() */
#define GET_SECTOR_SIZE		2
#define GET_BLOCK_SIZE		3	/* Mandatory for only f_mkfs() */
#define CTRL_POWER			4
#define CTRL_LOCK			5
#define CTRL_EJECT			6
/* MMC/SDC command */
#define MMC_GET_TYPE		10
#define MMC_GET_CSD			11
#define MMC_GET_CID			12
#define MMC_GET_OCR			13
#define MMC_GET_SDSTAT		14
/* ATA/CF command */
#define ATA_GET_REV			20
#define ATA_GET_MODEL		21
#define ATA_GET_SN			22


#endif
