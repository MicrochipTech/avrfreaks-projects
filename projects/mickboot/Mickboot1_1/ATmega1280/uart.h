
//time in mS after reset we look for a space
#define BOOTWINDOW 200

#define UART0BAUD 9600
#define UART1BAUD 9600
#define UART2BAUD 9600
#define UART3BAUD 9600

#define UART_TX(arg1)	uart_write_string(arg1)
char uart_init(void);
void uart_write_string(char*string);

	extern struct fifo_control uart_fifo_tx;
	extern struct fifo_control uart_fifo_rx;

