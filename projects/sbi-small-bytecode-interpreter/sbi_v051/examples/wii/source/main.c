#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>

#include "sbi.h"

#include <grrlib.h>

FILE* f;

int pos;
byte lastslot=0;
byte buffer[32];

byte getfch(void)
{
	byte slot=((int)(pos/sizeof(buffer)));
	if (slot!=lastslot)
	{
		fseek (f, slot*sizeof(buffer), SEEK_SET);
		fread(&buffer, 1, sizeof(buffer), f);
	}
	lastslot=slot;
	return buffer[(pos++)-(lastslot*sizeof(buffer))];
}

void setfpos(int p)
{
	byte slot=((int)(p/sizeof(buffer)));
	if (slot!=lastslot)
	{
		fseek (f, slot*sizeof(buffer), SEEK_SET);
		fread(&buffer, 1, sizeof(buffer), f);
	}
	lastslot=slot;
	pos=p;
	return;
}

int getfpos(void)
{
	return pos;
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------
	GRRLIB_Init();
	
	// Initialise the filing system
	printf("Initializing FAT filesystem... ");
  if (!fatInitDefault()) { printf("Error!\n"); goto error; }
	printf("Ok!\n");
	
	f = fopen("/program.sbi", "rb");
	if (!f) { printf("Error opening file!\n"); goto error; }
	
	// Init
	_getfch=&getfch;
	_setfpos=&setfpos;
	_getfpos=&getfpos;
	fread(&buffer, 1, sizeof(buffer), f);
	_sbi_init(); pos = 0;
	
	int ret = _sbi_begin();
	if (ret==1) printf("Initialization error (no function pointers)\n");
	if (ret==2) printf("Initialization error (old format version)\n");
	if (ret==3) printf("Initialization error (invalid program file)\n");
	if (ret>3) printf("Initialization error (unknow: %i)\n", ret);
	
	if (ret>0) goto error;
	
	printf("Running...\n");
	
	while (ret==0)
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		ret = _sbi_run();
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
	}
	
	fclose(f);
	
	if (ret==1) printf("Program reached end (no exit found)\n");
	if (ret==2) printf("Program exited (no errors)\n");
	if (ret==3) printf("Program exited (wrong instruction code)\n");
	if (ret==4) printf("Program exited (can't understand byte)\n");
	if (ret==5) printf("Program exited (user error)\n");
	
	error:
	while(1) {

		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);

		// Wait for the next frame
		VIDEO_WaitVSync();
	}

	return 0;
}
