
extern char* itoa(int val, int base);

extern int ctoi(char inp);	// Convert a char to int, within 0..9 and A..Z

extern void print_ruler(volatile avr32_usart_t *usart);

extern void print_summary(volatile avr32_usart_t *usart);

extern void print_pulses(volatile avr32_usart_t *usart);

extern void print_types(volatile avr32_usart_t *usart);

extern void print_menu(volatile avr32_usart_t *usart, char type); 

void getkey(volatile avr32_usart_t *usart, int pos, unsigned int out[]); // Get pattern