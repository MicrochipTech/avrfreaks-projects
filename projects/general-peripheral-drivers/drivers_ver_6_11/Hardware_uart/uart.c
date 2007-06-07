
/****************************************************************************
 Title  :   C  file for the UART FUNCTIONS library (uart.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      30/Aug/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/


#if !defined(AVRGCC_VERSION)
#if ((__GNUC__ * 100) + __GNUC_MINOR__) > 303
#warning "compatibility header file included"
#include "compatibility.h"
#define AVRGCC_VERSION      304        /* AVRGCC version for including the correct files  */
#elif ((__GNUC__ * 100) + __GNUC_MINOR__) >= 300  &&  ((__GNUC__ * 100) + __GNUC_MINOR__) <= 303
#warning "compatibility header file not included"
#define AVRGCC_VERSION      303        /* AVRGCC version for including the correct files  */
#else 
#warning "compatibility header file not included"
#define AVRGCC_VERSION   300    
#endif
#endif /* #if !defined(AVRGCC_VERSION) */

#if AVRGCC_VERSION >= 303

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#else

#include <io.h>
#include <sig-avr.h>
#include <eeprom.h>
#include <pgmspace.h>

#endif

#include "uart.h"

/* 
#if !defined(outp)
#define outp(value,port)  outb(port,value)
#endif
*/
/* REGISTER & DEVICE CAPABILITY LIBRARY */
#if defined (__AVR_ATmega128__)

#define UART0_UDR                  UDR0
#define UART0_UCR                  UCSR0B
#define UART0_USR                  UCSR0A
#define UART0_UBRRL                UBRR0L
#define UART0_UBRRH                UBRR0H
#define UART0_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART0_TRANS)
#define UART0_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART0_RECV)

#define UART1_UDR                  UDR1
#define UART1_UCR                  UCSR1B
#define UART1_USR                  UCSR1A
#define UART1_UBRRL                UBRR1L
#define UART1_UBRRH                UBRR1H
#define UART1_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART1_TRANS)
#define UART1_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART1_RECV)

#define NUMBER_OF_UARTS            2
#define UART_DOUBLE_SPEED_CAPABLE  1
#define USART_CAPABLE              1
/*------------------------------------------------------------------------------------------------*/
#elif defined (__AVR_ATmega161__)

#define UART0_UDR                  UDR0
#define UART0_UCR                  UCSR0B
#define UART0_USR                  UCSR0A
#define UART0_UBRRL                UBRR0
#define UART0_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART0_TRANS)
#define UART0_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART0_RECV)

#define UART1_UDR                  UDR1
#define UART1_UCR                  UCSR1B
#define UART1_USR                  UCSR1A
#define UART1_UBRRL                UBRR1
#define UART1_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART1_TRANS)
#define UART1_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART1_RECV)

#define NUMBER_OF_UARTS            2
#define UART_DOUBLE_SPEED_CAPABLE  1
#define USART_CAPABLE              0

/*------------------------------------------------------------------------------------------------*/
#elif defined(__AVR_ATmega163__) || defined(__AVR_ATmega323__)

#define UART0_UDR                  UDR
#define UART0_UCR                  UCSRB
#define UART0_USR                  UCSRA
#define UART0_UBRRL                UBRR
#define UART0_UBRRH                UBRRH
#define UART0_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART_TRANS)
#define UART0_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART_RECV)

#define NUMBER_OF_UARTS            1
#define UART_DOUBLE_SPEED_CAPABLE  1
#define USART_CAPABLE              0
#undef  UART1_RX_TERMINATION_CHAR
/*------------------------------------------------------------------------------------------------*/
#elif defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) || defined(__AVR_ATmega64__) || \
      defined(__AVR_ATmega8535__) || defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8__)


#define UART0_UDR                  UDR
#define UART0_UCR                  UCSRB
#define UART0_USR                  UCSRA
#define UART0_UBRRL                UBRRL
#define UART0_UBRRH                UBRRH
#define UART0_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART_TRANS)
#define UART0_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART_RECV)

