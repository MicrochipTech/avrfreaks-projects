/*
 * CX80_Mmcbb.c
 *
 * Created: 20/03/2013 11:12:21
 *  Author: Calogiuri Enzo Antonio
 */ 

#define F_CPU					16000000UL

#include "CX80.h"
#include "CX80_DiskIO.h"

#define	INIT_PORT()			Setup_CX80_SD_Bus()	/* Initialize MMC control port (CS=H, CLK=L, DI=H, DO=in) */
#define DLY_US(n)			_delay_us(n)	/* Delay n microseconds */

#define	CS_H()				{SD_CS_High(); asm ("nop");}
#define CS_L()				{SD_CS_Low(); asm ("nop");}
#define CK_H()				{DATA_BUS_PORT |= 0x02; asm ("nop");}
#define	CK_L()				{DATA_BUS_PORT &= 0xFD; asm ("nop");}
#define DI_H()				{DATA_BUS_PORT |= 0x01; asm ("nop");}
#define DI_L()				{DATA_BUS_PORT &= 0xFE; asm ("nop");}
#define DO					(DATA_BUS_PORT_READ & 0x04)

/* MMC/SD command (SPI mode) */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define CMD13	(13)		/* SEND_STATUS */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD41	(41)		/* SEND_OP_COND (ACMD) */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

/* Card type flags (CardType) */
#define CT_MMC		0x01		/* MMC ver 3 */
#define CT_SD1		0x02		/* SD ver 1 */
#define CT_SD2		0x04		/* SD ver 2 */
#define CT_SDC		0x06		/* SD */
#define CT_BLOCK	0x08		/* Block addressing */

static DSTATUS Stat = STA_NOINIT;	/* Disk status */
static byte CardType;			/* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */

/*-----------------------------------------------------------------------*/
/* Transmit bytes to the card (bitbanging)                               */
/*-----------------------------------------------------------------------*/

static void xmit_mmc(const byte *buff, uInt bc)
{
	byte d;
	
	do {
		d = *buff++;	/* Get a byte to be sent */
		if (d & 0x80) {DI_H();} else {DI_L();}	/* bit7 */
		CK_H(); CK_L();
		if (d & 0x40) {DI_H();} else {DI_L();}	/* bit6 */
		CK_H(); CK_L();
		if (d & 0x20) {DI_H();} else {DI_L();}	/* bit5 */
		CK_H(); CK_L();
		if (d & 0x10) {DI_H();} else {DI_L();}	/* bit4 */
		CK_H(); CK_L();
		if (d & 0x08) {DI_H();} else {DI_L();}	/* bit3 */
		CK_H(); CK_L();
		if (d & 0x04) {DI_H();} else {DI_L();}	/* bit2 */
		CK_H(); CK_L();
		if (d & 0x02) {DI_H();} else {DI_L();}	/* bit1 */
		CK_H(); CK_L();
		if (d & 0x01) {DI_H();} else {DI_L();}	/* bit0 */
		CK_H(); CK_L();
	} while (--bc);
}

/*-----------------------------------------------------------------------*/
/* Receive bytes from the card (bitbanging)                              */
/*-----------------------------------------------------------------------*/

static void rcvr_mmc(byte *buff, uInt bc)
{
	byte r;
	
	DI_H();	/* Send 0xFF */
	
	do {
		r = 0; asm ("nop"); if (DO) r++;	/* bit7 */
		CK_H(); CK_L();
		r <<= 1; asm ("nop"); if (DO) r++;	/* bit6 */
		CK_H(); CK_L();
		r <<= 1; asm ("nop"); if (DO) r++;	/* bit5 */
		CK_H(); CK_L();
		r <<= 1; asm ("nop"); if (DO) r++;	/* bit4 */
		CK_H(); CK_L();
		r <<= 1; asm ("nop"); if (DO) r++;	/* bit3 */
		CK_H(); CK_L();
		r <<= 1; asm ("nop"); if (DO) r++;	/* bit2 */
		CK_H(); CK_L();
		r <<= 1; asm ("nop"); if (DO) r++;	/* bit1 */
		CK_H(); CK_L();
		r <<= 1; asm ("nop"); if (DO) r++;	/* bit0 */
		CK_H(); CK_L();
		*buff++ = r;			/* Store a received byte */
	} while (--bc);
}

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static byte wait_ready(void)
{
	byte d;
	uInt tmr;
	
	for (tmr = 5000; tmr; tmr--) {	/* Wait for ready in timeout of 500ms */
		rcvr_mmc(&d, 1);
		if (d == 0xFF) break;
		DLY_US(100);
	}

	return tmr ? 1 : 0;
}

