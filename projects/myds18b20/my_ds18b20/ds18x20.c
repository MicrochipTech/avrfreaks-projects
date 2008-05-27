/*********************************************************************************
Title:    DS18X20-Functions via One-Wire-Bus
Author:   Martin Thomas <eversmith@heizung-thomas.de>   
          http://www.siwawi.arubi.uni-kl.de/avr-projects
Software: avr-gcc 3.4.1 / avr-libc 1.0.4 
Hardware: any AVR - tested with ATmega16/ATmega32 and 3 DS18B20

Partly based on code from Peter Dannegger and others

changelog:
20041124 - Extended measurements for DS18(S)20 contributed by Carsten Foss (CFO)
200502xx - function DS18X20_read_meas_single
20050310 - DS18x20 EEPROM functions (can be disabled to save flash-memory)
           (DS18X20_EEPROMSUPPORT in ds18x20.h)

**********************************************************************************/

#include <avr/io.h>

#include "ds18x20.h"
#include "onewire.h"
#include "crc8.h"

#ifdef DS18X20_EEPROMSUPPORT
// for 10ms delay in copy scratchpad
#include "delay.h"
#endif

/* 
   convert raw value from DS18x20 to Celsius
   input is: 
   - familycode fc (0x10/0x28 see header)
   - scratchpad-buffer
   output is:
   - cel full celsius
   - fractions of celsius in millicelsius*(10^-1)/625 (the 4 LS-Bits)
   - subzero =0 positiv / 1 negativ
   always returns  DS18X20_OK
   TODO invalid-values detection (but should be covered by CRC)
*/
uint8_t DS18X20_meas_to_cel( uint8_t fc, uint8_t *sp, 
	uint8_t* subzero, uint8_t* cel, uint8_t* cel_frac_bits)
{
	uint16_t meas;
	uint8_t  i;
	
	meas = sp[0];  // LSB
	meas |= ((uint16_t)sp[1])<<8; // MSB
	//meas = 0xff5e; meas = 0xfe6f;
	
	//  only work on 12bit-base
	if( fc == DS18S20_ID ) { // 9 -> 12 bit if 18S20
		/* Extended measurements for DS18S20 contributed by Carsten Foss */
		meas &= (uint16_t) 0xfffe;	// Discard LSB , needed for later extended precicion calc
		meas <<= 3;					// Convert to 12-bit , now degrees are in 1/16 degrees units
		meas += (16 - sp[6]) - 4;	// Add the compensation , and remember to subtract 0.25 degree (4/16)
	}
	
	// check for negative 
	if ( meas & 0x8000 )  {
		*subzero=1;      // mark negative
		meas ^= 0xffff;  // convert to positive => (twos complement)++
		meas++;
	}
	else *subzero=0;
	
	// clear undefined bits for B != 12bit
	if ( fc == DS18B20_ID ) { // check resolution 18B20
		i = sp[DS18B20_CONF_REG];
		if ( (i & DS18B20_12_BIT) == DS18B20_12_BIT ) ;
		else if ( (i & DS18B20_11_BIT) == DS18B20_11_BIT ) 
			meas &= ~(DS18B20_11_BIT_UNDF);
		else if ( (i & DS18B20_10_BIT) == DS18B20_10_BIT ) 
			meas &= ~(DS18B20_10_BIT_UNDF);
		else { // if ( (i & DS18B20_9_BIT) == DS18B20_9_BIT ) { 
			meas &= ~(DS18B20_9_BIT_UNDF);
		}
	}			
	
	*cel  = (uint8_t)(meas >> 4); 
	*cel_frac_bits = (uint8_t)(meas & 0x000F);
	
	return DS18X20_OK;
}

