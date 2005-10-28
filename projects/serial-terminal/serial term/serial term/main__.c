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
   .equ __lcd_port=0x1B ;PORTA
#endasm
#include <lcd.h>


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
 
flash unsigned char menuline[8][20]={
"  A)110   B)300  ",
"  C)1200  D)2400 ",
"  E)4800  F)9600 ",
"  G)14400 H)19200",
"  I)38400 J)57600",
"     k)115200    ",
" M) Manual UBRR  ",
" Z)Lock          "};
 
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
                        UBRR=0x17;          // 9600bps @ 3.6864 MHz
                        out=0;
                        break;
                
                        case 'b':
                        UBRR=0x0F;          // 14400bps @ 3.6864 MHz
                        out=0;
                        break;
                
                        case 'c':
                        UBRR=0x0B;          //19200 @ 3.6864 MHz
                        out=0;
                        break;
                
                        case 'd':
                        UBRR=0x05;
                        out=0;
                        break;
                
                        case 'e':
                        UBRR=0x05;
                        out=0;
                        break;
                
                        case 'f':
                        UBRR=0x05;
                        out=0;
                        break;
                
                        case 'g':
                        UBRR=0x05;
                        out=0;
                        break;
                
                        case 'h':
                        UBRR=0x05;
                        out=0;
                        break;
                
                        case 'i':
                        UBRR=0x05;
                        out=0;
                        break;
                
                        case 'j':
                        UBRR=0x05;
                        out=0;
                        break; 
                
                        case 'k':
                        UBRR=0x05;
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
                        out=0;
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

#include "gpr.h"

void print_hexbyte(unsigned char i)
{
    unsigned char h, l;
    
    h = i & 0xF0;               // High nibble
    h = h>>4;
    h = h + '0';
    
    if (h > '9')
        h = h + 7;

    l = (i & 0x0F)+'0';         // Low nibble
    if (l > '9')
        l = l + 7;


    putchar(h);
    putchar(l);
}


//void delay(char d)
//{
//    char i,j,k;
//    for(i=0; i<d; i++)
//        for(j=0; j<40; j++)
//            for(k=0; k<176; k++);
//}

//#include <pgmspace.h>
#include "kb.h"
//#include "serial.h"
#include "gpr.h"

#include "scancodes.h"

#define BUFF_SIZE 64

unsigned char edge, bitcount;                // 0 = neg.  1 = pos.

unsigned char kb_buffer[BUFF_SIZE];
unsigned char *inpt, *outpt;
unsigned char buffcnt;


void init_kb(void)
{
    inpt =  kb_buffer;                        // Initialize buffer
    outpt = kb_buffer;
    buffcnt = 0;

    MCUCR = 2;                                // INT0 interrupt on falling edge
    edge = 0;                                // 0 = falling edge  1 = rising edge
    bitcount = 11;
}

interrupt [EXT_INT0] void ext_int0_isr(void)
{
    static unsigned char data;                // Holds the received scan code

    if (!edge)                                // Routine entered at falling edge
    {
        if(bitcount < 11 && bitcount > 2)    // Bit 3 to 10 is data. Parity bit,
        {                                    // start and stop bits are ignored.
            data = (data >> 1);
            if(PIND & 8)
                data = data | 0x80;            // Store a '1'
        }

        MCUCR = 3;                            // Set interrupt on rising edge
        edge = 1;
        
    } else {                                // Routine entered at rising edge

        MCUCR = 2;                            // Set interrupt on falling edge
        edge = 0;

        if(--bitcount == 0)                    // All bits received
        {
            decode(data);
            bitcount = 11;
        }
    }
}


