#ifndef __heheap_h__
#define __heheap_h__
// heap.h
typedef int heapindex ;
extern heapindex heapused ;		// total used blocks


extern uint ncached ;			// CACHED
								// before open: so much blocks may remain in memory
								// after open: so much blocks are kept in memory

extern uint nheapbuf ;			// HEAPBUFMAX
								// before open:



/* #define HEAPPERBL 1600 */
#if INTSIZE <= 16
#define CACHED 1		// How many blocks are always in memory
#define HEAPBUFMAX 2048 // limit for parameter nheapbuf -the size of each buffer
#define MAXHEAP 2048
#else
#define CACHED 12		// How many blocks are always in memory

#define HEAPBUFMAX 8192 // limit for parameter nheapbuf - the size of each buffer
#define MAXHEAP 8192	// total possible blocks, maximum
#endif



int openheapv( uint bufgr, uint cached ) ;	// open heap with variable size
int closeheap(void) ;						// close heap

int freezeheap(void) ;						// save heap to disk, free memory
int dewheap(void) ;

int flushheap(void) ;

int readheap( void *wohin,heapindex was) ;	// get from heap a block
int storeheap( void *was, heapindex wohin) ; // save a block from heap
int insertheap(heapindex wo,uint wieviel) ; // insert ffree space
int delheap(heapindex wo,uint wieviel) ;
heapindex heapfill(void) ;		/* report filled blocks */
heapindex heapempty(void) ;		/* report free blocks */
extern heapindex heapused ;		/* total used blocks */
#endif
