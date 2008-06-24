//----------------------------------------------------------
//
// Priority Queue Class
//
// The elements of the Q consist of an Array of pointers
// To the various elements of the Q
// No real data actually exists in this thing.
//
//----------------------------------------------------------

#ifndef PQ__H
#define PQ__H

typedef struct pq {
	int (*cmp)(void **,void **);		/*	compare two objects		*/
	int nitems;							/*	number of items in heap		*/
	int maxitems;						/*	maximum number of items in heap	*/
	void **bottom;						/*	prt to most recently added item	*/
	void **heap;						/*	pointer to the heap itself		*/
}PQ;

extern void swap(void **,void **);
extern void ReheapUp(PQ* q);
extern void ReheapDown(PQ* q);
extern void InitPQ(PQ *q,int ne,int (*c)(void **,void **));
extern int Insert(PQ* q,void *item);
extern int Delete(PQ* q,void **target);
extern void *Get(PQ* q);	//return pointer to highest priority item
extern int NumElem(PQ* q);	//get total number of items
extern int Replace(PQ* q,void **target,void *item);
extern int Remove(PQ* q,void **target,int (*cmp)(void **,void **),void *item);
extern void *GetI(PQ* q,int i);	//return pointer to indexed item

#endif
