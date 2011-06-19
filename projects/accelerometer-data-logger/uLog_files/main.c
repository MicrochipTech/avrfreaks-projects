/*
    uLog
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#define FOSC 8000000// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

#define SCK 3	//PA3
#define SI 1	//PB1
#define SO 7	//PA7
#define CS 2	//PB2

#define STAT 0	//PB0


//Define functions
//======================
void delay_ms(short x);
void delay_us(uint8_t x);

void write_buffer(void);
char read_flash(short page);
void erase(void);
void buffer_to_main(short block);
char status_read(void);
void send_command(char command);
void flash_clk(void);

char getc(void);
void putchr(char x);


void menu(void);
void print_num(short num);



//======================
//Global variables
//=========================
volatile short nasty[33];
volatile char nasty_write = 0;
volatile char nasty_mark = 0;
volatile char record = 0;
short flash_buff_mark = 0;
short page_mark = 1;
char holdoff = 5;
int tmr_cnt;

volatile int x_accel;
volatile int y_accel;
volatile int z_accel;
unsigned char Elapsedcount = 0; // Make a new counter variable (count 20Hz) and initialize to zero.
unsigned char LEDcount = 0; // Count number of LED flashes


ISR (TIM1_OVF_vect)
{
	//PORTB ^= (1<<STAT);
	cli();//Disable Interrupts
	
	Elapsedcount++; // ?Hz has passed
	//TCNT1 = 56250;//100Hz
	//TCNT1 = 46965;//50Hz
	TCNT1 = 15537; // 20Hz
	
	if (Elapsedcount>=4)
	{
		
		//PORTB ^= (1<<STAT);
		
		int l2;//temp low
		int h2;//temp high

		x_accel = 0;
		y_accel = 0;
		z_accel = 0;
		
		h2 = 0; l2 = 0;

			
		//x_accel conversion====================================================================
		ADMUX = 0x02;
		ADCSRA = (1 << ADEN)|(1 << ADSC)|(1<<ADPS2)|(1<<ADPS1);
		
		while(ADCSRA & (1 << ADSC));
		l2 = (ADCL & 0xFF);
		h2 = (ADCH & 0x03);
		x_accel = ((h2<<8) | l2);

		h2 = 0; l2 = 0;


		//y_accel conversion===================================================================
		ADMUX = 0x01;
		ADCSRA = (1 << ADEN)|(1 << ADSC)|(1<<ADPS2)|(1<<ADPS1);
		
		while(ADCSRA & (1 << ADSC));
		l2 = (ADCL & 0xFF);
		h2 = (ADCH & 0x03);
		y_accel = ((h2<<8) | l2);
		
		h2 = 0; l2 = 0;


		//z_accel conversion=================================================================
		ADMUX = 0;
		ADCSRA = (1 << ADEN)|(1 << ADSC)|(1<<ADPS2)|(1<<ADPS1);
		
		while(ADCSRA & (1 << ADSC));
		l2 = (ADCL & 0xFF);
		h2 = (ADCH & 0x03);
		z_accel = ((h2<<8) | l2);
		
		h2 = 0; l2 = 0;
		
		
		if (record == 1)
		{
			//log the readings to the array...
			nasty[nasty_mark] = x_accel;
			nasty[nasty_mark+1] = y_accel;
			nasty[nasty_mark+2] = z_accel;
			nasty_mark += 3;
			if (nasty_mark >= 33) nasty_write = 1, nasty_mark = 0;
		}
		
		
		ADMUX = 0;
		Elapsedcount = 0;
	}
	
	sei();//Enable Interrupts
}




int main (void)
{
	char a;
	
	flash_buff_mark = 0;
	record = 0;
	
	DIDR0 = 0x07;//digital input disable ADC lines
	
	DDRA = 0b01001000;//PA6 = out (TX/MOSI), PA3 = SCK
	PORTB = 0x07;
	DDRB = 0b00000111;//PB0 = stat, PB1 = SI (local output), PB2 = CS // PB0 is sinking LED
	
	//setup timer0
	TCCR0B = 2;//clk/8
	
	while(!(status_read() & 0xA0));//check the ready bit on the flash  
	
	for (a = 0; a < 5; a++) // Rapid flashing when power is turned on
	{
		PORTB &= 0xFE; //stat on
		delay_ms(20);
		PORTB |= 0x01; //stat off
		delay_ms(50);
		
	}

	//check for UART connection
	PORTA &= 0xDF;//RX pin low to discharge a floating pin
	PORTA |= 0x40;//TX pin hi to start communication
	DDRA |= 0x20;
	DDRA &= 0xDF;
	
	delay_us(5);
	
	a = PINA;
	a &= 0x20;
	
	//If there's a high-ish voltage on the MISO line,
	//we're likely connected to a serial link
	if (a & 0x20) menu();
	
	//setup timer1
	//TCCR1A = 0;
	TCCR1B = 2;// clk/8
	TIMSK1 = 1;
	TIFR1 |= 0x01;
	TCNT1 = 65536 - 3000;//? 118Hz?
	
	sei();
	
	//start logging
	record = 1;
	
	
	while(1)
	{
		
		if (nasty_write == 1)
		{
			nasty_write = 0;
			
			write_buffer();
			flash_buff_mark += 66;
			//
			if (flash_buff_mark >= 528)
			{
				
				flash_buff_mark = 0;
				
				buffer_to_main(page_mark);
				page_mark++;
				if (page_mark >= 4096) while(1);
			}
		}
	}
    
}


//General short delays, not terribly accurate
void delay_us(uint8_t x)
{
	char y;
	
	TIFR0 |= 0x01;//Clear any interrupt flags on Timer0
	TCNT0 = 256 - x; 
	while(!(TIFR0 & 0x01));	
	
}

//General short delays
void delay_ms(short x)
{
	for (; x > 0 ; x--)
    {
        delay_us(250);
        delay_us(250);
        delay_us(250);
        delay_us(250);
    }
	
}



char getc(void)
{
	char k, l = 0, m = 0;
	
	k = 0x20;
	while(k & 0x20) 
	{
		k = PINA;
		k &= 0x20;
	}

	delay_us(35);
	
	for (k = 0; k < 8; k++)
	{
		l >>= 1;
		
		m = PINA;
		if (m & 0x20) l |= 0b10000000;
		else l &= 0b01111111;

		delay_us(23);
	}
	
	return l;
}




void putchr(char x)
{
	char k;
	
	PORTA &= 0b10111111;
	delay_us(23);//bit width at 38400
	
	for (k = 0; k < 8; k++)//8 data bits
	{
		if (x & 0b00000001) PORTA |= 0b01000000;
		else PORTA &= 0b10111111;
		x >>= 1;

		delay_us(23);
	}
	
	PORTA |= 0b01000000;//set it high again

	delay_us(23);
	
}



void write_buffer(void)
{
    //printf("Writing to buffer...\r\n",0);
    
    char i, j = 0;
	short x, y;
	
	send_command(0x84);//buffer 1 write
	
    //send 14 "dont care" bits
    PORTB &= ~(1<<SI);//SI low//SI = 0;
    for (i = 0 ; i < 14 ; i++) flash_clk();
	
	x = flash_buff_mark;
	
    for (i = 0 ; i < 10 ; i++)   //send 10 address bits, flash buffer marker
    {
        if (x & 0x200) PORTB |= (1<<SI);//SI high
		else PORTB &= ~(1<<SI);//SI low
		
		flash_clk();
        
        x <<= 1;
        
    }

    //send data out...
       
    for (x = 0; x < 33; x++)
    {		
		y = nasty[x];
		          
        for (i = 0 ; i < 16 ; i++)
        {   
            if (y & 0x8000) PORTB |= (1<<SI);//SI high
			else PORTB &= ~(1<<SI);//SI low
			
			flash_clk();
			
			y <<= 1;
        }

    }
	
	PORTB |= (1<<CS);//CS hi 
	
}


char read_flash(short page)
{
    char i,  x;
    short temp = 0, z;
	
	send_command(0x52);
	
	z = page;
	
	for (i = 0 ; i < 57 ; i++)//56 + 1, check AT45DB161 data sheet
	{
		//2 reserved bits, followed by 12 page address bits, then 9 bits for the exact starting byte
		if ((i >= 2) & (i < 14))
		{
			//page address is 12 bits
			if (z & 0x800) PORTB |= (1<<SI);//SI high
			else PORTB &= ~(1<<SI);//SI low
			z <<= 1;
		}
		
		else PORTB &= ~(1<<SI);//SI low
		
		flash_clk();

	}
	
	//read data out...
	temp = 0;
	
	for (z = 0; z < 264; z++)//pages are 528 bytes
	{ 
		if ((z % 3) == 0)
		{
			putchr(10);
			putchr(13);
		}
		
		for (x = 0; x < 16; x++)
		{
			
			temp <<= 1;
	
			PORTA |= (1<<SCK);//SCK hi
			
			i = PINA;
			if (i & (1<<SO)) temp += 1;
			
			PORTA &= ~(1<<SCK);//SCK low

		}
		
		if (temp == 0xFFFF) break;
		
		print_num(temp);
		putchr(9);
			
		temp = 0;
		
	}
	
    PORTB |= (1<<CS);//CS hi//flash_CS = 1;  
	
	if (temp == 0xFFFF) return 1;
	
    return 0;
}

//block erase, erases 8 pages at a time
void erase(void)
{
    char i;
	short j, temp;

	for (j = 0; j < 512; j++)//512*8=4096 pages
    {
		PORTB ^= (1<<STAT);
		send_command(0x50);
		
		temp = j;
		
		for (i = 0; i < 24; i++)
		{
			if ((i > 2) & (i < 12))
			{
				if (temp & 0x100) PORTB |= (1<<SI);//SI high
				else PORTB &= ~(1<<SI);//SI low
				temp <<= 1;
			}
			
			else PORTB &= ~(1<<SI);//SI low
			flash_clk();
		}
			
		PORTB |= (1<<CS);//bolck erases at rising edge of flash_CS
        
		while(!(status_read() & 0x80));//check the ready bit
        
    }
	
	flash_buff_mark = 0;
	page_mark = 1;
	nasty_mark = 0;
	
	PORTB |= 0x01; //stat off
    
}



void buffer_to_main(short page)
{    
    char i;
    
	if (page == 0) send_command(0x86);//write buffer 2 to page zero
	else send_command(0x83);
	
	for (i = 0; i < 24; i++)
	{
		if ((i >= 2) & (i < 14))
		{
			if (page & 0x800) PORTB |= (1<<SI);//SI high
			else PORTB &= ~(1<<SI);//SI low
			page <<= 1;
		}
		
		else PORTB &= ~(1<<SI);//SI low
		flash_clk();
	}
	
    PORTB |= (1<<CS);//CS hi   //buffer is written after this, takes 14mS
	
	delay_ms(14);
	
	// CHANGE HERE TO ELIMINATE FLASHING DURING LOGGING
	if (LEDcount<=4)
	{
	PORTB &= 0xFE; //stat on
	delay_ms(20);
	PORTB |= 0x01; //stat off
	LEDcount++;
    }
}


char status_read(void)
{
	char i, j, temp = 0;
	
	send_command(0x57);
    
	flash_clk();
	
	for (i = 0; i < 8; i++)
	{
		
		temp <<= 1;

		PORTA |= (1<<SCK);//SCK hi//SCK = 1
		
		j = PINA;
		if (j & (1<<SO)) temp += 1;

		PORTA &= ~(1<<SCK);//SCK low//SCK = 0;

	}

	PORTB |= (1<<CS);//CS hi   //
	
	return temp;
}


void send_command(char command)
{
	char i;
	
	PORTB &= ~(1<<CS);//CS low
	
    //send command
    for (i = 0 ; i < 8 ; i++)
    {
        if (command & 0x80) PORTB |= (1<<SI);//SI high
		else PORTB &= ~(1<<SI);//SI low
		
		flash_clk();
		
		command <<= 1;
        
    }

}


void flash_clk(void)
{
	PORTA |= (1<<SCK);//SCK hi//SCK = 1 
	PORTA &= ~(1<<SCK);//SCK low//SCK = 0;
}


void menu(void) // Command interface to terminal
{

	char x, z;
	short y;
	
	while(1)
	{
		putchr(10);
		putchr(13);
		putchr('?');
		
		x = getc();
		putchr(x);
		
		if (x == 'e') erase();
		
		else if (x == 'r')
		{
			for (y = 1; y < 4096; y++) 
			{
				x = read_flash(y);
				
				if (x == 1) break;
			
			}
			
			putchr(10);
			putchr(13);
			
		}
		
	}
	
}


void print_num(short num)
{
	short a;
	char b, c;
	a = num;
	
	//print hex
	for (c = 0; c < 4; c++)
	{
		b = (char)((a & 0xF000) >> 12);
		
		if (b < 10) putchr(b+48);

		else putchr(b+55);

		a <<= 4;
		
	}
	
}



