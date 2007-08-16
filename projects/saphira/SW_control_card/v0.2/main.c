/*

	 Shapira control demo (main.c) for Atmega16 for IAR

	 Version 0.2		2007-10-29

	 Copyright (C) Andreas Kingbäck  (andki@itn.liu.se)

   This program is free software; you can redistribute it and/or modify it 
	 under the terms of the GNU General Public License as published by the 
	 Free Software Foundation; either version 2 of the License, or 
	 (at your option) any later version.

   This program is distributed in the hope that it will be useful, 
	 but WITHOUT ANY WARRANTY; without even the implied warranty of 
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	 See the GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License along with this program; 
	 if not, write to the Free Software Foundation, Inc., 
	 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/

#pragma language=extended

#ifndef ENABLE_BIT_DEFINITIONS
#define ENABLE_BIT_DEFINITIONS
// Enable the bit definitions in the iom128.h file
#endif

#include "control.h"
#include "usart.h"

static unsigned int crc;					// global for crc calculation

void delay_1s( void )
{
	__delay_cycles(8000000); // Delay 1s
}


void crc16(unsigned char ser_data)
{
  // Update the CRC for transmitted and received data using
  // the CCITT 16bit algorithm (X^16 + X^12 + X^5 + 1).

  crc = (unsigned char)(crc >> 8) | (crc << 8);
  crc ^= ser_data;
  crc ^= (unsigned char)(crc & 0xff) >> 4;
  crc ^= (crc << 8) << 4;
  crc ^= ((crc & 0xff) << 4) << 1;
}


void send_byte(char data)
{
	int test = -1;
	while (test == -1) {
	 USART_Transmit(data);
   test = USART_Receive_time_out();
	}
}

unsigned char send_pos(unsigned char lr, long int pos)
{
	unsigned char b,b1,b2,b3;								// type conversion
	
	USART_Flush();													// Flush rx buffer
	
  // Reset crc

  crc = 0;

  // Connvert to bytes

  b  = (pos & 0xFF000000) >> 24;
  b1 = (pos & 0x00FF0000) >> 16;
  b2 = (pos & 0x0000FF00) >> 8;
  b3 = (pos & 0x000000FF);

  // Calculate crc for data

  crc16(lr);
  crc16(b);
  crc16(b1);
  crc16(b2);
  crc16(b3);

 // Send command

	USART_Transmit(lr);
  USART_Receive_time_out();

  // Send data

  USART_Transmit(b);
  USART_Receive_time_out();
	
  USART_Transmit(b1);
  USART_Receive_time_out();
  USART_Transmit(b2);
  USART_Receive_time_out();
  USART_Transmit(b3);
  USART_Receive_time_out();

  // Send crc16

  USART_Transmit((crc & 0xFF00) >> 8);
  USART_Receive_time_out();
  USART_Transmit((crc & 0xFF));
  USART_Receive_time_out();

  // Get acc  (10101010=OK)

  if (USART_Receive_time_out() == CM_OK)
    return CM_OK;
  else
    return CM_FAILED;
}

unsigned char send_max_speed(unsigned char lr, unsigned char maxspeed)
{
	
	USART_Flush();													// Flush rx buffer
	
  // Reset crc

  crc = 0;

  // Calculate crc for data

  crc16(lr);
  crc16(maxspeed);
 
 // Send command

	USART_Transmit(lr);
  USART_Receive_time_out();

  // Send data

  USART_Transmit(maxspeed);
  USART_Receive_time_out();
	
  // Send crc16

  USART_Transmit((crc & 0xFF00) >> 8);
  USART_Receive_time_out();
  USART_Transmit((crc & 0xFF));
  USART_Receive_time_out();

  // Get acc  (10101010=OK)

  if (USART_Receive_time_out() == CM_OK)
    return CM_OK;
  else
    return CM_FAILED;
}




long int turn(int r, float v)
{
	
	// Calculates no of puleses for the turn in v deg. (not calibrated!)
	// Radius = 0. Other radius than 0 is not implemented yet!
	
  float dist;
	long int p;
  dist = 0.00293705911111*v;
  p = dist / 5.753632903870394e-006;   
  return p;
}

int main( void )
{
	long int pr;
	long int pl;
	unsigned char maxspeed;
	int i;
	char rx[14];													

	USART_Init(9600);

	// Activate motor controler
	
	while (USART_Receive() != '?');				// Wait for activation ready signal
	USART_Transmit('A');									// Activate!
	delay_1s();														// Wait for controller to activate 
	USART_Flush();												// Flush rx buffer
	
	__enable_interrupt();									// enable interrupts
	
	
	// This program makes the robot to go in a box 2 by 2 meters.
	
	
	pr=0;pl=0; 
	
	// Set maxspeed 0x00 to 0x30
	
	maxspeed = 0x05;
	
	while (send_max_speed(CM_BOTH_SET_MAX_SPEED,maxspeed) == CM_FAILED);
	
	while (1==1) {
		pr+= 0x0002A6EB; // 0x00054DD6;			// pulses for 2 meters
		pl+= 0x0002A6EB; //0x00054DD6;			// pulses for 2 meters
		
		// Forward 2m
		
		while (send_pos(CM_RIGHT_LOAD_POS,pr) == CM_FAILED);
		while (send_pos(CM_LEFT_LOAD_POS, pl) == CM_FAILED);

		USART_Transmit(CM_BOTH_START_MOTION);
		USART_Receive();

		for (i = 0 ; i < 14 ; i++) 						// Receive trajectory complete reply 
				rx[i] = USART_Receive_time_out();
		
  	// Turn 90 deg
		
		pr+=turn(0,45);   											// Radius not implemented!
		pl-=turn(0,45);
		
		while (send_pos(CM_RIGHT_LOAD_POS,pr) == CM_FAILED);
		while (send_pos(CM_LEFT_LOAD_POS, pl) == CM_FAILED);

    USART_Transmit(CM_BOTH_START_MOTION);
		USART_Receive();
		
		for (i = 0 ; i < 14 ; i++) 						// Receive trajectory complete reply 
				rx[i] = USART_Receive_time_out();
		
		// Set maxspeed for demo
		
		switch(maxspeed) {
			case 0x05:
					maxspeed = 0x10;
					break;
			case 0x10:
					maxspeed = 0x20;
					break;
			case 0x20:
					maxspeed = 0x30;
					break;
			case 0x30:
					maxspeed = 0x05;
					break;
		}
	
		while (send_max_speed(CM_BOTH_SET_MAX_SPEED,maxspeed) == CM_FAILED);
		
	}	
}
