#include "scImConsole.h"
#include "avr/io.h"

#define UDRE 5
#define RXC 7
void fImputchar(char cData)
{
   while(!(UCSRA & (1<<UDRE)));
   UDR=cData;	
}
char fImgetchar()
{
	
	while(!(UCSRA & (1<<RXC)));
	return UDR;
}
void fImprintf(char *stream)
{
	int i=0;
	while(*(stream+i)!=0)
	{
		fImputchar(*(stream+i));
		i++;
	}
}
void fImscanf(char *sData)
{
	//sData store at program memory, so we can't change its value, so I use ssData
	static char *ssData;
	int i=0;
	ssData=sData;
	do
	{
		*(ssData+i)=fImgetchar();
		//ssData++;
		i++;
		fImputchar(*(ssData+i-1));
	}while (*(ssData+i-1)!=0x0d);
	fImputchar(0x0a);
	*(ssData+i-1)=0;	
}
int  fImInitSerial()
{
	// USART initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART Receiver: On
	// USART Transmitter: On
	// USART Mode: Asynchronous
	// USART Baud Rate: 19200 (Double Speed Mode)
	UCSRA=0x02;
	UCSRB=0x18;
	UCSRC=0x86;
	UBRRH=0x00;
	UBRRL=0x33;	
}


char *fImgets(char *str,unsigned int len)
{
unsigned int n=len;
char *p=str;
char k;

while (n)
      {
      l0:
      k=fImgetchar();
      fImputchar(k);
      if (k==8)
         {
         if (len>n)
            {
            --p;
            ++n;
			fImputchar(' ');
			fImputchar(0x08);
            };
         goto l0;
         };
      if (k==10||k==13) 
      {  
        fImputchar(10);
        break;
      }
      *p++=k;
      --n;
      };
*p=0;
return str;
}
void fImprinth(char *stream)
{
	int i=0;
	while(*(stream+i)!=0)
	{
		if(*(stream+i)>='0'&&*(stream+i)<='9')
			fImputchar(*(stream+i)-0x30);
		else if(*(stream+i)>='A'&&*(stream+i)<='F')
			fImputchar(*(stream+i)-0x37);
		else if(*(stream+i)>='a'&&*(stream+i)<='f')
			fImputchar(*(stream+i)-0x57);
		else
			fImputchar('X');
		i++;
	}
}
