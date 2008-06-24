/*******************************************************************
** Device Driver for UARTS on ATmega1281/2561
**
** Craeted July 10, 2007
** by Jim Patchell
**
** Written for WinAVR
**
********************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "system.h"
#include "rs232.h"
#include "task.h"

static SERIAL gSerial[N_PORTS];
ECB nSem0,nSem1,nbSem0,nbSem1;

ECB *portSems[2] = {
	&nSem0,
	&nSem1
};

ECB *BlockSems[2] = {
	&nbSem0,
	&nbSem1
};


static RREG SAdr[2] = {
	{(unsigned char *)&UDR0,(unsigned char *)&UCSR0A,(unsigned char *)&UCSR0B,(unsigned char *)&UCSR0C,(unsigned short *)&UBRR0},
	{(unsigned char *)&UDR1,(unsigned char *)&UCSR1A,(unsigned char *)&UCSR1B,(unsigned char *)&UCSR1C,(unsigned short *)&UBRR1},
};

int RGetC(SERIAL *pI)
{
	int retval;
	char sr;

	PendSemaphore(pI->Rx.pSem,0);
	sr = Disable();		//enter critical section
	if(pI->Rx.nChar)	//are there any characters to recieve
	{
		retval = (int)pI->Rx.buff[pI->Rx.tail++];	//get character	
		if(pI->Rx.tail == pI->Rx.size) pI->Rx.tail = 0;	//check tail pointer
		pI->Rx.nChar--;		//decrement the number of chars in buffer
	}
	else
		retval = -1;	//return error
	Enable(sr);	//exit critical section
	return retval;
}


void RPutC(SERIAL *pI,int c)
{
	char sr;

	sr = Disable();		//enter critical section
	if(pI->Tx.nChar < pI->Tx.size)	//and room?
	{
		pI->Tx.buff[pI->Tx.head++] = (unsigned char)c;
		if(pI->Tx.head == pI->Tx.size) pI->Tx.head = 0;	//check head pointer
		if(!pI->Tx.nChar)	//if first char
			*pI->regs->pUCSRnB |= BIT(UDRIE0);	//enable tx interrupts
		pI->Tx.nChar++;		//increment number of chars in buffer
	}
	Enable(sr);	//exit critical section
}

void RWrite(SERIAL *pI,char *b,int l)
{
	int i = 0;

	PendSemaphore(pI->Tx.pSem,0);	//blcok access
	for(i=0;i<l;++i)
		RPutC(pI,b[i]);	//output characters
	PostSemaphore(pI->Tx.pSem,0);	//release lock
}

int RStatus(SERIAL *pI)
{
	return pI->Rx.nChar;	//return number of chars in recieve buffer
}

int REmpty(SERIAL *pI)
{
	int rv=0;

	if(pI->Tx.nChar == 0)
		rv = 1;
	return rv;
}

static unsigned short CalculateBaudRate(long nBR)
{
	return (unsigned short)( ((SYSTEM_CLOCKRATE / nBR) / 16L) - 1   );
}

static char *PNames[2] = {
	"RX0",
	"RX1"
};

static char *TNames[2] = {
	"TX0",
	"TX1"
};

SERIAL *RInit(char *pTb,unsigned short nTSize,char *pRb,unsigned short nRSize, unsigned short nPort,long nBaudRate)
{
	gSerial[nPort].regs = &SAdr[nPort];
	gSerial[nPort].Tx.buff = pTb;
	gSerial[nPort].Tx.head = 0;
	gSerial[nPort].Tx.tail = 0;
	gSerial[nPort].Tx.nChar = 0;
	gSerial[nPort].Tx.size = nTSize;
	gSerial[nPort].Tx.pSem = BlockSems[nPort];
	gSerial[nPort].Rx.buff = pRb;
	gSerial[nPort].Rx.head = 0;
	gSerial[nPort].Rx.tail = 0;
	gSerial[nPort].Rx.nChar = 0;
	gSerial[nPort].Rx.size = nRSize;
	gSerial[nPort].Rx.pSem = portSems[nPort];
	CreateSemaphore(portSems[nPort],0,SEMAPHORE_MODE_BLOCKING,PNames[nPort]);
	CreateSemaphore(BlockSems[nPort],1,SEMAPHORE_MODE_BLOCKING,TNames[nPort]);
	*gSerial[nPort].regs->pUCSRnB = 0x00; 	//disable while setting baud rate
	*gSerial[nPort].regs->pUCSRnA = 0x00;	//Normal UART Mode (bits 1 and 0)
	*gSerial[nPort].regs->pUCSRnC = BIT(UCSZ01) | BIT(UCSZ00);	//8 bits, 1 Stop Bit, No Parity
	*gSerial[nPort].regs->pUBRR = CalculateBaudRate(nBaudRate);
	*gSerial[nPort].regs->pUCSRnB = BIT(RXCIE0) | BIT(RXEN0) | BIT(TXEN0); 	//Enable UART
	
	return &gSerial[nPort];
}

//******************************************************************************
// Interrupt service routines for Serial Ports
//******************************************************************************



static void RxIsr(SERIAL *pI)
{
	while(*pI->regs->pUCSRnA & BIT(RXC0))	//while chars availiable
	{
		pI->Rx.buff[pI->Rx.head++] = *pI->regs->pUDR;	//get char from UART
		if(pI->Rx.head == pI->Rx.size) pI->Rx.head = 0;
		pI->Rx.nChar++;	//increment number of chars in buffer
		PostSemaphore(pI->Rx.pSem,0);
	}
}

static void TxIsr(SERIAL *pI)
{
	char loop = 1;

	while((*pI->regs->pUCSRnA & BIT(UDRE0)) && loop)	//while space availiable in TX
	{
		if(pI->Tx.nChar)	//while there are characters
		{
			*pI->regs->pUDR = pI->Tx.buff[pI->Tx.tail++];	//stuff char
			if(pI->Tx.tail == pI->Tx.size) pI->Tx.tail = 0;
			pI->Tx.nChar--;	//decrement number of chars in buffer
		}
		else
		{
			//we have run out of characters...disable interrupt
			*pI->regs->pUCSRnB &= ~(BIT(UDRIE0));	//disable tx interrupts
			loop = 0;	//terminate loop
		}
	}
}

void USART0_RX_vect(void) __attribute__ ((naked));
void USART0_RX_vect(void)
{
	SAVE_IRQ();
	++InterruptCount;
	RxIsr(&gSerial[0]);
	ExitInterrupt();
	RESTORE_IRQ();
}

void USART0_UDRE_vect(void) __attribute__ ((naked));
void USART0_UDRE_vect(void)
{
	SAVE_IRQ();
	++InterruptCount;
	TxIsr(&gSerial[0]);
	ExitInterrupt();
	RESTORE_IRQ();
}

void USART1_RX_vect(void) __attribute__ ((naked));
void USART1_RX_vect(void)
{
	SAVE_IRQ();
	++InterruptCount;
	RxIsr(&gSerial[1]);
	ExitInterrupt();
	RESTORE_IRQ();
}

void USART1_UDRE_vect(void) __attribute__ ((naked));
void USART1_UDRE_vect(void)
{
	SAVE_IRQ();
	++InterruptCount;
	TxIsr(&gSerial[1]);
	ExitInterrupt();
	RESTORE_IRQ();
}





