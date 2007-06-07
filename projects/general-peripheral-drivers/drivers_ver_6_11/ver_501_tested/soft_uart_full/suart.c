
/****************************************************************************
 Title  :   C  file for the SOFTWARE UART FUNCTIONS library (suart.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      10/Nov/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  Based on application note AVR 305
            This software is FREE.

*****************************************************************************/


/*################### DO NOT CHANGE ANYTHING BELOW THIS LINE !!! #####################################*/
/*############## ALL CONFIGURATION IS DONE IN "softuart.cfg" file !!! ################################*/

#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1     /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1     /* This is for GCC 3.2 */
#endif

#ifndef AVRGCC_VERSION
#define AVRGCC_VERSION       330        /* AVRGCC version for including the correct files  */
#endif

#if AVRGCC_VERSION == 330

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#else

#include <io.h>
#include <eeprom.h>
#include <pgmspace.h>

#endif

#include "softuart_0.cfg"
#include "suart.h"














#if !defined(outp)
#define outp(value,port)  outb(port,value)
#endif

#if !defined(inp)
#define inp(port)  inb(port)
#endif


#if defined(__AVR_ATmega103__)

#undef  DDRC   
#undef  DDRF   
#undef  PINC   
#undef  PORTF  

#define DDRC   0
#define DDRF   0
#define PINC   0
#define PORTF  0

#if SUART_0_RX_PIN_REG == 0 
#error " THE SUART RX PIN IS OUTPUT ONLY "
#endif

#if SUART_0_TX_OUT_REG == 0 
#error " THE SUART TX PIN IS INPUT ONLY "
#endif

#if SUART_0_RX_PIN_REG == PINF
#undef  SUART_0_RX_PORT_IS_IO
#define SUART_0_RX_PORT_IS_IO    0
#endif

#if SUART_0_TX_OUT_REG == PORTC
#undef  SUART_0_TX_PORT_IS_IO
#define SUART_0_TX_PORT_IS_IO    0
#endif

#endif


#if SUART_0_DYNAMIC_BAUD_CHANGE == 1 && SUART_0_WILL_USE_A_TIMER == 1
#define DIVIDE_BY_1_LIMIT     ( 255 )
#define DIVIDE_BY_8_LIMIT     ( 255*8 )
#define DIVIDE_BY_32_LIMIT    ( 255*32 )
#define DIVIDE_BY_64_LIMIT    ( 255*64 )
#define DIVIDE_BY_128_LIMIT   ( 255*128 )
#define DIVIDE_BY_256_LIMIT   ( (unsigned long)255*256 )
#define DIVIDE_BY_1024_LIMIT  ( (unsigned long)255*1024 )
#endif



typedef unsigned char  u08;
typedef          char  s08;
typedef unsigned short u16;
typedef          short s16;

/* Global variables go here */

volatile unsigned char  suart_0_rx_buffer[SUART_0_BUF_SIZE+1];
volatile unsigned char  suart_0_rx_count=0;
volatile unsigned char  suart_0_string_received=0;
#if  SUART_0_TIMEOUT_ENABLED == 1    
volatile unsigned char  suart_0_timeout_flag=0;
#endif

volatile unsigned char  rx_c='\0';
volatile unsigned char  suart_0_receive_string=0;
unsigned char           sreg_memory=0;

#if   SUART_0_WILL_USE_INTERRUPT  == 1
volatile unsigned char  suart_0_set_mark;
#endif

#if   SUART_0_DYNAMIC_BAUD_CHANGE == 1 && SUART_0_WILL_USE_A_TIMER == 0

unsigned char           suart_0_delay1=0;
unsigned char           suart_0_delay2=0;
unsigned char           suart_0_delay3=0;

#elif SUART_0_DYNAMIC_BAUD_CHANGE == 1 && SUART_0_WILL_USE_A_TIMER == 1

unsigned char           suart_0_time_1_bit=0;
unsigned char           suart_0_time_half_bit=0;

#endif

