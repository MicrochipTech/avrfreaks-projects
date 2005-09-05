/******************************************************************************************************
REV 1.07

Copyright (C) 2005  Edward Welch welcher@edjusted.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


This program is written for the ATmega32

This is a multi-purpose software program to interface several devices to a ATmega8535 uController
Interfacing is accomplished using TWI, UART, and Parallel communication busses.

Supported Devices:
DS1624 Temp Sensor
DS1307 RTC
HD44780 compatible LCD
Siteplayer Integrated Webserver

Revision History:

1.00 Inital Version
1.01 Added LCD Support
1.02 Added Siteplayer Support
1.03 Added DS1624 Support
1.04 Added DS1307 Support
1.05 Interupt based TWI implemented
1.06 Interupt Driven 4x3 keypad implemented
1.07 Thermostat Controls Implemented

LCD Controller Chip HD44780 compatable
16x2 Character LCD


******************************************************************************************************/
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <inttypes.h>
#include <avr/twi.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <avr/wdt.h>


/*************
 Definitions
*************/

//LCD
#define RS 2
#define RW 3
#define E 4
#define DATA PORTA
#define COMM PORTC

//TWI Addresses
#define TEMPADDR 0x49	
#define TEMPADDR_O 0x4B
#define RTCADDR 0x68	

//Keypad Pins (not currently used)
#define R1 0
#define R2 1
#define R3 2
#define R4 3
#define C1 4
#define C2 5
#define C3 6
#define KEYPAD PORTB
#define KEYDIR DDRB

//Thermostat Control Pin
#define THERMPORT PORTD
#define THERMPIN PD7



/**************************
Declare Function Prototypes
**************************/
void main(void);
void lcd_display(uint8_t);
void init(void);
void func_set(void);
void disp(void);
void clear(void);
void entry(void);
void home(void);
void pause_half_ms(uint16_t);
void output(uint8_t,uint8_t);
void twi_readtemp(void);
void start_conversion(void);
void line2(void);
void print3(uint8_t);
void print2(uint8_t);
void update_time(void);
void twi_write(uint8_t,uint8_t);
void twi_read(uint8_t,uint8_t);
void twi_initRTC(void);
void twi_gettime(void);
void nop(void);
void sendchar(char *);
void sendstring(char *,uint8_t);
void keypad(void);
void check_code(void);
void update_siteplayer(void);
void read_alarm(void);
void thermostat(void);
void load_characters(void);
void udp_send(uint8_t);
void get_thermostat_vals(void);
void input(uint8_t,uint8_t);
void update_thermostat_vals(void);
void update_thermostat_times(void);
void check_hold(void);
void twi_readtemp1(void);
uint8_t format_vals(uint8_t);
void update_thermostat_temps(void);
void move(uint8_t,uint8_t);
void cgaddress(uint8_t);

/***************
Global Variables
***************/
uint8_t data,data1,slave_address,data_direction,bytes,twi_data[15],data_pointer,retry,status,trip,hours,minutes,temp_flag,alarm_status,txt_status,door_status,status_old;
uint8_t thermo_temps[7],thermo_times[12],current_temp,update_flag,temp_up_flag,o_temp,o_temp1,below_zero,heat_on,time_period,hold;



//RTC Pulse 1Hz
SIGNAL(SIG_INTERRUPT0)
{
	trip = 1;
}

//Used for Keypad
SIGNAL(SIG_INTERRUPT1)
{
	trip = 3;
	//keypad();
}

SIGNAL(SIG_2WIRE_SERIAL)
{
	switch (TW_STATUS)
	{
		case TW_START:										//Start or Repeated Start
		case TW_REP_START:									
		{
			if(retry > 2) 									//If after 3 tries SLA+R/W NACK recieved abort
			{			
				TWCR |= (1<<TWINT)|(1<<TWSTO);				//Generate stop condition
				status = 0;									//Set TWI free
				home();
				pause_half_ms(5);
				sendchar("A");								//Send error condition
			    //pause_half_ms(5000);
				return;									
				//break;
			}
			TWDR = slave_address | data_direction;			//Load SLA+R/W
			TWCR &= ~(1<<TWSTA);							//Clear Start bit (Also clears TWINT which sends SLA+R/W)
			break;
		}
		
		case TW_MT_SLA_NACK:								//SLA+R/W NACK received, retry 3 times
		case TW_MR_SLA_NACK:
		{
			retry++;
			TWCR |= (1<<TWINT)|(1<<TWSTA)|(1<<TWSTO);
			//sendchar("B");
			pause_half_ms(200);
			break;
		}
		
		case TW_MT_SLA_ACK:									//SLA+W ACK received
		{
			retry = 0;
			TWDR = twi_data[data_pointer];					//Load first data byte
			data_pointer++;									//Increment pointer
			TWCR |= (1<<TWINT);							//Send
			//sendchar("C");
			break;
		}
		
		case TW_MT_DATA_ACK:								//ACK received after data byte sent
		{
			retry = 0;
			if (--bytes > 0)								//Subtract 1 from total bytes pointer
			{
				TWDR = twi_data[data_pointer];				//If 1 byte or more remains load byte
				data_pointer++;								//Increment pointer
				TWCR |= (1<<TWINT);						//Send byte
				//sendchar("D");
			}
			else  
			{
				status = 0;									//If no bytes remain to be sent clear TWI busy flag
				TWCR |= (1<<TWINT)|(1<<TWSTO);				//Send STOP
				//sendchar("E");
			}
			break;
		}
		
		case TW_MT_DATA_NACK:								//NACK received after data byte sent
		{
			TWCR |= (1<<TWINT)|(1<<TWSTO);					//Send STOP
			status  = 0;									//Clear TWI busy flag
			//sendchar("F");
			break;
		}
		
		case TW_MR_SLA_ACK:									//ACK received after data byte received
		{
			//twi_data[data_pointer] = TWDR;
			//data_pointer++;
			if (--bytes > 0)								//Subtract 1 from total bytes pointer
			//if (bytes > 1)
			{
				TWCR |= (1<<TWINT)|(1<<TWEA);				//If more than 1 byte is to be received ACK will be returned
				//sendchar("1");
			}
			else 
			{
				TWCR |= (1<<TWINT);						//Else NACK will be returned
				//sendchar("2");
			}
			break;
		}
		
		case TW_MR_DATA_ACK:								//ACK received after data byte
		{
			twi_data[data_pointer] = TWDR;					//Load data from TWDR
			data_pointer++;									//Increment pointer
			if (--bytes > 0)								//Subtract 1 from total bytes pointer
			{
				TWCR |= (1<<TWINT)|(1<<TWEA);				//If more than 1 byte is still to be received ACK will be returned
				//sendchar("3");
			}
			else
			{
				TWCR &= ~(1<<TWEA);						//Else NACK will be returned
				//sendchar("4");
			}
			break;
		}
		
		case TW_MR_DATA_NACK:								//NACK received after data byte
		{
			twi_data[data_pointer] = TWDR;					//Load data from TWDR
			TWCR |= (1<<TWINT)|(1<<TWSTO);					//Send STOP
			status = 0;										//Clear TWI busy flag
			//sendchar("5");
			break;
		}
		case TW_MR_ARB_LOST:
		{
			TWCR |= (1<<TWINT);
			sendchar("X");
			pause_half_ms(1000);
			status = 0;
			break;
		}
		
		
		case TW_NO_INFO:
		{
			TWCR |= (1<<TWINT)|(1<<TWSTO);					//Send STOP
			status = 0;
			sendchar("I");
			pause_half_ms(1000);
			break;
		}
		case TW_BUS_ERROR:
		{
			TWCR |= (1<<TWINT)|(1<<TWSTO);					//Send STOP
			status = 0;
			sendchar("E");
			wdt_reset();
			cli();
			for(;;);
			//pause_half_ms(5000);
			break;
		}
		

	}
}


