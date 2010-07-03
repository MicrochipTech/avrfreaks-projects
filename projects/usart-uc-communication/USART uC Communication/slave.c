/**
    author: saripudin (saripudin2@gmail.com)
    ver: 03 Juli 2010
    @desc: this is uC slave software
    
*/  
          
#include <mega8.h> 
#include <stdio.h>
#include <delay.h> 
#include <stdlib.h>    

#asm
   .equ __lcd_port=0x18 ;PORTB
#endasm
#include <lcd.h>


void main(){
   unsigned char data; 
   char lcd[15];
   
   
   PORTB=0x00;DDRB=0x00;  
   PORTC=0x00;DDRC=0x00; 
   PORTD=0x00;DDRD=0x00;
      
   // USART initialization NO INTERRUPT
   // Communication Parameters: 8 Data, 1 Stop, No Parity
   // USART Receiver: On
   // USART Transmitter: On
   // USART Mode: Asynchronous
   // USART Baud Rate: 9600
   UCSRA=0x00;
   UCSRB=0x18;
   UCSRC=0x86;
   UBRRH=0x00;
   UBRRL=0x47; 
   
   lcd_init(16);          
   while(9)
   {
   data=getchar();
   
   if(data==19)
   {
        itoa(data,lcd);
        lcd_putsf("receive : ");
        lcd_puts(lcd);
        lcd_gotoxy(0,1);
        
        lcd_putsf("transmit : 38");
        putchar(38);
        delay_ms(2000);
   } 
   
   };    
   //-------------------------End of yours
}//end main