//**********************************************************************//
//FILE: rtc_ds1307.c 
//AUTHOR: Adam Kadolph
//DATE:	12-5-08
//DESCRIPTION: This file handles the DS1307 Real Time Clock interface to the AVR 
//via TWI. And includes RTC init, RTC get time values, RTC set time values
//**********************************************************************//

#include <avr/io.h>
#include "twi.h"
#include "rtc.h"
#include "lcd.h"

//start Real Time Clock functions
void RTCinit(void)
{
	//routine initializes the Real Time Clock by setting value in Control Register
	unsigned char error;
	
	error = I2C_start();
	if(error == 1)
	{
		I2C_stop();
		return;
	} 
   
	error = I2C_sendAddr(DS1307_W);
    if(error == 1)
	{
		I2C_stop();
		return;
	} 
   
	error = I2C_sendData(0x00);
	if(error == 1)
	{
		I2C_stop();
		return;
	} 

	I2C_stop();
}

void RTC_read(void)
{
	unsigned char error, i, data;
  
	error = I2C_start();
	if(error == 1)
	{
		I2C_stop();
		return;
	} 
   
	error = I2C_sendAddr(DS1307_W);
    if(error == 1)
	{
		I2C_stop();
		return;
	} 
   
	error = I2C_sendData(0x00);
	if(error == 1)
	{
		I2C_stop();
		return;
	} 

	error = I2C_repeatStart();
	if(error == 1)
	{
   		I2C_stop();
		return;
	} 
   
	error = I2C_sendAddr(DS1307_R);
    if(error == 1)
	{
		I2C_stop();
		return;
	} 
 
    for(i=0;i<7;i++)
	{
		if(i == 6)  	 //no Acknowledge after receiving the last byte
	   		data = I2C_receiveData_NACK();
		else
	  		data = I2C_receiveData_ACK();
		  
   		if(data == ERROR_CODE)
   		{
			I2C_stop();
	   		return;
   		}
	  
		rtc_register[i] = data;
	}
	
	I2C_stop();
}	  

void RTC_getTime(void)
{
   RTC_read();
   time[8] = 0x00;	  //NIL
   time[7] = (SECONDS & 0x0f) | 0x30;			//seconds(1's)
   time[6] = ((SECONDS & 0x70) >> 4) | 0x30;	//seconds(10's)
   time[5] = ':';
   
   time[4] = (MINUTES & 0x0f) | 0x30;			//minutes(1's)
   time[3] = ((MINUTES & 0x70) >> 4) | 0x30;	//minutes(10's)
   time[2] = ':'; 
   
   time[1] = (HOURS & 0x0f) | 0x30;				//hours(1's)
   time[0] = ((HOURS & 0x30) >> 4) | 0x30;		//hours(10's)
}

void RTC_displayTime(void)
{
  RTC_getTime();
  
  LCDstring(time,8);
}
	  		     	  
void RTC_updateRegisters(void)
{
  SECONDS = ((time[6] & 0x07) << 4) | (time[7] & 0x0f);
  MINUTES = ((time[3] & 0x07) << 4) | (time[4] & 0x0f);
  HOURS = ((time[0] & 0x03) << 4) | (time[1] & 0x0f);  
}  

unsigned char RTC_writeTime(void)
{
	unsigned char error, i;
  
	error = I2C_start();
	if(error == 1)
	{
   		I2C_stop();
		return(1);
	} 
   
	error = I2C_sendAddr(DS1307_W);
    if(error == 1)
	{
		I2C_stop();
		return(1);
	} 
   
	error = I2C_sendData(0x00);
	if(error == 1)
	{
		I2C_stop();
		return(1);
	} 

    for(i=0;i<3;i++)
    {
		error = I2C_sendData(rtc_register[i]);  
   		if(error == 1)
   		{
       		I2C_stop();
	   		return(1);
   		}
    }
	
	I2C_stop();
	return(0);
}