//UART Interupt (Not Utilized currently, more for Debug)
SIGNAL(SIG_UART_RECV)
{
uint8_t input;
input = UDR;

//clear LCD if input = 2
if (input==33)
{
//trip = 5;
//update_time();
}
else if (input==35)
{
//trip = 3;
//update_thermostat_temps();
}
else if (input==3)
{
//update_thermostat_times();
//trip = 4;
}
else
//line2();
//clear();
lcd_display(input);


}

SIGNAL(__vector_default)
{
    // catch all for interupts
}


void main (void)
{
	OSCCAL = 0xBF;						//Calabration Byte, Change if necessary for your uC.
	//Pause Function Timer Setup
	TCCR0 = 0x02;   					// prescale timer0 ck/8 
	TCNT0 = 0x00;   					// start value of T/C0 
	
	/*********
	TWI Setup
	*********/
	//TWSR = 0x00;
	//TWBR = 40;
	//status = 0;
	//TWCR = (1<<TWEN);
	
	//TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWSTO);	
	//pause_half_ms(100);
	
	//PORT Setup
	DDRA = 0xFF;					// PORTA OUTPUT
	DDRC = 0x9F;					// 10011111 PORTC Output
	PORTC = 0xE0;					// 11100000
	PORTC = 0xE1;					// 11100001
	PORTC = 0xE3;					// 11100011
	pause_half_ms(5);
	//pause_half_ms(1);
	//PORTC = 0xE1;
	//pause_half_ms(1);
	//PORTC = 0xE3;
	//DDRC = 0x9C;
	DDRD = 0x80;
	//PORTD = 0x00;
	
	//External Interupt Setup
	//MCUCR = (1<<ISC00)|(1<<ISC01)|(0<<ISC10)|(1<<ISC11);
	MCUCR = (1<<ISC00)|(1<<ISC01);
	//GICR = (1<<INT0)|(1<<INT1);
	GICR = (1<<INT0);
	
	//Initialize UART
	UCSRB = (1<<RXCIE)|(1<<TXEN)|(1<<RXEN);	// enable RX interrupt and TX
	UCSRC = (1<<URSEL)|(3<<UCSZ0);				// 8bit frame
	UBRRL = 12;									// init transfer speed for 8 MHz 38.4kbs
	
	//Enable Watch Dog Timer (To correct any coding bugs)
	wdt_enable(WDTO_2S);
	
	
	
	
	sei();
	//Initialize LCD
	init();
	load_characters();
	home();
	pause_half_ms(5);
	//print3(1);
	//pause_half_ms(1500);
	/*********
	TWI Setup
	*********/
	TWSR = 0x00;
	TWBR = 40;
	status = 0;
	TWCR = (1<<TWEN);
	
	TWCR = (1<<TWINT)|(1<<TWSTO);	
	pause_half_ms(100);
	
	wdt_reset();
	
	//home();
	//pause_half_ms(5);
	//print3(12);
	//pause_half_ms(1000);
	
	start_conversion();
	//home();
	//pause_half_ms(5);
	//print3(13);
	//pause_half_ms(1000);
	
	twi_initRTC();
	//home();
	//pause_half_ms(5);
	//print3(14);
	//pause_half_ms(1500);
	pause_half_ms(10);
	
	//Init Keypad
	//print3(2);
	
	//KEYDIR = (1<<C1)|(1<<C2)|(1<<C3);
	//KEYPAD = (1<<R1)|(1<<R2)|(1<<R3)|(1<<R4);
	
	uint8_t j,read_flag;
	
	/*******************
	Initialize Variables
	*******************/
	status_old = 0;
	alarm_status = 1;
	txt_status = 1;
	temp_up_flag = 1;
	
	/*****************
	Init Alarm and SP
	*****************/
	twi_data[1] = alarm_status;
	twi_data[2] = txt_status;
	twi_data[3] = 0;
	output(0x1C,3);
	
	
	
	//Siteplayer Enable UDP
		uint8_t del;
		
		outp(0x90,UDR);					//send writex command to siteplayer
		do								//wait for UDR to clear
			del = inp(UCSRA) & 0x20;
		while (del != 0x20);
		
		outp(0x20,UDR);					//send address to siteplayer LB
		do								//wait for UDR to clear
			del = inp(UCSRA) & 0x20;
		while (del != 0x20);
	
		outp(0xFF,UDR);					//send address to siteplayer HB
		do								//wait for UDR to clear
			del = inp(UCSRA) & 0x20;
		while (del != 0x20);
		
		outp(0x01,UDR);					//Set UDPRCVR
		do								//wait for UDR to clear
			del = inp(UCSRA) & 0x20;
		while (del != 0x20);
		
		udp_send(154);
		
			
	//Setup Thermostat on Powerup
	get_thermostat_vals();
	for (j=1;j<=6;j++)
	{
		twi_data[j] = thermo_temps[j];
	}
	output(0x0A,6);
	for (j=1;j<=12;j++)
	{
		twi_data[j] = thermo_times[j];
	}
	output(0x10,12);
	twi_data[1] = hold;
	output(0x26,1);
	
	

		clear();
		

	for(;;)
	{
		
		wdt_reset();
		
		check_code();		//Check keypad uC to see if a code has been entered
		pause_half_ms(5);
		read_alarm();		//Check SP for alarm settings
		
		
		if (status_old != door_status)
		{
						
			//Left Garage Door
			if ((status_old & 0x01) != (door_status & 0x01))
			{
				if ((door_status & 0x01) == 0x01)
				{
					if(txt_status ==1) udp_send(10);	//Left Garage Open
					pause_half_ms(10);
					udp_send(100);
				}
				else
				{
					if(txt_status ==1) udp_send(11);  //Left Garage Closed
					pause_half_ms(10);
					udp_send(101);
				}
			}
			//Right Garage Door
			if ((status_old & 0x02) != (door_status & 0x02))
			{
				if ((door_status & 0x02) == 0x02)
				{
					if(txt_status ==1) udp_send(12);  //Right Garage Open
					pause_half_ms(10);
					udp_send(102);
				}
				else
				{
					if(txt_status ==1) udp_send(13); //Right Garage Closed
					pause_half_ms(10);
					udp_send(103);
				}
			}
			//Front Door
			if ((status_old & 0x04) != (door_status & 0x04))
			{
				if ((door_status & 0x04) == 0x04)
				{
					if(txt_status ==1) udp_send(14);  //Front Door Open
					pause_half_ms(10);
					udp_send(104);
				}
				else
				{
					if(txt_status ==1) udp_send(15);  //Front Door Closed
					pause_half_ms(10);
					udp_send(105);
				}
			}
			//Garage Entry Door
			if ((status_old & 0x08) != (door_status & 0x08))
			{
				if ((door_status & 0x08) == 0x08)
				{
					if(txt_status ==1) udp_send(16);	//Garage Entry Door Open
					pause_half_ms(10);
					udp_send(106);
				}
				else
				{
					if(txt_status ==1) udp_send(17);	//Garage Entry Door Closed
					pause_half_ms(10);
					udp_send(107);
				}
			}
			
			
			status_old = door_status;
		}
		
	
		
		
		if (trip == 1)
		{
			input(0x1F,1);
			read_flag = twi_data[1];
			//Read_flag contains information from the SP pertaining to data entry on the webpage
			switch (read_flag)
			{
				case 0x01:
				{
					update_time();	//Update RTC Time Values with new values from web.
					break;
				}
				case 0x02:
				{
					update_thermostat_temps(); //Update Thermostat temp values 
					break;
				}
				case 0x04:
				{
					update_thermostat_times();	//Update Thermostat Time values
					break;
				}
				case 0x08:
				{
					check_hold();	//Check to see if hold is set on SP.
					break;
				}
			}
			//Reset read_flag to 0
			read_flag = 0;
			twi_data[1] = read_flag;
			output(0x1F,1);
			
				
			//Get Indoor Temp
			twi_readtemp();
			
			//Get Current Time, Function also resolves current time_period
			twi_gettime();
			
			//Thermostat Function
			thermostat();
			
			//Get Outdoor Temp
			twi_readtemp1();
			
			//Reset RTC pulse flag
			trip = 0;
			
			pause_half_ms(10);
			
			//Send Temp and Alarm values to siteplayer
			update_siteplayer();
			
			
			
		}
		
		
		

	}
}


