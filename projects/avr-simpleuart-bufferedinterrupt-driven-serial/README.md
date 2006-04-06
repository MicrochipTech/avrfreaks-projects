# AVR SimpleUART - buffered/interrupt driven serial

Uploaded by stevech on 2006-04-06 02:21:00 (rating 0 out of 5)

## Summary

// v1.0  

// 4/06 [stevech@san.rr.com](mailto:stevech@san.rr.com)  

Tested with mega8 at 10MHz.  

Should be compiler-independent. Developed with Atman IDE for GCC.


HERE IS THE .H FILE


#define UBBRL\_VALUE 31 // 10MHz CPU, 19.2Kbaud serial. See AVRcalc.exe


/* Buffersizes must be a power of 2 in size, for speed */  

#define TBUFSIZE 64  

#define RBUFSIZE 64


// functions in UART.c


void UART\_init(void); // initialize 


void UART\_putc(unsigned char c); // put ASCII or non-ASCII byte, blocks if buffer full  

unsigned char UART\_getc(void); // get ASCII or non-ASCII byte,  

 // blocks if none available. See UART\_rbuflen()


int UART\_gets(char *p, int max); // Get string that ends with a \r but subject to max chars  

void UART\_puts(char *p); // print string from RAM  

void UART\_puts\_P(char *p); // print string from FLASH MEMORY


unsigned char UART\_tbuflen(void); // get number of as yet untransmitted bytes  

unsigned char UART\_rbuflen(void); // get number of bytes in the receive buffer or zero


// buffered I/O if fdevopen() is to be used.  

// Matches AVR library as of 4/06  

// put the next two lines in main()  

// fdevopen( UART\_fputchar, UART\_fgetchar ); // establish buffered I/O callbacks  

// sei(); // Global interrupt enable  

int UART\_fgetc(FILE *);  

int UART\_fputc(char, FILE *);

## Compilers

- AtmanAvr C/C++ IDE

## Tags

- Complete code
- AtmanAvr C/C++ IDE