#if SUART_0_DYNAMIC_IO_SETTINGS == 1
#if SREG > 0X3F
unsigned char           suart_0_port=SUART_0_RX_OUT_REG;
#elif SREG <= 0X3F
unsigned char           suart_0_port=(SUART_0_RX_OUT_REG+0x20);
#endif
unsigned char           suart_0_rx_pin=(1<<SUART_0_RX_PIN);
unsigned char           suart_0_tx_pin=(1<<SUART_0_TX_PIN);
#endif

/* External functions go here */
extern volatile void          SUART_0_PUTCHAR(unsigned char data)__attribute__((naked));
#if   SUART_0_WILL_USE_INTERRUPT  == 0
extern volatile unsigned char SUART_0_RX_FUNCTION(void)__attribute__((naked));
#elif SUART_0_WILL_USE_INTERRUPT  == 1
extern volatile void          SUART_0_RX_FUNCTION(void)__attribute__((signal))__attribute__((naked));
#endif

/*####################################################################################################*/
/* Private functions go here */
/*####################################################################################################*/
static void reset_rx_buffer(void)
{
unsigned char x=0;

  for(x=0; x < sizeof(suart_0_rx_buffer); x++)  
    {
      suart_0_rx_buffer[x]=0;
    } 
  suart_0_string_received=0;
  suart_0_rx_count=0;

return;
}
/*####################################################################################################*/

static void disable_global_int(void)
{
   if(bit_is_set(GLOBAL_INT_REG, GLOBAL_INT_BIT))
    {
    	 sreg_memory=1;
         asm("cli");
    }
   else { sreg_memory=0; }   

return;
}
/*####################################################################################################*/

static void restore_global_int(void)
{
   if(sreg_memory) { asm("sei"); } 

return;
}


