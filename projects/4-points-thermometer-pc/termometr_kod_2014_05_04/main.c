/* Name: main.c
 (
 * board wirings for "USB" board, Mega8 socket
 *
 * PD7 	 - pushbutton - not working?
 * PB0 	 - pushbutton,
 * PD2	- (INT0) pushbutton,
 *
 * PB2 	 - RED LED - to Vcc
 * PB1 	 - GREEN LED
 *
 * 
 * PD0	 - RxD - header to USB ->serial converter.
 * PD1	-  TxD - header to USB ->serial converter.
 * PD2 (INT0)  - USB_D+ (for V-USB), circuit with 68ohm and 3V6 Zerers, also auxiliary pushbutton.
 * PD4 (T0)    - USB_D-
 *
 * PC5	 - Dallas with 4k7 pull-up.
 * PC4	 - ADC4 - pot between GND and VCC.
 
 * PC0-3 - LCD data D4-D7
 * PD3   - LCD RS
 * PD5	 - LCD RW
 * PD6	 - LCD E
 *
 * AUTHOR (C) laurentp@wp.pl
 *
 * License: GPL2+
 *
 * 2013-10-26 - main program finished, missing only error handling,
 *		what is to do: configuration menu, serial connection.
 *		just added CRC over scratchpad (works!), with signalling dead sensor.
 *		have to add "delta T" measurement (?).
 * 2013-11-05 - everything works, configuration code is ugly, but works.
 *
 * 2013-11-07 - have to add serial comm.
 * 2013-12-10 - serial transmit works, 
 * 2013-12-12 - added pushbutton on INT0/PD2, as on PD7 does not work (LCD lib?)
 * 		so now have to add "delta" for "head" sensor?
 * 2013-12-23 - corrected displaying of 2nd digit in temp_units -> was 07.00 is  7.00
 * 2014-01-02 - changed descriptions of T's (doesn't matter for 4 line, but may for 2 line LCD)
 * 		added "buzzer_mask" byte -> each bit will be 0.1s beep, 0.2s at end always silent
 *
 * 2014-02-09 - killed bug in ioinit procedures causing PD7 incorrect behaviour, 
 *		also removed code to init LCD pins -> let the library handle it.
 *
 * 2014-02-21 - PAY ATTENTION TO CLOCK SOURCE!
 *
 *		moved code from PORTx / PINx to KEY_x / LED_x
 *		have to improve display procedures
 *
 * 2014-03-15 - deltas done
 *
 * 2014-04-06 -
 * 		programming: 1). set HFUSE to D2, 
 *				 set LFUSE to EF, 
 *				 set LOCK  to 3F, 
 *			     2). flash Optiboot (my version( using USBasp,
 *				 flash EEPROM using USBasp,
 *			     3). put processor in target board, press and hold S2,
 *			         power the board, 
 *			         flash program using "Arduino" / /dev/ttyUSB0
 *
 * 2014-04-11 -
 *		wrote keys filtering (but not quadrature encoder),
 *		added "press & hold" for KEY_3 -> column "ALARM MODE",
 *		added "press & hold" for KEY_4,
 *		added commas on serial.
 *		temperature on serial is always absolute -> column for now
 *
 * 2014-04-13 -
 *		correct buzzer control (patterns), 
 *		moved key sensing in CONFIG mode to soft keys
 * 		moving to store temperature labels in EEPROM, but for now
 *			we need an external ISP / program, to SET EEPROM (bootloader does not support)
 *
 * 2014-04-17 -
 (		begin removing "PANMUFINEK" specific mods from code, leaving only different EEPROM contents
 *		
 * 2014-05-04 -
 * 		added second alarm (head), cleaned code which handles errors (--.--), 
 * 		rewrite of some printing / displaying code.
 * 
 * 		CODING STYLE CLEANUP NEEDED!!!
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

/* laurent */
#include <avr/sleep.h>
#include "defines.h"


#include <util/delay.h>

/* laurent */
#include "iocompat.h"

#include <util/setbaud.h>

/* pay attention to LCD_START_LINEx in *.h */
#include "lcd.h"

#include <string.h> 
#include <stdlib.h>

/* laurent - Dallas */
#include "crc8.h"
#include "onewire.h"
/*#include "ds18x20.h" */


/* Laurent's begin */
#define uchar uint8_t
/* set Timer1 to 10ms		*/
const 	 	uint16_t Timer1_prescale = Timer1_prescale_default;
/* half period (10ms! */
const uint16_t half_period = 0.01 * (F_CPU / Timer1_prescale_default);

volatile	uint8_t		flagi, flagi2, lcd_flagi;
volatile	uint8_t		config_mode;
volatile	uint8_t		time;
volatile	uint8_t		deciseconds;
volatile	uint8_t		error_code;

volatile	uint8_t		buffer_tx[120];
volatile	uint8_t		tx_pointer;

#if defined INCLUDE_RTC
volatile	uint8_t 	second;
volatile	uint8_t		minute;
volatile	uint8_t		hour;
#endif

		uint8_t		alarm_type;
		uint8_t		buzzer_mask;

/*#if !defined PANMUFINEK */
    volatile	char		lcd_line1[17] = "T       =  0.00C";
/*#else
    volatile	char		lcd_line1[17] = "   Tpary=  0.00C";
#endif */
volatile	uint8_t		lcd_time[10] = "00:00:00,";

volatile	uint8_t		lcd_virt_line[17] = "                ";

volatile	uint8_t		virt_lcd_line2[49] = "T       =  0.00C        =  0.00C        =  0.00C";

volatile	uint8_t		num_of_sensors;

uint8_t			sensor_addreses[4][8];

uint8_t		temperature_labels[4][7];

