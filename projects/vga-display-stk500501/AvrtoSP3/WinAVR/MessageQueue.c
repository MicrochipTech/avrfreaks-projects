/*******************************************************************************
** Message Queue Manager
** Created April 12, 2007
** Jim Patchell
*******************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "task.h"
#include "BlockAlloc.h"
#include "MessageQueue.h"

static MSG *MsgPool=0;
static ECB *MsgBlocker;
static ECB *MqBlocker;
int MsgAllocCount = 0;

void MqInit(MESSAGE_QUEUE *mq,MSG **buffer,int size,char *name)
{
	PendSemaphore(MqBlocker,0);		//protect access
	mq->Head = 0;
	mq->Tail = 0;
	mq->nMsg = 0;
	mq->b = buffer;
	mq->Size = size;
	mq->Sem = NewSemaphore(0,SEMAPHORE_MODE_BLOCKING,name);
	PostSemaphore(MqBlocker,0);		//release access
}

void MqPut(MESSAGE_QUEUE *mq,MSG *m)
{
	char sr;

	if(mq)	//are we live?
	{
		sr = Disable();		//enter critical section
		if(mq->nMsg < mq->Size)	//is there room?
		{
			mq->b[mq->Head++] = m;	//put message into queue
			if(mq->Head == mq->Size)
				mq->Head = 0;	//wrap head pointer
			mq->nMsg++;			//increment number of messages
			PostSemaphore(mq->Sem,0);
		}
		Enable(sr);			//exit critical section
	}
}

MSG *MqGet(MESSAGE_QUEUE *mq)
{
	char sr;
	MSG *retval = 0;

	if(mq)
	{
		PendSemaphore(mq->Sem,0);		//wait for message
		sr = Disable();				//endter critical section
		retval = mq->b[mq->Tail++];	//remove message from queue
		if(mq->Tail == mq->Size)	//are we at the end of the line
			mq->Tail = 0;			//wrap tail pointer
		mq->nMsg--;					//decrement number of messages in queue
		Enable(sr);
	}
	return retval;
}

MSG *NewMSG(int payloadSize)
{
	MSG *retval = 0;

	if(MsgPool == NULL)	//nothing in pool, then use malloc
	{
		PendSemaphore(MsgBlocker,0);	//protect access
		retval = (MSG *)malloc(sizeof(MSG) + payloadSize);
		MsgAllocCount++;
		PostSemaphore(MsgBlocker,0);	//release
	}
	else
	{
		PendSemaphore(MsgBlocker,0);	//protect access
		retval = MsgPool;
		MsgPool = MsgPool->next;
		PostSemaphore(MsgBlocker,0);	//release
	}
	retval->MaxPayload = payloadSize;
	retval->ReplyHandle = NULL;
	retval->PayloadSize = 0;
	retval->next = 0;
	return retval;
}

void DeleteMSG(MSG *m)
{
	//-------------------------------------
	//put message into message pool
	//-------------------------------------
	PendSemaphore(MsgBlocker,0);	//protect access
	m->next = MsgPool;
	MsgPool = m;
	PostSemaphore(MsgBlocker,0);	//release access
}

void InitMessageQueueManager(void)
{
	MsgBlocker = NewSemaphore(1,SEMAPHORE_MODE_BLOCKING,"MSG_BLOCKER");
	MqBlocker = NewSemaphore(1,SEMAPHORE_MODE_BLOCKING,"MSG_BLOCKER");
}

