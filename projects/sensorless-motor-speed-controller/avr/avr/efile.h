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
/* eeprom routines  
   * these allow storage of command strings into eeprom 
   * uses the same command processor as the uart command input 
*/
#ifdef EPROMFILE
#define EPROMFILE
#include <avr/io.h>
#include <inttypes.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/eeprom.h>
#include "fail.h"
#include "uart.h"
#include "command.h"

uint16_t efile_print();
char efile_find(uint8_t *cmd,uint8_t len);
char efile_seek(uint8_t pos);
char efile_write(uint8_t *cmd,char mode);
void  efile_exec();

#endif

