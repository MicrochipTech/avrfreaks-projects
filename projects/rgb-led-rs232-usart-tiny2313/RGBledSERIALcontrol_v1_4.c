/*****************************************************
Project : RGBled2014 USART controlled
Version : 1.4
Date    : 10.1.2014
Author  : Elias Paloniemi
Company : None
Comments: 
UART


Chip type               : ATtiny2313
AVR Core Clock frequency: 8,000000 MHz
Memory model            : Tiny
External RAM size       : 0
Data Stack size         : 32
*****************************************************/

#include <tiny2313.h>
#include <delay.h>
#include <stdlib.h>
// Standard Input/Output functions
#include <stdio.h>

#ifndef RXB8
#define RXB8 1
#endif

#ifndef TXB8
#define TXB8 0
#endif

#ifndef UPE
#define UPE 2
#endif

#ifndef DOR
#define DOR 3
#endif

#ifndef FE
#define FE 4
#endif

#ifndef UDRE
#define UDRE 5
#endif

#ifndef RXC
#define RXC 7
#endif

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

// USART Receiver buffer
#define RX_BUFFER_SIZE 8
char rx_buffer[RX_BUFFER_SIZE];

#if RX_BUFFER_SIZE <= 256
unsigned char rx_wr_index,rx_rd_index,rx_counter;
#else
unsigned int rx_wr_index,rx_rd_index,rx_counter;
#endif

// This flag is set on USART Receiver buffer overflow
bit rx_buffer_overflow;

// USART Receiver interrupt service routine
interrupt [USART_RXC] void usart_rx_isr(void)
{
char status,data;
status=UCSRA;
data=UDR;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer[rx_wr_index++]=data;
#if RX_BUFFER_SIZE == 256
   // special case for receiver buffer size=256
   if (++rx_counter == 0) rx_buffer_overflow=1;
#else
   if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
   if (++rx_counter == RX_BUFFER_SIZE)
      {
      rx_counter=0;
      rx_buffer_overflow=1;
      }
#endif
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void)
{
char data;
while (rx_counter==0);
data=rx_buffer[rx_rd_index++];
#if RX_BUFFER_SIZE != 256
if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0;
#endif
#asm("cli")
--rx_counter;
#asm("sei")
return data;
}
#pragma used-
#endif

// USART Transmitter buffer
#define TX_BUFFER_SIZE 8
char tx_buffer[TX_BUFFER_SIZE];

#if TX_BUFFER_SIZE <= 256
unsigned char tx_wr_index,tx_rd_index,tx_counter;
#else
unsigned int tx_wr_index,tx_rd_index,tx_counter;
#endif

