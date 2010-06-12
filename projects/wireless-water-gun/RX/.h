#include <avr/io.h>
#include <util/delay.h>

//define cpu clock speed if not defined
#ifndef F_CPU
#define F_CPU 4000000  //4MHz external crystal
#endif
#define BAUDRATE 1200  //set desired baud rate
#define UBRRVAL ((F_CPU/(BAUDRATE*16L))-1)  //calculate UBRR value
//define receive parameters
#define SYNC 0XAA  // synchro signal
#define RADDR 0x44  //address



//function definitions
void USART_init();
uint8_t get_rx_byte();
signed int receive_packet();
