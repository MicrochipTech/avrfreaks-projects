/* 
 * UART on Atmel MCU
 * 
 * V2.2  
 * 
 * By: Lars Ivar Miljeteig, September 2010
 * Platform: AVR ATmega168 (avr-gcc)
 * Dependencies: avr/io delay.h stdint.h
 *
 * Reading and writing data by UART
 */


#ifndef _UART_H
#define _UART_H

#define BAUD 19200
#define MYUBRR F_CPU/16/BAUD-1



//extern FILE * uart_0;

//extern volatile char new_data_0;
//volatile char new_data_available_0;


uint8_t uart_putc(uint8_t data);//, FILE* stream);
uint8_t uart_getc();
void uart_writestring(char *string);
void uart_writechar(char c);

void uart_init(void);



#endif //_UART_H