#define NUMBER_OF_UARTS            1
#define UART_DOUBLE_SPEED_CAPABLE  1
#define USART_CAPABLE              1
#undef  UART1_RX_TERMINATION_CHAR
/*------------------------------------------------------------------------------------------------*/
#else

#define UART0_UDR                  UDR
#define UART0_UCR                  UCR
#define UART0_USR                  USR
#define UART0_UBRRL                UBRR
#define UART0_TX_INTERRUPT_ISR()   SIGNAL(SIG_UART_TRANS)
#define UART0_RX_INTERRUPT_ISR()   SIGNAL(SIG_UART_RECV)

#define NUMBER_OF_UARTS            1
#define UART_DOUBLE_SPEED_CAPABLE  0
#define USART_CAPABLE              0
#undef  UART1_RX_TERMINATION_CHAR

#endif
/*------------------------------------------------------------------------------------------------*/
/* DEFINITION OF UART BIT POSITIONS  */
/* UART_USR */
#define UART_RXC                   7
#define UART_TXC                   6
#define UART_UDRE                  5
#define UART_FE                    4
#define UART_OVR                   3
#define UART_U2X                   1

/* UART_UCR */
#define UART_RXCIE                 7
#define UART_TXCIE                 6
#define UART_UDRIE                 5
#define UART_RXEN                  4
#define UART_TXEN                  3
/*------------------------------------------------------------------------------------------------*/
 

/* DECISION OF HOW MANY UARTS TO USE */
#if      NUMBER_OF_UARTS > UARTS_NEEDED
#undef   NUMBER_OF_UARTS
#define  NUMBER_OF_UARTS   UARTS_NEEDED

#elif    NUMBER_OF_UARTS == 2 && UARTS_NEEDED > 2
#warning YOU DONT HAVE THAT MANY UARTS! CONFIGURATION DONE USING 2 UART!

#elif    NUMBER_OF_UARTS == 1 && UARTS_NEEDED >= 2
#warning YOU DONT HAVE 2 UARTS! CONFIGURATION DONE USING 1 UART!

#endif

/* IF DYNAMIC BAUD RATE IS NOT MANDATORY */
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if UART_DYNAMIC_BAUD_CHANGE == 0 && NUMBER_OF_UARTS >= 1
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#define UART0_DIVIDER   16

#define UART0_BAUDRATE_ACCURATE   (F_CPU/(UART0_BAUDRATE/100))  
#define UART0_BAUDRATE_REAL       (((F_CPU/UART0_BAUDRATE)/UART0_DIVIDER)*UART0_DIVIDER)

/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if  ((UART0_BAUDRATE_ACCURATE/UART0_BAUDRATE_REAL)-100) > 2
/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#if UART_DOUBLE_SPEED_CAPABLE == 1

#undef UART0_DIVIDER 
#define UART0_DIVIDER 8
#define UART0_BAUDRATE_X2   1
#if  ((UART0_BAUDRATE_ACCURATE/UART0_BAUDRATE_REAL)-100) > 1
#error " UART0_BAUDRATE_ERROR_TOO_HIGH! "
#endif 

#elif UART_DOUBLE_SPEED_CAPABLE == 0  /* #if UART_DOUBLE_SPEED_CAPABLE == 1 */

#if  ((UART0_BAUDRATE_ACCURATE/UART0_BAUDRATE_REAL)-100) > 2
#error " UART0_BAUDRATE_ERROR_TOO_HIGH! "
#endif 

#endif  /* #elif UART_DOUBLE_SPEED_CAPABLE == 0 */

/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define UBRR0_VALUE ( ((F_CPU/UART0_BAUDRATE)/UART0_DIVIDER)-1 )


