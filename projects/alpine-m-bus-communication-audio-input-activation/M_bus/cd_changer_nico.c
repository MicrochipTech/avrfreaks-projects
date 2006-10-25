//Compiler = WinAVR - Win32 build of avr-gcc 3.3 (Sept.13,2003)
//File = CD_changer.c

//CD changer controller program. The controller consist of ATtiny26, LCD display 2x16 (HD44780)
//(4-bit interface) and 7-key keyboard. CD changer Sherwood connected through Alpine M-bus (1-wire,
//2- directions).

//************************************* #include **********************************
#include <avr/iotn26.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <avr/eeprom.h>

//************************************* #define ***********************************
#define EEPROM_SECTION  __attribute__ ((section (".eeprom")))

//************************************ prototypes *********************************
void init(void);
void out_instr(unsigned char value);
void out_data(unsigned char value);
void time_loop(int tenmicros);
void send(unsigned char nmbr);
void count_crc (unsigned char x);
void ping(void);
void copy_cmd_ch(void);
void chk_disk(void);

//************************************ variables **********************************
unsigned char value;
unsigned char temp;
unsigned char x;
volatile unsigned char received;
volatile unsigned char bus_bit;
volatile unsigned char bus_byte = 0;
volatile unsigned char n_bit = 0;
volatile unsigned char n_byte = 0;
volatile unsigned char bus_busy = 0;
volatile unsigned char s_flag = 0;
volatile unsigned char counter = 0;
volatile unsigned char ping_sent = 0;
volatile unsigned char curr_disk;
unsigned char disk_up = 0;
unsigned char disk_down = 0;

char R_BUS[16]={};
char S_BUS[9]={};

prog_char CH_D0[8] = {1,1,3,0,0,0,0,0};
prog_char PLAY[6] = {1,1,1,0,1,1};
prog_char STOP[6] = {1,1,1,4,0,6};
prog_char PAUSE[6] = {1,1,1,0,2,4};

