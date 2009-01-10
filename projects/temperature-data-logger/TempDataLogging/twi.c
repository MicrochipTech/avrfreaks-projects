//**********************************************************************//
//FILE: twi.c 
//AUTHOR: Adam Kadolph
//DATE:	12-5-08
//DESCRIPTION: This file handles the Two Wired Serial Interface to the AVR
//**********************************************************************//

#include "twi.h"
#include <avr/io.h>

void I2Cinit(void)
{
	TWBR = (F_CPU / 100000UL - 16) / 2;	//set bit rate for TWI, SCL f=100kHz, fclk=8Mhz
	TWSR = 0x00;				//no prescaler value
}

unsigned char I2C_start(void)
{
	//TWI transmit start
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);//send start condition
	
	while(!(TWCR & (1<<TWINT)));		//wait for TWINT flag set to indicate start condition has been transmitted
	
	if ((TWSR & STATUS_MASK) == START)	//check TWI status register
		return(0);
	else
		return(1);
}

unsigned char I2C_repeatStart(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); 	//Send START condition
    while (!(TWCR & (1<<TWINT)));   			//Wait for TWINT flag set. This indicates that the
		  										//START condition has been transmitted
    if ((TWSR & STATUS_MASK) == RESTART)	//Check value of TWI Status Register
 		return(0);
	else
		return(1);
}
unsigned char I2C_sendAddr(unsigned char addr)
{
	//TWI transmit address in Master Transmit Mode
	unsigned char STATUS;
   
	if((addr & 0x01) == 0) 
		STATUS = MT_SLA_ACK;
	else
		STATUS = MR_SLA_ACK; 
	
	TWDR = addr;					//load SLA+W into TWDR register
	TWCR = (1<<TWINT)|(1<<TWEN);

	while(!(TWCR & (1<<TWINT)));	//wait for TWINT flag set to indicate that SLA_W has been transmitted 
	
	if ((TWSR & STATUS_MASK) == STATUS)//check value of TWI status that ACK bit has been received
		return(0);
	else 
		return(1);
}
unsigned char I2C_sendData(unsigned char data)
{
	//TWI transmit data in Master Transmit Mode
	TWDR = data;					//load data into TWDR register
	TWCR = (1<<TWINT)|(1<<TWEN);

	while(!(TWCR & (1<<TWINT)));		//wait for TWINT flag set to indicate data has been transmitted
		
	if ((TWSR & STATUS_MASK) != MT_DATA_ACK)//check value of TWI status register
		return(1);
	else
		return(0);
}

unsigned char I2C_receiveData_ACK(void)
{
  unsigned char data;
  
  TWCR = (1<<TWEA)|(1<<TWINT)|(1<<TWEN);
  
  while (!(TWCR & (1<<TWINT)));	   	   //Wait for TWINT flag set. This indicates that the
   		 		   					   //data has been received
  if ((TWSR & STATUS_MASK) != MR_DATA_ACK)    //Check value of TWI Status Register
   	  return(ERROR_CODE);
  
  data = TWDR;
  return(data);
}

unsigned char I2C_receiveData_NACK(void)
{
  unsigned char data;
  
  TWCR = (1<<TWINT)|(1<<TWEN);
  
  while (!(TWCR & (1<<TWINT)));	   	   //Wait for TWINT flag set. This indicates that the
   		 		   					   //data has been received
  if ((TWSR & STATUS_MASK) != MR_DATA_NACK)    //Check value of TWI Status Register
   	  return(ERROR_CODE);
  
  data = TWDR;
  return(data);
}

void I2C_stop(void)
{
	//TWI transmit stop
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);	//transmit stop condition
}
