/*---------------------------------------------------------------*/
/* Petit FAT file system module test program R0.02 (C)ChaN, 2009 */
/*---------------------------------------------------------------*/

#define F_CPU 16000000

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "diskio.h"
#include "pff.h"
#include "sbi.h"
#include "uart.h"

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
byte buffer[16];
byte lastslot=0;

// Normal functions [SUPER SLOW]
/*int pos;

byte getfch(void)
{
	pf_read(rd, 1, &rn);
	pos++;
	return rd[0];
}
void setfpos(int p)
{
	pf_lseek(p);
	pos=p;
	return;
}

int getfpos(void)
{
	return pos;
}*/

// Buffered functions (faster)
int pos;

void initbuffered(void)
{
	pf_read(buffer, sizeof(buffer), &rn); // Init buffer
}

byte getfch(void)
{
	byte slot=((int)(pos/sizeof(buffer)));
	if (slot!=lastslot)
	{
		pf_lseek(slot*sizeof(buffer));
		pf_read(buffer, sizeof(buffer), &rn);
	}
	lastslot=slot;
	return buffer[(pos++)-(lastslot*sizeof(buffer))];
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
	pos=p;
	return;
}

int getfpos(void)
{
	return pos;
}

ISR(SIG_INTERRUPT0)
{
	_interrupt(0);
}

int main (void)
{
	BYTE res;
	FATFS fs;	/* File system object */
	
	DDRA=0x0F;	// Half port as input and half as output
	PORTA=0xF0;	// Pull-up
	
	byte ret;
	
	InitUART(19200);
	
	SPrint("\nSBI 0.51 on AVR (using Petit FatFs)\n");
	
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
	_getfpos=&getfpos;
	
	pos = 0; // Init program counter
	
	initbuffered();
	
	_sbi_init();
	
	SPrint("Loading program...\n");
	
	ret = _sbi_begin();
	if (ret==1) SPrint("Initialization error (no function pointers)\n");
	if (ret==2) SPrint("Initialization error (old format version)\n");
	if (ret==3) SPrint("Initialization error (invalid program file)\n");
	if (ret>3) SPrint("Initialization error (unknow)\n");
	
	if (ret>0) return 1;
	
	// Enable INT0
	MCUCR |= (1<<ISC01);
	GICR |= (1<<INT0);
	sei();
	
	SPrint("Running...\n");
	
	while (ret==0)
	{
		ret = _sbi_run();
	}
	
	if (ret==1) SPrint("Program reached end (no exit found)\n");
	if (ret==2) SPrint("Program exited (no errors)\n");
	if (ret==3) SPrint("Program exited (wrong instruction code)\n");
	if (ret==4) SPrint("Program exited (can't understand byte)\n");
	if (ret==5) SPrint("Program exited (user error)\n");
	
	if (ret<2) return 0; else return 1;
}
