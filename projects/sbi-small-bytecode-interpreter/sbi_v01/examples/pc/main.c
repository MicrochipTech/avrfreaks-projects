#include <stdio.h>
#include "sbi.h"

byte itr[8];

int main(int argc, char** argv)
{
	printf("\nSBI Runtime for Windows PC\n\n");
	
	// Vars
	int rd;
	int i;
	
	// Open executable
	if (argc!=2) { printf("Wrong arguments\n\nUse:\n %s <program.sbi>\n", argv[0]); return 8; }
	printf("Loading %s...\n", argv[1]);
	FILE* f = fopen((char*)argv[1], "rb");
	
	if (!f) { printf("Can't open file!\n"); return 1; }
	
	// Init
	_cursor=0;
	
	// Read head
	rd = fgetc(f); _cursor++;
	if (rd!=0xAA) { printf("Error - expect 0xAA on first byte\n"); return 1; }
	rd = fgetc(f); _cursor++;
	if (rd!=0x1B) { printf("Error - expect 0x1B on second byte\n"); return 1; }
	
	_inituserfunc();
	
	int ret;
	
	printf("Scanning code for labels...\n");
	_cursor=2;
	fseek ( f , _cursor , SEEK_SET );
	while (1)
	{
		fseek ( f , _cursor , SEEK_SET );
		for (i=0; i<8; i++) itr[i]=fgetc(f);
		ret = _scanbts(itr);
		for (i=0; i<8; i++) _cursor++;
		if (ret>1) break;
	}
	if (ret>2) { printf("Wrong instruction on position 0x%02X\n", _cursor); return 1; }
	
	printf("Running code...\n");
	_cursor=2;
	fseek ( f , _cursor , SEEK_SET );
	while (1)
	{
		//printf("DEBUG :: Cursor at %i\n", _cursor);
		fseek ( f , _cursor , SEEK_SET );
		for (i=0; i<8; i++) itr[i]=fgetc(f);
		ret = _intbts(itr);
		//printf("DEBUG :: Ret = %i\n", ret);
		for (i=0; i<8; i++) _cursor++;
		if (ret>0) break;
	}
	
	fclose(f);
	
	if (ret==1) printf("Program exited (no errors)\n");
	if (ret==2) printf("Program exited (program error)\n");
	if (ret==3) printf("Program exited (user error)\n");
	
	if (ret==1) return 0; else return 1;
}