/*####################################################################################################*/
/* Public functions go here */
/*####################################################################################################*/
#if SUART_0_DYNAMIC_BAUD_CHANGE == 1 && SUART_0_WILL_USE_A_TIMER == 0
unsigned char suart_0_initialize(unsigned int baudrate)
#elif SUART_0_DYNAMIC_BAUD_CHANGE == 1 && SUART_0_WILL_USE_A_TIMER == 1
unsigned char suart_0_initialize(unsigned long baudrate)
#elif SUART_0_DYNAMIC_BAUD_CHANGE == 0
unsigned char suart_0_init(void)
#endif
{
unsigned char return_value=0;


#if   SUART_0_WILL_USE_INTERRUPT  == 1
  suart_0_set_mark=0;
#endif

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/   
#if     SUART_0_USE_RS232_LOGIC == 0  
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/ 

   sbi(SUART_0_TX_OUT_REG, SUART_0_TX_PIN );                 /* set the SUART_0_TX_PIN pin at high state */
#if SUART_0_TX_PORT_IS_IO 
   sbi(SUART_0_TX_DDR_REG, SUART_0_TX_PIN ); 
#endif
#if SUART_0_RX_PORT_IS_IO 
   sbi(SUART_0_RX_OUT_REG, SUART_0_RX_PIN );
   cbi(SUART_0_RX_DDR_REG, SUART_0_RX_PIN ); 
#endif

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/ 
#elif   SUART_0_USE_RS232_LOGIC == 1
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/ 

#if SUART_0_TX_PORT_IS_IO 
   sbi(SUART_0_TX_DDR_REG, SUART_0_TX_PIN ); 
#endif
#if SUART_0_RX_PORT_IS_IO 
   cbi(SUART_0_RX_OUT_REG, SUART_0_RX_PIN );
   cbi(SUART_0_RX_DDR_REG, SUART_0_RX_PIN ); 
#endif

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/ 
#endif 
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/ 


/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if SUART_0_DYNAMIC_BAUD_CHANGE == 1
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#if   SUART_0_WILL_USE_A_TIMER == 0
unsigned char  remainder=0;
unsigned char  temp=0;
unsigned short cycles_1_bit=0;
#if   SUART_0_STOP_BIT_DETECTION == 0 

cycles_1_bit=((F_CPU/baudrate)/10);

   if( ((cycles_1_bit*SUART_0_STOP_BITS)+(cycles_1_bit/2)) < (SUART_0_RX_PROCESSING_CODE_SIZE*2))
    {
       return_value=SUART_0_BAUDRATE_TOO_HIGH; 
    }      

#elif SUART_0_STOP_BIT_DETECTION == 1

   if( (cycles_1_bit*SUART_0_STOP_BITS) < (SUART_0_RX_PROCESSING_CODE_SIZE*2) )
    {
       return_value=SUART_0_BAUDRATE_TOO_HIGH; 
    }      

#elif SUART_0_STOP_BIT_DETECTION == 2

   if( (cycles_1_bit/2) < (SUART_0_0_RX_PROCESSING_CODE_SIZE*2) )
    {
       return_value=SUART_0_BAUDRATE_TOO_HIGH; 
    }      

#endif 

   baudrate=((F_CPU/baudrate)-(SUART_0_CORRECTION_FACTOR*10))/6;
   remainder=baudrate%10;
   temp=(baudrate/10);

   if( (baudrate/10)>=0xFF ) { return_value=SUART_0_BAUDRATE_TOO_LOW; } 
   
   suart_0_delay1=(temp-(SUART_0_DELAY_OVERHEAD/3));
   suart_0_delay2=temp;
   suart_0_delay3=temp;
  
   if( suart_0_delay1 <= 0 || suart_0_delay2 <= 2 ) { return_value=SUART_0_BAUDRATE_TOO_HIGH; }


   if(remainder>1 && remainder<=3)       { suart_0_delay1++; }
   else if(remainder>=4 && remainder<=6) { suart_0_delay1++; suart_0_delay2++; }
   else if(remainder>=7)                 { suart_0_delay1++; suart_0_delay2++; suart_0_delay3++; }


#elif SUART_0_WILL_USE_A_TIMER == 1


  
#if SUART_0_OCIE > 0   
   outp((inp(TIMSK)& (~(1<<TOIE0)|(1<<OCIE0))),  TIMSK);
#elif SUART_0_OCIE == 0
      outp((inp(TIMSK)& (~(1<<SUART_0_TOIE))), TIMSK);
#endif

   baudrate=(F_CPU/baudrate);

   if(baudrate < (SUART_0_CORRECTION_FACTOR*2) ) { return_value=SUART_0_BAUDRATE_TOO_HIGH; } 
   else if(baudrate > DIVIDE_BY_1024_LIMIT ) { return_value=SUART_0_BAUDRATE_TOO_LOW; }

   if(baudrate<=DIVIDE_BY_1_LIMIT)
    { 
       outp(SUART_0_0_SUART_0_TIMER_DIV_BY_1, SUART_0_TCCR);   
       suart_0_time_1_bit=(baudrate-SUART_0_CORRECTION_FACTOR);
       suart_0_time_half_bit=((baudrate-SUART_0_DELAY_OVERHEAD)/2);
    } 
   else if(baudrate<=DIVIDE_BY_8_LIMIT)
         {
            outp(SUART_0_TIMER_DIV_BY_8, SUART_0_TCCR);      
            suart_0_time_1_bit=(baudrate-SUART_0_CORRECTION_FACTOR)/8;
            suart_0_time_half_bit=((baudrate-SUART_0_DELAY_OVERHEAD)/2)/8;
       }     
#if SUART_0_TIMER_DIV_BY_32 > 0
   else if(baudrate<=DIVIDE_BY_32_LIMIT )
         {
            outp(SUART_0_TIMER_DIV_BY_32, SUART_0_TCCR);     
            suart_0_time_1_bit=(baudrate-SUART_0_CORRECTION_FACTOR)/32;
            suart_0_time_half_bit=((baudrate-SUART_0_DELAY_OVERHEAD)/2)/32;
         }  
#endif
#if SUART_0_TIMER_DIV_BY_64 > 0
   else if(baudrate<=DIVIDE_BY_64_LIMIT)
         {
            outp(SUART_0_TIMER_DIV_BY_64, SUART_0_TCCR);     
            suart_0_time_1_bit=(baudrate-SUART_0_CORRECTION_FACTOR)/64;
            suart_0_time_half_bit=((baudrate-SUART_0_DELAY_OVERHEAD)/2)/64;
       }  
#endif
#if SUART_0_TIMER_DIV_BY_128 > 0
   else if(baudrate<=DIVIDE_BY_128_LIMIT)
         {
            outp(SUART_0_TIMER_DIV_BY_128, SUART_0_TCCR);      
            suart_0_time_1_bit=(baudrate-SUART_0_CORRECTION_FACTOR)/128;
            suart_0_time_half_bit=((baudrate-SUART_0_DELAY_OVERHEAD)/2)/128;
         }    
#endif
   else if( baudrate<=DIVIDE_BY_256_LIMIT )
         {
            outp(SUART_0_TIMER_DIV_BY_256, SUART_0_TCCR);      
            suart_0_time_1_bit=(baudrate-SUART_0_CORRECTION_FACTOR)/256;
            suart_0_time_half_bit=((baudrate-SUART_0_DELAY_OVERHEAD)/2)/256;
       }  
   else if( baudrate<=DIVIDE_BY_1024_LIMIT )
         {
            outp(SUART_0_TIMER_DIV_BY_1024, SUART_0_TCCR);     
            suart_0_time_1_bit=(baudrate-SUART_0_CORRECTION_FACTOR)/1024;
            suart_0_time_half_bit=((baudrate-SUART_0_DELAY_OVERHEAD)/2)/1024;
         }
   else { return_value=SUART_0_BAUDRATE_TOO_LOW; }   

   if( SUART_0_DELAY_OVERHEAD>=suart_0_time_half_bit ) 
    {
       return_value=SUART_0_BAUDRATE_TOO_HIGH; 
    }    

#if    SUART_0_STOP_BIT_DETECTION == 0 

   if( ((baudrate*SUART_0_STOP_BITS)+(baudrate/2)) < (SUART_0_RX_PROCESSING_CODE_SIZE*2) )
    {
       return_value=SUART_0_BAUDRATE_TOO_HIGH; 
    }    

#elif   SUART_0_STOP_BIT_DETECTION == 1 

   if( (baudrate*SUART_0_STOP_BITS) < (SUART_0_RX_PROCESSING_CODE_SIZE*2) )
    {
       return_value=SUART_0_BAUDRATE_TOO_HIGH; 
    }    

#elif   SUART_0_STOP_BIT_DETECTION == 2

   if( (baudrate/2) < (SUART_0_RX_PROCESSING_CODE_SIZE*2) )
    {
       return_value=SUART_0_BAUDRATE_TOO_HIGH; 
    }    

#endif 



#endif /* #elif SUART_0_WILL_USE_A_TIMER == 1 */

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif SUART_0_DYNAMIC_BAUD_CHANGE == 0
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#if SUART_0_WILL_USE_A_TIMER == 1
#if SUART_0_OCIE > 0
      outp((inp(TIMSK)& (~(1<<SUART_0_TOIE)|(1<<SUART_0_OCIE))), TIMSK);
#elif SUART_0_OCIE == 0
      outp((inp(TIMSK)& (~(1<<SUART_0_TOIE))), TIMSK);
#endif

      outp(SUART_0_PRESCALER_VALUE, SUART_0_TCCR); 

#endif

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif  /* #elif SUART_0_DYNAMIC_BAUD_CHANGE == 0 */ 
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#if   SUART_0_WILL_USE_INTERRUPT  == 1  


#if   SUART_0_INT_EDGE_TRIGERING == 1 &&  SUART_0_USE_RS232_LOGIC == 0 
/* EXTERNAL INTERRUPT IS TRIGERED BY A FALLING EDGE */
   cbi(SUART_0_INT_SENSE_CONTROL_REG, SUART_0_INT_SENSE_CONTROL_BIT_0);
   sbi(SUART_0_INT_SENSE_CONTROL_REG, SUART_0_INT_SENSE_CONTROL_BIT_1); 
#elif SUART_0_INT_EDGE_TRIGERING == 1 &&  SUART_0_USE_RS232_LOGIC == 1
/* EXTERNAL INTERRUPT IS TRIGERED BY A FALLING EDGE */
   sbi(SUART_0_INT_SENSE_CONTROL_REG, SUART_0_INT_SENSE_CONTROL_BIT_0);
   sbi(SUART_0_INT_SENSE_CONTROL_REG, SUART_0_INT_SENSE_CONTROL_BIT_1); 
#endif
/* Turn suart external interrupt and global interrupts on */
   sbi(SUART_0_INT_REG, SUART_0_INT_BIT);        
   asm("sei");
#endif



return(return_value);
}
/*####################################################################################################*/
#if SUART_0_DYNAMIC_IO_SETTINGS == 1
void suart_0_configure_io(unsigned char port_register, unsigned char rx_pin, unsigned char tx_pin)
{

 suart_0_rx_pin=(1<<rx_pin);
 suart_0_tx_pin=(1<<tx_pin);
 suart_0_port=port_register;

#if    SUART_0_USE_RS232_LOGIC == 0
 *((unsigned int*)(suart_0_port-0)) |= suart_0_tx_pin;
 *((unsigned int*)(suart_0_port-1)) |= suart_0_tx_pin;
 *((unsigned int*)(suart_0_port-0)) |= suart_0_rx_pin;
 *((unsigned int*)(suart_0_port-1)) &= (~suart_0_rx_pin);
#elif  SUART_0_USE_RS232_LOGIC == 1
 *((unsigned int*)(suart_0_port-0)) &= (~suart_0_tx_pin);
 *((unsigned int*)(suart_0_port-1)) |= suart_0_tx_pin;
 *((unsigned int*)(suart_0_port-0)) &= (~suart_0_rx_pin);
 *((unsigned int*)(suart_0_port-1)) &= (~suart_0_rx_pin);

#endif

return;
}
#endif
/*####################################################################################################*/

