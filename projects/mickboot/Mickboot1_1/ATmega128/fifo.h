//copied from cliftlib 100506, trimmed for bootloader
/*
todo, prevent buffer underflow while reading

Functions for handling fifo buffer.
Atomic blocks allow writing or reading buffers in ISR's, however:
	you cannot write to the same buffer from both ISR's and main code (this will corrupt the head pointer)
	you cannot read from the same buffer with both ISR's and main code (this will corrupt the tail pointer)

Functions are passed a pointer to a wait function 'void (*wait_fptr)(void)' , if this pointer is passed as NULL
then the function will simply loop while waiting, otherwise it will call the pointer repeatedly while waiting.
The function pointer can be used for releasing the thread in a cooperative multi-thread system, or can be used for error trapping buffer underflow/overflow events.

While writing,
if the source data is larger than the buffer size, the write function will fragment the write, waiting while the buffer is full,
otherwise it will wait until the buffer has room to write the source data in one call.

NOTE, there is a potential deadlock situation,
	thread1 is waiting for fifo to have X bytes free before writing X bytes
	thread2 is waiting for fifo to have X bytes in it before reading X bytes

*/

	struct fifo_control
    {
		volatile unsigned char bytes_free;	// ensure atomic access to these
		volatile unsigned char bytes_used;
		char* volatile head_ptr;
		char* volatile tail_ptr;
		char* volatile start;
		char* volatile end;
		void(*post_fptr)(void);		// post write function pointer, called after a write if not NULL, may be used to enable uart ISR's
    };

//void* fifo_read(struct fifo_control*, void* item, unsigned int size);
void fifo_write(struct fifo_control*, void* item, unsigned int size);
void fifo_write_P(struct fifo_control*, PGM_VOID_P, unsigned int size);

void fifo_init(struct fifo_control *obj, unsigned int size, void(*post_fptr)(void), void* data_space);

void fifo_flush(struct fifo_control *);

char fifo_read_char(void);
void fifo_write_char(struct fifo_control *, char);

void fifo_write_string(char*string);
