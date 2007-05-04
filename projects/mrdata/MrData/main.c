#include <avr/io.h>
#include <avr/interrupt.h>
#include "mmc.h"
#include "spi.h"
#include "delay.h"
#include "fat16.h"
#include "types.h"

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega88__)

//mega168
#define UDR UDR0
#define UCSRA UCSR0A
#define UCSRB UCSR0B
#define UCSRC UCSR0C
#define UBRRL UBRR0L
#define UBRRH UBRR0H
#define UDRE UDRE0
#define RXEN RXEN0
#define TXEN TXEN0
#define RXCIE RXCIE0
#define OCR2 OCR2A
#define TCCR0 TCCR0B
#define TCCR2 TCCR2B
#endif

#ifndef F_CPU
	#define F_CPU 8000000
#endif

// Buttons
#define KEY_PORT PIND
#define KEY_MASK 0x08

// Pins
#define PORT_LED PORTB
#define PIN_LED 1 //PORTB

// States
#define STOP 0
#define REC 2

// MMC
#define START_SECTOR 528	// This is the sector number where the search for midi files begins (no FAT32 readings!)
#define TX_BUF_SIZE 256	// lasts for at least 80ms on full MIDI data load (for *really* slow MMC write sector delays)

// Variables
u08 sharedmem[512];
u08 tx_buf[TX_BUF_SIZE], tx_wrp=0;//, tx_rdp=0, tx_wrp=0;	// TX buffer
u08 state=STOP, sample=0, channels=1;	// Flags
u08 file_cnt=0;	// File
u32 time;

extern struct fat_filedata_st fat_filedata;
extern u32 file_len;
extern u08 fat_cardfull;


// Local Functions
static void hw_init(void);


//---------------------------------------------------

static void error(void) {
	do {
		PORT_LED &= ~PIN_LED;
		delay_ms(100);
		PORT_LED |= PIN_LED;
		delay_ms(100);
	} while (1);
}


// Init MCU
static void hw_init(void) {
//	u08 k;

	// Oscillator
	// Ports
	DDRC = 0x00;	// adc
	PORTC = 0x00;

	DDRB = 0x2F;
	PORTB = 0xFE;

	DDRD = 0x12;	// Input-Port, auﬂer TxD
	PORTD = 0xEF;	// Pull-Ups aktivieren

	//Initialisierung der MMC/SD-Karte
	MMC_hw_init();
	spi_init();
	delay_ms(100);
	sei();

	if (!MMC_init()) {
		error();
	}

	TIMSK1 = 2;
}



// Uhr starten
/*static void start_time(u32 interval) {
	TCNT1 = 0;
	time = 0;
	if (interval < 65536UL) {
		OCR1A = interval;
		TCCR1B = 8+1;
	}
	else if (interval < (u32)65536UL*8UL) {
		OCR1A = interval/8;
		TCCR1B = 8+2;
	}
	else if (interval < (u32)65536UL*64UL) {
		OCR1A = interval/64;
		TCCR1B = 8+3;
	}
	else if (interval < (u32)65536UL*256UL) {
		OCR1A = interval/256;
		TCCR1B = 8+4;
	}
	else {
		OCR1A = interval/1024;
		TCCR1B = 8+5;
	}
}
*/

// Uhr stoppen
inline static void stop_time(void) {
	TCCR1B = 0;
}


// Dezimale Zahlen 0..65536
static void write_number_u32(u32 d) {
	u08 zehner=0, hunderter=0, tausender=0, zehntausender=0, hunderttausend=0, millionen=0, zehnmillionen=0, hundertmillionen=0, milliarden=0;
	while (d >= 1000000000) {
		d -= 1000000000;
		milliarden++;
	}
	while (d >= 100000000) {
		d -= 100000000;
		hundertmillionen++;
	}
	while (d >= 10000000) {
		d -= 10000000;
		zehnmillionen++;
	}
	while (d >= 1000000) {
		d -= 1000000;
		millionen++;
	}
	while (d >= 100000) {
		d -= 100000;
		hunderttausend++;
	}
	while (d >= 10000) {
		d -= 10000;
		zehntausender++;
	}
	while (d >= 1000) {
		d -= 1000;
		tausender++;
	}
	while (d >= 100) {
		d -= 100;
		hunderter++;
	}
	while (d >= 10) {
		d -= 10;
		zehner++;
	}
	u08 printzero=0;
	if (milliarden) {
		mmc_write_byte(milliarden+'0');
		printzero = 1;
	}
	if (hundertmillionen || printzero) {
		mmc_write_byte(hundertmillionen+'0');
		printzero = 1;
	}
	if (zehnmillionen || printzero) {
		mmc_write_byte(zehnmillionen+'0');
		printzero = 1;
	}
	if (millionen || printzero) {
		mmc_write_byte(millionen+'0');
		printzero = 1;
	}
	if (hunderttausend || printzero) {
		mmc_write_byte(hunderttausend+'0');
		printzero = 1;
	}
	if (zehntausender || printzero) {
		mmc_write_byte(zehntausender+'0');
		printzero = 1;
	}
	if (tausender || printzero) {
		mmc_write_byte(tausender+'0');
		printzero = 1;
	}
	if (hunderter || printzero) {
		mmc_write_byte(hunderter+'0');
		printzero = 1;
	}
	if (zehner || printzero) {
		mmc_write_byte(zehner+'0');
		printzero = 1;
	}
	mmc_write_byte(d+'0');
}