void suart_0_raw_rx(unsigned char number_of_chars)
{
/*******************************************************************************************************/ 
#if   SUART_0_WILL_USE_INTERRUPT  == 1 
/*******************************************************************************************************/ 

reset_rx_buffer();
suart_0_set_mark=number_of_chars;   
sbi(SUART_0_INT_REG, SUART_0_INT_BIT);
suart_0_receive_string=0;

/*******************************************************************************************************/ 
#elif   SUART_0_WILL_USE_INTERRUPT  == 0
/*******************************************************************************************************/ 

reset_rx_buffer();
suart_0_receive_string=0;
disable_global_int();  

#if    SUART_0_TIMEOUT_ENABLED == 1
   suart_0_timeout_flag=0;
#endif

while(suart_0_string_received==0 && suart_0_rx_count < number_of_chars  ) { SUART_0_RX_FUNCTION(); }     

   /* Restore Global Interrupts to previus state */
   restore_global_int();  

#endif 

return;
}
/*####################################################################################################*/

void suart_0_rx_off(void)
{
#if   SUART_0_WILL_USE_INTERRUPT  == 1
cbi(SUART_0_INT_REG, SUART_0_INT_BIT);
#endif

reset_rx_buffer();


return;
} 
/*####################################################################################################*/

unsigned char   suart_0_getc(void)
{
#if   SUART_0_WILL_USE_INTERRUPT  == 1
register unsigned char suart_0_rx_count_buf=0;

suart_0_rx_count_buf=suart_0_rx_count;

   while(suart_0_rx_count == suart_0_rx_count_buf); 


#elif SUART_0_WILL_USE_INTERRUPT  == 0

      
/* Disable global Interrupts */
disable_global_int();

#if    SUART_0_TIMEOUT_ENABLED == 1
   suart_0_timeout_flag=0;
#endif 
   SUART_0_RX_FUNCTION();
   /* Restore Global Interrupts to previus state */
   restore_global_int();
#endif

return(rx_c);    
}
/*####################################################################################################*/

