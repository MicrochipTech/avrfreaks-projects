/* copyright
   * (c) 2004 Stephen Humble stephenhumble at telstra.com
   *
   *  This file is part of robotx see http://Robotx.sourceforge.net
   *  dual sensorless motor driver firmware.
   *
   *  This is free software; you can redistribute it and/or modify
   *  it under the terms of the GNU General Public License as published by
   *  the Free Software Foundation; either version 2 of the License, or
   *  (at your option) any later version.
*/
/* this is the main communications and control loop
   * it's operation revolves around receiving
   * and sending messages using the UART
   * in order to allow some high level control computer to
   * control everything from here
   * the eeprom and debug commands all use human readable ascii characters 
   * and typically consist of a command followed by it's data 
   * each command string is terminated by a line feed 
   * the use of 7bit ascii permits the extra bit to be used 
   * for an rs485 multimcu mode adress byte ( though not implemented yet)
*/

#include <sig-avr.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include ATMEGA
#include <interrupt.h>
#include <inttypes.h>
#include "uart.h"
#include "irdds.h"
#include "servo.h"
#include "quad.h"
#include "irrx.h"
#include "pwm.h"
#include "adc.h"
#include "lcd.h"
#include "monorail.h"
#include "common.h"
#include "weldtime.h"
#include "bridge.h"

uint8_t ip;
#define MAX_NUM 40
char num[MAX_NUM+1];
char nump;
short unsigned int watch;
short unsigned int unchar;
uint16_t lcd;

// usart command state machine
void usart_data ()
{
   char ono=0;
   nump=0;
   unchar=0;
   watch=0;
   uint32_t value;
   //   SP=1000;
   // static enum{TYPE,ADDRL,ADDRH,DATA,DATAL,DATAH}state;
   // static uint8_t *addr=0;
   // puts("command\n\r");
   for(;;)
   {
      wdt_reset();
      if(getc(&ip)==0)
      {
	 // this is what we do if no data is available from the serial port
#ifdef WELDING
	 if(ono & 1 )caps();
	 if(! lcd--){
		       weld_menu();
		       //		       weld_show();  // calls LCD update routine roughly every 100ms
		       lcd=5000;
		    }
#endif
#ifdef TIMER
	 if(! lcd--){
		       timer_menu();
		       lcd=5000;
		    }
#endif
	 if(mainclock-watch >1000)
	 {
	    //    put_i16(mainclock);
	    //  put_cr();
	    watch=mainclock;
	    // RECIEVE;
	 }
      }else{
	      watch=mainclock;
	      if(nump<MAX_NUM)
	      {
		 num[nump++]=ip;
		 putc(ip);
	      }
	      else
	      {
		 putc('~');
	      }
	      if(ip=='['){
			    ono++;
			    continue;
			 }
/*
		 if(ip==' '){
		 lcd_put((TCNT1&0x1F)+'a',0);
		 lcd_ready();
		 puts(itoa( lcd_ready(),&buf,10));
		 continue;
		 }
*/
	      if(ip==0x18) exit(0);  // ctrl-x to exit program

	      // when we recieve a carridge return or etx the collected
	      // uart data is processed
	      if(ip=='\n' || ip== '\r' )
	      {
		 put_cr();
		 if(nump>1){
			      nump--;
			      num[nump]=0;  // zero terminate the string
			      process(&num[0],nump);
			   }
		 nump=0;
	      }
	   }
   }
}

