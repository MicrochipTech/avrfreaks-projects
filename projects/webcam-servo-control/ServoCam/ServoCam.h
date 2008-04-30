//
//    ServoCam.h
//    
//    R. Scott Coppersmith
//

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define UART_BAUD_REG              UBRRL
#define UART_BAUD_REG_H            UBRRH
#define UART_CONTROL_REG           UCSRB
#define UART_STATUS_REG            UCSRA   
#define UART_RCV_INT_VECTOR        SIG_UART_RECV
#define UART_REG_EMPTY_INT_VECTOR  SIG_UART_DATA
#define UART_DATA_REG              UDR
#define UDRIE 5

#define F_CPU	4000000UL	//8MHz CPU clock
#define FALSE 0
#define TRUE 1

/* Maps GCC-AVR  variables to standard data types  */
typedef unsigned char  BOOLEAN;
typedef unsigned char  CHARU;   /* Unsigned  8 bit quantity  */
typedef signed char    CHARS;   /* Signed    8 bit quantity  */
typedef unsigned int   INT16U;  /* Unsigned 16 bit quantity  */
typedef int            INT16S;  /* Signed   16 bit quantity   */
typedef unsigned long  INT32U;  /* Unsigned 32 bit quantity  */
typedef long           INT32S;  /* Signed   32 bit quantity  */
typedef float          FP32;    /* Single precision floating point  */
typedef double         FP64;    /* Double precision floating point  */
typedef enum 
    {
        BAUD115K = 3, BAUD76K = 6, BAUD57K = 8,
    	BAUD38K = 12, BAUD31250 = 15, BAUD19K = 25, BAUD14K = 34,
    	BAUD9600 = 51, BAUD4800 = 103, BAUD2400 = 207, 
    	BAUD1200 = 414, BAUD600 = 818, BAUD300 = 1636
    }   BaudRate;



unsigned char milliseconds = 0;

void AutoPan(void);
void Init(void);
void setbaud(BaudRate);
void pwmInit(int count);
void setOCR0(int count);
void setOCR1A(int count);
void setICR1(int count);
void Delay_ms(int ms);

