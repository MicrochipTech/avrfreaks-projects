extern void uart_init(void);
extern void uart_puts(char *);
extern void uart_putc (const uint8_t);
extern int uart_getc_nowait(void);
extern uint8_t uart_getc_wait(void);
extern void Dump_Char(unsigned char x);
extern void Dump_Buffer(unsigned char *p);
