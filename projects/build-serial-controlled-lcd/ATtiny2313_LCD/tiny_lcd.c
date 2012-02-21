// ***********************************************************
// Project:tiny_lcd
// Author: E. Klaus
// Module description: LCD module that accepts TTL level serial 
// or TWI input and writes to LCD.  Can also be used as a 
// mini development system.
// 02/08/12  v1.0 initial release
// 02/20/12  v1.1 minor bug fixes 
// ***********************************************************
#define F_CPU 8000000UL  //** NOTE: Unprogram CKDIV8 fuse bit on attiny2313 **

#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "lcd.h"           // by Peter Fleury    **modified to reduce size 
#include "usiTwiSlave.h"   // by Donald R. Blake

#define INBYTE_FROM_UART 1
#define INBYTE_FROM_TWI  2

void delay10ms(uint16_t count);

char input_data_ready(void);
char get_input_byte(char bWait);
void dev_port_test(void);
char read_dev_button(void);

static unsigned char i,k,row,col, inType;
static char buffer[17]="\n\rLCD Ready...\0\r";
static char lfcr[3]="\n\r";

int main(void)
{
  lcd_init(LCD_DISP_ON);  /* initialize display, cursor off */
  USART_init();           // ** Init the UART 
  usiTwiSlaveInit();      // ** Init the TWI slave systems **
  sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed
  
  inType=0;  
  row=0;
  col=0;
  
  lcd_command(1<<LCD_CLR);  // Clear LCD
  
  // ** Flash LED 3x ***
  DDRB |=0x10;             // PB4 = OUTPUT  
  for(i=0; i< 3; i++)
    {
     PORTB |= 0x10;            // PB4 = HIGH
     delay10ms(20);            // Delay 200ms
     PORTB &= 0xEF;            // PB4 = LOW
     delay10ms(20);            // Delay 200ms
    }
  DDRB &=0xEF;                 // PB4 = INPUT
  
  // Set I/O mode for dev button input
  DDRD &=0xDF;                // PD5 = INPUT
  PORTD |= 0x20;              // PD5 = enable pull-up
  
  //UART_put_str(buffer);     // Send Ready Message to UART
  
  lcd_puts((char *)&buffer[2]);  // Send Ready Message to LCD (No LF/CR)
  
  buffer[14]='\n';            // Add LF + CR
  UART_put_str(buffer);       // Send Ready Message to UART
  
  while(input_data_ready())  // wait for NO Data. (flush input stream)
       get_input_byte(0);
        
  k=0xFF;                   // Set this to detect first byte recieved.
  
  for (;;)    /* loop forever */ 
   {
     
       if(input_data_ready())  //Any data from USART or TWI?
          {
           if(k==0xFF)       // Is the the very first byte recieved after reset?
             {               // if so the clear the ready message from the LCD 
              lcd_command(1<<LCD_CLR);
              k=0;           // Reset the indicator
             }
             
           k = get_input_byte(0);  // Get the byte recieved
           
           if(k==0x1b) // esc?
             {
              k = get_input_byte(1);  // Get the escape code 
              switch(k)
               {
                case 'c': //esc-c = Clear Screen
                  lcd_command(1<<LCD_CLR);
                  row=0;
                  col=0;
                  k=0;
                break;
                
                case 'h': //esc-h = HOME Cursor
                  lcd_command(1<<LCD_HOME);
                  row=0;
                  col=0;
                  k=0;
                break;

                case 'k': //esc-k CURSOR OFF
                  lcd_command(LCD_DISP_ON );
                  k=0;
                break;
                
                case 'K': //esc-K CURSOR ON
                  lcd_command(LCD_DISP_ON_CURSOR );
                  k=0;
                break;
                
                case 'p': //esc-p r c = Set Cursor Position to row col
                   k = get_input_byte(1);  //expects '0' or '1'''
                   row=k &0x01;
                   
                   k = get_input_byte(1);   //expects '0'-'9' or 'A'-'F' (11-15)
                   if(k>'9')
                     col = (k-'A')+10;
                   else
                     col = k-'0';
                     
                   col &=0x0F;                     
                   lcd_gotoxy(col, row); 
                   k=0;        
                break; 
                
                case 'u':  //dev port test - toggle state of pins & LED 5x
                  dev_port_test();
                  k=0;
                break;
               }
               
               if(inType==INBYTE_FROM_UART)
                  UART_put_str(lfcr);    //echo LF & CR (UART ONLT)
                  
              } //END if(k==0x1b)
            
            if((k=='\n')||(k=='\r'))  // newline or carrage return
              {
                col=0;                // position to start of row
                if(row==0)            // if at top row goto bottom row
                  row=1;
                  
                lcd_gotoxy(col, row);  // set new positions
                
                if(inType==INBYTE_FROM_UART) 
                  UART_put_chr(k);     //echo byte (UART ONLT)
                  
                k=0;
              }  
              
            if(k!=0)   // If Not already processed
              {
               lcd_putc(k);
               col++;
               if(inType==INBYTE_FROM_UART)
                  UART_put_chr(k);  //echo byte (UART ONLT)

               if(col>15)         
                 {
                  col=0;
                  row++;
                  if(row>1)
                    row=0;
                    
                  lcd_gotoxy(col, row);
                  
                  if(inType==INBYTE_FROM_UART)
                     UART_put_str(lfcr); //echo byte (UART ONLT)
                 }
              } //END else - if(k!=0) 
              
          }  //END if(input_data_ready())
          
          
        // ***** Test the dev button *****  
        if(read_dev_button())   // Check the dev button if pressed for >50ms
          {
            UART_put_chr('@');   //send '@' 0x40 (UART ONLY)
            lcd_putc('@');       // display "@"  
          }  

   } // END loop forever   

 } //END main()

