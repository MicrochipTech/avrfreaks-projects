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
//#include <stdarg.h>
#ifndef _parselanguage_h
#define _parselanguage_h
#define BYTE unsigned char
uint8_t rawtomime(uint8_t *z,char **q,char x);
uint8_t rawtohex(uint8_t *z,char *q,char x);

#endif
