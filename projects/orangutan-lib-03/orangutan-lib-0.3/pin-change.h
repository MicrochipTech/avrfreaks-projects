// pin-change.h
//
// Tom Benedict

#ifndef _PIN_CHANGE_
#define _PIN_CHANGE_

// There's only one command for this subsystem: 
// pci_define().  It's where you say, "For a given PINx register and
// a given bit, run the following routine."  Of course there are caveats
// with how this is called:
//
// Let's say you have the following subroutines for running an incremental
// encoder with an index to indicate 0 degrees:
//
// 		void encoder_func(void)
// 		{
//			counter++;
//		}
//
//		void index_func(void)
//		{
//			counter = 0;
//		}
//
// To set these up on PB2 and PB3 respectively, you'd do:
//
//		pci_define(_SFR_IO_ADDR(PINB), 2, *encoder_func);
//		pci_define(_SFR_IO_ADDR(PINB), 3, *index_func);
//
// (If you've read through the servo.c code, the _SFR_IO_ADDR() will
// be familiar ground.)
//
// You can only call void func(void) style routines with this code.  No
// parameters.  Sorry.

void pci_define(uint8_t pinx, uint8_t bit, void (*pPCI_Func)(void));

#endif //_PIN_CHANGE_
