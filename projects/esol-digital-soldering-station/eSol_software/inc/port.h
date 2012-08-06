/*
 * port.h
 *
 *  Created on: Jan 14, 2012
 *      Author: Alex Costache
 */

#ifndef PORT_H_
#define PORT_H_

//Global defines
#define PowerHeater  PD5

#define EncoderA     PA4
#define EncoderB     PA5
#define Spare        PA3

/*7 segment display*/
#define DisplaySegA  PC0
#define DisplaySegB  PC7
#define DisplaySegC  PC4
#define DisplaySegD  PC2
#define DisplaySegE  PC1
#define DisplaySegF  PC5
#define DisplaySegG  PC6
#define DisplaySegDP PC3
#define Display0GND  PD2
#define Display1GND  PD0
#define Display2GND  PD1

/*Upper button*/
#define UpperButton  PD6
#define LowerButton  PD4

/*Lower button*/

#define ZERO ((1<<DisplaySegA) | (1<<DisplaySegB) | (1<<DisplaySegC) | (1<<DisplaySegD) | (1<<DisplaySegE) | (DisplaySegF))

typedef struct
{
   uint8_t           uiDisplayDigit0;
   uint8_t           uiDisplayDigit1;
   uint8_t           uiDisplayDigit2;
   bool              bDisplayDigit0dot;
   bool              bDisplayDigit1dot;
   bool              bDisplayDigit2dot;
}  tDisplayControl;

extern volatile tDisplayControl  DisplayControlInfo;

/*Global functions*/
extern void    gvPortInit(void);
extern void	   gvDisplayInit(void);
extern void    gvDisplayWriteDigit(uint8_t digit, uint8_t value, bool dot);
extern uint8_t gu8DisplayBcdTo7Segment(uint8_t value, bool dot);
extern void    gvDisplayRefreshFct(void);
extern void    gvDisplaySetNewValue(uint16_t value, bool digitdot0, bool digitdot1, bool digitdot2);

//Alex
extern void    gvWriteErrorDisplay(void);
extern void    gvWriteLowDisplay(void);
extern bool    gvReadUpperButtonPressed(void);
extern bool    gvReadLowerButtonPressed(void);
extern uint8_t guiPortReadPin(uint8_t port, uint8_t pin);
//end Alex

void itobcd(unsigned int val, bool digitdot0, bool digitdot1, bool digitdot2);

#endif /* PORT_H_ */
