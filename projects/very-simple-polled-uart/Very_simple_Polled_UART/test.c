#include <io.h>
#include "uart.h"


int main
	(
	void
	)
	{
	char ch;
	init_uart0();
	
	output_ch_0('H');
	if (input_ch_w_timeout_0(&ch, 2000) == TIMEOUT)
		{
		output_ch_0('T');
		}
	else
		{
		output_ch_0(ch);
		}
		
	while (1)
		{
		output_ch_0(input_ch_0());
		}
		
	return 1;
	}
	
	