//*************************************************************
//***** FUNCTIONS FOR RTC DS1307 ON I2C COMMUNICATION  *****
//*************************************************************
//Controller:	ATmega32 (16Mhz Crystal)
//Compiler:		ICCAVR Ver
//Author:		CC Dharmani, Chennai, India
//Date:			Aug 2008
//*************************************************************

#include <iom32v.h>
#include <macros.h>
#include "rtc_routines.h"
#include "UART_routines.h"
#include "i2c_routines.h"
#include "LCD_routines.h"

unsigned char rtc_register[7];
unsigned char time[10]; 		//xx:xx:xx;
unsigned char date[12];			//xx/xx/xxxx;
unsigned char day;

//******************************************************************
//Function to 
//******************************************************************
void RTC_setStartAddress(void)
{
   unsigned char errorStatus;
   
   errorStatus = i2c_start();
   if(errorStatus == 1)
   {
     transmitString_F("RTC start1 failed..");
   	 i2c_stop();
	 return;
   } 
   
   errorStatus = i2c_sendAddress(DS1307_W);
   
   if(errorStatus == 1)
   {
     transmitString_F("RTC sendAddress1 failed..");
	 i2c_stop();
	 return;
   } 
   
   errorStatus = i2c_sendData(0x00);
   if(errorStatus == 1)
   {
     transmitString_F("EEPROM write-2 failed..");
	 i2c_stop();
	 return;
   } 

   i2c_stop();
}

//******************************************************************
//Function to read 
//******************************************************************    
void RTC_read(void)
{

  unsigned char errorStatus, i, data;
  
  errorStatus = i2c_start();
   if(errorStatus == 1)
   {
     transmitString_F("RTC start1 failed..");
   	 i2c_stop();
	 return;
   } 
   
   errorStatus = i2c_sendAddress(DS1307_W);
   
   if(errorStatus == 1)
   {
     transmitString_F("RTC sendAddress1 failed..");
	 i2c_stop();
	 return;
   } 
   
   errorStatus = i2c_sendData(0x00);
   if(errorStatus == 1)
   {
     transmitString_F("RTC write-1 failed..");
	 i2c_stop();
	 return;
   } 

    errorStatus = i2c_repeatStart();
   if(errorStatus == 1)
   {
     transmitString_F("RTC repeat start failed..");
   	 i2c_stop();
	 return;
   } 
   
    errorStatus = i2c_sendAddress(DS1307_R);
   
   if(errorStatus == 1)
   {
     transmitString_F("RTC sendAddress2 failed..");
	 i2c_stop();
	 return;
   } 
 
    for(i=0;i<7;i++)
   {
      if(i == 6)  	 //no Acknowledge after receiving the last byte
	   	  data = i2c_receiveData_NACK();
	  else
	  	  data = i2c_receiveData_ACK();
		  
   	  if(data == ERROR_CODE)
   	  {
       		transmitString_F("RTC receive failed..");
			i2c_stop();
	   		return;
   	  }
	  
	  rtc_register[i] = data;
	}
	
	i2c_stop();
}	  

//******************************************************************
//Function to read 
//****************************************************************** 
void RTC_getTime(void)
{
   RTC_read();
   time[9] = 0x00;	  //NIL
   time[8] = ' ';
   time[7] = (SECONDS & 0x0f) | 0x30;		//seconds(1's)
   time[6] = ((SECONDS & 0x70) >> 4) | 0x30;		//seconds(10's)
   time[5] = ':';
   
   time[4] = (MINUTES & 0x0f) | 0x30;
   time[3] = ((MINUTES & 0x70) >> 4) | 0x30;
   time[2] = ':'; 
   
   time[1] = (HOURS & 0x0f) | 0x30;	
   time[0] = ((HOURS & 0x30) >> 4) | 0x30;
}

