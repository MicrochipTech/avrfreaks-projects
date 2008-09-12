#include<REG51.h>

void Delay(unsigned int MS)
	{
	unsigned int delay;
	for(delay=0;delay<MS;delay++);
	}

main()
	{
	char NumbersCode[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x27,0x7f,0x6f};
	char Counter=0;
	bit HolldSwitchStatus;
	P1_0 = 1;
	P3=~NumbersCode[Counter];
	while(1)
		{
		HolldSwitchStatus = P1_0;
		Delay(500);
		if(HolldSwitchStatus != P1_0)
			{
			HolldSwitchStatus = P1_0;
			Delay(5000);
				if(P1_0 == 0)
					{
					Counter++;
					if(Counter>3)
						{
						Counter=0;
						P0_0=~P0_0;
						}
					P3=~NumbersCode[Counter];
					}
			}
		}
	}