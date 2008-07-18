/*****************************************************
This program was produced by the
CodeWizardAVR V1.25.5 Professional
Automatic Program Generator
© Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : SimpleIO
Version : 1.0
Date    : 9/21/2007
Author  : SRINIVASAN.M
Company : Scorpion Controls
Comments:
First Task in IO Handeling..................

I have Adopted "SC"(Study Comparitively) method created by Scorpion Controls.


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


void main(void)
{

unsigned char s;
int delay;
PORTA=0x00;
DDRA=0x0F;

while (1)
      {
           s=PINA >> 4;
           PORTA=s;
           for(delay=0;delay<10000;delay++)
           {
                delay=delay;
           }

      };
}
