#include "includes.h"

//move data from source to destination, either filling destination, or emptying source
//note that void(*post_fptr)(void) may be called twice.
void fifo_copy(struct fifo_control *target, struct fifo_control *obj)
{
	unsigned int size_read, tempint, tempint2;

	if(target ==NULL && obj !=NULL)
		fifo_flush(obj);
	else if (target !=NULL && obj !=NULL)
	{
	
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			tempint = obj->bytes_used;
			tempint2= target->bytes_free;
		}
		if(tempint2 < tempint)
			size_read = tempint2;	//fill target
		else
			size_read = tempint;	//else empty source (or both)

//	this next code is basically a copy of the read fuction, except instead of memcpy to a buffer, we call fifo_write to the target

		// do we need to read twice?
		tempint = obj->end - obj->tail_ptr;
		if( tempint < size_read)
		{
			fifo_write(target, obj->tail_ptr, tempint);
			size_read-=tempint;
			obj->tail_ptr = obj->start;
		}
		else
			tempint=0;
	
		if(size_read)
			fifo_write(target, obj->tail_ptr, size_read);
		obj->tail_ptr += size_read;
	
		if(obj->tail_ptr == obj->end)
			obj->tail_ptr = obj->start;
	
		size_read+=tempint;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			obj->bytes_free += size_read;
			obj->bytes_used -= size_read;
			if(obj->bytes_used == 0)
				obj->empty=TRUE;
			if(obj->bytes_free != 0)
				obj->full=FALSE;
		}
	};
}

char fifo_read_char(struct fifo_control *obj)
{
	char retval=0;
	
	while(obj->empty)
	{
		if(obj->wait_fptr)
			obj->wait_fptr();
	};

	retval=*((char*)obj->tail_ptr);
	obj->tail_ptr++;	
	
	if(obj->tail_ptr == obj->end)
		obj->tail_ptr = obj->start;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		obj->bytes_free ++;
		obj->bytes_used --;
		if(obj->bytes_used == 0)
			obj->empty=TRUE;
		if(obj->bytes_free != 0)
			obj->full=FALSE;
	}

	return retval;
}

int fifo_read_int(struct fifo_control *obj)
{
	int retval;
	fifo_read(obj, &retval, sizeof(int));
	return retval;
}

long fifo_read_long(struct fifo_control *obj)
{
	long retval;
	fifo_read(obj, &retval, sizeof(long));
	return retval;
}

//read up to and including terminator, limited by maxsize
char* fifo_read_string(struct fifo_control *obj, char *target, int maxsize)
{
	char *retval;
	retval=target;
	if(maxsize)
		do
		{
			while(obj->empty)
			{
				if(obj->wait_fptr)
					obj->wait_fptr();
			};
			*target = fifo_read_char(obj);
		}while(*(target++) && --maxsize);
	return retval;
}

void fifo_write_char(struct fifo_control *obj, char data)
{
	if(obj!=NULL)
	{
		while(obj->full)
		{
			if(obj->wait_fptr)
				obj->wait_fptr();
		};
	
		*((char*)obj->head_ptr) = data;
		obj->head_ptr++;
	
		if(obj->head_ptr == obj->end)
			obj->head_ptr = obj->start;
	
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			obj->bytes_free --;
			obj->bytes_used ++;
			if(obj->bytes_free == 0)
				obj->full=TRUE;
			if(obj->bytes_used != 0)
				obj->empty=FALSE;
		}
		if(obj->post_fptr)
				obj->post_fptr();
	};
}

void fifo_write_int(struct fifo_control *obj, int data)
{
	fifo_write(obj, &data, sizeof(int));
}

void fifo_write_long(struct fifo_control *obj, long data)
{
	fifo_write(obj, &data, sizeof(long));
}

void fifo_write_string(struct fifo_control *obj, char*string)
{
	fifo_write(obj, string, strlen(string));
}

