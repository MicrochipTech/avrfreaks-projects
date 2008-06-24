/*****************************************************
** VGA display Driver
******************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "task.h"
#include "fpga.h"
#include "display.h"



ECB VgaSem;

static char fg=RED;
static char bg=BLACK;

void ExtInt5Init(void)
{
	EIMSK |= BIT(INT5);
}

void INT5_vect(void) __attribute__ ((naked));
void INT5_vect(void)
{
	SAVE_IRQ();
	++InterruptCount;
	EIFR = BIT(INT5);
	*((volatile char *)0x7010) = 0x02;	//clear interrupt
	PostSemaphore(&VgaSem,0);
	ExitInterrupt();
	RESTORE_IRQ();
}

void InitVGA(void)
{
	int i;
	unsigned *pI;

	for(i=0,pI = (unsigned *)VGA_CHARMEM ;i< (80*60);++i)
	{
		*pI++ = ' ' | FG(fg) | BG(bg);
	}
	i = 0;
	char *pL = (char *) VGA_COLOR_LUT;
	*pL++ = (BLACK << 3) | BLACK;
	*pL++ = (RED << 3) | RED;
	*pL++ = (GREEN << 3) | GREEN;
	*pL++ = (YELLOW << 3) | YELLOW;
	*pL++ = (BLUE << 3) | BLUE;
	*pL++ = (VIOLET << 3) | VIOLET;
	*pL++ = (CYAN << 3) | CYAN;
	*pL++ = (WHITE << 3) | WHITE;
	ExtInt5Init();
	CreateSemaphore(&VgaSem,0,SEMAPHORE_MODE_BLOCKING,"VgaSem");
}

void PutS(int x,int y,char c, char fg,char bg)
{
	unsigned *pU;
	char *pC;

	if(c != '\n')
	{
		int i = x + 80 * y;
		pU = (unsigned *)VGA_CHARMEM;
		pU += i;
		pC = (char *)pU;
		*pC++ = c;
		*pC = (fg << 3) | bg;
	}
}

int line;
int col;

void SPutC(char c)
{
	unsigned *pD,uD;
	int i;

	PutS(col,line,c,fg,bg);
	++col;
	if((col == 80) || (c == '\n'))
	{
		col = 0;
		line++;
		if(line == 60)	//time to scroll
		{
//			pD = 0x4000;
//			pS = 0x40a0;
//			for(i=0;i<4720;++i)
//				*pD++ = *pS++;
			*((volatile char *)0x7010) = 1;
			PendSemaphore(&VgaSem,0);
//			while( (*((volatile char *)0x7010) & 0x02) == 0);
			uD = 0x20 | FG(fg) | BG(bg);
			pD = (unsigned *)(0x4000 + 9440);
			for(i=0;i<80;++i)
				*pD++ = uD;
			line--;
		}
	}
}

void SWrite(char *s)
{
	while(*s)
		SPutC(*s++);
}