static s08 str_ncomp(u08 *buf, u08 *buf2, u08 len) {
	u08 cnt=0;

	for (cnt=0; cnt<len; cnt++) {
		if (buf[cnt] < buf2[cnt])
			return 1;
		else if (buf[cnt] > buf2[cnt])
			return -1;
	}
	return 0;
}


// Read string from MMC and verify content
static u08 eat_string(u08 *buf, u08 len) {
	u08 b, cnt=0;

	for (cnt=0; cnt<len; cnt++) {
		do {
			b = mmc_fetch_byte();
		} while (b == 13 || b == 10);
		if (b != (u08)buf[cnt])
			return 0;
	}
	return 1;
}


static u16 eat_number_u16(void) {
	u08 b;
	u16 erg=0;

	while (1) {
		b = mmc_fetch_byte();
		if (b>='0' && b<='9') {
			erg *= 10;
			erg += b-'0';
		}
		else
			return erg;
	}
}


//----------------------------------------------------------
// Main

void main(void) {
	u08 i;
	u16 overflow=781/*2*/, k;
	u08 prescaler=8+5;

	hw_init();
	PORT_LED |= PIN_LED;
	delay_ms(100);
	PORT_LED &= ~PIN_LED;
	if (!fat_init()) {
		error();
	}
	else {
		fat_count_files();
		if (file_cnt) {
			for (k=0; k<file_cnt; k++) {
				fat_read_filedata(k);
				if (str_ncomp(fat_filedata.name, "SETTINGS.TXT", 12) == 0 || str_ncomp(fat_filedata.name, "settings.txt", 12) == 0) {
					//mmc_read_sector(fat_filedata.startsect, sharedmem);
					fat_openfile(FILEMODE_READ, k);
					if (eat_string("ch=", 3)) {
						channels = eat_number_u16();
						if (channels == 0)
							channels = 1;
						else if (channels > 6)
							channels = 6;
					}
					if (eat_string("overflow=", 9)) {
						overflow = eat_number_u16();
					}
					if (eat_string("prescaler=", 10)) {
						prescaler = eat_number_u16();
						if (prescaler == 0)
							prescaler = 8+1;
					}
					fat_closefile();
				}
			}
		}
	}
	PORT_LED |= PIN_LED;

	while (1) {
		switch (state) {
			case STOP:
				if ((u08)(~KEY_PORT) & KEY_MASK) {
					delay_ms(100);
					state = REC;
					PORT_LED &= ~PIN_LED;
					while ((u08)(~KEY_PORT) & KEY_MASK);
				}
				break;

			case REC:
				// Rekorder-Task
				fat_openfile(FILEMODE_WRITE, file_cnt);
				if (fat_cardfull) {
					state = STOP;
					PORT_LED |= PIN_LED;
				}
				mmc_write_start(fat_filedata.startsect);
				file_len = 0;
			
				u08 tx_rdp=0;
				tx_wrp=0;
				//start_time((u32)(interval*F_CPU));
				time = 0;
				OCR1A = overflow;
				TCNT1 = overflow-1;
				TCCR1B = prescaler;

				while (1) {
					if (sample == channels) {
						PORT_LED &= ~PIN_LED;
						while (tx_rdp != tx_wrp) {
							write_number_u32(time);
							for (i=0; i<channels; i++) {
								mmc_write_byte(9);
								write_number_u32(tx_buf[tx_rdp]+256*tx_buf[tx_rdp+1]);
								tx_rdp+=2;
							}
							mmc_write_byte(0x0d);
							mmc_write_byte(0x0a);
						}
						sample = 0;
					}
					if (((u08)(~KEY_PORT) & KEY_MASK) || fat_cardfull) {
						stop_time();
						PORT_LED |= PIN_LED;
						delay_ms(100);
						state = STOP;
						while ((u08)(~KEY_PORT) & KEY_MASK);
						break;
					}
				}
				
				fat_filedata.len = file_len;
				mmc_complete_write();
				fat_closefile();
				file_cnt++;

				break;
		}
	}
}


//----------------------------------------------------------
// IRQ

ISR(SIG_OUTPUT_COMPARE1A) {
	sample = 0;
	PORT_LED |= PIN_LED;
	// ADC start
	ADMUX = 0x40+0;
	ADCSRA = 0xc8+6;
	time++;
}

//volatile u16 adcval;

ISR(SIG_ADC) {
//	adcval = ADC;
	tx_buf[tx_wrp++] = ADCL;
	tx_buf[tx_wrp++] = ADCH;
	sample++;
	if (sample < channels) {
		ADMUX++;
		ADCSRA = 0xc8+6;
	}
}
