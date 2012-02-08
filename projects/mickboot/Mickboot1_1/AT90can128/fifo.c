#include <includes.h>

char fifo_read_char(void)
{
	char retval=0;
	
	while(uart_fifo_rx.bytes_used==0);

	retval=*((char*)uart_fifo_rx.tail_ptr);
	uart_fifo_rx.tail_ptr++;	
	
	if(uart_fifo_rx.tail_ptr == uart_fifo_rx.end)
		uart_fifo_rx.tail_ptr = uart_fifo_rx.start;

	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		uart_fifo_rx.bytes_free ++;
		uart_fifo_rx.bytes_used --;
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
	}
}