/* built-in EEPROM */
const uint8_t		lcd_flags_in_eeprom		EEMEM = LCD_4LINE_FLAG;
const uint8_t		num_of_sensors_eeprom		EEMEM = 4;
const uint8_t		sensor_addreses_eeprom[4][8]	EEMEM = { 0x28,0x29,0x15,0x2C,0x02,0x00,0x00,0xBD,
								  0x28,0xBB,0xE4,0xCB,0x01,0x00,0x00,0x22,
								  0x28,0xAD,0x94,0xCC,0x00,0x00,0x00,0x8B,
								  0x28,0x82,0xAA,0x2D,0x05,0x00,0x00,0xCC };

#if !defined PANMUFINEK
    const uint8_t	temperature_labels_eeprom[4][8] EEMEM = { "kolumny",
								  "glowicy",
								  "  Tpary",
								  "Tcieczy" };
#else
    const uint8_t	temperature_labels_eeprom[4][8] EEMEM = { "   pary",
								  "glowicy",
								  "  Twody",
								  "  Tkega" };
#endif
/* /built-in EEPROM */

uint8_t		lcd_offset;
uint8_t		lcd_horiz_offset;

uint8_t		i, j, z;
uint8_t		scratchpad_table[4][9];
uint8_t		lcd_pointer;

uint8_t		temp_units[4];
uint16_t	temp_parts[4];
uint8_t		temp_sign[4];

uint16_t	temp_100s[4]; /* temperature as 1/100 degrees, for DELTA calculations  */

/* temporary storage */
uint8_t		temp_units_t;
uint16_t	temp_parts_t;
uint8_t		temp_sign_t;
uint16_t	temp_100s_t;
/* deltas */
uint8_t		temp_sign_d;
uint16_t	temp_100s_d;

uint16_t	temp_delta_base_100s[4] = { 0, 0, 0, 0 } ; /* temperature base for delta as 1/100 degrees */
uint16_t	temp_1_filter_10sec_100s[10];
uint16_t	temp_2_filter_10sec_100s[10];

volatile uint8_t	key_1_filter;
volatile uint8_t	key_2_filter;
volatile uint8_t	key_3_filter;
volatile uint8_t	key_4_filter;
volatile uint8_t	key_5_filter;
volatile uint8_t	key_6_filter;
volatile uint8_t	key_imp_filter;

volatile uint8_t	soft_keys;
volatile uint8_t	soft_keys2;
volatile uint8_t	soft_key_3_timer;
volatile uint8_t	soft_key_4_timer;
volatile uint8_t	soft_key_imp_timer;

const char	Initial_text[]		PROGMEM = "\n\r\n\rWersja 1.8 2014-05-04 (Mega8)\n\r(c) laurentp@wp.pl\n\r\n\r";

/* **********************************************************************/
/* counts ones in given (16-bit for now) code				*/
/* needed to calculate gap if CONST_LENGTH flag is used			*/
/* **********************************************************************/

uint8_t	ones_count8(uint8_t code, uint8_t bits) 
{

uint8_t		temp_counter;
uint8_t 	temp_count;

    if (bits == 0) { return(0); }

    temp_count = 0;
    code <<= (8 - bits);
    for (temp_counter = 0; temp_counter < bits; temp_counter++) {
	if ((code & 0x80) == 0x80) { temp_count++;
					}
	code <<= 1;
							      }
    return(temp_count);
}

/* ------------------------------------------ */
/* every 10ms */