/**************
Alarm Functions
**************/
void check_code(void)
{
	uint8_t update;
	update = 0;
	
	/*******************************************************
	NOTE: All codes changed to 00 to protect my "real" codes
	*******************************************************/
	
	
	twi_read(0x03,3);
	pause_half_ms(10);
	door_status = twi_data[3];
	//Ed Enter
	if ((twi_data[1] == 0) && (twi_data[2] == 0))
	{
		twi_data[1] = 3;
		twi_write(0x03,1);
		if(txt_status ==1) udp_send(30);
		pause_half_ms(10);
		udp_send(130);
	}
	//Disable Text
	else if ((twi_data[1] == 0) && (twi_data[2] == 0))
	{
		twi_data[1] = 3;
		twi_write(0x03,1);
		txt_status = 0;
		udp_send(151);
		update = 1;
		//pause_half_ms(5);
		//twi_data[1] = 0;
		//output(0x1D,1);
		//twi_data[1] = 2;
		//twi_write(0x03,1);
	}
	//Enable Text
	else if ((twi_data[1] == 0) && (twi_data[2] == 0))
	{
		twi_data[1] = 3;
		twi_write(0x03,1);
		txt_status = 1;
		udp_send(150);
		update = 1;
		//pause_half_ms(5);
		//twi_data[1] = 1;
		//output(0x1D,1);
		//twi_data[1] = 3;
		//twi_write(0x03,1);
	}
	//Page Enter
	else if ((twi_data[1] == 0) && (twi_data[2] == 0))
	{
		twi_data[1] = 3;
		twi_write(0x03,1);
		if(txt_status ==1) udp_send(31);
		pause_half_ms(10);
		udp_send(131);
	}
	//Landlord Enter
	else if ((twi_data[1] == 0) && (twi_data[2] == 0))
	{
		twi_data[1] = 3;
		twi_write(0x03,1);
		if(txt_status == 1) udp_send(32);
		pause_half_ms(10);
		udp_send(132);
	}
	//Code Entry Error
	else if ((twi_data[1] == 0) && (twi_data[2] == 0))
	{
		twi_data[1] = 2;
		twi_write(0x03,1);
		if(txt_status ==1) udp_send(40);
		pause_half_ms(10);
		udp_send(140);
	}
	//Wrong Code
	else
	{
		if ((twi_data[1] != 0) || (twi_data[2] != 0))
		{
			twi_data[1] = 2;
			twi_write(0x03,1);
			if(txt_status ==1) udp_send(40);
			pause_half_ms(10);
			udp_send(140);
		}
	}
	
	pause_half_ms(20);
	if (update==1)
	{
		twi_data[1]=txt_status;
		output(0x1D,1);
	}

}
void update_siteplayer(void)
{
	twi_data[1] = alarm_status;
	twi_data[2] = txt_status;
	twi_data[3] = door_status;
	output(0x1C,3);
	twi_data[1] = heat_on;
	twi_data[2] = time_period;
	twi_data[3] = current_temp;
	output(0x23,3);
}

void read_alarm(void)
{
	uint8_t temp_alarm_status, temp_txt_status,x;
	
	//UCSRB = (1<<TXEN)|(1<<RXEN); //Disable Rx Interupt
	cli();
			
	UDR = 0xC1;						//send Read command to siteplayer to read 8 bytes
									//Read Command 0xC0 + # of bytes - 1
	do								//wait for UDR to clear
		x = UCSRA & 0x20;
	while (x != 0x20);
	
	UDR = 0x1C;					//send address to siteplayer
	
	do								//Wait for 1st Byte
		x = UCSRA & 0x80;
	while (x != 0x80);
	temp_alarm_status = UDR;				//Read 1st Byte
	
	do								//Wait for 2nd Byte
		x = UCSRA & 0x80;
	while (x != 0x80);
	temp_txt_status = UDR;				//Read 2nd Byte
	
		
	//Re-Enable RXD interrupt
	//UCSRB = (1<<RXCIE)|(1<<TXEN)|(1<<RXEN);
	sei();
	
	if (temp_alarm_status != alarm_status)
	{
		alarm_status = temp_alarm_status;
	}
	if (temp_txt_status != txt_status)
	{
		txt_status = temp_txt_status;
	}
	

}


