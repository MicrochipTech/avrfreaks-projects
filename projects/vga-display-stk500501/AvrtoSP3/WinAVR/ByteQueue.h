//***************************************************************
// ByteQueue.h
//
//***************************************************************

#ifndef BYTEQUEUE__H
#define BYTEQUEUE__H

typedef struct {
	TCB *task_h;
	int nChar;
	unsigned Head;
	unsigned Tail;
	unsigned Size;
	char *pBfr;
}BQ;

extern void CreateBQ(BQ *pBQ,char *b,unsigned Size);
extern int BQget(BQ *pBQ);
extern int BQPut(BQ *pBQ,int c);
extern int BQWrite(BQ *pBQ,char *b,int n);

#endif

