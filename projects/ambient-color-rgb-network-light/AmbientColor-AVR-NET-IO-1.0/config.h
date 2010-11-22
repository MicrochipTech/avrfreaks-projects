#ifndef _CONFIG_H_
#define _CONFIG_H_	

/******************************************************************************
 *
 * ID      : $Id: config.h 11 2010-10-24 16:42:52Z SH $ 
 * Revsion : $Rev: 11 $
 * Author  : $Author: SH $
 * Date    : $Date: 2010-10-24 18:42:52 +0200 (So, 24 Okt 2010) $   
 *
 ******************************************************************************
 *
 * The MIT License
 *
 * Copyright (c) 2010 by Stefan Heesch, http://www.heesch.net
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

/******************************************************************************
 * CPU Clock
 ******************************************************************************/	
 #define F_CPU 16000000UL	


/******************************************************************************
 * UART baudrate
 ******************************************************************************/	
 #define BAUDRATE 38400

/******************************************************************************
 * Defines for LEDs
 ******************************************************************************/	
 #define LED_OUT         PORTA
 #define LED_DDR         DDRA
 #define RED             PA4
 #define GREEN           PA5
 #define BLUE            PA6

/******************************************************************************
 * Defines for network
 ******************************************************************************/	
 #define MULTICAST_IP      0xEA050607		// 234.5.6.7
 #define MULTICAST_PORT    12345

 #define USE_ENC28J60		1

 #define MYMAC1	         0x00
 #define MYMAC2	         0x22
 #define MYMAC3	         0xF9
 #define MYMAC4	         0x01
 #define MYMAC5	         0x1e
 #define MYMAC6	         0x3b


    
#endif //_CONFIG_H