#if UBRR0_VALUE > 255 && UBRR0_VALUE <= 0XFFF && UART0_UBRRH > 0
#define UBRRH0_VALUE ( (UBRR0_VALUE>>8) & 0X0F )
#elif  UBRR0_VALUE > 0XFFF && UART0_UBRRH > 0
#error " UART0_ERROR BAUDRATE TOO LOW! "
#elif UBRR0_VALUE > 255 
#error " UART0_ERROR BAUDRATE TOO LOW! "
#endif

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif  /* #if UART_DYNAMIC_BAUD_CHANGE == 0 && NUMBER_OF_UARTS >= 1 */
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if  UART_DYNAMIC_BAUD_CHANGE == 0 && NUMBER_OF_UARTS >= 2
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#define UART1_DIVIDER   16

#define UART1_BAUDRATE_ACCURATE   (F_CPU/(UART1_BAUDRATE/100))  
#define UART1_BAUDRATE_REAL       (((F_CPU/UART1_BAUDRATE)/UART1_DIVIDER)*UART1_DIVIDER)

/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if  ((UART1_BAUDRATE_ACCURATE/UART1_BAUDRATE_REAL)-100) > 2
/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#if UART_DOUBLE_SPEED_CAPABLE == 1

#undef UART1_DIVIDER 
#define UART1_DIVIDER 8
#define UART1_BAUDRATE_X2   1
#if  ((UART1_BAUDRATE_ACCURATE/UART1_BAUDRATE_REAL)-100) > 1
#error " UART1_BAUDRATE_ERROR_TOO_HIGH! "
#endif 

#elif UART_DOUBLE_SPEED_CAPABLE == 0  /* #if UART_DOUBLE_SPEED_CAPABLE == 1 */

#if  ((UART1_BAUDRATE_ACCURATE/UART1_BAUDRATE_REAL)-100) > 2
#error " UART1_BAUDRATE_ERROR_TOO_HIGH! "
#endif 

#endif  /* #elif UART_DOUBLE_SPEED_CAPABLE == 0 */

/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define UBRR1_VALUE ( ((F_CPU/UART1_BAUDRATE)/UART1_DIVIDER)-1 )


#if UBRR1_VALUE > 255 && UBRR1_VALUE <= 0XFFF && UART1_UBRRH > 0
#define UBRRH1_VALUE ( (UBRR1_VALUE>>8) & 0X0F )
#elif  UBRR1_VALUE > 0XFFF && UART1_UBRRH > 0
#error " UART1_ERROR BAUDRATE TOO LOW! "
#elif UBRR1_VALUE > 255 
#error " UART1_ERROR BAUDRATE TOO LOW! "
#endif

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif  /* UART_DYNAMIC_BAUD_CHANGE == 0 && NUMBER_OF_UARTS >= 2 */
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

/* Mistyped switches error catching */
#if UART_DYNAMIC_BAUD_CHANGE > 1 || UART_DYNAMIC_BAUD_CHANGE < 0
#error "UART_DYNAMIC_BAUD_CHANGE" CAN BE 0 OR 1 ! 
#endif

#if UART0_ERROR_CHECK_SUPPORT  > 1 || UART0_ERROR_CHECK_SUPPORT  < 0
#error  "UART0_ERROR_CHECK_SUPPORT"  CAN BE 0 OR 1 ! 
#endif

#if UART0_AT_COMMAND_SUPPORT > 1 || UART0_AT_COMMAND_SUPPORT < 0
#error "UART0_AT_COMMAND_SUPPORT"  CAN BE 0 OR 1 ! 
#endif

#if UART0_RAW_DATA_SUPPORT > 1 || UART0_RAW_DATA_SUPPORT < 0
#error "UART0_RAW_DATA_SUPPORT" CAN BE 0 OR 1 ! 
#endif

#if UART1_ERROR_CHECK_SUPPORT > 1 || UART1_ERROR_CHECK_SUPPORT < 0
#error  "UART1_ERROR_CHECK_SUPPORT"  CAN BE 0 OR 1 ! 
#endif

#if UART1_AT_COMMAND_SUPPORT > 1 || UART1_AT_COMMAND_SUPPORT < 0
#error "UART1_AT_COMMAND_SUPPORT"  CAN BE 0 OR 1 ! 
#endif

