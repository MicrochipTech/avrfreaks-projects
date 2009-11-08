#ifndef UART_H_
#define UART_H_

// baudrate=12: Baud rate =  9600bps for 1MHZ Clk,U2X=1
// baudrate= 8: Baud rate = 14400bps for 1MHZ Clk,U2X=1
// baudrate=12: Baud rate = 19200bps for 2MHZ Clk,U2X=1
// baudrate= 8: Baud rate = 28800bps for 2MHZ Clk,U2X=1
// baudrate=12: Baud rate = 38400bps for 4MHZ Clk,U2X=1
// baudrate= 8: Baud rate = 57600bps for 4MHZ Clk,U2X=1
// baudrate= 8: Baud rate = 115200bps for 8MHZ Clk,U2X=1
void USART_Init(unsigned int baudrate);

char Usart_Rx(void);
void Usart_Tx(char);

#endif
