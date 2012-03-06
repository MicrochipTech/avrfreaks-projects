/*			
/
/	DAQ Logger using AVR with self-programming   
/	----------------------------------------------
/
/	AUTHOR	:	Visakhan C 
/	Compiler:	AVR-GCC (WinAVR)
/	DATE	:	Jan 19, 2012	
/			
/	Description: 
/	
/	Stores a log of temperature(Celcius) and light intensity reading with the time (hh:mm).
/	Self programming of on-chip flash memory of the AVR device is used. Flash memory can be read back into PC and the logged data 
/	be displayed as a graph or table using a custom made application.
/	
/	
/	MCU: ATmega16 
/	XTAL:  8MHz
/	Temperature Sensor: LM35
/	Light Sensor: LDR
/	(Please see the documentation for detailed description of hardware and usage) 
/
/			
*/


#include <avr/io.h>
#include <avr/boot.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "adc.h"
#include "rtc.h"
#include "devboard.h"


#define START_PAGE 25   //change according to your code size and
#define END_PAGE 112	//size of AVR flash memory  
#define START_PAGE_ADDR  ( (START_PAGE)*(SPM_PAGESIZE) )
#define END_PAGE_ADDR 	 ( (END_PAGE)*(SPM_PAGESIZE) )
#define LOGS_PER_PAGE (SPM_PAGESIZE/6)		//size of one reading = 6 bytes 


/*		GLOBALS		*/

extern volatile time t; //time structure in time.h
static uint16_t ldr,tem; //ADC results for light and temperature measurement
static uint8_t buffer[SPM_PAGESIZE];   //temprary buffer in SRAM (PAGESIZE = 128 for ATmega16) 
static void* pMem ;	 		//pointer to buffer


/*		FUNCTIONS 		*/

static void avr_init(void);
static void update_readings(void);
static void set_time(void);
static void blinkLED2(uint8_t n);
static void log_ADC(void);
static void clear_flash(void);
void write_flash(uint16_t page_addr, uint8_t* buf) __attribute__ ((section (".bootloader")));


/*		Main		*/

int main(void)
{
    avr_init();
    
    for(;;)
    {
		update_readings();
		sleep_mode();
    }
    
    return(0);
}


static void avr_init(void)
{
	devboard_init();
	adc_init(REF_INTERNAL|LM35_CHANNEL);
	rtcInit();
	set_time();
	clear_flash();
	pMem = buffer;
	*(uint8_t*)pMem++ = t.month;
	*(uint8_t*)pMem++ = t.date;
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    return;
}


static void blinkLED2(uint8_t n)  //blink LED2 'n' times
{
	while (n--)
	{
		led2_on();
		_delay_ms(250);
		led2_off();
		_delay_ms(250);
	} 
}


static void set_time(void)
// set hour and minute using button and LED interface
{
	uint8_t hour=0,min=0,month=1,date=1;
	
	while(SW2_STATUS != SW_ON) {
	
		if(SW1_STATUS == SW_ON) {
		
			if(month == 12)
				month = 1;
			else
				month++;
			blinkLED2(1);
		}
		
	}
	
	led1_on();
	blinkLED2(month);
	_delay_ms(400);
	led1_off();
	
	while(SW2_STATUS != SW_ON) {
		
		if(SW1_STATUS == SW_ON) {
			if(date == 31) date = 1;
			else date++;
			blinkLED2(1);
		}
		
	}
	led1_on();
	blinkLED2(date);
	_delay_ms(400);
	led1_off();
	
	while(SW2_STATUS != SW_ON) {
		
		if(SW1_STATUS == SW_ON) {
			hour = (++hour)%24;
			blinkLED2(1);
		}
		
	}
	led1_on();
	
	blinkLED2(hour);
	_delay_ms(400);
	led1_off();
	
	while(SW2_STATUS != SW_ON) {
		
		if(SW1_STATUS == SW_ON) {
			min = (++min)%60;
			blinkLED2(1);
		}
		
	}	
	
	led1_on();
	blinkLED2(min);
	_delay_ms(400);
	led1_off();

	t.minute = min;
	t.hour = hour;
	t.date = date;
	t.month = month;	

}

static void update_readings(void) 
// Accumulates ADC readings to memory in 5 second intervals
// Calls log_ADC() at 60 second intervals 
{
	static uint8_t next_sec = 0;
	
	if(t.second == next_sec) {
		
		adc_configure(REF_AVCC|LDR_CHANNEL);
		_delay_ms(1);
		ldr += adc_read_10bit();
		adc_configure(REF_INTERNAL|LM35_CHANNEL);	
		_delay_ms(1);
		tem += adc_read_10bit();
		next_sec += 5;
		if(next_sec == 55) {		
			
			next_sec = 0;
			ldr = ldr/12;
			tem = tem/12;
			log_ADC();
		}
	}

}
	
static void log_ADC(void)
//Writes one reading to buffer; if buffer is full writes it to a flash page */
{

	static uint8_t logCount = 0;  //counts number of log events
	static uint16_t PageAddress = START_PAGE_ADDR;  
	
	led2_on(); //indication of writing to buffer (should show every minute) 

	*(uint8_t*)pMem++ = t.hour;
	*(uint8_t*)pMem++ = t.minute;
	*(uint16_t*)pMem = (tem*25);
	pMem +=2;
	*(uint16_t*)pMem = ldr;
	pMem +=2;
	
	//When Memory buffer full, call flash writing function
	if(++logCount == LOGS_PER_PAGE) 	
	{
		write_flash(PageAddress,buffer);
		PageAddress += SPM_PAGESIZE;
		
		//When end of Application Flash Section reached
		if(PageAddress == END_PAGE_ADDR)			
			PageAddress = START_PAGE_ADDR;	

		logCount = 0;
		
		//Reinitialize buffer pointer 
		pMem = (uint8_t*)buffer;	
	}	
	
	//Display the indication for some time
	_delay_ms(150);  		
	led2_off();

}		

static void clear_flash(void)
//clear the flash region between START_PAGE and END_PAGE 
{
	uint16_t PageAddress = START_PAGE_ADDR;
	uint8_t i=SPM_PAGESIZE-1;
	
	led1_on();
	do {
	
		buffer[i] = 0xFF;
		
	} while(i--);
	
	do {
	
		write_flash(PageAddress,buffer);
		PageAddress += SPM_PAGESIZE;
		
	} while(PageAddress != END_PAGE_ADDR);
	
	led1_off();
}
	

void write_flash (uint16_t page, uint8_t *buf)
// Write one flash page, starting at flash byte addess:'page') from the buffer memory address 'buf' 
// Disables interrupts, reenable at the end
{

	uint8_t i;
	uint8_t sreg;

	// Disable interrupts.
	sreg = SREG;
	cli();

	boot_page_erase(page);
	boot_spm_busy_wait();      // Wait until the memory is erased.

	for (i=0; i<SPM_PAGESIZE; i+=2)
	{
		// Set up little-endian word.
		uint16_t w = *buf++;
		w += (*buf++) << 8;
		boot_page_fill(page + i, w);
	}

	boot_page_write(page);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.

	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.
	boot_rww_enable(); 
	
	// Re-enable interrupts
	sei();
	SREG = sreg;
	
}

