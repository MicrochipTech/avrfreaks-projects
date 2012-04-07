/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * main.c - Basic ELF template
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 *
 * $Id: main.c 1888 2006-05-01 08:47:04Z tyranid $
 * $HeadURL$
 */
#include <pspkernel.h>
#include <pspdebug.h>

#include <pspdisplay.h>

#include <stdio.h>
#include <stdlib.h>

#include "sbi.h"

#define printf pspDebugScreenPrintf

/* Define the module info section */
PSP_MODULE_INFO("sbiruntime", 0, 1, 1);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();

	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, THREAD_ATTR_USER, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

FILE* f;
int pos;
byte lastslot=0;
byte buffer[32];

byte getfch(void)
{
	byte slot=((int)(pos/sizeof(buffer)));
	byte i;
	if (slot!=lastslot)
	{
		fseek (f, slot*sizeof(buffer), SEEK_SET);
		for (i=0; i<sizeof(buffer); i++) buffer[i]=fgetc(f);
	}
	lastslot=slot;
	return buffer[(pos++)-(lastslot*sizeof(buffer))];
}

void setfpos(int p)
{
	byte slot=((int)(p/sizeof(buffer)));
	byte i;
	if (slot!=lastslot)
	{
		fseek (f, slot*sizeof(buffer), SEEK_SET);
		for (i=0; i<sizeof(buffer); i++) buffer[i]=fgetc(f);
	}
	lastslot=slot;
	pos=p;
	return;
}

int getfpos(void)
{
	return pos;
}

void freeze(void)
{
	fclose(f);
	while (1)
	{
		sceDisplayWaitVblankStart();
	}
}

int main(int argc, char *argv[])
{
	pspDebugScreenInit();
	SetupCallbacks();
	
	printf("\nSBI Runtime for Windows PSP\n\n");
	
	// Open executable
	printf("Loading program.sbi...\n");
	f = fopen("program.sbi", "rb");
	
	if (!f) { printf("Can't open file!\n"); fclose(f); freeze(); }
	
	// Init
	_getfch=&getfch;
	_setfpos=&setfpos;
	_getfpos=&getfpos;
	byte i;
	for (i=0; i<sizeof(buffer); i++) buffer[i]=fgetc(f);
	_sbi_init(); pos = 0;
	
	int ret = _sbi_begin();
	if (ret==1) printf("Initialization error (no function pointers)\n");
	if (ret==2) printf("Initialization error (old format version)\n");
	if (ret==3) printf("Initialization error (invalid program file)\n");
	if (ret>3) printf("Initialization error (unknow: %i)\n", ret);
	
	if (ret>0) freeze();
	
	printf("Running...\n");
	
	while (ret==0)
	{
		ret = _sbi_run();
	}
	
	fclose(f);
	
	if (ret==1) printf("Program reached end (no exit found)\n");
	if (ret==2) printf("Program exited (no errors)\n");
	if (ret==3) printf("Program exited (wrong instruction code)\n");
	if (ret==4) printf("Program exited (can't understand byte)\n");
	if (ret==5) printf("Program exited (user error)\n");
	
	freeze();

	return 0;
}