void fifo_write_string_P(struct fifo_control *obj, PGM_P string)
{
	fifo_write_P(obj, string, strlen_P(string));
}

// (&control structure, size in bytes, post_write function pointer, wait function pointer, &data_space)
void fifo_init(struct fifo_control *obj, unsigned int size, void(*post_fptr)(void), void(*wait_fptr)(void), void *data_space)
{
	if(data_space == NULL)
	{
		if( (obj->start = malloc(size)) == NULL )
			while(1);
	}
	else
		obj->start = data_space;

	obj->end=obj->start + size;		// note, end points to last item in buffer + 1
	obj->post_fptr=post_fptr;
	obj->wait_fptr=wait_fptr;
	fifo_flush(obj);
} 

// read data from fifo
void* fifo_read(struct fifo_control *obj, void *item, unsigned int size_target)
{
	unsigned int tempint, size_read;
	unsigned char fragment=FALSE;

	if(obj->end - obj->start < size_target)
		fragment=TRUE;

	while(size_target)
	{
		if(fragment)
		{	//if fragmented read, wait for ANY amount of data
			while(obj->empty)
				if(obj->wait_fptr)
					obj->wait_fptr();
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				size_read = obj->bytes_used;
			}
			if(size_read > size_target)
				size_read=size_target;
		}
		else
		{
			//if non fragmented read, wait for data in buffer to be complete
			size_read = size_target;
			do
			{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
				{
					tempint = obj->bytes_used;
				}
				if(tempint < size_target && obj->wait_fptr)
					obj->wait_fptr();
			}while(tempint < size_target);
		};
		
		size_target-=size_read;
		// do we need to read twice?
		tempint = obj->end - obj->tail_ptr;
		if( tempint < size_read)
		{
			if(item!=NULL)
				memcpy(item, obj->tail_ptr, tempint);
			size_read-=tempint;
			obj->tail_ptr = obj->start;
			if(item!=NULL)
				item = (char*)item + tempint;
		}
		else
			tempint=0;
		
		if(size_read && item!=NULL)
			memcpy(item, obj->tail_ptr, size_read);
		obj->tail_ptr += size_read;
		if(item!=NULL)
			item = (char*)item + size_read;
		
		if(obj->tail_ptr == obj->end)
			obj->tail_ptr = obj->start;
		
		size_read+=tempint;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			obj->bytes_free += size_read;
			obj->bytes_used -= size_read;
			if(obj->bytes_used == 0)
				obj->empty=TRUE;
			if(obj->bytes_free != 0)
				obj->full=FALSE;
		}
		
	};

	return item;
}

// read data from fifo, without removing the data from the fifo
void* fifo_preview(struct fifo_control *obj, void *item, unsigned int size)
{
	unsigned int tempint=0;
	char* fake_tail;

	fake_tail = obj->tail_ptr;

	//wait for size bytes in buffer to preview
	do
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			tempint = obj->bytes_used;
		}
		if(tempint<size && obj->wait_fptr)
			obj->wait_fptr();
	}while(tempint<size);

	// do we need to read twice?
	tempint = obj->end - fake_tail;
	if( tempint < size )
	{
		if(item!=NULL)
			memcpy(item, fake_tail, tempint);
		size-=tempint;
		fake_tail = obj->start;
		item = (char*)item + tempint;
	};

	if(size && item!=NULL)
		memcpy(item, fake_tail, size);

	return item;
}

// write data to fifo
// If data size is <= buffer size, will wait for buffer to have enough room before writing
// otherwise will wait for buffer empty, then fill it, then repeat.

