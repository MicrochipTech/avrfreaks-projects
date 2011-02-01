#include <avr/io.h>
#include "stdio.h"

void uart_putchar(char c);
char uart_getchar(void);
void uart_getstring(char *string);
void uart_putstring(char *string);