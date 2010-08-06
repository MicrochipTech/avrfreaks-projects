/*
	Dominik Sevc
	Program pre citacku RFID 4095

	ATMEGA32 DIP40

	Pozor kristal musi byt aspon 16MHz
	inac nebude dobre fungovat

*/
#define 	outb(addr, data)   addr = (data)
#define 	inb(addr)   (addr)
#define 	outw(addr, data)   addr = (data)
#define 	inw(addr)   (addr)
#define 	outp(addr, data)   addr = (data)
#define 	inp(addr)   (addr)
#define 	BV(bit)   (1<<(bit))
#define 	cbi(reg, bit)   reg &= ~(BV(bit))
#define 	sbi(reg, bit)   reg |= (BV(bit))
#define 	cli()   __asm__ __volatile__ ("cli" ::)
#define 	sei()   __asm__ __volatile__ ("sei" ::)
#define 	DDR(x)   ((x)-1)
#define 	PIN(x)   ((x)-2)
/// Typedefs //////////
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "uartio.h"


/// Define port B
#define		CLOCK	0 // on port B  T0 input , just this pin can use or T1 but (T0 is better)
#define		DATA	1 // on port B  T1 , can use free pin
#define		SDOWN	2 // on port B , not used


#define		BUFSIZE 16
#define		DataSize 10
#define		TimerValue 255-31

char *txt;

unsigned char Fir,Sec,DefBit;
unsigned char Fir1,Sec1,DefBit1;


uint8_t EM_rx_buf[BUFSIZE];
uint8_t CardData[10];
 
char CheckToHex(uint8_t *Buffer,uint8_t *CData);
void RollData(uint8_t Count,uint8_t *Buffer);
void Send_Card_DataX(uint8_t Count,uint8_t *Buffer);


//Prerusenie od casovaca 0 naplnanie dat do buffra
SIGNAL(SIG_OVERFLOW0)
{
outb(TCNT0,TimerValue);					// Set new value to Timer
if (PINB&(1<<DATA)) Fir=1; else Fir=0;	// Store pin state
if ((Fir)&&(Sec))   {DefBit=1;Fir1=0;Sec1=1;}	// If detect 11 - then data are 1 and sequence is 10 1-Sec 0-Fir
if ((!Fir)&&(!Sec))	{DefBit=0;Fir1=1;Sec1=0;}	// If detect 00 - then data are 0 and sequence is 01 0-Sec 1-Fir
if ((Fir1==Fir)&&(Sec1==Sec)) 					// If detect sequence then roll buffer, set variable ....
	{
	if (DefBit) {RollData(13,EM_rx_buf);sbi(EM_rx_buf[0],0);} 
	else 		{RollData(13,EM_rx_buf);cbi(EM_rx_buf[0],0);}
	}
Sec=Fir;	// Store value for again compare
if ((EM_rx_buf[12]==0x0F)&&(EM_rx_buf[11]==0x1F)&&(!(EM_rx_buf[0]&(1<<0)))) // may be we have header
	{
	if (CheckToHex(EM_rx_buf,CardData))
		{
		cbi(TIMSK,TOIE0);
		Send_Card_DataX(5,CardData);
		uart_send_string("\r\n");
		sbi(TIMSK,TOIE0);
		}
	}
}


/*************************** main() *******************************/
		
int main(void)
{
//	outb(PORTA,0x00);	//internal pullups up
//	outb(DDRA,0xFF);	//internal pullups up
	outb(PORTD,0xff);	//internal pullups up
	outb(DDRD,0x00);	//internal pullups up
	outb(PORTB,(1<<CLOCK)|(1<<DATA)|(0<<SDOWN));	//internal pullups up
	outb(DDRB,(0<<CLOCK)|(0<<DATA)|(1<<SDOWN));	//internal pullups up
//	outb(PORTC,0x00);
//	outb(DDRC,0x00);

/********************** INIT UART 0 ******************************/
	uart_init(BAUD_57k6); //9k6
	txt=malloc(150);
	
 	outb(TCCR0,(1<<CS02)|(1<<CS01)); ///CTC MODE, FAILING EDGE T0

	outb(TCNT0,TimerValue);
	sbi(TIMSK,TOIE0);

/********************************** MAIN LOOP *************************************/

	sbi(TIMSK,TOIE0);

	Fir=0;Sec=0;DefBit=0; 
	Fir1=0;Sec1=0;DefBit1=0; 

	sei();
	while(1) 
	{

	}
}


/// Roll data on Buffer Count x to left , bit per bit
void RollData(uint8_t Count,uint8_t *Buffer)
{
signed char i;
for (i=Count-1;i>=0;i--)
	{
	Buffer[i]<<=1;		// roluje o jeden bit
	Buffer[i]&=~0xE0;	// zmaze horne bity na 0 - 7,6,5
	if (Buffer[i-1]&(1<<4)) sbi(Buffer[i],0); // ak je bit4 (5) 1 tak ho zadefinuje ako 0bit predchadzajuceho bytu
	}
}

/// Send data from Buffer to Uart Count is number to send
void Send_Card_DataX(uint8_t Count,uint8_t *Buffer)
{
	signed char i;
	unsigned char a;
	for(i=0;i<Count;i++) 
	{
	a = Buffer[i]>>4;
	uart_send_char(B2X((a)&0x0f));
	a = Buffer[i];
	uart_send_char(B2X((a)&0x0f));
	}// uart_send_char(B2X((Buffer[i])&0x0f));B2X((Buffer[i])&0x0f)
}


// Check data in buffer (use EM_rx_buf and Data) and change it to hex , return define if data are valid or corupt
char CheckToHex(uint8_t *Buffer,uint8_t *CData)
{
char c;
char temp;
char out;

out=1;

// Control horizontal parity
	for (c=10;c>0;c--)
		{
		temp=Buffer[c];
		if (!(temp==0||temp==3||temp==5||temp==6||temp==9||temp==10||temp==12||temp==15||
		     temp==17||temp==18||temp==20||temp==23||temp==24||temp==27||temp==29||temp==30))
		{out = 0;break;	}
		}
// Control horizontal parity

		
// Control vertical parity
	temp = 0;
	for (c=11;c>0;c--)	temp ^=Buffer[c-1];		// all data XOR 
	cbi(temp,0);			// clear bit 0 (this is parity)
	if (temp) out=0;		// and if result is 0 it's OK, if result is !0 then bad vertical prity
// Control vertical parity

Buffer[12] = 0;Buffer[11] = 0; // clear header data are OK.

//	Now can copy real data in to data buffer
for (c=5;c>0;c--)
	{
	temp = Buffer[c*2]<<3;	// copy data bit
	temp &= ~0x0F;			// clear parity bit
	temp |= Buffer[(c*2)-1]>>1; // copy next half bit (must shift , parity bit)
	CData[5-c]=temp;
	}

return out;
}

