#include "zoError.h"
#include "zoMcu.h"

void zoErrorInit(ZO_ERROR *p)
{
	enterCritical();
	p->ctr = 0;						//initialize counter to 0
	p->pIn = p->error;	//initialize in pointer to start of data
	p->pOut = p->error;	//initialize out pinter to start of data
	exitCritical();
}

inline u08 zoErrorGetIsr(ZO_ERROR *p)
{
	u08 c;
	
	p->ctr--;							//decrement count of bytes in buffer
	c = *p->pOut;					//get one piece of data
	p->pOut++;						//point to next place from which data will be fetched
	if(p->pOut == &p->error[ZO_ERROR_STORAGE_SIZE])	//wrap pout pointer
		p->pOut = &p->error[0];

	return c;
}

u08 zoErrorGet(ZO_ERROR *p)
{
	u08 c;
	
	enterCritical();
	c = zoErrorGetIsr(p);
	exitCritical();
	return c;
}

inline void zoErrorPutIsr(ZO_ERROR *p,const u08 error)
{
	if( p->ctr < ZO_ERROR_STORAGE_SIZE )
		p->ctr++;;

	*p->pIn = error;
	p->pIn++;
	if(p->pIn == &p->error[ZO_ERROR_STORAGE_SIZE])	//wrap the pIn ptr
		p->pIn = &p->error[0];	
}

void zoErrorPut(ZO_ERROR *p,const u08 error)
{
	enterCritical();
	zoErrorPutIsr(p,error);
	exitCritical();
}

inline bool zoErrorIsEmpty(ZO_ERROR *p)
{
	return (p->ctr>0)?FALSE:TRUE;
}
