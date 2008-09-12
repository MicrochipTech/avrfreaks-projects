#include <REG51.h>
#include <stdio.h> 

main()
	{
	char NumbersCode[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x27,0x7f,0x6f};
	char Number=0;

	SCON  = 0x50; /* 	SCON: mode 1, 8-bit UART, enable rcvr      		*/
    TMOD |= 0x20; /* 	TMOD: timer 1, mode 2, 8-bit reload        		*/
    TH1   = 0xfd; /* 	TH1:  reload value for 9600 baud @ 11.0592MHz  	*/
    TR1   = 1;    /* 	TR1:  timer 1 run                          		*/
    TI    = 1;    /* 	TI:   set TI to send first char of UART    		*/
	//ES=1;
	//EA=1;
	P2 = NumbersCode[Number];
	
	while(1)
		{
		//printf ("\n Enter Number Between 0&9\n");
		while(RI==0);
		RI=0;
		Number=SBUF;
		P2 = NumbersCode[Number&0x0f];
		SBUF=++Number;
		}
	}