/*
   * this routine reads a direction and speed in the format 
   * [fr][-]<decimal number>
   * and returns the direction in the return value and the 
   * velocity number in v
*/
#ifdef MONORAIL
uint8_t scmd(char **z,uint8_t *v)
{
   char dir=**z;
   uint8_t brop;
   (*z)++;
   int16_t tp=0;
   if(isprint(**z)){
		      tp=strtol(*z,0,10);
		      puts(" spd=");
		      put_i16(tp);
		   }
   *v=(tp>0)?tp:-tp;
   switch (dir)
   {
      case 'l':
	 puts(" lo");
	 brop=BR_LOW;
	 break;
      case 'h':
	 puts(" hi");
	 brop=BR_HIGH;
	 break;
      case 'f':
	 puts(" fwd");
	 if( tp >0 ){
		       puts(" acc");
		       brop=BR_FWD;
		    }
	 if( tp <0 ){
		       puts(" stop");
		       brop=BR_FWDSTOP;
		    }
	 if( tp ==0 ){
			puts(" wait");
			brop=BR_WAIT;
		     }
	 break;
      case 'r':
	 puts(" rev");
	 if( tp >0 ){
		       puts(" acc");
		       brop=BR_REV;
		    }
	 if(tp <0 ){
		      puts(" stop");
		      brop=BR_REVSTOP;
		   }
	 if( tp ==0 ){
			puts(" wait");
			brop=BR_WAIT;
		     }
      default:
   }
   put_cr();
   return(brop);
}
#endif
/* process a command line
   * reads a command string from z and processes it
*/
process(char *z,uint8_t nump)
{
   char *pt;
   int16_t tp;
   int16_t t;
   switch( *z++)
   {
#ifdef HOUSEALARM
      // some
      case 'w':   // forward
	 sbi(FLAGS,S_FORWARD);
	 puts("qfstop");  //bridge trip reset
	 put_cr();
	 break;
      case 'a':
	 hexputc(bitfield_get());
	 break;
      case 's':
	 {
	    uint8_t data;
	    hextoraw(&z,&data,char x)
	       bitfield_put(x));
	 }
	 break;
#endif
#ifdef MONORAIL
      case 'r':    // r prefix is used for most monorail specific control
	 {
	    pt=z;
	       rail_com(&pt);
	 }
	 break;
      case 'a':   // control bridge q
	 puts("brQ");
	    pt=z;
	    t=scmd(&pt,&tp);
	    bridge_set(&hbq,tp,t);
	    break;
      case 'b':   // control bridge u
	 puts("brU");
	    pt=z;
	    t=scmd(&pt,&tp);
	    bridge_set(&hbu,tp,t);
	    break;
      case 'E':
	 bridge_print();
	    break;
      case 'T':
	 show_tab();
	    break;
#endif
#ifdef EXERCISE
      case 'B':    // B is used for a regenerative breaking application
	 {
	    pt=z;
	       regeneration_com(&pt);
	 }
#endif
#ifdef DDRIVE
	 // d prefix is used for controlling a differential drive robot base application
      case 'd':
	 {
	    pt=z;
	       ddrive_com(&pt);
	 }
#endif
#ifdef DEBUG
      case 'A':   // diagnostic register dump
	 dump();
	    break;
      case 'Q':   // diagnostic register dump
	 dump2();
	    break;
#endif
#ifdef IRRX_CODE
      case 'i':   // infra red timing data
	 irrx_print();
	    break;
#endif
      case 'e':
	 {  // commands to manipulate eeprom
	    pt=z;
	       efile_cmd(&pt);
	 }
	 break;

#ifdef IRDDS_CODE
      case 'j':// show irdds code received
	 irdds_print();
	    break;
#endif
/*
	    case 'x':   // host asks for status
	    servo_print();
	    break;
	    case 'X':   // set servo pos
	    servo[0].endpos=quad;
	    puts("quad=");
	    puts(itoa(quad,&buf,10));
	    puts("\n\r");
	    for(;nump>0;nump--)num[nump]=0;
	    break;
	    case '<':   // move servo
	    putc('<');
	    servo[1].pos--;
	    break;
	    case '>':   // move servo
	    putc('>');
	    servo[1].pos++;
	    break;
	    case 'Y':   // host asks for status
	    servo[1].endpos=strtol(num,0,10);
	    puts("set Y =");
	    puts(itoa(quad,&buf,10));
	    puts("\n\r");
	    for(;nump>0;nump--)num[nump]=0;
	    break;
*/
#ifdef WELDING
      case 'w':   // set welding time in us
	 pt=z;
	    weld_com(&pt);
	    return;
#endif
#ifdef TIMER
      case 't':   // set welding time in us
	 pt=z;
	    timer_com(&pt);
	    return;
#endif
#ifdef BINARYCOMMANDS
      case 'Y':
	 //	 adc_data();
	 break;
#endif

      default:  // print hex value of and echo character
/*
	    hexputc(*(z-1));
	    putc(*(z-1));
	    putc('?');
*/
	 break;
   }
}

//initialise the remote control task
command_init()
{
   init_uart();
}

