#ifndef UART_H
#define UART_H

#define USART_BAUDRATE 9600 
#define UART_UBRR (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

void inicializar_uart(void);
void escribir_uart(char *s);
void escribir_uart_P(const char *s);
void caracter_uart(char car);
void leer_uart(char *serial);
void leer_socket(char *accion, char *velocidad);

#endif