// USART Transmitter interrupt service routine
interrupt [USART_TXC] void usart_tx_isr(void)
{
if (tx_counter)
   {
   --tx_counter;
   UDR=tx_buffer[tx_rd_index++];
#if TX_BUFFER_SIZE != 256
   if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;
#endif
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c)
{
while (tx_counter == TX_BUFFER_SIZE);
#asm("cli")
if (tx_counter || ((UCSRA & DATA_REGISTER_EMPTY)==0))
   {
   tx_buffer[tx_wr_index++]=c;
#if TX_BUFFER_SIZE != 256
   if (tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0;
#endif
   ++tx_counter;
   }
else
   UDR=c;
#asm("sei")
}
#pragma used-
#endif

// Declare your global variables here

void serial_print_RGB(void)
{  
    printf("RED: %0u GRE: %0u BLU: %0u             \r",OCR0A,OCR0B,OCR1AL);
}           

void automode1(void)
{
    unsigned int viiveam1=500;
    while(rx_buffer[0] == 0x61)
    {
        OCR0A=81;
        delay_ms(viiveam1);
        OCR0A=0;
        //delay_ms(viiveam1);
        OCR0B=100;
        delay_ms(viiveam1);
        OCR0B=0;
        //delay_ms(viiveam1);
        OCR1AL=255;
        delay_ms(viiveam1);
        OCR1AL=0;
        //delay_ms(viiveam1);
    }
}
void automode2(void)
{
    unsigned int viiveam2 = 200;
    while(rx_buffer[0] == 0x73)
    {
        OCR0A=rand();
        if(OCR0A > 81)
        {
            OCR0A=81;
        }
        OCR0B=rand();
        if(OCR0B > 100)
        {
            OCR0B=100;
        }
        OCR1AL=rand();
        delay_ms(viiveam2);
        while(viiveam2 > 600)
        {
            viiveam2=rand();
        }
        if(viiveam2 < 100)
        {
            viiveam2=100;
        }
    }
    OCR0A=0;
    OCR0B=0;
    OCR1AL=0;       
}
void automode3(void)
{
    unsigned int viiveam3 = 10;
    unsigned int viiveam3end=viiveam3*15;
    while(OCR0A < 81)
    {
        OCR0A++;
        delay_ms(viiveam3);
    }
    while(rx_buffer[0] == 0x64)
    {
        while(OCR0B < 100)        
        {
            if(OCR0A > 0)
            {
                OCR0A--;
            }
            OCR0B++;
            delay_ms(viiveam3);
        }
        while(OCR1AL < 255)
        {
            if(OCR0B > 0)
            {
                OCR0B--;
            }
            OCR1AL++;
            delay_ms(viiveam3);
        }
        while(OCR0A < 81)
        {
            OCR0A++;
            delay_ms(viiveam3);
        }
        while(OCR0B < 100)
        {
            OCR0B++;
            delay_ms(viiveam3);
        }
        while(OCR0B + OCR1AL > 0)
        {
            if(OCR0B > 0)
            {
                OCR0B--;
            }
            OCR1AL--;
            delay_ms(viiveam3);
        }
        /*
        while(OCR1AL > 0)
        {
            OCR1AL--;
            OCR0A++;
            delay_ms(viiveam3);
        }
        */
        delay_ms(viiveam3end);
    }
    while(OCR0A > 0)
    {
        OCR0A--;
        delay_ms(viiveam3);
    }
}

void main(void)
{
// Declare your local variables here
//unsigned int viive=50;
char rx_char=0;
//char tx_char=0;
//char tx_data[] = "RGB LED\r\n";
//int red=OCR0A;
//int gre=OCR0B;
//int blu=OCR1A;

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port A initialization
// Func2=In Func1=In Func0=In 
// State2=T State1=T State0=T 
PORTA=0x00;
DDRA=0x00;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=Out Func3=Out Func2=Out Func1=In Func0=In 
// State7=T State6=T State5=T State4=0 State3=0 State2=0 State1=T State0=T 
PORTB=0x00;
DDRB=0x1C;

// Port D initialization
// Func6=In Func5=Out Func4=In Func3=In Func2=In Func1=In Func0=In 
// State6=T State5=0 State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x22;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=0xF1;
TCCR0B=0x02;
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0xF1;
TCCR1B=0x02;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0xFF;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// Interrupt on any change on pins PCINT0-7: Off
GIMSK=0x00;
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

// Universal Serial Interface initialization
// Mode: Disabled
// Clock source: Register & Counter=no clk.
// USI Counter Overflow Interrupt: Off
USICR=0x00;

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 19200
UCSRA=0x00;
UCSRB=0xD8;
UCSRC=0x06;
UBRRH=0x00;
UBRRL=0x19;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
DIDR=0x00;

// Global enable interrupts
#asm("sei")

//send initial text to terminal screen
//puts(tx_data);
printf("RGB LED\r\n\n");
printf("RED/r & t/ GRE/g & h/ BLU/b & n/ /a/ /s/ /d/\r\n\n");

    while (1)
    {   
        serial_print_RGB();
        rx_char=getchar();
        if(rx_char == 0x61)
        {
            printf("Auto mode 1                                 \r");
            automode1();
        }
        if(rx_char == 0x73)
        {
            printf("Auto mode 2                                 \r");
            automode2();
        }
        if(rx_char == 0x64)
        {
            printf("Auto mode 3                                 \r");
            automode3();
        }   

        while(rx_char == 0x72 && OCR0A > 0)        
        {
            rx_char=getchar();
            OCR0A--;
            serial_print_RGB();
            //delay_ms(viive);
        }
        while(rx_char == 0x74 && OCR0A < 255)
        {
            rx_char=getchar();
            OCR0A++;
            serial_print_RGB();
            //delay_ms(viive);
        }
        while(rx_char == 0x67 && OCR0B > 0)
        {
            rx_char=getchar();
            OCR0B--;
            serial_print_RGB();
            //delay_ms(viive);
        }
        while(rx_char == 0x68 && OCR0B < 255)
        {
            rx_char=getchar();
            OCR0B++;
            serial_print_RGB();
            //delay_ms(viive);
        }
        while(rx_char == 0x62 && OCR1AL > 0)
        {
            rx_char=getchar();
            OCR1AL--;
            serial_print_RGB();
            //delay_ms(viive);
        }
        while(rx_char == 0x6E && OCR1AL < 255)
        {
            rx_char=getchar();
            OCR1AL++;
            serial_print_RGB();
            //delay_ms(viive);
        }
    }
}
