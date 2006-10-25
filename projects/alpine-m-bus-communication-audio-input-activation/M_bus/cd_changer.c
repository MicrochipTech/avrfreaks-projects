//Compiler = WinAVR - Win32 build of avr-gcc 3.3 (Sept.13,2003)
//File = CD_changer.c

//CD changer controller program. The controller consist of ATtiny26, LCD display 2x16 (HD44780)
//(4-bit interface) and 7-key keyboard. CD changer Sherwood connected through Alpine M-bus (1-wire,
//2- directions).

//************************************* #include **********************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <avr/eeprom.h>

//************************************* #define ***********************************
#define EEPROM_SECTION  __attribute__ ((section (".eeprom")))

//************************************ prototypes *********************************
void init(void);
void init_lcd(void);
void out_instr(unsigned char value);
void out_data(unsigned char value);
void out_lcd(unsigned char value);
void time_loop(int tenmicros);
void out_1_lcd(void);
void out_2_lcd(void);
unsigned char read_keyboard(void);
void send(unsigned char nmbr);
void count_crc (unsigned char x);
void copy_mm_ss(void);
void ping(void);
void copy_t(void);
void copy_cmd_ch(void);
void chk_disk(void);

//************************************ variables **********************************
unsigned char value;
unsigned char temp;
unsigned char key;
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

