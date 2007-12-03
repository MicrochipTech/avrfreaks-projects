/*

 recording barometer/serial LCD SCP1000 code.
 last update 11/20/2007
 Jim Remington, sjames_remington at yahoo dot com

    SCP1000-D01 SPI pressure sensor 
    ATmega8
	DX-160 serial 128/64 LCD

 based on original SCP1000 code:
    8-6-06
    3-6-07
    Copyright Spark Fun Electronics© 2007
    Nathan Seidle
    nathan at sparkfun.com
    
 For graph scaling, pressure data for one year were downloaded from Teddington England station
 Data have mean +/- sd of 1012.76 +/- 9.9 mB. 
 
 So, to cover a range of two standard deviations, need to plot +/- 20 mB about 
 mean of 1013.25 (standard atmosphere).Tests show that this is an excessive range 
 for short term observations. 
 
 Graph range currently set by dividing constant SCALE=8 (default)
 so, pixel range [0,47] corresponds to millbar range {1032.5 , 994.8 mB}


	*** for ATmega8 compile with -Os (optimization for size)

*/

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
//#include <math.h>

//OFFSET converts to sea level, was 2299 for first scp1000 

#define OFFSET 2498

#define MINUTES 20	// number of minutes for linear regression of pressure trend
#define SAMPLES 60	// number of 1-second samples per "minute"

// SCALE converts (0.1 mB units to +/- 24 about 1 Atm) y=24-int(pressure_mB - 1013.25)*10/SCALE)

#define SCALE 8		//graph pressure scale factor (dividing)


// globals for circular FIFO data buffer.  CBUFSIZE must be power of 2

#define CBUFSIZE 128
#define MASK (CBUFSIZE-1)

#define FOSC 8000000	//internal RC
#define BAUD 9600      //higher baud rates don't work well with internal RC
#define MYUBRR FOSC/16/BAUD-1

#define sbi(var, mask)   ((var) |= (uint8_t) (1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

// PORT bit definitions

// PORTB

#define STATUS_LED 0

// PORTC, bit banged SPI for interface with SCP1000

#define SCP_CSB     0
#define SCP_MISO    1
#define SCP_MOSI    2
#define SCP_SCK		3


void ioinit (void);      // initializes IO

void scp_read(void);
uint8_t read_register(uint8_t register_name);
uint16_t read_register16(uint8_t register_name);
void write_register(uint8_t register_name, uint8_t register_value);
uint8_t spi_comm(uint8_t outgoing_byte);

void print_decimal(int32_t number, uint8_t place, uint8_t pad);
void Delay_us( unsigned int microseconds );
void Delay_ms( unsigned int milliseconds );

void uart_putc(char c);
void uart_puts(char *str);

void lcd_send(void);
void lcd_cls(void);
void lcd_rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void lcd_loc(unsigned char row, unsigned char col);
void lcd_putpixel(uint8_t x, uint8_t y, uint8_t color);
void lcd_printat(unsigned char row, unsigned char col, char *txt);

signed int fetch(void);
signed int store(signed int value);

// global variables

volatile signed long int pressure,pav,tav;
volatile signed int p,np,pmin,pmax,temperature;	//****negative temps OK?****
volatile signed int int0_flag=0;			//switch interrupt flag

volatile unsigned char indx=0,jndx=0;	//pointers to next storage location
volatile signed int cbuf[CBUFSIZE];		//Circular buffer for pressure data

volatile char buf[21];					//buffer for graphics commands
volatile int nbuf;						//number of characters in buf

volatile unsigned char global_ticks;	//interrupt routine
volatile unsigned char flag,i,j;		//need to put the loop variables for main() here because
volatile float nx,x,y,sx,sx2,sxy,sy;	//of apparent optimizer errors when compiling with -Os

/*
** interrupt service routines
*/

ISR (INT0_vect)							//button press, turn on backlight
{
int0_flag=30;							//for 30 seconds
}

ISR (TIMER2_OVF_vect) 
{
    global_ticks++;

    if(global_ticks == 250) 			//250 ticks/second
    {
        flag = 1; 						//One second has hit
        global_ticks = 0;
    }
    
    TCNT2 = 256-125; //Preload timer 2 for 125 clicks. 8MHz/256 = 31250 incs/sec / 125 => 250 incs/sec
}


