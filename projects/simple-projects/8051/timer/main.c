#include <REG51.h>

char Counter=0;
char NumbersCode[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x27,0x7f,0x6f};

void timer0 (void) interrupt 1 using 1  
	{
	Counter++;
	if(Counter>9)
		Counter=0;
	P2 = ~NumbersCode[Counter];
	}


main()
	{
	TMOD=1;
    ET0=1;
	TR0=1;
	EA=1;
	P2 = ~NumbersCode[Counter];
	while(1);
	}