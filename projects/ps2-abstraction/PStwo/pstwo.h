/*********************************************************************************
 * Title: PS/2 Library
 * Author: Akshay Srinivasan<akshaysrinivasan@nitk.ac.in>
 * Date: 22 June 2009
 * Software: AVR-GCC 4.3.2
 
 *-------------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <akshaysrinivasan@gmail.com> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Akshay Srinivasan
 *-------------------------------------------------------------------------------------

 DESCRIPTION
    This library provides an abstraction layer for the PS/2 protocol. This library
    provides functions for receiving and sending bytes via PS/2. This library can
    be used to read and write data in PS/2 devices, although interpreting the data 
    is left for you to do.

    For more information goto,
    http://computer-engineering.org/ps2protocol/
**********************************************************************************/


#ifndef F_CPU
#define F_CPU 16000000UL
#endif

//Data
#define DAPORT D
#define DBIT  3

//Clock
#define CLKPORT D
#define CBIT  2

/////////////////////////////
///General purpose macros////
#define CONCAT(a,b)     a ## b
#define CCPORT(name)    CONCAT(PORT,name)
#define CCPIN(name)     CONCAT(PIN,name)
#define CCDDR(name)     CONCAT(DDR,name)
/////////////////////////////

#define DPORT   CCPORT(DAPORT)
#define DPIN    CCPIN(DAPORT)
#define DDDR    CCDDR(DAPORT)

#define CPORT   CCPORT(CLKPORT)
#define CPIN    CCPIN(CLKPORT)
#define CDDR    CCDDR(CLKPORT)
//////////////////////////////

#define DATA ((DPIN & (1 << DBIT)) >> DBIT)
#define CLK  ((CPIN & (1 << CBIT)) >> CBIT)
///////////////////////////////
/*
#define DELX (dapack[1]-(((dapack[0] & (1 << 4)) >> 4) << 8))
#define DELY (dapack[2]-(((dapack[0] & (1 << 5)) >> 5) << 8))
*/

void send_packet(uint8_t);
uint8_t read_packet(void);
