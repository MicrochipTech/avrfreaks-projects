
#include <avr/io.h>
#include "uart.h"
#ifndef F_CPU
  #define F_CPU 12000000UL      // MC Clock Rate 12MHZ
#endif

#include "uart.h"

static unsigned char inBuf[UART_INBUF_SIZE];
static unsigned char outBuf[UART_OUTBUF_SIZE];
static unsigned char inBufHead=0;
static unsigned char inBufTail=0;
static unsigned char outBufHead=0;
static unsigned char outBufTail=0;

// ***********************************************************
// UART_init(int baud)
//  Set Up USART   
// ***********************************************************
void UART_init(unsigned long baud)
{
   unsigned long bps;
  // Set baud rate -  per datasheet: UBRR=(F_CPU/(16*baud))-1
   bps = 16*baud;
   bps = (unsigned long)(F_CPU / bps)-1;
   // Set 16bit UBRR Register
   UBRRH = (unsigned char)(bps>>8);
   UBRRL = (unsigned char)bps;

   //   When the function writes to the UCSRC Register,
   //   the URSEL bit (MSB) must be set due to the sharing
   //   of I/O location by UBRRH and UCSRC.
   // Set frame format: 8data, 2stop bit
   UCSRC = _BV(URSEL)| _BV(UCSZ1)|_BV(UCSZ0)|_BV(USBS);

   // Enable receiver and transmitter
   UCSRB= (_BV(RXEN)| _BV(TXEN));
}


// ***********************************************************
// UART_get_chr(void)
//  Wait for a character to be recieved and return it's value
// ***********************************************************
char UART_get_chr(void)
{
   // Wait for byte receive to be complete. (RXC flag set)
   while ((UCSRA & _BV(RXC))==0);

   return UDR;   // return the byte received
}

// ***********************************************************
//  UART_put_chr(char c)
//  Wait for output buffer to be empty then send the character.
// ***********************************************************
void UART_put_chr(char c)
{
 // Wait for output buffer to be EMPTY (UDRE flag set)
  while ((UCSRA & _BV(UDRE))==0);

  UDR =c;  // Send the character
}

// ***********************************************************
//  UART_put_str(char *str)
//  Send characters to USART (str) until NULL is encountered.
// ***********************************************************
void UART_put_str(char *str)
{
  int p=0;  // character pointer set to start of string

  while(str[p]!=0)             // While char is not = 0
      UART_put_chr(str[p++]);  // send it and inc. pointer
}


// ***********************************************************
//  UART_poll()
//  Process input and output data Host to UART and UART to Host
// ***********************************************************
void UART_poll(void)
{
 // ** If byte recieved from host in buffer (inBuf)***         
 if(UART_inbufsize()>0)
   {
    // ** if UART output not busy **
    if(UART_out_ready())
       UART_put_chr((char)UART_inBuf_get());   // Send 1 byte to UART
   }

   // ** If data ready from UART ***
   if(UART_inp_ready())
      UART_outBuf_put((unsigned char)UART_get_chr());   // store in outBuf for transmit to Host
        
}

// ***********************************************************
//  UART_calc_bufsize(head, tail, size)
//  calculate the number of bytes in the specified bufer (inBuf or outBuf)
// ***********************************************************
unsigned char UART_calc_bufsize(unsigned char head, unsigned char tail, unsigned char size)
  {
    if(tail< head)
      return (unsigned char)((size-head)+tail);
    else   
      return (unsigned char)(tail-head);   
  }
  
// ***********************************************************
//  UART_outbufsize()
//  return the number of bytes in outBuf
// ***********************************************************  
unsigned char UART_outbufsize(void)
{  
  return UART_calc_bufsize(outBufHead, outBufTail, UART_OUTBUF_SIZE);
}

// ***********************************************************
//  UART_inbufsize()
//  return the number of bytes in inBuf
// ***********************************************************  
unsigned char UART_inbufsize(void)
{  
  return UART_calc_bufsize(inBufHead, inBufTail, UART_OUTBUF_SIZE);
}

// ***********************************************************
//  UART_inBuf_put(c)
//  Add a byte to InBuf and adjust pointer
// ***********************************************************
void UART_inBuf_put(unsigned char c)
 {
  inBuf[inBufTail]=c;             //Save the byte
  inBufTail++;                    //Update buffer pointer
  inBufTail &= UART_INBUF_MASK;   //Mask the pointer value 
 }
 
// ***********************************************************
//  UART_outBuf_put(c)
//  Add a byte to outBuf and adjust pointer 
// ***********************************************************
void UART_outBuf_put(unsigned char c)
 {
  outBuf[outBufTail]=c;            //Save the byte
  outBufTail++;                    //Update buffer pointer
  outBufTail &= UART_OUTBUF_MASK;  //Mask the pointer value 
 }
 
// ***********************************************************
//  UART_inBuf_get()
//  Get a byte from inBuf and adjust pointer
// ***********************************************************
unsigned char UART_inBuf_get(void)
 {
  unsigned char c = inBuf[inBufHead];   //Get a byte from the inBuf buffer
  inBufHead++;                          //Update buffer pointer
  inBufHead &= UART_INBUF_MASK;         //Mask the pointer value
  return c;
 }
 
// ***********************************************************
//  UART_outBuf_get()
//  Get a byte from outBuf and adjust pointer
// ***********************************************************
unsigned char UART_outBuf_get(void)
 {
  unsigned char c = outBuf[outBufHead];  //Get a byte from the outBuf buffer
  outBufHead++;                          //Update buffer pointer
  outBufHead &= UART_OUTBUF_MASK;        //Mask the pointer value
  return c;
 }

