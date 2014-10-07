/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * pjv [at] post.com wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 *
 * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
 * OR IMPLIED. ANY USE IS AT YOUR OWN RISK.
 *

 * Atmel Studio 6 (Version: 6.1.2674 - Service Pack 1)
 * Atmel AVR (8 bit) AVR/GNU C Compiler : 3.4.2
 * ATmega324P
 * F_CPU = 16000000UL 
 * -Opt=s
 * Program Memory Usage : 19180/19860 bytes (DEBUG==0/1)
 * Data Memory Usage : 1513 bytes
 * Author: PjV
 * Version: 1.0 
*/


#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "macros.h"
#include <util/delay.h>
#include "tracker.h"
#include "gps.h"
#include "sd_interface.h"
#include "pc_int.h"
#include "db_uart.h"


/**********************************
* static functions
**********************************/
static void _init_io( void );
static unsigned char _check_button(void);
static void _enter_sleep(void);


/**********************************
* main function state machine
***********************************/
int main()
{
	unsigned int state = INIT;
	char* ptr_to_valid = NULL;
	
    _init_io();
	set_sleep_mode(SLEEP_MODE_IDLE);/*we will just use ordinary idle mode*/
	YELLOW_LED_ON;		/*test leds*/	
	GREEN_LED_ON;
	_delay_ms(50);
	GREEN_LED_OFF;
	GPS_UartInit();		/*NMEA receiving (RX), GPS configuring (TX)*/
#if DEBUG
	db_uart_init();		/*Debug messages to terminal, TX only*/
	db_uart_puts_p(PSTR("Searching signals...\r"));
#endif
	state = SEARCH_SIGNALS;
	sei();

	while (1)
	{
		switch (state)
		{
			case SEARCH_SIGNALS:
						
						if (rmc_ready_flag == 1)
						{
							rmc_ready_flag = 0;
							ptr_to_valid = NULL;//clear old value
							ptr_to_valid = strchr( (const char*) rmc_rx_data_g, (int) 'V');// V- isn't hex
							
							/* found valid NMEA RMC data? */
							if (!ptr_to_valid)
							{
								/* Valid NMEA Data 'A' found */
								/* When we search satellite signals, we have had 
								 * a default GPS configuration and max power on....
								 * Now we configuring GPS module */
								GREEN_LED_ON;
								YELLOW_LED_ON;
								GPS_config();
								PCINT_Init();	/*Pushbutton Pin Change Interrupt, 
												  start/stop-button is ready to use after this*/
								YELLOW_LED_OFF;
								GREEN_LED_OFF;
								state = READY;
							}
							else {/* Invalid NMEA Data 'V', toggle once per second */
								TOGGLEBIT(YELLOW_LED_PORT,YELLOW_LED_PIN);
								#if DEBUG
									db_uart_puts_p(PSTR("Search valid signals...\r"));
								#endif
							}
						}
						break;
						
			case READY:	
			
						#if DEBUG 
							db_uart_puts_p(PSTR("Ready, press button...\r"));
							/* wait until transmit buffer is empty before enter sleep*/
							while(!(UCSR0A & (1 << UDRE0)));
							_delay_ms(1);
						#endif			
						
						//GPS_LowerPower();		/*TODO*/
						PRR0 = 0b11111111;		/*turn off all peripherals, only pushbutton wakes up system*/
						_enter_sleep();
						if ( _check_button()){	/*if pressed -> start tracking */
							GPS_UartInit();		/*just in case*/
							state = START;
						}
						else state = READY;
						break;
						
			case START:	
						YELLOW_LED_ON;
						SD_Initialize();
						YELLOW_LED_ON;
						#if DEBUG 
							db_uart_puts_p(PSTR("GPS time&date...\r") );			
						#endif

						while(!GPS_GetTimeAndDate());//wait there 'til ready
						
						#if DEBUG
							db_uart_puts_p(PSTR("UTC:") );
							db_uart_puts( (const char*)rmc.hhmmss );
							db_uart_puts_p(PSTR(" Date:") );
							db_uart_puts( (const char*)rmc.ddmmyy );
							db_uart_putc('\r');
						#endif
						
						if ( ! SD_CreateOpenedFile()){
							state = CLOSE_FILE;//something's wrong
						}
						else{
							GPS_EcoPower();//save power during tracking
							rmc_ready_flag = 0;
							state = SEARCH_VALID_RMC;
						}
						YELLOW_LED_OFF;
						GREEN_LED_ON;
						break;
						
			case SEARCH_VALID_RMC:/* wait valid GPS-NMEA-RMC data */

						/*period is configured in gps.c (7sec)*/
						if (rmc_ready_flag == 1)
						{
							rmc_ready_flag = 0;
							ptr_to_valid = NULL;//clear old value
							ptr_to_valid = strchr( (const char*) rmc_rx_data_g, (int) 'V');// V- isn't hex

							/* found valid NMEA RMC data? */
							if (!ptr_to_valid)
							{
								GREEN_LED_ON;//blink the green for write to file operation
								strcpy( (char*) rmc_last_valid_data_g, (const char*) rmc_rx_data_g );
								GPS_ProcessData(rmc_last_valid_data_g);//parser
								sprintf(line_to_file, "%s,%s,0\r\n", (const char*)rmc.longitude, (const char*)rmc.latitude );
								#if DEBUG 
									db_uart_puts((const char*)line_to_file);
								#endif
								state = WRITE_LINE;	//valid latitude'n'longitude received
								break;
							}
							else
							{
								/* Invalid RMC Data 'V' */
								YELLOW_LED_ON;
								GREEN_LED_OFF;
								#if DEBUG 
									db_uart_puts_p(PSTR("WAIT_VALID_POSITION...\r"));
								#endif
								_delay_ms(10);
								YELLOW_LED_OFF;
								state = SEARCH_VALID_RMC;	//keep state
							} 
						}
						if (_check_button())
							state = CLOSE_FILE;
						break;

			case WRITE_LINE :
			
						if ( ! SD_WriteToFile(line_to_file)){
							//db_uart_puts_p(PSTR("writing failed\n"));
							state = CLOSE_FILE;//something went wrong
							break;
						}
						else{
							GREEN_LED_OFF;//end of write operation
							PRR0 = 0b11101111;/*disable all peripherals except GPS-uart (wake up)*/
							_enter_sleep();
						}
						if ( _check_button() )
							state = CLOSE_FILE;
						else //GPS_uart RXComplete, keep on running
							state =  SEARCH_VALID_RMC;
						break;
						
			case CLOSE_FILE :
			
						YELLOW_LED_ON;
						#if DEBUG
							if (SD_CloseKmlFile()){
								db_uart_puts_p(PSTR("SD closed OK\npress button...\r"));
								/* wait until transmit buffer is empty before enter sleep*/
								while(!(UCSR0A & (1 << UDRE0)));
								_delay_ms(1);
							}
						#else
							SD_CloseKmlFile();
						#endif						
						YELLOW_LED_OFF;
						GREEN_LED_OFF;
						//GPS_LowerPower();		/*TODO: power down*/
						PRR0 = 0b11111111;		/*turn off all peripherals, only pushbutton wakes up system*/
						_enter_sleep();
						GPS_UartInit();			/*just in case*/	
						state = START;
						break;
						
			default:break;
		}
	}
}

