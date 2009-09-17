
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
	
#define F_CPU 	1000000;			// 1MHz

volatile unsigned long RPM=0;		// Global RPM variable

void initLCD(void);
void lcd_write_byte(unsigned int CONTROL, unsigned int DATA);
void updateLCD(unsigned long RPM_OLD);
void check_BF(void);

void main(void)
{	
	TCCR1B=(1<<CS10)|(1<<CS12);		// Set up prescaler for CLK/1024
	GIMSK=(1<<INT0);				// Enable external interrupt INT0
	MCUCR=(1<<ISC00)|(1<<ISC01);	// Setup INT0 for rising edge detection

	unsigned long RPM_OLD=1;		// Internal loop RPM variable

	DDRB=0x00;						// Set PB2 for input

	initLCD();						// Initialize the LCD

	sei();							// Enable interrupts

	while(1)
	{
		if((RPM != RPM_OLD)	& (TCNT1 < 4000))		 
		{							
			RPM_OLD=RPM;			// If new updated RPM value is not the same
			updateLCD(RPM_OLD);		// as the one inside the loop, update the LCD.			
		}
		
		if(TCNT1 >= 4000)			// If it's been longer than 4 seconds, RPM=0.
		{	
			RPM=0;
			if (RPM != RPM_OLD)
			{
				RPM_OLD=RPM;		// If new updated RPM value is not the same
				updateLCD(RPM_OLD);	// as the one inside the loop, update the LCD.
			}
		}
	}
}


ISR(EXT_INT0_vect)					// External interrupt on PB2
{
	unsigned long ELAPSED;
		
	if(TCNT1 < 4000)				// If Timer1 value is valid:
	{
		ELAPSED=TCNT1;				// Get Timer1 value before it changes much
		TCNT1=0;					// Reset Timer1
		RPM=(58594/ELAPSED);		// Calculate RPM based on Timer1 elapsed time
	}

	if(TCNT1 >= 4000)				// If the timer overflowed, Timer1 value is no good.
	{
		TCNT1=0;					// Reset Timer1
	}
}


void initLCD()
{
	DDRA=0xFF;						// Set PortA for output
	_delay_ms(250);					// Wait for HD44780
	PORTA=0x04;						// Intialize to 4 Bit, one line
	PORTA |= 0x10;
	_delay_ms(1);
	PORTA &= ~0x10;
	_delay_ms(10);					// Intialize to 4 Bit, one line
	PORTA |= 0x10;	
	_delay_ms(1);				
	PORTA &= ~0x10;
	_delay_ms(10);					// Intialize to 4 Bit, one line
	PORTA |= 0x10;
	_delay_ms(1);
	PORTA &= ~0x10;
	_delay_ms(10);
	lcd_write_byte(0,0x0C);			// Turn on display 
	lcd_write_byte(0,0x06);			// Increment by one after write

}


void lcd_write_byte(unsigned int CONTROL, unsigned int DATA)
{
	// check_BF();

	if(CONTROL == 1) PORTA |= 0x20; else PORTA &= ~0x20;
	if((DATA & 0x80) == 0x80) PORTA |= 0x01; else PORTA &= ~0x01;
	if((DATA & 0x40) == 0x40) PORTA |= 0x02; else PORTA &= ~0x02;
	if((DATA & 0x20) == 0x20) PORTA |= 0x04; else PORTA &= ~0x04;
	if((DATA & 0x10) == 0x10) PORTA |= 0x08; else PORTA &= ~0x08;
	PORTA |= 0x10;
	_delay_us(1);
	PORTA &= ~0x10;

	if((DATA & 0x08) == 0x08) PORTA |= 0x01; else PORTA &= ~0x01;
	if((DATA & 0x04) == 0x04) PORTA |= 0x02; else PORTA &= ~0x02;
	if((DATA & 0x02) == 0x02) PORTA |= 0x04; else PORTA &= ~0x04;
	if((DATA & 0x01) == 0x01) PORTA |= 0x08; else PORTA &= ~0x08;
	PORTA |= 0x10;
	_delay_us(1);
	PORTA &= ~0x10;
	_delay_ms(4);
}


void updateLCD(unsigned long RPM_OLD)
{
	unsigned long RPM_BCD = ((((RPM_OLD/10)+((RPM_OLD/100)*6))*16)+(RPM_OLD%10));
	
	unsigned int ONES = 0x00;
	unsigned int TENS = 0x00;
	unsigned int HUND = 0x00;
	
	if((RPM_BCD & 0x0800) == 0x0800) HUND |= 0x08; else HUND &= ~0x08;
	if((RPM_BCD & 0x0400) == 0x0400) HUND |= 0x04; else HUND &= ~0x04;
	if((RPM_BCD & 0x0200) == 0x0200) HUND |= 0x02; else HUND &= ~0x02;
	if((RPM_BCD & 0x0100) == 0x0100) HUND |= 0x01; else HUND &= ~0x01;

	if((RPM_BCD & 0x0080) == 0x0080) TENS |= 0x08; else TENS &= ~0x08;
	if((RPM_BCD & 0x0040) == 0x0040) TENS |= 0x04; else TENS &= ~0x04;
	if((RPM_BCD & 0x0020) == 0x0020) TENS |= 0x02; else TENS &= ~0x02;
	if((RPM_BCD & 0x0010) == 0x0010) TENS |= 0x01; else TENS &= ~0x01;

	if((RPM_BCD & 0x0008) == 0x0008) ONES |= 0x08; else ONES &= ~0x08;
	if((RPM_BCD & 0x0004) == 0x0004) ONES |= 0x04; else ONES &= ~0x04;
	if((RPM_BCD & 0x0002) == 0x0002) ONES |= 0x02; else ONES &= ~0x02;
	if((RPM_BCD & 0x0001) == 0x0001) ONES |= 0x01; else ONES &= ~0x01;

	ONES |= 0x30;
	TENS |= 0x30;
	HUND |= 0x30;

	lcd_write_byte(0x00, 0x01);
	lcd_write_byte(0x01, 0x20);
	lcd_write_byte(0x01, HUND);
	lcd_write_byte(0x01, TENS);
	lcd_write_byte(0x01, ONES);
	lcd_write_byte(0x01, 0x20);
	lcd_write_byte(0x01, 0x52);
	lcd_write_byte(0x01, 0x50);
	lcd_write_byte(0x01, 0x4D);

}

void check_BF(void)
{
	DDRA = 0xF0;					// Set PortA 4-7 (R/W, RS, E) to output and 0-3 (DBx) for input
	PORTA &= ~0x20;					// Clear register select
	PORTA |= 0x40;					// Set read 
	PORTA |= 0x10;					// Set enable 
	_delay_us(1);

	while((PA0 & 0x01) == 0x01)		// Read DB7 of LCD (busy flag)
	{
		PORTA &= ~0x10;				// Clear enable
		_delay_us(1);	
		PORTA |= 0x10;				// Set enable
		_delay_us(1);
		PORTA &= ~0x10;				// Clear enable
		_delay_us(1);	
		PORTA |= 0x10;				// Set enable
		_delay_us(1);
	}

	PORTA &= ~0x10;					// Clear enable
	_delay_us(1);
	PORTA |= 0x10;					// Set enable
	_delay_us(1);
	PORTA &= ~0x10;					// Clear enable
	PORTA &= ~0x40;					// Clear read
	DDRA = 0xFF;					// Set for output

}
