#include <reg51.h>

char Counter=0;
char NumbersCode[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x27,0x7f,0x6f};

void Delay(unsigned int MS)
	{
	unsigned int delay;
	for(delay=0;delay<MS;delay++);
	}



void externa0 (void) interrupt 0 using 0  
	{
	EX0=0;
	Delay(500);
	if(P3_2==0)
		{
		Counter++;
		if(Counter>9)
			Counter=0;
		P2 = ~NumbersCode[Counter];
		}
    EX0=1;
	}


main()
	{
	EX0=1;
	IT0=1;
	EA=1;
	P2 = ~NumbersCode[Counter];
	while(1)
		{

		}
	}
