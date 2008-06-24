//----------------------------------------------------------
//
// Priority Queue Class
//
// The elements of the Q consist of an Array of pointers
// To the various elements of the Q
// No real data actually exists in this thing.
//
//
//----------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pq.h"


//---------------------------------------------------------
//
// PQ constructor
//
// int ne;	//maximum number of elements in the priority queue
// int (*c)(void **,void**);	//compare function.  The compare
//								//function,when called is passed
//								//a pair of pointers to pointers
//								//to the objects to be compared
// The constructor initializes all of the other parameters from
// these.
//
//---------------------------------------------------------
//
void InitPQ(PQ *q,int ne,int (*c)(void **,void **))
{
	q->cmp = c;
	q->nitems = 0;
	q->maxitems = ne;
	q->heap = (void **)malloc(sizeof(void *) * ne);
    q->bottom = &q->heap[-1];
}


//---------------------------------------------------------
//
// Insert
//
// This function puts a new object pointer into the heap
// if space is availiable.  It then reheaps
//
//---------------------------------------------------------
//
int Insert(PQ* q,void *item)
{
	int space_avail;

	space_avail = q->maxitems - q->nitems;
	if((space_avail) > 0)
	{
		++q->nitems;
		*(++q->bottom) = item;
		ReheapUp(q);
	}
	return space_avail;
}

//---------------------------------------------------------
//
// Delete
//
// This function deletes the highest proiority item from
// the heap.  It stores the pointer to the object in the
// location pointed to by target.  It then reheaps
//
//---------------------------------------------------------

int Delete(PQ* q,void **target)
{
	int SlotsInUse;

	if((SlotsInUse = q->nitems) > 0)
	{
		*target = *q->heap;		//get item from top of heap
		*q->heap = *q->bottom--;
		--q->nitems;
		ReheapDown(q);
	}
	return SlotsInUse;
}

//-----------------------------------------------------------
//
// Replace
//
// This function removes the hightest priority item from the
// heap and puts it in the location that is pointed to by
// target.  It then replaces it with the object pointed to
// by item.  Then the function reheaps.
//
//-----------------------------------------------------------

int Replace(PQ* q,void **target,void *item)
{
	int SlotsInUse;

	if((SlotsInUse = q->nitems) > 0)
	{
		if ((*q->cmp)(&item,q->heap) > 0)	//only replace if priority is less
		{
			*target = item;		//item is higher priority
		}
		else
		{
			*target = *q->heap;
			*q->heap = item;
			ReheapDown(q);
		}
	}
	else
		*target = item;
	return SlotsInUse;
}

//------------------------------------------------------------
//
// Remove
//
// This function removes the object pointed to by item from the
// heap and puts it into the location pointed to by target.
// Then the function reheaps
//
//------------------------------------------------------------
//
int Remove(PQ* q,void **target,int (*cmp)(void **,void **),void *item)
{
	int SlotsInUse;

	if((SlotsInUse = q->nitems)>0)
	{
		int i;
		int loop;

		for(i=0,loop=1;i<q->nitems && loop;++i)
		{
			if( (*cmp)(&item,&q->heap[i]) == 0)
			{
				*target = q->heap[i];
				memcpy(&q->heap[i],&q->heap[i+1],(q->nitems - i - 1) * sizeof(void *));
				q->bottom--;
				q->nitems--;
				ReheapDown(q);
				loop = 0;
			}
		}
	}
	return SlotsInUse;
}

/************************************************************************
**
** These functions are private to the class and are not accessable
**
************************************************************************/

void swap(void **s1,void **s2)
{
	void *temp;

	temp = *s1;
	*s1 = *s2;
	*s2 = temp;
}

void ReheapUp(PQ* q)
{
	int parent;		//index of parent
	int child;		//index of child
	void **pparent;	//pointer to parent
	void **pchild;	//pointer to child

	child = q->nitems - 1;
	while((parent = (child - 1)/2) >= 0)
	{
		pchild = &q->heap[child];
		pparent = &q->heap[parent];
		if( (*q->cmp)(pparent,pchild) >= 0)
			break;
		swap(pparent,pchild);
		child = parent;
	}	//end of while statement
}

void ReheapDown(PQ* q)
{
	int parent;	//index of parent
	int child;	//index of child
	void **pparent;	//pointer to parent
	void **pchild;   //pointer to child
	void **psibling;	//pointer to sibling
	void **pheap;	//pointer to heap

	pheap = q->heap;
	for(parent=0,child=1;child < q->nitems;)
	{
		pparent = &pheap[parent];
		pchild = &pheap[child];
		if(child + 1 < q->nitems)
		{
			psibling = pchild + 1;
			if((*q->cmp)(pchild,psibling) < 0)
			{
				pchild = psibling;
				child++;
			}
		}	//end of if(child + 1 < nitems)
		if((*q->cmp)(pparent,pchild) >= 0)
			break;
		swap(pparent,pchild);
		parent = child;
		child = parent * 2 + 1;
	}	//end of for loop
}

void *Get(PQ* q)
{
	return q->heap[0];
}

int NumElem(PQ* q)
{
	return q->nitems;
}

void *GetI(PQ* q,int i)
{
	return q->heap[i];
}