#if UART1_RAW_DATA_SUPPORT > 1 || UART1_RAW_DATA_SUPPORT < 0
#error "UART1_RAW_DATA_SUPPORT" CAN BE 0 OR 1 ! 
#endif

#if UART_MULTI_COMMAND_SUPPORT > 1 || UART_MULTI_COMMAND_SUPPORT < 0
#error "UART_MULTI_COMMAND_SUPPORT"  CAN BE 0 OR 1 ! 
#endif
 


/*####################################################################################################*/ 
/* uart globals */
/*####################################################################################################*/ 
/* TYPE DEFINITIONS  */
typedef unsigned char  u08;
typedef          char  s08;
typedef unsigned short u16;
typedef          short s16;



/* TxD variables */
volatile u08    tx_busy;
volatile u08    *tx_ptr;
         u08    tx_on;
         u08    echo;
         u08    data_location;
         u08    tx_c=0;
 
/* RxD variables */
volatile u08    uart_rx_buffer[(UART_RX_BUFFER_SIZE+1)];
volatile u08    rx_c;
volatile u08    rx_ISR_counter;
         u08    uart_rx_count;
         u08    uart_string_received;
         u08    uart_at_cmd_detected;
         u08    uart_error;
         u08    rx_mode;
         u08    temp1=0;

/*####################################################################################################*/ 
/* PRIVATE ROUTINES  */
/*####################################################################################################*/

/* private function prototypes */
#if UART_MULTI_COMMAND_SUPPORT == 1
static unsigned char execute_uart_command(unsigned int command);
#endif
static void          flush_rx_buffer(void);

/*####################################################################################################*/

