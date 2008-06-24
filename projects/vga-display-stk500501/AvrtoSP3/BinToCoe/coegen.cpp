//////////////////////////////////////////////////
// xilinx coe file generator for fpgas....
//
// created Dec 5, 2007
// Jim Patchell
//////////////////////////////////////////////////

#include "stdio.h"
#include "stdlib.h"

void main(int argc, char *argv[])
{
	FILE *in;

	if((in = fopen(argv[1],"rb")) == NULL)
	{
		fprintf(stderr,"Could not open %s for input\n");
		exit(1);
	}

	int i;
	char *d = new char[32768];
	int loop = 1;
	int n;

	printf("memory_initialization_radix=16;\n");

	printf("memory_initialization_vector=\n");


	while(loop)
	{
		n = fread(d,1,32768,in);
		if(n < 32768)
			loop = 0;
		for(i=0;i<n;++i)
			printf("%02x%c\n",d[i] & 0x0ff,((loop == 0) && (i == (n-1)))?';':',');
	}
	fclose(in);
	delete[] d;
}