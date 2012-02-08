#include<includes.h>

#define FLASH_PAGE_SIZE 256
#define FLASH_PAGE_MASK 0xFFFFFF00ul

	char writer_failed=FALSE;
	
	unsigned long current_page=0xFFFFFFFFul;
	char buffer_verify[FLASH_PAGE_SIZE];

//	void flash_word(unsigned long address, int word);
	char memcmp_farP(void* ram_ptr, unsigned long progmem_add, unsigned int size);
	void write_page(void);

// call with bytes, builds pages and writes pages to flash.
void flash_byte(unsigned long address, char byte)
{
	char tempbuf[10];

	static char write_needed=FALSE;
	int tempint=0;
	unsigned long progmem_add;

	//new page?
	if((address & FLASH_PAGE_MASK) != current_page)
	{
		//write previous page (if there is one)
		if(write_needed)
			write_page();
		
		current_page = address & FLASH_PAGE_MASK;
		
		UART_TX("PAGE ");
		UART_TX(ltoa(current_page, tempbuf, 16));
		UART_TX("\r\n");
		
		//read new page into buffer
		progmem_add = current_page;
		tempint=0;
		while(tempint!=FLASH_PAGE_SIZE)
			buffer_verify[tempint++]=pgm_read_byte_far(progmem_add++);
	};

	buffer_verify[address&0xFF]=byte;
//	buffer_verify[(address+1)&0xFF]=(char)(word>>8);
	write_needed=TRUE;
}

void flash_finish(void)
{
	write_page();
}

char memcmp_farP(void* ram_ptr, unsigned long progmem_add, unsigned int size)
{
	char retval=0;

	while(size--)
	{
		if( (*(char*)ram_ptr++) != pgm_read_byte_far(progmem_add++) )
		{
			retval=1;
			size=0;
		};
	};
	return retval;
}

void write_page(void)
{		
	unsigned char retry=10, verified=FALSE;
	unsigned long progmem_add;
	unsigned int tempint, tempint2;
	
	do
	{
		//fill buffer
		progmem_add = current_page;
		tempint=0;
		while(tempint!=FLASH_PAGE_SIZE)
		{
			tempint2=buffer_verify[tempint+1];
			tempint2<<=8;
			tempint2+=buffer_verify[tempint];
			ATOMIC_BLOCK(ATOMIC_FORCEON)
			{
				boot_page_fill(progmem_add, tempint2);	
			}
			progmem_add+=2;
			tempint+=2;
		};
		
		ATOMIC_BLOCK(ATOMIC_FORCEON)
		{
			boot_page_erase(current_page);
		}
		boot_spm_busy_wait();      // Wait until the memory is erased.
		ATOMIC_BLOCK(ATOMIC_FORCEON)
		{
			boot_page_write(current_page);
		}
		boot_spm_busy_wait();      // Wait until the memory is programmed.
		
		progmem_add = current_page;
		boot_rww_enable();
		
		if( memcmp_farP(buffer_verify, progmem_add, FLASH_PAGE_SIZE) )
		{
			UART_TX("Retrying\r\n");
			retry--;
		}
		else
			verified=TRUE;
			
	}while(retry && !verified);
	
	if(!verified)
		writer_failed=TRUE;
}
