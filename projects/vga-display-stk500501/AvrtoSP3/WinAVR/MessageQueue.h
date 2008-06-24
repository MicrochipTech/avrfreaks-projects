/*******************************************************************************
** MessageQueue Manager
**
** Created April 12, 2007
** Jim Patchell
**
*******************************************************************************/

#ifndef MESSAGEQUEUE__H
#define MESSAGEQUEUE__H

typedef struct _msg{
	short int MsgCmd;		//message command
	short int PayloadSize;	//size of payload
	short int MaxPayload;	//max size of payload
	short int ReplyCmd;		//command to use to reply
	void *ReplyHandle;		//handle of (pointer to Message queue) reply
	struct _msg *next;		//used for message management
	char Payload[1];			//payload data...word...but can be any size
}MSG;

typedef struct _msgqueue {
	ECB *Sem;		//counting Semaphore
	int Head;		//head pointer
	int Tail;		//tail pointer
	int nMsg;		//number of messages
	int Size;		//maximum number of messages
	MSG **b;		//message buffer
}MESSAGE_QUEUE;

extern void MqInit(MESSAGE_QUEUE *mq,MSG **buffer,int size,char *name);
extern void MqPut(MESSAGE_QUEUE *mq,MSG *m);
extern MSG *MqGet(MESSAGE_QUEUE *mq);
extern MSG *NewMSG(int payloadSize);
extern void DeleteMSG(MSG *m);
extern void InitMessageQueueManager(void);

extern int MsgAllocCount;

#endif