char FSTLINE[16]={'C','D',' ','C','h','a','n','g','e','r',' ',' ','v','.','0','1'};
char SNDLINE[16]={' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
char R_BUS[16]={};
char S_BUS[9]={};

prog_char CH_D0[8] = {1,1,3,0,0,0,0,0};
prog_char PLAY[6] = {1,1,1,0,1,1};
prog_char STOP[6] = {1,1,1,4,0,6};
prog_char PAUSE[6] = {1,1,1,0,2,4};

char PATERN1[16]   EEPROM_SECTION = {'D','i','s','k',':',' ',' ',' ','T','r','a','c','k',':',' ',' '};
char PATERN2_1[16] EEPROM_SECTION = {'E','m','p','t','y',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
char PATERN2_2[16] EEPROM_SECTION = {'N','o','t',' ','c','o','n','n','e','c','t','e','d',' ',' ',' '};
char PATERN2_3[16] EEPROM_SECTION = {'C','h','a','n','g','i','n','g',' ',' ',' ',' ',' ',' ',' ',' '};
char PATERN2_4[16] EEPROM_SECTION = {'P','l','a','y','i','n','g',' ',' ',' ',' ',' ',' ',':',' ',' '};
char PATERN2_5[16] EEPROM_SECTION = {'P','a','u','s','e','d',' ',' ',' ',' ',' ',' ',' ',':',' ',' '};
char PATERN2_6[16] EEPROM_SECTION = {'S','t','o','p','p','e','d',' ',' ',' ',' ',' ',' ',':',' ',' '};

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
				copy_cmd_ch();							//copy command CHANGE
				*(S_BUS+3)=(curr_disk -1);						//copy and dec number of current disk
				count_crc (7);							//crc
				send(8);								//send command to CD_changer
			break;
			case 2:									//- - - NEXT DISK
				copy_cmd_ch();							//copy command CHANGE
				*(S_BUS+3)=(curr_disk +1);						//copy and inc number of current disk
				count_crc (7);							//crc
				send(8);								//send command to CD_changer
			break;
			case 3:									//- - - PREV. TRACK
				copy_cmd_ch();							//copy command CHANGE
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
				copy_cmd_ch();							//copy command CHANGE
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
			out_2_lcd();								//output second line			 	
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
void chk_disk(void)											//check number of current disk, and
{												//compare with displayed
	if (*(R_BUS+5)>((*(FSTLINE+5)-48))) disk_up=1;							//if current > dispayed, set disk_up
	if (*(R_BUS+5)<((*(FSTLINE+5)-48))) disk_down=1;						//if current < dispayed, set disk_down
}

//*********************************************************************************
void copy_mm_ss(void)										//copy minutes and seconds to the LCD buffer
{
	*(SNDLINE+11)=(*(R_BUS+7)+48);								//copy and convert to digit - minutes
	*(SNDLINE+12)=(*(R_BUS+8)+48);
	*(SNDLINE+14)=(*(R_BUS+9)+48);
	*(SNDLINE+15)=(*(R_BUS+10)+48);								//copy and convert to digit - seconds
}

//*********************************************************************************
void copy_t(void)											//copy track number to the LCD buffer
{
	*(FSTLINE+14)=(*(R_BUS+3)+48);
	*(FSTLINE+15)=(*(R_BUS+4)+48);								//copy and convert to digit - track
}

//*********************************************************************************
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
	DDRA = 0xff;										//DB7,  DB6,  DB5,  DB4,  RS,   S2,   S1,   E
												//PA7   PA6   PA5   PA4   PA3   PA2   PA1   PA0
												// O     O     O     O     O     O     O     O
												// 1     1     1     1     1     1     1     1
										
	PORTA = 0xfe;										// 1     1     1     1     1     1     1     0
	
	DDRB = 0x0e;										//RES,  INT0, SW'S, NC,   OC1B, SCK,  MISO, MOSI
												//PB7   PB6   PB5   PB4   PB3   PB2   PB1   PB0
												// I     I     I     I     O     O     O     I
												// 0     0     0     0     1     1     1     0
	
	PORTB = 0x42;										// 0     1     0     0     0     0     1     0
	
	init_lcd();											//lcd 4-bit interface initialize
	out_1_lcd();										//output first line
	out_2_lcd();										//output second line
	
	TIMSK |= 0x02;										//T0 overflow interrupt enable
	MCUCR = 0x03;										//INT0 rising edge sensitive
	GIMSK |= 0x40;										//INT0 interrupt enable
	sei();	 										//global interrupt enable
}

//*********************************************************************************
void init_lcd(void)											//lcd initialization
{
	time_loop(4000);										//40000us = 40ms
	PORTA &= 0xf6;										//RS=0, E=0
	time_loop(200);										//2000us = 2ms
	PORTA = 0x36;										//D7=0, D6=0, D5=1, D4=1
	asm volatile ("sbi 0x1b,0" "\n\t");								//E=1
	asm volatile ("cbi 0x1b,0" "\n\t");								//E=0
	time_loop(500);										//5000us = 5ms
	asm volatile ("sbi 0x1b,0" "\n\t");								//E=1
	asm volatile ("cbi 0x1b,0" "\n\t");								//E=0
	time_loop(200);										//2000us = 2ms
	asm volatile ("sbi 0x1b,0" "\n\t");								//E=1
	asm volatile ("cbi 0x1b,0" "\n\t");								//E=0
	time_loop(200);										//2000us = 2ms
	PORTA &= 0xef;										//D4=0
	asm volatile ("sbi 0x1b,0" "\n\t");								//E=1
	asm volatile ("cbi 0x1b,0" "\n\t");								//E=0
	time_loop(200);										//2000us = 2ms
	out_instr(0x28);										//function set (DL=0[4-bit], N=1[2-lines], F=0[5x8])
	out_instr(0x0c);										//display on/off (D=1[dis-on],C=0[curs-off],B=0[blink-off])
	out_instr(0x06);										//entry mode set (I/D=1[incr], S=0[shift])
	out_instr(0x01);										//cls lcd
	time_loop(200);										//2000us = 2ms
}

//*********************************************************************************
void out_instr(unsigned char value)									//lcd instruction output
{
	PORTA &= 0xf7;										//RS=0
	out_lcd(value);										//go on
}

//*********************************************************************************
void out_data(unsigned char value)									//lcd data output
{
	PORTA |= 0x08;										//RS=1
	out_lcd(value);										//go on
}

//*********************************************************************************
void out_lcd(unsigned char value)									//output into lcd
{
	unsigned char tempor;
	tempor = value;										//copy
	value &= 0xf0;										//value=MSB nibble
	PORTA &= 0x0f;										//D7=0, D6=0, D5=0, D4=0
	PORTA |= value;										//output MSB
	asm volatile ("sbi 0x1b,0" "\n\t");								//E=1
	asm volatile ("cbi 0x1b,0" "\n\t");								//E=0
	value = tempor;										//restore
	value &= 0x0f;										//value=LSB nibble
	value = (value<<4);										//shift left
	PORTA &= 0x0f;										//D7=0, D6=0, D5=0, D4=0
	PORTA |= value;										//output LSB
	asm volatile ("sbi 0x1b,0" "\n\t");								//E=1
	asm volatile ("cbi 0x1b,0" "\n\t");								//E=0
	time_loop(20);										//200us
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
void out_1_lcd(void)										//output first line to the lcd
{
	unsigned char n;
	out_instr(0x80);										//DDRAM = 00 - first line
	for(n=0;n<16;n++)
	{
		out_data(*(FSTLINE+n));								//output data
	}
}

//*********************************************************************************
void out_2_lcd(void)										//output second line to the lcd
{
	unsigned char n;
	out_instr(0xc0);										//DDRAM = 40 - second line
	for(n=0;n<16;n++)
	{
		out_data(*(SNDLINE+n));								//output data
	}
}

//*********************************************************************************
unsigned char read_keyboard(void)									//check keyboard and return code of pressed key
{
	DDRA = 0x01;										//DB7,  DB6,  DB5,  DB4,  RS,   S2,   S1,   E
												//PA7   PA6   PA5   PA4   PA3   PA2   PA1   PA0
												// I     I     I     I     I     I     I     O
												// 0     0     0     0     0     0     0     1
	PORTA = 0xfe;										// 1     1     1     1     1     1     1     0
	
	DDRB = 0x2e;										//RES,  INT0, SW'S, NC,   OC1B, SCK,  MISO, MOSI
												//PB7   PB6   PB5   PB4   PB3   PB2   PB1   PB0
												// I     I     O     I     O     O     O     I
												// 0     0     1     0     1     1     1     0
	PORTB &= 0xdf;										//PB5=0
	
	asm volatile ("push r1""\n\t"
			"pop r1""\n\t");								//time for synchronization
	if (key != 1)										//if none key was hold
	{
		temp = PINA;									//read
		if (temp != 0xfe)									//if some key is pressed
		{
			time_loop(1000);								//10000us = 10ms
			if (temp != PINA)								//if after time situation changed
			{
				temp = 0xff;							//set temp to ff
			}
			else key = 1;								//memory of pressed key
		}
		else
		{
			temp = 0xff;								//set temp to ff
		}
		switch (temp)
		{
			case 0xfc :								//SW1 pressed
				temp = 1;
				break;
			case 0xfa :								//SW2 pressed
				temp = 2;
				break;
			case 0xf6 :								//SW3 pressed
				temp = 3;
				break;
			case 0xee :								//SW4 pressed
				temp = 4;
				break;
			case 0xde :								//SW5 pressed
				temp = 5;
				break;
			case 0xbe :								//SW6 pressed
				temp = 6;
				break;
			case 0x7e :								//SW7 pressed
				temp = 7;
				break;
			case 0xff :								//none SW pressed
				temp = 0;
				break;
			default :
				temp = 0;
				break;
		}
	}
	else											//if some key is hold
	{
		if (PINA == 0xfe)									//if key is released
		{
			key = 0;									//reset memory of pressed key
		}
			temp = 0;								//reset temp
	}
	DDRA = 0xff;										//DB7,  DB6,  DB5,  DB4,  RS,   S2,   S1,   E
												//PA7   PA6   PA5   PA4   PA3   PA2   PA1   PA0
												// O     O     O     O     O     O     O     O
												// 1     1     1     1     1     1     1     1

	DDRB = 0x0e;										//RES,  INT0, SW'S, NC,   OC1B, SCK,  MISO, MOSI
												//PB7   PB6   PB5   PB4   PB3   PB2   PB1   PB0
												// I     I     I     I     O     O     O     I
												// 0     0     0     0     1     1     1     0

	time_loop(1000);										//10 000us = 10ms
	return temp;
}

//*********************************************************************************
//************************************ the end ************************************