// ***********************************************************************
//  delay10ms(uint16_t count)
//  Delay 10ms * count
// ***********************************************************************
void delay10ms(uint16_t count)
 {
  uint16_t i;
  for(i=0; i<count; i++)       // Delay 10ms * count
       _delay_ms(10);
 }

// ***********************************************************************
//  input_data_ready(void)
//  If either UART or TWI data ready return a non-zero code
//  Otherwise return zero.
// ***********************************************************************
char input_data_ready(void)
{
 inType = 0;

 if(rbuflen() >0)
     inType = INBYTE_FROM_UART;
 else
    if(usiTwiDataInReceiveBuffer())
       inType = INBYTE_FROM_TWI;
    
 return inType;         
} 

// ***********************************************************************
//  get_input_byte(void)
//  get a waiting byte from UART or TWI ** if bWait = 1 wait for data.
// ***********************************************************************
char get_input_byte(char bWait)
{
 if(bWait)
   while(!input_data_ready());
    
 if(rbuflen() >0)
    return UART_get_chr();

 if(usiTwiDataInReceiveBuffer())
    return usiTwiReceiveByte();
    
 return 0;         
} 
 
// ***********************************************************************
// dev_port_test(void)
// toggle state of pins & LED 5x
// ***********************************************************************
void dev_port_test(void)
  {
    uint8_t x;
    DDRA |=0x03;   // PA0 & PA1 = OUTPUT
    DDRB |=0x10;   // PB4 = OUTPUT
    DDRD |=0x60;   // PD5 & PD6 = OUTPUT
    for(x=0; x<5; x++)
       {
        PORTA |= 0x03;  // PA0 & PA1 = HIGH
        PORTB |= 0x10;  // PB4 = HIGH
        PORTD |= 0x60;  // PA0 & PA1 = HIGH
        delay10ms(25);  // Delay 1/4 sec.
        PORTA &= 0xFC;  // PA0 & PA1 = LOW
        PORTB &= 0xEF;  // PB4 = LOW
        PORTD &= 0x9F;  // PD5 & PD6 = LOW
        delay10ms(25);  // Delay 1/4 sec.
       }
    DDRA &=0xFC;   // PA05 & PA1 = INPUT
    DDRB &=0xEF;   // PB4 = INPUT
    DDRD &=0x9F;   // PD5 & PD6 = INPUT
    PORTD |= 0x20; // PD5 = enable pull-up (dev button)
  }

// ***********************************************************************
// read_dev_button(void)
// Return 1 if button pressed for at least 50ms (debounce)
// ***********************************************************************
char read_dev_button(void)
{
 uint8_t x=0;
 if(!(PIND & 0x20))  // If Button Pressed
   {
    delay10ms(1);          // Delay 10ms
    while(!(PIND & 0x20))  // Button must be down for 50ms
      {
       delay10ms(1);  // Delay 10ms
       if(x++ > 100)  // Don't wait more than 1 sec. for release
           break;
       }
      
    if(x > 4)         // If button pressed for at least 50ms
      return 1;       // return YES! 
   }
     
 return 0;        // return NO        
}