ISR (TIMER1_COMPA_vect)
{
    time++;

/* keyboard filtering */
    key_1_filter <<= 1;
    key_1_filter &= 0x0F;
    if ((KEY_1_PINR & _BV(KEY_1_BIT)) == 0) { key_1_filter |= 1; }
    if (ones_count8(key_1_filter, 4) > 1) { soft_keys |= SOFT_KEY_1_MASK; }
	else 				 { soft_keys &= ~SOFT_KEY_1_MASK; }

    key_2_filter <<= 1;
    key_2_filter &= 0x0F;
    if ((KEY_2_PINR & _BV(KEY_2_BIT)) == 0) { key_2_filter |= 1; }
    if (ones_count8(key_2_filter, 4) > 1) { soft_keys |= SOFT_KEY_2_MASK; }
	else 				 { soft_keys &= ~SOFT_KEY_2_MASK; }

#if defined PRODUCTION_BOARD
/* keyboard filtering */
    key_3_filter <<= 1;
    key_3_filter &= 0x0F;
    if ((KEY_3_PINR & _BV(KEY_3_BIT)) == 0) { key_3_filter |= 1; }
    if (ones_count8(key_3_filter, 4) > 1) { soft_keys |= SOFT_KEY_3_MASK; }
	else 				 { soft_keys &= ~SOFT_KEY_3_MASK; }

    key_4_filter <<= 1;
    key_4_filter &= 0x0F;
    if ((KEY_4_PINR & _BV(KEY_4_BIT)) == 0) { key_4_filter |= 1; }
    if (ones_count8(key_4_filter, 4) > 1) { soft_keys |= SOFT_KEY_4_MASK; }
	else 				 { soft_keys &= ~SOFT_KEY_4_MASK; }

    key_5_filter <<= 1;
    key_5_filter &= 0x0F;
    if ((KEY_5_PINR & _BV(KEY_5_BIT)) == 0) { key_5_filter |= 1; }
    if (ones_count8(key_5_filter, 4) > 1) { soft_keys |= SOFT_KEY_5_MASK; }
	else 				 { soft_keys &= ~SOFT_KEY_5_MASK; }

    key_6_filter <<= 1;
    key_6_filter &= 0x0F;
    if ((KEY_6_PINR & _BV(KEY_6_BIT)) == 0) { key_6_filter |= 1; }
    if (ones_count8(key_6_filter, 4) > 1) { soft_keys |= SOFT_KEY_6_MASK; }
	else 				 { soft_keys &= ~SOFT_KEY_6_MASK; }
	
#endif
/* quick & dirty but working */
/* every 100ms */
    if (time % 10 == 0) { flagi |= MS100_FLAG; 

	if ((time / 10) < 8) { if (alarm_type != 0) { switch (alarm_type) {
						    case 1: buzzer_mask = 0x01;
							    break;
						    case 2: buzzer_mask = 0x03;
							    break;
						    case 3: buzzer_mask = 0x0F;
							    break;
						    case 4: buzzer_mask = 0x11;
							    break;
						    case 5: buzzer_mask = 0x75;
							    break;
						    case 6: buzzer_mask = 0xFF;
							    break;
								     }
						buzzer_mask <<= (time / 10);
						if ((buzzer_mask & 0x80) != 0)  {
							   LED_1_PORT &= ~_BV(LED_1_BIT);
					    					}
						    else { LED_1_PORT |= _BV(LED_1_BIT); }
						}
    /* debug */
/*		virt_lcd_line2[16] = alarm_type + '0'; */
    /* /debug */
			 }
	else 		 { LED_1_PORT |= _BV(LED_1_BIT); }

#if defined PRODUCTION_BOARD
			  if (((soft_keys & SOFT_KEY_3_MASK) != 0) && 
			     ((soft_keys & SOFT_KEY_3_OLD_MASK) == 0)) { flagi2 ^= COLUMN_IS_DELTA_FLAG;
								    if ((flagi2 & COLUMN_IS_DELTA_FLAG) != 0) {
									    temp_delta_base_100s[0] = temp_100s[0];
									    lcd_line1[0] = 'D';	      	      }
								    else {  lcd_line1[0] = 'T';
									    flagi2 &= ~COLUMN_ALARM_ENABLED_FLAG;
									    if ((alarm_type == 1) || 
									        (alarm_type == 2) || 
									        (alarm_type == 3 ))   { alarm_type = 0; }
									 }
								       }

			  if (((soft_keys & SOFT_KEY_4_MASK) != 0) && 
			      ((soft_keys & SOFT_KEY_4_OLD_MASK) == 0)) { flagi2 ^= HEAD_IS_DELTA_FLAG;
								    if ((flagi2 & HEAD_IS_DELTA_FLAG) != 0) {
									temp_delta_base_100s[1] = temp_100s[1];
									virt_lcd_line2[0] = 'D';	      }
								    else { virt_lcd_line2[0] = 'T';
									   flagi &= ~HEAD_ALARM_ENABLED_FLAG;
									   if ((alarm_type == 1) || 
									       (alarm_type == 2) || 
									       (alarm_type == 3 ))   { alarm_type = 0; }
									 }
									}
/* quick & dirty */
			  if ((soft_keys & SOFT_KEY_2_MASK) != 0) { alarm_type = 0;
								    if ((flagi2 & COLUMN_IS_DELTA_FLAG) != 0) {
									lcd_line1[0] = 'D';		      }
								    else {
									lcd_line1[0] = 'T';
									 }
								  }
#else                                /* test board */
			  if (((soft_keys & SOFT_KEY_2_MASK) != 0) && 
			      ((soft_keys2 & SOFT_KEY_2_OLD_MASK) == 0)) { flagi2 ^= COLUMN_IS_DELTA_FLAG;
								    if ((flagi2 & COLUMN_IS_DELTA_FLAG) != 0) {
									temp_delta_base_100s[0] = temp_100s[0];
									lcd_line1[0] = 'D';	      	      }
								    else { lcd_line1[0] = 'T'; }
								  }
#endif
			/* should NOT use eeprom_write, as it may take 10ms to complete... */
			  if (((soft_keys & SOFT_KEY_1_MASK) != 0) && 
			      ((soft_keys2 & SOFT_KEY_1_OLD_MASK) == 0) && 
			       (flagi & IN_CONFIG_FLAG) == 0)	    {
							if((lcd_flagi & LCD_4LINE_FLAG) != 0) { lcd_flagi &= ~LCD_4LINE_FLAG;
												  lcd_flagi &= ~LCD_OPERATION_MODE_FLAG;
											      }
							else if ((lcd_flagi & LCD_OPERATION_MODE_FLAG) == 0) {
								lcd_flagi |= LCD_OPERATION_MODE_FLAG; 	     }
								else {  lcd_flagi |= LCD_4LINE_FLAG; }
							eeprom_write_byte(&lcd_flags_in_eeprom, lcd_flagi);
								    }
			/* "press and hold for key 3 - 1st lcd line */
			  if (((soft_keys & SOFT_KEY_3_MASK) != 0) && 
			      ((soft_keys & SOFT_KEY_3_OLD_MASK) != 0) && 
			      ((soft_key_3_timer < 100))) { soft_key_3_timer++; }
			  if ((soft_keys & SOFT_KEY_3_MASK) == 0) { soft_key_3_timer = 0; }
			/* "press and hold for key 4 - 2nd lcd line */
			  if (((soft_keys & SOFT_KEY_4_MASK) != 0) && 
			      ((soft_keys & SOFT_KEY_4_OLD_MASK) != 0) && 
			      ((soft_key_4_timer < 100))) { soft_key_4_timer++; }
			  if ((soft_keys & SOFT_KEY_4_MASK) == 0) { soft_key_4_timer = 0; }

			  if ((soft_key_3_timer > 20) && 
			     ((flagi2 & COLUMN_IS_DELTA_FLAG) != 0)) { flagi2 |= COLUMN_ALARM_ENABLED_FLAG; }
			  if ((soft_key_4_timer > 20) && 
			     ((flagi2 & HEAD_IS_DELTA_FLAG) != 0)) { flagi |= HEAD_ALARM_ENABLED_FLAG; }

			/* update OLD soft keys */
			if ((soft_keys & SOFT_KEY_3_MASK) != 0) { soft_keys |= SOFT_KEY_3_OLD_MASK; }
			else					{ soft_keys &= ~SOFT_KEY_3_OLD_MASK; }
			if ((soft_keys & SOFT_KEY_4_MASK) != 0) { soft_keys |= SOFT_KEY_4_OLD_MASK; }
			else					{ soft_keys &= ~SOFT_KEY_4_OLD_MASK; }
			if ((soft_keys & SOFT_KEY_1_MASK) != 0) { soft_keys2 |= SOFT_KEY_1_OLD_MASK; }
			else					{ soft_keys2 &= ~SOFT_KEY_1_OLD_MASK; }
			if ((soft_keys & SOFT_KEY_2_MASK) != 0) { soft_keys2 |= SOFT_KEY_2_OLD_MASK; }
			else					{ soft_keys2 &= ~SOFT_KEY_2_OLD_MASK; }
			if ((soft_keys & SOFT_KEY_5_MASK) != 0) { soft_keys2 |= SOFT_KEY_5_OLD_MASK; }
			else					{ soft_keys2 &= ~SOFT_KEY_5_OLD_MASK; }
			if ((soft_keys & SOFT_KEY_6_MASK) != 0) { soft_keys2 |= SOFT_KEY_6_OLD_MASK; }
			else					{ soft_keys2 &= ~SOFT_KEY_6_OLD_MASK; }
			
			} /* /every 100ms */

    if (time == 50)  { flagi |= MS500_FLAG; }
    /* 1s elapsed */
    if (time == 100) { time = 0;
		       flagi |= S1_FLAG | MS500_FLAG;
			/* RTC */
#if defined INCLUDE_RTC
		       second++;
		       if (second > 59) { second = 0;
					  minute++;
					  if (minute > 59) { minute = 0;
							     hour++;
							     if (hour > 23) { hour = 0; }
							   }
					}
#endif
		     }
}

