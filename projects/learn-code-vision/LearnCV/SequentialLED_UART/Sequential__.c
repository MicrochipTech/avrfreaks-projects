/*****************************************************
This program was produced by the
CodeWizardAVR V1.25.5 Professional
Automatic Program Generator
© Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : SequentialLED
Version : 1.0
Date    : 9/21/2007
Author  : Srinivasan
Company : Scorpion Controls
Comments:
Second IO Handeling...............................

I have adopted "SC" [Study Comparitively] method by Scorpion Controls.


Chip type           : ATmega8535
Program type        : Application
Clock frequency     : 1.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 128
*****************************************************/

#include <mega8535.h>
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x40
	.EQU __sm_mask=0xB0
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0xA0
	.EQU __sm_ext_standby=0xB0
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif

#include <stdio.h>

void delay(unsigned int);

void main(void)
{
unsigned char flag,cmd;

DDRA=0x0F;
PORTA=0x0F;

UCSRA=0x02;
UCSRB=0x18;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x0C;

printf("HAI");
flag=0;

while(1)
{
   cmd= ((PINA & 0xF0) >> 4);
   if ((cmd==0x0E) && (flag==0))
   {
           PORTA.0=0;
           delay(65000);
           //PORTA.0=1;
           flag++;
   }
   if ((cmd==0x0D) && (flag==1))
   {
           PORTA.1=0;
           delay(65000);
           //PORTA.1=1;
           flag++;
   }
   if ((cmd==0x0B) && (flag==2))
   {
           PORTA.2=0;
           delay(65000);
           //PORTA.2=1;
           flag++;
   }
   if ((cmd==0x07) && (flag==3))
   {
           PORTA.3=0;
           delay(65000);
           //PORTA.3=1;
           flag=0;
           PORTA=0x0F;
   }
};
}
void delay(unsigned int i)
{
        for(;i==0;i--)
        ;
}

