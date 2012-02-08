/*

*/

#define TIMEOUT_ZPROMPT		15
#define TIMEOUT_HEX_DATA	60

#include <includes.h>


// **** External prototypes ****

// **** Local prototypes ****

// **** External variables ****

	extern struct fifo_control uart_fifo;
	extern union hexline_union hexline;

// **** Global variables ****

	void (*mainapp)(void)=NULL;
	extern volatile unsigned char uart_select; //1=uart0 2=uart1
	
// Main application routine.
int main(void)
{
	int timeout;
	char *char_ptr, failed, finished;
	
	if(uart_init())	// detect ' ' on com 1 or 0, then configure that port
	{
		MCUCR = _BV(IVCE);
		MCUCR = _BV(IVSEL);
		
		sei();
		do
		{
			timeout=TIMEOUT_ZPROMPT/0.010;
			UART_TX("\r\nMICKBOOT V1.0 Uart");
			
			if(UCSR1B & _BV(RXEN1))
				UART_TX("1");
			else
				UART_TX("0");
			
			UART_TX("\r\nSend Z\r\n");
			
			finished=FALSE;
			failed=FALSE;
			do
			{
				if(uart_fifo_rx.empty==FALSE)
				{
					timeout=TIMEOUT_ZPROMPT/0.010;
					if(fifo_read_char(&uart_fifo_rx)=='Z')
					{
						UART_TX("\r\nSend hex file in ASCII\r\n");
						timeout=TIMEOUT_HEX_DATA/0.010;
						failed=FALSE;
						finished=FALSE;
						ihex_init();
						do
						{
							if(uart_fifo_rx.empty==FALSE)
							{
								timeout=TIMEOUT_HEX_DATA/0.010;
								ihex_char(fifo_read_char(&uart_fifo_rx));
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

