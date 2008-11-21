#include "keymap.h"
//#include "usb.h"
//#include "serial.h"
#include "WiSHABI_RX.h"

//The number of loop iterations in function Type_By_Sector before
//the typed character is confirmed and the function is exited.
#define TYPE_TIMEOUT 50

/////////////////////////////////////////////////////////
//Defined in the main code file - WiSHABI_RX.c
extern unsigned char Global_RX_Bytes[4];
extern unsigned char Global_Timeout;
extern unsigned char Global_Mode;

extern void usbPoll (void);
extern unsigned char Poll_Sector (void);
extern void Poll_And_Update_LEDs(unsigned char mode);
/////////////////////////////////////////////////////////

//Exclusive to keyboard_mode.c:

//Sends a keyboard keypress (c) along with any relevant modifiers (mod) (shift, alt, ctrl or GUI)
void Send_Keyboard_Char (unsigned char mod, unsigned char c);
//When a button press is detected, this function can be used to wait
//until said button is released.  The parameter 'key' represents a
//keyboard character that will continuously be sent to the PC until
//the button is released.  Usually this will be KEY_NO_KEY.
void Wait_Till_Release(unsigned char button, unsigned char key);
//This function takes the address of the character that has just
//been sent to the the PC and replaces the contents of that address
//with the next character in the cycle (depending on the tilt
//orientation).  E.g. a->b->c->2->a
void Increment_Char(unsigned char *mod, unsigned char *c);
//This loop runs when button E is pressed.  It checks the tilt orientation
//and sends to the PC the corresponding keystroke.  It then waits for the
//typing timeout before exiting.  If button E is pressed again before said
//timeout, the previously typed character will be replaced with the next
//character in the cycle (depending on the tilt orientation).
void Type_By_Sector(unsigned char sect);
//Main Keyboard Mode loop
void Keyboard_Mode (void);

