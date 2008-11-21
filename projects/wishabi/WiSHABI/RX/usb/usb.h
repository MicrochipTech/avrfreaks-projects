#include <avr/pgmspace.h>
#include "usbdrv.h"
#include "oddebug.h"

#define nop()	__asm__ __volatile__ ("nop");
#define reset()	__asm__ __volatile__ ("rjmp 0");

// The LED states - Used to determine the current state of keyboard leds.
// Currently only care about the Caps Lock LED. (See usbFunctionWrite)
/*#define LED_NUM     0
#define LED_CAPS    1
#define LED_SCROLL  2
#define LED_COMPOSE 3
#define LED_KANA    4*/

// The uC end LED configuration
/*#define LED_PORT		PORTC
#define LED_CAPS_LOCK	0
#define LED_NUM_LOCK	1*/

extern uchar inputBuffer1[8];	//The input buffer sent to usbSetInterrupt() to emulate a keyboard event.
extern uchar inputBuffer2[4];	//The input buffer sent to usbSetInterrupt() to emulate a mouse event.
extern uchar idleRate;			//In 4 ms units.  Used in usbFunctionSetup();
extern uchar protocolVer;		//Used in usbFunctionSetup();
extern uchar tx_kbd;			//Flag used to determine whether or not inputBuffer1[] has changed and needs to be sent.
extern uchar tx_mouse;			//Flag used to determine whether or not inputBuffer2[] has changed and needs to be sent.
extern uchar expectReport;		//Flag used to determine if the status of the keyboard LEDs is to be checked.
extern uchar LEDstate;			//Variable used to represent the condition of the 5 LEDs (Bits 0 to 4 see #defines).

//The following function defines how data received from the PC is handled
extern uchar usbFunctionSetup(uchar data[8]);

//The following function is used to process data received from the PC.  In this case it checks the condition of the
//PC's Caps Lock LED and toggles the condition of an LED accordingly.
extern uchar usbFunctionWrite(uchar *data, uchar len);

//A thorough reset: loop 4 times outputting SE0 then idle.
//This works around some intermitent connection problems.
extern void usbReset(void);

//Sets all variable inputBuffer values to 0.
extern void hid_clear(void);

//Sends the latest inputBuffers to usbSetInterrupt().
extern void send_packets(void);

