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

byte rd[1];
WORD rn;
byte buffer[32];
byte lastslot=0;

// Normal functions
/*byte getfch(void)
{
	WORD rn;
	pf_read(rd, 1, &rn);
	return rd[0];
}
void setfpos(int p)
{
	pf_lseek(_cursor);
	return;
}
*/

// Buffered functions (faster)
byte getfch(void)
{
	byte slot=((int)(_cursor/sizeof(buffer)));
	if (slot!=lastslot)
	{
		pf_lseek(slot*sizeof(buffer));
		pf_read(buffer, sizeof(buffer), &rn);
	}
	lastslot=slot;
	return buffer[_cursor-(lastslot*sizeof(buffer))];
}


void setfpos(int p)
{
	byte slot=((int)(p/sizeof(buffer)));
	if (slot!=lastslot)
	{
		pf_lseek(slot*sizeof(buffer));
		pf_read(buffer, sizeof(buffer), &rn);
	}
	lastslot=slot;
	return;
}

int main (void)
{
	BYTE res;
	FATFS fs;	/* File system object */
	
	DDRA=0xFF;	// All port as output
	
	byte ret;
	
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
	
	// Init
	_getfch=&getfch;
	_setfpos=&setfpos;
	
	pf_read(buffer, sizeof(buffer), &rn); // Init buffer
	
	_sbi_begin();
	
	SPrint("Running...\n");
	
	ret = _sbi_run();
	
	if (ret==0) SPrint("Program reached end (no exit found)\n");
	if (ret==1) SPrint("Program exited (no errors)\n");
	if (ret==2) SPrint("Program exited (program error)\n");
	if (ret==3) SPrint("Program exited (user error)\n");
	if (ret==4) SPrint("Initialization error (no function pointers)\n");
	if (ret==5) SPrint("Can't execute program (old format version)\n");
	
	if (ret<=1) return 0; else return 1;
}
