#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C"
{
#endif

// INBUF_SIZE & OUTBUFSIZE MUST be power of 2 (e.g. 2,4,8,16,32,64,128...)
#define UART_INBUF_SIZE          128
#define UART_OUTBUF_SIZE         128
#define UART_INBUF_MASK          UART_INBUF_SIZE-1
#define UART_OUTBUF_MASK         UART_OUTBUF_SIZE-1

/* ** NOTE: These macros may need to be adjusted for your device ** */
#define UART_inp_ready() (UCSRA & _BV(RXC))!=0
#define UART_out_ready() (UCSRA & _BV(UDRE))!=0

// ***********************************************************
// UART_init(int baud)
//  Set Up USART   NOT Interrupt drived
// ***********************************************************
void UART_init(unsigned long baud);

// ***********************************************************
// UART_get_chr(void)
//  Wait for a character to be recieved and return it's value
// ***********************************************************
char UART_get_chr(void);

// ***********************************************************
//  UART_put_chr(char c)
//  Wait for output buffer to be empty then send the character.
// ***********************************************************
void UART_put_chr(char c);

// ***********************************************************
//  UART_put_str(char *str)
//  Send characters to USART (str) until NULL is encountered.
// ***********************************************************
void UART_put_str(char *str);

// ***********************************************************
//  UART_poll()
//  Process input and output data Host to UART and UART to Host
// ***********************************************************
void UART_poll(void);


// ***********************************************************
//  UART_calc_bufsize(head, tail, size)
//  calculate the number of bytes in the specified bufer (inBuf or outBuf)
// ***********************************************************
unsigned char UART_calc_bufsize(unsigned char head, unsigned char tail, unsigned char size);

// ***********************************************************
//  UART_outbufsize()
//  return the number of bytes in outBuf
// ***********************************************************  
unsigned char UART_outbufsize(void);

// ***********************************************************
//  UART_inbufsize()
//  return the number of bytes in inBuf
// ***********************************************************  
unsigned char UART_inbufsize(void);

// ***********************************************************
//  UART_inBuf_put(c)
//  Add a byte to InBuf and adjust pointer
// ***********************************************************
void UART_inBuf_put(unsigned char c);

// ***********************************************************
//  UART_outBuf_put(c)
//  Add a byte to outBuf and adjust pointer 
// ***********************************************************
void UART_outBuf_put(unsigned char c);

// ***********************************************************
//  UART_inBuf_get()
//  Get a byte from inBuf and adjust pointer
// ***********************************************************
unsigned char UART_inBuf_get(void);

// ***********************************************************
//  UART_outBuf_get()
//  Get a byte from outBuf and adjust pointer
// ***********************************************************
unsigned char UART_outBuf_get(void);

#ifdef __cplusplus
}
#endif

#endif


