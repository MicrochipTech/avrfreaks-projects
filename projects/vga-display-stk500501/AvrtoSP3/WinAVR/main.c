#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "system.h"
#include "task.h"
#include "rs232.h"
#include "MessageQueue.h"
#include "blockalloc.h"
#include "fpga.h"
#include "display.h"

#define STACKSIZET	256
#define STACKSIZE_EC	256


////////////////////////////////////////////////////////////////////////////
// external interrupt 4 defines
////////////////////////////////////////////////////////////////////////////

#define TIMER_CNTRL       *((volatile unsigned char *)0xf040)   //address of counter low
#define TIMER_CNTRH       *((volatile unsigned char *)0xf041)
#define TIMER_RLDLO       *((volatile unsigned char *)0xf042)
#define TIMER_RLDHI       *((volatile unsigned char *)0xf043)
#define TIMER_LCHLO       *((volatile unsigned char *)0xf044)
#define TIMER_LCHHI       *((volatile unsigned char *)0xf045)
#define TIMER_STATUS      *((volatile unsigned char *)0xf046)   //read only address
#define TIMER_DOLATCH     *((volatile unsigned char *)0xf046)   //write only address
#define TIMER_CTRL        *((volatile unsigned char *)0xf047)

char Stack1[STACKSIZET],Stack2[STACKSIZET],Stack3[STACKSIZET],Stack4[STACKSIZET],Stack5[STACKSIZE_EC];
TCB T1,T2,TIdle,T3,T5;
char TX0[256],RX0[256],TX1[256],RX1[256];
SERIAL *pR0,*pR1;
char s[80];
ECB T1Sem;
ECB TimerSem;

long Idle,TaskTwoCount;


volatile int Ticks;
ECB delay;

void printDone(void)
{
	RWrite(pR0,"Done\r\n",6);
}
void FillSectorBuffer(long Sector,char *b)
{
	long *lp;
	int i;

	lp = (long *)b;

	for(i=0;i<128;++i)
		lp[i] = Sector;
}

char MakeHex(char v)
{
	if(v > 9)
		v += 'A' - 10;
	else
		v += '0';
	return v;
}

int CharToHex(char v,char *s,char c)
{
	int i = 0;

	s[i++] = MakeHex((v >> 4) & 0x0f);
	s[i++] = MakeHex(v & 0x0f);
	if(c)
		s[i++] = c;
	return i;
}

int UnsignedToHex(unsigned v,char *s,char c)
{
	char *t = (char *)&v;
	int i = 0;

	++t;
	i += CharToHex(*t--,&s[i],0);
	i += CharToHex(*t,&s[i],c);
	return i;
}

void HexDump(char *b,int n)
{
	char *s = NewBlock();
	int i,j,l;
	unsigned adr;

	adr = (unsigned)b;

	for(j=0;j<n;j+=8)
	{
		l = 0;
		l += UnsignedToHex(adr,&s[l],' ');
		for(i=0;i<8;++i)
		{
			l += CharToHex(b[i+j],&s[l],(i==7)?' ':'-');
		}
		for(i=0;i<8;++i)
		{
			if(isprint(b[i+j]))
				s[l++] = b[i+j];
			else
				s[l++] = '.';
		}
		s[l++] = '\r';
		s[l++] = '\n';
		RWrite(pR0,s,l);
		adr += 8;
	}
	DeleteBlock(s);
}

////////////////////////////////////////////////////////////////////////////////

void ExtInt4Init(void)
{
	EIMSK |= BIT(INT4);
	TIMER_DOLATCH = 0;
	EIFR = BIT(INT4);
	TIMER_CTRL = 0xc4;	//enable counter and counter interrupt
}

int I4Ticks;

void INT4_vect(void) __attribute__ ((naked));
void INT4_vect(void)
{
	SAVE_IRQ();
	++InterruptCount;
	TIMER_DOLATCH = 0;
	EIFR = BIT(INT4);
	++I4Ticks;
	PostSemaphore(&TimerSem,0);
	ExitInterrupt();
	RESTORE_IRQ();
}