static void flush_rx_buffer(void)
{
/* clear uart_rx_buffer any rx system messages */
    
    for(uart_rx_count=0;uart_rx_count<=UART_RX_BUFFER_SIZE;uart_rx_count++)
      { 
         *(uart_rx_buffer+uart_rx_count)='\0';
      }   
    uart_rx_count=0;
    uart_string_received=0;
    
}
/*####################################################################################################*/
#if UART_MULTI_COMMAND_SUPPORT == 1
static unsigned char execute_uart_command(unsigned int command)
#elif UART_MULTI_COMMAND_SUPPORT == 0
unsigned char        uart_command(unsigned int command)
#endif
{
unsigned char x = 0;
   
switch(command)
     {
        case(UART_RX_OFF)         :  cbi(UART0_UCR, UART_RXCIE);
                                     cbi(UART0_UCR, UART_RXEN);
                                     break;                          

        case(UART_GET_STRING)     :  cbi(UART0_UCR, UART_RXCIE);
                                     x = inp(UART0_UDR);
                                     flush_rx_buffer(); 
                                     uart_error=0;
                                     rx_mode=UART_GET_AT_CMD;
                                     uart_at_cmd_detected=1;
                                     sbi(UART0_UCR, UART_RXCIE);
                                     sbi(UART0_UCR, UART_RXEN);
                                     break;                           
#if UART0_AT_COMMAND_SUPPORT == 1 
        case(UART_GET_AT_CMD)     :  cbi(UART0_UCR, UART_RXCIE);
                                     x = inp(UART0_UDR);
                                     flush_rx_buffer(); 
                                     uart_error=0;
                                     rx_mode=UART_GET_AT_CMD;
                                     uart_at_cmd_detected=0;
                                     temp1=0;
                                     sbi(UART0_UCR, UART_RXCIE);
                                     sbi(UART0_UCR, UART_RXEN);
                                     break;                                                          
#endif
#if UART0_RAW_DATA_SUPPORT == 1         
        case(UART_GET_RAW_DATA)   :  cbi(UART0_UCR, UART_RXCIE);
                                     x = inp(UART0_UDR);
                                     flush_rx_buffer();
                                     uart_error=0;
                                     rx_mode=UART_GET_RAW_DATA;
                                     sbi(UART0_UCR, UART_RXCIE);
                                     sbi(UART0_UCR, UART_RXEN);
                                     break;  
#endif
        case(UART_GET_RX_COUNT)   :  return(uart_rx_count);
                                     break;  

        case(UART_STRING_RECEIVED): return(uart_string_received);
                                    break;  


        case(UART_AT_CMD_RECEIVED): return(uart_string_received);
                                    break;  
                            
#if UART0_ERROR_CHECK_SUPPORT == 1          
        case(UART_GET_ERROR)      : return(uart_error);
                                    break;  
#endif
        case(UART_TX_ON)          : sbi(UART0_UCR, UART_TXCIE);
                                    sbi(UART0_UCR, UART_TXEN);
                                    tx_on=1;
                                    break;                 
           
        case(UART_TX_OFF)         : while(tx_busy);
                                    cbi(UART0_UCR, UART_TXCIE);
                                    cbi(UART0_UCR, UART_TXEN);
                                    tx_on=0;
                                    break;  

        case(UART_ECHO_ON)        : echo=1;
                                    break;                                                    
           
        case(UART_ECHO_OFF)       : echo=0;
                                    break;  

              
           /* IF NONE OF THE ABOVE */
        default :                   break;  
           
         }  
return(0); 
}
/*####################################################################################################*/
/* PUBLIC  CONTROL  FUNCTIONS */
/*####################################################################################################*/

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if   UART_DYNAMIC_BAUD_CHANGE == 0
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
void uart_init(void)
/* initialize uart */
{
  
    while(tx_busy);
    asm("cli");

/* INITIALIZE THE UART AND SET BAUD RATE */
    rx_c='\0';
    flush_rx_buffer();

    /* DISABLE RxD / Txd and associated interrupts  */
    outp(0,UART0_UCR);
 
#if defined(UART0_BAUDRATE_X2)
    sbi(UART0_USR, UART_U2X);
#endif

#if defined(UART0_UBRRH) && defined(UBRRH0_VALUE) 
    outp((unsigned char)UBRRH0_VALUE, UART0_UBRRH);
#endif

    outp((unsigned char)UBRR0_VALUE, UART0_UBRRL);

    asm("sei");    

return;
}

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif UART_DYNAMIC_BAUD_CHANGE == 1
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

unsigned char uart_init(unsigned long int baud_rate, unsigned int command)
/* initialize uart */
{
unsigned char divider=0;
union word_union {
                     unsigned int  ubrr_value; 
                     unsigned char ubrr_byte[2];
                 } baud;
    
    while(tx_busy);
    asm("cli");
/*  EXECUTE THE GIVEN UART COMMANDS */
    uart_command(command);

/* INITIALIZE THE UART AND SET BAUD RATE */
    rx_c='\0';
    flush_rx_buffer();

    /* DISABLE RxD / Txd and associated interrupts  */
    outp(0,UART0_UCR);

    divider=16;
    /* set baud rate */
    baud.ubrr_value = (((F_CPU/baud_rate)/divider)-1);   /* UBRR=25 for 4 mhz and 9600 baud  */
    if( (((F_CPU/(baud_rate/100))/((baud.ubrr_value+1)*divider))-100) > 2  )
     { 
#if UART_DOUBLE_SPEED_CAPABLE == 1
        divider=8;
        sbi(UART0_USR, UART_U2X);
        baud.ubrr_value = (((F_CPU/baud_rate)/divider)-1);
        if( (((F_CPU/(baud_rate/100))/((baud.ubrr_value+1)*divider))-100) > 1  )
         { 
             return(UART_HIGH_BAUDRATE_ERROR); 
         }
#else 
        return(UART_HIGH_BAUDRATE_ERROR);
#endif
     }

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/    
#if defined(UART0_UBRRH)
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

    if(baud.ubrr_byte[1] > 0x0F) { return(UART_BAUDRATE_TOO_LOW); }
    outp(baud.ubrr_byte[1], UART0_UBRRH);

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif !defined(UART0_UBRRH)
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

    if(baud.ubrr_byte[1] > 0) { return(UART_BAUDRATE_TOO_LOW); }

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
    outp(baud.ubrr_byte[0], UART0_UBRRL);

    asm("sei");    

return(0);
}

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif  /* #elif UART_DYNAMIC_BAUD_CHANGE == 1 */
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

