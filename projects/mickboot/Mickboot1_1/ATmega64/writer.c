#include<includes.h>

#define FLASH_PAGE_SIZE 256
#define FLASH_PAGE_MASK 0xFF00ul

	char writer_failed=FALSE;
	
	unsigned int current_page=0xFFFF;
	char buffer_verify[FLASH_PAGE_SIZE];

	char memcmp_farP(void* ram_ptr, unsigned long progmem_add, unsigned int size);
	void write_page(void);

// call with bytes, builds pages and writes pages to flash.
void flash_byte(unsigned long address, char byte)
{
	char tempbuf[10];

	static char write_needed=FALSE;
	int tempint=0;
	unsigned int progmem_add;

	//new page?
	if((address & FLASH_PAGE_MASK) != current_page)
	{
		//write previous page (if there is one)
		if(write_needed)
			write_page();
		
		current_page = (unsigned int)address & FLASH_PAGE_MASK;
		
		UART_TX("PAGE ");
		UART_TX(ltoa(current_page, tempbuf, 16));
		UART_TX("\r\n");
		
		//read new page into buffer
		progmem_add = current_page;
		tempint=0;
		while(tempint!=FLASH_PAGE_SIZE)
			buffer_verify[tempint++]=pgm_read_byte(progmem_add++);
	};

	buffer_verify[address&0xFF]=byte;

	write_needed=TRUE;
}

void flash_finish(void)
{
	write_page();
}

void write_page(void)
{		
	unsigned char retry=10, verified=FALSE;
	unsigned int progmem_add;
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
		
		if( memcmp_P(buffer_verify, (PGM_P)progmem_add, FLASH_PAGE_SIZE) )
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
