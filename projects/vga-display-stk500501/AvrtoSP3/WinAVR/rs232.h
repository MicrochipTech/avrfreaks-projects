/*******************************************************************
** Device Driver for UARTS on ATmega640/1280/1281/2561
**
** Craeted July 10, 2007
** by Jim Patchell
**
** Written for WinAVR
**
********************************************************************/

#ifndef RS232__H
#define RS232__H

#include "task.h"

#define N_PORTS		2

typedef struct {
	unsigned char *pUDR;	//uart data register
	unsigned char *pUCSRnA;	//control and status register
	unsigned char *pUCSRnB;	//control and status register
	unsigned char *pUCSRnC;	//control and status register
	unsigned short *pUBRR;	//baud rate generator (16 bits)
}RREG;

typedef struct {
	unsigned short head;
	unsigned short tail;
	unsigned short nChar;
	unsigned short size;
	ECB *pSem;
	char *buff;
	RREG *regs;
}IOREC;

typedef struct {
	IOREC Rx;
	IOREC Tx;
	RREG *regs;
} SERIAL;

extern int RGetC(SERIAL *pI);
extern void RPutC(SERIAL *pI,int c);
extern void RWrite(SERIAL *pI,char *b,int l);
extern int RStatus(SERIAL *pI);
extern int REmpty(SERIAL *pI);
extern SERIAL *RInit(char *pTb,unsigned short TSize,char *pRb,unsigned short nRSize,unsigned short nPort,long nBaudRate);

#endif
