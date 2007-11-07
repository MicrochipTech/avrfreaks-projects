/***************************

MSF Clock

Written by Ken J Jewell

Compiled with AVR IAR

******************************/

#pragma language=extended // enable use of extended keywords

#ifndef ENABLE_BIT_DEFINITIONS // enable the bit definitions in the iom32.h file
#define ENABLE_BIT_DEFINITIONS
#endif

#include <inavr.h>
#include <ioavr.h>

// day of week calculation
static unsigned char __flash DAYINMONTH[12] = { 
	6, 2, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4
};

// max days in a month (non-leap year)
static unsigned char __flash MAX_DAY[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

// week day names
static unsigned char __flash DAYS[7][3] = {
"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};

// macros
#define	BIN2BCD(b) (((b)%10) | (((b)/10)<<4)) /* Binary 2 BCD conversion */
#define	BCD2BIN(b) (((b)&0xf) + ((b)>>4)*10)  /* BCD 2 Binary conversion */

// PORT A
#define	RS			0x02;
#define	RW			0x04;
#define	E				0x08;
#define	DB4			0x10;
#define	DB5			0x20;
#define	DB6			0x40;
#define	DB7			0x80;

// PORT D
#define	MSF60Hz	0x04;
#define	EM2SPWR	0x20;
#define	RTCLED	0x40;
#define	MSFLED	0x80;

// function prototypes
int main(void);
void printDate(unsigned char year, unsigned char month, unsigned char date, unsigned char day);
void printTime(unsigned char offset, unsigned char hour, unsigned char minute, unsigned char second);
void clockLCD(unsigned char data);
void writeString(unsigned char *string);
void writeASCII(unsigned char data);
void writeLCD(unsigned char data);
unsigned char readLCD(void);
unsigned char MaxDay(unsigned char year, unsigned char month);
unsigned char DayOfWeek(unsigned char year, unsigned char month, unsigned char date);
void delay_ms(unsigned int x);
void delay_1us(void);
void delay_us(unsigned int x);
unsigned char ParityCheck(unsigned char argv1, unsigned char argv2, unsigned char bit);

// registers
unsigned char msf_hour_offset, rtc_hour_offset;
unsigned char msf_hour, rtc_hour;
unsigned char msf_minute, rtc_minute;
unsigned char msf_second, rtc_second;

unsigned char msf_year, rtc_year;
unsigned char msf_month, rtc_month;
unsigned char msf_date, rtc_date;
unsigned char msf_day, rtc_day;

unsigned char msf_year_parity;
unsigned char msf_month_date_parity;
unsigned char msf_day_parity;
unsigned char msf_hour_minute_parity;

unsigned char rtc_second_shadow;
unsigned char rtc_date_shadow;

unsigned char MSFbitA, MSFbitB, MSFbitLock;
unsigned int MSFcount;

int main(void)
{
	PORTA = 0xFF;
	DDRA = 0xFF;
	
	PORTD = 0xFF;
	DDRD = 0xFB;
	
	ASSR = (1<<AS2);
	
	TCCR2 = (1<<CS22) | (1<<CS20);
	
  MCUCSR = (1<<JTD);
  GICR = (1<<INT0); 
  MCUCR = (1<< ISC00);

  TCCR1B = (1<<WGM12) | (1<<CS11); // timer1: ctc, clk/8
  TIMSK = (1<<OCIE1A) | (1<<TOIE2); // timer0: overflow, timer1: output_compare a
  
  OCR1A = 1000;	// 1ms

  __enable_interrupt();
	
	delay_ms(100);		// wait > 15ms
		
	clockLCD(0x38);
	delay_ms(5);
	clockLCD(0x38);
	delay_us(150);
	clockLCD(0x38);
	delay_us(100);

	clockLCD(0x28);
	delay_us(100);
	
	writeLCD(0x28);	// function set DL: 4bit, N: 2line, F: 5x7
	
	writeLCD(0x08);	// display off
	
	writeLCD(0x01);	// clear display
	
	writeLCD(0x06);	// entry mode set I/D: 1, S: 0
		
	writeLCD(0x0C);	// display on, C: off, B: off

	rtc_hour_offset = 0;
	rtc_hour = 12;
	rtc_minute = 1;
	rtc_second = 0;
	
	rtc_year = 0;
	rtc_month = 1;
	rtc_date = 1;
	rtc_day = DayOfWeek(rtc_year, rtc_month, rtc_date);
	
	rtc_date_shadow = 0xFF;
	rtc_second_shadow = 0xFF;
				
	for (;;)
	{
		if (rtc_second != rtc_second_shadow)
		{
			rtc_second_shadow = rtc_second;	
			printTime(rtc_hour_offset, rtc_hour, rtc_minute, rtc_second);
		}
		
		if (rtc_date != rtc_date_shadow)
		{
			rtc_date_shadow = rtc_date;
			printDate(rtc_year, rtc_month, rtc_date, rtc_day);
		}
	}
}

void printDate(unsigned char year, unsigned char month, unsigned char date, unsigned char day)
{
	unsigned char temp;
	unsigned char i;
	
	writeLCD(0xC0);	// line 2, position 0
	
	for(i=0; i<3; i++)
	{
		writeASCII(DAYS[day][i]);	// display week day name
	}

	writeASCII(' ');
	
	temp = BIN2BCD(date);
	writeASCII(((temp & 0xF0) >> 4) + 0x30);
	writeASCII((temp & 0x0F) + 0x30);	// display date

	writeASCII('/');

	temp = BIN2BCD(month);
	writeASCII(((temp & 0xF0) >> 4) + 0x30);
	writeASCII((temp & 0x0F) + 0x30);	// display date

	writeASCII('/');
	
	temp = BIN2BCD(year);
	writeASCII(((temp & 0xF0) >> 4) + 0x30);
	writeASCII((temp & 0x0F) + 0x30);	// display year
}

void printTime(unsigned char offset, unsigned char hour, unsigned char minute, unsigned char second)
{
	unsigned char temp;
	
	writeLCD(0x80);	// line 1, position 0
	
	temp = BIN2BCD(hour);
	writeASCII(((temp & 0xF0) >> 4) + 0x30);
	writeASCII((temp & 0x0F) + 0x30);	// display hours

	writeASCII(':');

	temp = BIN2BCD(minute);
	writeASCII(((temp & 0xF0) >> 4) + 0x30);
	writeASCII((temp & 0x0F) + 0x30);	// display minute

	writeASCII(':');
	
	temp = BIN2BCD(second);
	writeASCII(((temp & 0xF0) >> 4) + 0x30);
	writeASCII((temp & 0x0F) + 0x30);	// display second
	
	writeASCII(' ');
	writeASCII('+');
	writeASCII(offset + 0x30);
}

void clockLCD(unsigned char data)
{
	PORTA = data;

	delay_1us();
	PORTA &= ~E;
	delay_1us();
	PORTA |= E;
	delay_1us();
}

void writeString(unsigned char *string)
{
	while (*string)
	{
 	  writeASCII(*string++);
	}
}

void writeASCII(unsigned char data)
{
	unsigned char temp;
	
	temp = data & 0xF0;
	temp |= RS;
	temp &= ~RW;
	temp |= E;
	clockLCD(temp);	// write high nibble

	temp = (data & 0x0F) << 4;
	temp |= RS;
	temp &= ~RW;
	temp |= E;
	clockLCD(temp);	// write low nibble
	
	while (readLCD() & 0x80)	// wait for busy flag
		;
}

void writeLCD(unsigned char data)
{
	unsigned char temp;
	
	temp = data & 0xF0;
	temp &= ~RS;
	temp &= ~RW;
	temp |= E;
	clockLCD(temp);	// write high nibble

	temp = (data & 0x0F) << 4;
	temp &= ~RS;
	temp &= ~RW;
	temp |= E;
	clockLCD(temp);	// write low nibble
	
	while (readLCD() & 0x80)	// wait for busy flag
		;
}

unsigned char readLCD(void)
{
	unsigned char temp;
	DDRA = 0x0F;
	
	PORTA |= 0xF0;
	PORTA &= ~RS;
	PORTA |= RW;
	
	PORTA |= E;
	delay_1us();
	temp = PINA & 0xF0;	// read high nibble
	delay_1us();
	PORTA &= ~E;
	delay_1us();

	PORTA |= E;
	delay_1us();
	temp |= (PINA & 0xF0) >> 4;	// read low nibble
	delay_1us();
	PORTA &= ~E;
	delay_1us();
	
	DDRA = 0xFF;
		
	return temp;
}

// Returns the maximum day in month for a given month & year
unsigned char MaxDay(unsigned char year, unsigned char month)
{
	unsigned char lastday, leap;
	
	leap = year%4 == 0 && year%100 !=0 || year%400 == 0;
	lastday = MAX_DAY[month - 1];
	
	if ((leap > 0) && (month == 2))
		lastday ++;
	
	return lastday;
}

// Returns the weekday number for a given date Monday is 1 ... Sunday is 7
unsigned char DayOfWeek(unsigned char year, unsigned char month, unsigned char date)
{
  unsigned char weekday, leap; 
 	
 	leap = year%4 == 0 && year%100 !=0 || year%400 == 0;
	
	weekday = (year + (year/4));
	
	weekday += DAYINMONTH[month - 1];
	
	if (leap && ((month == 1) || (month == 2)))
		weekday --;
	
	weekday += date;
	
	while (weekday > 7)
	{
		weekday -= 7;
	}
  
  return weekday;
}
	
// delay x millisecond
void delay_ms(unsigned int x)
{
  delay_us(997); // The function call and setup will take exactly 3 us.
  // This while loop will take exactly 1 ms per turn. Run ms - 1 times.
  while (--x) 
  {
    delay_us(999);
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
  }
}

// delay 1 microsecond
void delay_1us(void)
{
  // call is 1 instruction
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  // return is 1 instruction
}
  
// delay x microsecond
void delay_us(unsigned int x) 
{
  x--;
  __no_operation();
  __no_operation();
    
  // This while loop will take exactly 1 us per turn.
  while (--x) 
  {
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __watchdog_reset(); // kick the dog
  }
}

unsigned char ParityCheck(unsigned char argv1, unsigned char argv2, unsigned char bit)
{
	unsigned char i;
	unsigned char mask_bit = 0x01;
	unsigned char bit1Count = 0;
		
	for (i=0; i<8; i++)
	{
		if ((argv1 & mask_bit) == 1)
		{
			bit1Count++;
		}

		if ((argv2 & mask_bit) == 1)
		{
			bit1Count++;
		}
		
		mask_bit <<= 1;	
	}
	
	bit1Count += bit;
	
	return bit1Count;
}

#pragma vector=INT0_vect
__interrupt void INT0_irqHandler(void)
{
	if (MSFbitLock == 1)
	{
		MSFbitLock = 0;
		MSFcount = 0;
	}
	
	if ((PIND & 0x04) == 0)
	{
		TCNT1 = 0;	// re-sync timer 1 (1ms)
	}
	else
	{
		if (MSFcount > 498)	// minute marker (500ms)
		{
			TCNT2 = 0;	// re-sync timer 2 (1s)
			
			msf_year = 0;
			msf_month = 0;
			msf_date = 0;
			msf_day = 0;			
			msf_hour_offset = 0;
			msf_hour = 0;
			msf_minute = 0;
			msf_second = 0;
			
			msf_year_parity = 0;
			msf_month_date_parity = 0;
			msf_day_parity = 0;
			msf_hour_minute_parity = 0;
		}
	}
}

#pragma vector=TIMER1_COMPA_vect
__interrupt void OCR1A_irqHandler(void)
{
	if (MSFcount < 0xFFFF)
		MSFcount++;
		
	if (MSFcount == 50)
	{
		MSFbitA = 1;
		MSFbitB = 1;
	}
	else if (MSFcount == 150)
	{
		if (PIND & 0x04)
			MSFbitA = 0;
	}
	else if (MSFcount == 250)
	{
		if (PIND & 0x04)
			MSFbitB = 0;
	}
	else if (MSFcount == 600)
	{
		PORTD ^= MSFLED;
		
		MSFbitLock = 1;

		if ((msf_second >= 17) && (msf_second <= 24))
		{
			msf_year = msf_year << 1;
			msf_year |= MSFbitA;
		}
		else if ((msf_second >= 25) && (msf_second <= 29))
		{
			msf_month = msf_month << 1;
			msf_month |= MSFbitA;
		}
		else if ((msf_second >= 30) && (msf_second <= 35))
		{
			msf_date = msf_date << 1;
			msf_date |= MSFbitA;
		}
		else if ((msf_second >= 36) && (msf_second <= 38))
		{
			msf_day = msf_day << 1;
			msf_day |= MSFbitA;
		}
		else if ((msf_second >= 39) && (msf_second <= 44))
		{
			msf_hour = msf_hour << 1;
			msf_hour |= MSFbitA;
		}
		else if ((msf_second >= 45) && (msf_second <= 51))
		{
			msf_minute = msf_minute << 1;
			msf_minute |= MSFbitA;
		}
		else if (msf_second == 54)
		{
			msf_year_parity = MSFbitB;
		}
		else if (msf_second == 55)
		{
			msf_month_date_parity = MSFbitB;
		}
		else if (msf_second == 56)
		{
			msf_day_parity = MSFbitB;
		}
		else if (msf_second == 57)
		{
			msf_hour_minute_parity = MSFbitB;
		}
		else if (msf_second == 58)
		{
			msf_hour_offset = MSFbitB;
		}
		else if (msf_second == 59)
		{
			msf_year = BCD2BIN(msf_year);
			
			if (ParityCheck(msf_year, 0, msf_year_parity == 1))
			{
				rtc_year = msf_year;
			}
						
			msf_month = BCD2BIN(msf_month);
			msf_date = BCD2BIN(msf_date);

			if (ParityCheck(msf_month, msf_date, msf_month_date_parity == 1))
			{
				rtc_month = msf_month;
				rtc_date = msf_date;
			}
			
			msf_day = BCD2BIN(msf_day);
			
			if (ParityCheck(msf_day, 0, msf_day_parity == 1))
			{	
				rtc_day = BCD2BIN(msf_day);
			}
			
			rtc_hour_offset = msf_hour_offset;
			
			msf_hour = BCD2BIN(msf_hour);
			msf_minute = BCD2BIN(msf_minute);
			
			if (ParityCheck(msf_hour, msf_minute, msf_year_parity == 1))
			{
				rtc_hour = msf_hour;
				rtc_minute = msf_minute;
			}
			
			rtc_second = msf_second;
		}
		
		msf_second++;
	}
}

#pragma vector=TIMER2_OVF_vect
__interrupt void TMR2_irqHandler(void)
{
	PORTD ^= RTCLED;
	
	if (++rtc_second > 59)
	{
		rtc_second = 0;
		
		if (++rtc_minute > 59)
		{
			rtc_minute = 0;
			
			if (++rtc_hour > 23)
			{
				rtc_hour = 0;	
		
				if (++rtc_date > MaxDay(rtc_year, rtc_month))
				{
					rtc_date = 1;
					
					if (++rtc_month > 12)
					{
						rtc_month = 1;
					
						if (++rtc_year > 99)
						{
							rtc_year = 0;
						}
					}
				}
			}
		}
	}						
}
