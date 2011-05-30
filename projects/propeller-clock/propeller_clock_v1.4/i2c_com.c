#include <util/twi.h>
#include "i2c_com.h"
#include "rs232.h"
#define SLA_W 0xD0
#define SLA_R 0xD1
#define F_CPUI2C 24
enum BOOLEAN {false,true} error;

void i2c_configure(void)
{

//TWSR&=~(1<<TWPS1)|(1<<TWPS2);
// 
//DDRD |=~((1<<PD0) ||(1<<PD1));
//PORTD |=(1<<PD0) ||(1<<PD1);
TWBR=F_CPUI2C*1.25-2;

}

// start condiction

void start_TWI_RTC(void)
{
	error=false;
	TWCR = ((1<<TWINT)|(1<<TWSTA)|(1<<TWEN))&(~(1<<TWSTO));
	while (!(TWCR & (1<<TWINT))){;
	}

	if ((TWSR & 0xF8) != TW_START){
		error=true;
		serial_send_message((unsigned char*)"Error starting...\r\n");
	}		
	
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

		if ((TWSR & 0xF8) != TW_REP_START ){
			error=true;
			serial_send_message((unsigned char*)"Error repeating start...\r\n");
		}			
	}
		
}




// send RTC adress to i2c
void send_addr_TWI_RTC(unsigned char RTC_ADDR)
{
	if(error==false)
	{
			TWDR = RTC_ADDR;
			TWCR = (1<<TWINT) | (1<<TWEN);
			while (!(TWCR & (1<<TWINT))){;}
			if (((TWSR & 0xF8) != TW_MT_SLA_ACK)&&((TWSR & 0xF8) != TW_MR_SLA_ACK)){
				error=true;
				serial_send_message((unsigned char*)"Error send addr...\r\n");
			}			
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
		if ((TWSR & 0xF8) != TW_MT_DATA_ACK){
			error=true;
			serial_send_message((unsigned char*)"Error send ptr...\r\n");
		}		
	}
}	

// send data to RTC
void send_DATA_TWI_RTC(unsigned char rtc_data)
{
	if(error==false)
	{
		TWDR = rtc_data;
		TWCR = (1<<TWINT) | (1<<TWEN);
		while (!(TWCR & (1<<TWINT))){;}
		if ((TWSR & 0xF8) != TW_MT_DATA_ACK){
			error=true;
			serial_send_message("Error send data...\r\n");
		}		
	}
}

unsigned char readACK_DATA_TWI_RTC(void)
{
	unsigned char recv_rtc_data;
	if(error==false)
	{
		
		TWCR = (1<<TWINT) | (1<<TWEA)|(1<<TWEN);
		while (!(TWCR & (1<<TWINT))){;}
		if ((TWSR & 0xF8) !=TW_MR_DATA_ACK){
			error=true;
			serial_send_message("Error read ack...\r\n");
		}			
		else
		{
			recv_rtc_data=TWDR;
			return recv_rtc_data;
		}
	}
}

/// read with not ack
unsigned char readNACK_DATA_TWI_RTC(void)
{
	unsigned char recv_rtc_data;
	if(error==false)
	{
		
		TWCR = (1<<TWINT)|(1<<TWEN);
		while (!(TWCR & (1<<TWINT))){;}
		if ((TWSR & 0xF8) !=TW_MR_DATA_NACK ){
			error=true;
			serial_send_message("Error read nack...\r\n");
		}			
		else
		{
			recv_rtc_data=TWDR;
			return recv_rtc_data;
		}
	}

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
	campo=readACK_DATA_TWI_RTC();
	relogio->sec= (campo>>4)*10 +(campo & 0x0F);
		
	campo=readACK_DATA_TWI_RTC();
	relogio->min= (campo>>4)*10 +(campo & 0x0F);
		
	campo=readACK_DATA_TWI_RTC();
	relogio->hour= (campo>>4)*10 +(campo & 0x0F);
	
	readNACK_DATA_TWI_RTC();	
	stop_TWI_RTC();
	
	
}


unsigned char *get_clock1(){
	unsigned char *clock;
	int i,j;
	unsigned char dig1,dig2;
	clock=(unsigned char*)malloc(10*sizeof(unsigned char));
	memset(clock,0,10);
	start_TWI_RTC();
	send_addr_TWI_RTC(SLA_W);
	send_ptr_value_TWI_RTC(0);
	rep_start_TWI_RTC();	
	send_addr_TWI_RTC(SLA_R);
	
	for (i=0;i<9;i+=3)
	{
		
		dig1=readACK_DATA_TWI_RTC();		
		dig2=dig1;
		dig1 =(dig1 & 0x0F)|(0x30);
		dig2 = (((dig2>>4) | 0x30));
		clock[6-i]=dig2;
		clock[6-i+1]=dig1;
		clock[6-i+2]=':';
		
	}
	clock[8]=0x0D;
	readNACK_DATA_TWI_RTC();
	stop_TWI_RTC();
	return clock;
	
}


void  gget_clock(unsigned char clk_digits[8]){
	int i;
	unsigned char dig1,dig2;
	start_TWI_RTC();
	send_addr_TWI_RTC(SLA_W);
	send_ptr_value_TWI_RTC(0);
	rep_start_TWI_RTC();	
	send_addr_TWI_RTC(SLA_R);
	//sec=readACK_DATA_TWI_RTC();
	
	for (i=0;i<=6;i+=3)
	{
		
		if(i<6)
		{
			
			//clk_digits[i]=readACK_DATA_TWI_RTC();
			dig1=readACK_DATA_TWI_RTC();		
			dig2=dig1;
			dig1 =(dig1 & 0x0F)|(0x30);
			dig2 =((dig2>>4) | 0x30);
			clk_digits[7-i]=dig1;
			clk_digits[7-i-1]=dig2;
			clk_digits[7-i-2]=':';
		}
		else
		{
			//clk_digits[i]=readACK_DATA_TWI_RTC();
			dig1=readNACK_DATA_TWI_RTC();		
			dig2=dig1;
			dig1=(dig1 & 0x0F)|(0x30);
			dig2=((dig2>>4) | 0x30);
			clk_digits[7-i]=dig1;
			clk_digits[7-1-i]=dig2;
			stop_TWI_RTC();
		}
	}
			
}
void  gget_date(unsigned char *date_digits){
	int i;
	unsigned char dig1,dig2;
	start_TWI_RTC();
	send_addr_TWI_RTC(SLA_W);
	send_ptr_value_TWI_RTC(4);
	rep_start_TWI_RTC();	
	send_addr_TWI_RTC(SLA_R);
	for (i=0;i<=6;i+=3)
	{		
		if(i<6)	{
			dig1=readACK_DATA_TWI_RTC();		
			dig2=dig1;
			dig1 =(dig1 & 0x0F)|(0x30);
			dig2 = (((dig2>>4) | 0x30));
			date_digits[i]=dig1;
			date_digits[i+1]=dig2;
			date_digits[i+2]='-';
		}
		else{
			dig1=readNACK_DATA_TWI_RTC();		
			dig2=dig1;
			dig1 =(dig1 & 0x0F)|(0x30);
			dig2 = (((dig2>>4) | 0x30));
			date_digits[i]=dig1;
			date_digits[i+1]=dig2;
			stop_TWI_RTC();
		}
	}
			
}




