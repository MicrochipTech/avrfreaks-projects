/*
	WWVB.c - Simulate WWVB
	Written by Frank Kienast in November, 2007

	Target is ATMega16 with 12MHz xtal clock.
	Modulated 60kHz carrier is output on PD7 and can be sent to clock being tested
	using a loop of wire with a series 330 ohm resistor (and possibly a LED so can
	monitor the output).

	All WWVB bits can be defined in program.  Note that time and date values will
	increment appropriately each minute, but of course flags such as DST and UTC 
	correction do not change from their start values.
*/

#include <avr/io.h>
#include <avr/interrupt.h>



/* Define the starting WWVB parameters */

//2-digit year number
uint8_t year = 07;

//Day number in year 0 - 366
uint16_t day = 213;

//UTC hour 0 - 23 
uint8_t hour = 13;

//UTC minute 0 - 59
uint8_t minute;

//UT1 correction, tenths of a second 0 - 9
uint8_t ut1corr = 1;

//UT1 correction sign, 5 is +, 2 is -
uint8_t ut1sign = 5;

//Leap year / second
uint8_t leap = 0;

//DST flag, 0=no, 1=ended today, 2 = started today, 3=yes
uint8_t dst = 3;



//Forward declarations
int main();
void init_timer2();
void init_timer1();
void carrier_on();
void carrier_off();
void set_constants();
void set_year_and_day();
void set_hour_and_minute();
void set_flags();
void process_second(uint8_t second);
void set_bcd(uint8_t,uint8_t, uint8_t);



//Keeps track of location in current minute
volatile uint8_t g_seconds = 0;
volatile uint8_t g_centiseconds = 0;

//Internally computed - number of tenths of a second no carrier each second
uint8_t value[60]; //Number of hundredths of a second no carrier each second




int main()
{
	uint8_t second;

	init_timer2();
	carrier_on();

	set_constants();
	set_year_and_day();
	set_hour_and_minute();
	set_flags();

	init_timer1();


	while(1)
	{
		for(second = 0; second < 60; second++)
			process_second(second);

		minute++;
		if(minute > 59)
		{
			minute = 0;
			hour++;
			if(hour > 23)
			{
				hour = 0;
				day++;
				if((day > 365 && (year % 4) != 0)
				|| (day > 366 && (year % 4) == 0))
				{
					day = 1;
					year++;
					if(year > 99)
						year = 0;
				}
				set_year_and_day();
			}
		}
		set_hour_and_minute();


		//Wait until start of next minute
		while(g_seconds >= 59);
	}

}

void init_timer2()
{
	//Timer2 creates the 60kHz carrier

	//No PWM, clear on match, no prescaler 
	TCCR2 = _BV(WGM21) | _BV(CS20);

	// 12MHz / 2 / 100 = 60kHz
	OCR2 = 100 - 1; 

	// D7 is output for OC2
	DDRD = _BV(PD7); 
 
}

void init_timer1()
{
	//Timer1 generates interrupt every 1ms to update g_milliseconds

	OCR1A = 12000 - 1; // For overflow every 1ms
	TCCR1B = _BV(WGM12) | _BV(CS10); // Clear on match; Use unscaled clock as input
	TIMSK = _BV(OCIE1A); // Interrupt on timer 1A match

	sei(); // Global interrupt enable
}

ISR(TIMER1_COMPA_vect)
{
	/*
	fgk 2007-11-16
	It would seem logical to just keep one global time value in milliseconds.  But
	when do this, encounter a race condition due to multi-byte value being updated
	by interrupt while it is being read.  By using single-byte values (seconds and 
	centiseconds), no multi-byte values to change during read.
	*/

	static volatile uint8_t milliseconds = 0;

	milliseconds++;
	if(milliseconds >= 10) 
	{
		g_centiseconds++;
		milliseconds = 0;
		if(g_centiseconds >= 100)
		{
			g_seconds++;
			g_centiseconds = 0;
			if(g_seconds >= 60)
				g_seconds = 0;
		}	
	}
}

void carrier_on()
{
	// Start 60kHz carrier on PD7
	TCCR2 |= _BV(COM20);
}

void carrier_off()
{
	// Stop 60 kHz carrier on PD7
	TCCR2 &= (255 - _BV(COM20));
}

void set_constants()
{
	//Set the second values that are always the same
	value[0] = 80;
	value[4] = 20;
	value[9] = 80;
	value[10] = 20;
	value[11] = 20;
	value[14] = 20;
	value[19] = 80;
	value[20] = 20;
	value[21] = 20;
	value[24] = 20;
	value[29] = 80;
	value[34] = 20;
	value[35] = 20;
	value[39] = 80;
	value[44] = 20;
	value[49] = 80;
	value[54] = 20;
	value[59] = 80;
}

void set_year_and_day()
{
	uint16_t temp;
	uint8_t oneyear, tenyear, oneday, tenday, hundredday;

	temp = year;
	oneyear = temp % 10; set_bcd(oneyear,53,4);
	temp /= 10;
	tenyear = temp % 10; set_bcd(tenyear,48,4);

	temp = day;
	oneday = temp % 10; set_bcd(oneday,33,4);
	temp /= 10;
	tenday = temp % 10; set_bcd(tenday,28,4);
	temp /= 10;
	hundredday = temp % 10; set_bcd(hundredday,23,2);
}

void set_hour_and_minute()
{
	uint8_t temp;
	uint8_t onehour, tenhour, oneminute, tenminute;

	temp = hour;
	onehour = temp % 10; set_bcd(onehour,18,4);
	temp /= 10;
	tenhour = temp % 10; set_bcd(tenhour,13,2);

	temp = minute;
	oneminute = temp % 10; set_bcd(oneminute,8,4);
	temp /= 10;
	tenminute = temp % 10; set_bcd(tenminute,3,3);
}

void set_flags()
{
	set_bcd(ut1corr,43,4);
	set_bcd(ut1sign,38,3);
	set_bcd(leap,56,2);
	set_bcd(dst,58,3);
}

void set_bcd(uint8_t val, uint8_t lsb, uint8_t bits)
{
	uint8_t i;
	uint8_t bitval;

	for(i = 0; i < bits; i++)
	{
		bitval = val % 2;
		value[lsb - i] = (bitval == 0 ? 20 : 50);
		val /= 2;
	} 

}

void process_second(uint8_t second)
{
	//Wait until start of second, then turn off carrier
	while(g_seconds < second);
	carrier_off();

	//Wait until time, then turn carrier back on
	while(g_centiseconds < value[second]);
	carrier_on();
}





