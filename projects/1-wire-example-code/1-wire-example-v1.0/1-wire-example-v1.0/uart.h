#ifndef __uart__header__
#define __uart__header__

#include <stdio.h>
#include <stdlib.h>

void initUart(void);
int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);
int uart_getword(char *buf, int max);
#endif // ! __uart__header__
