
/****************************************************************************
 Title  :   C  include file for the SRF08 FUNCTIONS library (srf08.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      13/Jul/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/

#ifndef SRF08_H
#define SRF08_H     1

/*##############################################################################################*/
/*                             START OF CONFIGURATION BLOCK                                     */
/*##############################################################################################*/
 
/*                            THERE IS NOTHING TO CONFIGURE                                     */


/*##############################################################################################*/
/*                              END OF CONFIGURATION BLOCK                                      */
/*##############################################################################################*/

/* Normally you shouldn't need to change anything */
/* Up to 16 SRF08 units are possible on the I2C bus */

#define SRF08_UNIT_0   0xE0  /* FIRST SRF08 MODULE I2C address      */
#define SRF08_UNIT_1   0xE2  /* SECOND SRF08 MODULE I2C address     */
#define SRF08_UNIT_2   0xE4  /* THIRD SRF08 MODULE I2C address      */
#define SRF08_UNIT_3   0xE6  /* FOURTH SRF08 MODULE I2C address     */
#define SRF08_UNIT_4   0xE8  /* FIFTH SRF08 MODULE I2C address      */
#define SRF08_UNIT_5   0xEA  /* SIXTH SRF08 MODULE I2C address      */
#define SRF08_UNIT_6   0xEC  /* SEVENTH SRF08 MODULE I2C address    */
#define SRF08_UNIT_7   0xEE  /* EIGHTH SRF08 MODULE I2C address     */
#define SRF08_UNIT_8   0xF0  /* NINTH SRF08 MODULE I2C address      */
#define SRF08_UNIT_9   0xF2  /* TENTH SRF08 MODULE I2C address      */
#define SRF08_UNIT_10  0xF4  /* ELEVENTH SRF08 MODULE I2C address   */
#define SRF08_UNIT_11  0xF6  /* TWELFTH SRF08 MODULE I2C address    */
#define SRF08_UNIT_12  0xF8  /* THIRTEENTH SRF08 MODULE I2C address */
#define SRF08_UNIT_13  0xFA  /* FOURTEENTH SRF08 MODULE I2C address */
#define SRF08_UNIT_14  0xFC  /* FIFTEENTH SRF08 MODULE I2C address  */
#define SRF08_UNIT_15  0xFE  /* SIXTEENTH SRF08 MODULE I2C address  */


#define SRF08_BROADCAST_ADDRESS   0X00

#define SRF08_MIN_GAIN        0      /* sets gain to 94   */
#define SRF08_MAX_GAIN        31     /* sets gain to 1025 */
#define SRF08_MIN_RANGE       0      /* in millimeters    */
#define SRF08_MAX_RANGE       11008  /* in millimeters    */

#define SRF08_INCHES          0X50
#define SRF08_CENTIMETERS     0X51
#define SRF08_MICROSECONDS    0X52

/* register positions */
#define SRF08_SW_VERSION      0         /* when reading it */
#define SRF08_COMMAND         0         /* when writing it */
#define SRF08_LIGHT           1         /* when reading it */
#define SRF08_GAIN            1         /* when writing it */
#define SRF08_RANGE           2         /* when writing it */
#define SRF08_ECHO_1          2         /* when reading it */
#define SRF08_ECHO_2          4         /* read only register */
#define SRF08_ECHO_3          6         /* read only register */
#define SRF08_ECHO_4          8         /* read only register */
#define SRF08_ECHO_5          10        /* read only register */
#define SRF08_ECHO_6          12        /* read only register */
#define SRF08_ECHO_7          14        /* read only register */
#define SRF08_ECHO_8          16        /* read only register */
#define SRF08_ECHO_9          18        /* read only register */
#define SRF08_ECHO_10         20        /* read only register */
#define SRF08_ECHO_11         22        /* read only register */
#define SRF08_ECHO_12         24        /* read only register */
#define SRF08_ECHO_13         26        /* read only register */
#define SRF08_ECHO_14         28        /* read only register */
#define SRF08_ECHO_15         30        /* read only register */
#define SRF08_ECHO_16         32        /* read only register */
#define SRF08_ECHO_17         34        /* read only register */

/* Function Declaration */

extern void          srf08_select_unit(unsigned char srf08_address);

extern void          srf08_set_gain(unsigned char gain);
extern void          srf08_set_range(unsigned int millimeters);

extern unsigned int  srf08_ping(unsigned char range_unit);
extern unsigned int  srf08_read_register(unsigned char srf08_register);

extern void          srf08_change_i2c_address(unsigned char new_i2c_address);


#endif  /* #ifndef SRF08_H */
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