/*******************
Thermostat Functions
*******************/
void thermostat(void)
{
	//sendstring(" S:",3);
	//print2(current_temp);
	//sendchar(" ");
	
	if (data < (current_temp-1))
	{
		THERMPORT |= (1<<THERMPIN);
		//sendchar("1");
		move(14,1);
		sendstring("ON",2);
		heat_on = 1;
		if(temp_up_flag == 0)
		{
			udp_send(155);
			temp_up_flag = 1;
		}
	}
	if (data >= (current_temp))
	{
		THERMPORT &= ~(1<<THERMPIN);
		//sendchar("0");
		move(14,1);
		sendstring("  ",2);
		heat_on = 0;
		if(temp_up_flag == 1)
		{
			udp_send(156);
			temp_up_flag = 0;
		}
	}

}
void get_thermostat_vals(void)
{
	uint8_t j;
		
	for (j=2;j<=7;j++)
		{
			//k = j-1;
			thermo_temps[j-1] = eeprom_read_byte(j);
		}
	for (j=8;j<=19;j++)
		{
			//k = j-7;
			thermo_times[j-7] = eeprom_read_byte(j);
		}
	
	hold = eeprom_read_byte(20);
		
	
	
}

void update_thermostat_temps(void)
{
		uint8_t i,thermo_val[6],x;
		
		//UCSRB = (1<<TXEN)|(1<<RXEN); //Disable Rx Interupt
		cli();
			
		UDR = 0xC5;						//send Read command to siteplayer to read 8 bytes
										//Read Command 0xC0 + # of bytes - 1
		do								//wait for UDR to clear
			x = UCSRA & 0x20;
		while (x != 0x20);
			
		UDR = 0x0A;					//send address to siteplayer
			
		for (i = 2; i<=7 ; i++)
		{
			do								//Wait for Byte
				x = UCSRA & 0x80;
			while (x != 0x80);
			thermo_val[i] = UDR;				//Read Byte
		}	
		//clear();
		
		for (i = 2; i<=7; i++)
		{
			//print3(thermo_val[i]);
			eeprom_write_byte(i,thermo_val[i]);
			//pause_half_ms(2000);
			//home();
			//pause_half_ms(5);
		}
		for (i=1; i<=6; i++)
		{
			thermo_temps[i] = thermo_val[i+1];
		//	print3(thermo_temps[i]);
		//	pause_half_ms(2000);
		//	home();
		//	pause_half_ms(5);
			
		}
		//UCSRB = (1<<RXCIE)|(1<<TXEN)|(1<<RXEN);
		sei();
}



void update_thermostat_times(void)
{
	uint8_t i,thermo_val[13],x;
	
	//UCSRB = (1<<TXEN)|(1<<RXEN);
	cli();
	
	UDR = 0xCB;
	do								//wait for UDR to clear
			x = UCSRA & 0x20;
	while (x != 0x20);
	UDR = 0x10;
	for (i = 8; i<=19 ; i++)
		{
			do								//Wait for Byte
				x = UCSRA & 0x80;
			while (x != 0x80);
			thermo_val[i] = UDR;				//Read Byte
		}	
	//clear();
	
		for (i = 8; i<=19; i++)
		{
			//print3(thermo_val[i]);
			//eeprom_addr = i;
			//eeprom_var1 = i;
			eeprom_write_byte(i,thermo_val[i]);
			//pause_half_ms(1000);
			//home();
			//pause_half_ms(5);
		}
		for (i = 1; i<=12; i++)
		{
			thermo_times[i] = thermo_val[i+7];
		}
		
	//UCSRB = (1<<RXCIE)|(1<<TXEN)|(1<<RXEN);
	//print3(111);
	sei();
	//pause_half_ms(1);
}

void check_hold(void)
{
	input(0x26,1);
	hold = twi_data[1];
	eeprom_write_byte(20,hold);
}

