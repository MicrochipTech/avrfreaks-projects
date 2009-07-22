#ifndef USART_H
#define USART_H

#ifndef F_CPU
#define F_CPU 32000000
#endif

void usart_init(PORT_t *port, USART_t *usart, uint32_t baudrate);
void usart_rx_buf_flush(void);
uint8_t usart_rx_buf_has_data(void);
void usart_rx_buf_put_byte(uint8_t data);
uint8_t usart_rx_buf_get_byte(void);

void usart_send_byte(uint8_t byte);


#endif
