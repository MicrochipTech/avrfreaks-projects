#include <util/twi.h> 
#include "i2c_com.h"
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#define SLA_W 0xD0
#define SLA_R 0xD1
#ifndef true 
	enum BOOLEAN {false,true} error;
#endif

unsigned char testers232;

// start condiction

void start_TWI_RTC(void)
{
	error=false;
	TWCR = ((1<<TWINT)|(1<<TWSTA)|(1<<TWEN))&(~(1<<TWSTO));
	while (!(TWCR & (1<<TWINT))){;}

	if ((TWSR & 0xF8) != TW_START)
		error=true;
	
}


// stop condition

void stop_TWI_RTC(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

// repeted start
void rep_start_TWI_RTC(void)
{
	if(error==false)
	{
		TWCR = ((1<<TWINT)|(1<<TWSTA)|(1<<TWEN))&(~(1<<TWSTO));
		while (!(TWCR & (1<<TWINT))){;}

		if ((TWSR & 0xF8) != TW_REP_START )
			error=true;
	}
	else 
		error=true;
}




// send RTC adress to i2c
void send_addr_TWI_RTC(unsigned char RTC_ADDR)
{
	if(error==false)
	{
			TWDR = RTC_ADDR;
			TWCR = (1<<TWINT) | (1<<TWEN);
			while (!(TWCR & (1<<TWINT))){;}
			if (((TWSR & 0xF8) != TW_MT_SLA_ACK)&&((TWSR & 0xF8) != TW_MR_SLA_ACK))
			error=true;
	}
			
}

// send pointer of RTC memory
void send_ptr_value_TWI_RTC(unsigned char ptr_value)
{
	if(error==false)
	{
		TWDR = ptr_value;
		TWCR = (1<<TWINT) | (1<<TWEN);
		while (!(TWCR & (1<<TWINT))){;}
		if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
		error=true;
	}
	else
	 error =true;
}

// send data to RTC
void send_DATA_TWI_RTC(unsigned char rtc_data)
{
	if(error==false)
	{
		TWDR = rtc_data;
		TWCR = (1<<TWINT) | (1<<TWEN);
		while (!(TWCR & (1<<TWINT))){;}
		if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
			error=true;
	}
	else
		error=true;
}

// read with ack

unsigned char readACK_DATA_TWI_RTC(void)
{
	unsigned char recv_rtc_data;
	//if(error==false)
	//{
		recv_rtc_data=TWDR;
		TWCR = (1<<TWINT) | (1<<TWEA)|(1<<TWEN);
		while (!(TWCR & (1<<TWINT))){;}
		if ((TWSR & 0xF8) !=TW_MR_DATA_ACK)
			error=true;
		else
			return recv_rtc_data;
		//}
		 //error=true;
		 //return 0;
		 return;
}

// read with not ack
unsigned char readNACK_DATA_TWI_RTC(void)
{
	unsigned char recv_rtc_data;
	if(error==false)
	{
		recv_rtc_data=TWDR;
		TWCR = (1<<TWINT)|(1<<TWEN);
		while (!(TWCR & (1<<TWINT))){;}
		if ((TWSR & 0xF8) !=TW_MR_DATA_NACK )
			error=true;
		else
			return recv_rtc_data;
	}
	
		error=true;	
		return 0;
}

void set_clockRTC(unsigned int second,unsigned int minut,unsigned int hour){
	start_TWI_RTC();
	send_addr_TWI_RTC(SLA_W);
	send_ptr_value_TWI_RTC(0);
	send_DATA_TWI_RTC((unsigned char)(second));
	send_DATA_TWI_RTC((unsigned char)(minut));
	send_DATA_TWI_RTC((unsigned char)(hour));
	stop_TWI_RTC();
	//serial_send_message("Clock set");
	
}


void get_clock(timep_t *relogio)
{
	unsigned char campo;
	start_TWI_RTC();
	send_addr_TWI_RTC(SLA_W);
	send_ptr_value_TWI_RTC(0);
	
	rep_start_TWI_RTC();	
	send_addr_TWI_RTC(SLA_R);
	sec=readACK_DATA_TWI_RTC();
	campo=readACK_DATA_TWI_RTC();
	relogio->sec= (campo>>4)*10 +(campo & 0x0F);
		
	campo=readACK_DATA_TWI_RTC();
	relogio->min= (campo>>4)*10 +(campo & 0x0F);
		
	campo=readACK_DATA_TWI_RTC();
	relogio->hour= (campo>>4)*10 +(campo & 0x0F);
	
	readNACK_DATA_TWI_RTC();	
	stop_TWI_RTC();
	
	
}