/************
TWI Functions
************/


		void twi_write(uint8_t sla, uint8_t num)
		{
			while (status) nop();									//Wait for TWI if busy
			sla = sla << 1;											//Shift Slave Address one bit for R/W
			slave_address = sla;									//Load Slave Address
			data_direction = TW_WRITE;								//Set Data Direction
			data_pointer = 1;										//Reset Data Pointer
			bytes = num;											//Set number of bytes to send
			retry = 0;												//Clear retry counter
			status = 1;												//Set TWI busy
			//sendchar("N");
			//pause_half_ms(1000);
			TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE);		//Send Start (Interrupt handles the rest)
		}
			
		void twi_read(uint8_t sla, uint8_t num)
		{
			while (status) nop();									//Wait for TWI if busy
			sla = sla << 1;											//Shift Slave Address one bit for R/W
			slave_address = sla;									//Load Slave Address
			data_direction = TW_READ;								//Set Data Direction
			data_pointer = 1;										//Reset Data Pointer
			bytes = num;											//Set number of bytes to send
			retry = 0;												//Clear retry counter
			status = 1;												//Set TWI busy
			TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE);		//Send Start (Interrupt handles the rest)
		
		}
	
	
	
	/**********
	 DS1307 RTC
	***********/
	
		//Get Time
		void twi_gettime(void)
		{
			uint8_t seconds, day, date, month, year, PM;
			div_t ans;			
			
			twi_data[1] = 0x00;			//Set Register Address
			twi_write(RTCADDR,1);
			twi_read(RTCADDR,7);		//Read 7 Bytes
			while(status) nop();
			
			seconds = twi_data[1];
			minutes = twi_data[2];
			hours   = twi_data[3];
			day     = twi_data[4];
			date    = twi_data[5];
			month   = twi_data[6];
			year    = twi_data[7];
			
				
			
					
			move(7,1);
			
			//This section determines if the time is AM or PM... Since I didn't want to display 24hr time mode
			//I enabled the AM/PM function of the RTC, this turned out to be a huge pain in the ass when doing
			//The thermostat as you will see in that section, but I chose to just deal with it instead of
			//doing it the right way and using 24hr mode and just doing a simple subtraction to get 12hr time
			//Meh, just one of those things....
			
			uint8_t hour1;
						
			if ((hours & 0x20) == 0x20) 
			{
				PM = 1;
				twi_data[8] = 1;
			}
			else 
			{
				PM = 0;
				twi_data[8] = 0;
			}
			hour1 = hours & 0x0F;
			hours = ((hours & 0x1F) >> 4)*10;
			hours = hours + hour1;
			twi_data[3] = hours;
			print2(hours);
			sendchar(":");
			
			//Since time data from RTC is in a differnt format, it must be converted to a decimal.
			//Check RTC pdf for more info on format.
		
			minutes = format_vals(minutes);
			twi_data[2] = minutes;
			print2(minutes);
			//sendchar(":");
			
			
			seconds = format_vals(seconds);
			twi_data[1] = seconds;
			//print2(seconds);
			
			date = format_vals(date);
			twi_data[5] = date;
			
			month = format_vals(month);
			twi_data[6] = month;
			
			year = format_vals(year);
			twi_data[7] = year;
			
			if (PM)
			{
				sendchar("P");
				//sendchar("M");
			}
			else
			{
				sendchar("A");
				//sendchar("M");
			}
			
			output(0x02,8);
			/*************************************************************************************
			As mentioned above, this section could have been simplified with 24hr time format...
			Oh well, too late now, it's working and I don't really feel like changing it.
			
			Generally the algorithm is to take the current time and check to see what day it is.
			M-F have a morning/daytime/night settings Sat-Sun just have day/night night is the
			same as weekend night, but day is different.
			
			After that, the time is checked to see what time period it is.  First, because of the
			whole 12/24hr time thing, it checks to see if it's in the midnight hour, then
			it checks against the daytime time setting first if it's greater than 1 hour from the 
			daytime time setting, if not then it checks against the daytime time setting hour and
			minute because the daytime time setting could be 7:45, and the first check would miss
			that.  The reason I did this was that after you roll over into a new hour (ie 8:00) 
			you don't care that what the minute is, however in the 7:00 hour you do, so that you
			catch the 7:45 rollover into a new time period, you could probably incorporate all this 
			into one larger conditional statment, but I chose to do it this way.  
			
			This general pattern repeats through all the time periods.
			
			**************************************************************************************/
			
			if(hold == 0)
			{
				if((day<6) && (PM == 0))			//M-F Before Noon
				{
					if (hours == 12)
					{
						current_temp = thermo_temps[4];  	//Night
						time_period = 1;
					}
					else if (hours >= (thermo_times[1] + 1))
					{
						current_temp = thermo_temps[2];		//Daytime		
						time_period = 3;
					}
					else if ((hours >= thermo_times[1]) && (minutes >= thermo_times[2]))
					{
						current_temp = thermo_temps[2];		//Daytime
						time_period = 3;
					}
					else if (hours >= (thermo_times[7] + 1))
					{
						current_temp = thermo_temps[1];		//Morning
						time_period = 2;
					}
					else if ((hours >= thermo_times[7]) && (minutes >= thermo_times[8]))
					{
						current_temp = thermo_temps[1];		//Morning
						time_period = 2;
					}
					else 
					{
						current_temp = thermo_temps[4]; 	//Night
						time_period = 1;
					}
				}
				if((day<6) && (PM == 1))		//M-F Afternoon
				{
					if (hours == 12)
					{
						current_temp = thermo_temps[2];		//Daytime
						time_period = 3;
					}
					else if (hours >= (thermo_times[5] + 1))
					{
						current_temp = thermo_temps[4];		//Night
						time_period = 1;
					}
					else if ((hours >= thermo_times[5]) && (minutes >= thermo_times[6]))
					{
						current_temp = thermo_temps[4];		//Night
						time_period = 1;
					}
					else if (hours >= (thermo_times[3] + 1))
					{
						current_temp = thermo_temps[3];		//Evening
						time_period = 4;
					}
					else if ((hours >= thermo_times[3]) && (minutes >= thermo_times[4]))
					{
						current_temp = thermo_temps[3];		//Evening
						time_period = 4;
					}
					else
					{
						current_temp = thermo_temps[2];		//Daytime
						time_period = 3;
					}
				}
				if((day>=6) && (PM == 0))		//Sat-Sun Before Noon
				{
					if (hours == 12)
					{
						current_temp = thermo_temps[4];		//Night
						time_period = 1;
					}
					else if (hours >= (thermo_times[9] + 1))
					{
						current_temp = thermo_temps[5];		//Weekend
						time_period = 5;
					}
					else if ((hours >= thermo_times[9]) && (minutes >= thermo_times[10]))
					{
						current_temp = thermo_temps[5];		//Weekend
						time_period = 5;
					}
					else
					{
						current_temp = thermo_temps[4];		//Night
						time_period = 1;
					}
				}
				if((day>=6) && (PM == 1))		//Sat-Sun Afternoon
				{
					if (hours == 12)
					{
						current_temp = thermo_temps[5];		//Weekend
						time_period = 5;
					}
					else if (hours >= (thermo_times[11] + 1))
					{
						current_temp = thermo_temps[4];		//Night
						time_period = 1;
					}
					else if ((hours >= thermo_times[11]) && (minutes >= thermo_times[12]))
					{
						current_temp = thermo_temps[4];		//Night
						time_period = 1;
					}
					else
					{
						current_temp = thermo_temps[5];		//Weekend
						time_period = 5;
					}
				}
				
				move(7,2);
				sendstring("Auto: ",6);
				print2(current_temp);
				lcd_display(0);
			
			}
			else 
			{
				current_temp = thermo_temps[6]; 			//hold_temp
				time_period = 6;
				move(7,2);
				sendstring("Hold: ",6);
				print2(current_temp);
				lcd_display(0);
			}
			
			
			//Function which checks to see if current time is a multiple of 5.
			//If so, current temp info is sent to SP to be relayed to vB program
			ans = div(minutes,5);
			if ((ans.rem == 0) && (update_flag == 0)) 
			{
				udp_send(200);
				pause_half_ms(15);
				udp_send(data);
				pause_half_ms(15);
				udp_send(data1);
				pause_half_ms(15);
				udp_send(o_temp);
				pause_half_ms(15);
				udp_send(o_temp1);
				pause_half_ms(15);
				udp_send(below_zero);
				pause_half_ms(15);
				update_flag = 1;
			}
			if (ans.rem != 0) update_flag = 0;
			
			
			
			
		}

		uint8_t format_vals(uint8_t input)
		{
			uint8_t temp_val;
			
			temp_val = input & 0x0F;
			input = (input >> 4) * 10;
			input = temp_val + input;
			return input;
		}
	
		//Initialize RTC
		void twi_initRTC(void)
		{
			uint8_t hr_mode,seconds;
			
			//Read Current Seconds Information
			twi_data[1] = 0x00;
			twi_write(RTCADDR,1);
			twi_read(RTCADDR,1);
			while(status) nop();
			seconds = twi_data[1];
			
			twi_data[1] = 0x00;				//Set Register Address
			twi_data[2] = seconds & 0x7F;	//Start Oscillator if need be
			twi_write(RTCADDR,2);
			while(status) nop();
			twi_data[1] = 0x07;			//Set Register Address
			twi_data[2] = 0x10;			//Enable 1Hz Square Wave
			twi_write(RTCADDR,2);
			while(status) nop();
			
			//Set 12HR bit without resetting current time info
			twi_data[1] = 0x02;			//Set Register Address
			twi_write(RTCADDR,1);
			twi_read(RTCADDR,1);		//Read 0x02 Register (Current Hour)
			while(status) nop();
			hr_mode = twi_data[1];
			twi_data[2] = hr_mode | 0x40;	//Make sure 12HR bit is set
			twi_data[1] = 0x02;
			twi_write(RTCADDR,2);			//Send to RTC
			
		}
		
		//Update RTC Time and Date Values
		void update_time(void)
		{
			uint8_t x, second, minute, hour, day, date, month, year, PM, convert, convert1;
			cli();
			//Read Values From Siteplayer
			
			UCSRB = (1<<TXEN)|(1<<RXEN); //Disable Rx Interupt
					
			UDR = 0xC7;						//send Read command to siteplayer to read 8 bytes
											//Read Command 0xC0 + # of bytes - 1
			do								//wait for UDR to clear
				x = UCSRA & 0x20;
			while (x != 0x20);
			
			UDR = 0x02;					//send address to siteplayer
			
			do								//Wait for 1st Byte
				x = UCSRA & 0x80;
			while (x != 0x80);
			second = UDR;				//Read 1st Byte
			
			do								//Wait for 2nd Byte
				x = UCSRA & 0x80;
			while (x != 0x80);
			minute = UDR;				//Read 2nd Byte
			
			do								//....
				x = UCSRA & 0x80;
			while (x != 0x80);
			hour = UDR;
			
			do
				x = UCSRA & 0x80;
			while (x != 0x80);
			day = UDR;
			
			do
				x = UCSRA & 0x80;
			while (x != 0x80);
			date = UDR;
			
			do
				x = UCSRA & 0x80;
			while (x != 0x80);
			month = UDR;
			
			do								
				x = UCSRA & 0x80;
			while (x != 0x80);
			year = UDR;				//....
			
			do  x = UCSRA & 0x80;		//Wait for 8th Byte
			while (x != 0x80);
			PM = UDR;					//Read 8th Byte
			
			
			
			//Re-Enable RXD interrupt
			UCSRB = (1<<RXCIE)|(1<<TXEN)|(1<<RXEN);
			/*******************************************
			 Convert Values into correct format for RTC  
			  Bit| 7  6  5  4 | 3  2  1  0 | 
			     |   Tens     |   Ones     |
			*******************************************/
		
			twi_data[1] = 0x00;   						//Set RTC Address 0x00
			
			//Seconds
			convert = (second / 10)*10;				//Find 10's place value
			convert = second - convert;					//Subtract it from convert to give you the 1's place value
			convert = convert & 0x0F;					//Mask off 4 MSB's to make sure they are zero
			second = (second/10) << 4;					//Divide by 10 to get 10's place digit in the 1's spot then shift it to the right.
			second = second|convert;					//Combine 10's Value and 1's Value to meet the correct format
			twi_data[2] = second;
			
			//Minutes
			convert = (minute / 10)*10;				//Same as Seconds
			convert = minute - convert;
			convert = convert & 0x0F;
			minute = (minute/10) << 4;
			minute = minute|convert;
			twi_data[3] = minute;
			
			//Hours
			if (PM) convert1 = 0x20;					//IF PM bit set, convert 1 =  0010 0000
			else convert1 = 0;
			convert = ((hour & 0x1F)/10)*10;			//Mask 3 MSB's from hour (contain 12/24 HR and AM/PM Info)
			convert = (hour & 0x1F) - convert;
			convert = convert & 0x0F;
			hour = (((hour & 0x1F)/10) << 4);
			hour = hour | convert | convert1 | 0x40;	//Combine 10's, 1's, AM/PM info and keep 12HR bit set (0x40)
			twi_data[4] = hour;
			
			//Day
			twi_data[5] = day;							//Value is only from 1-7 so no conversion is needed
			
			//Date
			convert = (date / 10)*10;					//Same as Seconds
			convert = date - convert;
			convert = convert & 0x0F;
			date = (date/10) << 4;
			date = date|convert;
			twi_data[6] = date;
			
			//Month
			convert = (month/10)*10;					//Same as Seconds
			convert = month - convert;
			convert = convert & 0x0F;
			month = (month/10) << 4;
			month = month|convert;
			twi_data[7] = month;
			
			//Year
			convert = (year/10)*10;					//Same as Seconds
			convert = year - convert;
			convert = convert & 0x0F;
			year = (year/10) << 4;
			year = year|convert;
			twi_data[8] = year;
			
			twi_write(RTCADDR,8);						//Send values to RTC
			
			sei();
		}

	/*************************
	 DS1624 Temperature Sensor
	 ************************/

		void start_conversion(void)
		{			
			
			//home();
			//pause_half_ms(5);
			//print3(20);
			//print3(status);
			//pause_half_ms(1000);
			
			wdt_reset();
			
			twi_data[1] = 0xEE;			//Start Temperature Conversion in free run mode
			twi_write(TEMPADDR,1);
			//print3(TW_STATUS);
			while(status) nop();
			
			//home();
			//pause_half_ms(5);
			//print3(21);
			//pause_half_ms(1500);
			//wdt_reset();
			
			twi_data[1] = 0xEE;
			twi_write(TEMPADDR_O,1);
			while(status) nop();
			
			//home();
			//pause_half_ms(5);
			//print3(22);
			pause_half_ms(150);
			//wdt_reset();
		}

		void twi_readtemp(void)
		{
			uint16_t temp;
			uint8_t y,y1,y2;
			
			temp = 0;
			
			twi_data[1] = 0xAA;						//Load Read Command
			twi_write(TEMPADDR,1);					//Send
			twi_read(TEMPADDR,2);					//Read 2 bytes
			while(status) nop();					//Wait for transfer to finish
			data = twi_data[1];						//load integer and decimal values into temp variables
			data1 = twi_data[2];
						
			data1 = (data1 >> 3) & 0x1F;			//Bitshift empty bits from decimal
			data1 = (data1*3);						//Convert binary to decimal 00. to ~99.
			temp = (data * 100);					//Shift data over by 2 bits
			temp = temp + data1;					//Add decimal 00-99 to empty space
			
						
			temp = (((temp*18)/100)+320);			//Convert to degrees F with 1's place representing tenths of a degree
			y2=temp/1000;							//Find 100's Digit
			temp-=(y2*1000);
			y1=temp/100;							//Find 10's Digit
			temp-=(y1*100);
			y = temp/10;							//Find 1's Digit
			temp-=(y*10);							//Remainder = Decimal
			data1 = temp;							//Put decimal place back in data 1
			data = (y2*100) + (y1*10) + y;			//Multiply out Temp
			twi_data[1] = data;
			twi_data[2] = data1;
			
			output(0x00,2);
			
			move(1,1);
			pause_half_ms(2);
			print2(data);							//Display on LCD
			sendchar(".");
			lcd_display(data1+0x30);
			lcd_display(0);
			//sendchar("F");
			
			
			
		}	
		
		void twi_readtemp1(void)
		{
			int32_t temp;
			uint8_t y,y1,y2,neg_flag;
			
			temp = 0;
			
			twi_data[1] = 0xAA;						//Load Read Command
			twi_write(TEMPADDR_O,1);					//Send
			twi_read(TEMPADDR_O,2);					//Read 2 bytes
			while(status) nop();					//Wait for transfer to finish
			o_temp = twi_data[1];						//load integer and decimal values into temp variables
			o_temp1 = twi_data[2];
			
			o_temp1 = (o_temp1 >> 3) & 0x1F;			//Bitshift empty bits from decimal
			
			
			if (o_temp > 150)
			{
				o_temp = 255 - o_temp;
				o_temp1 = 32 - o_temp1;
				neg_flag = 1;
			}
			else neg_flag = 0;
			
						
			
			o_temp1 = (o_temp1*3);						//Convert binary to decimal 00. to ~99.
			temp = (o_temp * 100);					//Shift data over by 2 bits
			temp = temp + o_temp1;					//Add decimal 00-99 to empty space
			
			
			if (neg_flag == 1)
			{
				temp = 0 - temp;
			}
			
						
			temp = (((temp*18)/100)+320);			//Convert to degrees F with 1's place representing tenths of a degree
			
			if (temp < 0)
			{
				below_zero = 2;
				temp = abs(temp);
			}
			else below_zero = 1;
			
			y2=temp/1000;							//Find 100's Digit
			temp-=(y2*1000);
			y1=temp/100;							//Find 10's Digit
			temp-=(y1*100);
			y = temp/10;							//Find 1's Digit
			temp-=(y*10);							//Remainder = Decimal
			o_temp1 = temp;							//Put decimal place back in data 1
			o_temp = (y2*100) + (y1*10) + y;		//Multiply out Temp
			twi_data[1] = o_temp;
			twi_data[2] = o_temp1;
			twi_data[3] = below_zero;
			
			output(0x20,3);
			
			move(1,2);
			pause_half_ms(2);
			print2(o_temp);							//Display on LCD
			sendchar(".");
			lcd_display(o_temp1+0x30);
			lcd_display(0);
			//sendchar("F");
			
			
			
		}	


