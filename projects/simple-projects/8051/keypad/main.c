#include<REG51.h>

#define Row1  P2_2
#define Row2  P2_1
#define Row3  P2_0

#define ColA  P2_3
#define ColB  P2_4
#define ColC  P2_5
#define ColD  P2_6


char KPread(void)
	{
	ColA=0;
	if(Row1==0)
		{
		ColA=1;
		return 1;
		}
	if(Row2==0)
		{
		ColA=1;
		return 2;
		}
	if(Row3==0)
		{
		ColA=1;
		return 3;
		}

////////////////////
	ColA=1;
	ColB=0;
	if(Row1==0)
		{
		ColB=1;
		return 4;
		}
	if(Row2==0)
		{
		ColB=1;
		return 5;
		}
	if(Row3==0)
		{
		ColB=1;
		return 6;
		}
////////////////////
	ColB=1;
	ColC=0;
	if(Row1==0)
		{
		ColC=1;
		return 7;
		}
	if(Row2==0)
		{
		ColC=1;
		return 8;
		}
	if(Row3==0)
		{
		ColC=1;
		return 9;
		}
////////////////////
	ColD=0;
	ColC=1;
	if(Row1==0)
		{
		ColD=1;
		return '*';
		}
	if(Row2==0)
		{
		ColD=1;
		return 0;
		}
	if(Row3==0)
		{
		ColD=1;
		return '#';
		}
	ColD=1;
////////////////////
	}

main()
	{
	char NumbersCode[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x27,0x7f,0x6f};
	Row1 = 1;
	Row2 = 1;
	Row3 = 1;
	ColA = 1;
	ColB = 1;
	ColC = 1;
	ColD = 1;

	while(1)
		{
		P3=NumbersCode[KPread()];
		}
	}