/* ------------------------------------------ */

ISR (USART_UDRE_vect)
{
    if ((flagi & FLAG_SERIAL_TX_TO_SEND) == FLAG_SERIAL_TX_TO_SEND) {
	if (buffer_tx[tx_pointer] != 0) { UDR0 = buffer_tx[tx_pointer++]; }
	else {
	    UCSR0B &= ~_BV(UDRIE0);	/* disARM Tx buffer empty interrupt */
	    for (tx_pointer = 0; tx_pointer < (sizeof buffer_tx) ; tx_pointer++)
		{ buffer_tx[tx_pointer] = 0; }	/* clear buffer_tx */
	    tx_pointer = 0;
	    flagi &= ~FLAG_SERIAL_TX_TO_SEND;
	    }
	} else
    { UCSR0B &= ~_BV(UDRIE0); }		/* disARM Tx buffer empty interrupt */
						/* spurious interrupt */
}

/* ------------------------------------------ */
/* prepares (decodes) byte into buffer_tx, as decimal no 100's (for time)  */
/* ------------------------------------------ */
#if defined INCLUDE_RTC
void prepare_time_to_display(void)
{
	lcd_time[0] = (hour / 10) + '0';
	lcd_time[1] = (hour % 10) + '0';
	lcd_time[2] = ':';
	lcd_time[3] = (minute / 10) + '0';
	lcd_time[4] = (minute % 10) + '0';
	lcd_time[5] = ':';
	lcd_time[6] = (second / 10) + '0';
	lcd_time[7] = (second % 10) + '0';
	lcd_time[8] = ',';
}
#endif

/* -------------------------------------------- */

void
Send_Initial_text(void)
{
/* wait until previous buffer is sent */
      while ((flagi & FLAG_SERIAL_TX_TO_SEND) != 0) { sleep_mode(); }
      strcpy_P((char *)&buffer_tx, (PGM_P)&Initial_text);

/* arming serial Tx  */
	flagi |= FLAG_SERIAL_TX_TO_SEND;			/* buffer_tx to send */
	tx_pointer = 0;			/* reset pointer */
	UCSR0B |= _BV(UDRIE0);		/* ARM Tx buffer empty interrupt */
    /* wait to complete */
	while ((flagi & FLAG_SERIAL_TX_TO_SEND) != 0) { sleep_mode(); }
}

/* -------------------------------------------- */
/* display "virtual" LCD, using scroll and	*/
/* horizontal scroll				*/
/* -------------------------------------------- */

void LCD_virt_display(void) {
    uint8_t i, j;
		/* ONLY every 500ms/1s 	  */
		/* Virtual 4 line display */
	    lcd_gotoxy(0,0);
	    lcd_puts(&lcd_line1);
	    lcd_gotoxy(0,1);
	    if ((lcd_flagi & LCD_4LINE_FLAG) == 0) { /* 2 lines display, scroll it */
		for (i = 0; i < 16; i++) {
		    if ((lcd_flagi & LCD_OPERATION_MODE_FLAG) != 0) {
		    /* horizontal scroll */
			j = (i + lcd_horiz_offset);
			if (j > 47) { j -= 48; }
								    }
		    else {    /* vertical scroll */
			j = (i + (16 * lcd_offset));
		         }
		lcd_putc(virt_lcd_line2[j]);
					} /* for */
						   } /* 2 lines display */
	    else { /* 4 lines display */
		for (i = 0; i < 16; i++) {
		    lcd_gotoxy(i,1);
		    lcd_putc(virt_lcd_line2[i]);
		    lcd_gotoxy(i,2);
		    lcd_putc(virt_lcd_line2[i + 16]);
		    lcd_gotoxy(i,3);
		    lcd_putc(virt_lcd_line2[i + 32]);
					 }
		 }
/* debug - try to detect 4 line display */
/*	    lcd_gotoxy(0,3);
	    lcd_putc('0');
	    lcd_gotoxy(0,2);

	    i = lcd_read(1);
	    lcd_gotoxy(0,1);
	    lcd_putc(i); */
/* /debug */
			}