void decode(unsigned char sc)
{
    static unsigned char is_up=0, shift = 0, mode = 0, scrollup = 0,ctrl =0;
    unsigned char i;

    if (!is_up)                // Last data received was the up-key identifier
    {
        switch (sc)
        {
              
                
          case 0xF0 :        // The up-key identifier
            is_up = 1;
            break;
                   
          case 0x14 :       //ctrl
            ctrl = 1;
            break;
            
          case 0x12 :        // Left SHIFT
            shift = 1;
            break;

          case 0x59 :        // Right SHIFT
            shift = 1;
            break;

          case 0x05 :        // F1
            if(mode == 0)
                mode = 1;    // Enter scan code mode
            if(mode == 2)
                mode = 3;    // Leave scan code mode
            break;

          default:
            if(mode == 0 || mode == 3)        // If ASCII mode
            {  
                if(ctrl == 1)
                {   
                    if(sc == 0x21)
                    putchar(0x03);
                }
                if(!shift )                    // If shift not pressed,
                {                            // do a table look-up
                    for(i = 0; unshifted[i][0]!=sc && unshifted[i][0]; i++);
                    if (unshifted[i][0] == sc) 
                    {
                        put_kbbuff(unshifted[i][1]);
                    }
                } 
                else 
                {   
                    
                                    // If shift pressed
                        for(i = 0; shifted[i][0]!=sc && shifted[i][0]; i++);
                        if (shifted[i][0] == sc) 
                            {
                                put_kbbuff(shifted[i][1]);
                            }

                    
                    
                }
            } else{                            // Scan code mode
                print_hexbyte(sc);            // Print scan code
                put_kbbuff(' ');
                put_kbbuff(' ');
            }
            break;
        }
    } else {
        is_up = 0;
                                    // Two 0xF0 in a row not allowed
        switch (sc)
        {
        case 0x14:              //left  ctrl
        ctrl = 0;
        break;
        
          case 0x12 :                        // Left SHIFT
            shift = 0;
            break;
            
          case 0x59 :                        // Right SHIFT
            shift = 0;
            break;

          case 0x05 :                        // F1
            if(mode == 1)
                mode = 2;
            if(mode == 3)
                mode = 0;
            break;
          case 0x06 :                        // F2
            put_kbbuff(0xff);
           // printf("scroll up");
           break;
           case 0x04:
            put_kbbuff(0xfe);
            break;
            case 0x03:
            put_kbbuff(0xfd);
            break;      
        } 
    }    
} 

void put_kbbuff(unsigned char c)
{
    if (buffcnt<BUFF_SIZE)                        // If buffer not full
    {
        *inpt = c;                                // Put character into buffer
        inpt++;                                    // Increment pointer

        buffcnt++;

        if (inpt >= kb_buffer + BUFF_SIZE)        // Pointer wrapping
            inpt = kb_buffer;
    }
}

int getchar_kb(void)
{
    int byte;  
   
    //while(buffcnt == 0);                        // Wait for data
    if(buffcnt == 0)
    {         
    byte=0;
    return byte;
    }
    else
    { 
    byte = *outpt;                                // Get byte
    outpt++;                                    // Increment pointer

    if (outpt >= kb_buffer + BUFF_SIZE)            // Pointer wrapping
        outpt = kb_buffer;
    
    buffcnt--;                                    // Decrement buffer count

    return byte;
    }
}


//#include <ina90.h>
#include <90s8515.h>

void low_level_init(void)
{
        
        UBRR=0x17;          // 19200bps @ 4 MHz
    //UCR  = 0x08;        // RX enable, TX enable
    GIMSK= 0x40;        // Enable INT0 interrupt
    //UCR=0x58; 
    UCR=0xD8; 

#asm("sei")
    //return 1;
}

