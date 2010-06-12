#ifndef TX_H
#define TX_H


#include <avr/io.h>
#include <util/delay.h>

//define cpu clock speed if not defined
#ifndef F_CPU
#define F_CPU     8000000  //8 MHz external crystal
#endif
#define BAUDRATE  1200  //set desired baud rate
#define UBRRVAL   ((F_CPU/(BAUDRATE*16L))-1)  //calculate UBRR value
//define receive parameters
#define SYNC      0XAA  // synchro signal
#define RADDR     0x44  //address


//function definitions
void USART_init();
void send_tx_byte(uint8_t data);
void send_packet(uint8_t addr, uint8_t cmd);


#endif
