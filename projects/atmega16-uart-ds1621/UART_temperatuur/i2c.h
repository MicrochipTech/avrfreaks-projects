/***********************************************************************
Content: Library for the I2C interface inc. some HW specific functions
Created: 23.11.2003
Last modified: 17.02.2004
Copyrights: Free to use, free to change, free to delete :-)
Compiler: ImageCraft AVR
Written by: Knut Baardsen @ Baardsen Software, Norway
Updates: http://www.baso.no
------------------------------------------------------------------------
This software is provided "as is"; Without warranties either express or
implied, including any warranty regarding merchantability, fitness for 
a particular purpose or noninfringement. 
In no event shall Baardsen Software or its suppliers be liable for any 
special,indirect,incidential or concequential damages resulting from 
the use or inability to use this software.
***********************************************************************/



/***********************************************************************
Port and Pin configuration 
***********************************************************************/
#define I2CPORT 					PORTD
#define I2CPIN						PIND
#define I2CDDR						DDRD
#define SCL                         3
#define SDA                         2


/***********************************************************************
Macros
***********************************************************************/
#define SET_SCL()                   SETBIT(I2CPORT, SCL)
#define CLEAR_SCL()                 CLEARBIT(I2CPORT, SCL)
#define SCL_SET()                   (I2CPIN & BIT(SCL))
#define SET_SCL_TO_OUTPUT()         SETBIT(I2CDDR, SCL)
#define SET_SDA()                   SETBIT(I2CPORT, SDA)
#define CLEAR_SDA()                 CLEARBIT(I2CPORT, SDA)
#define SDA_SET()                   (I2CPIN & BIT(SDA))
#define SET_SDA_TO_OUTPUT()         SETBIT(I2CDDR, SDA)
#define SET_SDA_TO_INPUT()          CLEARBIT(I2CDDR, SDA)


/***********************************************************************
Common prototypes
------------------------------------------------------------------------
Init the hardware for I2C communication
***********************************************************************/
void i2c_init(void);

/***********************************************************************
Generate I2C START condition
***********************************************************************/
void i2c_start(void);

/***********************************************************************
Generate I2C STOP condition
***********************************************************************/
void i2c_stop(void);

/***********************************************************************
Write one data byte to the I2C bus
***********************************************************************/
void i2c_write(BYTE data);

/***********************************************************************
Read one data byte from the I2C bus (8-bit MSB first)
Set lastbyte to true in order to read the crc byte as last reading
***********************************************************************/
BYTE i2c_read(BOOLEAN lastbyte);


/***********************************************************************
Prototypes for reading the Dallas DS1621 temperature sensor
The address must be the same as the hardware address of sensor (0..7)
------------------------------------------------------------------------
Function for reading the Dallas DS1621 temperature sensor
The address must be the same as the hardware address of sensor (0..7)
Returns -99 if device is not found or is not responding.
***********************************************************************/
double get_ds1621_temperature(BYTE address);


/***********************************************************************
End of code listing
***********************************************************************/