void fifo_write(struct fifo_control *obj, void *item, unsigned int size_source)
{
	unsigned int tempint, size_write;
	unsigned char fragment=FALSE;
	
	if(obj!=NULL)
	{
		//is remaining data to write bigger than the entire buffer?
		if(size_source > obj->end - obj->start)
			fragment=TRUE;

		while(size_source)
		{
			if(fragment)
			{	//if fragmented write, wait for ANY amount of space
				while(obj->full)
				{
					if(obj->wait_fptr)
						obj->wait_fptr();
				};
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
				{
					size_write = obj->bytes_free;
				}
				if(size_write > size_source)
					size_write=size_source;
			}			
			else
			{
				//if non fragmented write, wait for buffer space to fit remaining data
				size_write=size_source;
				do
				{
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
					{
						tempint = obj->bytes_free;
					}
					if(tempint < size_source && obj->wait_fptr)
						obj->wait_fptr();
				}while(tempint < size_source);
			}
	
			size_source-=size_write;
			
			//do we need to write twice?
			tempint = obj->end - obj->head_ptr;
			if(tempint < size_write)					//if space before end is < amonut to write
			{
				memcpy(obj->head_ptr, item, tempint);	//write up to end first
				size_write-=tempint;
				obj->head_ptr = obj->start;
				item = (char*)item + tempint;		
			}
			else
				tempint=0;
		
			if(size_write)
				memcpy(obj->head_ptr, item, size_write);
			obj->head_ptr += size_write;
			item = (char*)item + size_write;
		
			if(obj->head_ptr == obj->end)
				obj->head_ptr = obj->start;
		
			size_write+=tempint;
		
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				obj->bytes_free -= size_write;
				obj->bytes_used += size_write;
				if(obj->bytes_free == 0)
					obj->full=TRUE;
				if(obj->bytes_used != 0)
					obj->empty=FALSE;
			}
			if(obj->post_fptr)
				obj->post_fptr();
		};
	};
}

// write data to fifo
void fifo_write_P(struct fifo_control *obj, PGM_VOID_P item, unsigned int size_source)
{
	unsigned int tempint, size_write;
	unsigned char fragment=FALSE;

	if(obj!=NULL)
	{
		//is remaining data to write bigger than the entire buffer?
		if(size_source > obj->end - obj->start)
			fragment=TRUE;
	
		while(size_source)
		{
			if(fragment)
			{	//if fragmented write, wait for ANY amount of space
				while(obj->full)
				{
					if(obj->wait_fptr)
						obj->wait_fptr();
				};
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
				{
					size_write = obj->bytes_free;
				}
				if(size_write > size_source)
					size_write=size_source;
			}			
			else
			{
				//if non fragmented write, wait for buffer space to fit remaining data
				size_write=size_source;
				do
				{
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
					{
						tempint = obj->bytes_free;
					}
					if(tempint < size_source && obj->wait_fptr)
						obj->wait_fptr();
				}while(tempint < size_source);
			}
			
			size_source-=size_write;
			
			//do we need to write twice?
			tempint = obj->end - obj->head_ptr;
			if(tempint < size_write)					//if space before end is < amonut to write
			{
				memcpy_P(obj->head_ptr, item, tempint);	//write up to end first
				size_write-=tempint;
				obj->head_ptr = obj->start;
				item = (char*)item + tempint;		
			}
			else
				tempint=0;
			
			if(size_write)
				memcpy_P(obj->head_ptr, item, size_write);
			obj->head_ptr += size_write;
			item = (char*)item + size_write;
			
			if(obj->head_ptr == obj->end)
				obj->head_ptr = obj->start;
			
			size_write+=tempint;
			
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				obj->bytes_free -= size_write;
				obj->bytes_used += size_write;
				if(obj->bytes_free == 0)
					obj->full=TRUE;
				if(obj->bytes_used != 0)
					obj->empty=FALSE;
			}
			
			if(obj->post_fptr)
				obj->post_fptr();
		};
	};
}

//100506, made atomic so pointers cannot be corrupted by ISR's
void fifo_flush(struct fifo_control *obj)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		obj->tail_ptr = obj->start;
		obj->head_ptr = obj->start;
		obj->bytes_free=obj->end-obj->start;
		obj->bytes_used=0;
		obj->empty=TRUE;
		obj->full=FALSE;
	}
}
