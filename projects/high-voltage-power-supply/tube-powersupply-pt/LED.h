 #ifndef _LED_h_
 #define _LED_h_
/****************************************************************************
 *
 *  Module:       $HeadURL$
 *  Author:       $Author$
 *  Revision:     $Revision$
 *  Date:         $Date$
 *
 *  Copyright:    (C) 2008 by Dipl.-Ing. Stefan Heesch
 *                address:  Bollstrasse 14, CH-5619 Büttikon, Switzerland
 *                callsign: DB7HS, HB9TWS  
 *                email:    radio@heesch.net
 *
 *  Description:  LED functions
 *
 ****************************************************************************/
 
/****************************************************************************
 * Function prototypes
 ****************************************************************************/
 void LED_Initialize();
 int  LED_Check();
 void LED_On();
 void LED_Off();
 void LED_Toggle();

/****************************************************************************
 * The end
 ****************************************************************************/
 #endif