/*####################################################################################################*/
#if UART_MULTI_COMMAND_SUPPORT == 1
unsigned char uart_command(unsigned int command)
{   
unsigned char temp=0, return_value=0;

       do
        {
           if( command & (1<<temp) ) { return_value=execute_uart_command( (1<<temp) ); }
           temp++;
        }
       while( temp<16 );

return(return_value);
}
#endif

/*####################################################################################################*/
/* PUBLIC   INTERRUPT MODE DATA FUNCTIONS */
/*####################################################################################################*/

void  uart_get_buf(unsigned char *data, unsigned char size)
{
unsigned char x=0, y=0;   

  
        cbi(UART0_UCR, UART_RXCIE);
        for(x=0; (x<(UART_RX_BUFFER_SIZE+1) && x<size); x++)
          {
             *(data+x)= *(uart_rx_buffer+x);
             *(data+x+1)='\0';
          }
        /* ADJUST THE RX BUFFER POINTERS SO THE NEW FOUND SPACE IS ANOUNCED */
        uart_rx_count=uart_rx_count-x;
        uart_string_received=uart_string_received-x; 

        /* MOVE THE UNREAD BUFFER CONTENTS AT THE BEGGINING OF THE RX BUFFER */
        for(; x <= UART_RX_BUFFER_SIZE; x++, y++)
          {
             *(uart_rx_buffer+y)= *(uart_rx_buffer+x);
             *(uart_rx_buffer+y+1)='\0';  
          }
        
        sbi(UART0_UCR, UART_RXCIE);


return;  
}
/*####################################################################################################*/

void uart_putc(unsigned char tx_data)
/* send buffer <tx_data> to uart */
{   
 
  while(tx_busy);
  tx_busy=1;
  outp(tx_data, UART0_UDR); 

return;  
}
/*####################################################################################################*/

void uart_puts(u08 *tx_data)
/* send  <tx_data> to uart */
{   
   if(*tx_data && tx_data && tx_on) /* check for TX_on, a NULL pointer or an EMPTY string */
    {
      while(tx_busy);
      tx_busy=1; data_location=LOCATION_IS_RAM;
      tx_ptr=tx_data; outp(*tx_data, UART0_UDR);
    }
}
/*####################################################################################################*/

void uart_puts_p(const char *progmem_tx_data)
/* send progmem  <progmem_tx_data> to uart */
{   
  if(tx_on) /* If TX_on send the string else terminate */
   {
      while(tx_busy); 
      tx_busy=1; data_location=LOCATION_IS_FLASH;
      tx_ptr=(u08*)progmem_tx_data;
      outp(PRG_RDB(tx_ptr), UART0_UDR);
   }
}
/*####################################################################################################*/

void uart_puts_e(unsigned char *eeprom_tx_data)
/* send progmem  <eeprom_tx_data> to uart */
{   
  
  if(tx_on) /* If TX_on send the string else terminate */
   {
      while(tx_busy); 
      tx_busy=1; data_location=LOCATION_IS_EEPROM;
      tx_ptr=(u08*)eeprom_tx_data;
      outp(eeprom_rb((u16)tx_ptr), UART0_UDR);
   }
}
/*####################################################################################################*/
void uart_puti(int value, unsigned char dot_position)
{
unsigned char lcd_data[7]={' ',' ',' ',' ',' ',' ',' ' }; 
unsigned char position=sizeof(lcd_data), radix=10; 
        

/* Some adjustments if the number is negative */
if(value<0) { value=-value; uart_putc('-'); } 

do{
     if(dot_position)
      {
         if((sizeof(lcd_data)-position)>=dot_position)
          {
             position--;
             *(lcd_data+position)=PUTI_DECIMAL_POINT;
             dot_position=0;
          }
      } 
     position--;
     *(lcd_data+position)=(value%radix)+'0';
     value/=radix;

  }while((value || dot_position)&& dot_position <= 5); 

          
/* Display the number on the LCD screen */
for(; position<sizeof(lcd_data); position++)
  {
     uart_putc(lcd_data[position]);
  }

return;
}
/*####################################################################################################*/