/* *************************************************************************/

void prepare_hex_byte_to_lcd1(uint8_t bajt)
{
	if (((bajt & 0xF0) >> 4) < 10) { 
	    lcd_line1[lcd_pointer++] = ((bajt & 0xF0) >> 4) + '0'; }
	else { lcd_line1[lcd_pointer++] = ((bajt & 0xF0) >> 4) + 'A' -10; }
	if ((bajt & 0x0F) < 10) { 
	    lcd_line1[lcd_pointer++] = (bajt & 0x0F) + '0'; }
	else { lcd_line1[lcd_pointer++] = (bajt & 0x0F) + 'A' -10; }
}

/* *************************************************************************/

void adress_dallas(uint8_t dallas_idx) {
    uint8_t i;
	ow_reset();
	ow_byte_wr(OW_MATCH_ROM);
	for (i = 0; i < 8; i++) {
	    ow_byte_wr(sensor_addreses[dallas_idx][i]);
				}
					}

/* *************************************************************************/
/* should return ZERO if CRC is correct */

uint8_t read_dallas(uint8_t sensor_idx) {
    uint8_t i;
	ow_byte_wr(DS18X20_READ_SCRATCHPAD);
	    for (i = 0; i < 9; i++) {
		scratchpad_table[sensor_idx][i] = ow_byte_rd();
				    }
	return crc8(&scratchpad_table[sensor_idx][0],9);
				     }

/* *************************************************************************/
/* decodes scratchpads into temperature sign/value/decimals */

void decode_dallas(uint8_t sensor_idx) {
	/* decode Temp, depending on sensor family */
	temp_sign[sensor_idx] = (scratchpad_table[sensor_idx][1] & 0xF0) >> 4;
	switch (sensor_addreses[sensor_idx][0]) {
	    case 0x28: /* DS18B20 12bit model */
		if (temp_sign[sensor_idx] != 0) { /* negative NOT working? */ 
			      temp_units[sensor_idx] = ~((scratchpad_table[sensor_idx][0] >> 4) | (scratchpad_table[sensor_idx][1] << 4));
			      temp_parts[sensor_idx] = 625 * ~(scratchpad_table[sensor_idx][0] & 0x0F); }
		    else	{ /* positive */
			      temp_units[sensor_idx] = (scratchpad_table[sensor_idx][0] >> 4) | (scratchpad_table[sensor_idx][1] << 4);
			      temp_parts[sensor_idx] = 625 * (scratchpad_table[sensor_idx][0] & 0x0F); }
		break;
	    case 0x10: /* DS1820 OLD model */
		       /* have to add negative handling */
		temp_units[sensor_idx] = scratchpad_table[sensor_idx][0] >> 1;
		temp_parts[sensor_idx] = 5000 * (scratchpad_table[sensor_idx][0] & 0x01);
		break;
						} /* switch */
	temp_100s[sensor_idx] = (uint16_t)temp_units[sensor_idx] * 100 + (temp_parts[sensor_idx] / 100);
	if ((temp_parts[sensor_idx] % 100) > 50) { temp_100s[sensor_idx]++; }
					}

/* *************************************************************************/

void prepare_hex_byte_to_lcd2(int8_t bajt) {
	if (((bajt & 0xF0) >> 4) < 10) { 
	    virt_lcd_line2[lcd_pointer++] = ((bajt & 0xF0) >> 4) + '0'; }
	else { virt_lcd_line2[lcd_pointer++] = ((bajt & 0xF0) >> 4) + 'A' -10; }
	if ((bajt & 0x0F) < 10) { 
	    virt_lcd_line2[lcd_pointer++] = (bajt & 0x0F) + '0'; }
	else { virt_lcd_line2[lcd_pointer++] = (bajt & 0x0F) + 'A' -10; }
								}

/* *************************************************************************/

#include "ioinit.c"