/* converts to decicelsius
   input is ouput from meas_to_cel
   returns absolute value of temperatur in decicelsius
	i.e.: sz=0, c=28, frac=15 returns 289 (=28.9°C)
0	0	0	
1	625	625	1
2	1250	250	
3	1875	875	3
4	2500	500	4
5	3125	125	
6	3750	750	6
7	4375	375	
8	5000	0	
9	5625	625	9
10	6250	250	
11	6875	875	11
12	7500	500	12
13	8125	125	
14	8750	750	14
15	9375	375	*/
uint16_t DS18X20_temp_to_decicel(uint8_t subzero, uint8_t cel,uint8_t cel_frac_bits)
{
	uint16_t h;
	uint8_t  i;
	uint8_t need_rounding[] = { 1, 3, 4, 6, 9, 11, 12, 14 };
	
	h = cel_frac_bits*DS18X20_FRACCONV/1000;
	h += cel*10;
	if (!subzero) {
		for (i=0; i<sizeof(need_rounding); i++) {
			if ( cel_frac_bits == need_rounding[i] ) {
				h++;
				break;
			}
		}
	}
	return h;
}

/* compare temperature values (full celsius only)
   returns -1 if param-pair1 < param-pair2 
            0 if == 
			1 if >    */
int8_t DS18X20_temp_cmp(uint8_t subzero1, uint16_t cel1, 
	uint8_t subzero2, uint16_t cel2)
{
	int16_t t1 = (subzero1) ? (cel1*(-1)) : (cel1);
	int16_t t2 = (subzero2) ? (cel2*(-1)) : (cel2);
	
	if (t1<t2) return -1;
	if (t1>t2) return 1;
	return 0;
}

/* find DS18X20 Sensors on 1-Wire-Bus
   input/ouput: diff is the result of the last rom-search
   output: id is the rom-code of the sensor found */
void DS18X20_find_sensor(uint8_t *diff, uint8_t id[])
{
	for (;;) {
		*diff = ow_rom_search( *diff, &id[0] );
		if ( *diff==OW_PRESENCE_ERR || *diff==OW_DATA_ERR ||
		  *diff == OW_LAST_DEVICE ) return;
		if ( id[0] == DS18B20_ID || id[0] == DS18S20_ID ) return;
	}
}

/* get power status of DS18x20 
   input  : id = rom_code 
   returns: DS18X20_POWER_EXTERN or DS18X20_POWER_PARASITE */
uint8_t	DS18X20_get_power_status(uint8_t id[])
{
	uint8_t pstat;
    ow_reset();
    ow_command(DS18X20_READ_POWER_SUPPLY, id);
    pstat=ow_bit_io(1); // pstat 0=is parasite/ !=0 ext. powered
    ow_reset();
	return (pstat) ? DS18X20_POWER_EXTERN:DS18X20_POWER_PARASITE;
}

/* start measurement (CONVERT_T) for all sensors if input id==NULL 
   or for single sensor. then id is the rom-code */
uint8_t DS18X20_start_meas( uint8_t with_power_extern, uint8_t id[])
{
	ow_reset(); //**
	if( ow_input_pin_state() ) { // only send if bus is "idle" = high
		ow_command( DS18X20_CONVERT_T, id );
		if (with_power_extern != DS18X20_POWER_EXTERN)
			ow_parasite_enable();
		return DS18X20_OK;
	} 
	else { 
		#ifdef DS18X20_VERBOSE
		#endif
		return DS18X20_START_FAIL;
	}
}

/* reads temperature (scratchpad) of sensor with rom-code id
   output: subzero==1 if temp.<0, cel: full celsius, mcel: frac 
   in millicelsius*0.1
   i.e.: subzero=1, cel=18, millicel=5000 = -18,5000°C */
uint8_t DS18X20_read_meas(uint8_t id[], uint8_t *subzero, 
	uint8_t *cel, uint8_t *cel_frac_bits)
{
	uint8_t i;
	uint8_t sp[DS18X20_SP_SIZE];
	
	ow_reset(); //**
	ow_command(DS18X20_READ, id);
	for ( i=0 ; i< DS18X20_SP_SIZE; i++ ) sp[i]=ow_byte_rd();
	if ( crc8( &sp[0], DS18X20_SP_SIZE ) ) 
		return DS18X20_ERROR_CRC;
	DS18X20_meas_to_cel(id[0], sp, subzero, cel, cel_frac_bits);
	return DS18X20_OK;
}

/* reads temperature (scratchpad) of a single sensor (uses skip-rom)
   output: subzero==1 if temp.<0, cel: full celsius, mcel: frac 
   in millicelsius*0.1
   i.e.: subzero=1, cel=18, millicel=5000 = -18,5000°C */