/***********
 4x3 Keypad
***********/
/*
	void keypad(void)
	{
		uint8_t row,column,j,key,code[2],code_ac;
		j = 0;
		code_ac = 0;
		//Disable Keypress Interupt
		GICR &= ~(1<<INT1);
		
		//Setup LCD
		clear();
		sendstring("Enter Code:",11);
		line2();
				
		
		while ( j < 4 ) 
		{
			row = 0x00;
			column = 0x70;
			
			//Scan Column
			pause_half_ms(2);
			while (column == 0x70) column = PINB & 0x70;			//Wait for keypress
			KEYDIR = (1<<C1)|(1<<C2)|(1<<C3);						//Set Column as Ouput
			KEYPAD = (1<<R1)|(1<<R2)|(1<<R3)|(1<<R4);				//Set Pullups on Rows
			pause_half_ms(1);
			row = PINB & 0x0F;										//CHANGE THIS IF MOVING PINS
			
			//Scan Row
			KEYDIR = (1<<R1)|(1<<R2)|(1<<R3)|(1<<R4);				//Set Row as Output
			KEYPAD = (1<<C1)|(1<<C2)|(1<<C3);						//Set Pullups on Columns
			pause_half_ms(1);
			column = PINB & 0x70;									//CHANGE THIS IF MOVING PINS
			
			key = 255 - (row + column);							//Combine result (255- is probably not necessary, added for different reason)
			
			
			switch (key)
			{
				case 145:
				{
					code[j] = 1;
					break;
				}
				case 161:
				{
					code[j] = 2;
					break;
				}
				case 193:
				{
					code[j] = 3;
					break;
				}
				case 146:
				{
					code[j] = 4;
					break;
				}
				case 162:
				{
					code[j] = 5;
					break;
				}
				case 194:
				{
					code[j] = 6;
					break;
				}
				case 148:
				{
					code[j] = 7;
					break;
				}
				case 164:
				{
					code[j] = 8;
					break;
				}
				case 196:
				{
					code[j] = 9;
					break;
				}
				case 152:
				{
					code[j] = 10;
					break;
				}
				case 168:
				{
					code[j] = 0;
					break;
				}
				case 200:
				{
					code[j] = 11;
					break;
				}
			}
			
			
			while (column != 0x70) column = PINB & 0x70; 			//Wait for stop pressing key
							
			if (code[j] == 10)										//Keypress = *  do nothing
			{
				
			}
			else													//All other keypresses, display code, increment counter
			{
				lcd_display(code[j]+48);
				j++;
			}
			
			if (j==4) pause_half_ms(2000);						 	//Pause after last keypress
		
		}
		
		
		for(j = 0; j<4; j++)
		{
			code_ac = code_ac + code[j];
		}
		
		if (code_ac == 4)
		udp_send(20);
		
		if (code_ac == 6)
		udp_send(30);
		
		if (code_ac == 7)
		udp_send(40);
		
		//Set port for next code entry, re-enable interupt
		KEYDIR = (1<<C1)|(1<<C2)|(1<<C3);
		KEYPAD = (1<<R1)|(1<<R2)|(1<<R3)|(1<<R4);
		GICR |= (1<<INT1);
		
		
	}*/