int main(void)
{
/*    uint8_t	x,y,z; */

    ioinit();

    num_of_sensors = eeprom_read_byte(&num_of_sensors_eeprom);
    if (num_of_sensors > 5) { num_of_sensors = 0; }

    lcd_flagi = eeprom_read_byte(&lcd_flags_in_eeprom);
    if (lcd_flagi = 0xff) { lcd_flagi = LCD_DEFAULT_FLAGS; }

    for (i = 0; i < num_of_sensors; i++) {
	for (j = 0; j < 8; j++) {
	    sensor_addreses[i][j] = eeprom_read_byte(&sensor_addreses_eeprom[i][j]);
				}
					 }
    /* read temperature labels from EEPROM */
    for (i = 0; i < 4; i++) {
	for (j = 0; j < 7; j++) {
	    temperature_labels[i][j] = eeprom_read_byte(&temperature_labels_eeprom[i][j]);
			    }
					 }
    /* copy them to (virtual) display */
    memcpy(&lcd_line1[1], &temperature_labels[0][0], 7);
    memcpy(&virt_lcd_line2[1], &temperature_labels[1][0], 7);
    memcpy(&virt_lcd_line2[17], &temperature_labels[2][0], 7);
    memcpy(&virt_lcd_line2[33], &temperature_labels[3][0], 7);
        
    /* switch off LEDs */
    LED_1_PORT |= _BV(LED_1_BIT);
#if !defined PRODUCTION_BOARD
    LED_2_PORT |= _BV(LED_2_BIT);
#endif
    lcd_init(LCD_DISP_ON);

    Send_Initial_text();

    if ((KEY_1_PINR & _BV(KEY_1_BIT)) == 0) { flagi |= IN_CONFIG_FLAG; }

    if ((num_of_sensors == 0) || (num_of_sensors == 0xFF)) { flagi |= IN_CONFIG_FLAG; }

    if (( flagi & IN_CONFIG_FLAG ) == 0) { /* normal work */
      
    for(;;) { /* main event loop */
/*        wdt_reset(); */
	sleep_mode();
/* ---- 100ms -------------------------------------- */
	if ((flagi & MS100_FLAG) != 0) {
	    flagi &= ~MS100_FLAG;
				       }
/* ---- 100ms test --------------------------------- */

	if ((flagi & MS500_FLAG) != 0) {
	    flagi &= ~MS500_FLAG;
#if defined HEARTBEAT_LED
	    LED_1_PORT ^= _BV(LED_1_BIT);
#endif
/* debug */
	if ((soft_keys & SOFT_KEY_6_MASK) != 0) { alarm_type--; }
	if ((soft_keys & SOFT_KEY_5_MASK) != 0) { alarm_type++; }
/* /debug */

	    /* LCD horizontal scroll*/
	    if (lcd_horiz_offset < 48)  { lcd_horiz_offset++; }
		else			{ lcd_horiz_offset = 0; }

	    LCD_virt_display();
				       } /* 500ms test */

/* ------------------------------------------ */

/* 1s */
	if ((flagi & S1_FLAG) != 0) {
	    flagi &= ~S1_FLAG;

	    prepare_time_to_display();
	    memcpy(&buffer_tx[0], &lcd_time[0], 9);
	    /* read Dallas */
	    lcd_pointer = 0;
	    for (j = 0; j < num_of_sensors; j++) {
		adress_dallas(j);
		error_code = read_dallas(j);
		if (error_code == 0) {		  
		    decode_dallas(j);
		    /* display temperature on VIRTUAL display*/
		    temp_units_t = temp_units[j];
		    temp_parts_t = temp_parts[j];
		    temp_sign_t = temp_sign[j];
		    temp_100s_t = temp_100s[j];
		    /* always calcuate delta */
		    if (temp_100s_t >= temp_delta_base_100s[j]) {
		        temp_100s_d = temp_100s_t - temp_delta_base_100s[j];
		        temp_sign_d = 0;		   }
		    else {
		        temp_100s_d = temp_delta_base_100s[j] - temp_100s_t;
		        temp_sign_d = 1;		   }

		    switch (j) {
		        case 0:	
			        buffer_tx[9] = 'T';
			  	/* copy labels 1st line */
			        memcpy(&buffer_tx[10],&temperature_labels[0][0], 7);
				buffer_tx[17] = '=';
				buffer_tx[21] = '.';
				buffer_tx[24] = 'C';
				/* print (possibly) 100's */
				if (temp_100s_t > 10000)    	{ buffer_tx[18] = '1'; }
				    else			{ buffer_tx[18] = ' '; }
				if (temp_100s_t < 1000)  	{ buffer_tx[19] = ' '; }
				    else		 	{ buffer_tx[19] = ( temp_100s_t % 10000) / 1000 + '0'; }
				buffer_tx[20] = (temp_100s_t  % 1000) / 100  + '0';
				buffer_tx[22] = (temp_100s_t % 100) / 10 + '0';
				switch (sensor_addreses[j][0]) {
				    case 0x28: /* DS18B20 12bit model */
					buffer_tx[23] = (temp_100s_t % 10) + '0';
					break;
				    case 0x10: /* OLD 1820 0.5deg resolution */
					buffer_tx[23] = '-';
					break;
								}

				/* FIRST digit (100's) / SIGN */
				if ((flagi2 & COLUMN_IS_DELTA_FLAG) != 0) {
				/* for deltas display sign */
				    if (temp_sign_d == 0)     { lcd_line1[9] = '+'; }
					else		      { lcd_line1[9] = '-'; }

				    if (temp_100s_d < 1000)  { lcd_line1[10] = ' '; }
						      else   { lcd_line1[10] = (temp_100s_d % 10000) / 1000 + '0'; }

				    lcd_line1[11] = (temp_100s_d  % 1000) / 100  + '0';
				    lcd_line1[13] = (temp_100s_d % 100) / 10 + '0';

				    switch (sensor_addreses[j][0]) {
					case 0x28: /* DS18B20 12bit model */
					    lcd_line1[14] = (temp_100s_d % 10) + '0';
					    break;
					case 0x10: /* OLD 1820 0.5deg resolution */
					    lcd_line1[14] = '-';
					    break;
								    }

									  } /* COLUMN_IS_DELTA != 0  */
				
				else { /* COLUMN_IS_DELTA == 0  */
				       /* work is already done for printing, so don't do twice */
				       memcpy(&lcd_line1[0], &buffer_tx[9], 16);
				     } /* COLUMN_IS_DELTA == 0  */

				if ((flagi2 & COLUMN_ALARM_ENABLED_FLAG) != 0) {
				    lcd_line1[0] = 'A';
				    if (temp_100s_d > 15)	{ if (alarm_type == 0) { alarm_type = 1; }
										  else { alarm_type = 3; }
								}
									       }
				 break;

			    case 1: /* initialize output 2nd line */
				    memcpy(&buffer_tx[26],&virt_lcd_line2[0], 16);
				    buffer_tx[26] = 'T';

				    if ((temp_100s_t / 10000) > 0)  { buffer_tx[35] = '1'; }
						else		    { buffer_tx[35] = ' '; }
				    if (temp_100s_t < 1000)  	{ buffer_tx[36] = ' '; }
				      else		 	{ buffer_tx[36] = ( temp_100s_t % 10000) / 1000 + '0'; }
				    buffer_tx[37] = (temp_100s_t  % 1000) / 100  + '0';
				    buffer_tx[39] = (temp_100s_t % 100) / 10 + '0';
				    switch (sensor_addreses[j][0]) {
					case 0x28: /* DS18B20 12bit model */
					    buffer_tx[40] = (temp_100s_t % 10) + '0';
					    break;
					case 0x10: /* OLD 1820 0.5deg resolution */
					    buffer_tx[40] = '-';
					    break;
								   }
				    /* finished printing, now displaying */
						
				    if ((flagi2 & HEAD_IS_DELTA_FLAG) != 0) {
				    /* for deltas display sign */
				    if (temp_sign_d == 0)     { virt_lcd_line2[9] = '+'; }
				      else		      { virt_lcd_line2[9] = '-'; }

				      if (temp_100s_d <  1000) 		{ virt_lcd_line2[10] = ' '; }
					else 				{ virt_lcd_line2[10] = (temp_100s_d % 10000) / 1000 + '0'; }
					
				      virt_lcd_line2[11] = (temp_100s_d % 1000) / 100 + '0';
				      virt_lcd_line2[13] = ((temp_100s_d % 100) / 10)+ '0';	
				      switch (sensor_addreses[j][0]) {
					    case 0x28: /* DS18B20 12bit model */
						virt_lcd_line2[14] = (temp_100s_d % 10) + '0';
						break;
					    case 0x10: /* OLD 1820 model */
						virt_lcd_line2[14] = '-';
						break;
								      }
									    }
									    
				    else {  /* HEAD_IS_DELTA_FLAG == 0 */
					    /* display (possibly) 100's */
					if ((temp_100s_t / 10000) > 0)  { virt_lcd_line2[9] = '1'; }
						else			{ virt_lcd_line2[9] = ' '; }

					if (temp_100s_t <  1000) 	{ virt_lcd_line2[10] = ' '; }
					else 				{ virt_lcd_line2[10] = (temp_100s_t % 10000) / 1000 + '0'; }
					
					virt_lcd_line2[11] = (temp_100s_t % 1000) / 100 + '0';
					virt_lcd_line2[13] = ((temp_100s_t % 100) / 10)+ '0';	
					switch (sensor_addreses[j][0]) {
					    case 0x28: /* DS18B20 12bit model */
						virt_lcd_line2[14] = (temp_100s_t % 10) + '0';
						break;
					    case 0x10: /* OLD 1820 model */
						virt_lcd_line2[14] = '-';
						break;
									}
				         }
				if ((flagi & HEAD_ALARM_ENABLED_FLAG) != 0) {
				    virt_lcd_line2[0] = 'A';
				    if (temp_100s_d > 10)	{ if (alarm_type == 0) { alarm_type = 2; }
										  else { alarm_type = 3; }
								}
									    }
				break;

			    case 2:
			    case 3: 
				      if ((temp_100s_t / 10000) > 0)    { virt_lcd_line2[9 + (16 * (j - 1))] = '1'; }
					    else			{ virt_lcd_line2[9 + (16 * (j - 1))] = ' '; }

					    if (temp_100s_t <  1000) { virt_lcd_line2[10 + (16 * (j - 1))] = ' '; }
					else {			   virt_lcd_line2[10 + (16 * (j - 1))] = (temp_100s_t % 10000) / 1000 + '0';
					     }
					virt_lcd_line2[11 + (16 * (j - 1))] = (temp_100s_t % 1000) / 100 + '0';
					virt_lcd_line2[13 + (16 * (j - 1))] = ((temp_100s_t % 100) / 10)+ '0';
					switch (sensor_addreses[j][0]) {
					    case 0x28: /* DS18B20 12bit model */
						virt_lcd_line2[14 + (16 * (j - 1))] = (temp_100s_t % 10) + '0';
						break;
					    case 0x10: /* OLD 1820 model */
						virt_lcd_line2[14 + (16 * (j - 1))] = '-';
						break;
									}
					break;
				} /* switch J*/
			if (j == 3) { 
			    buffer_tx[25] = ',';
			    buffer_tx[42] = ',';
			    memcpy(&buffer_tx[43], &virt_lcd_line2[16], 16);
			    buffer_tx[59] = ',';
			    memcpy(&buffer_tx[60], &virt_lcd_line2[32], 16);
			    buffer_tx[76] = '\r';
			    buffer_tx[77] = '\n';
			    buffer_tx[78] = 0;

			    flagi |= FLAG_SERIAL_TX_TO_SEND;  /* buffer_tx to send */
			    tx_pointer = 0;			 /* reset pointer */
			    UCSR0B |= _BV(UDRIE0);		 /* ARM Tx buffer empty interrupt */
				     }
				      } /* if error_code */
			else {
				/* error handling */
				switch (j) {
				    case 0: /* Tgłowicy */
					    lcd_line1[9] = ' ';
					    lcd_line1[10] = '-';
					    lcd_line1[11] = '-';
					    lcd_line1[13] = '-';
					    lcd_line1[14] = '-';
					/* start printing on serial */
					    memcpy(&buffer_tx[9], &lcd_line1[0], 16);
					    buffer_tx[25] = ',';
					    buffer_tx[9] = 'T';
					 break;
				    case 1: 
				    case 2:
				    case 3:
					    virt_lcd_line2[9 + 16 * (j - 1)] = ' ';
					    virt_lcd_line2[10 + 16 * (j - 1)] = '-';
					    virt_lcd_line2[11 + 16 * (j - 1)] = '-';
					    virt_lcd_line2[13 + 16 * (j - 1)] = '-';
					    virt_lcd_line2[14 + 16 * (j - 1)] = '-';
					    memcpy(&buffer_tx[9 + 16 * j + j], &virt_lcd_line2[(16 * (j - 1))], 16);
					    buffer_tx[8 + 16 * j + j] = ',';
					    if (j == 1) { buffer_tx[26] = 'T'; }
					 break;
					   } /* switch J */
				if (j == 3) {   /* WHATS the difference?? */
						/* buffer_tx[(25 + (16 * j) + j)] = '\r';
						buffer_tx[(25 + (16 * j) + j)] = '\n';
						buffer_tx[(25 + (16 * j) + j)] = 0; */
						buffer_tx[76] = '\r';
						buffer_tx[77] = '\n';
						buffer_tx[78] = 0;
						
						flagi |= FLAG_SERIAL_TX_TO_SEND;  /* buffer_tx to send */
						tx_pointer = 0;			 /* reset pointer */
	    					UCSR0B |= _BV(UDRIE0);		 /* ARM Tx buffer empty interrupt */
							  }			
			      } /* /error handling */
						 } /* for J */
	    /* start temperature measuring again */
	    for (j = 0; j < num_of_sensors; j++) {
		    adress_dallas(j);
		    ow_byte_wr(DS18X20_CONVERT_T);
						 } /* for J */
	    /* vertical scroll */
	    if (lcd_offset < 2) { lcd_offset++; }
		else		{ lcd_offset = 0; }

				      } /* /S1_FLAG test */

			} /* neverending for */

	} /* IF !IN_CONFIG */
	else { /* IN CONFIG */
		lcd_gotoxy(0,0); 
		lcd_puts("Tryb konfigur.");
		lcd_gotoxy(0,1);
		lcd_puts("Liczba czujn.:");
		lcd_putc(num_of_sensors + '0');

		_delay_ms(2000);

		for (z = 0; z < 4; z++) {
		    lcd_gotoxy(0,0);
		    switch (z) {
/* #if !defined PANMUFINEK */
			case 0:
				lcd_putc('T');
				for (i = 0; i < 7; i++) {
				    lcd_putc(temperature_labels[0][i]);
							}
/*#else
			case 0: lcd_puts("   Para");
#endif */
				break;
			case 1:
				lcd_putc('T');
				for (i = 0; i < 7; i++) {
				    lcd_putc(temperature_labels[1][i]);
							}
				break;
/* #if !defined PANMUFINEK */
			case 2:
				lcd_putc(' ');
				for (i = 0; i < 7; i++) {
				    lcd_putc(temperature_labels[2][i]);
							}
				break;
			case 3:
				lcd_putc(' ');
				for (i = 0; i < 7; i++) {
				    lcd_putc(temperature_labels[3][i]);
							}
/* #else
			case 2: lcd_puts("Woda   ");
				break;
			case 3: lcd_puts("Keg    ");
#endif */
				}
		    lcd_puts(":        ");

		    while ((soft_keys & SOFT_KEY_1_MASK) == 0) {
		    /* while not keypressed & ROM OK */
			sleep_mode();
			ow_reset();
			ow_byte_wr(OW_READ_ROM);
/* debug  */
/*			    lcd_gotoxy(0,1); */
/* /debug */
			for (i = 0; i < 8; i++) {
			    scratchpad_table[z][i] = ow_byte_rd();
/* debug  */
/*			    if (((scratchpad_table[z][i] & 0xF0) >> 4) < 10) {
			    lcd_putc(((scratchpad_table[z][i] & 0xF0) >> 4) + '0'); }
			    else { lcd_putc(((scratchpad_table[z][i] & 0xF0) >> 4) + 'A' - 10); }
			    if (((scratchpad_table[z][i] & 0x0F)) < 10) {
			    lcd_putc(((scratchpad_table[z][i] & 0x0F)) + '0'); }
			    else { lcd_putc(((scratchpad_table[z][i] & 0x0F)) + 'A' - 10); } */
/* /debug */
						}
			j = crc8(&scratchpad_table[z][0],8);
			lcd_gotoxy(9,0);
			if ( j == 0 ) { lcd_puts("OK"); }
			else	  { lcd_puts("--"); }
			lcd_gotoxy(0,1);
/* debug */
			lcd_puts("Numer  czujn.:");
			lcd_putc(z + '1');
						    } /* while */
		    /* store ROM address in EEPROM */
		    for (i = 0; i < 8; i++) {
			eeprom_write_byte(&sensor_addreses_eeprom[z][i], scratchpad_table[z][i]);
					    }
		    /* wait key released */
		    while ((soft_keys & SOFT_KEY_1_MASK) != 0) { 
			sleep_mode();
			if ((flagi & MS100_FLAG) != 0) { deciseconds++; 
							 flagi &= ~ MS100_FLAG;
							 if (deciseconds > 60) {
							    for (i = 0; i < 4; i++) {
								for (j = 0; j < 8; j++) {
								    eeprom_write_byte(&sensor_addreses_eeprom[i][j], 0xFF);
										        }
										    }
							    eeprom_write_byte(&num_of_sensors_eeprom, 0);
							    eeprom_write_byte(&lcd_flags_in_eeprom, LCD_DEFAULT_FLAGS);
							    flagi |= CONFIG_CLEARED_FLAG;
							    break;
										}
							}
								} /* while */
		/* ! IN_CONFIG but z =0-3 */
		    if ((flagi & CONFIG_CLEARED_FLAG) == 0) { eeprom_write_byte(&num_of_sensors_eeprom, (z + 1)); }
		    else { break; }
			 } /* FOR IN_CONFIG */
		lcd_clrscr();
		lcd_gotoxy(0,0);
		lcd_puts("Konfiguracja");
		lcd_gotoxy(0,1);
		if (eeprom_read_byte(&num_of_sensors_eeprom) != 0) { lcd_puts("Zapisana"); }
		    else					   { lcd_puts("Skasowana"); }
		wdt_enable(WDTO_2S);  /* reset after 2s */
		while (1 == 1) { } /* dummy loop */
	     } /* IN CONFIG */
    return 0;
}

/* END PROGRAM */
