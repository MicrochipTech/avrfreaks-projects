#include "m_interrupts.h"
#include "definitions.h"
#include "usart.h"
#include "cycle_counter.h"


// =============== FUNCTIONS ===================
void enable_usart_interrupts(volatile avr32_usart_t *usart, int enableTX, int enableRX)
{
	if(enableTX && enableRX)
	{
		usart->ier = 0x00000003;
		return;
	}
	
	if(enableRX)
	{
		usart->ier = 0x00000001;
		return;
	}
	
	if(enableTX)
	{
		usart->ier = 0x00000002;
		return;
	}
	
	if(!enableTX && !enableRX)
	{
		usart->ier = 0x00000000;
		return;
	}
}


// ==================== ISRs =====================
__attribute__((__interrupt__)) void uart_interrupt(void)
{
	    char dataReceived = (EXAMPLE_USART->rhr);
		
		// check for end of sentence condition
		if((dataReceived == 0x0D) && (CurrentPosition.recordData))
		{
			CurrentPosition.recordData = FALSE;	
			CurrentPosition.parseGPSBuff = TRUE;
			return;
		}
		
		// check for start of sentence condition
		if(dataReceived == '$')
		{
			CurrentPosition.recordData = TRUE;
			CurrentPosition.index = 0;
			return;
		}
		
		// just add byte to buffer
		if(CurrentPosition.recordData)
		{
			if(CurrentPosition.index < gps_buff_size)
			{
				CurrentPosition.gpsBuff[CurrentPosition.index] = dataReceived;
				CurrentPosition.index++;
			}
			return;	
		}
}

__attribute__((__interrupt__)) void compare_irq_handler(void)
{
  TimeOut = 1;
  // Disable the compare and exception generation feature: set COMPARE to 0.
  Set_sys_compare(0);
}
