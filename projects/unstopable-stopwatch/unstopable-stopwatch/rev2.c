/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.3 Standard
Automatic Program Generator
© Copyright 1998-2011 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 6/5/2012
Author  : PerTic@n
Company : If You Like This Software,Buy It
Comments: 


Chip type               : ATmega16
Program type            : Application
AVR Core Clock frequency: 12.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*****************************************************/

unsigned char count, temp[16], indeks=0;
unsigned char detik=0, menit=0, jam=0;

#include <mega16.h>
#include <delay.h>
#include <stdio.h>
#include <stdlib.h>

// Alphanumeric LCD functions
#asm
        .equ __lcd_port=0x15 ;PORTC
#endasm

#include <alcd.h>


// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
// Reinitialize Timer 0 value
TCNT0=0x8A;
count++;
}
//-------------------------------------------------------

//subrutin hitung waktu
void hitung_waktu()
{
if (count>=100)
    { 
    lcd_clear();
    detik++;
    count=0;
    }
    
if (detik>=60)
    {
    menit++;
    detik=0;
    }

if (menit>=60)
    {
    jam++;
    menit=0;
    }
}
//-------------------------------------------------------

//subrutin input tombol
void detek_tombol()
{
if (PINA.0==0 && (indeks==0 || indeks==2))//run timer
    {
    indeks=1;
    TIMSK=0x01;
    lcd_clear();
    } 
    
if (PINA.1==0 && indeks==1)//stop timer
    {   
    indeks=2;
    TIMSK=0x00;
    lcd_clear();
    } 

if (PINA.2==0 && indeks==2)//reset timer
    {   
    indeks=0;
    count=0;
    detik=0;
    menit=0;
    jam=0;   
    lcd_clear();
    }
}
//-------------------------------------------------------

//subrutin tampil ke lcd
void tampil_lcd()
{
lcd_gotoxy(0,0);
sprintf(temp,"Timer %d:%d:%d:%d",jam,menit,detik,count);
lcd_puts(temp);//tampilkan waktu di LCD baris pertama

if (indeks==0)
    {
    lcd_gotoxy(0,1);
    lcd_putsf("START");
    }   
    
if (indeks==1)
    {
    lcd_gotoxy(0,1);
    lcd_putsf("STOP");
    }    
    
if (indeks==2)
    {
    lcd_gotoxy(0,1);
    lcd_putsf("START      RESET");
    }
}
//-------------------------------------------------------

// Declare your global variables here
void main(void)
{
//--------------------------------------------------------
// Declare your local variables here
PORTA=0x0f;
DDRA=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 11.719 kHz
// Mode: Normal top=FFh
// OC0 output: Disconnected
TCCR0=0x05;
TCNT0=0x8A;
OCR0=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
//TIMSK=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;
//------------------------------------------------------------
// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTC Bit 0
// RD - PORTC Bit 1
// EN - PORTC Bit 2
// D4 - PORTC Bit 4
// D5 - PORTC Bit 5
// D6 - PORTC Bit 6
// D7 - PORTC Bit 7
// Characters/line: 16
lcd_init(16);
        lcd_clear();
        lcd_gotoxy(0,0);
        lcd_putsf("    Loading!  ");      
        lcd_gotoxy(0,1);
        lcd_putsf("....");
        delay_ms(3000);
        lcd_putsf("........");
        delay_ms(3000);  
        lcd_putsf("............");
        delay_ms(3000); 
        lcd_putsf("...............");
        delay_ms(3000);lcd_clear(); 
           
// Global enable interrupts
#asm("sei")
        

while (1)
      {
      // Place your code here       
      detek_tombol();
      hitung_waktu();
      tampil_lcd();
      }
}