#include "capture.h"
void cap_init (void)
{
j=0;
i=0;
}
void do_states (char s)
{

           
                //if(up_BN==0)
                //////printf("it worked");            
              
                  
                state=bla;
                if(s==0x0a )
                state=crlf;
                if(s==0x08)
                state=bkspce;
                if(s==0x0d || s==0)
                state=ignor;
                if(s==0xff)
                state=scrollup;
                if(s==0xfe)
                state=scrolldown;
                switch(state)
                { 
                      
                        case crlf:
                        if(wasscroll==1)
                        {
                                //printf("wasscroll just got cleared by crlf");
                                j=jstore;
                                i=istore;
                                lcd_clear();
                                        
                                        if((j-3)>-1)
                                        {
                                                 lcd_puts(line[j-3]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-3]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                                                 
                                        lcd_gotoxy(0,3);
                                        lcd_puts(line[j]);
                                        wasscroll=0;
                        }
                        
                        ////printf("CRLF\n\r");
                        for(x=20-i;x!=0;x--)                 //this just fills the remainng of line with spaces
                        {                                                                                                                                                                                                                                                                                                
                                line[j][i]=0x20;
                                i++;
                                ////printf("%i",i);   
                        }
                        i=0;
                        if (lineflag==3)
                        {
                                
                                
                                        lcd_clear();
                                        
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        lcd_puts(line[j]);                         
                                        lcd_gotoxy(0,3);
                                ////printf("crlf line 1\n\r'%s'\n\r'                '\n\r",line[j]);
                                ////printf("line number is %i\ni is %i\n\r",j,i);
                                if((j+1)<buffersize)
                                j++;
                                else
                                j=0;
                                lcd_gotoxy(0,3);
                                break;
                                
                        }
                        else 
                        
                        {
                                //lcd_clear();
                                ////printf("crlf line 0\n\r'%s'\n\r'                '\n\r",line[j]);
                                ////printf("line number is %i\ni is %i\n\r",j,i);
                                if((j+1)<buffersize)
                                j++;
                                else
                                j=0;
                                lineflag++;
                                lcd_gotoxy(i,lineflag);
                        } 
                              
                        break; 
                        
                        case bla:
                        if(wasscroll==1)
                        {
                                //printf("wasscroll just got cleared by crlf");
                                j=jstore;
                                i=istore;
                                lcd_clear();
                                        
                                        if((j-3)>-1)
                                        {
                                                 lcd_puts(line[j-3]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-3]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                                                 
                                        lcd_gotoxy(0,3);
                                        lcd_puts(line[j]);
                                        wasscroll=0;
                        }
                        
                        if(s<0x20)
                        break;
                        if(i<20)
                        {       
                                line[j][i]=s;
                                ////printf("buffer was updated j=%i i=%i s=%c\n\r",j,i,s); 
                                lcd_gotoxy(i,lineflag);
                                lcd_putchar(s);
                                if(i==19)
                                {
                                if(lineflag!=3)
                                lcd_gotoxy(0,lineflag+1);
                                else
                                {
                                _lcd_ready();
                                _lcd_write_data(0x0c);
                                }
                                }
                        
                                
                                ////printf("buffer contains \n\r'%s'\n\r",line[j]);
                                i++;  
                                ////printf("%i",i);
                        }  
                        else
                        {       
                                if(lineflag!=3)
                                {
                                        if((j+1)<buffersize)
                                            j++;
                                        else
                                            j=0;
                                        ////printf("line down j is %i\n\r",j);
                                        
                                        lineflag++;
                                        //printf("%i",lineflag);
                                        lcd_gotoxy(0,lineflag);
                                        lcd_putchar(s);
                                        i=1; 
                                        line[j][0]=s;                                        
                                        break;
                                }
                                else
                                {      


                                
                                        lcd_clear();
                                        
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        lcd_puts(line[j]);                         
                                        lcd_gotoxy(0,3);
                                        
                                        
                                        ////printf("j is currently %i\n\r",j);
                                        ////printf("buffern contains \n\r%s\n\r",line[j]);
                                        
                                        if((j+1)<buffersize)
                                        j++;
                                        else
                                        j=0;
                                        lcd_putchar(s);
                                        line[j][0]=s;
                                        i=1;
                                        
                                        ////printf("line over flow \n%s\n\r",line[j]);
                                        ////printf("line number is %i\ni is %i\n\r",j,i);       
                                }
                        }
                        
                        break;
                        case bkspce:
                        if(wasscroll==1)
                        {
                                //printf("wasscroll just got cleared by crlf");
                                j=jstore;
                                i=istore;
                                lcd_clear();
                                        
                                        if((j-3)>-1)
                                        {
                                                 lcd_puts(line[j-3]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-3]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                                                 
                                        lcd_gotoxy(0,3);
                                        lcd_puts(line[j]);
                                        wasscroll=0;
                        }
                        if(i!=0) //if you are not at the start of a line
                        {      
                                
                                i--;
                                
                                lcd_gotoxy(i,lineflag);
                                lcd_putchar(0x20);
                                lcd_gotoxy(i,lineflag);
                                line[j][i]=(0x20);
                        }
                        else
                        {     printf("%i",lineflag);          
                                        if(lineflag!=0)
                                        lineflag--;
                                        else
                                        {
                                                if(j>0)
                                                j--;
                                                else
                                                j=buffersize-1;
                                                lcd_puts(line[j]);
                                        }
                                        i=19;
                                        lcd_gotoxy(i,lineflag);
                                        lcd_putchar(0x20);
                                        line[j][i]=0x20;
                                        lcd_gotoxy(i,lineflag);      
                                        if(j>0)
                                        j--;
                                        else
                                        j=buffersize-1;
                                        
                                        
                                        
                                
                                
                        }
                        
                                break;

                        
                        case ignor:
                        break;
 
                        case scrolldown :
                        
                        
                                if (wasscroll==0)
                                { 
                                //printf("wasscroll just got set");
                                      wasscroll=1;      
                                
                                        istore=i;
                                        jstore=j; 
                                        for(x=20-i;x!=0;x--)                 //this just fills the remainng of line with spaces
                        {                                                                                                                                                                                                                                                                                                
                                line[j][i]=0x20;
                                i++;
                                ////printf("%i",i);   
                        }

                                }
                                        
                                
                            if((j+1)<buffersize)
                                        j++;
                            else
                                        j=0;
                             lcd_clear();
                             
                                //printf("\n\r%s \n\r",line[j-1]);
                                       lcd_clear();
                                        
                                        if((j-3)>-1)
                                        {
                                                 lcd_puts(line[j-3]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-3]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                                                 
                                        lcd_gotoxy(0,3);
                                        lcd_puts(line[j]);
                             //printf("%s \n\r",line[j]);
                             //printf("j is %i\n\r",j);
                             break;
                             
                             
                             case scrollup:
                        
                        
                        
                                if (wasscroll==0)
                                { 
                                //printf("wasscroll just got set");
                                      wasscroll=1;      
                                
                                        istore=i;
                                        jstore=j;
                                        for(x=20-i;x!=0;x--)                 //this just fills the remainng of line with spaces
                        {                                                                                                                                                                                                                                                                                                
                                line[j][i]=0x20;
                                i++;
                                ////printf("%i",i);   
                        }
                                }
                                        
                                
                             if(j>0)   
                                    j--;
                             else
                                   j=buffersize-1;
                                    
                             lcd_clear();
                                        
                                        if((j-3)>-1)
                                        {
                                                 lcd_puts(line[j-3]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-3]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                                                 
                                        lcd_gotoxy(0,3);
                                        lcd_puts(line[j]);
                             //printf("%s \n\r",line[j]);
                             //printf("j is %i\n\r",j);
                             break;        
                        default:
                        break;
                }
                        
             
      
      

      
}
 unsigned char key;
int comunicate (void)

{
       key=getchar_kb();
       if(key==0xfd)
       return 0;
       else
       
       if(key==0xff || key == 0xfe)
        do_states(key);
       else
       {
       if(key!=0 )
       {
        //putchar(key);
        //key=getchar();
        if(key==13)
        {
                //putchar(13);
                putchar(10);
        }
        else 
        
        putchar(key);       
        //do_states(key);
       }
       } 
       //key=getchar();
       //if(key!=1)
       //{
       
       key=getchar();
       if(key==0x1b)
              {        
                //while (key!=0x6d)
                //{
                  //      key=getchar();
                //}
       }
       else
       {
               if(key!=0 && key!=0xff && key!=0x0fe)
               //printf("%c",key);
               {
               do_states(key);
               }
       }  
       
       return 1;
    }
