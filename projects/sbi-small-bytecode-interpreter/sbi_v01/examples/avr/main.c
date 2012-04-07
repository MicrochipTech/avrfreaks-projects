/*---------------------------------------------------------------*/
/* Petit FAT file system module test program R0.02 (C)ChaN, 2009 */
/*---------------------------------------------------------------*/

#define F_CPU 16000000

#include <string.h>
#include <avr/io.h>
#include "diskio.h"
#include "pff.h"
#include "sbi.h"
#include "uart.h"

byte itr[8];

/*---------------------------------------------------------*/
/* Work Area                                               */
/*---------------------------------------------------------*/

static
void put_rc (FRESULT rc)
{
	SPrint("rc=");
	SPrintN((WORD)rc);
	SPrint("\n");
}



static
void put_drc (BYTE res)
{
	SPrint("rc=");
	SPrintN(res);
	SPrint("\n");
}


/*-----------------------------------------------------------------------*/
/* Main                                                                  */


int main (void)
{
	BYTE res;
	WORD s1;
	FATFS fs;			/* File system object */
	
	DDRA=0xFF; // All port as output
	
	byte ret;
	char rd[1];
	
	_inituserfunc();
	
	InitUART(19200);
	
	SPrint("\nSBI on AVR (using Petit FatFs)\n");
	
	SPrint("Initializing disk... \n");
	res = disk_initialize();
	put_drc(res);
	if (res>0) {SPrint("SD/FS Error - exit\n"); return 1;}
	
	SPrint("Mounting volume... \n");
	res = pf_mount(&fs);
	put_rc(res);
	if (res>0) {SPrint("SD/FS Error - exit\n"); return 1;}
	
	SPrint("Opening \"program.sbi\"... \n");
	res = pf_open("PROGRAM.SBI");
	put_rc(res);
	if (res>0) {SPrint("SD/FS Error - exit\n"); return 1;}
	
	SPrint("Checking head... \n");
	// Read head
	pf_read(rd, 1, &s1); _cursor++;
	if (rd[0]!=0xAA) { SPrint("Error - expect 0xAA on first byte\n"); return 1; }
	pf_read(rd, 1, &s1); _cursor++;
	if (rd[0]!=0x1B) { SPrint("Error - expect 0x1B on second byte\n"); return 1; }
	
	SPrint("Scanning program... \n");
	if (res>0) {put_rc(res); SPrint("SD/FS Error - exit\n"); return 1;}
	_cursor=2;
	res = pf_lseek(_cursor);
	while (1)
	{
		pf_lseek(_cursor);
		pf_read(itr, 8, &s1);
		ret = _scanbts(itr);
		_cursor+=8;
		if (ret>1) break;
	}
	if (ret>2) { SPrint("Wrong instruction on position "); SPrintN(_cursor); SPrint("\n"); return 1; }
	
	SPrint("Executing it... \n");
	_cursor=2;
	res = pf_lseek(_cursor);
	while (1)
	{
		pf_lseek(_cursor);
		pf_read(itr, 8, &s1);
		ret = _intbts(itr);
		_cursor+=8;
		if (ret>0) break;
	}
	
	if (ret==1) SPrint("Program exited (no errors)\n");
	if (ret==2) SPrint("Program exited (program error)\n");
	if (ret==3) SPrint("Program exited (user error)\n");
	
	if (ret==1) return 0; else return 1;
}
