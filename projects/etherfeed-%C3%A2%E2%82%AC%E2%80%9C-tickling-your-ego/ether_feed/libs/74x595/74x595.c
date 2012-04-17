/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Orig File From: eth_rem_flowercare-3.2/main.c
 * Orig Author: Guido Socher
 * Copyright: GPL V2
 * See http://www.gnu.org/licenses/gpl.html
 *
 * Change By: Quentin Arce 3/2011
 *            Pull 74x595 code out to create a lib
 *            in avr-uip project on google code
 *
 *********************************************/
/*
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
*/

#include "74x595.h"

//
// Max amount of switches (0..3) to display on the webpage:
//#define MAX_RELAY 4
// the positions on PORTD where the relays are connected:
//static uint8_t relay2port[MAX_RELAY] = {4,5,6,7}; // PD4, PD5, PD6, PD7

#define nop()  asm volatile("nop\n\t"::)

#ifdef MULTI_SUPPORT_74x595
void multi_set74x595(uint8_t *banks, uint8_t bank_count)
{
	int8_t bank_index;

	for (bank_index=0; bank_index < bank_count; bank_index++)
	{
		uint8_t i=8;
		while(i)
		{
			i--;
			S74595_CLOCKDOWN;
			if ((*(banks + bank_index)) & (1<<i))
			{
				S74595_1;
			}
			else
			{
				S74595_0;
			}
			S74595_CLOCKUP;
			nop();
		}
	}
	S74595_CLOCKDOWN;
	S74595_RCLKUP;
	nop();
	S74595_RCLKDOWN;
}
#endif


#ifndef MULTI_SUPPORT_74x595
void set74x595(uint8_t val)
{
      uint8_t i=8;
      while(i){
              i--;
              S74595_CLOCKDOWN;
              if (val & (1<<i)){
                      S74595_1;
              }else{
                      S74595_0;
              }
              S74595_CLOCKUP;
              nop();
      }
      S74595_CLOCKDOWN;
      S74595_RCLKUP;
      nop();
      S74595_RCLKDOWN;
}
#endif

void init74x595(void)
{
        DDRC|= (1<<DDC4); // enable as output line
        S74595_RCLKDOWN;
        DDRC|= (1<<DDC5); // enable as output line
        S74595_0;
        DDRC|= (1<<DDC3); // enable as output line
        S74595_CLOCKDOWN;
#ifdef MULTI_SUPPORT_74x595

#else		
        set74x595(0);
#endif
}

/*
        while(switchnum<MAX_RELAY){ 
                if (PORTD & (1<<relay2port[switchnum])){
                        pl=fill_tcp_data_p(buf,pl,PSTR("ON "));
                }else{
                        pl=fill_tcp_data_p(buf,pl,PSTR("OFF"));
                }
                pl=fill_tcp_data_p(buf,pl,PSTR(" <a href=s"));
                itoa(switchnum,gStrbuf,10); // convert integer to string
                pl=fill_tcp_data(buf,pl,gStrbuf); // switch number in ascii
                pl=fill_tcp_data(buf,pl,"-");
                if (PORTD & (1<<relay2port[switchnum])){
                        pl=fill_tcp_data(buf,pl,"0");
                }else{
                        pl=fill_tcp_data(buf,pl,"1");
                }
                pl=fill_tcp_data_p(buf,pl,PSTR(">s"));
                itoa(switchnum,gStrbuf,10); // convert integer to string
                pl=fill_tcp_data(buf,pl,gStrbuf); // switch number in ascii
                pl=fill_tcp_data_p(buf,pl,PSTR("</a>"));
                pl=fill_tcp_data(buf,pl,"\n");
                switchnum++;
        }
#ifdef USE_74595
        pl=fill_tcp_data(buf,pl,"\n");
        switchnum=0;
        while(switchnum<8){ // port extended switch 0..7 via 74hc595 
                if (e74595val & (1<<switchnum)){
                        pl=fill_tcp_data_p(buf,pl,PSTR("ON "));
                }else{
                        pl=fill_tcp_data_p(buf,pl,PSTR("OFF"));
                }
                pl=fill_tcp_data_p(buf,pl,PSTR(" <a href=e"));
                itoa(switchnum,gStrbuf,10); // convert integer to string
                pl=fill_tcp_data(buf,pl,gStrbuf);
                pl=fill_tcp_data(buf,pl,"-");
                if (e74595val & (1<<switchnum)){
                        pl=fill_tcp_data(buf,pl,"0");
                }else{
                        pl=fill_tcp_data(buf,pl,"1");
                }
                pl=fill_tcp_data_p(buf,pl,PSTR(">e"));
                itoa(switchnum,gStrbuf,10); // convert integer to string
                pl=fill_tcp_data(buf,pl,gStrbuf); // switch number in ascii
                pl=fill_tcp_data_p(buf,pl,PSTR("</a>"));
                pl=fill_tcp_data(buf,pl,"\n");
                switchnum++;
        }
#endif
*/
/*
        // str is now something like password/sw0-1 or just end of url
        if (strncmp("/s",str,2)==0){
                str+=2;
                i=*str-'0'; // char to integer
                if (i>=0 && i<MAX_RELAY){ // positions 0 to MAX_RELAY-1 in relay2port
                        str++;
                        if (*str!='-'){
                                return(-1);
                        }
                        str++;
                        if (*str=='0'){
                                PORTD &= ~(1<<relay2port[i]);// transistor off
                                return(0);
                        }
                        if (*str=='1'){
                                PORTD|= (1<<relay2port[i]);// transistor on
                                return(0);
                        }
                        // switch value is not 1 and not 0, e.g "x":
                        print_page_show_switch(PORTD & (1<<relay2port[i]));
                        return(10);
                }
                return(-1);
        }
#ifdef USE_74595
        // str is now something like password/es0-1 or just end of url
        if (strncmp("/e",str,2)==0){
                str+=2;
                i=*str-'0'; // char to integer
                if (i>=0 && i<=7){ // switch 0 to 7
                        str++;
                        if (*str!='-'){
                                return(-1);
                        }
                        str++;
                        if (*str=='0'){
                                set74595(e74595val&~(1<<i));// off
                                return(0);
                        }
                        if (*str=='1'){
                                set74595(e74595val|(1<<i));// on
                                return(0);
                        }
                        // switch value is not 1 and not 0, e.g "x":
                        print_page_show_switch(e74595val & (1<<i));
                        return(10);
                }
        }
#endif
*/


/* 
#ifdef USE_74595
        init74595(); // it is best to do 74hc595 init right after power on
#endif
		*/
