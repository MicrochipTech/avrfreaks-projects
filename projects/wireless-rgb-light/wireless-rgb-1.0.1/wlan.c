/******************************************************************************
 *
 * ID      : $Id: wlan.c 26 2010-11-21 12:47:24Z SH $ 
 * Revsion : $Rev: 26 $
 * Author  : $Author: SH $
 * Date    : $Date: 2010-11-21 13:47:24 +0100 (So, 21 Nov 2010) $   
 *
 ******************************************************************************
 *
 * License
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
 #include "wlan.h"

/****************************************************************************
 * IP Address and Port
 ****************************************************************************/
 ip4_address  MyAddress;
 unsigned int MyPort;


/****************************************************************************
 * Initialize WLAN
 *
 * For now we live with a hardcoded address and port. This can be exteneded 
 * in a later version, e.g. to read the data from the EEPROM
 *
 ****************************************************************************/
 void InitializeWlan()
 {
   /* Set IP address to 192.168.1.99
    */
    MyAddress.Byte[0] = 192;
    MyAddress.Byte[1] = 168;
    MyAddress.Byte[2] = 1;
    MyAddress.Byte[3] = 99;

   /* Set port 
    */
    MyPort = 12345;
 }


/****************************************************************************
 * WLAN setup
 ****************************************************************************/
 
/* SSID
 * 
 * Replace "any" with the WLAN´s SSID you want to connect to 
 */ 
 const prog_char ssid[] PROGMEM = {"any"};      // max 32 bytes
 unsigned char ssid_len = 3;


/* Wireless security type:
 *
 * Set the security type to the WLAN´s type you want to connect to 
 *
 * ZG_SECURITY_TYPE_NONE	-> no   encryption
 * ZG_SECURITY_TYPE_WEP	   -> WEP  encryption
 * ZG_SECURITY_TYPE_WPA	   -> WPA  encryption
 * ZG_SECURITY_TYPE_WPA2	-> WPA2 encryption
 */
 unsigned char security_type = ZG_SECURITY_TYPE_NONE; 

 
/* Wireless mode:
 *
 * Select the WLAN´s mode, select infrastructur if an access point is used
 *
 * WIRELESS_MODE_INFRA	   -> Use infrastructure (e.g. access point)
 * WIRELESS_MODE_ADHOC     -> Adhoc mode
 */
 unsigned char wireless_mode = WIRELESS_MODE_INFRA;


/* WPA passphrase
 *
 * Here the password needs to be stored if WPA encryption is used
 */
 const prog_char security_passphrase[] PROGMEM = {"my_wpa_password"};    
 unsigned char security_passphrase_len = 15;


/* WEP keys
 *
 * The WEP passwords needs to be stored here if WEP encryption is used
 */
 prog_uchar wep_keys[] PROGMEM = 
 {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Key 0
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Key 1
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Key 2
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    // Key 3
 };
