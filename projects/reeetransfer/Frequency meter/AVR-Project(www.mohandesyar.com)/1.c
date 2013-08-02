/*****************************************************
This program was produced by the
CodeWizardAVR V1.25.3 Professional
Automatic Program Generator
© Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 2007/04/23
Author  : www.eca.ir                           
Company : ECA                            
Comments: 


Chip type           : ATmega16
Program type        : Application
Clock frequency     : 1.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256
*****************************************************/

#include <mega16.h>

#asm
   .equ __lcd_port=0x12 ;PORTD
#endasm
#include <lcd.h>
#include <delay.h>
#include <stdlib.h>

int n=0;
char *s;
bit p=0;
int x=0;
int y=0;
int count1=0;
int count2=0;


interrupt [EXT_INT1] void ext_int1_isr(void)
{

int i;
TCCR0=0x14;
TCNT0=0x00;
OCR0=0x00;
if (n>4 & p!=1) {
n=0;
x++;
};
n++;
for (i=0;i<=4;i++) {
delay_ms(30);
    };
}



interrupt [TIM0_COMP] void timer0_comp_isr(void)
{
count1++;
if(count1>=5)
count2++;
if(count2>=10)
{
x++;
n=1;
count2=0;
count1=0;  
TCCR0=0x00;
TCNT0=0x00;
OCR0=0x00;
};
}


