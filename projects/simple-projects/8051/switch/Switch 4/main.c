#include<REG51.h>

void Delay(unsigned int MS)
	{
	unsigned int delay;
	for(delay=0;delay<MS;delay++);
	}

main()
	{
	bit HolldSwitchStatus;
	char Loop;
	P1_0 = 1;
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
					for(Loop=0;Loop<6;Loop++)
						{
						P0_0=~P0_0;
						Delay(50000);
						}
					}
			}
		}
	}