int main (void)
{

 	global_ticks=0;
    
    ioinit (); 			//set defaults
	int0_flag=70;		//backlight on for 70 seconds

	sbi(PORTB, STATUS_LED);
	Delay_ms(500);
	lcd_cls();

	uart_putc(170); uart_putc(30); Delay_ms(50);	//turn on backlight

    lcd_printat(4,4,"Barometer v2.3");
	Delay_ms(1000);
	cbi(PORTB, STATUS_LED);

// startup error checking

	for(i=1; i<7; i++) {
	j = read_register(0x07);			//STATUS register
	if ((j&1)==0) break;				//successful if LSB=0
	Delay_ms(10);
	}

// reinitialize to fix checksum error
// write_register(address, data)

	write_register(0x06, 0x01);			//write 1 to RSTR (software reset)
	Delay_ms(60);

	for(i=1; i<7; i++) {
	j = read_register(0x07);				//STATUS register
	if ((j&1)==0) break;
	Delay_ms(10);
	}
	j = read_register(0x1F);				//DATARD8 register
	if (j!=1) lcd_printat(6,2,"Startup failure!");		//should equal 1 if eeprom checksum OK
   	Delay_ms(2000);

    //Configure SCP1000 with low noise configuration
    //=====================================
    //write_register(address, data)
    write_register(0x02, 0x2D);
    write_register(0x01, 0x03);
    write_register(0x03, 0x02);
    
    Delay_ms(100);

    //Select High Resolution Mode
    write_register(0x03, 0x0A);
    //=====================================

	lcd_cls();//clear display

	lcd_rect(0,0,127,47);	//outline for graph

	lcd_printat(7,1,"Sampling");

//	buf[0]=170; buf[1]=0; nbuf=2; lcd_send; //doesn't work!! backlight off
//	uart_putc(170); uart_putc(30);	//this does work to set backlight to 0/255 intensity
 
 	sx=0.; sx2=0.; 	nx = (float) MINUTES;
	for(i=0; i<MINUTES; i++) {  //constants for linear regression
		x= (float) i;
		sx += x;
		sx2 += x*x;
	}

    while(1)
    {

	sxy=0. ; sy=0.;				//initialize linear regression

	for(i=0; i<MINUTES; i++) {  //sample over some minutes for estimation of trend in pressure

	tav=0; pav=0;

// loop executes each second

		for(j=0; j<SAMPLES; j++) {	//sample over SAMPLES (60) seconds for each reading
			scp_read();				//read pressure and temperature from SCP1000
			tav+=temperature;
			pav+=pressure+OFFSET;

			while(flag == 0) {			//wait until 1 second has passed
										//check backlight switch. If low, set backlight=30/255
				if(int0_flag != 0) {uart_putc(170); uart_putc(30); Delay_ms(100);}
				}
			flag = 0;
			if (int0_flag>0) int0_flag--; //count down seconds for backlight, then off

			if (int0_flag == 0) {uart_putc(170); uart_putc(0); Delay_ms(10);}	//backlight off

		} //end for j
		
		 
		sbi(PORTB, STATUS_LED);

		tav=tav/SAMPLES; pav=pav/SAMPLES;
		x= (float) i; y=(float) pav;		//do linear regression sums to 
		sy += y;							// calculate rate of change of pressure (dPa/hr)
		sxy += x*y;
		lcd_loc(7,1);
		print_decimal(pav, 2, 0);
		lcd_printat(7,8," mB");
		lcd_loc(7,16);
		print_decimal(tav, 1, 0);
		lcd_printat(7,20," C");
		lcd_printat(8,20,"  ");
		if ((MINUTES-i-1)>9) lcd_loc(8,20); else lcd_loc(8,21);
		print_decimal((MINUTES-i-1),0,1); // countdown to update
 		cbi(PORTB, STATUS_LED);            

		} //end for i

//		temperature = (90*tav)/5 + 3200;  //degrees F
// data for running plot of pressure (last 43 hours for 20 minute sample period)

		lcd_cls();  //clear display
		lcd_rect(0,0,127,47);	//outline for graph
		lcd_loc(7,1);
		print_decimal(pav, 2, 0); //decimal place = 2 digits from right
		lcd_printat(7,8," mB");
		lcd_loc(7,16);
		print_decimal(tav, 1, 0);  //temp in Celcius
		lcd_printat(7,20," C");

		p=(pav-101325L)/10L;	// (Pa-std atmosphere) in units of 0.1 millibar (integer)
		np=store(p);			//save it in circular buffer
/*
graph autoscaling not yet implemented...
*/

//plot the most recent pressure data, up to 128 points

		jndx=indx;				//point to last value stored+1
		for (i=np;i>0;i--) {
			p=24-(fetch()/SCALE);
			if( (p>=0) && (p<=47) ) {j=p; lcd_putpixel(i-1,j,2);}  //2=invert pixel at location
			}

// calculate trend in pressure = slope of P versus time 
// = (sum(x*y) - sum(x)*sum(y)/N)/(sum(x^2)-(sum(x))^2/N)
// -> delta Pa/hr

		x=(60.0/nx)*(sxy-sx*sy/nx)/(sx2-sx*sx/nx);  //(60./nx) -> per hour basis

		pav = x+0.5;		//round to integer
		lcd_printat(8,1,"dPa/hr");  
		lcd_loc(8,8);
		print_decimal(pav,0,1); 
    
    }

    return (0);
}