//************************************ main program *******************************
int main (void)
{
 	unsigned char n;
	unsigned char button;

	init();											//initialization
	eeprom_read_block(FSTLINE,PATERN1,16);
	for(;;)
	{
		button = read_keyboard();								//read keyboard
		if (button!=0)
		{
			for(n=0;n<10;n++)
			{
				*(SNDLINE+n)=' ';							//clear part of second line lcd
			}
			out_2_lcd();								//output second line
		}
		switch (button)
		{
			case 1:									//- - - PREV. DISK
				*(S_BUS+3)=(curr_disk -1);						//copy and dec number of current disk
				count_crc (7);							//crc
				send(8);								//send command to CD_changer
			break;
			case 2:									//- - - NEXT DISK
				*(S_BUS+3)=(curr_disk +1);						//copy and inc number of current disk
				count_crc (7);							//crc
				send(8);								//send command to CD_changer
			break;
			case 3:									//- - - PREV. TRACK
				*(S_BUS+4)=*(R_BUS+3);						//copy number of current track
				*(S_BUS+5)=(*(R_BUS+4)-1);					//dec number of track
				if (*(S_BUS+5)<0) 							//if units too small
					{
						*(S_BUS+5)=9;					//units = 9
						*(S_BUS+4)-=1;					//dec tens
						if (*(S_BUS+4)<0)					//if tens too small
						{
							*(S_BUS+4)=0;				//let tens = 0
							*(S_BUS+5)=1;				//and units = 1
						}
					}
				count_crc (7);							//crc
				send(8);								//send command to CD_changer
			break;
			case 4:									//- - - NEXT TRACK
				*(S_BUS+4)=*(R_BUS+3);						//copy number of current track
				*(S_BUS+5)=(*(R_BUS+4)+1);					//inc number of track
				if (*(S_BUS+5)>9) 							//if units too big
					{
						*(S_BUS+5)=0;					//units = 0
						*(S_BUS+4)+=1;					//inc tens
					}
				count_crc (7);							//crc
				send(8);								//send command to CD_changer
			break;
			case 5:									//- - - PLAY
				for(n=0;n<6;n++)
				{
					*(S_BUS+n)=PRG_RDB (&PLAY[n]);				//command PLAY
				}
				send(6);								//send command to CD_changer
			break;
			case 6:									//- - - STOP
				for(n=0;n<6;n++)
				{
					*(S_BUS+n)=PRG_RDB (&STOP[n]);				//command STOP
				}
				send(6);								//send command to CD_changer
			break;
			case 7:									//- - - PAUSE
				for(n=0;n<6;n++)
				{
					*(S_BUS+n)=PRG_RDB (&PAUSE[n]);			//command PAUSE
				}
				send(6);								//send command to CD_changer
			break;
		}

		if (received == 1)									//information from CD changer analysis
		{
			if (((*R_BUS)==9)&&((*(R_BUS+1))==9))					//if code beginning is 99
			{
				if ((*(R_BUS+2))==4)						//and 4
				{
					chk_disk();						//check disk
					eeprom_read_block(SNDLINE,PATERN2_4,16);			//state Playing
					copy_mm_ss();						//copy minutes and seconds
					copy_t();							//copy track number
				}
				if ((*(R_BUS+2))==3)						//and 3
				{
					eeprom_read_block(SNDLINE,PATERN2_5,16);			//state Paused
					copy_mm_ss();						//copy minutes and seconds
					copy_t();							//copy track number
				}
				if (((*(R_BUS+2))==2) || (((*(R_BUS+2))==0)&&((*(R_BUS+5))!=0)))
				{								//if code is 992.. or 990xx not0..
					eeprom_read_block(SNDLINE,PATERN2_6,16);			//state Stopped
					copy_mm_ss();						//copy minutes and seconds
					copy_t();							//copy track number
				}
				if (((*(R_BUS+2))==0)&&((*(R_BUS+5))==0))
				{								//if code is 990xx0..
					eeprom_read_block(SNDLINE,PATERN2_1,16);			//state Empty
				}
				out_1_lcd();							//output first line
				out_2_lcd();							//output second line
			}
			if (((*R_BUS)==9)&&((*(R_BUS+1))==0x0b))					//if code is 9B
			{
				curr_disk = *(R_BUS+3);						//remember current disk
				eeprom_read_block(SNDLINE,PATERN2_3,16);				//state Changing
				*(FSTLINE+5)=(curr_disk+48);					//disk number on LCD
				out_1_lcd();							//output first line
				out_2_lcd();							//output second line
			}
			counter=0;								//reset ping counter
			ping_sent=0;								//reset flag "ping sent"
			received = 0;								//reset M-Bus receive flag
		}

		if ((ping_sent>0)&&(counter>100))
		{
			eeprom_read_block(SNDLINE,PATERN2_2,16);					//state Not connected
		}
		counter++;									//inc counter
		if (counter>250)									//after some time
		{
			ping();									//send ping
			counter=0;								//reset counter
			ping_sent=1;								//set flag "ping sent"
		}

		if (disk_up>0)
		{
			copy_cmd_ch();								//copy command CHANGE
			*(S_BUS+3)=(*(R_BUS+5)+1);						//copy and inc number of current disk
			count_crc (7);								//crc
			send(8);									//send command to CD_changer
			disk_up=0;
		}
		if (disk_down>0)
		{
			copy_cmd_ch();								//copy command CHANGE
			*(S_BUS+3)=(*(R_BUS+5)-1);						//copy and dec number of current disk
			count_crc (7);								//crc
			send(8);									//send command to CD_changer
			disk_down=0;
		}
	}
}

//************************************ functions **********************************
void copy_cmd_ch(void)										//copy command CHANGE to the S_BUS
{
	unsigned char n;
	for(n=0;n<8;n++)
	{
		*(S_BUS+n)=PRG_RDB (&CH_D0[n]);							//command CHANGE
	}
}

//*********************************************************************************
void ping(void)											//send ping to changer
{
	*(S_BUS)=1;
	*(S_BUS+1)=8;
	*(S_BUS+2)=0x0a;										//put 18A to the S_BUS
	send(3);											//send command to CD_changer
}

//*********************************************************************************
void count_crc (unsigned char x)									//count CRC, and put it
{												//in the last send location
	unsigned char temp =0;
	unsigned char n ;
	for (n=0;n<x;n++)
	{
		temp = *(S_BUS+n) ^ temp;								//XOR all bytes
	}
	temp++;											//and increment
	if (temp>15) temp=0;									//reset if too big
	*(S_BUS+x) = temp;									//put in the last location to send
}

