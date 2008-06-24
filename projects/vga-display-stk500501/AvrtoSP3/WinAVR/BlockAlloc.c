//------------------------------------------------------------------------------
// We need a simple memory manager
// This module allocates and frees blocks of 256 bytes
// Created September 12, 2005
// By James Patchell
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "BlockAlloc.h"
#include "task.h"

ALLOC_QUEUE *Pool;
ECB *ABBlocker;

static void InsertList(ALLOC_LIST *l,ALLOC_BLOCK *m)
{
	//**************************************************************************
	// insert a memory block into a queue
	//
	// parameter:
	//	l.........List to insert memory block into
	//	m.........memory block to insert
	//**************************************************************************
	if(l->head == NULL)	//there is nothing in the list
	{
		l->head = m;				//start a queue
		l->end = &l->head->next;	//point to the end of the queue
	}
	else	//add to end of list
	{
		*l->end = m;		//add new buffer to end of queue
		l->end = &m->next;	//update end pointer
	}
	
}

static ALLOC_BLOCK *RemoveList(ALLOC_LIST *l)
{
	//**************************************************************************
	// Remove a block of memory from a queue
	//
	// parameter:
	//	l...........list to remove memory block from
	//
	// return value:
	//	returns a pointer to an ALLOC_BLOCK on success
	//	returns NULL on failure
	//**************************************************************************
	ALLOC_BLOCK *g = NULL;

	if(l->head)
	{
		g = l->head;
		l->head = l->head->next;
		if(l->head == NULL)
			l->end = NULL;	//list is empty again
		g->next = NULL;	//unlink this object completely
	}
	return g;
}

static ALLOC_QUEUE *InitAlloc(char *b,int size,int blocks)
{
	//**************************************************************************
	// This function initializes a block of memory to use as a set of
	// dynamically allocated memory sections.
	// It should be noted that the size of "b" should be as follows:
	//	(sizeof(b)) = size * blocks + sizeof(ALLOC_BLOCK) * blocks + sizeof(ALLOC_QUEUE)
	//
	// parameter:
	//	b........pointer to a block of ram to set up
	//	size.....size of each block withing the block of ram
	//	blocks...number of blocks to create
	//
	// return value:
	//	returns a pointer to the Queue of memory blocks on success
	//	returns NULL on failure
	//**************************************************************************
	int i;
	ALLOC_QUEUE *a = (ALLOC_QUEUE *)b;
	char *b1 = &b[sizeof(ALLOC_QUEUE)];
	ALLOC_BLOCK *m;
	
	a->free.head = NULL;
	a->free.end = NULL;
	for(i=0;i<blocks;++i)
	{
		m = (ALLOC_BLOCK *)b1;
		m->next = NULL;
		InsertList(&a->free,m);
		b1 += (size + sizeof(ALLOC_BLOCK));
	}
	ABBlocker = NewSemaphore(1,SEMAPHORE_MODE_BLOCKING,"AllocBlocker");
	return a;
}

static char *AllocBlock(ALLOC_QUEUE *a)
{
	//**************************************************************************
	// Allocate a block of memory
	//
	// parameter:
	//	a...........pointer to an ALLOC_QUEUE object to get memory block from
	//
	// return value:
	//	returns pointer to memory on success
	//	returns NULL on failure
	//**************************************************************************
	char *b = NULL;
	ALLOC_BLOCK *m;
	
	if((m = RemoveList(&a->free) ) != NULL)	//remove memory
	{
		b = (char *)m;
		b += sizeof(ALLOC_BLOCK);
	}
	else
		printf("***ERROR*** No Free Memory\n");
	return 	b;
}

static void FreeBlock(ALLOC_QUEUE *a,char *m)
{
	//**************************************************************************
	// Free a block of memory
	//
	// parameter:
	//	a..........pointer to an ALLOC_QUEUE object to return memory block to
	//	m..........pointer to the memory to return to the free queue
	//**************************************************************************
	ALLOC_BLOCK *b;

	b = (ALLOC_BLOCK *)(m - sizeof(ALLOC_BLOCK));
	InsertList(&a->free,b);		//return memory
}

void DumpPool(void)
{
	//--------------------------------------------------------------------------
	// DEBUG FUNCTION
	// prints the list of pointers for the pool
	//--------------------------------------------------------------------------
	
	ALLOC_BLOCK *m;
	int i=0;

	PendSemaphore(ABBlocker,0);
	m = Pool->free.head;
//	Printf("------------------Pool----------------\n");
	while(m)
	{
//		Printf("%02d:P:%04x N:%04x\n",i,m,m->next);
		++i;
		m = m->next;
		if(i > 16)
		{
//			Printf("Pool Error\n");
			m = NULL;
		}
	}
//	Printf("--------------------------------------\n");
	PostSemaphore(ABBlocker,0);
}


void InitBlockAlloc(int size,int blocks)
{
	//**************************************************************************
	// Initailize Block Memory Allocation Routine
	// parameters:
	//	size..........size of desired memory block
	//	blocks........total number of blocks to have in pool
	//
	// It should be noted that you must return blocks to the pool, otherwise
	// you will end up running out of memory blocks
	//**************************************************************************
	char *m = (char *)malloc(size * blocks + sizeof(ALLOC_BLOCK) * blocks + sizeof(ALLOC_QUEUE));
	Pool = InitAlloc(m,size,blocks);
}

char *NewBlock(void)
{
	//**************************************************************************
	// Get a Block of memory
	// This is what you should call in order to get your memory request fullfilled
	//
	// return value:
	//	returns pointer to memory on success
	//	returns NULL on fail
	//**************************************************************************
	char *pB;

	PendSemaphore(ABBlocker,0);
	pB = AllocBlock(Pool);
	PostSemaphore(ABBlocker,0);
	return pB;
}

void DeleteBlock(char *m)
{
	//**************************************************************************
	// return a block of memory back to the block memory pool
	// parameters:
	//	m..........pointer to the block of memory
	//
	// It should be noted that the memory you returns should be one that was
	// allocated by NewBlock()...and it had better be the same value as when
	// you got it.
	//**************************************************************************

	PendSemaphore(ABBlocker,0);
	FreeBlock(Pool,m);
	PostSemaphore(ABBlocker,0);
}
