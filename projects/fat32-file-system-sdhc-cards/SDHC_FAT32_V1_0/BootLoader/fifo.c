#include <includes.h>

char fifo_read_char(struct fifo_control *obj)
{
	char retval=0;
	
	while((*obj).empty);

	retval=*((char*)(*obj).tail_ptr);
	(*obj).tail_ptr++;	
	
	if((*obj).tail_ptr == (*obj).end)
		(*obj).tail_ptr = (*obj).start;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		(*obj).bytes_free ++;
		(*obj).bytes_used --;
		if((*obj).bytes_used == 0)
			(*obj).empty=TRUE;
		if((*obj).bytes_free != 0)
			(*obj).full=FALSE;
	}

	return retval;
}



void fifo_init(struct fifo_control *obj, unsigned int size, void(*post_fptr)(void), void* data_space)
{
	(*obj).start = data_space;
	(*obj).end=(*obj).start + size;		// note, end points to last item in buffer + 1
	(*obj).post_fptr=post_fptr;
	fifo_flush(obj);
} 

//100506, made atomic so pointers cannot be corrupted by ISR's
void fifo_flush(struct fifo_control *obj)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		(*obj).tail_ptr = (*obj).start;
		(*obj).head_ptr = (*obj).start;
		(*obj).bytes_free=(*obj).end-(*obj).start;
		(*obj).bytes_used=0;
		(*obj).empty=TRUE;
		(*obj).full=FALSE;
	}
}
