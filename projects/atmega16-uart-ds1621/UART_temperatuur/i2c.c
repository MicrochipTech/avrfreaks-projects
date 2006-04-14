/***********************************************************************
Content: Library for the I2C protocol inc. some HW specific functions
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
Includes
***********************************************************************/
#include "common.h"
#include "i2c.h"


/***********************************************************************
Init the hardware for I2C communication
***********************************************************************/
void i2c_init(void)
{
  SET_SDA();
  SET_SCL();
  SET_SDA_TO_OUTPUT();
  SET_SCL_TO_OUTPUT();
  waitms(500);
}


/***********************************************************************
Generate I2C START condition
***********************************************************************/
void i2c_start(void)
{
  SET_SDA();
  SET_SCL();
  waitms(2);
  CLEAR_SDA();
  waitms(2);
  CLEAR_SCL();
  waitms(2);
}


/***********************************************************************
Generate I2C STOP condition
***********************************************************************/
void i2c_stop(void)
{
  CLEAR_SDA();
  SET_SCL();
  waitms(2);
  SET_SDA();
  waitms(2);
}


/***********************************************************************
Write one data byte to the I2C bus
***********************************************************************/
void i2c_write(BYTE data)
{
  BYTE idx;
  SET_SDA_TO_OUTPUT();
  for(idx=0; idx<8; ++idx)
  {
    if(data & 0x80) SET_SDA(); else CLEAR_SDA();
    data <<= 1;
    SET_SCL();
    waitms(2);
    CLEAR_SCL();
	waitms(2);
  }
  SET_SDA_TO_INPUT();
  CLEAR_SDA();
  SET_SCL();
  waitms(2);
  CLEAR_SCL();
  waitms(2);
  SET_SDA_TO_OUTPUT();
}


/***********************************************************************
Read one data byte from the I2C bus (8-bit MSB first)
Set lastbyte to true in order to read the crc byte as last reading
***********************************************************************/
BYTE i2c_read(BOOLEAN lastbyte)
{
  BYTE idx, data;
  SET_SDA_TO_INPUT();
  CLEAR_SDA();
  data = 0;
  for(idx=0; idx<8; ++idx)
  {
    SET_SCL();
    waitms(2);
    data <<= 1;
    if(SDA_SET()) ++data;
    CLEAR_SCL();
    waitms(2);
  }
  if(lastbyte) SET_SDA(); else CLEAR_SDA();
  SET_SDA_TO_OUTPUT();
  SET_SCL();
  waitms(2);
  CLEAR_SCL();
  waitms(2);
  return data;
}


/***********************************************************************
Function for reading the Dallas DS1621 temperature sensor
The address must be the same as the hardware address of sensor (0..7)
Returns -99 if device is not found or is not responding.
***********************************************************************/
double get_ds1621_temperature(BYTE address)
{
  BYTE msb,lsb;
  double tmp;
  address = address * 2;
  i2c_start();
  i2c_write(0x90 + address);
  i2c_write(0xAC);
  i2c_write(0x20);
  i2c_start();
  i2c_write(0x90 + address);
  i2c_write(0xEE);
  i2c_start();
  i2c_write(0x90 + address);
  i2c_write(0xAA);
  i2c_start();
  i2c_write(0x91 + address);
  msb = i2c_read(FALSE);
  lsb = i2c_read(TRUE);
  i2c_stop(); 
  tmp = msb * 256;
  tmp = tmp + lsb;
  tmp = tmp / 128;
  tmp = tmp * 5;
  tmp = tmp / 10;
  if (msb > 127) tmp = tmp - 256;
  if (tmp < -55.0 || tmp > 150.0) tmp = -99;
  return tmp;  
}


/***********************************************************************
End of code listing
***********************************************************************/