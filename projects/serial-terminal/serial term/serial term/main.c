// Remember to include avr313.xcl as your linker file. (Modified for AT90S8515)

//#include <pgmspace.h>
//#include <stdio.h>
//#include <stdlib.h>
#include <90s8515.h>
#include <delay.h>

//#include "serial.h"
#include "gpr.h"
#include "kb.h"
//#include "capture.h"
// Alphanumeric LCD Module functions
#asm
   .equ __lcd_port=0x18 ;PORTB  
   
   
#endasm
#include <lcd.h>

#define backlight PORTD.5
#define RXB8 1
#define TXB8 0
#define OVR 3
#define FE 4
#define UDRE 5
#define RXC 7

#define FRAMING_ERROR (1<<FE)
#define DATA_OVERRUN (1<<OVR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

// UART Receiver buffer
#define RX_BUFFER_SIZE 96
char rx_buffer[RX_BUFFER_SIZE];

#if RX_BUFFER_SIZE<256
unsigned char rx_wr_index,rx_rd_index,rx_counter;
#else
unsigned int rx_wr_index,rx_rd_index,rx_counter;
#endif

// This flag is set on UART Receiver buffer overflow
bit rx_buffer_overflow;

// UART Receiver interrupt service routine
interrupt [UART_RXC] void uart_rx_isr(void)
{
char status,data;
status=USR;
data=UDR;
if ((status & (FRAMING_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer[rx_wr_index]=data;
   if (++rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
   if (++rx_counter == RX_BUFFER_SIZE)
      {
      rx_counter=0;
      rx_buffer_overflow=1;
      };
   };
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the UART Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void)
{
char data;
//while (rx_counter==0);
if(rx_counter!=0)
{
data=rx_buffer[rx_rd_index];
if (++rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0;
#asm("cli")
--rx_counter;
#asm("sei")
return data;
}
else
return 0;
}
#pragma used-
#endif

// UART Transmitter buffer
#define TX_BUFFER_SIZE 8
char tx_buffer[TX_BUFFER_SIZE];

#if TX_BUFFER_SIZE<256
unsigned char tx_wr_index,tx_rd_index,tx_counter;
#else
unsigned int tx_wr_index,tx_rd_index,tx_counter;
#endif

// UART Transmitter interrupt service routine
interrupt [UART_TXC] void uart_tx_isr(void)
{
if (tx_counter)
   {
   --tx_counter;
   UDR=tx_buffer[tx_rd_index];
   if (++tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;
   };
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the UART Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c)
{
while (tx_counter == TX_BUFFER_SIZE);
#asm("cli")
if (tx_counter || ((USR & DATA_REGISTER_EMPTY)==0))
   {
   tx_buffer[tx_wr_index]=c;
   if (++tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0;
   ++tx_counter;
   }
else
   UDR=c;
#asm("sei")
}
#pragma used-
#endif

// Standard Input/Output functions
#include <stdio.h>
#include <stdlib.h>
 
flash unsigned char menuline[6][20]={
" A)9200  B)14400 ",
" C)19200 D)38400 ",
" E)56000 F)57600 ",
"     G)115200    ",
" M) Manual UBRR  ",
"Z)Backligh on/off"};
 
flash unsigned char  uparrow[8]={
0b0000100,
0b0001110,
0b0011111,
0b0000100,
0b0000100,
0b0000100,
0b0000100,
0b0000100};

flash unsigned char  Downarrow[8]={
0b0000100,
0b0000100,
0b0000100,
0b0000100,
0b0000100,
0b0011111,
0b0001110,
0b0000100};

flash unsigned char  f2[8]={
0b0011111,
0b0010000,
0b0011100,
0b0010111,
0b0010001,
0b0010010,
0b0010100,
0b0010111}; 

flash unsigned char  f3[8]={
0b0011111,
0b0010000,
0b0011100,
0b0010111,
0b0010001,
0b0010011,
0b0010001,
0b0010111};



void define_char(unsigned char flash *pc,unsigned char char_code)
{
unsigned int i1,a1;
a1=(char_code<<3) | 0x40;
for (i1=0; i1<8; i1++) lcd_write_byte(a1++,*pc++);
}



void menu (void);                     
int comunicate (void);
void cap_init(void);
void low_level_init(void);
void do_states (char);
void main(void)

{ 

int display;
PORTD=0x00;
DDRD=0x20;



     
    low_level_init();
    lcd_init(20);
    _lcd_ready();
    _lcd_write_data(0xf);
    
    
    
   // init_uart();    // Initializes the UART transmit buffer
    init_kb();        // Initialize keyboard reception
    cap_init();
   
        
    menu();
    while(1)
    {   
        
             
        display=1;
        while(display==1) 
        {   
                
                display=comunicate();
        }
        menu();
        
    }
}
     
void menu (void)

{ 
        //backlight=0;
        char  key1,out,i,i1,i2,h1,h2,loop=0;
        out=1;
        i=2;
        define_char(uparrow,0);
        define_char(f2,1);
        define_char(f3,2);
        define_char(Downarrow,3);
        

        
        lcd_clear();
        lcd_putchar(0);
        lcd_putsf("  WELCOME TO DANS");
        lcd_gotoxy(0,1);
        lcd_putchar(1);
        lcd_putsf("   LCD TERMINAL");
        lcd_gotoxy(0,2);
        lcd_putchar(2);
        lcd_putsf( menuline[i]);      
        lcd_gotoxy(0,3);
        lcd_putchar(3);
        i++;
        lcd_putsf( menuline[i]);
        _lcd_ready();
       _lcd_write_data(0xc);
        while (out==1)
        {
                key1=getchar_kb();
                while (key1==0)
                {         
                key1=getchar_kb();
                }
        
        
                switch (key1)
                {
                        case 'a':
                        UBRR=0x2f;          // 9600bps @ 3.6864 MHz
                        out=0;
                        break;
                
                        case 'b':
                        UBRR=0x1F;          // 14400bps @ 3.6864 MHz
                        out=0;
                        break;
                
                        case 'c':
                        UBRR=0x17;          //19200 @ 3.6864 MHz
                        out=0;
                        break;
                
                        case 'd':
                        UBRR=0x0b;
                        out=0;
                        break;
                
                        case 'e':
                        UBRR=0x07;
                        out=0;
                        break;
                
                        case 'f':
                        UBRR=0x07;
                        out=0;
                        break;
                
                        case 'g':
                        UBRR=0x03;
                        out=0;
                        break;
                
                       
                
                        case 0xff:
                        key1=0;
                                if (i-1>0)
                                {
                                        i--;
                                        i--;
                                        lcd_gotoxy(1,2);
                                        lcd_putsf( menuline[i]);
                                        i++;
                                        lcd_gotoxy(1,3);
                                        lcd_putsf( menuline[i]);
                                        break;
                                }
                                else
                                 break;
                        
                                
                
                
                
                        case 0xfe:
                        key1=0;
                                if (i<7)
                                {
                                        
                                        
                                        lcd_gotoxy(1,2);
                                        lcd_putsf( menuline[i]);
                                        i++;
                                        lcd_gotoxy(1,3);
                                        lcd_putsf( menuline[i]);
                                        break;
                                }
                                else
                                 break;
                        
                
                        break;
                
                        case 0:
                        out=1;
                        break;
                        
                        case 'm':
                        out=2;
                        loop=0;
                        UBRR=0x17;
                        break;
                                 
                        case 'z':
                       backlight=!backlight;
                        break;
                
                        default:
                        out=1;
                        break;
                }
        }       
        lcd_clear();
        if(out==0)
        return;
        else
        { 
                if(out==2)
                
                {
                while(loop!=1)
                {
                        lcd_clear();
                        lcd_putchar(0);
                        lcd_putsf(" PlEASE ENTER THE");
                        lcd_gotoxy(0,1);
                        lcd_putchar(1);
                        lcd_putsf("  HEX UBRR U WANT");
                        lcd_gotoxy(0,2);
                        lcd_putchar(2);      
                        lcd_gotoxy(0,3);
                        lcd_putchar(3);
                        lcd_putsf("  UBRR=0x" );
                        _lcd_ready();
                        _lcd_write_data(0xf);
                        key1=getchar_kb();
                        while (key1==0)
                        {         
                        key1=getchar_kb();
                        }
                        lcd_putchar(key1);
                        i1=key1;
                        key1=0;
                        while (key1==0)
                        {         
                        key1=getchar_kb();
                        }
                        lcd_putchar(key1);
                        i2=key1;
                        lcd_clear();
                        lcd_putchar(0);
                        lcd_putsf(" YOU HAVE ENTERED");
                        lcd_gotoxy(0,1);
                        lcd_putchar(1);
                        lcd_putsf(" '0x");
                        lcd_putchar(i1);
                        lcd_putchar(i2);
                        lcd_putsf("' PRESS ENTER");
                        lcd_gotoxy(0,2);
                        lcd_putchar(2);
                        lcd_putsf(" TO USE THIS UBRR");      
                        lcd_gotoxy(0,3);
                        lcd_putchar(3);
                        lcd_putsf(" OR F5 TO RE-ENTER");
                        _lcd_ready();
                        _lcd_write_data(0xc);
                        while (key1!=0xfd)
                        {
                        if(key1==0x0d)
                        {       
                                h1=i1-0x30;
                                h2=i2-0x30;
                                i1=h1*16;
                                h1=i1+h2; 
                                UBRR=0x17;
                                loop=1;
                                lcd_clear();
                                i=0;
                                return;
                        }         
                        key1=getchar_kb();
                        }
                        
                        }
                        
                        
                }
                    
        }
        
        
        
        
        
} 

