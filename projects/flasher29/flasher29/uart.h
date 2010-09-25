void uart_init(void (*uart_callback)(uint16_t len, uint8_t *buf));
void putch(char c);
void set_limit(uint8_t lim, char term);
void print_adr(uint32_t adr);


