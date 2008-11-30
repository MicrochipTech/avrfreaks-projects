#ifndef MOUSE_H_
#define MOUSE_H_
#include "_ps2.h"
/**
 * PS2 Mouse driver for the AVRII-Mistral debug port
 * To use this driver you must first call init_mouse();
 * Then you must register mouse-handler function.
 * If you unplug the mouse you must call init_mouse again.
 * 
 * Author: Sverre Sundsdal (summerstudent 2004)
 */

/**
 * Mouseevent struct used for passing mouse events to the program
 */
struct mouseevent{
  int x; //relative x movement
  int y; //relative y movement
  unsigned char middle; //middle button pressed
  unsigned char left; //left button pressed
  unsigned char right; //right button pressed
};
/*
 * This function enables the debugport and interrupt lines.
 * It resets the mouse and enables data-reporting.
 */
int init_mouse();
/*
 * Registers a mouse handling function
 */
void register_mouse_listener(void (*fp)(struct mouseevent*));
/**
 * Removes the mouse handler
 */
void unregister_mouse_listener();


/*
 * Mouse defines - only used internal to the driver
 */
#define MOUSE_RESET 		0xFF
#define MOUSE_RESEND 		0xFE
#define MOUSE_DEFAULT 		0xF6
#define MOUSE_DISABLE 		0xF5
#define MOUSE_ENABLE 		0xF4
#define MOUSE_SET_SAMPLE 	0xF3
#define MOUSE_GET_ID 		0xF2
#define MOUSE_REMOTE 		0xF0
#define MOUSE_WRAP 			0xEE
#define MOUSE_RESET_WRAP 	0xEC
#define MOUSE_READ 			0xEB
#define MOUSE_STREAM 		0xEA
#define MOUSE_STATUS 		0xE9
#define MOUSE_SET_RES 		0xE8
#define MOUSE_SCALE_2 		0xE7
#define MOUSE_SCALE_1 		0xE6
#define MOUSE_ERROR 		0xFC
#define MOUSE_ACK 			0xFA
#define MOUSE_BAT_OK 		0xAA

/*
 * Bits in the first databyte during data reporting.
 */
#define MOUSE_YO 	7 //Y overflow
#define MOUSE_XO 	6 //X overflow
#define MOUSE_YS 	5 //X sign
#define MOUSE_XS 	4 //Y sign
#define MOUSE_MID 	2 //Middle button
#define MOUSE_RIGHT 1 //Right button
#define MOUSE_LEFT 	0 //Left button

#endif /*MOUSE_H_*/
