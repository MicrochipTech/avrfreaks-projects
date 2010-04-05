#include "zoBuffer.h"
#include <stdlib.h>
#include <string.h>
#include "zoMcu.h"

inline bool zoBufferIsEmpty(ZO_BUFFER *p)
{
	return (p->ctr > 0)?FALSE:TRUE;
}

inline bool zoBufferIsFull(ZO_BUFFER *p)
{
	return (p->ctr < p->size)?FALSE:TRUE;
}

void zoBufferFlush(ZO_BUFFER *p)
{
	enterCritical();
	p->pIn = p->data;
	p->pOut = p->data;
	p->ctr = 0;
	exitCritical();
}

bool zoBufferInit(ZO_BUFFER *p,u08 size)
{
	bool success = FALSE;
	enterCritical();

	p->ctr = 0;						//initialize counter to 0
	p->size = size;					//store size
	p->data = (u08*)calloc(size,1);		//allocate memory for data
	
	if( p->data != NULL )				//if allocation was successfull
	{
		p->pIn = p->data;	//initialize in pointer to start of data
		p->pOut = p->data;	//initialize out pinter to start of data
		success = TRUE;
	}

	exitCritical();

	return success;
}

inline u08  zoBufferGetIsr(ZO_BUFFER *p)
{
	u08 c;

	p->ctr--;							//decrement count of bytes in buffer
	c = *p->pOut++;					//get one piece of data
	if(p->pOut == &p->data[p->size])	//wrap pout pointer
		p->pOut = &p->data[0];
	
	return c;
}

u08  zoBufferGet(ZO_BUFFER *p)
{
	u08 c;
	
	enterCritical();
	c= zoBufferGetIsr(p);
	exitCritical();
	return c;	
}

inline void zoBufferPutIsr(ZO_BUFFER *p,const u08 data)
{
	p->ctr++;
	*p->pIn++ = data;
	if(p->pIn == &p->data[p->size])	//wrap the pIn ptr
		p->pIn = &p->data[0];
}

void zoBufferPut(ZO_BUFFER *p,const u08 data)
{
	enterCritical();
	zoBufferPutIsr(p,data);
	exitCritical();
}

inline u08 zoBufferReadIsr(ZO_BUFFER *p,u08 index)
{
	u08* c;
	
	c = p->pOut + index;
	if( c > &p->data[p->size] )
		c = p->pOut - p->size + index;
	
	return *c;
}

u08  zoBufferRead(ZO_BUFFER *p,u08 index)
{
	u08 c;
	enterCritical();
	c = zoBufferReadIsr(p,index);
	exitCritical();
	return c;
}

