/* copyright
   * (c) 2004 Stephen Humble stephenhumble at telstra.com
   *
   *  This file is part of robotx see http://Robotx.sourceforge.net
   *  dual sensorless motor driver firmware.
   *
   *  This is free software; you can redistribute it and/or modify
   *  it under the terms of the GNU General Public License as published by
   *  the Free Software Foundation; either version 2 of the License, or
   *  (at your option) any later version.
*/
#include <io.h>
#include <avr/io.h>
// #include <inttypes.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#ifndef BRIDGE
#define BRIDGE

// below are the supported bridge operations for [qu]bridge_set()
enum {  BROFF=0,
	   BRWAIT,
	   BRCAL1,
	   BRCAL2,
	   BRGO,
	   TRIP,
	   BR_FWDSTOP,
	   BR_REVSTOP,
	   BR_FWD,
	   BR_REV,
	   BR_WAIT,
	   BR_HIGH,
	   BR_LOW,
	   BR_OFF
     };


// struct encapsulate most of the info to operate one H bridge
struct hb{
      uint16_t current1; // latest current sample
      uint16_t voltage1; // latest voltage sample
      uint16_t current2; // latest current sample
      uint16_t voltage2; // latest voltage sample
      uint8_t  dir;  // present drive direction
      uint8_t  set; // PWM drive value
      int8_t  dirnext; // next state value to shift to from neutral
      uint8_t   head;  // index pointer for linked list head
      uint8_t   tail;
      int16_t   speed; // required motor speed
};

void qbridge_set(uint8_t speed,uint8_t dir);
void ubridge_set(uint8_t speed,uint8_t dir);
void bridge_set(struct hb *q,uint8_t speed,uint8_t dir);
extern uint16_t qcurrent1; // latest current sample
extern uint16_t qvoltage1; // latest voltage sample
extern uint16_t ucurrent1; // latest current sample
extern uint16_t uvoltage1; // latest voltage sample
extern uint16_t qcurrent2; // latest current sample
extern uint16_t qvoltage2; // latest voltage sample
extern uint16_t ucurrent2; // latest current sample
extern uint16_t uvoltage2; // latest voltage sample

extern struct hb hbq;
extern struct hb hbu;
#endif

