#ifndef PS2_KEYBOARD_H
#define PS2_KEYBOARD_H
#include "_ps2.h"
/**
 * PS2 Keyboard driver for the AVRII-Mistral debug port
 * To use this driver you must first call init_kbd();
 * Then you must register keyboard-handler function.
 * 
 * This driver does not support the special keys PAUSE and PRINT SCREEN
 * 
 * The setled function may not work.
 *
 *
 * Author: Sverre Sundsdal (summerstudent 2004)
 */


/**
 * The keyboard event struct that is sent to the program when a key is pressed
 * The driver knows whether the modifier keys are down.
 */
struct keyevent {
  int key; //scancode
  unsigned char release; //is this a release-keyevent?
  unsigned char shift_down; 
  unsigned char ctrl_down; 
  unsigned char alt_down;
};

/**
 * This functions enables the debug-ps2-keyboard port and registers interrupts
 */
void init_kbd();
/**
 * This driver sets the led lamps.
 * NOT TESTED
 */
void ps2_kbd_setled(int caps, int scroll, int num);
/**
 * Registers a keyboard handling function
 */
void register_kbd_listener(void (*fp)(struct keyevent*));
/**
 * Unregisters the keyboard handling function
 */
void unregister_kbd_listener();

#endif /*KEYBOARD_H_*/