void   suart_0_gets(unsigned char number_of_chars)
{
/*******************************************************************************************************/  
#if SUART_0_WILL_USE_INTERRUPT  == 1
/*******************************************************************************************************/

suart_0_receive_string=1;
reset_rx_buffer();  
suart_0_set_mark=number_of_chars; 

sbi(SUART_0_INT_REG, SUART_0_INT_BIT);

/*******************************************************************************************************/
#elif SUART_0_WILL_USE_INTERRUPT  == 0
/*******************************************************************************************************/   

/* Disable global Interrupts */
disable_global_int();  
reset_rx_buffer();
suart_0_receive_string=1;


#if    SUART_0_TIMEOUT_ENABLED == 1
suart_0_timeout_flag=0;
#endif

while( suart_0_string_received==0 && suart_0_rx_count < number_of_chars ) { SUART_0_RX_FUNCTION(); }  

/* Restore Global Interrupts to previus state */
restore_global_int();  


/*******************************************************************************************************/  
#endif 
/*******************************************************************************************************/

return;
}
/*####################################################################################################*/

void  suart_0_get_buf(unsigned char *data, unsigned char size)
{
register unsigned char x=0, y=0;   


#if SUART_0_WILL_USE_INTERRUPT  == 1
        cbi(SUART_0_INT_REG, SUART_0_INT_BIT);
#endif

        for(x=0; (x<SUART_0_BUF_SIZE+1 && x<size); x++)
          {
             *(data+x)= *(suart_0_rx_buffer+x);
             *(data+x+1)='\0';
          }

        suart_0_rx_count=(suart_0_rx_count-x);

        for(; x <= SUART_0_BUF_SIZE; x++, y++)
          {
             *(suart_0_rx_buffer+y)= *(suart_0_rx_buffer+x);
             *(suart_0_rx_buffer+y+1)='\0';   
          }

#if SUART_0_WILL_USE_INTERRUPT  == 1
        sbi(SUART_0_INT_REG, SUART_0_INT_BIT);
#endif

return;  
}
/*####################################################################################################*/

