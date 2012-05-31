# AVR SimpleUART V2

Uploaded by Kun.io on 2012-05-31 16:24:00 (rating 0 out of 5)

## Summary

This is an update to project "AVR SimpleUART - buffered/interrupt driven serial".


It is a simple interrupt driven UART program.  

The function UART\_gets() has an autodetect for CR, LF, CRLF.


The functions are:  

void UART\_init(void); // initialize UART0  

void UART\_putc(unsigned char c); // Put ASCII or non-ASCII byte  

void UART\_puts(char *p); // Print string from RAM  

void UART\_puts\_P(const char *p); // Print string from FLASH MEMORY  

unsigned char UART\_getc(void); // Get ASCII or non-ASCII byte  

int UART\_gets(char *p, int max); // Get a string.  

unsigned char UART\_tbuflen(void); // Get number untransmitted bytes.  

unsigned char UART\_rbuflen(void); // Get number receive buffer.


Quickstart:


(1) The baudrate has to be set. Either calculate UBBR\_VALUE in uart.h, or include a file with F\_CPU in uart.c  

See uart.h for explanation about calculating the UBBR\_VALUE.


(2) Add uart.c to the compiler in your project.


(3) Call UART\_init(); and call UART\_puts("Hello World\r\n");


V2\_1: Parameter of UART\_puts\_P() has now the keword "const", and defines are added to make it compatible for the AT90USB1286 of the Teensy Board. Thanks Simon !

## Compilers

- WinAVR (GNU GCC)

## Tags

- Part-specific Lib.functions
- WinAVR (GNU GCC)
