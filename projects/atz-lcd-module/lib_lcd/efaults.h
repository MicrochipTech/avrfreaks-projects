#ifndef __LCD_DEFAULTS_H__
#define __LCD_DEFAULTS_H__

#ifndef LCD_WAIT_40NS
#define LCD_INCLUDE_WAIT_H
#define LCD_WAIT_40NS() (wait_40ns())
#endif

#ifndef LCD_WAIT_50NS
#define LCD_INCLUDE_WAIT_H
#define LCD_WAIT_50NS() (wait_50ns())
#endif

#ifndef LCD_WAIT_80NS
#define LCD_INCLUDE_WAIT_H
#define LCD_WAIT_80NS() (wait_80ns())
#endif

#ifndef LCD_WAIT_150NS
#define LCD_INCLUDE_WAIT_H
#define LCD_WAIT_150NS() (wait_150ns()) 
#endif

#ifndef LCD_WAIT_150NS_AND_1CLOCK
#define LCD_INCLUDE_WAIT_H
#define LCD_WAIT_150NS_AND_1CLOCK() (wait_150ns_and_1clock()) 
#endif

#ifdef LCD_INCLUDE_WAIT_H
#include <misc.h>
#endif

#endif
