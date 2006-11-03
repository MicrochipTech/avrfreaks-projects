#ifndef __AVRCONFIG_H_
#define __AVRCONFIG_H_

/*
$Id: avrconfig.h,v 1.1.1.1 2006/01/18 05:29:35 nemo Exp $
*/
//The maximal possible delay is 262.14 ms / F_CPU in MHz.

//We are using a 8MHz crystal
//#define F_CPU 8322300UL

//default chip runs at 1MHz via internal clk
//max delay is 53.33 ms
//#define F_CPU 4915200UL

//avronics main board
//21.845 ms max delay
#define F_CPU 1000000UL


//RTI.H
//heartbeat light controls to show that the system is running
/* enable heartbeat */
#define HEARTBEAT
/* set flash rate */
#define HEARTRATE 128 
/* set the output pin to use */
#define HEARTBIT_TOGGLE PORTB ^= 4

#endif
