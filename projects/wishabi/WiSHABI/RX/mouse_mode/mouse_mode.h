//#include "usb.h"
//#include "serial.h"
#include "WiSHABI_RX.h"

//Transmitter thumb buttons correspond to a mouse click when in mouse mode
#define MOUSE_BUTTON_LEFT	0	//Transmitter Button B
#define MOUSE_BUTTON_RIGHT	1	//Transmitter Button C
#define MOUSE_BUTTON_CENTER	2	//Transmitter Button A

//The number of mouse mode loop iterations while sending delta x and y for
//mouse motion that the magnitude of the motion remains at its current value.
//This way the cursor moves slowly at first, then speeds up slightly, then
//speeds up again to max speed.
#define DELTA_MAGNITUDE_COUNTER_SPEED_1 10
#define DELTA_MAGNITUDE_COUNTER_SPEED_2 200

//The magnitude of delta x and y depending on the current speed.
#define DELTA_MAGNITUDE_SPEED_1 1
#define DELTA_MAGNITUDE_SPEED_2 5
#define DELTA_MAGNITUDE_SPEED_3 10

/////////////////////////////////////////////////////////
//Defined in the main code file - WiSHABI_RX.c
extern unsigned char Global_RX_Bytes[4];
extern unsigned char Global_Timeout;
extern unsigned char Global_Mode;

extern void usbPoll (void);
extern unsigned char Poll_Sector (void);
extern void Poll_And_Update_LEDs(unsigned char mode);
/////////////////////////////////////////////////////////

//Exclusive to mouse_mode.c:

//Sends mouse commands to PC including button conditions (buttons) plus motion in x (delta_x) and y (delta_y) directions.
void Send_Mouse_Command (unsigned char buttons, int delta_x, int delta_y);
//Check Buttons A, B & C for mouse button control
unsigned char Get_Mouse_Buttons (void);
//Determine mouse motion value along the PC's X axis.
signed char Get_Delta_X (unsigned char sector, unsigned char magnitude);
//Determine mouse motion value along the PC's Y axis.
signed char Get_Delta_Y (unsigned char sector, unsigned char magnitude);
//Main Mouse Mode loop
void Mouse_Mode (void);
