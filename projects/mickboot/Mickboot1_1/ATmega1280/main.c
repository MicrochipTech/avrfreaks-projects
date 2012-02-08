/*

*/

#define TIMEOUT_ZPROMPT		30
#define TIMEOUT_HEX_DATA	60

#include <includes.h>


// **** External prototypes ****

// **** Local prototypes ****

// **** External variables ****

	extern struct fifo_control uart_fifo;
	extern union hexline_union hexline;

// **** Global variables ****

	void (*mainapp)(void)=NULL;


// Watchdog fix for 1281/1280
void get_mcusr(void) \
      __attribute__((naked)) \
      __attribute__((section(".init3")));
void get_mcusr(void)
{
	MCUSR = 0;
    wdt_disable();
}
	
// Main application routine.
int main(void)
{
	int timeout;
	char *char_ptr, failed, finished;
	
	if(uart_init())	// detect ' ' on com 3,2,1 or 0, then configure that port
	{
		MCUCR = _BV(IVCE);
		MCUCR = _BV(IVSEL);
		
		sei();
		do
		{
			timeout=TIMEOUT_ZPROMPT/0.010;
			UART_TX("\r\nMICKBOOT V1.1 Uart");
			
			if(UCSR3B & _BV(RXEN3))
				UART_TX("3");
			else if(UCSR2B & _BV(RXEN2))
				UART_TX("2");
			else if(UCSR1B & _BV(RXEN1))
				UART_TX("1");
			else
				UART_TX("0");
			
			UART_TX("\r\nSend Z\r\n");
			
			finished=FALSE;
			failed=FALSE;
			do
			{
				if(uart_fifo_rx.bytes_used)
				{
					timeout=TIMEOUT_ZPROMPT/0.010;
					if(fifo_read_char()=='Z')
					{
						UART_TX("\r\nSend hex file in ASCII\r\n");
						timeout=TIMEOUT_HEX_DATA/0.010;
						failed=FALSE;
						finished=FALSE;
						ihex_init();
						do
						{
							if(uart_fifo_rx.bytes_used)
							{
								timeout=TIMEOUT_HEX_DATA/0.010;
								ihex_char(fifo_read_char());
								if(ihex_byte_count)
								{
									char_ptr = ihex_data;
									while(ihex_byte_count--)
										flash_byte(ihex_address++, *char_ptr++);
									ihex_byte_count=0;
								};
								if(ihex_status==IHEX_STATUS_EOF)
								{
									flash_finish();
									if(!writer_failed)
									{
										UART_TX("Success!\r\n");
										finished=TRUE;
									};							
								};
							}
							else
							{
								timeout--;
								_delay_ms(10);
							};
							
							if(!timeout)	//if timeout looking for hex data, repeat Z prompt
							{
								UART_TX("Timeout waiting for hex data\r\n");
								failed=TRUE;
							};
							
							if(ihex_status == IHEX_STATUS_ERROR)
							{
								UART_TX("Hex file ERROR!\r\n");
								failed=TRUE;
							};
							
						}while(!failed && !finished);
					};
				}
				else
				{
					timeout--;
					_delay_ms(10);
					if(!timeout)	//if timeout looking for Z, finish and run main app
					{
						finished=TRUE;
						UART_TX("Timeout waiting for Z\r\n");
					};
				};
				
			//look for Z while not failed and not finished (a failure must repeat z prompt)
			}while(!failed && !finished);
			
		//repeat Z prompt until finished
		}while(!finished);
		
		UART_TX("RUNNING\r\n");
		_delay_ms(500);
		cli();
		MCUCR = _BV(IVCE);
		MCUCR = 0;
		wdt_enable(WDTO_15MS);
		while(1);
		//once finished run main app
	};
	
	(*mainapp)();

	return 0;
}