unsigned char uart_getc(void)
{   
u08 buffer=rx_ISR_counter;  
 
  while(buffer==rx_ISR_counter); 
 
return(rx_c);  
}

/*####################################################################################################*/
/* INTERRUPT SERVICE ROUTINES  */
/*####################################################################################################*/

UART0_TX_INTERRUPT_ISR()        /* signal handler for uart txd ready interrupt */
{

   tx_ptr++;
   if(data_location==LOCATION_IS_EEPROM)
    {
       if((tx_c=eeprom_rb((u16)tx_ptr)) != 0xFF) { outp(tx_c,UART0_UDR ); }
       else { data_location=0; tx_busy=0; }
    }
   else if(data_location==LOCATION_IS_FLASH)
         {
            if( (tx_c=PRG_RDB(tx_ptr)) )  { outp(tx_c, UART0_UDR); }
            else { data_location=0; tx_busy=0; }
         }
   else if(data_location==LOCATION_IS_RAM)
         {
            if(*tx_ptr) { outp(*tx_ptr, UART0_UDR); }
            else { data_location=0; tx_busy=0; }
         }
   else  { tx_busy=0; }
}
/*####################################################################################################*/

UART0_RX_INTERRUPT_ISR()        /* signal handler for receive complete interrupt */
{
#if UART0_ERROR_CHECK_SUPPORT == 1
  if( bit_is_set(UART0_USR, UART_FE) ) { uart_error=FRAMING_ERROR; }
#endif
  rx_c=inp(UART0_UDR);
#if UART0_ERROR_CHECK_SUPPORT == 1
  if( bit_is_set(UART0_USR, UART_OVR) ) { uart_error=OVERUN_ERROR; }
#endif
  rx_ISR_counter++;         
  if(echo) if(tx_on) if(!tx_busy) outp(rx_c,UART0_UDR);
#if UART0_RAW_DATA_SUPPORT == 1
  if(rx_mode==UART_GET_RAW_DATA)
   {
      if( uart_rx_count >= UART_RX_BUFFER_SIZE )
       { 
          uart_error=BUFFER_OVERUN;
          rx_mode=0;
       }
      else{
             *(uart_rx_buffer+uart_rx_count)=rx_c;
             uart_rx_count++;
          }     
   }
  else
#endif
       if(rx_mode==UART_GET_AT_CMD)
        {
#if UART0_AT_COMMAND_SUPPORT == 1
           if(uart_at_cmd_detected)
            {
#endif
               if( rx_c==UART0_RX_TERMINATION_CHAR  )
                {
                    uart_string_received=(uart_rx_count+1);     /* valid rx data arrived */
                    rx_mode=0;
                } 
               else {
                       if(uart_rx_count >= UART_RX_BUFFER_SIZE)  
                        { 
#if UART0_ERROR_CHECK_SUPPORT == 1
                           uart_error=BUFFER_OVERUN;
#endif
                           uart_string_received=UART_RX_BUFFER_SIZE;
                           rx_mode=0;
                        }
                       else { *(uart_rx_buffer+uart_rx_count)=rx_c; uart_rx_count++; }
                    }
#if UART0_AT_COMMAND_SUPPORT == 1
            }
           else {  /* else wait for AT combination before copying rx data to uart_rx_buffer */
                   if(rx_c=='A') { temp1=1;  }
                   else if(rx_c=='T' && temp1==1)
                         {
                            uart_at_cmd_detected=1;
                         }
                   else  { temp1=0; }
                }
#endif
        }

}

/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/