uint8_t DS18X20_read_meas_single(uint8_t familycode, uint8_t *subzero, 
	uint8_t *cel, uint8_t *cel_frac_bits)
{
	uint8_t i;
	uint8_t sp[DS18X20_SP_SIZE];
	
	ow_command(DS18X20_READ, NULL);
	for ( i=0 ; i< DS18X20_SP_SIZE; i++ ) sp[i]=ow_byte_rd();
	if ( crc8( &sp[0], DS18X20_SP_SIZE ) ) 
		return DS18X20_ERROR_CRC;
	DS18X20_meas_to_cel(familycode, sp, subzero, cel, cel_frac_bits);
	return DS18X20_OK;
}

void DS18X20_show_id( uint8_t *id, size_t n )
{
	size_t i;
	for( i = 0; i < n; i++ ) {
	    if ( i == 0 ) lcd_puts( "FC:" );
		else if ( i == n-1 ) lcd_puts( "CRC:" );
		if ( i == 1 ) lcd_puts( "SN: " );
		lcd_puts("%0X",id[i]);
		//lcd_putc(' ');
		//if ( i == 0 ) 
		//{
		//	if ( id[0] == DS18S20_ID ) lcd_puts ("(18S)");
		//	else if ( id[0] == DS18B20_ID ) lcd_puts("(18B)");
		//	else lcd_puts("( ? )");
		//}
		lcd_puts("\n");
	}
	//if ( crc8( id, OW_ROMCODE_SIZE) )
	//	lcd_puts( " CRC FAIL " );
	//else 
	//	lcd_puts( " CRC O.K. " );
}



#ifdef DS18X20_EEPROMSUPPORT

uint8_t DS18X20_write_scratchpad( uint8_t id[], 
	uint8_t th, uint8_t tl, uint8_t conf)
{
	ow_reset(); //**
	if( ow_input_pin_state() ) { // only send if bus is "idle" = high
		ow_command( DS18X20_WRITE_SCRATCHPAD, id );
		ow_byte_wr(th);
		ow_byte_wr(tl);
		if (id[0] == DS18B20_ID) ow_byte_wr(conf); // config avail. on B20 only
		return DS18X20_OK;
	} 
	else { 
		#ifdef DS18X20_VERBOSE
		#endif
		return DS18X20_ERROR;
	}
}

uint8_t DS18X20_read_scratchpad( uint8_t id[], uint8_t sp[] )
{
	uint8_t i;
	
	ow_reset(); //**
	if( ow_input_pin_state() ) { // only send if bus is "idle" = high
		ow_command( DS18X20_READ, id );
		for ( i=0 ; i< DS18X20_SP_SIZE; i++ )	sp[i]=ow_byte_rd();
		return DS18X20_OK;
	} 
	else { 
		#ifdef DS18X20_VERBOSE
		#endif
		return DS18X20_ERROR;
	}
}

uint8_t DS18X20_copy_scratchpad( uint8_t with_power_extern, 
	uint8_t id[] )
{
	ow_reset(); //**
	if( ow_input_pin_state() ) { // only send if bus is "idle" = high
		ow_command( DS18X20_COPY_SCRATCHPAD, id );
		if (with_power_extern != DS18X20_POWER_EXTERN)
			ow_parasite_enable();
		delay_ms(DS18X20_COPYSP_DELAY); // wait for 10 ms 
		if (with_power_extern != DS18X20_POWER_EXTERN)
			ow_parasite_disable();
		return DS18X20_OK;
	} 
	else { 
		#ifdef DS18X20_VERBOSE
		#endif
		return DS18X20_START_FAIL;
	}
}

uint8_t DS18X20_recall_E2( uint8_t id[] )
{
	ow_reset(); //**
	if( ow_input_pin_state() ) { // only send if bus is "idle" = high
		ow_command( DS18X20_RECALL_E2, id );
		// TODO: wait until status is "1" (then eeprom values
		// have been copied). here simple delay to avoid timeout 
		// handling
		delay_ms(DS18X20_COPYSP_DELAY);
		return DS18X20_OK;
	} 
	else { 
		#ifdef DS18X20_VERBOSE
		#endif
		return DS18X20_ERROR;
	}
}
#endif
