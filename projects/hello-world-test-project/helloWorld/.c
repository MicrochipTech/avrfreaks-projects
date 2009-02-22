#include <stdio.h>
#include <stdlib.h>

char	buffer[128];

int	main(int argc, char *argv[])
{
	int	cnt;
	cnt=0;
	while(cnt < 10){
		sprintf(buffer,"Hello world %d\n",cnt);
		cnt++;
	}
	exit(0);
}
