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

#include<avr/io.h>
#include "pstwo.h"
#include<util/delay.h>

#define ACK 0
#define DELAY 150

void data(uint8_t x) {
    DDDR |= (1 << DBIT);
    if(x==0)
	   	DPORT &= ~(1 << DBIT);
    else if(x==1)
	    DPORT |= (1 << DBIT);
	return;
}

void clk(uint8_t x) {
    CDDR |= (1 << CBIT);
	if(x==0)
		CPORT &= ~(1 << CBIT);
	else if(x==1)
		CPORT |= (1 << CBIT);
	return;
}

void serout(uint8_t bit) {
    while(CLK){
        asm("nop");
    }
    data(bit);
    while(!CLK) {
        asm("nop");
    }
}

uint8_t serin() {
    uint8_t state;
    while(CLK){
        asm("nop");
    }
    state = DATA;
    while(!CLK){
        asm("nop");
    }
    return state;
}

uint8_t oparity(uint8_t byte) {
    uint8_t par=1;
    par ^= ((byte & (1 << 0)) >> 0);
    par ^= ((byte & (1 << 1)) >> 1);
    par ^= ((byte & (1 << 2)) >> 2);
    par ^= ((byte & (1 << 3)) >> 3);
    par ^= ((byte & (1 << 4)) >> 4);
    par ^= ((byte & (1 << 5)) >> 5);
    par ^= ((byte & (1 << 6)) >> 6);
    par ^= ((byte & (1 << 7)) >> 7);
    return par;
}

void send_packet(uint8_t byte) {
    uint8_t parity;
    parity = oparity(byte);
	clk(0);
	_delay_us(DELAY);
    data(0); //Start
    clk(1);
	CDDR &= ~(1 << CBIT); // Release clock
	CPORT |= (1 << CBIT); //Set the pull up on Clock
    /////////////
    serout((byte & (1 << 0)) >> 0);
    serout((byte & (1 << 1)) >> 1);
    serout((byte & (1 << 2)) >> 2);
    serout((byte & (1 << 3)) >> 3);
    serout((byte & (1 << 4)) >> 4);
    serout((byte & (1 << 5)) >> 5);
    serout((byte & (1 << 6)) >> 6);
    serout((byte & (1 << 7)) >> 7);
    /////////////
    serout(parity);
    /////////////
    serout(1); //Stop
    DDDR &= ~(1 << DBIT); //Release the Data line
    DPORT |= (1 << DBIT); //Set the pull up on Data
    /////////////
    if(serin() != ACK)
        send_packet(byte); // Try again if ACK has not been received

	return;
}

uint8_t read_packet(void) {
    uint8_t byte=0,par;
    serin(); //Start
    byte |= (serin() << 0);
    byte |= (serin() << 1);
    byte |= (serin() << 2);
    byte |= (serin() << 3);
    byte |= (serin() << 4);
    byte |= (serin() << 5);
    byte |= (serin() << 6);
    byte |= (serin() << 7);
    par = serin(); //Parity
    serin(); //Stop

    if(par != oparity(byte))
        send_packet(0xfe); //Resend

	return byte;
}