/********************
Siteplayer Interface
*********************/
	void udp_send(uint8_t val)
	{
			uint8_t x;
			
			UDR = 0x80;
			do								//wait for UDR to clear
				x = UCSRA & 0x20;
			while (x != 0x20);
			
			UDR = 0x00;
			do								//wait for UDR to clear
				x = UCSRA & 0x20;
			while (x != 0x20);
			
			UDR = val;
			do								//wait for UDR to clear
				x = UCSRA & 0x20;
			while (x != 0x20);
			
			UDR = 0x50;
			do								//wait for UDR to clear
				x = UCSRA & 0x20;
			while (x != 0x20);
	}

	void output(uint8_t address, uint8_t num_bytes)
	{
		
		uint8_t x,i;
		
		for (i = 1; i <= num_bytes; i++)
		{
			UDR = 0x80;					//send write command to siteplayer
			
			do								//wait for UDR to clear
				x = UCSRA & 0x20;
			while (x != 0x20);
			
			UDR = address;					//send address to siteplayer
			
			do								//wait for UDR to clear
				x = UCSRA & 0x20;
			while (x != 0x20);
			
			UDR = twi_data[i];					//send Temperature values to siteplayer
			
			do								//wait for UDR to clear
				x = UCSRA & 0x20;
			while (x != 0x20);
			
			address = address + 1;
			
			
		}
	}
	void input(uint8_t address, uint8_t num_bytes)
	{
		uint8_t x,i,temp;
		temp = num_bytes - 1;
		temp = 0xC0 + temp;
		
		cli();
		UDR = temp;			//send Read command to siteplayer to read 8 bytes
										//Read Command 0xC0 + # of bytes - 1
		do								//wait for UDR to clear
			x = UCSRA & 0x20;
		while (x != 0x20);
			
		UDR = address;					//send address to siteplayer
			
		for (i = 1; i<=num_bytes ; i++)
		{
			do								//Wait for Byte
				x = UCSRA & 0x80;
			while (x != 0x80);
			twi_data[i] = UDR;				//Read Byte
			address = address + 1;
		}
		sei();
	}



