#include <stdio.h>
#include <string.h>

#define byte						unsigned char

#define HEADER_0				0xAA
#define HEADER_1				0x1B

#define SEPARATOR				0xA3

#define FOOTER_0				0x3A
#define FOOTER_1				0xF0

#define assign					0x01
#define move						0x02
#define add							0x10
#define sub							0x11
#define mul							0x12
#define div							0x13
#define incr						0x20
#define decr						0x21
#define inv							0x22
#define tob							0x23
#define cmp							0x30
#define high						0x31
#define low							0x32
#define label						0x40
#define jump						0x41
#define cmpjump					0x42
#define	_ret						0x43
#define debug						0x50
#define debugstr				0x51
#define error						0x52
#define sint						0x60
#define _int						0x61
#define exit						0xFF

#define varid						0x04
#define value						0xF4

FILE* f;

byte itr[8];

int linen=1;

void beginsbi(FILE* fp)
{
	fputc(0xAA, fp);
	fputc(0x1B, fp);
}

void writeitr(FILE* fp)
{
	int i;
	for (i=0;i<8;i++) fputc(itr[i], fp);
}

void endsbi(FILE* fp)
{
	fputc(0x3A, fp);
	fputc(0xF0, fp);
}

void makeitr(byte com, byte p1t, byte p1, byte p2t, byte p2, byte p3t, byte p3)
{
	itr[0]=com;
	itr[1]=p1t;
	itr[2]=p1;
	itr[3]=p2t;
	itr[4]=p2;
	itr[5]=p3t;
	itr[6]=p3;
	itr[7]=0xBA;
}

int pline(char* command, char* p1s, char* p2s, char* p3s, int tokn)
{
	byte p1t=0, p1=0, p2t=0, p2=0, p3t=0, p3=0;
	if (tokn>1)
	{
		if ((p1s[0]=='_')&&(p1s[1]=='t'))
		{
			p1s++;
			p1s++;
			p1t=varid;
			p1=atoi(p1s);
		} else {
			p1t=value;
			p1=atoi(p1s);
		}
	}
	if (tokn>2)
	{
		if ((p2s[0]=='_')&&(p2s[1]=='t'))
		{
			p2s++;
			p2s++;
			p2t=varid;
			p2=atoi(p2s);
		} else {
			p2t=value;
			p2=atoi(p2s);
		}
	}
	if (tokn>3)
	{
		if ((p3s[0]=='_')&&(p3s[1]=='t'))
		{
			p3s++;
			p3s++;
			p3t=varid;
			p3=atoi(p3s);
		} else {
			p3t=value;
			p3=atoi(p3s);
		}
	}
	//printf("tokn: %i\np1: %i, p2: %i, p3: %i\n", tokn, p1, p2, p3);
	if (strcmp(command, "assign")==0)
	{
		if (tokn!=3) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(assign, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "move")==0)
	{
		if (tokn!=3) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(assign, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "move")==0)
	{
		if (tokn!=3) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(move, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "add")==0)
	{
		if (tokn!=4) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(add, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "sub")==0)
	{
		if (tokn!=4) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(sub, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "mul")==0)
	{
		if (tokn!=4) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(mul, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "div")==0)
	{
		if (tokn!=4) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(div, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "incr")==0)
	{
		if (tokn!=2) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(incr, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "decr")==0)
	{
		if (tokn!=2) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(decr, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "inv")==0)
	{
		if (tokn!=2) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(inv, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "tob")==0)
	{
		if (tokn!=2) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(tob, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "cmp")==0)
	{
		if (tokn!=4) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(cmp, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "high")==0)
	{
		if (tokn!=4) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(high, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "low")==0)
	{
		if (tokn!=4) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(low, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "label")==0)
	{
		if (tokn!=2) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(label, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "jump")==0)
	{
		if (tokn!=2) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(jump, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "cmpjump")==0)
	{
		if (tokn!=4) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(cmpjump, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "ret")==0)
	{
		if (tokn!=2) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(_ret, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "debug")==0)
	{
		if (tokn!=2) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(debug, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "error")==0)
	{
		if (tokn!=2) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(error, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "sint")==0)
	{
		if (tokn!=2) { printf("%i: Wrong number of parameters\n", linen); return 1; }
		makeitr(sint, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "int")==0)
	{
		makeitr(_int, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
	if (strcmp(command, "exit")==0)
	{
		makeitr(exit, p1t, p1, p2t, p2, p3t, p3);
		writeitr(f);
		return 0;
	}
}

void showhelp(void)
{
	
}

int main (int argc, char** argv)
{
	printf("SASM Compiler\n ver 0.1 by Gi@cky98\n\n");
	if ((argc!=2)&&(argc!=3))
	{
		printf("Usage: %s input.sasm [output.sbi]\n\n", (char*)argv[0]);
		printf("input.sasm\t\tSASM assembly file to compile\n", (char*)argv[0]);
		printf("output.sbi\t\tSBI output filename\n", (char*)argv[0]);
		return 1;
	}
  char* filename = (char*)argv[1];
  FILE *file = fopen ( filename, "r" );
  if (argc==3)
  	f = fopen ( argv[2], "wb" );
  else
  	f = fopen ( "out.sbi", "wb" );
  beginsbi(f);
  if ( file != NULL )
  {
  	printf("Assembling, please wait...\n");
  	char line [1024]; /* or other suitable maximum line size */
    while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */
    {
    	while (line[0]==' ')
    	{
				int a;
				for (a=1; a<1024; a++) line[a-1]=line[a];
				line[1023]='\0';
			}
			while (line[0]=='\t')
    	{
				int a;
				for (a=1; a<1024; a++) line[a-1]=line[a];
				line[1023]='\0';
			}
    	int tokn = 0;
      char* tok;
      char* com;
      char* p1;
      char* p2;
      char* p3;
      int c=0;
			tok=strtok(line, " ");
      while (tok!=NULL)
      {
      	switch (c)
      	{
					case 0:
						com=tok;
					case 1:
						p1=tok;
					case 2:
						p2=tok;
					case 3:
						p3=tok;
				}
				tokn++;
				c++;
				tok=strtok(NULL, " ");
			}
			int ret=pline(com, p1, p2, p3, tokn);
			if (ret>0) { fclose(f); fclose(file); return 1; }
			linen++;
  	}
  	endsbi(f);
  	fflush(f);
  	fclose(f);
    fclose ( file );
    printf("Done\n");
  }
  else
  {
  	perror ( filename ); /* why didn't the file open? */
  }
  return 0;
}
