#include <90s2313.h>
#include <delay.h>
#include <lcd.h>
#include <stdlib.h>
#asm
.equ __lcd_port=0x18 ;PORTB
#endasm
unsigned char q,s=59,m=59,h=12;
unsigned char a,b,*str;
interrupt [TIM1_COMP] void timer1_comp_isr(void)
{
if (s==59)
{
s=0;
if(m==59)
{
m=0;
if(h==12)
h=0;
h++;
}
else
m++;
}
else
s++;
}
void main(void)
{
//bit q=0;
PORTB=0x00;
DDRB=0xFF;
PORTD=0x60;
DDRD=0x1f;
lcd_init(16);
// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 4000.000 kHz
// // Compare Match Interrupt: On
TCCR1A=0x00;
TCCR1B=0x0b;
OCR1H=0xF4;
OCR1L=0x24;
// // Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x40;
// Global enable interrupts
#asm("sei")
while(1)
{
if(PIND.5==0 && q==0 )
{
if (m==59)
{
m=0;
}
m++;
q=1;
}
if(PIND.6==0 && q==0 )
{
if (h==12)
{
h=0;
}
h++;
q=1;
}
if (PIND.5==1 && PIND.6==1)
{
q=0;
}
lcd_gotoxy(0,0);
b=h/10;
itoa(b,str);
lcd_puts(str);
a=h%10;
itoa(a,str);
lcd_puts(str);
lcd_putchar(':');
lcd_gotoxy(3,0);
b=m/10;
itoa(b,str);
lcd_puts(str);
a=m%10;
itoa(a,str);
lcd_puts(str);
lcd_putchar(':');
lcd_gotoxy(6,0);
b=s/10;
itoa(b,str);
lcd_puts(str);
a=s%10;
itoa(a,str);
lcd_puts(str);
}
}