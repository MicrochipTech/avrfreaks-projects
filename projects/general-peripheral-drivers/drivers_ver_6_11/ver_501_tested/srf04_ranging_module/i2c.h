
/****************************************************************************
 Title	:   C  include file for the I2C FUNCTIONS library (i2c.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:	    9/3/2003 2:07:24 μμ
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/
/*
    The i2c driver now has better error control. Due to different hardware vendor
    techniques used to signal a device busy situation, i made some alterations so
    the driver can cope with the varius techniques.
    Remember that there might be some uknown to me situations where things might not
    go as expected. After all i dont know everything in this world.
    If you find something wrong please drop me a note.
*/


#ifndef I2C_H
#define I2C_H	1

/*##############################################################################################*/
/*			       START OF CONFIGURATION BLOCK					*/
/*##############################################################################################*/
/*
   The I2C_CLOCK normally is 100 Khz so the delay between level states is 
   (1/I2C_CLOCK)/2 = ( (1/100000)/2 ) = 5 microseconds.
   The use of delay loop for timing is highly accurate due to the use of inline asm code.
   There is no need to use a timer but if you insist you have that option.
   The I2C_TIMEOUT_TIME_IN_MICROSECONDS is set to whatever is the maximum time you think
   your device will take to perform the requested task. After that time the i2c functions
   will return a "I2C_ERROR_DEVICE_NOT_RESPONDING" or "I2C_ERROR_DEVICE_BUSY" error code. 
*/ 


#ifndef  F_CPU
#define  F_CPU		              3686400   /* The cpu clock frequency in Hertz */
#endif

#define I2C_SDA_PORT	              B	      /* The SDA port. Use capital letter (A,B,C,D... etc.) */
#define SDA_PIN 	              7	      /* The SDA port pin */

#define I2C_SCL_PORT	              B	      /* The SCL port. Use capital letter (A,B,C,D... etc.) */
#define SCL_PIN 	              5	      /* The SCL port pin */

#define I2C_CLOCK_IN_HERTZ            400000  /* in HERTZ */
#define I2C_TIMEOUT_IN_MILLISECONDS   100     /* in milliseconds  */

#define I2C_WILL_USE_A_TIMER          0       /* 0=use delay loop, 1=use a 8 bit timer */
#define I2C_USE_TIMER                 0       /* 0=TIMER0, 2=TIMER2 (valid if I2C_WILL_USE_A_TIMER==1) */

#define I2C_ERROR_CHECK_ENABLE        1       /* 0=no error checking. Smaller code size */

/* If I2C_ERROR_CHECK_ENABLE == 0 then the below 3 definitions are inactive  */
/* If there is only one master on the bus there is no need for additional bus testing */
#define I2C_ISSUE_BLIND_START         1       /* 1=skip start checks, 0=check for an occupied bus */
#define I2C_ISSUE_BLIND_STOP          1       /* 1=skip stop checks, 0=check for a correct stop.  */
#define I2C_START_CHECK_CYCLES        3       /* The time in cycles to watch for an occupied i2c bus */





/*##############################################################################################*/
/*				END OF CONFIGURATION BLOCK					*/
/*##############################################################################################*/

/* Keyword definitions */

#define I2C_BROADCAST                    0X00

#define I2C_READ                   	 1
#define I2C_WRITE                    	 0

#define I2C_QUIT                       	 0
#define I2C_CONTINUE	                 1

#if I2C_ERROR_CHECK_ENABLE == 1

#define I2C_NO_ERROR			 0
#define I2C_ERROR_DEVICE_BUSY		 1
#define I2C_ERROR_DEVICE_NOT_RESPONDING  2
#define I2C_ERROR_BUS_OCCUPIED           3

#endif

/********************************************************************************************************/
/*                             PUBLIC FUNCTION PROTOTYPES                                               */
/********************************************************************************************************/


extern void           i2c_start(void);
extern void	      i2c_stop(void);
extern unsigned char  i2c_get_byte(unsigned char ack);
extern void           i2c_put_byte(unsigned char data);
#if I2C_ERROR_CHECK_ENABLE == 1
extern unsigned char  i2c_get_error(void);
#endif

/********************************************************************************************************/
/*                                 MACRO DEFINITIONS                                                    */
/********************************************************************************************************/

#define i2c_init()                { }
#define i2c_start_tx(address)     { i2c_start(); i2c_put_byte(address); }         
#define i2c_start_rx(address)     { i2c_start(); i2c_put_byte(address|0x01); }

/* Macros intended for compatibility with older versions of the i2c driver. */
#define I2C_START_TX(address)     i2c_start_tx(address)              
#define I2C_START_RX(address)     i2c_start_rx(address)


#endif	/*  #ifndef I2C_H */
/*######################################################################################################*/
/*					   T H E   E N D						*/
/*######################################################################################################*/