//******************************************************************
//Function to read 
//****************************************************************** 
void RTC_getDate(void)
{
  RTC_read();
  date[11] = 0x00;  //NIL
  date[11] = ' ';
  date[9] = (YEAR & 0x0f) | 0x30;
  date[8] = ((YEAR & 0xf0) >> 4) | 0x30;
  date[7] = '0';
  date[6] = '2';
  date[5] = '/';
  date[4] = (MONTH & 0x0f) | 0x30;
  date[3] = ((MONTH & 0x10) >> 4) | 0x30;
  date[2] = '/';
  date[1] = (DATE & 0x0f) | 0x30;
  date[0] = ((DATE & 0x30) >> 4) | 0x30;
}  
  
//******************************************************************
//Function to read 
//****************************************************************** 
void RTC_displayTime(void)
{
  RTC_getTime();
  
  LCD_DisplayString(2,1,time);
  TX_NEWLINE;
  transmitString_F("Time:"); 
  transmitString(time);     
}

//******************************************************************
//Function to read 
//****************************************************************** 
void RTC_displayDate(void)
{
  RTC_getDate();
  
  LCD_DisplayString(1,1,date);
  TX_NEWLINE;
  transmitString_F("Date:"); 
  transmitString(date);  
  RTC_displayDay();   
}

//******************************************************************
//Function to read 
//****************************************************************** 
void RTC_displayDay(void)
{
  transmitString_F("    Day: ");
  
  switch(DAY)
  {
   case 0:transmitString_F(" Sunday");
          break; 
   case 1:transmitString_F(" Monday");
          break; 
   case 2:transmitString_F(" Tuesday");
          break; 
   case 3:transmitString_F(" Wednesday");
          break; 
   case 4:transmitString_F(" Thursday");
          break; 		  
   case 5:transmitString_F(" Friday");
          break; 		  
   case 6:transmitString_F(" Saturday");
          break; 
   default:transmitString_F(" Unknown");  
  }
}	  		  
		  		     	  
//******************************************************************
//Function to read 
//****************************************************************** 
void RTC_updateRegisters(void)
{
  SECONDS = ((time[6] & 0x07) << 4) | (time[7] & 0x0f);
  MINUTES = ((time[3] & 0x07) << 4) | (time[4] & 0x0f);
  HOURS = ((time[0] & 0x03) << 4) | (time[1] & 0x0f);  
  DAY = date[10];
  DATE = ((date[0] & 0x03) << 4) | (date[1] & 0x0f);
  MONTH = ((date[3] & 0x01) << 4) | (date[4] & 0x0f);
  YEAR = ((date[8] & 0x0f) << 4) | (date[9] & 0x0f);
}  


