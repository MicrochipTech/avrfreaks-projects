//------------------------------------------------------------------------------
// We need a simple memory manager
// This module allocates and frees blocks of 256 bytes
// Created September 12, 2005
// By James Patchell
//------------------------------------------------------------------------------

#ifndef BBALLOC__H
#define BBALLOC__H

#define ALLOC_MAXBLOCKS		16		//maximum of 16 blocks to allocate
#define MEMBLOCK_SIZE		640		//size of the memory block

typedef struct alloc_block {
	struct alloc_block *next;
}ALLOC_BLOCK;

typedef struct {
	ALLOC_BLOCK *head,**end;
}ALLOC_LIST;

typedef struct {
	ALLOC_LIST free;
}ALLOC_QUEUE;

extern ALLOC_QUEUE *Pool;

//extern ALLOC_QUEUE *InitAlloc(char *b,int size,int blocks);
extern void DumpPool(void);
extern void InitBlockAlloc(int size,int blocks);
extern char *NewBlock(void);
extern void DeleteBlock(char *);

#endif