void main(void)
{

int BK=0;
int z=0;

PORTA=0x00;
DDRA=0x1F;


PORTB=0x00;
DDRB=0x08;


PORTC=0x00;
DDRC=0x00;


PORTD=0x00;
DDRD=0x00;


TCCR0=0x00;
TCNT0=0x00;
OCR0=0x00;


TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;


ASSR=0x00;
TCCR2=0x07;
TCNT2=0x00;
OCR2=0x00;


GICR|=0x80;
MCUCR=0x0C;
MCUCSR=0x00;
GIFR=0x80;


TIMSK=0x42;


ACSR=0x80;
SFIOR=0x00;


lcd_init(16);

#asm("sei")

while (1)
      {
      if(x>=15)
      {
      x=0;
      y=1;
      };

      PORTA.0=1;
      if(PINA.7==1)
      {
      p=0;
      if(BK!=1){

      z=1;
      BK=1;
      n=1;
      };
        itoa(1,s);
        lcd_puts(s);
        PORTA.4=0;
        PORTA.4=1;
    };
    
    if (PINA.6==1) {
    p=0;
    if(BK!=2){  

    z=1;
      BK=2;
      n=1;
      };

         if(n==1){
    lcd_gotoxy(x,y);
    lcd_putsf("a");
    };
    if(n==2){
    lcd_gotoxy(x,y);
    lcd_putsf("b");
    };
    if(n==3){
lcd_gotoxy(x,y);
    lcd_putsf("c");
    };
    if(n==4){
    lcd_gotoxy(x,y);
    itoa(2,s);
    lcd_puts(s); 
    };
        
    PORTA.4=0;
    PORTA.4=1;
};

if (PINA.5==1) {
      p=0;
      if(BK!=3){ 

      z=1;
      BK=3;
      n=1;
      };
      if(n==1){  
      lcd_gotoxy(x,y);
    lcd_putsf("d");
    };
    if(n==2){
        lcd_gotoxy(x,y);
    lcd_putsf("e");
    };
    if(n==3){   
        lcd_gotoxy(x,y);
    lcd_putsf("f");
    };
    if(n==4){ 
        lcd_gotoxy(x,y);
    itoa(3,s);
    lcd_puts(s);
    };       
    
 PORTA.4=0;
 PORTA.4=1;
};

      PORTA.0=0;
      PORTA.1=1;
      PORTA.2=0;
      PORTA.3=0;
      if(PINA.7==1)
      {
      p=0;
      if(BK!=4){
      z=1;
      BK=4;
      n=1;
      };
    if(n==1){
    lcd_gotoxy(x,y);
    lcd_putsf("g");
    };
    if(n==2){
    lcd_gotoxy(x,y);
    lcd_putsf("h");
    };
    if(n==3){  
    lcd_gotoxy(x,y);
    lcd_putsf("i");
    };
    if(n==4){
    lcd_gotoxy(x,y);
    itoa(4,s);
    lcd_puts(s);
    };  

       PORTA.4=0;
       PORTA.4=1;
    };
    
    if (PINA.6==1) {
    p=0; 
    if(BK!=5){
      BK=5;
      z=1;
      n=1;
      };
     if(n==1){ 
    lcd_gotoxy(x,y);
    lcd_putsf("j");
    };
    if(n==2){
    lcd_gotoxy(x,y);
    lcd_putsf("k");
    };
    if(n==3){   
    lcd_gotoxy(x,y);
    lcd_putsf("l");
    };
    if(n==4){  
    lcd_gotoxy(x,y);
    itoa(5,s);
    lcd_puts(s);
    };

    PORTA.4=0;
    PORTA.4=1;
};

if (PINA.5==1) {
      p=0;
      if(BK!=6){
      z=1;
      BK=6;
      n=1;
      };
 if(n==1){
 lcd_gotoxy(x,y);
    lcd_putsf("m");
    };
    if(n==2){
    lcd_gotoxy(x,y);
    lcd_putsf("n");
    };
    if(n==3){
    lcd_gotoxy(x,y);
    lcd_putsf("o");
    };
    if(n==4){
    lcd_gotoxy(x,y);
    itoa(6,s);
    lcd_puts(s);
    }; 

 PORTA.4=0;
 PORTA.4=1;
};

      PORTA.0=0;
      PORTA.1=0;
      PORTA.2=1;
      PORTA.3=0;
      if(PINA.7==1)
      { 
      p=1;
      if(BK!=7){
      z=1;
      BK=7;
      n=1;
      };
 if(n==1){  
 lcd_gotoxy(x,y);
    lcd_putsf("p");
    };
    if(n==2){
    lcd_gotoxy(x,y);
    lcd_putsf("q");
    };
    if(n==3){
    lcd_gotoxy(x,y);
    lcd_putsf("r");
    };
        if(n==4){
        lcd_gotoxy(x,y);
    lcd_putsf("s");
    };
    if(n==5){
    lcd_gotoxy(x,y);
    itoa(7,s);
    lcd_puts(s);
    };
        if(n>=5){
        p=0;
        };
       PORTA.4=0;
       PORTA.4=1;
    };
    
    if (PINA.6==1) {
    p=0;
    if(BK!=8){
      z=1;
      BK=8;
      n=1;
      };
 if(n==1){
 lcd_gotoxy(x,y);
    lcd_putsf("t");
    };
    if(n==2){
    lcd_gotoxy(x,y);
    lcd_putsf("u");
    };
    if(n==3){
    lcd_gotoxy(x,y);
    lcd_putsf("v");
    };
    if(n==4){ 
    lcd_gotoxy(x,y);
    itoa(8,s);
    lcd_puts(s);
    };
    PORTA.4=0;
    PORTA.4=1;
};

if (PINA.5==1) {
p=1; 
      if(BK!=9){
      z=1;
      BK=9;
      n=1;
      };
 if(n==1){ 
 lcd_gotoxy(x,y);
    lcd_putsf("w");
    };
    if(n==2){
    lcd_gotoxy(x,y);
    lcd_putsf("x");
    };
    if(n==3){
    lcd_gotoxy(x,y);
    lcd_putsf("y");
    };
    if(n==4){
    lcd_gotoxy(x,y);
    lcd_putsf("z");
    };
    if(n==5){
    lcd_gotoxy(x,y);
    itoa(9,s);
    lcd_puts(s);
    };
    if(n>=5){
    p=0;
    };
 PORTA.4=0;
 PORTA.4=1;
};
      PORTA.0=0;
      PORTA.1=0;
      PORTA.2=0;
      PORTA.3=1;
      if(PINA.7==1)
      {
      p=0;
      lcd_clear();
      lcd_putsf("*");
    };
    if (PINA.6==1) {
    p=0;
    itoa(0,s);
    lcd_puts(s);
};
    if (PINA.5==1) {
      p=0;
      lcd_clear();
      lcd_putsf("#");
};
      };
}
