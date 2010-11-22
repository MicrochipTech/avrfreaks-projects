/******************************************************************************
 *
 * ID      : $Id: main.c 34 2010-11-21 20:43:00Z SH $ 
 * Revsion : $Rev: 34 $
 * Author  : $Author: SH $
 * Date    : $Date: 2010-11-21 21:43:00 +0100 (So, 21 Nov 2010) $   
 *
 ******************************************************************************
 *
 * License for WLAN controlled RGB-Light
 *
 * Copyright (c) 2010 by Stefan Heesch, http://www.heesch.net
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 ******************************************************************************/


/****************************************************************************
 * Include files
 ****************************************************************************/
 #include <avr/pgmspace.h>
 #include <avr/interrupt.h>
 #include "config.h"
 #include "g2100.h"
 #include "net.h"
 #include "timer.h"
 #include "led.h"
 #include "log.h"
 #include "wlan.h"


/****************************************************************************
 * Program entry point
 ****************************************************************************/
 int main( void )
 {

   /* Initialize hardware
    */
    LED_Off(RED);
    LED_Off(BLUE);    
    LED_Off(GREEN);

    InitializeNetwork();
    InitializeWlan();
    InitializeTimer();
    InitializeLogging;

   /* Enable interrupts
    */
    sei();

    log("\n\r\n\r");
    log("Wireless RGB\n\r");
    log("------------\n\r");
    log("\n\r");
    log("IP Address : %i.%i.%i.%i\n\r", MyAddress.Byte[0],MyAddress.Byte[1],MyAddress.Byte[2],MyAddress.Byte[3]);

    log("Connecting to WLAN ( SSID = ");
    for (int i=0; i< ssid_len; i++ )
    {
       log("%c", pgm_read_byte(ssid +i) );
    }
    log(" ) ...");
 
   /* Connect to WLAN
    */
    unsigned char ok = 0;
    do
    {
       ok = ConnectNetwork();
    }  
    while( ok == 0);
   
    log("\n\rConnected to WLAN.\n\r\n\r" );
    
   /* Event loop
	 */
	 for (;;)
	 {
       HandleNetworkData();
	 }

    return 0;
 }


