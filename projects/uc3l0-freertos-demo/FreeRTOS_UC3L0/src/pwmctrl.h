/********************************************************************
	created:	2012/03/05
	created:	5:3:2012   22:51
	filename: 	C:\Users\Wedge\Dropbox\school\Thesis\Code\AVR\FreeRTOS_UC3L0\FreeRTOS_UC3L0\src\pwmctrl.h
	file path:	C:\Users\Wedge\Dropbox\school\Thesis\Code\AVR\FreeRTOS_UC3L0\FreeRTOS_UC3L0\src
	file base:	pwmctrl
	file ext:	h
	author:		Dustin Sanders
	
	purpose:	Handles control of PWM
*********************************************************************/
 

// May not need you
#ifndef PWMCTRL_H_
#define PWMCTRL_H_

portBASE_TYPE lPWMInit( void );

portBASE_TYPE lPWMHandleMenu( U8 *menuPtr );

portBASE_TYPE lPWMHandleMenuActions(U8 *menuPtr, portBASE_TYPE CH, portBASE_TYPE pwmValue);

portBASE_TYPE lPWMHandleMenuFreq( U8 *menuPtr, portBASE_TYPE pwmValue );

void vPWMHandleDisplay( unsigned long dev );

#endif /* PWMCTRL_H_ */
