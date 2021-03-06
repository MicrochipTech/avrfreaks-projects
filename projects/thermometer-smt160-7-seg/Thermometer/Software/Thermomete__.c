          /****************************************************
          ** Processor    : ATtiny2313-20PI                   **
         *** Frequncy     : 12.000000 MHz                     ***
        **** AUTHOR       : Behnam Esckandariun               ****
       ***** Project      : Thermometer with SMT160 V.1.00.5  *****
        **** Date         : 2008/02/24 11:49PM IRAN-ZANJAN    ****
         *** Compiler     : CodeVisionAVR  V 1.25.8           ***
          ** comment      : Just for educational use          **
           ****************************************************

                        behnam.esckandariun@gmail.com


/***********************************************************************************
************               Include Used library's                       ************
***********************************************************************************/
#include <tiny2313.h>
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x20
	.EQU __sm_mask=0x50
	.EQU __sm_powerdown=0x10
	.EQU __sm_standby=0x40
	.SET power_ctrl_reg=mcucr
	#endif
#include <stdlib.h>
#include <delay.h>
/***********************************************************************************
************                  Define Used  I/O                          ************
***********************************************************************************/
#define smt160 PORTD.1
#define on 1
#define off 0
#define segment PORTB
#define control PORTD
#define sega PORTD.6
#define segb PORTD.5
#define segc PORTD.4
#define segd PORTD.3
/***********************************************************************************
************              Declare function of init                      ************
***********************************************************************************/
void system_init(void);
/***********************************************************************************
************        Declare global variable's and const here            ************
***********************************************************************************/
unsigned char a[5],b;           //used for 7-segment refresh program
unsigned char i=0;              //used for 7-segment refresh program
float t1,t2,t;                  //contain on & off time of SMT160 output signal
signed int temp;                //Contain temp*10 for one poine
bit edge=0;                     //Used in musering t1 & t2
unsigned int memory;
//This line Save Com-Anode 7-segment Data on Flash ROM
flash char s_data[]={192,249,164,176,153,146,130,248,128,144,191,255};
/***********************************************************************************
************           INT0 Service routin Used for SMT160              ************
************                   Duty-cycle mesure                        ************
***********************************************************************************/
//This ISR just mesure on and off time of the SMT160 Out signal in t1 & t2
//Globale variable's . calculate of temp accure in main function
interrupt [EXT_INT0] void ext_int0_isr(void)
{
if (edge)
        {
        t2=TCNT1;
        TCNT1=0;
        //Falling Edge
        MCUCR=0x02;
        edge=0;
        }
else
        {
        t1=TCNT1;
        TCNT1=0;
        //Rising edge
        MCUCR=0x03;
        edge=1;
        }
}
/***********************************************************************************
************      Timer0 INT Service routin for 7-segment refresh       ************
***********************************************************************************/
//This ISR refresh 7-segment 22 time's per second
//temp varible showing on display
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
i++;
switch (i)
        {
        case 1:
                memory = temp;
                if (temp<0)
                        {
                        a[1]=10;
                        temp *= -1;
                        }
                else
                        a[1]=11;
                temp %= 1000;
                a[2] = temp / 100;
                b = temp % 100;
                a[3] = b / 10;
                a[4] = b % 10;
                control |= 0xf8;
                segment = s_data[a[i]];
                sega = 0;
                break;
        case 2:
                control |= 0xf8;
                segment = s_data[a[i]];
                segb = 0;
                break;
        case 3:
                control |= 0xf8;
                segment = s_data[a[i]];
                segment &= 0b01111111;
                segc = 0;
                break;
        case 4:
                control |= 0xf8;
                segment = s_data[a[i]];
                segd = 0;
                i = 0;
                break;
        };
temp = memory;
TCNT0=0xd7;                      //Reinitlaiz timer start value for display refresh
}
/***********************************************************************************
************               Starting main program                        ************
***********************************************************************************/
void main(void)
{
float dc;                       //Duty cycle save in this variable

system_init();                  // System initalize port and register's value
while (1)
      {
      smt160=on;                //Turn SMT160 on
      //delay_ms(100);          //Waiting for mesuring temp
      //smt160=off;             //Turn SMT160 off
      dc=t1/(t1+t2);            //this line's calculate temp from duty-cycle's
      t=(dc-0.32)/0.0047;
      temp = t*10;
      delay_ms(500);            //wait for next mesurment in 5sec
      };
}
/***********************************************************************************
************          System initlaizing in this Function               ************
***********************************************************************************/
void system_init(void)
{

#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif
PORTB=0x00;
DDRB=0xFF;
PORTD=0x78;
DDRD=0x7A;
//Timer0: 11.71MHz
TCCR0A=0x00;
TCCR0B=0x05;
//Timer1: 12MHz
TCCR1A=0x00;
TCCR1B=0x01;
//INT0:Falling
GIMSK=0x40;
MCUCR=0x02;
EIFR=0x40;
TIMSK=0x02;
#asm("sei")
}
/***********************************************************************************
************            End of thermometer Program                      ************
***********************************************************************************/