//******************************************************************
//Function to read 
//******************************************************************   
unsigned char RTC_writeTime(void)
{
  unsigned char errorStatus, i;
  
   errorStatus = i2c_start();
   if(errorStatus == 1)
   {
     transmitString_F("RTC start1 failed..");
   	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendAddress(DS1307_W);
   
   if(errorStatus == 1)
   {
     transmitString_F("RTC sendAddress1 failed..");
	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendData(0x00);
   if(errorStatus == 1)
   {
     transmitString_F("RTC write-1 failed..");
	 i2c_stop();
	 return(1);
   } 

    for(i=0;i<3;i++)
    {
	  errorStatus = i2c_sendData(rtc_register[i]);  
   	  if(errorStatus == 1)
   	  {
       		transmitString_F("RTC write time failed..");
			i2c_stop();
	   		return(1);
   	  }
    }
	
	i2c_stop();
	return(0);
}


//******************************************************************
//Function to read 
//******************************************************************   
unsigned char RTC_writeDate(void)
{
  unsigned char errorStatus, i;
  
   errorStatus = i2c_start();
   if(errorStatus == 1)
   {
     transmitString_F("RTC start1 failed..");
   	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendAddress(DS1307_W);
   
   if(errorStatus == 1)
   {
     transmitString_F("RTC sendAddress1 failed..");
	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendData(0x03);
   if(errorStatus == 1)
   {
     transmitString_F("RTC write-1 failed..");
	 i2c_stop();
	 return(1);
   } 

    for(i=3;i<7;i++)
    {
	  errorStatus = i2c_sendData(rtc_register[i]);  
   	  if(errorStatus == 1)
   	  {
       		transmitString_F("RTC write date failed..");
			i2c_stop();
	   		return(1);
   	  }
    }
	
	i2c_stop();
	return(0);
}
  
//******************************************************************
//Function to read 
//******************************************************************   
void RTC_updateTime(void)
{
  unsigned char i, data;
  TX_NEWLINE;
  transmitString_F("Enter Time in 24h format(hh:mm:ss):"); 
  
    data = receiveByte(); 	   	  	  				//receive hours
	transmitByte(data);
	if(data < 0x30 || data > 0x32)
	   goto TIME_ERROR;
	   
	time[0]= data;
	 
	data = receiveByte();
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	   goto TIME_ERROR;
	 
	time[1]= data;
	
	transmitByte(':');
	
	if(((time[1] & 0x0f) + ((time[0] & 0x03)*10)) > 23)
	   goto TIME_ERROR;
	 
	 data = receiveByte();			   			  //receive minutes
	 transmitByte(data);
	if(data < 0x30 || data > 0x35)
	   goto TIME_ERROR;
	   
	time[3]= data; 
	
	data = receiveByte();
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	   goto TIME_ERROR;
	   
	time[4]= data; 
	
	transmitByte(':');
	
	 data = receiveByte();			   			  //receive seconds
	 transmitByte(data);
	if(data < 0x30 || data > 0x35)
	   goto TIME_ERROR;
	   
	time[6]= data; 
	
	data = receiveByte();
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	   goto TIME_ERROR;
	   
	time[7]= data; 
	
	 	  
  RTC_updateRegisters(); 
  data = RTC_writeTime();
  
  if(data == 0)
  {
     TX_NEWLINE;
  	 transmitString_F("Time Updated sucessfully.."); 
  }	  
  return;
  
TIME_ERROR:

  TX_NEWLINE;
  transmitString_F("Invalid Entry.."); 
  return;
}  
  
  
//******************************************************************
//Function to read 
//******************************************************************   
void RTC_updateDate(void)
{
  unsigned char i, data;
  TX_NEWLINE;
  transmitString_F("Enter Date (dd/mm/yy):"); 
  
    data = receiveByte(); 	   				  		//receive date
	transmitByte(data); 	   	  	  				
	if(data < 0x30 || data > 0x33)
	   goto DATE_ERROR;
	   
	date[0]= data;
	 
	data = receiveByte();
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	   goto DATE_ERROR;
	   
	date[1]= data;
	
	if(((date[8] & 0x0f) + ((date[9] & 0x03)*10)) > 31)
	   goto DATE_ERROR;
	
	transmitByte('/');
	date[2] = '/';
	   
	
	 data = receiveByte();			   			  //receive month
	 transmitByte(data);
	if(data < 0x30 || data > 0x31)
	  goto DATE_ERROR;
	  
	date[3]= data; 
	
	data = receiveByte();
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	  goto DATE_ERROR;
	  
	date[4] = data; 
	
	if(((date[4] & 0x0f) + ((date[3] & 0x03)*10)) > 12)
	   goto DATE_ERROR;
	   
	transmitByte('/');
	date[5] = '/';	  		  		 //receive special char (/,-, etc.)
			
	date[6] = '2'; 	   	   	  	  	 //year is 20xx
	date[7] = '0';
	
	data = receiveByte();			 //receive seconds
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	   goto DATE_ERROR;
	   
	date[8]= data; 
	
	data = receiveByte();
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	   goto DATE_ERROR;
	   
	date[9]= data; 
	
	TX_NEWLINE;
    transmitString_F("Enter Day (Sunday:0, Monday:1...) (0-6):"); 
	
	data = receiveByte();				   //receive Day of the week
	transmitByte(data);
	if(data < 0x30 || data > 0x36)
	   goto DATE_ERROR;
	   
	date[10] = data & 0x0f;
		 	  
  RTC_updateRegisters(); 
  data = RTC_writeDate();
  
  if(data == 0)
  {
     TX_NEWLINE;
  	 transmitString_F("Date Updated sucessfully.."); 
  }	  
  return;
  
DATE_ERROR:

  TX_NEWLINE;
  transmitString_F("Invalid Entry.."); 
  return;
}  
  
  
	
	  
 
 

 