void ioinit (void)
{
    //1 = output, 0 = input
    DDRB = (1<<PB0); 					//status LED
    DDRC = (1<<PC0)|(1<<PC2)|(1<<PC3); 	//PC 0=CSB, 1=MISO, 2=MOSI, 3=SCK
    DDRD = (1<<PD1); 					//PORTD (Switch interrupt on PD2, TX on PD1, RX on PD0)
	PORTD = (1<<PD2);					//turn on pullup on PD2 for switch
	MCUCR = 0;							//low level activates interrupt
	GICR = (1<<INT0);					//enable INT0;

    // USART Baud rate: 9600
    UBRRH = (MYUBRR) >> 8;
    UBRRL = MYUBRR;
    UCSRB = (1<<RXEN)|(1<<TXEN);
    UCSRC = (1<<URSEL)|(3<<UCSZ0);
    
    
    sbi(PORTC, SCP_CSB); //Deselect SCP1000
    
    //Init timer 2
    ASSR = 0;
    TCCR2 = (1<<CS22)|(1<<CS21)|(0<<CS20); //Set Prescaler to 256. CS22=1, CS21=1,CS20=0
    TIMSK = (1<<TOIE2); //Enable Timer 2 Interrupt
    TCNT2=256-125;	//125 clicks to overflow, 250 clock ticks per second
    sei();

}

//Takes an int and prints a decimal in a given place
//(87004, 3, 0) = 87.004
//10 digit max = 1234567890
//pad will put zeros in front to make the length equal to "pad" characters
//(87004, 3, 7) = 0087.004
// stores character result in buf[]

// Bug for number=0 and pad=0 -- nothing printed! Fix: set pad=1

void print_decimal(int32_t number, uint8_t place, uint8_t pad)
{
    uint8_t i;
    unsigned char nums[11];
    
    for(i = 0 ; i < 11 ; i++) nums[i] = 0;
	nbuf=0;									//output character buffer index
	if (number < 0) {number=-number; buf[nbuf++]='-';} // else buf[nbuf++]=' ';
	    
    for(i = 10 ; number > 0 ; i--)
    {
        nums[i] = number % 10;
        number /= 10;
    }
    
    //Pad the number of spots
    if (pad <= 10 && (pad > (10 - i)) ) i = 10 - pad;
    for(i++ ; i < 11 ; i++)
    {
        buf[nbuf++]=nums[i]+'0';
        
        //Check for decimal place
        if ( ((i + place) == 10) && (place != 0) ) buf[nbuf++]='.';
    }
	buf[nbuf++]=255;	//terminate string for lcd, strlen=nbuf
	lcd_send();			//write it to the lcd
	Delay_ms(20);		//need a short delay
}

//
void scp_read(void)
{
    int32_t temp_data;

    //Wait for new data
    while(1)
    {
     temp_data = read_register(0x07); //Check the status register for bit 6 to go high
     if (temp_data & 32) break;
    }

    temperature = read_register16(0x21); //Read the temperature data
    temperature /= 2; //This is (temp * 10) / 20 so that the answer of 27.9 comes out as 279 (easier to print)

    temp_data = read_register(0x1F); //Read MSB pressure data - 3 lower bits
    pressure = read_register16(0x20);

    temp_data <<= 16; //Shift it
    pressure = temp_data | pressure;
    pressure >>= 2;

}

///////////////////////////////////////////////////////////////
// Print character to uart
void uart_putc(char c)
{
  loop_until_bit_is_set(UCSRA, UDRE);
  UDR = c;
}

//Read 8-bit register
uint8_t read_register(uint8_t register_name)
{
    uint8_t in_byte;
    
    register_name <<= 2;
    register_name &= 0b11111100; //Read command

    cbi(PORTC, SCP_CSB); //Select SPI Device

    in_byte = spi_comm(register_name); //Write byte to device
    //in_byte is nothing, we need to clock in another 8 bits
    in_byte = spi_comm(0x00); //Send nothing, but we should get back the register value

    sbi(PORTC, SCP_CSB);
    
    return(in_byte);
}

//Read 16-bit register
uint16_t read_register16(uint8_t register_name)
{
    uint16_t in_byte;

    register_name <<= 2;
    register_name &= 0b11111100; //Read command

    cbi(PORTC, SCP_CSB); //Select SPI Device

    in_byte = spi_comm(register_name); //Write byte to device
    //in_byte is nothing, we need to clock in another 8 bits
    in_byte = spi_comm(0x00); //Send nothing, but we should get back the register value
    in_byte <<= 8;
    in_byte |= spi_comm(0x00); //Send nothing, but we should get back the register value

    sbi(PORTC, SCP_CSB);
    
    return(in_byte);
}

