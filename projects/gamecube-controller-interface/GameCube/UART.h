// v1.0
// 4/06 stevech@san.rr.com

#define UBBRL_VALUE 51	// 10MHz CPU, 19.2Kbaud serial. See AVRcalc.exe

/* Buffersizes must be a power of 2 in size, for speed */
#define TBUFSIZE	64
#define RBUFSIZE	64

// functions in UART.c

void 			UART_init(void);				// initialize 

void 			UART_putc(unsigned char c);  	// put ASCII or non-ASCII byte, blocks if buffer full
unsigned char 	UART_getc(void);				// get ASCII or non-ASCII byte, 
												//      blocks if none available. See UART_rbuflen()

int 			UART_gets(char *p, int max);	// Get string that ends with a \r  but subject to max chars
void 			UART_puts(char *p);				// print string from RAM
void 			UART_puts_P(char *p);			// print string from FLASH MEMORY

unsigned char 	UART_tbuflen(void);				// get number of as yet untransmitted bytes
unsigned char 	UART_rbuflen(void);				// get number of bytes in the receive buffer or zero

// buffered I/O if fdevopen() is to be used. 
// Matches AVR library as of 4/06
// put the next two lines in main()
//    	fdevopen( UART_fputchar, UART_fgetchar );			// establish buffered I/O callbacks
//  	sei();												// Global interrupt enable
int 			UART_fgetc(FILE *);
int 			UART_fputc(char, FILE *);
