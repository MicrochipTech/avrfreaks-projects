#include<REG51.h>
void Delay(unsigned int MS)
	{
	unsigned int delay;
	for(delay=0;delay<MS;delay++);
	}
main()
	{
	char NumbersCode[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x27,0x7f,0x6f};
	char loop;
	while(1)
		{
		for(loop=0;loop<10;loop++)
			{
			P1=loop;
			P3=~NumbersCode[loop];
			Delay(50000);
			}

		}
	}