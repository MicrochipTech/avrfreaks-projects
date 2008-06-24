//********************************************************
//
// ByteQueue.c
//
//********************************************************

#include "stdio.h"
#include "task.h"
#include "ByteQueue.h"

void CreateBQ(BQ *pBQ,char *b,unsigned size)
{
	pBQ->task_h = NULL;
	pBQ->pBfr = b;
	pBQ->Size = size;
	pBQ->Head = 0;
	pBQ->Tail = 0;
	pBQ->nChar = 0;
}

int BQget(BQ *pBQ)
{
	//******************************************************
	// Get a character from the queue
	//
	//
	// parameters:
	//	pBQ........pointer to the queue to get a char from
	//
	// return value:
	//	returns a negative value on error
	//	returns a positive value on success
	//	suspends task if there are no characters to get
	//******************************************************
	char sr;
	int retval;

	sr = Disable();
	if(pBQ->nChar)	//are there any characters to get?
	{
		//-------------------------------------
		// Get Character from Buffer
		// Check Head pointer for Wrap
		// Increment number of chars in buffer
		//------------------------------------
		retval = (int)((unsigned char)pBQ->pBfr[pBQ->Head++]);
		if(pBQ->Head == pBQ->Size) pBQ->Head = 0;
		pBQ->nChar++;
		Enable(sr);
	}
	else			//no, suspend task
	{
		pBQ->task_h = CurrentTask;
		CurrentTask->status = EVENT_NOERROR;
		//---------------------------------
		//swap out current task
		//---------------------------------
		CurrentTask->TimeStamp = TStamp;
		if(Delete(&ActiveTasks,(void **)&NextTask))	//get highest priority task
		{
			NextTask->TcbSwaps++;
			DoSwap();                    //do context swap
		}
		else
		{
			CurrentTask->status = EVENT_NOTASKS;
		}
		retval = CurrentTask->status;
		Enable(sr);
	}
	return retval;
}

int BQPut(BQ *pBQ,int c)
{
	//-------------------------------------
	// Put a character into the queue
	//
	// This function is intended to be used
	// inside of an interrupt routine to
	// signal a waiting task that data is
	// ready.  It can also be used at the
	// task level, however if the buffer
	// overflows, we just do nothing (well
	// we return an error...but that is it
	// Interrupt routines cannot care if
	// the queue is full
	//
	// parameters:
	//	pBQ.....pointer to queue to put data into
	//	c.......character to add to queue
	//
	// return value:
	//	0 on success
	//	negative value on fail
	//--------------------------------------
	char sr;
	int retval = EVENT_NOERROR;

	sr = Disable();		//just in case we are at task level
	if(pBQ->nChar < pBQ->Size)	//any place for a character?
	{
		pBQ->pBfr[pBQ->Tail++] = (char)c;
		if(pBQ->Tail == pBQ->Size) pBQ->Tail = 0;
		pBQ->nChar++;
	}
	else
	{
		retval = EVENT_OVERFLOW;
	}
	if(pBQ->task_h)	//is there a task pending?
	{
		pBQ->task_h->status = retval;	//set its status to retval
		Insert(&ActiveTasks,pBQ->task_h);	//put task back onto active list
		pBQ->task_h = NULL;
		Enable(sr);	//enable interrupts
		Yeild();	//if we are at task level...we will yeild
	}
	else
		Enable(sr);
	return retval;
}

int BQWrite(BQ *pBQ,char *b,int n)
{
	return 0;
}