//*********************************************************************************
SIGNAL (SIG_INTERRUPT0)  									//INT0 interrupt handler
{
	bus_busy = 1;                                       							//set flag "bus is busy"
	if (PINB & 0x40)										//if INT0 = 1
	{
		GIMSK &= 0xbf;									//INT0 interrupt disable
 		MCUCR = 0x02;									//INT0 falling edge sensitive
		GIMSK |= 0x40;									//INT0 interrupt enable
		TCNT0 = 186;									//start T0 with the value
		TCCR0 = 0x0b;									//and prescaler CLK/64 (reset prescaler)
	}
	if (!(PINB & 0x40))										//if INT0 = 0
	{
	 	if (TCNT0 > 208) bus_byte ++; 							// > 1.3 ms
	 	bus_byte = (bus_byte << 1);								//shift byte left
		n_bit++;										//inc bit number
		if (n_bit == 4)									//if bit number is 4
		{
			n_bit = 0;									//reset bit number
			bus_byte = (bus_byte >> 1);							//correction of shifting
			*(R_BUS + n_byte)=bus_byte;						//put the byte in table
			bus_byte = 0;								//reset byte
			n_byte++;								//inc byte number
			if (n_byte == 16)								//if byte number is 16
			{
				n_byte = 0;							//reset byte number
				received = 1;							//set M-Bus receive flag
				bus_busy = 0;							//reset flag "bus is busy"
			}
		}
 	GIMSK &= 0xbf;										//INT0 interrupt disable
 	MCUCR = 0x03;										//INT0 rising edge sensitive
	GIMSK |= 0x40;										//INT0 interrupt enable
	}
}

//*********************************************************************************
SIGNAL (SIG_OVERFLOW0)									//T0 overflow interrupt handler
{
	TCCR0 = 0x00;										//stop T0
	n_bit = 0;											//reset bit number
	n_byte = 0;										//reset byte number
	bus_byte = 0;										//reset byte
	received = 1;										//set M-Bus receive flag
	bus_busy = 0;										//reset flag "bus is busy"
}

//*********************************************************************************
void send(unsigned char nmbr)									//send to changer
{
	unsigned char n;
	unsigned char m;
	unsigned char s_temp;

	while (bus_busy>0);									//wait if there is receiving
	GIMSK &= 0xbf;										//INT0 interrupt disable
	TIMSK = 0x20;										//T1 compare 1B interrupt enable (T0 disable)

												//PWM settings (use OC1B)
	TCCR1A = 0x21;										//COM1B1=H, COM1B0=L, PWM1B=H
	TCCR1B = 0x80;										//CTC1=H, T1-stoped
	OCR1C = 187;										//upper limit T1 (3000us)
	for(m=0;m<nmbr;m++)
	{
		s_temp=*(S_BUS+m);								//copy byte to temp
		for(n=4;n>0;n--)									//4 times - 4 bits
		{
												//put the value to compare register
			if(s_temp&0x08) OCR1B = 112;						//if bit is 1 (1800us)
			else 			OCR1B = 37; 					//if bit is 0 (600us)
			TCCR1B = 0x85;          							//start T1 with prescaler clk/16
			s_temp = (s_temp << 1);							//shift temp left 1 time
			while (s_flag==0);
			s_flag = 0;								//reset flag

		}
	}
	TCCR1B = 0x80;										//CTC1=H, T1-stoped
	TIMSK = 0x02;										//T0 overflow interrupt enable
	MCUCR = 0x03;										//INT0 rising edge sensitive
	GIMSK |= 0x40;										//INT0 interrupt enable
	s_flag = 0;										//reset flag
	bus_busy=0;										//reset flag "bus is busy"
}

//*********************************************************************************
SIGNAL (SIG_OUTPUT_COMPARE1B)           								//T1 compare 1B interrupt handler
{
	s_flag = 1;										//set flag
}

//*********************************************************************************
void init(void)											//initialization
{
	DDRA = 0xff;								//DB7,  DB6,  DB5,  DB4,  RS,   S2,   S1,   E
												//PA7   PA6   PA5   PA4   PA3   PA2   PA1   PA0
												// O     O     O     O     O     O     O     O
												// 1     1     1     1     1     1     1     1

	PORTA = 0xfe;										// 1     1     1     1     1     1     1     0

	DDRB = 0x0e;										//RES,  INT0, SW'S, NC,   OC1B, SCK,  MISO, MOSI
												//PB7   PB6   PB5   PB4   PB3   PB2   PB1   PB0
												// I     I     I     I     O     O     O     I
												// 0     0     0     0     1     1     1     0

	PORTB = 0x42;										// 0     1     0     0     0     0     1     0

	TIMSK |= 0x02;										//T0 overflow interrupt enable
	MCUCR = 0x03;										//INT0 rising edge sensitive
	GIMSK |= 0x40;										//INT0 interrupt enable
	sei();	 										//global interrupt enable
}

//*********************************************************************************
void time_loop(int tenmicros)										//time loop => one loop = 10 us
{
	for(;tenmicros>0;tenmicros--)
	{
		asm volatile ("push r1""\n\t"
				"pop r1""\n\t");
	}
}
//*********************************************************************************
//************************************ the end ************************************