/**************************************************************//**
 * return	1 if button pressed
 *			0 else
 *****************************************************************/	
static unsigned char _check_button()
{
	unsigned char rc = 0;	//return code
	
	if( BUTTON_PRESSED )
	{
		_delay_ms(80);
		while ( BUTTON_PRESSED == TRUE )
		{
			_delay_ms(10);
			rc = 1;
		}
		_delay_ms(50);
	}
	return(rc);
}

/**************************************************************//**
 * Save power, just use ordinary idle mode. 
 *****************************************************************/	
static void _enter_sleep(void)
{
	UNSELECT_CARD;	
	cli();
	sleep_enable();
	sleep_bod_disable();
	sei();
	sleep_cpu();
	sleep_disable();
	
#ifdef DEBUG
	PRR0 = 0b11101001;			/*enable SPI, GPS-uart & debug uart*/
#else
	PRR0 = 0b11101011;			/*else enable SPI & GPS-uart*/
#endif	
	SELECT_CARD;
}

/**************************************************************//**
 * 
 *****************************************************************/	
static void _init_io(void){

	/*
	0 - pin37 - //in PU, m23g wakes up external device, GPIO10 = gsmpin 39
	1 - pin36 - //in PU, m23g power off status, default low (connect), GPIO11 = gsmpin 27
	2 - pin35 - //out, m23g PWRON, pulled low for at least 120ms to operate, gsmpin 38
	3 - pin34 - NC input pulled up
	4 - pin33 - NC input pulled up
	5 - pin32 - NC input pulled up
	6 - pin31 - Pushbutton, Input PU
	7 - pin30 - yellow led, output hi -> led off
	*/
	DDRA =  0b10000000; // led out, others initialization IN 
	PORTA = 0b11111111; // inputs Pulled Up, led pin PA7 hi

	/*
	0 - Pin40 - NC input pulled up
	1 - Pin41 - NC input pulled up
	2 - Pin42 - NC input pulled up
	3 - Pin43 - NC input pulled up
	4 - Pin44 - NC input pulled up
	5 - Pin1 - MOSI input pulled up
	6 - Pin2 - MISO input pulled up
	7 - Pin3 - CLK input pulled up
	*/
	DDRB  = 0b00000000; // 
	PORTB = 0b11111111; // inputs Pulled Up,
	
	/*
	0 - Pin19 - NC input pulled up
	1 - Pin20 - debug conn, input pulled up
	2 - Pin21 - jtag, NC input pulled up
	3 - Pin22 - jtag, debug conn, input pulled up
	4 - Pin23 - jtag, NC input pulled up
	5 - Pin24 - jtag, NC input pulled up
	6 - Pin25 - SPI #Slave Select - output, external PU - chip is not selected yet
	7 - Pin26 - Debug LED green, output hi -> led off
	*/
	DDRC  = 0b11000000; // green led 7-out, SPI chip select 6-out
	PORTC = 0b11111111; // inputs Pulled Up, 6th&7th->hi->led off, SS-hi, 
	
	/*
	0 - Pin9  - RXD0, GSMTX
	1 - Pin10 - TXD0, GSMRX
	2 - Pin11 - RXD1, GPS
	3 - Pin12 - TXD1, GPS
	4 - Pin13 - NC input pulled up
	5 - Pin14 - NC input pulled up
	6 - Pin15 - GPS 1PPS input pulled up
	7 - Pin16 - GPS RST, input pulled up (CHANGE out)
	*/
	DDRD  = 0b00000010; // all in, excpt pin1
	PORTD = 0b11111111; // inputs Pulled Up
	
	/* Power Reduction Register0 
	0 - Bit 0 – PRADC: ADC
	1 - Bit 1 – PRUSART0: USART0
	2 - Bit 2 – PRSPI: Serial Peripheral Interface
	3 - Bit 3 – PRTIM1: Timer/Counter1
	4 - Bit 4 – PRUSART1: USART1
	5 - Bit 5 – PRTIM0: Timer/Counter0
	6 - Bit 6 – PRTIM2: Timer/Counter2
	7 - Bit 7 – PRTWI: TWI
	*/	
	PRR0 = 0b11101001;
	
	/*Bit 7 – ACD: Analog Comparator Disable, This will reduce power consumption in
	Active and Idle mode.
	*/
	ACSR = 0b10000000;
	
	_delay_ms(100);	//stabilizing
}


