/********************************************************************************

Two Wire Interface (TWI) utility functions.

Copyright (C) 2005 by Donald R. Blake
donblake at worldnet.att.net

This program is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

---------------------------------------------------------------------------------

Change Activity:

    Date       Description
   ------      -------------
  02 Sep 2005  Created.

********************************************************************************/



#ifndef _TWI_H_
#define _TWI_H_



/********************************************************************************

                                    Includes

********************************************************************************/

#include <stdint.h>



/*******************************************************************************

                                    twiInit

Iinitializes the TWI.  twiInit should be called before executing twiRead or
twiWrite.

*******************************************************************************/

void
twiInit(
  uint8_t timeout  // TWI timeout count (in time-out counter increments - see
                   // the twiDecTo function)
);



/*******************************************************************************

                                    twiWrite

TWI Master Transmit.

*******************************************************************************/

uint8_t           // returns zero if no error; non-zero if error
twiWrite(
  uint8_t  sla,   // slave address (0..127)
  uint8_t* data,  // data byte(s) to write
  uint8_t  count  // number of bytes to write
);



/*******************************************************************************

                                    twiRead

TWI Master Receive.

*******************************************************************************/

uint8_t           // returns zero if no error; non-zero if error
twiRead(
  uint8_t  sla,   // slave address (0..127)
  uint8_t* data,  // data byte(s) read
  uint8_t  count  // number of bytes to read
);



/*******************************************************************************

                                    twiDecTo

Decrements the TWI time-out counter.  twiDecTo should be called at a fixed
periodic rate (see timeout parameter of the twiInit function).

*******************************************************************************/

void
twiDecTo(
  void
);



#endif // ifndef _TWI_H_