/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static void deselect(void)
{
	byte d;
	
	INIT_PORT();
	
	CS_H();
	
	rcvr_mmc(&d, 1);	/* Dummy clock (force DO hi-z for multiple slave SPI) */
}

/*-----------------------------------------------------------------------*/
/* Select the card and wait for ready                                    */
/*-----------------------------------------------------------------------*/

static byte select(void)
{
	byte d;
	
	INIT_PORT();
	
	CS_L();
	
	rcvr_mmc(&d, 1);	/* Dummy clock (force DO enabled) */
	
	if (wait_ready())
		return 1;	/* OK */
		
	deselect();
	
	return 0;			/* Failed */
}

/*-----------------------------------------------------------------------*/
/* Receive a data packet from the card                                   */
/*-----------------------------------------------------------------------*/

static char rcvr_datablock(byte *buff, uInt btr)
{
	byte d[2];
	uInt tmr;
	
	for (tmr = 1000; tmr; tmr--) {	/* Wait for data packet in timeout of 100ms */
		rcvr_mmc(d, 1);
		if (d[0] != 0xFF) break;
		DLY_US(100);
	}
	if (d[0] != 0xFE) return 0;		/* If not valid data token, return with error */

	rcvr_mmc(buff, btr);			/* Receive the data block into buffer */
	rcvr_mmc(d, 2);					/* Discard CRC */

	return 1;						/* Return with success */
}

/*-----------------------------------------------------------------------*/
/* Send a data packet to the card                                        */
/*-----------------------------------------------------------------------*/

static char xmit_datablock(const byte *buff, byte token)
{
	byte d[2];
	
	if (!wait_ready()) return 0;

	d[0] = token;
	xmit_mmc(d, 1);				/* Xmit a token */
	if (token != 0xFD) {		/* Is it data token? */
		xmit_mmc(buff, 512);	/* Xmit the 512 byte data block to MMC */
		rcvr_mmc(d, 2);			/* Xmit dummy CRC (0xFF,0xFF) */
		rcvr_mmc(d, 1);			/* Receive data response */
		if ((d[0] & 0x1F) != 0x05)	/* If not accepted, return with error */
			return 0;
	}

	return 1;
}

/*-----------------------------------------------------------------------*/
/* Send a command packet to the card                                     */
/*-----------------------------------------------------------------------*/

static byte send_cmd(byte cmd, DWord arg)
{
	byte n, d, buf[6];
	
	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		n = send_cmd(CMD55, 0);
		if (n > 1) return n;
	}

	/* Select the card and wait for ready */
	deselect();
	if (!select()) return 0xFF;

	/* Send a command packet */
	buf[0] = 0x40 | cmd;			/* Start + Command index */
	buf[1] = (byte)(arg >> 24);		/* Argument[31..24] */
	buf[2] = (byte)(arg >> 16);		/* Argument[23..16] */
	buf[3] = (byte)(arg >> 8);		/* Argument[15..8] */
	buf[4] = (byte)arg;				/* Argument[7..0] */
	n = 0x01;						/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;		/* (valid CRC for CMD0(0)) */
	if (cmd == CMD8) n = 0x87;		/* (valid CRC for CMD8(0x1AA)) */
	buf[5] = n;
	xmit_mmc(buf, 6);

	/* Receive command response */
	if (cmd == CMD12) rcvr_mmc(&d, 1);	/* Skip a stuff byte when stop reading */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		rcvr_mmc(&d, 1);
	while ((d & 0x80) && --n);

	return d;			/* Return with the response value */
}

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

