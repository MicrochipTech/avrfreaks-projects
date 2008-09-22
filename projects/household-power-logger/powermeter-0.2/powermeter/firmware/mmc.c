/*-----------------------------------------------------------------------*/
/* MMC/SDSC/SDHC (in SPI mode) control module  (C)ChaN, 2007             */
/*-----------------------------------------------------------------------*/
/* Only rcvr_spi(), xmit_spi(), disk_timerproc() and some macros         */
/* are platform dependent.                                               */
/*-----------------------------------------------------------------------*/

/*
 * Modified by Dan Stahlke for use in low-memory AVR chips
 */


#include <avr/io.h>

#include "config.h"
#include "mmc.h"
#include "io_stuff.h"

#define DEBUG_SPI 0

uint8_t mmc_buf[MMC_BUF_LEN];

#if DEBUG_SPI || DEBUG_MMC_INIT
#include <avr/pgmspace.h>
#include "serial.h"
#endif
#if DEBUG_MMC_INIT
uint8_t show_mmc_debug_init;
#endif

/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define	ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD13	(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD23	(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */


/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */

static volatile
uint8_t Timer1, Timer2;	/* 100Hz decrement timer */

static
uint8_t CardType;			/* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */



/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/*-----------------------------------------------------------------------*/


static void xmit_spi(uint8_t val) {
	#if DEBUG_SPI
	printP("s[");
	print_hex8(val);
	printP("]\n");
	#endif
	SPDR=(val); 
	while(!(SPSR & (1<<SPIF)));
}



/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */
/*-----------------------------------------------------------------------*/

static
uint8_t rcvr_spi (void)
{
	SPDR = 0xFF;
	while(!(SPSR & (1<<SPIF)));
	uint8_t val = SPDR;
	#if DEBUG_SPI
	printP("r[");
	print_hex8(val);
	printP("]\n");
	#endif
	return val;
}



/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
uint8_t wait_ready (void)
{
	uint8_t res;


	Timer2 = 50;	/* Wait for ready in timeout of 500ms */
	rcvr_spi();
	do {
		res = rcvr_spi();
	} while ((res != 0xFF) && Timer2);

	return res;
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static BOOL rcvr_datablock (
	uint8_t *buff,			/* Data buffer to store received data */
	uint16_t nread,			/* Byte count */
	uint16_t ndiscard		/* Byte count */
) {
	uint8_t token;


	Timer1 = 10;
	do {							/* Wait for data packet in timeout of 100ms */
		token = rcvr_spi();
	} while ((token == 0xFF) && Timer1);
	if(token != 0xFE) return FALSE;	/* If not valid data token, return with error */

	while(nread--) {
		*(buff++) = rcvr_spi();
	}
	while(ndiscard--) {
		rcvr_spi();
	}

	rcvr_spi();						/* Discard CRC */
	rcvr_spi();

	return TRUE;					/* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

static BOOL xmit_datablock (
	uint8_t token			/* Data/Stop token */
) {
	if (wait_ready() != 0xFF) return FALSE;

	xmit_spi(token);					/* Xmit data token */
	if (token != 0xFD) {	/* Is data token */
		const uint8_t *buff = mmc_buf;

		uint16_t nread = MMC_BUF_LEN;
		while(nread--) {
			xmit_spi(*(buff++));
		};
		nread = 512 - MMC_BUF_LEN;
		while(nread--) {
			xmit_spi(0);
		};

		xmit_spi(0xFF);					/* CRC (Dummy) */
		xmit_spi(0xFF);
		uint8_t resp = rcvr_spi();				/* Reveive data response */
		if ((resp & 0x1F) != 0x05)		/* If not accepted, return with error */
			return FALSE;
	}

	return TRUE;
}



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static uint8_t send_cmd (
	uint8_t cmd,		/* Command byte */
	uint32_t arg		/* Argument */
) {
	uint8_t n, res;

	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card and wait for ready */
	deselect_mmc();
	select_mmc();
	if (wait_ready() != 0xFF) return 0xFF;

	/* Send command packet */
	xmit_spi(cmd);						/* Start + Command index */
	xmit_spi((uint8_t)(arg >> 24));		/* Argument[31..24] */
	xmit_spi((uint8_t)(arg >> 16));		/* Argument[23..16] */
	xmit_spi((uint8_t)(arg >> 8));			/* Argument[15..8] */
	xmit_spi((uint8_t)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive command response */
	if (cmd == CMD12) rcvr_spi();		/* Skip a stuff byte when stop reading */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do {
		res = rcvr_spi();
	} while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize () {
	uint8_t n, cmd, ty, ocr[4];

	#if DEBUG_MMC_INIT
	char dbg[20];
	char *dbg_p = dbg;
	*(dbg_p++) = '1';
	#endif

	enable_spi_mmc();
	for (n = 10; n; n--) rcvr_spi();	/* 80 dummy clocks */

	#if DEBUG_MMC_INIT
	*(dbg_p++) = '2';
	#endif

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Enter Idle state */
		#if DEBUG_MMC_INIT
		*(dbg_p++) = '3';
		#endif

		Timer1 = 100;						/* Initialization timeout of 1000 msec */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDHC */
			#if DEBUG_MMC_INIT
			*(dbg_p++) = '4';
			#endif
			for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();		/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* The card can work at vdd range of 2.7-3.6V */
				while (Timer1 && send_cmd(ACMD41, 1UL << 30)) {	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				}
				if (Timer1 && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
					ty = (ocr[0] & 0x40) ? 12 : 4;
				}
			}
		} else {							/* SDSC or MMC */
			#if DEBUG_MMC_INIT
			*(dbg_p++) = '5';
			#endif
			if (send_cmd(ACMD41, 0) <= 1) 	{
				#if DEBUG_MMC_INIT
				*(dbg_p++) = '6';
				#endif
				ty = 2; cmd = ACMD41;	/* SDSC */
			} else {
				#if DEBUG_MMC_INIT
				*(dbg_p++) = '7';
				#endif
				ty = 1; cmd = CMD1;		/* MMC */
			}
			#if DEBUG_MMC_INIT
			*(dbg_p++) = '8';
			#endif
			while (Timer1 && send_cmd(cmd, 0));			/* Wait for leaving idle state */
			#if DEBUG_MMC_INIT
			*(dbg_p++) = '9';
			#endif
			if (!Timer1 || send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
				ty = 0;
		}
	}

	#if DEBUG_MMC_INIT
	*(dbg_p++) = 'A';
	#endif

	CardType = ty;
	release_spi_mmc();

	#if DEBUG_MMC_INIT
	*(dbg_p++) = 0;
	if(show_mmc_debug_init) {
		printP("DBG");
		printstr(dbg);
		putchr('\n');
	}
	#endif

	if (ty) {			/* Initialization succeded */
		Stat &= ~STA_NOINIT;		/* Clear STA_NOINIT */
	} else {			/* Initialization failed */
		Stat |= STA_NOINIT;
	}

	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(uint32_t sector) {
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	enable_spi_mmc();

	if (!(CardType & 8)) sector *= 512;	/* Convert to byte address if needed */

	DRESULT res = RES_ERROR;
	if((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
		&& rcvr_datablock(mmc_buf, MMC_BUF_LEN, 512-MMC_BUF_LEN)) res = RES_OK;

	release_spi_mmc();

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write(uint32_t sector) {
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	enable_spi_mmc();

	if (!(CardType & 8)) sector *= 512;	/* Convert to byte address if needed */

	DRESULT res = RES_ERROR;
	if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
		&& xmit_datablock(0xFE)) res = RES_OK;
	
	release_spi_mmc();

	return res;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL != 0
DRESULT disk_ioctl (
	uint8_t ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	uint8_t n, csd[16], *ptr = buff;
	uint32_t csize;

	enable_spi_mmc();

	res = RES_ERROR;

	//if (ctrl == CTRL_POWER) {
	//} else {
		if (Stat & STA_NOINIT) return RES_NOTRDY;

		switch (ctrl) {
		case CTRL_SYNC :		/* Make sure that no pending write process */
			select_mmc();
			if (wait_ready() == 0xFF)
				res = RES_OK;
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (uint32_t) */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16, 0)) {
				if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
					csize = csd[9] + ((uint32_t)csd[8] << 8) + 1;
					*(uint32_t*)buff = (uint32_t)csize << 10;
				} else {					/* SDC ver 1.XX or MMC*/
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					csize = (csd[8] >> 6) + ((uint32_t)csd[7] << 2) + ((uint32_t)(csd[6] & 3) << 10) + 1;
					*(uint32_t*)buff = (uint32_t)csize << (n - 9);
				}
				res = RES_OK;
			}
			break;

		case GET_SECTOR_SIZE :	/* Get R/W sector size (uint32_t) */
			*(uint32_t*)buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (uint32_t) */
			if (CardType & 4) {			/* SDC ver 2.00 */
				if (send_cmd(ACMD13, 0) == 0) {		/* Read SD status */
					rcvr_spi();
					if (rcvr_datablock(csd, 16, 0)) {				/* Read partial block */
						for (n = 64 - 16; n; n--) rcvr_spi();	/* Purge trailing data */
						*(uint32_t*)buff = 16UL << (csd[10] >> 4);
						res = RES_OK;
					}
				}
			} else {					/* SDC ver 1.XX or MMC */
				if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16, 0)) {	/* Read CSD */
					if (CardType & 2) {			/* SDC ver 1.XX */
						*(uint32_t*)buff = (((csd[10] & 63) << 1) + ((uint32_t)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
					} else {					/* MMC */
						*(uint32_t*)buff = ((uint32_t)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
					}
					res = RES_OK;
				}
			}
			break;

		case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
			*ptr = CardType;
			res = RES_OK;
			break;

		case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
			if (send_cmd(CMD9, 0) == 0		/* READ_CSD */
				&& rcvr_datablock(ptr, 16, 0))
				res = RES_OK;
			break;

		case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
			if (send_cmd(CMD10, 0) == 0		/* READ_CID */
				&& rcvr_datablock(ptr, 16, 0))
				res = RES_OK;
			break;

		case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
			if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
				for (n = 4; n; n--) *ptr++ = rcvr_spi();
				res = RES_OK;
			}
			break;

		case MMC_GET_SDSTAT :	/* Receive SD statsu as a data block (64 bytes) */
			if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
				rcvr_spi();
				if (rcvr_datablock(ptr, 64, 0))
					res = RES_OK;
			}
			break;

		default:
			res = RES_PARERR;
		}

		release_spi_mmc();
	//}

	return res;
}
#endif /* _USE_IOCTL != 0 */


/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure  (Platform dependent)                */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */

void disk_timerproc (void) {
	uint8_t n;

	n = Timer1;						/* 100Hz decrement timer */
	if(n) Timer1 = --n;
	n = Timer2;
	if(n) Timer2 = --n;
}