/*********************
LCD HD44780 Compatable
*********************/
	
	//Load Custom Characters
	void load_characters(void)
	{
		cgaddress(0);
		lcd_display(0x18);
		cgaddress(1);
		lcd_display(0x18);
		cgaddress(2);
		lcd_display(0x07);
		cgaddress(3);
		lcd_display(0x04);
		cgaddress(4);
		lcd_display(0x06);
		cgaddress(5);
		lcd_display(0x04);
		cgaddress(6);
		lcd_display(0x04);
		cgaddress(7);
		lcd_display(0x00);
	}

	/****************
	 Output Functions
	****************/

		//Send Byte to LCD
		void lcd_display(uint8_t x)
		{
			uint8_t delay;
			COMM = (1<<RS)|(0<<RW);	//Set Data Entry Mode
			for (delay=1;delay<255;delay++);
			COMM = (1<<E)|(1<<RS);		//Enable
			DATA = x;
			COMM = (0<<E);
			
		}

		//Set CGRAM Address
		void cgaddress(uint8_t x)
		{
			COMM = (0<<RS)|(0<<RW);	//Set Data Entry Mode
			COMM = (1<<E);
			pause_half_ms(2);
			DATA = x+64;
			COMM = (0<<E);
		}
		
		//Send Character
		void sendchar(char *str) 
		{  
		
			uint8_t delay;
			COMM = (1<<RS)|(0<<RW);	//Set Data Entry Mode
			for (delay=1;delay<255;delay++);
			COMM = (1<<E)|(1<<RS);		//Enable
			DATA = *str;
			COMM = (0<<E);
		}

		void sendstring(char *str,uint8_t len)
		{
			uint8_t delay,i;
			i = 1;
			while(i<=len)
			{
				COMM = (1<<RS)|(0<<RW);	//Set Data Entry Mode
				for (delay=1;delay<255;delay++);
				COMM = (1<<E)|(1<<RS);		//Enable
				DATA = *str;
				COMM = (0<<E);
				*str++;
				i++;
			}
		}
		

		//Goto Line 2
		void line2(void) {
		
			home();
			COMM = (0<<RS)|(0<<RW);	//Set Data Entry Mode
			COMM = (1<<E);
			pause_half_ms(2);
			DATA = 192;
			COMM = (0<<E);
			}


		//Print 3 Digit Number
		void print3(uint8_t x)
		{
		  unsigned int y;
			y=x/100;lcd_display(y+48);x-=(y*100);
			y=x/10;lcd_display(y+48);x-=(y*10);
			lcd_display(x+48); 
		}
		
		//Print 2 Digit Number
		void print2(uint8_t x)
		{
		  unsigned int y;
			y=x/10;lcd_display(y+48);x-=(y*10);
			lcd_display(x+48); 
		}


	/*************
	LCD INITIALIZE
	*************/
		//Initialize
		void init(void)
		{
			func_set();
			func_set();
			func_set();
			disp();
			clear();
			entry();
		}

		//Send Function Set
		void func_set(void)
		{
			pause_half_ms(40);
			COMM = (1<<E);
			COMM = (0<<RS)|(0<<RW);
			pause_half_ms(2);
			DATA = 56;						//00110000
			COMM = (0<<E);				
			pause_half_ms(50);
		}

		//Send Display On/Off and Cursor Info
		void disp(void)
		{
			COMM = (0<<RS)|(0<<RW);
			COMM = (1<<E);
			pause_half_ms(2);
			DATA = 12;						//00001111
			COMM = (0<<E);
			pause_half_ms(100);
		}

		//Clear Display
		void clear(void)
		{
			COMM = (0<<RS)|(0<<RW);
			COMM = (1<<E);
			pause_half_ms(2);
			DATA = 1;
			COMM = (0<<E);
			pause_half_ms(100);
		}

		//Send Entry Mode Set
		void entry(void)
		{
			COMM = (0<<RS)|(0<<RW);
			COMM = (1<<E);
			pause_half_ms(2);
			DATA = 6;						//00000110
			pause_half_ms(100);
		}

		//Home
		void home(void)
		{
			COMM = (0<<RS)|(0<<RW);
			COMM = (1<<E);
			pause_half_ms(2);
			DATA = 2;
			COMM = (0<<E);
		}
		
		void move(uint8_t x, uint8_t y)
		{
			uint8_t g;
			
			x=x-1;
			g=0;
			if (y == 2) line2();
			if (y == 1) home();
					
			while(x != g)
			{
				COMM = (0<<RS)|(0<<RW);	//Set Data Entry Mode
				COMM = (1<<E);
				pause_half_ms(1);
				DATA = 20;
				COMM = (0<<E);
				g++;
			}
	
	}
			

/****************
 Pause Functions
*****************/
	void nop(void)
	{
		asm volatile("nop"::);
	}
	
	void pause_half_ms(uint16_t ms) 				//Pause half milliseconds
	  {
		uint16_t j;
		uint8_t state,k;
		j=0;
		do
		{
		
			for(k=0; k<2; k++){				//8000000/1000=8000 cyc/ms Clk/8 = 8000/8 =
				do 								//1000 clock increments per ms = 1000/256 ~ 4
				state = TIFR & 0x1;		//(3.90) clock interupts per ms or 1
				while (state != 0x1);			//interrupt in 1/4 ms.... run twice for
				TIFR = (1<<TOV0);			//delay of ~1/2ms
				}
		j++;
		}
		while(j<ms);
	}