void Timer0Init(void)
{
	TCCR0B = 0x00;	//stop
	ASSR  = 0x00;	//async mode
	TCNT0 = 0x80;
	OCR0A = 0xff;
	TCCR0B = BIT(CS01) | BIT(CS00); //start timer
	TIMSK0 = BIT(OCIE0A);
}

void TIMER0_COMPA_vect(void) __attribute__ ((naked));
void TIMER0_COMPA_vect(void)
{
	SAVE_IRQ();
	TCNT0 = 0x80;
	++InterruptCount;
	++Ticks;
	TimerTicker();
	ExitInterrupt();
	RESTORE_IRQ();
}

void reset(void)
{
	*((volatile char *)0xf001) = 0x5a;
}

void PortInit(void)
{
	PORTA = 0x00;	//IDE Data LSB
	DDRA  = 0x00;	//leave as inputs unless writing
	PORTB = 0xff;	//IDE control lines;
	DDRB  = 0xff;	//output port
	PORTC = 0x00;	//IDE Data MSB
	DDRC  = 0x00;	//leave as inputs unless writing
	PORTD = 0x00;
	DDRD  = 0x00;
	PORTE = 0x00;
	DDRE  = 0x00;
	PORTF = 0x00;
	DDRF  = 0x00;	//these are all outputs
	PORTG = 0;	//IRQ
	DDRG  = BIT(3);	//irq is an input
	PORTG |= BIT(3);	//reset
	PORTG &= ~BIT(3);
}

void TaskOne(void* arg)
{
	unsigned char v;
	char f;
	int c;
	int j;
	static char s[80];

	v = 1;
	f = 1;
	c = 0;
	j = 0;

	while(1)
	{
		PendSemaphore(&T1Sem,50);
		++c;
		Ticks = 0;
		if(f)
		{
			v <<= 1;
			if(v & 0x80)
				f = 0;
		}
		else
		{
			v >>= 1;
			if(v & 0x01)
				f = 1;
		}
		PORTB = ~v;
		++j;
		sprintf(s,"This is J = %d\n",j);
		SWrite(s);
	}
}

void TaskTwo(void* arg)
{
	unsigned char v;
	char f;
	int c;

	v = 1;
	f = 1;
	c = 0;
	while(1)
	{
		PendSemaphore(&TimerSem,0);
		if(f)
		{
			v <<= 1;
			if(v & 0x80)
				f = 0;
		}
		else
		{
			v >>= 1;
			if(v & 0x01)
				f = 1;
		}
		FPGA_LED = v;
		++c;
		*((unsigned *)0xf002) = (unsigned)c;
	}
}

void IdleTask(void *arg)
{
	Idle = 0;
	while(1)
	{
		++Idle;
	}
}

void (*func)(void);

int main()
{
	Disable();
	XMCRA = BIT(SRE);
	OS_InitMalloc();
	InitBlockAlloc(64,8);
	PortInit();
	InitTask();
	Timer0Init();
	pR0 = RInit(TX0,256,RX0,256,0,115200);
	pR1 = RInit(TX1,256,RX1,256,1,57600);
	CreateTask(&TIdle,IdleTask,Stack3,STACKSIZET,1,"Idle",NULL);
	Insert(&ActiveTasks,&TIdle);
	CreateTask(&T1,TaskOne,Stack1,STACKSIZET,2,"T1",NULL);
	Insert(&ActiveTasks,&T1);
	CreateSemaphore(&T1Sem,0,SEMAPHORE_MODE_TIMEOUT,"T1Sem");
	CreateTask(&T2,TaskTwo,Stack2,STACKSIZET,2,"T2",(void *)0x1234);
	Insert(&ActiveTasks,&T2);
	CreateSemaphore(&TimerSem,0,SEMAPHORE_MODE_BLOCKING,"TmrSem");
	InitVGA();
	reset();
	ExtInt4Init();
	StartOS();
	while(1);
	return 0;
}
