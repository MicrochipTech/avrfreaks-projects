#include<avr/io.h> 
#include<util/delay.h> 
 
/*Macros definition*/ 

#define CHECKBIT(x,b) x&(1<<b)  	//Checks bit status 
#define SETBIT(x,b) x|=(1<<b)   	//Sets the particular bit 
#define CLEARBIT(x,b) x&=~(1<<b) 	//Sets the particular bit 
#define TOGGLEBIT(x,b) x^=(1<<b) 	//Toggles the particular bit 


void right(void)   		
{
CLEARBIT(PORTB,6);
CLEARBIT(PORTB,5);
CLEARBIT(PORTE,3);
SETBIT(PORTE,2);
}

void left(void)			
{
CLEARBIT(PORTB,0);
SETBIT(PORTB,5);
CLEARBIT(PORTE,3);
CLEARBIT(PORTE,2);
}

void str(void)			
{
CLEARBIT(PORTB,6);
SETBIT(PORTB,5);
CLEARBIT(PORTE,3);
SETBIT(PORTE,2);
}
void back(void)			
{
CLEARBIT(PORTB,5);
SETBIT(PORTB,6);
CLEARBIT(PORTE,2);
SETBIT(PORTE,3);
}

void stop(void)
{
CLEARBIT(PORTB,6);
CLEARBIT(PORTB,5);
CLEARBIT(PORTE,3);
CLEARBIT(PORTE,2);
}
void uart_init(void)
{
	UBRR1L=0x67;
	UBRR1H=0x00;
	UCSR1B=0x18;
	UCSR1C=0x06;
}
unsigned char uart_rec(void)
{
	while(!(UCSR1A & (1<<RXC)));
	return UDR1;
}
void trans(unsigned char data)
{
	while (!(UCSR1A & (1<<UDRE)));
	UDR1=data;
}
void transtring(char string[100])
{
unsigned int n=0;
while(string[n]!='\0')
{
trans(string[n]);
n++;
}
}
int main(void)
{
unsigned char rxd;
//intialize motor ports
SETBIT(DDRB,6);
SETBIT(DDRB,5);
SETBIT(DDRE,3);
SETBIT(DDRE,2);
CLEARBIT(PORTB,6);
CLEARBIT(PORTB,5);
CLEARBIT(PORTE,3);
CLEARBIT(PORTE,2);

uart_init();


while(1)
{
//recieve function
while(!(CHECKBIT(UCSR1A,RXC1)));  //wait until command is received
rxd = UDR1;

if(rxd=='f')
	{
	str();
	transtring("\nforward");
	}

else if(rxd=='l')
	{
	left();
	transtring("\nleft");
	}


else if(rxd=='r')
	{
	right();
	transtring("\nright");
	}
else if(rxd=='b')
	{
	back();
	transtring("\nback");
	}
else if(rxd=='s')
	{
	stop();
	transtring("\nStop");
	}
else
	{
	transtring("\ninvald");
	stop();

	}
}
return(0);
}