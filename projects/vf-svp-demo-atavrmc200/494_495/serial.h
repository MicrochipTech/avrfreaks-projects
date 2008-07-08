/**
* @file serial.h
*
* Copyright (c) 2004 Atmel.
*
* @brief This module provide function's prototypes for AT90PWM3 Only
* @version 1.0 (CVS revision : 1.1)      
* @date 2006/03/09 16:33:40    
* @author raubree 
*****************************************************************************/

void init_uart(void);
void initbootuart();
void sendchar( char );
char recchar( void );
char tstrx(void);
void putstring(char *str);
void putint(int number);
void putU8(U8 number);
void putS16(S16 number);
