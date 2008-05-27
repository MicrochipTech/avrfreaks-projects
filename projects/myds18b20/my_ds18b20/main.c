/* 
   DS18x20 Demo-Programm
   
   V 0.7, 3/2005
	PetrKpy 6/2007 modified for LCD 
   
   by Martin Thomas <eversmith@heizung-thomas.de>
   http://www.siwawi.arubi.uni-kl.de/avr-projects
    
   features:
   - DS18X20 and 1-Wire code is based on a sample from Peter 
     Dannegger
   - uses Peter Fleury's uart-library which is very portable 
     between AVRs, added some functions in the uart-lib
   - CRC-check based on code from Colin O'Flynn
   - access multiple sensors on multiple 1-Wire busses
   - samples how to address every sensor in the bus by ROM-code
   - independant of system-timers (more portable) but some
     (very short) delays used
   - avr-libc inttypes 
   - no central include-file, parts of the code can be used as
     "library"
   - verbose output (different levels configureable)
   - one-wire-bus can be changed at runtime if OW_ONE_BUS
     is not defined in onewire.h. There are still timing issues.
	 Tests done with ATmega16 3,68MHz XTAL OK, , 8MHz intRC OK, 
	 4MHz intRC OK, 2MHz intRC OK, 1,84MHz XTAL OK, 1MHz intRC 
	 failed in runtime-configureable OW-Bus. All frequencies do 
	 work in OW_ONE_BUS-Mode.
   - support-functions for DS18x20 internal EEPROM
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "onewire.h"
#include "ds18x20.h"

#include "delay.h"
#include "lcd.h"

#define MAXSENSORS 5

uint8_t gSensorIDs[MAXSENSORS][OW_ROMCODE_SIZE];

char lcd_buffer[19];
	
uint8_t search_sensors(void)
{
	uint8_t i;
	uint8_t id[OW_ROMCODE_SIZE];
	uint8_t diff, nSensors;
	
	/* clear display and home cursor */
    lcd_clrscr();
	lcd_puts("Bus scanning ...\n");
	
	nSensors = 0;
	
	for( diff = OW_SEARCH_FIRST;
		diff != OW_LAST_DEVICE && nSensors < MAXSENSORS ; )
	{
		DS18X20_find_sensor( &diff, &id[0] );
		
		if( diff == OW_PRESENCE_ERR ) {
			lcd_puts("No sensor found\n");
			break;
		}
		
		if( diff == OW_DATA_ERR ) {
			lcd_puts("Bus error\n");
			break;
		}
		
		for (i=0;i<OW_ROMCODE_SIZE;i++)
			gSensorIDs[nSensors][i]=id[i];
		
		nSensors++;
	}
	
	delay_ms(1000);
	return nSensors;
}

void lcd_put_temp(const uint8_t subzero, uint8_t cel, 
	uint8_t cel_frac_bits)
{
	char symbl=223;
	
	/*
	uint8_t buffer[sizeof(int)*8+1];
	uint8_t i, j;
	
	lcd_putc((subzero)?'-':'+');
	sprintf(lcd_buffer,"%d",(int)cel);
	lcd_puts(lcd_buffer);
	lcd_puts(".");
	itoa((cel_frac_bits*DS18X20_FRACCONV),buffer,10);
	j=4-strlen(buffer);
	for (i=0;i<j;i++) lcd_puts("0");
	lcd_puts(buffer);
	sprintf(lcd_buffer,"%cC\n",symbl);
	lcd_puts(lcd_buffer);
	*/
	
	// "rounding"
	uint16_t decicelsius;
	
	lcd_putc((subzero)?'-':'+');
	decicelsius = DS18X20_temp_to_decicel(subzero, cel, cel_frac_bits);
	sprintf(lcd_buffer,"%d", (int)(decicelsius/10) );
	lcd_puts(lcd_buffer);
	lcd_puts(".");
	sprintf(lcd_buffer,"%d", (decicelsius%10) + '0');
	lcd_puts(lcd_buffer);
	sprintf(lcd_buffer,"%cC\n",symbl);
	lcd_puts(lcd_buffer);
	
}

int main( void )
{
	uint8_t nSensors, i;
	uint8_t subzero, cel, cel_frac_bits;	
	
	// Edit ports by user
	#ifndef OW_ONE_BUS
	ow_set_bus(&PINB,&PORTB,&DDRB,PB0);
	#endif
	
	sei();
	
	/* initialize display, cursor off */
    lcd_init(LCD_DISP_ON);
	
	/*  Procedure for searching sensors */
	nSensors = search_sensors();
	
	/* clear display and home cursor */
    lcd_clrscr();
	
	/* number of searched sensors */
	sprintf(lcd_buffer,"%d",(int)nSensors);
	lcd_puts(lcd_buffer);
	lcd_puts( " DS18x20 sensor(s)\n" );
	
	#ifdef DS18X20_VERBOSE
	for (i=0; i<nSensors; i++) {
		lcd_puts("# in Bus :");
		sprintf(lcd_buffer,"%d",(int) i+1);
		lcd_puts(lcd_buffer);
	}
	#endif
	
	/* delay before next operation */
	delay_ms(2000);
	
	/* clear display and home cursor */
    lcd_clrscr();
	
	/* Types of searched sensors */
	for (i=0; i<nSensors; i++) 
	{
		lcd_puts("S");
		sprintf(lcd_buffer,"%d#",(int) i+1);
		lcd_puts(lcd_buffer);
		lcd_puts(" is ");
		if ( gSensorIDs[i][0] == DS18S20_ID)
			lcd_puts("DS18S20/1820\n");
		else lcd_puts("DS18B20\n");
		
	}
	
	/* delay before next operation */
	delay_ms(2000);
		
	if ( nSensors == 1 ) 
	{
		/* clear display and home cursor */
		lcd_clrscr();
		lcd_puts( "Only one sensor\n" ); 
		i = gSensorIDs[0][0]; // family-code for conversion-routine
		DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL );
		delay_ms(DS18B20_TCONV_12BIT);
		DS18X20_read_meas_single(i, &subzero, &cel, &cel_frac_bits);
		lcd_put_temp(subzero, cel, cel_frac_bits);
		/* delay before next operation */
		delay_ms(2000);
	}
		
	/* clear display and home cursor */
	lcd_clrscr();
	lcd_puts( "Read sensors by one\n" );
	
	/* delay before next operation */
	delay_ms(500);
	
	for(;;) 
	{	// main loop
		/* clear display and home cursor */
		lcd_clrscr();
		for ( i=0; i<nSensors; i++ ) 
		{
			if ( DS18X20_start_meas(DS18X20_POWER_PARASITE,&gSensorIDs[i][0]) == DS18X20_OK ) 
			{
				delay_ms(DS18B20_TCONV_12BIT);
				sprintf(lcd_buffer,"T#(%d) ",(int)i+1);
				lcd_puts(lcd_buffer);
				if ( DS18X20_read_meas( &gSensorIDs[i][0], &subzero, &cel, &cel_frac_bits) == DS18X20_OK ) 
				{
					lcd_put_temp(subzero, cel, cel_frac_bits);
				}
			}
			else lcd_puts("Start meas. failed\n");
		}
	}
}