//Sends a write command to SCP1000
void write_register(uint8_t register_name, uint8_t register_value)
{
    uint8_t in_byte;
    
    register_name <<= 2;
    register_name |= 0b00000010; //Write command

    cbi(PORTC, SCP_CSB); //Select SPI device

    in_byte = spi_comm(register_name); //Send register location
    in_byte = spi_comm(register_value); //Send value to record into register
    
    sbi(PORTC, SCP_CSB);
    
    //Return nothing
}

//Basic SPI send and receive
uint8_t spi_comm(uint8_t outgoing_byte)
{
    uint8_t incoming_byte, x;

    incoming_byte = 0;
    
    for(x = 8 ; x > 0 ; x--)
    {
        cbi(PORTC, SCP_SCK); //Toggle the SPI clock

        //Put bit on SPI data bus    
        if(outgoing_byte & (1 << (x-1)))
            sbi(PORTC, SCP_MOSI);
        else
            cbi(PORTC, SCP_MOSI);

        sbi(PORTC, SCP_SCK);

        //Read bit on SPI data bus
        incoming_byte <<= 1;
        if ( (PINC & (1 << SCP_MISO)) ) incoming_byte |= 1;
    }
    
    return(incoming_byte);
}
/*
// globals for circular data buffer.  CBUFSIZE must be power of 2

#define CBUFSIZE 128
#define MASK (CBUFSIZE-1)

volatile unsigned char indx=0,jndx=0;  //pointers to next storage location
volatile int cbuf[CBUFSIZE];

*/

// store value in circular buffer. 
// Returns nstore, # of values stored
// updates global variable "indx" to point to next free location

signed int store(signed int value) {

	static signed int nstore=0;

	cbuf[indx] = value;
	indx++;
	indx &= MASK;		//MASK obviates range check

	if (nstore < CBUFSIZE) nstore++;
	return nstore;
}

// fetch the jndx value from the circular buffer
// jndx must be initialized externally and is predecremented

signed int fetch(void) {

	volatile signed int value;
	jndx--;
	jndx &= MASK;
	value = cbuf[jndx];
	return value;
}

////////////////////////////////////////////////////////////////////
//DX-160 graphics routines
////////////////////////////////////////////////////////////////////

void lcd_send() {
	int m;
	for (m=0;m<nbuf;m++) {
	uart_putc(buf[m]); Delay_ms(1);
	}
	return;
	}

void lcd_cls() {

	buf[0]=186; nbuf=1; lcd_send(); 
	Delay_ms(50); 
	return; 
	}

// coordinate system upper left = (0,0), lower right=(127, 63)

void lcd_rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {

	buf[0]=200; buf[1]=2; buf[2]=x1; buf[3]=y1;
	buf[4]=x2; buf[5]=y2; buf[6]=1; buf[7]=200;
	nbuf=8; lcd_send(); Delay_ms(150);
	return;
	}
	
// put a pixel at (x,y) color 0=white, 1=black, 2=invert

void lcd_putpixel(uint8_t x, uint8_t y, uint8_t color) {

	buf[0]=200;
	buf[1]=5; buf[2]=x; buf[3]=y; 
	buf[4]=color; buf[5]=200; 
	nbuf=6; lcd_send();
	Delay_ms(1); //not needed?
	return;
	}
	
void lcd_loc(unsigned char row, unsigned char col) {

	buf[0]=126+row;
	buf[1]=135+col;
	nbuf=2; lcd_send(); //set print lcation to (row,col)
	Delay_ms(10);
}
void lcd_printat(unsigned char row, unsigned char col, char *txt) {

	lcd_loc(row,col);
	nbuf=0;
	while(*txt != 0) buf[nbuf++]=*txt++; //pack buffer, terminate with 255 and send
	buf[nbuf++]=255;
	lcd_send();		//nbuf needs to be one more than the buf location
	Delay_ms(10);
	return;
	}

/*
 *  Delay_us
 *
 *  wait in a loop for the specified number of microseconds.
 *
 */

void Delay_us( unsigned int microseconds )
{
    register unsigned int loop_count;
    /* 8mhz clock, 4 instructions per loop_count  */
    loop_count = microseconds<<1;

    __asm__ volatile (
        "1: sbiw %0,1" "\n\t"
        "brne 1b"
        : "=w" ( loop_count )
        : "0"  ( loop_count )
    );
}

/* Delay_ms
 *
 *  wait in a loop for the specified number of milliseconds.
 *
 */

void Delay_ms( unsigned int milliseconds )
{
    uint16_t i;

    for ( i = 0; i < milliseconds; ++i )
    {
        Delay_us( 1000 );
    }
}
