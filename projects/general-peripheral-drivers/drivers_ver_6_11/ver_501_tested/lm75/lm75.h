
/****************************************************************************
 Title  :   C  include file for the LM75 FUNCTIONS library (srf08.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      13/Jul/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/

#ifndef LM75_H
#define LM75_H     1

/*##############################################################################################*/
/*                             START OF CONFIGURATION BLOCK                                     */
/*##############################################################################################*/

#define LM75_WILL_USE_A_TIMER     0     /* 1=use a 8 bit timer, 0=use software delay loop */
#define LM75_USE_8_BIT_TIMER      0     /* the 8 bit timer to be used */
#define LM75_CONVERSION_TIME_MS   110   /* milliseconds LM75 need to complete an AD conversion */

/*##############################################################################################*/
/*                              END OF CONFIGURATION BLOCK                                      */
/*##############################################################################################*/

#define LM75_UNIT_0       0x90  /* FIRST LM75 MODULE I2C address      */
#define LM75_UNIT_1       0x92  /* SECOND LM75 MODULE I2C address     */
#define LM75_UNIT_2       0x94  /* THIRD LM75 MODULE I2C address      */
#define LM75_UNIT_3       0x96  /* FOURTH LM75 MODULE I2C address     */
#define LM75_UNIT_4       0x98  /* FIFTH LM75 MODULE I2C address      */
#define LM75_UNIT_5       0x9A  /* SIXTH LM75 MODULE I2C address      */
#define LM75_UNIT_6       0x9C  /* SEVENTH LM75 MODULE I2C address    */
#define LM75_UNIT_7       0x9E  /* EIGHTH LM75 MODULE I2C address     */

#define LM75_TEMP_REG     0
#define LM75_CONFIG_REG   1
#define LM75_HYST_REG     2
#define LM75_TOS_REG      3

/* Configuration register Bit positions */ 
#define LM75_SHUTDOWN     0
#define LM75_INT_MODE     1
#define LM75_OS_HIGH      2
#define LM75_FQUE0        3
#define LM75_FQUE1        4

/*
    The temperature can be negative or positive and the accuracy is 1 degree Celsius
    The LM75 is sending the temperature over the I2C bus in 0,5 degrees steps 
    but there is no point to read the whole value from the temperature register
    since the LM75 sensor accuracy is 2~3 degrees Celsius.
*/
extern signed char  lm75_get_temp(void);
extern void         lm75_select_unit(unsigned char lm75_address);
extern void         lm75_configure(unsigned char config_byte);
extern signed int   lm75_get_reg(unsigned char reg_number);
extern void         lm75_set_hyst(signed char hyst_temp);
extern void         lm75_set_tos(signed char tos_temp);


#endif  /* #ifndef LM75_H */
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

