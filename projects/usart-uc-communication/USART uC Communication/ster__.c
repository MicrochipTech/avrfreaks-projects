/**
    author: saripudin (saripudin2@gmail.com)
    ver: 03 Juli 2010
    @desc: this is uC master software

*/

//pake interrupt untuk Rx dan Tx ternyata sama2 jalan juga

#include <mega2560.h>
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x01
	.EQU __sm_mask=0x0E
	.EQU __sm_powerdown=0x04
	.EQU __sm_powersave=0x06
	.EQU __sm_standby=0x0C
	.EQU __sm_ext_standby=0x0E
	.EQU __sm_adc_noise_red=0x02
	.SET power_ctrl_reg=smcr
	#endif
#include <stdlib.h>
#include <delay.h>
#include <stdio.h>
#asm
   .equ __lcd_port=0x11 ;PORTF
#endasm
#include <lcd.h>

// Declare your global variables here

void main(void)
{
unsigned char data;

char lcd[15];
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

lcd_init(16);
// USART1 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART1 Receiver: On
// USART1 Transmitter: On
// USART1 Mode: Asynchronous
// USART1 Baud Rate: 9600
UCSR0A=0x00;
UCSR0B=0x18;
UCSR0C=0x06;
UBRR0H=0x00;
UBRR0L=0x47;

lcd_putsf("transmit : 19");
putchar(19);
data=getchar();
if(data==38)
{
    lcd_gotoxy(0,1);
    lcd_putsf("receive : ");
    itoa(data, lcd);
    lcd_puts(lcd);
}

while (1);
}