DSTATUS disk_status(void)
{
	DSTATUS s;
	byte d;
	
	if (select() == 0) return STA_NOINIT;
	
	Enable_CX80_Led(SDR_LED);

	/* Check if the card is kept initialized */
	s = Stat;
	if (!(s & STA_NOINIT)) {
		if (send_cmd(CMD13, 0))	/* Read card status */
			s = STA_NOINIT;
		rcvr_mmc(&d, 1);		/* Receive following half of R2 */
		deselect();
	}
	
	Disable_CX80_Led(SDR_LED);
	
	Stat = s;

	return s;
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(void)
{
	byte n, ty, cmd, buf[4];
	uInt tmr;
	DSTATUS s;	
	
	Enable_CX80_Led(SDR_LED);

	INIT_PORT();				/* Initialize control port */
	for (n = 10; n; n--) rcvr_mmc(buf, 1);	/* 80 dummy clocks */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Enter Idle state */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2? */
			rcvr_mmc(buf, 4);							/* Get trailing return value of R7 resp */
			if (buf[2] == 0x01 && buf[3] == 0xAA) {		/* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state (ACMD41 with HCS bit) */
					if (send_cmd(ACMD41, 1UL << 30) == 0) break;
					DLY_US(1000);
				}
				if (tmr && send_cmd(CMD58, 0) == 0) {	/* Check CCS bit in the OCR */
					rcvr_mmc(buf, 4);
					ty = (buf[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 */
				}
			}
		} else {							/* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state */
				if (send_cmd(cmd, 0) == 0) break;
				DLY_US(1000);
			}
			if (!tmr || send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
				ty = 0;
		}
	}
	CardType = ty;
	s = ty ? 0 : STA_NOINIT;
	Stat = s;

	deselect();
	
	Disable_CX80_Led(SDR_LED);

	return s;
}

DRESULT disk_read(byte *buff, DWord sector, byte count)
{
	if (disk_status() & STA_NOINIT)
		return RES_NOTRDY;
		
	if (!count)
		return RES_PARERR;
		
	Enable_CX80_Led(SDR_LED);
		
	if (!(CardType & CT_BLOCK))
		sector *= 512;	/* Convert LBA to byte address if needed */
		
	if (count == 1)		/* Single block read */
	{
		if ((send_cmd(CMD17, sector) == 0) && rcvr_datablock(buff, 512))
			count = 0;
	}
	else
	{
		if (send_cmd(CMD18, sector) == 0)	/* READ_MULTIPLE_BLOCK */
		{
			do 
			{
				if (!rcvr_datablock(buff, 512))
					break;
					
				buff += 512;
			} while (--count);
			
			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	
	deselect();
	
	Disable_CX80_Led(SDR_LED);
	
	return count ? RES_ERROR : RES_OK;
}

DRESULT disk_write(const byte *buff, DWord sector, byte count)
{
	if (disk_status() & STA_NOINIT)
		return RES_NOTRDY;
		
	if (!count)
		return RES_PARERR;
		
	Enable_CX80_Led(SDW_LED);
		
	if (!(CardType & CT_BLOCK))
		sector *= 512;	/* Convert LBA to byte address if needed */
		
	if (count == 1)
	{
		if ((send_cmd(CMD24, sector) == 0) && xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else
	{
		if (CardType & CT_SDC)
			send_cmd(ACMD23, count);
			
		if (send_cmd(CMD25, sector) == 0)
		{
			do 
			{
				if (!xmit_datablock(buff, 0xFC))
					break;
					
				buff += 512;
			} while (--count);
			
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	
	deselect();
	
	Disable_CX80_Led(SDW_LED);
	
	return count ? RES_ERROR : RES_OK;
}

DRESULT disk_ioctl(byte ctrl, void *buff)
{
	DRESULT res;
	byte n, csd[16];
	word cs;
	
	if (disk_status() & STA_NOINIT)
		return RES_NOTRDY;	/* Check if card is in the socket */ 
		
	res = RES_ERROR;
	
	switch (ctrl)
	{
		case CTRL_SYNC :		/* Make sure that no pending write process */
			if (select())
			{
				deselect();
				
				res = RES_OK;
			}
			break;
			
		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
			Enable_CX80_Led(SDR_LED);
		
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16))
			{
				if ((csd[0] >> 6) == 1)
				{
					cs = csd[9] + ((word)csd[8] << 8) + 1;
					*(DWord*)buff = (DWord)cs << 10;
				}
				else
				{
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					cs = (csd[8] >> 6) + ((word)csd[7] << 2) + ((word)(csd[6] & 3) << 10) + 1;
					*(DWord*)buff = (DWord)cs << (n - 9);
				}
				
				res = RES_OK;
			}
			
			Disable_CX80_Led(SDR_LED);
			break;
			
		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
			*(DWord*)buff = 128;
			res = RES_OK;
			break;
			
		default:
			res = RES_PARERR;
	}
	
	deselect();
	
	return res;
}