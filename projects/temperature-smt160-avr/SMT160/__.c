/*********************************************
Chip type           : ATmega8
Program type        : Application
Clock frequency     : 8.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256
*********************************************/

#include <mega8.h>
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif

#asm
   .equ __lcd_port=0x12 ;PORTb
#endasm
#include <lcd.h>
#include <stdlib.h>
#include <stdio.h>
#include <delay.h>

unsigned int full,half;
unsigned char edge=1;
char dutycycle_str[10],temprature_str[10],mm[10];
float dutycycle,temprature,temp_buf;

// External Interrupt 1 service routine
interrupt [EXT_INT1] void ext_int1_isr(void)
{
TCCR1B=0x00;
if(edge>0)
    {//10
    full=TCNT1;
    MCUCR =0x08;            // Set interrupt on falling edge
    edge=0;
    }
else
    {
    half=TCNT1;
    MCUCR =0x0c;            // Set interrupt on rising edge
    edge=1;
    }
TCNT1=0;
TCCR1B=0x01;
}

interrupt [TIM1_CAPT] void timer1_capt_isr(void)
{
/*static unsigned char state=0;
TCCR1B&=~0x01;
switch(state){
case 0:
	TCCR1B&=~0x40;
	state=1;
	full=TCNT1;
	break;
case 1:
	TCCR1B|=0x40;
	state=0;
	half=TCNT1;
	break;
	}
TCNT1=0;
TCCR1B|=0x01;*/
}

void main(void)
{
char i;
DDRD=0XF7;
DDRC.0=1;
// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 8000.000 kHz
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x41;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x20;

// External Interrupt(s) initialization
// INT0: Off
// INT1: On
// INT1 Mode: Rising Edge
GICR|=0x80;
MCUCR=0x0C;
GIFR=0x80;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
// Analog Comparator Output: Off
ACSR=0x80;
SFIOR=0x00;

//OSCCAL=255;

lcd_init(16);
lcd_putsf("Temp:\n");
lcd_putsf("D.C.:");
#asm("sei")
//delay_ms(1000);
while (1){
    /*
    frequency of the output signal of the sensor lies between 1 and 4 kHz.

    D.C. = 0.320+0.00470*t      D.C.= duty cycle
                                t = Temperature in °C
    or
    duty cycle (d.c.)= 0.31924 + (0.00472 * Temperature(oC))
    */
  temp_buf=0;
  for(i=0;i<10;i++){
	#asm("cli");
	dutycycle=(float)half/(float)(full+half);
	#asm("sei");
	temprature= dutycycle;
	temprature-=0.31924;
	temprature/=0.00472;
	temp_buf+=temprature;
	delay_ms(100);
	}
	temp_buf/=10;
	ftoa(temp_buf,1,temprature_str);
	ftoa(dutycycle*100 ,1,dutycycle_str);
    lcd_gotoxy(7,0);
	lcd_puts(temprature_str);
	lcd_putsf(" C");
    lcd_gotoxy(7,1);
	lcd_puts(dutycycle_str);
	lcd_putsf(" %");
	/*ftoa(full ,1,mm);
	lcd_gotoxy(0,0);
	lcd_puts(mm);
	ftoa(half ,1,mm);
	lcd_gotoxy(0,1);
	lcd_puts(mm);*/
	//delay_ms(500);
	 }
}
