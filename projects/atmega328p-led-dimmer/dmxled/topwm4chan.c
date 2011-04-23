//Aaron Rackoff
//Spectrum Lighting
//994 Jefferson st
//Fall River, MA 02721
//Last Revised: 12 April 2011
//Program Description:
// Inputs : Serial DMX stream
// Output : Four independently controllable PWM channels

// Pin descriptions:
// MAIN PIN FUNCTIONS:                       (ISP programmer use)
// **************************************************************
// Pin  1: 10k resistor to ground            (used for ARES line)
// Pin  2: RX, 
// Pin  5: LED PWM output (OCR2B): white
// Pin  7: VCC  (5V)                         (connect to ISP VCC)
// Pin  8: GND                               (connect to ISP GND)
// Pin 11: LED PWM output (OCR0B): green
// Pin 12: LED PWM output (OCR0A): red 
// Pin 17: LED PWM output (OCR2A): blue      (used for MOSI line)
// Pin 18:                                   (used for MISO line)
// Pin 19:                                   (used for SCLK line)
// Pin 20: AVCC (5V) **optional
// Pin 21: AREF (5V) **optional
// Pin 22: GND       **optional

#include <avr/io.h> 
#include <avr/interrupt.h>

//adjust as needed
#define F_CPU 8000000UL
#include <util/delay.h>

//RGBW values (0-255)
static unsigned char red = 0x00;
static unsigned char grn = 0x00;
static unsigned char blu = 0x00;
static unsigned char wht = 0x00;

int i,j;

#define DMX_CH_BASE 0
#define DMX_BUF_SIZE 24
#define DMX_SYNC 1
#define DMX_SIZE 8

unsigned char *init_dmx (int, int);
static volatile unsigned char dmx_buf[DMX_BUF_SIZE];
static volatile int dmx_ch, dmx_size, dmx_addr;
static volatile char dmx_mode;
int mod0 = 0;
int mod1 = 0;

void pwmsetup(void){
//set direction registers to PWM mode:
	DDRB 	= 0b00001000;
	DDRD 	= 0b01101000;
//Configure Timer/Counter Contol Regiseters	
	TCCR0A	= 0b10100011;
	TCCR0B	= 0b00000001; 
	TCCR2A	= 0b10100011;
	TCCR2B	= 0b00000001;
};

unsigned char *init_dmx (int ch, int size) {
//set baud rate:
	UBRR0H	= 0b00000000;
	UBRR0L	= DMX_SYNC;
//configure USART Control/Status Registers:
	UCSR0A	= 0b00000010;
	UCSR0B	= 0b10010000;
	UCSR0C	= 0b00001110;
// buffer clear
	dmx_mode = 0;
	dmx_addr = 0;
	dmx_ch = ch;
	dmx_size = size;
	if (size > DMX_BUF_SIZE) {
		dmx_size = DMX_BUF_SIZE;
	}//end if
    for (i = 0; i < dmx_size; i ++) {
        dmx_buf[i] = 0;
    }//end for
    return (unsigned char *)&dmx_buf;
};

ISR (USART_RX_vect){
	register unsigned char flg, dat;

    flg = UCSR0A;
    dat = UDR0;

    if (flg & 0x0c) {
        // data over run, parity error
        dmx_mode = 99; // data error
        while (UCSR0A & 0x80) { dat = UDR0; } // buffer clear
        return;
    }
	else if (flg & 0x10) {
        // Frame error, detect BreakTime
        dmx_mode = 1;
        return;
    };
    
    if (dmx_mode == 1) {
        // Start Code
        if (dat == 0) {
            // Start Code = 0
            dmx_addr = 0;
            dmx_mode = 2;
        }
        else {
            // Start Code error
            dmx_mode = 99;
        };
    } 
	else if (dmx_mode == 2) {
        // DMX base address

        if (dmx_addr == dmx_ch) {
            dmx_addr = 0;
            dmx_mode = 3;
        } 
		else {
            dmx_addr ++;
        };
    };
    
    if (dmx_mode == 3) {
        // recieve the data
        dmx_buf[dmx_addr] = dat;
        dmx_addr ++;
        if (dmx_addr >= dmx_size) {
            // complete receive
            dmx_mode = 4;
        };
    };
    
	OCR0A	= dmx_buf[0];
	OCR0B	= dmx_buf[1];
	OCR2A	= dmx_buf[2];
	OCR2B	= dmx_buf[3];
}


void colorotate(void){
	red 	= 0x00;
	grn 	= 0x00;
	blu 	= 0x00;
	wht 	= 0x00;
	for (j=0;j<40;j++){red++;_delay_ms(10);};	
	while(1){
		for(j = 0; j <120; j++){
			if((j>=0)&&(j<40)){
				grn++;
				_delay_ms(10);
				red--;
				_delay_ms(10);
			}
			else if((j>=40)&&(j<80)){
				grn--;
				_delay_ms(10);
				blu++;
				_delay_ms(10);
			}
			else if((j>=80)&&(j<120)){
				blu--;
				_delay_ms(10);
				red++;
				_delay_ms(10);
			}
			OCR0A	= 1*red;
			OCR0B	= 1*grn;
			OCR2A	= 1*blu;
			OCR2B	= 1*wht;

			_delay_ms(10);
			_delay_ms(10);
			_delay_ms(10);
			_delay_ms(10);
		};//end for
	};//end while
};//end colorotate

void preset(void){
//change these to get different colors.
	red 	= 0x00;
	grn 	= 0xff;
	blu 	= 0x00;
	wht 	= 0x00;
//change the multipliers to scale output, but note that any overflow from color (ie red > 255)
//will cause unpredictable behaviour 
	OCR0A	= 1*red;
	OCR0B	= 1*grn;
	OCR2A	= 1*blu;
	OCR2B	= 1*wht;

};//end preset

/*
void transmitter(void){
	while(1){
		//check inputs
		//figure out how to send dmx messages based on inputs
		;
	};//end while
};//end transmitter
*/



int main(void){
	volatile unsigned char *dmx_buf;
	pwmsetup();
	dmx_buf = init_dmx(i * DMX_SIZE, DMX_SIZE);
	sei();

//:MODES:
//comment out to to change to a different mode.
	colorotate();
	//preset();
	//transmitter();

//to set up device to act as DMX receiver,
//comment out all above modes, but leave the next line:
	while(1);
//it is important to leave this in, since preset allows lights to hold a setting until interrupted by DMX input.
return 1; 
}
