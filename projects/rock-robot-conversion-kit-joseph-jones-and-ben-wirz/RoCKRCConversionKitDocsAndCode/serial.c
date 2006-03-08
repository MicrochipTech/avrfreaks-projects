/* Serial access for AVR */

#include <io8535.h>
#include <macros.h>
#include <stdlib.h>
#include "defines.h"

#pragma interrupt_handler serial_rcv:12

unsigned char srw_flag = 0;	// Read or write serial date
unsigned char s_adr_hi = 0; // High two bits of ser dat address
unsigned char s_adr_lo = 0; // Low eight bits of ser dat address
unsigned char s_dat    = 0;	// One byte of serial data
unsigned char s_ndx    = 0; // Buffer index (volatile?)

void init_serial (void) {	  // Initialize serial communications
//  DDRD = BIT(DDD1);			  // Make serial_out an output
  PORTD = 0x01;				  // Activate pullup resistor
  UBRR = 51;	 			  // 9600 Baud assuming 8Mhz Xtal
  UCR  = BIT(RXCIE) | BIT(RXEN) | BIT(TXEN); // Xmit, Rcv, intrpt enabled
  }

// Serial line receive interrupt routine
void serial_rcv(void) {			 // Called only when a byte is in UDR
//  register unsigned char uc;	 // Make a local register for temp storage
  s_ndx++;			  // Byte just received, advance state (pointer)
  if (s_ndx == 1) {	  // Just received 1st byte => flag and hi addr
    srw_flag = 0x80 & UDR;	  // Grab flag
	s_adr_hi = 0x03 & UDR; } // Grab highest two bits of address
  else if (s_ndx == 2) {	// Just received 2nd byte => low address
    s_adr_lo = UDR; } 	// Save low address
  else { // if (s_ndx == 3) // Serial input complete, data or dummy in UDR
    s_ndx = 0;			// Reset for next time
	s_dat = UDR;		// This is the data we want to write
	asm("push r25");	// Manual allocation, ICC's dosen't work in v6.22b
    asm("push r26");		// Push X lo
	asm("push r27");		// Push X hi
	asm("lds r27,%s_adr_hi"); // High part addr into X
	asm("lds r26,%s_adr_lo"); // Low part addr into X	
	if (srw_flag) {			// Non-zero => write
	  asm("lds r25,%s_dat");   // Move data to reg so we can store it
	  asm("st X,r25");		   // Store data to given addr 
	  asm("ld r25,X");		   // Read back the data just stored
	  asm("sts %s_dat,r25");   // Write data to serial data variable 
	}
	else {		  			// srw_flag = 0 => read 
	  asm("ld r25,X");		// Use addr in X to move data into reg
	  asm("sts %s_dat,r25");   // Save data in local var	  
	}
	UDR = s_dat;			// Report value just read or written to host
  	asm("pop r27");			// Restore X register hi
  	asm("pop r26");			// Restore X register lo
	asm("pop r25");			// Restore temp register
 	}
  }