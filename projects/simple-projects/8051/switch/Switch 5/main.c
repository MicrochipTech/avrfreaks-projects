#include<REG51.h>

main()
	{
	P1_0 = 1;
	while(1)
		{
		P0_0=P1_0;
		}
	}