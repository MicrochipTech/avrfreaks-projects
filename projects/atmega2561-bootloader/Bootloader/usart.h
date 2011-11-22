#ifndef USART_H
#define USART_H

typedef struct usart_data {
  unsigned char data;
} usart_data_t;

void usart0_init(unsigned char baud);
unsigned char usart0_rx_byte();
void usart0_tx_byte(unsigned char byte);
void usart0_tx_app_info();
void usart0_tx_int(unsigned int data);
void usart0_tx_str(char *data, unsigned char len);
void usart0_tx_long(unsigned long data);
void process_usart_queue();
void usart0_tx_block(void *block, unsigned int size);

#endif
