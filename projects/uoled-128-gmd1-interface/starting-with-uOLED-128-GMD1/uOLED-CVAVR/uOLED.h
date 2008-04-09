/*****************************************************
File: uoled.h

Project : uoled-interface
Version : 0.99
Date    : 3/18/2008
Author  : Walt Sacuta
Company : Arctan Systems
Comments:

*****************************************************/


#define RXB8 1
#define TXB8 0
#define UPE 2
#define OVR 3
#define FE 4
#define UDRE 5
#define RXC 7
#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<OVR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

#define FULL_RED        (0xF800)
#define FULL_BLUE       (0x001F)
#define FULL_GREEN      (0x07E0)

#define FORWARD         1
#define REVERSE         0
#define TRUE            1
#define CIRC_RADIUS     10
#define CIRC_DIA        (CIRC_RADIUS*2)
#define DISP_MIN        0
#define DISP_MAX        127
#define START_X         (CIRC_RADIUS+1)
#define START_Y         (CIRC_RADIUS+1)
#define BOX_WID         (CIRC_DIA+2)
#define BOX_LEN         (CIRC_DIA+2)

// msec delays (STARTUP_DELAY is specified in datasheet).
#define BAUD_SYNC_DELAY 20
#define LOOP_DELAY      10
#define STARTUP_DELAY   750
#define PIXEL_DIV       100
#define COMMAND_DELAY   (3)

// arbitrary velocities, should add up to 200 for average 2 pixels shift per loop.
#define X_VEL   (-76)
#define Y_VEL   (-124)

// uOLED commands with data byte count.
//      Command                     data bytes (after command)
#define CM_SHIFT_BLOCK  0x63        //  6
#define CM_DRAW_CIRCLE  0x43        //  5
#define CM_BAUD_SYNC    0x55        //
#define CM_CLEAR_DISP   0x45        //  0
#define CM_BACKGND_COL  0x42        //
#define CM_TEXT_BUTTON  0x62        //  variable
#define CM_DRAW_BITMCH  0x44        //  5
#define CM_SET_FONTSIZE 0x46        //  1
#define CM_DRAW_TRIANG  0X47        //  8
#define CM_DRAW_POLY    0x67        //  variable
#define CM_DRAW_IMAGE   0x49        //  variable
#define CM_DRAW_LINE    0x4C        //  6
#define CM_TEXT_MODE    0x4F        //  1
// etc...



typedef unsigned char u8;
typedef unsigned int u16;

void drawRandomColourCircle(u8 xC,u8 yC);
void drawCircle(u8 xstart,u8 ystart,u8 radius,u16 colour);
void shiftBlock(u8 xstart,u8 ystart,u8 xnew,u8 ynew,u8 width,u8 height);
void clearDisplay(void);
void baudRateSync(void);
void initRegisters(void);



/*****************************************************
 end of file: uoled.h
*****************************************************/


