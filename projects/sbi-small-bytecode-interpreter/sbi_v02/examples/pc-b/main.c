#include <stdio.h>
#include "sbi.h"

FILE* f;
byte lastslot=0;
byte buffer[32];

byte getfch(void)
{
	byte slot=((int)(_cursor/sizeof(buffer)));
	byte i;
	if (slot!=lastslot)
	{
		fseek (f, slot*sizeof(buffer), SEEK_SET);
		for (i=0; i<sizeof(buffer); i++) buffer[i]=fgetc(f);
	}
	lastslot=slot;
	return buffer[_cursor-(lastslot*sizeof(buffer))];
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
	return;
}

int main(int argc, char** argv)
{
	printf("\nSBI Runtime for Windows PC\n\n");
	
	// Open executable
	if (argc!=2) { printf("Wrong arguments\n\nUse:\n %s <program.sbi>\n", argv[0]); return 8; }
	printf("Loading %s...\n", argv[1]);
	f = fopen((char*)argv[1], "rb");
	
	if (!f) { printf("Can't open file!\n"); return 1; }
	
	// Init
	_getfch=&getfch;
	_setfpos=&setfpos;
	byte i;
	for (i=0; i<sizeof(buffer); i++) buffer[i]=fgetc(f);
	_sbi_begin();
	
	int ret;
	
	printf("Running...\n");
	
	ret = _sbi_run();
	
	fclose(f);
	
	if (ret==0) printf("Program reached end (no exit found)\n");
	if (ret==1) printf("Program exited (no errors)\n");
	if (ret==2) printf("Program exited (program error)\n");
	if (ret==3) printf("Program exited (user error)\n");
	if (ret==4) printf("Initialization error (no function pointers)\n");
	if (ret==5) printf("Can't execute program (old format version)\n");
	
	if (ret<=1) return 0; else return 1;
}