void  suart_0_putc(unsigned char data)
{

   
   /* Disable global Interrupts */
   disable_global_int(); 
   SUART_0_PUTCHAR(data);
    /* Restore Global Interrupts to previus state */
   restore_global_int();
}
/*####################################################################################################*/

void suart_0_puts(unsigned char *tx_data)
/* send progmem  <progmem_tx_data> to suart */
{   
register unsigned char c=0;  

   
   /* Disable global Interrupts */
   disable_global_int();   
   while( (c=*tx_data++) ) SUART_0_PUTCHAR(c); 
   /* Restore Global Interrupts to previus state */
   restore_global_int(); 
}
/*####################################################################################################*/

void suart_0_puts_p(const char *progmem_tx_data)
/* send progmem  <progmem_tx_data> to suart */
{   
register unsigned char c=0;  
   
   /* Disable global Interrupts */
   disable_global_int();   
   while( (c=PRG_RDB(progmem_tx_data++)) ) SUART_0_PUTCHAR(c); 
   /* Restore Global Interrupts to previus state */
   restore_global_int();    
}
/*####################################################################################################*/

void suart_0_puts_e(unsigned char *eeprom_tx_data)
/* send progmem  <eeprom_tx_data> to suart */
{   
register unsigned char c=0;  
   
   /* Disable global Interrupts */
   disable_global_int();   
   while( (c=eeprom_rb((unsigned int)eeprom_tx_data++)) ) { if(c==0xFF) break; SUART_0_PUTCHAR(c); }
   /* Restore Global Interrupts to previus state */
   restore_global_int();       
}
/*####################################################################################################*/

void suart_0_puti(int value, unsigned char dot_position)
{
unsigned char data[7]={' ',' ',' ',' ',' ',' ',' ' }; 
unsigned char position=sizeof(data), radix=10; 
        

/* Some adjustments if the number is negative */
if(value<0) { value=-value; suart_0_putc('-'); } 

do{
     if(dot_position)
      {
         if((sizeof(data)-position)>=dot_position)
          {
             position--;
             *(data+position)=SUART_0_PUTI_DECIMAL_POINT;
             dot_position=0;
          }
      } 
     position--;
     *(data+position)=(value%radix)+'0';
     value/=radix;

  }while((value || dot_position)&& dot_position <= 5); 

          
/* Display the number on the LCD screen */
for(; position<sizeof(data); position++)
  {
     suart_0_putc(data[position]);
  }

return;
}
/*########################################################################################################*/
/*                                     T   H   E      E   N   D                                           */
/*########################################################################################################*/


