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
/* 
   * main entry point is here this file contains some common functionallity 
   * for my various targets which is a communications initialisation and a 
   * main 16 bit clock counter word and a temporary data buffer for general usage  
   * also calls startup()
   * the startup() call is defined in other files depending on the hardware target
*/

#include <avr/io.h>
#include <avr/wdt.h>
#include ATMEGA
#include <inttypes.h>
#include <stdlib.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "adc.h"
#include "uart.h"
#include "fail.h"
#include "string.h"
#include "command.h"
#include "common.h"
#include "monorail.h"
#include "efile.h"

volatile uint16_t mainclock;

static uint32_t lasttime=0;
static uint32_t utime=0;

char buf[BUFMAX];

//__C_task
void main(void)
{
   RED_DDR|=_BV(RED_BIT);
   GREEN_DDR|=_BV(GREEN_BIT);
   RED_ON;
   GREEN_ON;
   command_init();  // start uart RX & TX interrupt thread
   sei();
   puts("Com ok");
   put_cr();
   syncwait();
   char ct;
   startup();
   wdt_enable(WDTO_2S);
   wdt_reset();
   usart_data ();   // jump into the serial command processing loop
   while(0);
   return;
}

